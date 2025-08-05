#include <agg_web2/biz/recall/recall_web_agg_processor.h>
#include "agg_web2/common/common.h"
#include <agg2/helper/agg_context_helper.h>
#include <agg2/util/string_tool.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, RecallWebAggProcessor);

const static string PARAM_AGG_HOST = "agg_host";

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(RecallWebAggProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(RecallWebAggProcessor)

RecallWebAggProcessor::RecallWebAggProcessor()
{
}

RecallWebAggProcessor::~RecallWebAggProcessor()
{
}

bool RecallWebAggProcessor::DoInit(const String2String & params)
{
    std::string tmpHost;
    PARSE_PROCESSOR_PARAM(params, PARAM_AGG_HOST,tmpHost);
    std::string::size_type pos = tmpHost.find(":");
    if (pos != std::string::npos) {
        mAggHost = tmpHost.substr(0,pos);
    }
    else
    {
        mAggHost = tmpHost;
    }
    return true;
}

void RecallWebAggProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

int RecallWebAggProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
   
    mDebugInfoPtr = LLMDebugInfoPtr(new LLMDebugInfo);
    string query = AggContextHelper::GetRawQuery(aggContext);
    if (!query.empty())
        query = StringTool::ReplaceStr(query, "\n", "");

    // ToDo：增加Query改写逻辑
    
    // step 0, Request Agg
    HttpResponsePtr aggResponse = AsyncRequestAgg(aggContext, query); // 异步请求agg
    
    if (!aggResponse) {
        return PR_REQUEST_EXIT;
    }
    
    // step 1, Wait Agg
    AggWebResultPtr aggResultPtr = nullptr;
    ResultPtr result(new AggWebResult(mDebugInfoPtr, false, false));
    aggResultPtr = dynamic_pointer_cast<AggWebResult>(result);
    // aggResultPtr->SetGaoKaoMode(); //TODO

    if (!aggResponse->GetResult(result)) {
        AGG_LOG(ERROR, "agg web response failed");
        return PR_REQUEST_EXIT; 
    }
    
    if (aggResultPtr) {
        // step 2, SetValue
        //AGG_LOG(ERROR, "agg result:%d, qrs:%s", (int32_t)aggResultPtr->mDocPtrVec.size(), aggResultPtr->mQrsClause.c_str());
        //
        for (size_t i = 0; i < aggResultPtr->mDocPtrVec.size(); ++i) {
            aggResultPtr->mDocPtrVec[i]->mRecall = "WEB_AGG";
        }
        mAggContext->SetValue("RAWQUERY_WEB_AGG_RESULT", aggResultPtr->mDocPtrVec);
        mAggContext->SetValue("RAWQUERY_WEB_AGG_QRS_CLAUSE", aggResultPtr->mQrsClause);
    }

    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "RecallWebAggProcessor Cost:%d", end_time - start_time);
    return PR_SUCCESS;
}

HttpResponsePtr RecallWebAggProcessor::AsyncRequestAgg(AggContext *aggContext, std::string query)
{
    AggWebRequest * aggRequest = new AggWebRequest(aggContext, query);
    aggRequest->SetHttpMethod(HM_GET);
    aggRequest->SetHost(mAggHost);
    HttpRequestPtr httpRequest(aggRequest);
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "agg_web_server/agg");
    HttpResponsePtr aggResponse = wrapper->AsyncRequest(httpRequest);
    
    string aggRequestStr;
    aggRequest->GetRequestStr(aggRequestStr);
    AGG_LOG(INFO,"agg web query=%s, request=%s", query.c_str(), aggRequestStr.c_str());
    return aggResponse;
}

BaseProcessor * RecallWebAggProcessor::DoClone()
{
    return new RecallWebAggProcessor(*this);
}

void RecallWebAggProcessor::DoDestroy()
{
    delete this;
}

bool RecallWebAggProcessor::RegisterMethods()
{
    return true;
}

bool RecallWebAggProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);

