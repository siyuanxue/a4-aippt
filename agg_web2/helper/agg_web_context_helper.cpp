#include <A4/common/url_decoder.h>
#include <A4/util/hash_algorithm.h>
#include <A4/wrapper/latency_wrapper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg2/data/map_user_data.h>
#include <agg2/wrapper/agg_data_wrapper.h>
#include <agg2/wrapper/agg_cache_wrapper.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/helper/hit_helper.h>
#include <agg2/helper/qrs_hit_helper.h>
#include <agg2/util/dedup/lcs.h>
#include <agg2/wrapper/agg_cache_wrapper.h>
#include <agg2/util/hash_util.h>

A4_USE_NAMESPACE(module);
AGG_BEGIN_NAMESPACE(helper) ;
AGG_LOG_SETUP(agg_web2, AggWebContextHelper);

const std::string KEY_QRS_RESULT_INFO_MAP = "qrs_result_info_map";

void AggWebContextHelper::SetChatResponse(AggContext * aggContext, ChatResponsePtr& responsePtr)
{
    AggUserContext* useContext = aggContext->GetAggUserContext();
    AggWebUserContext* aggWebUserContext =
            dynamic_cast<AggWebUserContext*>(useContext);
    return aggWebUserContext->SetChatResponse(responsePtr);
}

ChatResponsePtr AggWebContextHelper::GetChatResponse(AggContext * aggContext)
{
    AggUserContext* useContext = aggContext->GetAggUserContext();
    AggWebUserContext* aggWebUserContext =
            dynamic_cast<AggWebUserContext*>(useContext);
    if (aggWebUserContext == NULL) {
        return {};
    }
    return aggWebUserContext->GetChatResponse();
}

#if 0
void AggWebContextHelper::SetMidPageResult(AggContext * aggContext, MidPageResult& midPageResult)
{
    AggUserContext* useContext = aggContext->GetAggUserContext();
    AggWebUserContext* aggWebUserContext =
            dynamic_cast<AggWebUserContext*>(useContext);

    if (aggWebUserContext != NULL)
    {
        aggWebUserContext->SetMidPageResult(midPageResult);
    }
}

MidPageResult * AggWebContextHelper::GetMidPageResultPtr(AggContext * aggContext)
{
    AggUserContext* useContext = aggContext->GetAggUserContext();
    AggWebUserContext* aggWebUserContext =
            dynamic_cast<AggWebUserContext*>(useContext);
    if (aggWebUserContext == NULL)
    { return NULL;}

    MidPageResult & midPageResult =  aggWebUserContext->GetMidPageResult();
    return &midPageResult;
}
#endif
void AggWebContextHelper::SetMedChatFilterTag(AggContext * aggContext, std::string filterTag)
{
    AggUserContext* useContext = aggContext->GetAggUserContext();
    AggWebUserContext* aggWebUserContext =
            dynamic_cast<AggWebUserContext*>(useContext);
    return aggWebUserContext->SetMedChatFilterTag(filterTag);
}

std::string AggWebContextHelper::GetMedChatFilterTag(AggContext * aggContext)
{
    AggUserContext* useContext = aggContext->GetAggUserContext();
    AggWebUserContext* aggWebUserContext =
            dynamic_cast<AggWebUserContext*>(useContext);
    return aggWebUserContext->GetMedChatFilterTag();
}

void AggWebContextHelper::SetQrsResultInfo(AggContext * aggContext, const QrsResultPtr& qrsResult, const std::string & info)
{
    QrsResultInfoMapPtr infoPtr;
    if (!aggContext->GetValue<QrsResultInfoMap>(KEY_QRS_RESULT_INFO_MAP, infoPtr)) {
        infoPtr.reset(new QrsResultInfoMap());
        aggContext->SetValue(KEY_QRS_RESULT_INFO_MAP, infoPtr);
    }
    infoPtr->insert(std::make_pair(uint64_t(qrsResult.get()), info));
    //AGG_LOG(DEBUG, "Set qrs result info. key=%lu, value=%s", uint64_t(qrsResult.get()), info.c_str());
}

bool AggWebContextHelper::GetQrsResultInfo(AggContext * aggContext, const QrsResultPtr& qrsResult, std::string & info)
{
    QrsResultInfoMapPtr infoPtr;
    if (!aggContext->GetValue<QrsResultInfoMap>(KEY_QRS_RESULT_INFO_MAP, infoPtr)) {
        AGG_LOG(DEBUG, "Can not find qrs result info.");
        return false;
    }
    auto iter = infoPtr->find(uint64_t(qrsResult.get()));
    if (iter != infoPtr->end()) {
        info = iter->second;
        //AGG_LOG(DEBUG, "Get qrs result info. key=%lu, value=%s", uint64_t(qrsResult.get()), info.c_str());
        return true;
    }
    return false;
}


AGG_END_NAMESPACE(helper);
