#ifndef AGG_WEB2_BIZ_KNOWLEDGE_SIMPLE_RANK_PROCESSOR_H
#define AGG_WEB2_BIZ_KNOWLEDGE_SIMPLE_RANK_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>
#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class SimpleRankProcessor : public BaseProcessor
{
public:
    SimpleRankProcessor();
    virtual ~SimpleRankProcessor();

public:
    virtual bool DoInit(const String2String & params) override;
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    virtual BaseProcessor * DoClone() override;
    virtual void DoDestroy() override;
    virtual int DoProcess(AggContext * aggContext) override;

public:
    // register UDF MethodFunc
    virtual bool RegisterMethods() override;

public:
    // register UDF actor
    virtual bool RegisterActors() override;

private:
    void FormatValidResult(const std::string rawQuery, std::vector<LLMDocPtr>& hitQueueAgg); 
    void FormatEmptyResult(const std::string message);
    std::vector<double> GetQtScores(AggContext* aggContext, const std::string& query, const std::vector<std::string>& titles);
    
    void SplitDoc2Psg(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    void GetScores(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    void SelectDocs(std::vector<LLMDocPtr>& LLMDocPtrQueue, std::string& Source);

	void GetRerankResult(std::vector<LLMDocPtr>& docPtrVec, std::string& Source);
    bool GetAggResult(std::vector<LLMDocPtr>& docPtrVec);
    
    HttpResponsePtr RequestQTC(std::string& qtc_request_dat);
    std::vector<double> GetQTC(HttpResponsePtr resp);
    void BatchRequestQTC(const std::string& query,
            std::vector<LLMPsgPtr>& cands,
            std::vector<std::vector<LLMPsgPtr>>& psgSampleVec,
            std::vector<HttpResponsePtr>& qtcRespVec);
    void FilterRequestQtc(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void FilterRequestPrOld(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void FilterRequestPr(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void FilterRequestPrLLM(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void FilterRequestPrMiniCpm(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter);
    void ReRank(std::vector<LLMDocPtr>& LLMDocPtrQueue);
    
    // 答案定位
    void RequestAnswerLocateModel(vector<LLMDocPtr> &LLMDocPtrQueue, uint32_t topN);
    bool ReCatAnswer(const string &alResult, const string& query, const string& title, const string& passage, string& answer);
    bool GetIndexedPassage(const string &doc, string &indexed_passage, vector<string> &sentences, vector<pair<int, int> > &sent_group);
    int GetUtf8LetterNumber(const string &s);
    HttpResponsePtr AsyncRequestSummary(const std::string& query, const std::string& title, const std::string& passage);

private:
    std::string mDebugInfo;
    std::string mQTCNode;
    std::string mAnswerLocateLLMName;
    std::string mApiKey;
    std::string mGeneralLLMSyncNode;

private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(SimpleRankProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_KNOWLEDGE_SIMPLE_RANK_PROCESSOR_H
