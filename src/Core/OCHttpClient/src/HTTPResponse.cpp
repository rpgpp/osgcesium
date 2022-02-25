#include "HTTPResponse.h"

namespace OC
{
    HTTPResponse::HTTPResponse(long _code) :
        _response_code(_code),
        _canceled(false),
        _duration_s(0.0),
        _lastModified(0u)
    {
        _parts.reserve(1);
    }

    HTTPResponse::HTTPResponse(const HTTPResponse& rhs) :
        _response_code(rhs._response_code),
        _parts(rhs._parts),
        _mimeType(rhs._mimeType),
        _canceled(rhs._canceled),
        _duration_s(0.0),
        _lastModified(0u)
    {
        //nop
    }

    unsigned
        HTTPResponse::getCode() const {
        return _response_code;
    }

    unsigned
        HTTPResponse::getCodeCategory() const
    {
        return
            getCode() < 100 ? CATEGORY_UNKNOWN :
            getCode() < 200 ? CATEGORY_INFORMATIONAL :
            getCode() < 300 ? CATEGORY_SUCCESS :
            getCode() < 400 ? CATEGORY_REDIRECTION :
            getCode() < 500 ? CATEGORY_CLIENT_ERROR :
            CATEGORY_SERVER_ERROR;
    }

    bool
        HTTPResponse::isOK() const {
        return _response_code == 200L && !isCanceled();
    }

    unsigned int
        HTTPResponse::getNumParts() const {
        return _parts.size();
    }

    unsigned int
        HTTPResponse::getPartSize(unsigned int n) const {
        return _parts[n]->_size;
    }

    const std::string&
        HTTPResponse::getPartHeader(unsigned int n, const std::string& name) const {
        return _parts[n]->_headers[name];
    }

    std::istream&
        HTTPResponse::getPartStream(unsigned int n) const {
        return _parts[n]->_stream;
    }

    std::string
        HTTPResponse::getPartAsString(unsigned int n) const {
        std::string streamStr;
        if (n < _parts.size())
            streamStr = _parts[n]->_stream.str();
        return streamStr;
    }

    const std::string&
        HTTPResponse::getMimeType() const {
        return _mimeType;
    }

    osgEarth::Config
        HTTPResponse::getHeadersAsConfig() const
    {
        osgEarth::Config conf;
        if (_parts.size() > 0)
        {
            for (osgEarth::Headers::const_iterator i = _parts[0]->_headers.begin(); i != _parts[0]->_headers.end(); ++i)
            {
                conf.set(i->first, i->second);
            }
        }
        return conf;
    }
}

