//
// Created by Avazu Holding on 2018/12/4.
//

#ifndef APX_CLICK_DSPSCHEDULER_H
#define APX_CLICK_DSPSCHEDULER_H

#include "talus/util/singleton.h"
#include "talus/protocol/simple_http_request.h"
#include "talus/protocol/simple_http_response.h"
#include "talus/util/string_algo.h"
#include "talus/thread/delegated_thread_pool_manager.h"
#include "talco/log/log.h"
#include "track_data_notify.h"

struct DSPInfo {
    std::string url;
    std::string post_content;
    int64_t timeout_usec;
};

// DSP调度通知类，对外接口call_third_dsp
class __DSPScheduler {
public:
    __DSPScheduler() {}
    virtual ~__DSPScheduler() {}

    virtual bool call_third_dsp(const std::string& reqId, const std::vector<DSPInfo>& dsplist, const std::function<void(const std::string&, const std::string&, talus::SimpleHttpResponse&)>& func)
    {
        if (dsplist.empty())
        {
            AC_WARNING("call_third_dsp failed: dsplist is empty");
            return false;
        }
        for(auto dspInfo : dsplist)
        {
            talus::async_delegate("dspcall_threads", [reqId,dspInfo,func]() {
                talus::SimpleHttpResponse resp;
                request(dspInfo, resp);
		func(reqId, dspInfo.url, resp);
            });
        }
        return true;
    }

private:
    static void request(const DSPInfo& dspInfo, talus::SimpleHttpResponse& resp)
    {
        TrackDataNotify::instance().notify(dspInfo.url, dspInfo.post_content, dspInfo.timeout_usec, resp);
    }


};

typedef talus::Singleton<__DSPScheduler> DSPScheduler;

#endif //APX_CLICK_DSPSCHEDULER_H
