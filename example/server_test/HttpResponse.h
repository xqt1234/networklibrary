#include <iostream>
#include <string>
#include <unordered_map>
class HttpResponse
{
private:
    HttpResponse(/* args */);
    ~HttpResponse();
    std::unordered_map<std::string,std::string> m_sources;
    void createCache();
    
public:
    static HttpResponse& getInstance();
    
    std::string getFile(std::string filename);
    std::string getCacheFile(std::string filename);
};

