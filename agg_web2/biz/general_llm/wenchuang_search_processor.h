#ifndef AGG_WEB2_BIZ_GENERAL_LLM_WENCHUANG_SEARCH_PROCESSOR_H
#define AGG_WEB2_BIZ_GENERAL_LLM_WENCHUANG_SEARCH_PROCESSOR_H 
#include <agg2/processor/base_processor.h>
#include <agg_web2/biz/general_llm/general_llm_processor.h>
#include <agg2/common/agg_context.h>
#include <agg_web2/common/common.h>
#include <agg2/util/string_tool.h>
#include <agg_web2/util/AggWebStringTool.h>
#include <agg_web2/upstream/web_agg/aggweb_request.h>
#include <agg_web2/upstream/web_agg/aggweb_result.h>
#include <agg_web2/upstream/common/common_request.h>
#include <agg_web2/upstream/common/common_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);

//class WenChuangSearchProcessor : public  BaseProcessor
class WenChuangSearchProcessor : public GeneralLLMProcessor 
{
public:
    WenChuangSearchProcessor() {}
    ~WenChuangSearchProcessor() {}

public:
    bool DoInit(const String2String& params) override;
    bool RegisterMethods() override;
    BaseProcessor* DoClone() override;
    void DoDestroy() override;
    
    int DoProcess(AggContext * aggContext) override;

private:
    AggWebResultPtr GetQueryResult(AggContext *aggContext, std::string &query);
    HttpResponsePtr AsyncRequestCommon(AggContext *aggContext, std::string query, const std::string& type);
    HttpResponsePtr AsyncRequestAgg(AggContext *aggContext, std::string query);
    void RequestAggFormatResult(AGG::AggContext *aggContext, AggWebResultPtr aggResultPtr);
    void GetDebugInfoPostfix(AggWebResultPtr aggResultPtr);
    
    void ReRankByHost(vector<RankNode> &rankVec);

private:
    LLMDebugInfoPtr mDebugInfoPtr;
    
    std::string mAggHost;
    std::string mReferInfo;

private:
    AGG_LOG_DECLARE();
    AGG_LOG_DECLARE_WITH_NAME(displayLog);

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(WenChuangSearchProcessor);
AGG_END_NAMESPACE(agg_web2)
BOOST_CLASS_EXPORT_KEY(AGG_NS(medical)::WenChuangSearchProcessor)
#endif //AGG_WEB2_BIZ_GENERAL_LLM_WENCHUANG_SEARCH_PROCESSOR_H

