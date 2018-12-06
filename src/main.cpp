#include "DSPScheduler.h"

#include <stdio.h>
#include <signal.h>
#include <tr1/memory>

#include "talco/log/log.h"
#include "talco/log/stacktrace.h"
#include "talus/server_wrapper.h"


talus::ServerWrapper::shared_ptr                g_p_serv_wrapper;

void handle_signal(int)
{
    _AC_STACK_TRACE();
    printf("handle signal\n");
    if (!!g_p_serv_wrapper)
    {
        g_p_serv_wrapper->stop();
    }
}

void usage(const char* prog)
{
    printf("Usage: %s [OPTION]\n", prog);
    printf("OPTION:\n");
    printf("  -h                    show this help\n");
    printf("  -d                    daemonize (default: run at frontground)\n");
    printf("  -c CONFIG_FILE        framework config file path (no default)\n");
    printf("  -f CONFIG_FILE        service config file path (no default)\n");
    printf("EXAMPLE:\n");
    printf("  %s -c ../conf/apx_tracking_framework.cfg -f ../conf/apx_tracking.cfg\n", prog);
}

int main(int argc, char *argv[])
{
	bool            daemonize   = false;
    std::string     framework_config_file;
    char            ch;

    while ((ch = getopt(argc, argv, "hdc:f:")) != -1)
    {
        switch (ch)
        {
        case 'h':
            usage(argv[0]);
            return 0;
        case 'd':
            daemonize       = true;
            break;
        case 'c':
            framework_config_file     = optarg;
            break;
        default:
            fprintf(stderr, "ERROR: illegal argument - '%c'!\n\n", ch);
            usage(argv[0]);
            return 1;
        }
    }
    if (framework_config_file.empty())
    {
        fprintf(stderr, "ERROR: config file path must be given\n!\n");
        usage(argv[0]);
        return 1;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    g_p_serv_wrapper.reset(new talus::ServerWrapper());
    talus::ServerWrapper::shared_ptr p_serv_wrapper = g_p_serv_wrapper;
    if (!g_p_serv_wrapper)
    {
        fprintf(stderr, "Create server_wrapper object failed.\n");
        return 1;
    }
    if (g_p_serv_wrapper->init(framework_config_file, daemonize, false) != 0)
    {
        fprintf(stderr, "ERROR: init tcp_serv failed!\n");
        return 1;
    }
	// test
	DSPInfo dsp_test_info1;
        dsp_test_info1.url = "http://sandbox1-mdsp.avazutracking.net/rtbserver1.php";
	dsp_test_info1.post_content = "";
        dsp_test_info1.timeout_usec = 100000000; // 100ms
	
	DSPInfo dsp_test_info2;
        dsp_test_info2.url = "http://sandbox1-mdsp.avazutracking.net/rtbserver2.php";
	dsp_test_info2.post_content = "";
        dsp_test_info2.timeout_usec = 100000000; // 100ms

	DSPInfo dsp_test_info3;
        dsp_test_info3.url = "http://sandbox1-mdsp.avazutracking.net/rtbserver3.php";
	dsp_test_info3.post_content = "";
        dsp_test_info3.timeout_usec = 100000000; // 100ms

        std::vector<DSPInfo> dsplist;
        dsplist.push_back(dsp_test_info1);
        dsplist.push_back(dsp_test_info2);
        dsplist.push_back(dsp_test_info3);
        auto func_parse_resp = [](const std::string& reqId, const std::string& url, talus::SimpleHttpResponse& resp) {
		AC_DEBUG("checkResponse:-------->reqId[%s], url[%s], response_content: %s", reqId.c_str(), url.c_str(), resp.to_string().c_str());
	};
	std::string testReqId = "1234567";
        DSPScheduler::instance().call_third_dsp(testReqId, dsplist, func_parse_resp);
    
    _AC_INFO("Start Servers ...");
    if ( g_p_serv_wrapper->run() != 0 )
    {
        fprintf(stderr, "ERROR: init tcp_serv failed!\n");
    }

    g_p_serv_wrapper->uninit();
    _AC_INFO("%s exit\n", g_p_serv_wrapper->get_serv_name().c_str());

    return 0;
}

