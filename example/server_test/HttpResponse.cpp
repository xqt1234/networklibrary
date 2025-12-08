#include "HttpResponse.h"
#include <fstream>
#include "Logger.h"
HttpResponse::HttpResponse()
{
    createCache();
}
HttpResponse::~HttpResponse()
{
}
HttpResponse &HttpResponse::getInstance()
{
    static HttpResponse res;
    return res;
}

std::string buildHtmlResponse(const std::string& htmlContent) {
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: " + std::to_string(htmlContent.length()) + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n" +
        htmlContent;
    return response;
}

std::string HttpResponse::getFile(std::string filename)
{
    std::ifstream ss(filename);
    if(!ss.is_open())
    {
        LOG_INFO("文件不存在");
    }
    std::stringstream buffer;
    buffer << ss.rdbuf();

    return buildHtmlResponse(buffer.str());
}

std::string HttpResponse::getCacheFile(std::string filename)
{
    auto it = m_sources.find(filename);
    if(it != m_sources.end())
    {
        return it->second;
    }
    return getFile("");
}

void HttpResponse::createCache()
{
    std::string res = getFile("index.html");
    m_sources["index.html"] = res;
}
