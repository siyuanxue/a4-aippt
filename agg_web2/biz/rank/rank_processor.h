#ifndef MEDCHATAGG_AIGC_RANK_PROCESSOR_H
#define MEDCHATAGG_AIGC_RANK_PROCESSOR_H

#include <agg2/processor/base_processor.h>
#include <agg_web2/biz/general_llm/general_llm_processor.h>
#include <agg2/common/agg_context.h>
#include <agg_web2/common/common.h>
#include <agg2/util/string_tool.h>
#include <agg_web2/util/AggWebStringTool.h>
//#include <agg_web2/upstream/web_agg/aggweb_request.h>
//#include <agg_web2/upstream/web_agg/aggweb_result.h>
//#include <agg_web2/upstream/qt/qt_request.h>
//#include <agg_web2/upstream/qt/qt_result.h>
//#include <agg_web2/upstream/risk/risk_request.h>
//#include <agg_web2/upstream/risk/risk_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class RankProcessor : public  BaseProcessor
{
public:
    RankProcessor() {}
    ~RankProcessor() {}

public:
    bool DoInit(const String2String& params) override;

    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;

    BaseProcessor* DoClone() override;
    void DoDestroy() override;
    int DoProcess(AggContext* aggContext) override;

private:
    void SplitDoc2Psg(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    void GetScores(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    void SelectDocs(std::vector<LLMDocPtr>& LLMDocPtrQueue, std::string& Source);

	void GetRerankResult(std::vector<LLMDocPtr>& docPtrVec, std::string& Source);
    bool GetAggResult(std::vector<LLMDocPtr>& docPtrVec);
    bool GetAigcResult(std::vector<LLMDocPtr>& docPtrVec);

    void MergeFinalQueue(std::vector<LLMDocPtr>& hitQueueAgg,
            std::vector<LLMDocPtr>& hitQueueAigc);

	double GetAuthorityWeight(const LLMDocPtr docPtr);

    HttpResponsePtr RequestQTC(std::string& qtc_request_dat);
    HttpResponsePtr RequestPr(std::string& pr_request_dat);
    std::vector<double> GetQTC(HttpResponsePtr resp);
    std::vector<double> GetPr(HttpResponsePtr resp);
    void BatchRequestPR(const std::string& query,
            std::vector<LLMPsgPtr>& cands,
            std::vector<std::vector<LLMPsgPtr>>& psgSampleVec,
            std::vector<HttpResponsePtr>& prRespVec);
    void BatchRequestQTC(const std::string& query,
            std::vector<LLMPsgPtr>& cands,
            std::vector<std::vector<LLMPsgPtr>>& psgSampleVec,
            std::vector<HttpResponsePtr>& qtcRespVec);
    void FilterRequestQtc(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void FilterRequestPr(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void FilterRequestPrNew(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void ReRankByHost(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    void ReRankBySource(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    void ReRankByTime(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    void DeDuplicate(std::vector<LLMDocPtr>& LLMDocPtrQueue, float sim_threshold, bool bySource);
    std::string GetFormatTimeStr(int32_t Currenttime);
    int32_t GetFormatTimeInt32(std::string timestr);
    std::vector<int> extractYears(const std::string& text);
    void RequestAnswerLocateModel(vector<LLMDocPtr> &LLMDocPtrQueue, uint32_t topN);
    bool ReCatAnswer(const string &alResult, const string& query, const string& title, const string& passage, string& answer);
    bool GetIndexedPassage(const string &doc, string &indexed_passage, vector<string> &sentences, vector<pair<int, int> > &sent_group);
    int GetUtf8LetterNumber(const string &s);
    HttpResponsePtr AsyncRequestSummary(const std::string& query, const std::string& title, const std::string& passage);

protected:
    std::string mQTCNode;
    std::string mPrNode;
    std::string mAnswerLocateLLMName;
    std::string mApiKey;
    std::string mGeneralLLMSyncNode;

    Context* mContext;
    AggContext* mAggContext;

private:
    AGG_LOG_DECLARE();
    AGG_LOG_DECLARE_WITH_NAME(displayLog);

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(RankProcessor);
AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(aigc)::RankProcessor);

#endif //MEDCHATAGG_RANK_PROCESSOR_H
