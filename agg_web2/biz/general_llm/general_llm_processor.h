#ifndef MEDCHATAGG_GENERAL_LLM_PROCESSOR_H
#define MEDCHATAGG_GENERAL_LLM_PROCESSOR_H
#include <agg2/processor/base_processor.h>
#include <agg2/common/agg_context.h>
#include <agg_web2/common/common.h>
#include <agg2/util/string_tool.h>
#include <agg_web2/util/AggWebStringTool.h>
#include <agg_web2/upstream/web_agg/aggweb_request.h>
#include <agg_web2/upstream/web_agg/aggweb_result.h>
#include <agg_web2/upstream/qt/qt_request.h>
#include <agg_web2/upstream/qt/qt_result.h>
#include <agg_web2/upstream/intent/intent_request.h>
#include <agg_web2/upstream/intent/intent_result.h>
#include <agg_web2/upstream/risk/risk_request.h>
#include <agg_web2/upstream/risk/risk_result.h>
#include <agg_web2/upstream/ae/ae_request.h>
#include <agg_web2/upstream/ae/ae_result.h>
#include <agg_web2/upstream/qdoc_correlation/qd_request.h>
#include <agg_web2/upstream/qdoc_correlation/qd_result.h>
#include <agg_web2/upstream/knowledge/knowledge_request.h>
#include <agg_web2/upstream/knowledge/knowledge_result.h>
#include <agg_web2/upstream/knowledge_grade/knowledge_grade_request.h>
#include <agg_web2/upstream/knowledge_grade/knowledge_grade_result.h>
#include <agg_web2/upstream/common/common_request.h>
#include <agg_web2/upstream/common/common_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class GeneralLLMProcessor : public  BaseProcessor
{
public:
    GeneralLLMProcessor() {}
    ~GeneralLLMProcessor() {}

public:
    bool DoInit(const String2String& params) override;
    bool RegisterMethods() override;
    BaseProcessor* DoClone() override;
    void DoDestroy() override;

public:
    int RequestAgg(AggContext *aggContext);

public:
    bool ConstructRewriteInput(AggContext* aggContext, const std::string& query,
            std::string& prompt, size_t round_limit=3);
    void WriteSlsLog(const std::string& modelOut, const std::string& debugInfo);

private:
    void RequestRewriteModel(AGG::AggContext *aggContext);
    void RequestAggFormatResult(AGG::AggContext *aggContext, AggWebResultPtr aggResultPtr);
    std::string RequestQueryIntent(AGG::AggContext *aggContext, const std::string& query);
    std::string RequestQueryIntentNew(AGG::AggContext *aggContext, const std::string& query, const std::string& intentVersion);

    HttpResponsePtr AsyncRequestAgg(AggContext *aggContext, std::string query);
    HttpResponsePtr AsyncRequestAggRaw(AggContext *aggContext, std::string query);
    AggWebResultPtr WaitAgg(AggContext *aggContext, String2String& cacheMap,
                            HttpResponsePtr aggResponse, HttpResponsePtr knowledge=NULL, HttpResponsePtr news=NULL);
    void ReRankOld(AggWebResultPtr& aggResultPtr);
    AggWebResultPtr ChatPlan(AggContext *aggContext);

    std::string GetDebugInfoPrefix(const std::string& plan);
    void GetDebugInfoPostfix(AggWebResultPtr aggResultPtr);

    void AddSearchDoc(Json::Value& root, AggWebResultPtr aggResultPtr,
            const std::string& postfix, const std::string& intent);

    std::string GetLatestHistoryQuery(AGG::AggContext *aggContext);
    std::vector<double> GetQT(AggContext* aggContext, const std::string& query, const std::vector<std::string>& titles);
    std::string GetSimQuery(AggContext* aggContext, const std::string& query);
    std::string GetQueryIntentMakeup(const std::string& query);
    bool GetNewQueryIntent(AGG::AggContext *aggContext, std::string &query, std::string &mBucket, std::string &mRawQueryIntent, int &status, std::string intentVersion);
    bool RequestSummary(AGG::AggContext *aggContext, const std::string& query
                        , const std::string& title, const std::string& passage
                        , std::string& summary, double& prscore);
    HttpResponsePtr AsyncRequestSummaryNew(AGG::AggContext *aggContext, const std::string& query
                        , const std::string& title, const std::string& passage);
    bool RequestSummaryQdoc(AGG::AggContext *aggContext, const std::string& query
                        , const std::string& title, const std::string& passage
                        , std::string& summary, double& prscore);
    bool RequestKnowledgeGradeResult(AGG::AggContext *aggContext, const std::string& query
                                     , const std::string& doc, std::vector<std::string>& labels
                                     , std::vector<double>& scores);
    void PassageClean(std::string& passage);

    HttpResponsePtr AsyncRequestKnowledge(AggContext *aggContext, std::string query);
    HttpResponsePtr AsyncRequestCommon(AggContext *aggContext, std::string query, const std::string& type);

    bool ConstructSingleRewriteInput(AGG::AggContext *aggContext,
        const std::string& query, std::string& prompt);

    void RequestSingleRewriteModel(AGG::AggContext *aggContext, std::string query);

    int32_t GetQueryIntentStatus(AggContext *aggContext, std::string &query, std::string& queryIntent);
    AggWebResultPtr GetQueryResult(AggContext *aggContext, std::string &query);

    void ClearHeadTailSpace(std::string& input_str);

    void GetCacheMap(String2String& cacheMap, std::string &query);
    void GetCacheMap(AggCacheWrapperPtr& wrapper, String2String& cacheMap,
                     const std::string& sessionid, const std::string& type, const std::string& query);
    void SetCache(const std::string& type, const std::string& query, const std::string& val);

    HttpResponsePtr SafeRequest(AggContext *aggContext, String2String& cacheMap,
                                std::string query, const std::string& type);
    AggWebResultPtr SafeGetAggWeb(String2String& cacheMap, HttpResponsePtr& aggResponse, bool isAnquanMode);
    KnowledgeResultPtr SafeGetKnowledge(String2String& cacheMap, HttpResponsePtr& knowledgeResponse);
    CommonResultPtr SafeGetNews(String2String& cacheMap, HttpResponsePtr& newsResponse);
    //void DeDuplicate(AggWebResultPtr& aggResultPtr);
    bool GetIndexedPassage(const string &doc, string &indexed_passage, vector<string> &sentences, vector<pair<int, int> > &sent_group);
    bool ReCatAnswer(const string &alResult, const string& query, const string& title, const string& passage, string& answer);
    int GetUtf8LetterNumber(const string &s);


public:
    // 重排模块
    struct RankNode {
        LLMDocPtr mDocPtr = nullptr;
        bool mNeedRank = true;
        double mRankScore = 0.0;
        double mPr = 0.0;
        int32_t mHostWeight = -1;

        double mDeltaBodyLen = 0.0;
        double mDeltaTime = 0.0;
        double mDeltaHost = 0.0;
        double mDeltaKg = 0.0;

        std::string toString() {
            std::ostringstream ss;
            if (mDocPtr) {
                ss << mDocPtr->GetDebugString() << ",";
            }
            //ss << "rankScore:" << mRankScore << ",";
            //ss << "pr:" << mPr << ",";
            ss << "hostwei:" << mHostWeight << ",";
            ss << "delta, body:" << mDeltaBodyLen;
            ss << ", time:" << mDeltaTime;
            ss << ", host:" << mDeltaHost;
            ss << ", kg:" << mDeltaKg;
            return ss.str();
        }
    };

    void ReRankNew(AggWebResultPtr& aggResultPtr);
    void ReSetPR(std::vector<RankNode> &rankVec, const std::string &query);
    void ReRankByBodyLen(vector<RankNode> &rankVec);
    void ReRankByTime(vector<RankNode> &rankVec);
    void ReRankByHost(vector<RankNode> &rankVec, const std::string& query);
    void ReRankByKnowledgeGrade(vector<RankNode> &rankVec, const string& query);
    void RequestAnswerLocateModel(std::vector<RankNode> &rankVec, const std::string &query, uint32_t topN);
    
    void DeDuplicate(AggWebResultPtr& aggResultPtr); // 去重模块

private:
    std::string mGeneralLLMSyncNode;
    std::string mQueryIntentLLMNameNew;
    std::string mRewriteLLMName;
    std::string mRewriteLLMNameNew;
    std::string mSingleRewriteLLMName;
    std::string mAnswerLocateLLMName;

    std::string mAiVendor;
    std::string mAggHost;
    std::string mApiKey;
    std::string mTairNodeName;
    bool mSummaryModelSelection = false;
    bool mAggUseRawQuery = true;
    bool mAggNotUseSingleRewrite = true;
    bool mUseRewriteAggResultPtr = false;
    std::string mPrompt;
    std::string mRewriteStr;
    std::string mReferInfo;
    std::string mAggRequestStr;

    std::string mSingleRewriteStr;

    std::string mQuerySelection; // 选择哪个Query

    std::vector<std::string> mQueryRewriteVec;
    std::vector<std::string> mQuerySearchVec;
    std::vector<std::string> mQuerySingleRewriteVec;

    std::string mBucket; // abbucket

    AggWebResultPtr mRawQueryAggResultPtr; // 原始query agg结果
    AggWebResultPtr mRewriteQueryAggResultPtr; // 改写query agg结果
    std::string mRawQueryIntent;
    std::string mRewriteQueryIntent;
    int mChatRound = 0; // 第几轮

    bool mIsOffline = false; // 离线刷库
    std::string mQTModelNode;
    LLMDebugInfoPtr mDebugInfoPtr;

    String2String mCacheMap;

private:
    AGG_LOG_DECLARE();
    AGG_LOG_DECLARE_WITH_NAME(displayLog);

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(GeneralLLMProcessor);
AGG_END_NAMESPACE(agg_web2)
BOOST_CLASS_EXPORT_KEY(AGG_NS(medical)::GeneralLLMProcessor)
#endif //MEDCHATAGG_GENERAL_LLM_PROCESSOR_H

