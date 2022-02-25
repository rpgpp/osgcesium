#ifndef _OC_HTTP_Response_H_
#define _OC_HTTP_Response_H_

#include "HttpClientDefine.h"
#include <osgEarth/IOTypes>

namespace OC
{
    class _HttpClientExport HTTPResponse
    {
    public:
        enum Code {
            NONE = 0,
            OK = 200,
            NOT_MODIFIED = 304,
            BAD_REQUEST = 400,
            NOT_FOUND = 404,
            CONFLICT = 409,
            INTERNAL_SERVER_ERROR = 500
        };
        enum CodeCategory {
            CATEGORY_UNKNOWN = 0,
            CATEGORY_INFORMATIONAL = 100,
            CATEGORY_SUCCESS = 200,
            CATEGORY_REDIRECTION = 300,
            CATEGORY_CLIENT_ERROR = 400,
            CATEGORY_SERVER_ERROR = 500
        };

    public:
        /** Constructs a response with the specified HTTP response code */
        HTTPResponse(long code = 0L);

        /** Copy constructor */
        HTTPResponse(const HTTPResponse& rhs);

        /** dtor */
        virtual ~HTTPResponse() { }

        /** Gets the HTTP response code (Code) in this response */
        unsigned getCode() const;

        /** Gets the HTTP response code category for this response */
        unsigned getCodeCategory() const;

        /** True is the HTTP response code is OK (200) */
        bool isOK() const;

        /** True if the request associated with this response was cancelled before it completed */
        void setCanceled(bool value) { _canceled = value; }
        bool isCanceled() const { return _canceled; }

        /** Gets the number of parts in a (possibly multipart mime) response */
        unsigned int getNumParts() const;

        /** Gets the input stream for the nth part in the response */
        std::istream& getPartStream(unsigned int n) const;

        /** Gets the nth response part as a string */
        std::string getPartAsString(unsigned int n) const;

        /** Gets the length of the nth response part */
        unsigned int getPartSize(unsigned int n) const;

        /** Gets the HTTP header associated with the nth multipart/mime response part */
        const std::string& getPartHeader(unsigned int n, const std::string& name) const;

        /** Gets the master mime-type returned by the request */
        void setMimeType(const std::string& value) { _mimeType = value; }
        const std::string& getMimeType() const;

        /** How long did it take to fetch this response (in seconds) */
        void setDuration(double value) { _duration_s = value; }
        double getDuration() const { return _duration_s; }

        void setMessage(const std::string& value) { _message = value; }
        const std::string& getMessage() const { return _message; }

        void setLastModified(osgEarth::TimeStamp value) { _lastModified = value; }
        osgEarth::TimeStamp getLastModified() const { return _lastModified; }

        struct Part : public osg::Referenced
        {
            Part() : _size(0) { }
            osgEarth::Headers _headers;
            unsigned int _size;
            std::stringstream _stream;
        };
        typedef std::vector< osg::ref_ptr<Part> > Parts;

        Parts& getParts() { return _parts; }

    private:
        Parts       _parts;
        long        _response_code;
        std::string _mimeType;
        bool        _canceled;
        double      _duration_s;
        osgEarth::TimeStamp   _lastModified;
        std::string _message;

        osgEarth::Config getHeadersAsConfig() const;

        friend class HTTPClient;
    };

    /**
     * Object that lets you modify and incoming URL before it's passed to the server
     */
    struct _HttpClientExport URLRewriter : public osg::Referenced
    {
        virtual std::string rewrite(const std::string& url) = 0;
    };

    /**
     * A configuration handler to apply settings. It can be used for setting client certificates
     */
    struct _HttpClientExport ConfigHandler : public osg::Referenced
    {
        virtual void onInitialize(void* handle) = 0;
        virtual void onGet(void* handle) = 0;
    };
}

#endif // !1
