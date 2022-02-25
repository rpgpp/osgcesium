#ifndef _OC_HTTP_Client_H_
#define _OC_HTTP_Client_H_

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <osgEarth/Progress>

namespace OC
{
    class _HttpClientExport HTTPClient
    {
    public:
        //! Interface for pluggable HTTP implementations
        class Implementation : public osg::Referenced
        {
        public:
            virtual void initialize() = 0;

            virtual HTTPResponse doGet(
                const HTTPRequest& request,
                const osgDB::Options* options,
                osgEarth::ProgressCallback* progress) const = 0;

            virtual void setUserAgent(const std::string&) { }

            virtual void setTimeout(long) { }
            virtual void setReferer(const std::string&) { }
            virtual void setEncoding(const std::string&) { }

            virtual void setConnectTimeout(long) { }

            //! Implementation-specific handle if applicable
            virtual void* getHandle() const { return NULL; }

        protected:
            virtual ~Implementation() {}
        };

        //! Factory object to create implementation instances.
        class ImplementationFactory
        {
        public:
            virtual Implementation* create() const = 0;

            virtual ~ImplementationFactory() {};
        };

        //! Install an implementation factory. Do this before anything else
        static void setImplementationFactory(ImplementationFactory* factory);

        /**
         * Returns true is the result code represents a recoverable situation,
         * i.e. one in which retrying might work.
         */
        static bool isRecoverable(osgEarth::ReadResult::Code code)
        {
            return
                code == osgEarth::ReadResult::RESULT_OK ||
                code == osgEarth::ReadResult::RESULT_SERVER_ERROR ||
                code == osgEarth::ReadResult::RESULT_TIMEOUT ||
                code == osgEarth::ReadResult::RESULT_CANCELED;
        }

        /** Gest the user-agent string that all HTTP requests will use.
            TODO: This should probably move into the Registry */
        static const std::string& getUserAgent();

        /** Sets a user-agent string to use in all HTTP requests.
            TODO: This should probably move into the Registry */
        static void setUserAgent(const std::string& userAgent);

        /** Sets up proxy info to use in all HTTP requests.
            TODO: This should probably move into the Registry */
        static void setProxySettings(const osgEarth::optional<osgEarth::ProxySettings>& proxySettings);

        /** Gets up proxy info to use in all HTTP requests.
            TODO: This should probably move into the Registry */
        static const osgEarth::optional<osgEarth::ProxySettings>& getProxySettings();

        /**
           Gets the timeout in seconds to use for HTTP requests.*/
        static long getTimeout();

        /**
           Sets the timeout in seconds to use for HTTP requests.
           Setting to 0 (default) is infinite timeout */
        static void setTimeout(long timeout);

        /** Sets the suggested delay (in seconds) before a retry should be attempted
            in the case of a canceled request */
        static void setRetryDelay(float value_seconds);
        static float getRetryDelay();

        /**
           Gets the timeout in seconds to use for HTTP connect requests.*/
        static long getConnectTimeout();

        /**
           Sets the timeout in seconds to use for HTTP connect requests.
           Setting to 0 (default) is infinite timeout */
        static void setConnectTimeout(long timeout);

        /**
         * Gets the URLRewriter that is used to modify urls before sending them to the server
         */
        static URLRewriter* getURLRewriter();

        /**
         * Sets the URLRewriter that is used to modify urls before sending them to the server
         */
        static void setURLRewriter(URLRewriter* rewriter);

        static ConfigHandler* getConfigHandler();

        /**
        * Sets the CurlConfigHandler to configurate the CURL library. It can be used for apply client certificates
        */
        static void setConfigHandler(ConfigHandler* handler);

        /**
         * One time thread safe initialization. In osgEarth, you don't need
         * to call this directly; osgEarth::Registry will call it at
         * startup.
         */
        static void globalInit();

        void setReferer(const std::string& value);
        void setEncoding(const std::string& value);
    public:
        /**
         * Reads an image.
         */
        static osgEarth::ReadResult readImage(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions = 0L,
            osgEarth::ProgressCallback* progress = 0L);

        /**
         * Reads an osg::Node.
         */
        static osgEarth::ReadResult readNode(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions = 0L,
            osgEarth::ProgressCallback* progress = 0L);

        /**
         * Reads an object.
         */
        static osgEarth::ReadResult readObject(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions = 0L,
            osgEarth::ProgressCallback* progress = 0L);

        /**
         * Reads a string.
         */
        static osgEarth::ReadResult readString(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions = 0L,
            osgEarth::ProgressCallback* progress = 0L);

        /**
         * Downloads a file directly to disk.
         */
        static bool download(
            const std::string& uri,
            const std::string& localPath);

    public:

        /**
         * Performs an HTTP "GET".
         */
        static HTTPResponse get(const HTTPRequest& request,
            const osgDB::Options* dbOptions = 0L,
            osgEarth::ProgressCallback* progress = 0L);

        static HTTPResponse get(const std::string& url,
            const osgDB::Options* options = 0L,
            osgEarth::ProgressCallback* progress = 0L);

    public:
        HTTPClient();
        virtual ~HTTPClient();

        void readOptions(const osgDB::ReaderWriter::Options* options, std::string& proxy_host, std::string& proxy_port) const;

        HTTPResponse doGet(const HTTPRequest& request,
            const osgDB::Options* options = 0L,
            osgEarth::ProgressCallback* callback = 0L) const;

        osgEarth::ReadResult doReadObject(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions,
            osgEarth::ProgressCallback* progress);

        osgEarth::ReadResult doReadImage(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions,
            osgEarth::ProgressCallback* progress);

        osgEarth::ReadResult doReadNode(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions,
            osgEarth::ProgressCallback* progress);

        osgEarth::ReadResult doReadString(
            const HTTPRequest& request,
            const osgDB::Options* dbOptions,
            osgEarth::ProgressCallback* progress);

        /**
         * Convenience method for downloading a URL directly to a file
         */
        bool doDownload(const std::string& url, const std::string& filename);

        static HTTPClient& getClient();
    private:
        void* _curl_handle;
        std::string _previousPassword;
        long        _previousHttpAuthentication;
        bool        _initialized;
        long        _simResponseCode;
        
        osg::ref_ptr<Implementation> _impl;

        void initialize() const;
        void initializeImpl();

        static ImplementationFactory* _implFactory;

    };


    class _HttpClientExport CURLHTTPImplementationFactory : public HTTPClient::ImplementationFactory
    {
    public:
        HTTPClient::Implementation* create() const;
    };
}

#endif // !1
