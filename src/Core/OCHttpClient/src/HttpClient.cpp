#include "HttpClient.h"
#include <curl/curl.h>
#include "OCMain/osg.h"

namespace OC
{
#define LC "[HTTPClient] "
    static int s_simResponseCode = -1;

    struct StreamObject
    {
        StreamObject(std::ostream* stream) : _stream(stream) { }

        void write(const char* ptr, size_t realsize)
        {
            if (_stream) _stream->write(ptr, realsize);
        }

        void writeHeader(const char* ptr, size_t realsize)
        {
            std::string header(ptr);
            osgEarth::StringTokenizer tok(":");
            StringVector tized;
            tok.tokenize(header, tized);
            if (tized.size() >= 2)
                _headers[tized[0]] = tized[1];
        }

        std::ostream* _stream;
        osgEarth::Headers _headers;
        std::string     _resultMimeType;
    };

    static size_t
        StreamObjectReadCallback(void* ptr, size_t size, size_t nmemb, void* data)
    {
        size_t realsize = size * nmemb;
        StreamObject* sp = (StreamObject*)data;
        sp->write((const char*)ptr, realsize);
        return realsize;
    }

    static size_t
        StreamObjectHeaderCallback(void* ptr, size_t size, size_t nmemb, void* data)
    {
        size_t realsize = size * nmemb;
        StreamObject* sp = (StreamObject*)data;
        sp->writeHeader((const char*)ptr, realsize);
        return realsize;
    }

    osgEarth::TimeStamp
        getCurlFileTime(void* curl)
    {
        long filetime;
        if (CURLE_OK != curl_easy_getinfo(curl, CURLINFO_FILETIME, &filetime))
            return osgEarth::TimeStamp(0);
        else if (filetime < 0)
            return osgEarth::TimeStamp(0);
        else
            return osgEarth::TimeStamp(filetime);
    }

