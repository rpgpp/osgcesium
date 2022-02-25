#ifndef _OC_HTTP_Request_H_
#define _OC_HTTP_Request_H_

#include "HttpClientDefine.h"
#include <osgEarth/IOTypes>

namespace OC
{
    class _HttpClientExport HTTPRequest
    {
    public:
        /** Constructs a new HTTP request that will acces the specified base URL. */
        HTTPRequest(const std::string& url);

        /** copy constructor. */
        HTTPRequest(const HTTPRequest& rhs);

        /** dtor */
        virtual ~HTTPRequest() { }

        /** Adds an HTTP parameter to the request query string. */
        void addParameter(const std::string& name, const std::string& value);
        void addParameter(const std::string& name, int value);
        void addParameter(const std::string& name, double value);

        typedef osgEarth::UnorderedMap<std::string, std::string> Parameters;

        /** Ready-only access to the parameter list (as built with addParameter) */
        const Parameters& getParameters() const;

        //! Add a header name/value pair to an HTTP request
        void addHeader(const std::string& name, const std::string& value);

        //! Collection of headers in this request
        const osgEarth::Headers& getHeaders() const;

        //! Collection of headers in this request
        osgEarth::Headers& getHeaders();

        /**
         * Sets the last modified date of any locally cached data for this request.  This will
         * automatically add a If-Modified-Since header to the request
         */
        void setLastModified(const osgEarth::DateTime& lastModified);

        /** Gets a copy of the complete URL (base URL + query string) for this request */
        std::string getURL() const;

    private:
        Parameters _parameters;
        osgEarth::Headers _headers;
        std::string _url;
    };

}

#endif // !1
