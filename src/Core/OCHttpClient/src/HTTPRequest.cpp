#include "HTTPRequest.h"

namespace OC
{
    HTTPRequest::HTTPRequest(const std::string& url)
        : _url(url)
    {
        //NOP
    }

    HTTPRequest::HTTPRequest(const HTTPRequest& rhs) :
        _parameters(rhs._parameters),
        _headers(rhs._headers),
        _url(rhs._url)
    {
        //nop
    }

    void
        HTTPRequest::addParameter(const std::string& name, const std::string& value)
    {
        _parameters[name] = value;
    }

    void
        HTTPRequest::addParameter(const std::string& name, int value)
    {
        std::stringstream buf;
        buf << value;
        std::string bufStr;
        bufStr = buf.str();
        _parameters[name] = bufStr;
    }

    void
        HTTPRequest::addParameter(const std::string& name, double value)
    {
        std::stringstream buf;
        buf << value;
        std::string bufStr;
        bufStr = buf.str();
        _parameters[name] = bufStr;
    }

    const HTTPRequest::Parameters&
        HTTPRequest::getParameters() const
    {
        return _parameters;
    }

    void
        HTTPRequest::addHeader(const std::string& name, const std::string& value)
    {
        _headers[name] = value;
    }

    const osgEarth::Headers&
        HTTPRequest::getHeaders() const
    {
        return _headers;
    }

    osgEarth::Headers&
        HTTPRequest::getHeaders()
    {
        return _headers;
    }

    void HTTPRequest::setLastModified(const osgEarth::DateTime& lastModified)
    {
        addHeader("If-Modified-Since", lastModified.asRFC1123());
    }


    std::string
        HTTPRequest::getURL() const
    {
        if (_parameters.size() == 0)
        {
            return _url;
        }
        else
        {
            std::stringstream buf;
            buf << _url;
            for (Parameters::const_iterator i = _parameters.begin(); i != _parameters.end(); i++)
            {
                buf << (i == _parameters.begin() && _url.find('?') == std::string::npos ? "?" : "&");
                buf << i->first << "=" << i->second;
            }
            std::string bufStr;
            bufStr = buf.str();
            return bufStr;
        }
    }
}

