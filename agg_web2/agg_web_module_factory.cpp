#include <agg_web2/agg_web_module_factory.h>
#include <agg_web2/loader/agg_web_data_loader.h> 
#include <agg_web2/loader/agg_web_http_data_loader.h> 
#include <agg_web2/biz/general_llm/general_llm_processor.h>
#include <agg_web2/biz/general_llm/wenchuang_search_processor.h>
#include <agg_web2/biz/general_llm/gaokao_search_processor.h>
#include <agg_web2/biz/common/medchat_parse_processor.h>
#include <agg_web2/biz/common/medchat_cache_processor.h>
#include <agg_web2/biz/common/latency_calculate_processor.h>

#include <agg_web2/biz/trigger/trigger_processor.h>
#include <agg_web2/biz/recall/recall_web_agg_processor.h>
#include <agg_web2/biz/recall/recall_index_processor.h>
#include <agg_web2/biz/rank/rank_processor.h>
#include <agg_web2/biz/llm/llm_processor.h>

#include <agg_web2/biz/qlog/request_qp_processor.h>
#include <agg_web2/biz/qlog/request_index_processor.h>
#include <agg_web2/biz/qlog/qlog_rerank_processor.h>
#include <agg_web2/biz/qlog/sgs_rerank_processor.h>

#include <agg_web2/biz/mount/mount_trigger_processor.h>
#include <agg_web2/biz/debug/simple_rank_processor.h>
#include <agg_web2/biz/debug/xueshu_rank_processor.h>
#include <agg_web2/biz/academic_recall/academic_recall_processor.h>

#ifdef MEM_CHECK_BY_ADDRESS_SANITIZE
/** Address Sanitizer Detector  **/
#include <iostream>
#include <string>
#include <csignal>
#include <unistd.h>
#include <sanitizer/lsan_interface.h>
#endif

#ifdef MEM_CHECK_BY_ADDRESS_SANITIZE
void SignalHandler(int signum)
{
    __lsan_do_recoverable_leak_check();
}

// to be deleted
void SignalRegister()
{
    signal(SIGQUIT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGSEGV, SignalHandler);
    signal(SIGINT,  SignalHandler);
}
#endif

A4_USE_NAMESPACE(module);
AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, AggWeb2ModuleFactory);

std::string AggWeb2ModuleFactory::mLocalIpStr = "";

AggWeb2ModuleFactory::AggWeb2ModuleFactory()
{
}

AggWeb2ModuleFactory::~AggWeb2ModuleFactory() 
{ 
}

bool AggWeb2ModuleFactory::AggInit(const String2String& params)
{
    mParams = params;
    
    return true;
}

Processor* AggWeb2ModuleFactory::AggCreateProcessor(const std::string& className)
{
    if (className == "GeneralLLMProcessor") {
        return new GeneralLLMProcessor;
    }else if (className == "MedChatParseProcessor") {
        return new MedChatParseProcessor;
    }else if (className == "MedChatCacheProcessor") {
        return new MedChatCacheProcessor;
    }else if (className == "LatencyCalculateProcessor") {
        return new LatencyCalculateProcessor;
    }else if (className == "WenChuangSearchProcessor") {
        return new WenChuangSearchProcessor;
    }else if (className == "GaoKaoSearchProcessor") {
        return new GaoKaoSearchProcessor;
    }else if (className == "TriggerProcessor") {
        return new TriggerProcessor;
    }else if (className == "RecallWebAggProcessor") {
        return new RecallWebAggProcessor;
    }else if (className == "RecallIndexProcessor") {
        return new RecallIndexProcessor;
    }else if (className == "RankProcessor") {
        return new RankProcessor;
    }else if (className == "LLMProcessor") {
        return new LLMProcessor;
    }else if (className == "RequestQpProcessor") {
        return new RequestQpProcessor;
    }else if (className == "RequestIndexProcessor") {
        return new RequestIndexProcessor;
    }else if (className == "QlogRerankProcessor") {
        return new QlogRerankProcessor;
    }else if (className == "SgsRerankProcessor") {
        return new SgsRerankProcessor;
    }else if (className == "MountTriggerProcessor") {
        return new MountTriggerProcessor;
    }else if (className == "SimpleRankProcessor") {
        return new SimpleRankProcessor;
    }else if (className == "XueshuRankProcessor") {
        return new XueshuRankProcessor;
    }else if (className == "AcademicRecallProcessor") {
        return new AcademicRecallProcessor;
    }

    return NULL;
}

AppSelector* AggWeb2ModuleFactory::AggCreateAppSelector(const std::string& className)
{
    return NULL;
}

DataLoader* AggWeb2ModuleFactory::AggCreateDataLoader(const std::string& className)
{
    if (className == "AggWebDataLoader") {
        return new AggWebDataLoader;
    } else if (className == "AggWebHttpDataLoader") {
        return new AggWebHttpDataLoader;
    }    
    return NULL;
}

void AggWeb2ModuleFactory::Destroy()
{
#ifdef MEM_CHECK_BY_ADDRESS_SANITIZE
    __lsan_do_leak_check();
#endif
    delete this;
}

bool AggWeb2ModuleFactory::OnAggPreStartup(const GlobalManager* globalMgr)
{
    AGG_LOG(INFO, "agg web module factory pre-start-up successfully");
    return true;
}

AGG_END_NAMESPACE(agg_web);