    static int CurlProgressCallback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        osgEarth::ProgressCallback* callback = (osgEarth::ProgressCallback*)clientp;
        bool cancelled = false;
        if (callback)
        {
            cancelled = callback->isCanceled() || callback->reportProgress(dlnow, dltotal);
            if (cancelled)
                OE_DEBUG << "An HTTP request was canceled mid-stream" << std::endl;
        }
        return cancelled;
    }

    bool decodeMultipartStream(const std::string& boundary,
        HTTPResponse::Part* input,
        HTTPResponse::Parts& output)
    {
        std::string bstr = std::string("--") + boundary;
        std::string line;
        char tempbuf[256];

        // first thing in the stream should be the boundary.
        input->_stream.read(tempbuf, bstr.length());
        tempbuf[bstr.length()] = 0;
        line = tempbuf;
        if (line != bstr)
        {
            OE_INFO << LC
                << "decodeMultipartStream: protocol violation; "
                << "expecting boundary; instead got: \""
                << line
                << "\"" << std::endl;
            return false;
        }

        for (bool done = false; !done; )
        {
            osg::ref_ptr<HTTPResponse::Part> next_part = new HTTPResponse::Part();

            // first finish off the boundary.
            std::getline(input->_stream, line);
            if (line == "--")
            {
                done = true;
            }
            else
            {
                // read all headers. this ends with a blank line.
                line = " ";
                while (line.length() > 0 && !done)
                {
                    std::getline(input->_stream, line);

                    // check for EOS:
                    if (line == "--")
                    {
                        done = true;
                    }
                    else
                    {
                        osgEarth::StringTokenizer tok(":");
                        StringVector tized;
                        tok.tokenize(line, tized);
                        if (tized.size() >= 2)
                            next_part->_headers[tized[0]] = tized[1];
                    }
                }
            }

            if (!done)
            {
                // read data until we reach the boundary
                unsigned int bstr_ptr = 0;
                std::string temp;
                //unsigned int c = 0;
                while (bstr_ptr < bstr.length())
                {
                    char b;
                    input->_stream.read(&b, 1);
                    if (b == bstr[bstr_ptr])
                    {
                        bstr_ptr++;
                    }
                    else
                    {
                        for (unsigned int i = 0; i < bstr_ptr; i++)
                        {
                            next_part->_stream << bstr[i];
                        }
                        next_part->_stream << b;
                        next_part->_size += bstr_ptr + 1;
                        bstr_ptr = 0;
                    }
                }
                output.push_back(next_part.get());
            }
        }

        return true;
    }

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define USER_AGENT "osgearth" QUOTE(OSGEARTH_MAJOR_VERSION) "." QUOTE(OSGEARTH_MINOR_VERSION)


    namespace
    {
        // TODO: consider moving this stuff into the osgEarth::Registry;
        // don't like it here in the global scope
        // per-thread client map (must be global scope)
        static osgEarth::PerThread<HTTPClient>       s_clientPerThread("HTTPClient(OE)");

        static osgEarth::optional<osgEarth::ProxySettings>     s_proxySettings;

        static std::string                 s_userAgent = USER_AGENT;

        static std::string                 s_encoding;
        static std::string                 s_referer;
        static long                        s_timeout = 0;
        static long                        s_connectTimeout = 0;
        static float                       s_retryDelay_s = 0.5f;

        // HTTP debugging.
        static bool                        s_HTTP_DEBUG = false;
        static osgEarth::Threading::Mutex            s_HTTP_DEBUG_mutex;
        static int                         s_HTTP_DEBUG_request_count;
        static double                      s_HTTP_DEBUG_total_duration;

        static osg::ref_ptr< URLRewriter > s_rewriter;

        static osg::ref_ptr< ConfigHandler > s_curlConfigHandler;
    }

    //.........................................................................

    namespace
    {
        class CURLImplementation : public HTTPClient::Implementation
        {
        public:
            CURLImplementation() : _curl_handle(0), _previousHttpAuthentication(0) { }

            void initialize()
            {
                _previousHttpAuthentication = 0L;

                _curl_handle = curl_easy_init();

                curl_easy_setopt(_curl_handle, CURLOPT_WRITEFUNCTION, StreamObjectReadCallback);
                curl_easy_setopt(_curl_handle, CURLOPT_HEADERFUNCTION, StreamObjectHeaderCallback);
                curl_easy_setopt(_curl_handle, CURLOPT_FOLLOWLOCATION, (void*)1);
                curl_easy_setopt(_curl_handle, CURLOPT_MAXREDIRS, (void*)5);
                curl_easy_setopt(_curl_handle, CURLOPT_PROGRESSFUNCTION, &CurlProgressCallback);
                curl_easy_setopt(_curl_handle, CURLOPT_NOPROGRESS, (void*)0); //0=enable.
                curl_easy_setopt(_curl_handle, CURLOPT_FILETIME, true);

                // Enable automatic CURL decompression of known types. An empty string will automatically add all supported encoding types that are built into curl.
                // Note that you must have curl built against zlib to support gzip or deflate encoding.
                curl_easy_setopt(_curl_handle, CURLOPT_ENCODING, "");

                osg::ref_ptr< ConfigHandler > curlConfigHandler = HTTPClient::getConfigHandler();
                if (curlConfigHandler.valid()) {
                    curlConfigHandler->onInitialize(_curl_handle);
                }
            }

            ~CURLImplementation()
            {
                if (_curl_handle)
                    curl_easy_cleanup(_curl_handle);
                _curl_handle = 0;
            }

            HTTPResponse doGet(
                const HTTPRequest& request,
                const osgDB::Options* options,
                osgEarth::ProgressCallback* progress) const
            {
                OE_START_TIMER(http_get);

                std::string url = request.getURL();

                const osgDB::AuthenticationMap* authenticationMap = (options && options->getAuthenticationMap()) ?
                    options->getAuthenticationMap() :
                    osgDB::Registry::instance()->getAuthenticationMap();

                std::string proxy_host;
                std::string proxy_port = "8080";
                std::string proxy_auth;

                //TODO: don't do all this proxy setup on every GET. Just do it once per client, or only when
                // the proxy information changes.

                //Try to get the proxy settings from the global settings
                if (s_proxySettings.isSet())
                {
                    proxy_host = s_proxySettings.get().hostName();
                    std::stringstream buf;
                    buf << s_proxySettings.get().port();
                    proxy_port = buf.str();

                    std::string proxy_username = s_proxySettings.get().userName();
                    std::string proxy_password = s_proxySettings.get().password();
                    if (!proxy_username.empty() && !proxy_password.empty())
                    {
                        proxy_auth = proxy_username + std::string(":") + proxy_password;
                    }
                }

                //Try to get the proxy settings from the local options that are passed in.
                readOptions(options, proxy_host, proxy_port);

                osgEarth::optional< osgEarth::ProxySettings > proxySettings;
                osgEarth::ProxySettings::fromOptions(options, proxySettings);
                if (proxySettings.isSet())
                {
                    proxy_host = proxySettings.get().hostName();
                    proxy_port = osgEarth::toString<int>(proxySettings.get().port());
                    OE_DEBUG << LC << "Read proxy settings from options " << proxy_host << " " << proxy_port << std::endl;
                }

                //Try to get the proxy settings from the environment variable
                const char* proxyEnvAddress = getenv("OSG_CURL_PROXY");
                if (proxyEnvAddress) //Env Proxy Settings
                {
                    proxy_host = std::string(proxyEnvAddress);

                    const char* proxyEnvPort = getenv("OSG_CURL_PROXYPORT"); //Searching Proxy Port on Env
                    if (proxyEnvPort)
                    {
                        proxy_port = std::string(proxyEnvPort);
                    }
                }

                const char* proxyEnvAuth = getenv("OSGEARTH_CURL_PROXYAUTH");
                if (proxyEnvAuth)
                {
                    proxy_auth = std::string(proxyEnvAuth);
                }

                // Set up proxy server:
                std::string proxy_addr;
                if (!proxy_host.empty())
                {
                    std::stringstream buf;
                    buf << proxy_host << ":" << proxy_port;
                    std::string bufStr;
                    bufStr = buf.str();
                    proxy_addr = bufStr;

                    if (s_HTTP_DEBUG)
                    {
                        OE_NOTICE << LC << "Using proxy: " << proxy_addr << std::endl;
                    }

                    //curl_easy_setopt( _curl_handle, CURLOPT_HTTPPROXYTUNNEL, 1 );
                    curl_easy_setopt(_curl_handle, CURLOPT_PROXY, proxy_addr.c_str());

                    //Setup the proxy authentication if setup
                    if (!proxy_auth.empty())
                    {
                        if (s_HTTP_DEBUG)
                        {
                            OE_NOTICE << LC << "Using proxy authentication " << proxy_auth << std::endl;
                        }

                        curl_easy_setopt(_curl_handle, CURLOPT_PROXYUSERPWD, proxy_auth.c_str());
                    }
                }
                else
                {
                    OE_DEBUG << LC << "Removing proxy settings" << std::endl;
                    curl_easy_setopt(_curl_handle, CURLOPT_PROXY, 0);
                }

                // Rewrite the url if the url rewriter is available
                osg::ref_ptr< URLRewriter > rewriter = HTTPClient::getURLRewriter();
                if (rewriter.valid())
                {
                    std::string oldURL = url;
                    url = rewriter->rewrite(oldURL);
                    OE_DEBUG << LC << "Rewrote URL " << oldURL << " to " << url << std::endl;
                }

                const osgDB::AuthenticationDetails* details = authenticationMap ?
                    authenticationMap->getAuthenticationDetails(url) :
                    0;

                if (details)
                {
                    const std::string colon(":");
                    std::string password(details->username + colon + details->password);
                    curl_easy_setopt(_curl_handle, CURLOPT_USERPWD, password.c_str());
                    _previousPassword = password;

                    // use for https.
                    // curl_easy_setopt(_curl, CURLOPT_KEYPASSWD, password.c_str());

#if LIBCURL_VERSION_NUM >= 0x070a07
                    if (details->httpAuthentication != _previousHttpAuthentication)
                    {
                        curl_easy_setopt(_curl_handle, CURLOPT_HTTPAUTH, details->httpAuthentication);
                        _previousHttpAuthentication = details->httpAuthentication;
                    }
#endif
                }
                else
                {
                    if (!_previousPassword.empty())
                    {
                        curl_easy_setopt(_curl_handle, CURLOPT_USERPWD, 0);
                        _previousPassword.clear();
                    }

#if LIBCURL_VERSION_NUM >= 0x070a07
                    // need to reset if previously set.
                    if (_previousHttpAuthentication != 0)
                    {
                        curl_easy_setopt(_curl_handle, CURLOPT_HTTPAUTH, 0);
                        _previousHttpAuthentication = 0;
                    }
#endif
                }


                // Set any headers
                struct curl_slist* headers = NULL;
                if (!request.getHeaders().empty())
                {
                    for (HTTPRequest::Parameters::const_iterator itr = request.getHeaders().begin(); itr != request.getHeaders().end(); ++itr)
                    {
                        std::stringstream buf;
                        buf << osgEarth::toLower(itr->first) << ": " << itr->second;
                        headers = curl_slist_append(headers, buf.str().c_str());
                    }
                }

                // Disable the default Pragma: no-cache that curl adds by default.
                headers = curl_slist_append(headers, "pragma: ");
                curl_easy_setopt(_curl_handle, CURLOPT_HTTPHEADER, headers);

                osg::ref_ptr<HTTPResponse::Part> part = new HTTPResponse::Part();
                StreamObject sp(&part->_stream);

                //Take a temporary ref to the callback (why? dangerous.)
                //osg::ref_ptr<ProgressCallback> progressCallback = callback;
                curl_easy_setopt(_curl_handle, CURLOPT_URL, url.c_str());
                if (progress)
                {
                    curl_easy_setopt(_curl_handle, CURLOPT_PROGRESSDATA, progress);
                }

                CURLcode res;
                long response_code = 0L;

                OE_START_TIMER(get_duration);

                //if ( _simResponseCode < 0 )
                //{
                char errorBuf[CURL_ERROR_SIZE];
                errorBuf[0] = 0;
                curl_easy_setopt(_curl_handle, CURLOPT_ERRORBUFFER, (void*)errorBuf);
                curl_easy_setopt(_curl_handle, CURLOPT_WRITEDATA, (void*)&sp);
                curl_easy_setopt(_curl_handle, CURLOPT_HEADERDATA, (void*)&sp);

                //Disable peer certificate verification to allow us to access in https servers where the peer certificate cannot be verified.
                curl_easy_setopt(_curl_handle, CURLOPT_SSL_VERIFYPEER, (void*)0);

                osg::ref_ptr< ConfigHandler > configHandler = HTTPClient::getConfigHandler();
                if (configHandler.valid()) {
                    configHandler->onGet(_curl_handle);
                }

                res = curl_easy_perform(_curl_handle);

                curl_easy_setopt(_curl_handle, CURLOPT_WRITEDATA, (void*)0);
                curl_easy_setopt(_curl_handle, CURLOPT_PROGRESSDATA, (void*)0);

                if (!proxy_addr.empty())
                {
                    long connect_code = 0L;
                    CURLcode r = curl_easy_getinfo(_curl_handle, CURLINFO_HTTP_CONNECTCODE, &connect_code);
                    if (r != CURLE_OK)
                    {
                        OE_WARN << LC << "Proxy connect error: " << curl_easy_strerror(r) << std::endl;

                        // Free the headers
                        if (headers)
                        {
                            curl_slist_free_all(headers);
                        }

                        return HTTPResponse(0);
                    }
                }

                curl_easy_getinfo(_curl_handle, CURLINFO_RESPONSE_CODE, &response_code);

                if (s_simResponseCode > 0)
                {
                    unsigned hash = std::hash<double>()(osg::Timer::instance()->tick()) % 10;
                    if (hash == 0)
                        response_code = s_simResponseCode;
                }

                HTTPResponse response(response_code);



                // read the response content type:
                char* content_type_cp;

                curl_easy_getinfo(_curl_handle, CURLINFO_CONTENT_TYPE, &content_type_cp);

                if (content_type_cp != NULL)
                {
                    response.setMimeType(content_type_cp);
                }

                // read the file time:
                response.setLastModified(getCurlFileTime(_curl_handle));

                if (res == CURLE_OK)
                {
                    // check for multipart content
                    if (response.getMimeType().length() > 9 &&
                        ::strstr(response.getMimeType().c_str(), "multipart") == response.getMimeType().c_str())
                    {
                        OE_DEBUG << LC << "detected multipart data; decoding..." << std::endl;

                        //TODO: parse out the "wcs" -- this is WCS-specific
                        if (!decodeMultipartStream("wcs", part.get(), response.getParts()))
                        {
                            // error decoding an invalid multipart stream.
                            // should we do anything, or just leave the response empty?
                        }
                    }
                    else
                    {
                        for (osgEarth::Headers::iterator itr = sp._headers.begin(); itr != sp._headers.end(); ++itr)
                        {
                            part->_headers[itr->first] = itr->second;
                        }

                        // Write the headers to the metadata
                        response.getParts().push_back(part.get());
                    }
                }

                else if (res == CURLE_ABORTED_BY_CALLBACK || res == CURLE_OPERATION_TIMEDOUT)
                {
                    //If we were aborted by a callback, then it was cancelled by a user
                    response.setCanceled(true);
                }

                else
                {
                    response.setMessage(curl_easy_strerror(res));

                    if (res == CURLE_GOT_NOTHING)
                    {
                        OE_DEBUG << LC << "CURLE_GOT_NOTHING for " << url << std::endl;
                    }
                }

                response.setDuration(OE_STOP_TIMER(get_duration));

                if (s_HTTP_DEBUG)
                {
                    osgEarth::TimeStamp filetime = getCurlFileTime(_curl_handle);

                    OE_NOTICE << LC
                        << "GET(" << response_code << ") " << response.getMimeType() << ": \""
                        << url << "\" (" << osgEarth::DateTime(filetime).asRFC1123() << ") t="
                        << std::setprecision(4) << response.getDuration() << "s" << std::endl;

                    for (HTTPRequest::Parameters::const_iterator itr = request.getHeaders().begin();
                        itr != request.getHeaders().end();
                        ++itr)
                    {
                        OE_NOTICE << LC << "    Header: " << itr->first << " = " << itr->second << std::endl;
                    }

                    {
                        osgEarth::Threading::ScopedMutexLock lock(s_HTTP_DEBUG_mutex);
                        s_HTTP_DEBUG_request_count++;
                        s_HTTP_DEBUG_total_duration += response.getDuration();

                        if (s_HTTP_DEBUG_request_count % 60 == 0)
                        {
                            OE_NOTICE << LC << "Average duration = " << s_HTTP_DEBUG_total_duration / (double)s_HTTP_DEBUG_request_count
                                << std::endl;
                        }
                    }

#if 0
                    // time details - almost 100% of the time is spent in
                    // STARTTRANSFER, which is the time until the first byte is received.
                    double td[7];

                    curl_easy_getinfo(_curl_handle, CURLINFO_TOTAL_TIME, &td[0]);
                    curl_easy_getinfo(_curl_handle, CURLINFO_NAMELOOKUP_TIME, &td[1]);
                    curl_easy_getinfo(_curl_handle, CURLINFO_CONNECT_TIME, &td[2]);
                    curl_easy_getinfo(_curl_handle, CURLINFO_APPCONNECT_TIME, &td[3]);
                    curl_easy_getinfo(_curl_handle, CURLINFO_PRETRANSFER_TIME, &td[4]);
                    curl_easy_getinfo(_curl_handle, CURLINFO_STARTTRANSFER_TIME, &td[5]);
                    curl_easy_getinfo(_curl_handle, CURLINFO_REDIRECT_TIME, &td[6]);

                    for (int i = 0; i < 7; ++i)
                    {
                        OE_NOTICE << LC
                            << std::setprecision(4)
                            << "TIMES: total=" << td[0]
                            << ", lookup=" << td[1] << " (" << (int)((td[1] / td[0]) * 100) << "%)"
                            << ", connect=" << td[2] << " (" << (int)((td[2] / td[0]) * 100) << "%)"
                            << ", appconn=" << td[3] << " (" << (int)((td[3] / td[0]) * 100) << "%)"
                            << ", prexfer=" << td[4] << " (" << (int)((td[4] / td[0]) * 100) << "%)"
                            << ", startxfer=" << td[5] << " (" << (int)((td[5] / td[0]) * 100) << "%)"
                            << ", redir=" << td[6] << " (" << (int)((td[6] / td[0]) * 100) << "%)"
                            << std::endl;
                    }
#endif
                }

                // Free the headers
                if (headers)
                {
                    curl_slist_free_all(headers);
                }

                return response;
            }

            void* getHandle() const
            {
                return _curl_handle;
            }

            void setUserAgent(const std::string& value)
            {
                curl_easy_setopt(_curl_handle, CURLOPT_USERAGENT, value.c_str());
            }

            void setTimeout(long value)
            {
                curl_easy_setopt(_curl_handle, CURLOPT_TIMEOUT, value);
            }

            void setReferer(const std::string& value)
            {
                curl_easy_setopt(_curl_handle, CURLOPT_REFERER, value.c_str());
            }

            void setEncoding(const std::string& value)
            {
                curl_easy_setopt(_curl_handle, CURLOPT_ENCODING, value.c_str());
            }
            
            void setConnectTimeout(long value)
            {
                curl_easy_setopt(_curl_handle, CURLOPT_CONNECTTIMEOUT, value);
            }

            void readOptions(const osgDB::Options* options, std::string& proxy_host, std::string& proxy_port) const
            {
                // try to set proxy host/port by reading the CURL proxy options
                if (options)
                {
                    std::istringstream iss(options->getOptionString());
                    std::string opt;
                    while (iss >> opt)
                    {
                        int index = opt.find('=');
                        if (opt.substr(0, index) == "OSG_CURL_PROXY")
                        {
                            proxy_host = opt.substr(index + 1);
                        }
                        else if (opt.substr(0, index) == "OSG_CURL_PROXYPORT")
                        {
                            proxy_port = opt.substr(index + 1);
                        }
                    }
                }
            }

        private:
            void* _curl_handle;
            mutable std::string _previousPassword;
            mutable long _previousHttpAuthentication;
        };
    }

    HTTPClient::Implementation*
        CURLHTTPImplementationFactory::create() const
    {
        return new CURLImplementation();
    }

    HTTPClient::ImplementationFactory* HTTPClient::_implFactory = new CURLHTTPImplementationFactory();

    void
        HTTPClient::setImplementationFactory(ImplementationFactory* factory)
    {
        if (factory != NULL)
        {
            if (_implFactory != NULL)
                delete _implFactory;

            _implFactory = factory;
        }
    }

    HTTPClient&
        HTTPClient::getClient()
    {
        return s_clientPerThread.get();
    }

    HTTPClient::HTTPClient() :
        _initialized(false),
        _curl_handle(0L),
        _simResponseCode(-1L),
        _previousHttpAuthentication(0L),
        _impl(NULL)
    {
        //nop
        //do no CURL calls here.
        if (_implFactory)
        {
            _impl = _implFactory->create();
        }
    }

    void
        HTTPClient::initialize() const
    {
        if (!_initialized)
        {
            const_cast<HTTPClient*>(this)->initializeImpl();
        }
    }

    void
        HTTPClient::initializeImpl()
    {
        _previousHttpAuthentication = 0;

        //Get the user agent
        std::string userAgent = s_userAgent;
        long timeout = s_timeout;

        const char* timeoutEnv = getenv("OSGEARTH_HTTP_TIMEOUT");
        if (timeoutEnv)
        {
            timeout = osgEarth::as<long>(std::string(timeoutEnv), 0);
        }
        OE_DEBUG << LC << "Setting timeout to " << timeout << std::endl;

        long connectTimeout = s_connectTimeout;
        const char* connectTimeoutEnv = getenv("OSGEARTH_HTTP_CONNECTTIMEOUT");
        if (connectTimeoutEnv)
        {
            connectTimeout = osgEarth::as<long>(std::string(connectTimeoutEnv), 0);
        }
        OE_DEBUG << LC << "Setting connect timeout to " << connectTimeout << std::endl;

        const char* retryDelayEnv = getenv("OSGEARTH_HTTP_RETRY_DELAY");
        if (retryDelayEnv)
        {
            s_retryDelay_s = osgEarth::as<double>(std::string(retryDelayEnv), 0.0);
        }
        OE_DEBUG << LC << "Setting retry delay to " << s_retryDelay_s << std::endl;

        _impl->initialize();

        _impl->setUserAgent(userAgent.c_str());
        _impl->setReferer(s_referer);
        _impl->setEncoding(s_encoding);
        _impl->setTimeout(timeout);
        _impl->setConnectTimeout(connectTimeout);
        
        _initialized = true;
    }

    HTTPClient::~HTTPClient()
    {
    }

    void
        HTTPClient::setProxySettings(const osgEarth::optional<osgEarth::ProxySettings>& proxySettings)
    {
        s_proxySettings = proxySettings;
    }

    const osgEarth::optional<osgEarth::ProxySettings>&
        HTTPClient::getProxySettings()
    {
        return s_proxySettings;
    }

    const std::string& HTTPClient::getUserAgent()
    {
        return s_userAgent;
    }

    void  HTTPClient::setUserAgent(const std::string& userAgent)
    {
        s_userAgent = userAgent;
    }

    long HTTPClient::getTimeout()
    {
        return s_timeout;
    }

    void HTTPClient::setTimeout(long timeout)
    {
        s_timeout = timeout;
    }

    long HTTPClient::getConnectTimeout()
    {
        return s_connectTimeout;
    }

    void HTTPClient::setConnectTimeout(long timeout)
    {
        s_connectTimeout = timeout;
    }

    void HTTPClient::setRetryDelay(float value_s)
    {
        s_retryDelay_s = value_s;
    }

    float HTTPClient::getRetryDelay()
    {
        return s_retryDelay_s;
    }

    URLRewriter* HTTPClient::getURLRewriter()
    {
        return s_rewriter.get();
    }

    void HTTPClient::setURLRewriter(URLRewriter* rewriter)
    {
        s_rewriter = rewriter;
    }

    ConfigHandler* HTTPClient::getConfigHandler()
    {
        return s_curlConfigHandler.get();
    }

    void HTTPClient::setConfigHandler(ConfigHandler* handler)
    {
        s_curlConfigHandler = handler;
    }

    void HTTPClient::setReferer(const std::string& value)
    {
        s_referer = value;
        if (_initialized)
        {
            _impl->setReferer(value);
        }
    }

    void HTTPClient::setEncoding(const std::string& value)
    {
        s_encoding = value;
        if (_initialized)
        {
            _impl->setEncoding(value);
        }
    }

    void
        HTTPClient::globalInit()
    {
#ifndef OSGEARTH_USE_WININET_FOR_HTTP
        curl_global_init(CURL_GLOBAL_ALL);
#endif
    }

    void
        HTTPClient::readOptions(const osgDB::Options* options, std::string& proxy_host, std::string& proxy_port) const
    {
        // try to set proxy host/port by reading the CURL proxy options
        if (options)
        {
            std::istringstream iss(options->getOptionString());
            std::string opt;
            while (iss >> opt)
            {
                int index = opt.find('=');
                if (opt.substr(0, index) == "OSG_CURL_PROXY")
                {
                    proxy_host = opt.substr(index + 1);
                }
                else if (opt.substr(0, index) == "OSG_CURL_PROXYPORT")
                {
                    proxy_port = opt.substr(index + 1);
                }
            }
        }
    }

    HTTPResponse
        HTTPClient::get(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* progress)
    {
        return getClient().doGet(request, options, progress);
    }

    HTTPResponse
        HTTPClient::get(const std::string& url,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* progress)
    {
        return getClient().doGet(url, options, progress);
    }

    osgEarth::ReadResult
        HTTPClient::readImage(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* progress)
    {
        return getClient().doReadImage(request, options, progress);
    }

    osgEarth::ReadResult
        HTTPClient::readNode(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* progress)
    {
        return getClient().doReadNode(request, options, progress);
    }

    osgEarth::ReadResult
        HTTPClient::readObject(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* progress)
    {
        return getClient().doReadObject(request, options, progress);
    }

    osgEarth::ReadResult
        HTTPClient::readString(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* progress)
    {
        return getClient().doReadString(request, options, progress);
    }

    bool
        HTTPClient::download(const std::string& uri,
            const std::string& localPath)
    {
        return getClient().doDownload(uri, localPath);
    }

    HTTPResponse
        HTTPClient::doGet(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* progress) const
    {
        // OE_PROFILING_ZONE;
        // OE_PROFILING_ZONE_TEXT(osgEarth::Stringify() << "url " << request.getURL());

        initialize();

        HTTPResponse response = _impl->doGet(request, options, progress);

        // OE_PROFILING_ZONE_TEXT(osgEarth::Stringify() << "response_code " << response.getCode());
        if (response.isCanceled())
        {
            // OE_PROFILING_ZONE_TEXT("cancelled");
        }
        return response;
    }

    bool
        HTTPClient::doDownload(const std::string& url, const std::string& filename)
    {
        initialize();

        // download the data
        HTTPResponse response = this->doGet(HTTPRequest(url));

        if (response.isOK())
        {
            if (response.getNumParts() < 1)
                return false;

            unsigned int part_num = response.getNumParts() > 1 ? 1 : 0;
            std::istream& input_stream = response.getPartStream(part_num);

            std::ofstream fout;
            fout.open(filename.c_str(), std::ios::out | std::ios::binary);

            input_stream.seekg(0, std::ios::end);
            int length = input_stream.tellg();
            input_stream.seekg(0, std::ios::beg);

            char* buffer = new char[length];
            input_stream.read(buffer, length);
            fout.write(buffer, length);
            delete[] buffer;
            fout.close();
            return true;
        }
        else
        {
            OE_WARN << LC << "Error downloading file " << filename
                << " (" << response.getCode() << ")" << std::endl;
            return false;
        }
    }

    namespace
    {
        osgDB::ReaderWriter*
            getReader(const std::string& url, const HTTPResponse& response)
        {
            osgDB::ReaderWriter* reader = 0L;

            // try extension first:
            std::string ext = osgDB::getFileExtension(url);
            if (!ext.empty())
            {
                reader = osgDB::Registry::instance()->getReaderWriterForExtension(ext);
            }

            if (!reader)
            {
                // try to look up a reader by mime-type first:
                const std::string& mimeType = response.getMimeType();
                if (!mimeType.empty())
                {
                    reader = osgDB::Registry::instance()->getReaderWriterForMimeType(mimeType);
                }
            }

            if (!reader && s_HTTP_DEBUG)
            {
                OE_WARN << LC << "Cannot find an OSG plugin to read response data (ext="
                    << ext << "; mime-type=" << response.getMimeType()
                    << ")" << std::endl;

                if (osgEarth::endsWith(response.getMimeType(), "xml", false) && response.getNumParts() > 0)
                {
                    OE_WARN << LC << "Content:\n" << response.getPartAsString(0) << "\n";
                }
            }

            return reader;
        }
    }

    osgEarth::ReadResult
        HTTPClient::doReadImage(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* callback)
    {
        initialize();

        osgEarth::ReadResult result;

        HTTPResponse response = this->doGet(request, options, callback);

        if (response.isOK())
        {
            osgDB::ReaderWriter* reader = getReader(request.getURL(), response);
            if (!reader)
            {
                result = osgEarth::ReadResult(osgEarth::ReadResult::RESULT_NO_READER);
            }

            else
            {
                osgDB::ReaderWriter::ReadResult rr;

                if (response.getNumParts() > 0)
                    rr = reader->readImage(response.getPartStream(0), options);

                if (rr.validImage())
                {
                    result = osgEarth::ReadResult(rr.takeImage());
                }
                else
                {
                    if (s_HTTP_DEBUG)
                    {
                        OE_WARN << LC << reader->className()
                            << " failed to read image from " << request.getURL()
                            << "; message = " << rr.message()
                            << std::endl;
                    }
                    result = osgEarth::ReadResult(osgEarth::ReadResult::RESULT_READER_ERROR);
                    result.setErrorDetail(rr.message());
                }
            }

            // last-modified (file time)
            result.setLastModifiedTime(response._lastModified); //getCurlFileTime(_curl_handle) );

            // Time of query
            result.setDuration(response.getDuration());
        }
        else
        {
            result = osgEarth::ReadResult(
                response.isCanceled() ? osgEarth::ReadResult::RESULT_CANCELED :
                response.getCode() == HTTPResponse::NOT_FOUND ? osgEarth::ReadResult::RESULT_NOT_FOUND :
                response.getCode() == HTTPResponse::NOT_MODIFIED ? osgEarth::ReadResult::RESULT_NOT_MODIFIED :
                response.getCodeCategory() == HTTPResponse::CATEGORY_SERVER_ERROR ? osgEarth::ReadResult::RESULT_SERVER_ERROR :
                osgEarth::ReadResult::RESULT_UNKNOWN_ERROR);

            // for request errors, return an error result with the part data intact
            // so the user can parse it as needed. We only do this for readString.
            if (response.getNumParts() > 0u)
            {
                result.setErrorDetail(response.getPartAsString(0));

                if (s_HTTP_DEBUG)
                {
                    OE_WARN << LC << "SERVER REPORTS: " << result.errorDetail() << std::endl;
                }
            }

            //If we have an error but it's recoverable, like a server error or timeout then set the callback to retry.
            if (HTTPClient::isRecoverable(result.code()))
            {
                if (callback)
                {
                    callback->setRetryDelay(getRetryDelay());
                    callback->cancel();

                    if (response.getCode() == 503)
                    {
                        callback->message() = "Server deferral";
                    }

                    if (s_HTTP_DEBUG)
                    {
                        if (response.isCanceled())
                        {
                            OE_NOTICE << LC << "Request was cancelled" << std::endl;
                        }
                        else
                        {
                            OE_NOTICE << LC << "Recoverable error in HTTPClient for " << request.getURL() << std::endl;
                        }
                    }
                }
            }
        }

        // encode headers
        result.setMetadata(response.getHeadersAsConfig());

        // set the source name
        if (result.getImage())
            result.getImage()->setName(request.getURL());

        return result;
    }

    osgEarth::ReadResult
        HTTPClient::doReadNode(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* callback)
    {
        initialize();

        osgEarth::ReadResult result;

        HTTPResponse response = this->doGet(request, options, callback);

        if (response.isOK())
        {
            osgDB::ReaderWriter* reader = getReader(request.getURL(), response);
            if (!reader)
            {
                result = osgEarth::ReadResult(osgEarth::ReadResult::RESULT_NO_READER);
            }

            else
            {
                osgDB::ReaderWriter::ReadResult rr;

                if (response.getNumParts() > 0)
                    rr = reader->readNode(response.getPartStream(0), options);

                if (rr.validNode())
                {
                    result = osgEarth::ReadResult(rr.takeNode());
                }
                else
                {
                    if (s_HTTP_DEBUG)
                    {
                        OE_WARN << LC << reader->className()
                            << " failed to read node from " << request.getURL()
                            << "; message = " << rr.message()
                            << std::endl;
                    }
                    result = osgEarth::ReadResult(osgEarth::ReadResult::RESULT_READER_ERROR);
                    result.setErrorDetail(rr.message());
                }
            }

            // last-modified (file time)
            result.setLastModifiedTime(response._lastModified); //getCurlFileTime(_curl_handle) );
        }
        else
        {
            result = osgEarth::ReadResult(
                response.isCanceled() ? osgEarth::ReadResult::RESULT_CANCELED :
                response.getCode() == HTTPResponse::NOT_FOUND ? osgEarth::ReadResult::RESULT_NOT_FOUND :
                response.getCode() == HTTPResponse::NOT_MODIFIED ? osgEarth::ReadResult::RESULT_NOT_MODIFIED :
                response.getCodeCategory() == HTTPResponse::CATEGORY_SERVER_ERROR ? osgEarth::ReadResult::RESULT_SERVER_ERROR :
                osgEarth::ReadResult::RESULT_UNKNOWN_ERROR);

            // for request errors, return an error result with the part data intact
            // so the user can parse it as needed. We only do this for readString.
            if (response.getNumParts() > 0u)
            {
                result.setErrorDetail(response.getPartAsString(0));

                if (s_HTTP_DEBUG)
                {
                    OE_WARN << LC << "SERVER REPORTS: " << result.errorDetail() << std::endl;
                }
            }

            //If we have an error but it's recoverable, like a server error or timeout then set the callback to retry.
            if (HTTPClient::isRecoverable(result.code()))
            {
                if (callback)
                {
                    callback->setRetryDelay(getRetryDelay());
                    callback->cancel();

                    if (s_HTTP_DEBUG)
                    {
                        if (response.isCanceled())
                        {
                            OE_NOTICE << LC << "Request was cancelled" << std::endl;
                        }
                        else
                        {
                            OE_NOTICE << LC << "Recoverable error in HTTPClient for " << request.getURL() << std::endl;
                        }
                    }
                }
            }
        }

        // encode headers
        result.setMetadata(response.getHeadersAsConfig());

        return result;
    }

    osgEarth::ReadResult
        HTTPClient::doReadObject(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* callback)
    {
        initialize();

        osgEarth::ReadResult result;

        HTTPResponse response = this->doGet(request, options, callback);

        if (response.isOK())
        {
            osgDB::ReaderWriter* reader = getReader(request.getURL(), response);
            if (!reader)
            {
                result = osgEarth::ReadResult(osgEarth::ReadResult::RESULT_NO_READER);
            }

            else
            {
                osgDB::ReaderWriter::ReadResult rr;

                if (response.getNumParts() > 0)
                    rr = reader->readObject(response.getPartStream(0), options);

                if (rr.validObject())
                {
                    result = osgEarth::ReadResult(rr.takeObject());
                }
                else
                {
                    if (s_HTTP_DEBUG)
                    {
                        OE_WARN << LC << reader->className()
                            << " failed to read object from " << request.getURL()
                            << "; message = " << rr.message()
                            << std::endl;
                    }
                    result = osgEarth::ReadResult(osgEarth::ReadResult::RESULT_READER_ERROR);
                    result.setErrorDetail(rr.message());
                }
            }

            // last-modified (file time)
            result.setLastModifiedTime(response._lastModified); //getCurlFileTime(_curl_handle) );
        }
        else
        {
            result = osgEarth::ReadResult(
                response.isCanceled() ? osgEarth::ReadResult::RESULT_CANCELED :
                response.getCode() == HTTPResponse::NOT_FOUND ? osgEarth::ReadResult::RESULT_NOT_FOUND :
                response.getCode() == HTTPResponse::NOT_MODIFIED ? osgEarth::ReadResult::RESULT_NOT_MODIFIED :
                response.getCodeCategory() == HTTPResponse::CATEGORY_SERVER_ERROR ? osgEarth::ReadResult::RESULT_SERVER_ERROR :
                osgEarth::ReadResult::RESULT_UNKNOWN_ERROR);

            // for request errors, return an error result with the part data intact
            // so the user can parse it as needed. We only do this for readString.
            if (response.getNumParts() > 0u)
            {
                result.setErrorDetail(response.getPartAsString(0));

                if (s_HTTP_DEBUG)
                {
                    OE_WARN << LC << "SERVER REPORTS: " << result.errorDetail() << std::endl;
                }
            }

            //If we have an error but it's recoverable, like a server error or timeout then set the callback to retry.
            if (HTTPClient::isRecoverable(result.code()))
            {
                if (callback)
                {
                    callback->setRetryDelay(getRetryDelay());
                    callback->cancel();

                    if (s_HTTP_DEBUG)
                    {
                        if (response.isCanceled())
                        {
                            OE_NOTICE << LC << "Request was cancelled" << std::endl;
                        }
                        else
                        {
                            OE_NOTICE << LC << "Recoverable error in HTTPClient for " << request.getURL() << std::endl;
                        }
                    }
                }
            }
        }

        result.setMetadata(response.getHeadersAsConfig());

        return result;
    }


    osgEarth::ReadResult
        HTTPClient::doReadString(const HTTPRequest& request,
            const osgDB::Options* options,
            osgEarth::ProgressCallback* callback)
    {
        initialize();

        osgEarth::ReadResult result;

        HTTPResponse response = this->doGet(request, options, callback);
        if (response.isOK() && response.getNumParts() > 0)
        {
            result = osgEarth::ReadResult(new osgEarth::StringObject(response.getPartAsString(0)));
        }
        else
        {
            result = osgEarth::ReadResult(
                response.isCanceled() ? osgEarth::ReadResult::RESULT_CANCELED :
                response.getCode() == HTTPResponse::NOT_FOUND ? osgEarth::ReadResult::RESULT_NOT_FOUND :
                response.getCode() == HTTPResponse::NOT_MODIFIED ? osgEarth::ReadResult::RESULT_NOT_MODIFIED :
                response.getCodeCategory() == HTTPResponse::CATEGORY_SERVER_ERROR ? osgEarth::ReadResult::RESULT_SERVER_ERROR :
                osgEarth::ReadResult::RESULT_UNKNOWN_ERROR);

            // for request errors, return an error result with the part data intact
            // so the user can parse it as needed. We only do this for readString.
            if (response.getNumParts() > 0u)
            {
                result.setErrorDetail(response.getPartAsString(0));

                if (s_HTTP_DEBUG)
                {
                    OE_NOTICE << LC << "SERVER REPORTS: " << result.errorDetail() << std::endl;
                }
            }

            //If we have an error but it's recoverable, like a server error or timeout then set the callback to retry.
            if (HTTPClient::isRecoverable(result.code()))
            {
                if (callback)
                {
                    callback->setRetryDelay(getRetryDelay());
                    callback->cancel();

                    if (s_HTTP_DEBUG)
                    {
                        if (response.isCanceled())
                        {
                            OE_NOTICE << LC << "HTTP request was cancelled" << std::endl;
                        }
                        else
                        {
                            OE_NOTICE << LC << "Recoverable error in HTTPClient for " << request.getURL() << std::endl;
                        }
                    }
                }
            }
        }

        // encode headers
        result.setMetadata(response.getHeadersAsConfig());

        // last-modified (file time)
        result.setLastModifiedTime(response._lastModified);

        return result;
    }
}