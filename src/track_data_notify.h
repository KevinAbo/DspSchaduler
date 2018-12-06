#ifndef _SRC_DATAMANAGER_NOTIFY_H_
#define _SRC_DATAMANAGER_NOTIFY_H_

#include "talco/util/tss.h"
#include "talus/util/singleton.h"
#include "talus/util/string_algo.h"
#include "http_client.h"
#include <memory>

class ___TrackDataNotify{
public:
    
    typedef std::unordered_map<std::string,std::shared_ptr<HttpClient>> MapHttpClient;

    ___TrackDataNotify():_tss(___TrackDataNotify::clean_up){

    }
    
    void notify(const std::string& url, const std::string& req_content, int64_t timeout_usec, talus::SimpleHttpResponse& resp){
        MapHttpClient* mapHttpClient = (MapHttpClient*)_tss.get();
        if (mapHttpClient == NULL)
        {
            mapHttpClient = new MapHttpClient();
            _tss.set(mapHttpClient);
        }
        std::shared_ptr<HttpClient> httpclient = create_http_client(*mapHttpClient,url, timeout_usec);
        if (!httpclient){
            return;
        }

        send_notify_to_server(httpclient,url,req_content,resp);
    }
private:
    std::shared_ptr<HttpClient> create_http_client(MapHttpClient& clients,const std::string& url, int64_t timeout_usec){
        std::string             protocol;
        std::string             host;
        std::string             path;
        talus::algo::http_uri_parse(protocol, host, path, url);
        
        auto it = clients.find(host);
        if (it != clients.end()){
            return it->second;
        }
        std::shared_ptr<HttpClient> client = std::make_shared<HttpClient>();
        if(client->open(host,timeout_usec) != 0){
            AC_WARNING("open httpclient faild,host:%s%s,protocol:%s",host.c_str(),path.c_str(),protocol.c_str());
            return NULL;
        }
        clients.emplace(host,client);
        return client;
    }
    
    static void send_notify_to_server(const std::shared_ptr<HttpClient>& client,const std::string& url, const std::string& req_content, talus::SimpleHttpResponse &resp){
        std::string protocol;
        std::string host;
        std::string path;
        std::unordered_map<std::string, std::string> query_params;
        talus::algo::http_uri_parse(protocol, host, path, query_params, url);
        if(client->post(path,req_content,resp) != 0){
            AC_WARNING("notify url:%s failed",url.c_str())
            return;
        }
        if (resp.http_status() != "200"){
            AC_WARNING("notify url:%s failed,http_status:%s",url.c_str(),resp.http_status().c_str());
        }
        AC_DEBUG("notify url:%s ok resp:%s",url.c_str(),resp.http_status().c_str());
    }
    
    static void clean_up(void* ptr){
        if (ptr){
            delete (MapHttpClient*)ptr;
        }
    }
    talco::TSS _tss;
};
typedef talus::Singleton<___TrackDataNotify>    TrackDataNotify;


#endif
