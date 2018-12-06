#ifndef _COMMON_HTTPCLIENT_H_
#define _COMMON_HTTPCLIENT_H_

#include <string>
#include <unordered_map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "talco/log/log.h"
#include "talus/util/string_algo.h"
#include <talus/protocol/simple_http_request.h>
#include <talus/protocol/simple_http_response.h>
#include "talus/network/tcp_client.h"
#include "talus/network/network_common.h"
#include "talus/network/service.h"
#include "talus/network/services/impl/service_tcp_impl.h"

class HttpClient{
private:
    struct inner_node_type
    {
        int64_t                                         default_timeout_usec;
        std::string                                     ip;
        uint16_t                                        port;
        std::string                                     host;
        std::string                                     path;
        std::string                                     url;
        talus::TcpClient                                       client;
        inner_node_type() : ip(), port(), host(), path(), client()
        {}
    };
    typedef std::shared_ptr<inner_node_type>            inner_node_ptr_type;    

    inner_node_ptr_type                                 _node;
public:
    HttpClient():_node(new inner_node_type())
    {
        
    }
    
    int open(const std::string& url,int64_t default_timeout_usec)
    {
        if (url.empty())
        {
            _AC_ERROR("invalid url:%s", url.c_str());
            return -1;
        }
        std::string             protocol;
        std::string             host;
        uint16_t                port = 80;
        std::string             path;

        talus::algo::http_uri_parse(protocol, host, path, url);
        if (!protocol.empty() && protocol != "http")
        {
            _AC_ERROR("invalid protocol '%s'", protocol.c_str());
            return -1;
        }
        
        std::vector<std::string> host_port;
        talus::algo::split(host_port, host, ":", false);
        if (host_port.size() == 1)
        {
            host = host_port[0];
        }
        else if (host_port.size() == 2)
        {
            host = host_port[0];
            port = ::atoi(host_port[1].c_str());
            if (port <= 0)
            {
                _AC_ERROR("invalid port '%s'", host_port[1].c_str());
                return -1;
            }
        }
        else
        {
            _AC_ERROR("invalid host-port '%s'", host.c_str());
            return -1;
        }
        
        return construct(std::string(), port, host, path, url, default_timeout_usec);
    }
    
    int get(const std::string& path,const std::string& params,talus::SimpleHttpResponse& res){
        talus::SimpleHttpRequest req;
        req.init(_node->host + path +"?" + params);
        return query(req,res,_node->default_timeout_usec);
    }
    
    int get(const std::string& params,talus::SimpleHttpResponse& res){
        return get(_node->path,params,res);
    }

    int get(talus::SimpleHttpResponse& res){
        talus::SimpleHttpRequest req;
        req.init(_node->url);
        return query(req,res,_node->default_timeout_usec);
    }

    int get(const std::string& path,const std::unordered_map<std::string,std::string> params,talus::SimpleHttpResponse& res){
        talus::SimpleHttpRequest req;
        req.init(_node->host + path + "?" + talus::algo::http_query_build(params));
        return query(req,res,_node->default_timeout_usec);
    }

    int get(const std::unordered_map<std::string,std::string> params,talus::SimpleHttpResponse& res){
        return get(_node->path,params,res);
    }
    
    int post(const std::string& path,const std::string& content,talus::SimpleHttpResponse& res){
        talus::SimpleHttpRequest req;
        req.init(_node->host + path,content);
        return query(req,res,_node->default_timeout_usec);
    }
    
    int post(const std::string& content,talus::SimpleHttpResponse& res){
        return post(_node->path,content,res);
    }
    
    virtual int query(talus::Request& request, talus::Response& response, int64_t timeout_usec)
    {
        talus::SimpleHttpRequest*     req  = dynamic_cast<talus::SimpleHttpRequest*>(&request);
        talus::SimpleHttpResponse*    resp = dynamic_cast<talus::SimpleHttpResponse*>(&response);
        if (!req)
        {
            _AC_WARNING("invalid request type - '%s'", request.protocol().c_str());
            return -1;
        }
        if (!resp)
        {
            _AC_WARNING("invalid response type - '%s'", response.protocol().c_str());
            return -1;
        }
        if (timeout_usec <= 0)
        {
            _AC_WARNING("url:%s%s no time left", _node->host.c_str(),_node->path.c_str());
            return -1;
        }
        timeout_usec = std::min(timeout_usec, _node->default_timeout_usec);
        return talus::impl::service_tcp_query(*req, *resp, timeout_usec, _node->client, name(), _node->ip, _node->port);
    }

    const std::string name() {return "http_client";}
private:
    
    int construct(const std::string& ip, uint16_t port, const std::string& host, const std::string& path, const std::string& url, int64_t default_timeout_usec)
    {
        if (ip.empty())
        {
            std::vector<std::string> ip_list;
            if (talus::network::get_ip_from_host(ip_list, host) < 0 || ip_list.empty())
            {
                _AC_ERROR("%s_service: fail to get ip from hostname '%s'", name().c_str(), host.c_str());
                return -1;
            }
            _node->ip   = ip_list[0];
        }
        else
        {
            _node->ip   = ip;
        }
        if (default_timeout_usec < 1)
        {
            default_timeout_usec    = talus::Service::DEFAULT_TIMEOUT_USEC;
        }
        _node->url                  = url;
        _node->port                 = port;
        _node->host                 = host;
        _node->path                 = path;
        _node->default_timeout_usec = default_timeout_usec;
        return 0;
    }
};



#endif
