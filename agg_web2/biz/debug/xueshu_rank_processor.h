#ifndef AGG_WEB2_BIZ_KNOWLEDGE_XUESHU_RANK_PROCESSOR_H
#define AGG_WEB2_BIZ_KNOWLEDGE_XUESHU_RANK_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>
#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class XueshuRankProcessor : public BaseProcessor
{
public:
    XueshuRankProcessor();
    virtual ~XueshuRankProcessor();

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

public:
    struct DocNode {
        std::string mID;

        // 原文Chunk信息
        std::vector<DocumentNode> mChunks;
        uint32_t mBestChunkIdx = 0;
        double mBestChunkLLMScore = 0.0;
        double mBestChunkQtcScore = 0.0;
        double mBestChunkQtScore = 0.0;
        
        // 题录信息
        std::string mTitle;
        std::string mSummary;
        //...
        double mMetaLLMScore = 0.0;
        double mMetaQtcScore = 0.0;

        double mQTScore = 0.0;
        double mQTCScore = 0.0;
        double mRankScore = 0.0;
    };
private:
    void GetQrsResult(AggContext * aggContext, std::vector<QrsHit*>& qrsHitVec);
    std::string getHa3Value(const string & body, const string &key);
    
    void GetNoteFeature(vector<DocumentNode>& rankVec, uint32_t batchSize);
    void GetHa3Result(vector<DocumentNode> &rankVec);
    void GetLLMScore(const std::string &query, vector<DocumentNode> &rankVec, uint32_t batchSize);
    void GetQtcScore(const string& query, vector<DocumentNode> &rankVec, uint32_t batchSize);
    void SortAndMerge(vector<DocumentNode> &rankVec);
    void BatchRequestQTC(const std::string& query,
        std::vector<DocumentNode>& cands,
        std::vector<HttpResponsePtr>& qtcRespVec);
    HttpResponsePtr RequestQTC(std::string& qtc_request_dat);
    std::vector<double> GetQTC(HttpResponsePtr httpResponse);
    void MergeChunks(vector<DocumentNode> &rankVec, vector<DocNode> &docVec);
    
    std::vector<double> GetQT(const std::string& query, const std::vector<std::string>& titles);
    void GetDocScores(const string& query, vector<DocNode> &docVec);
    void GetPrScore(const string& query, vector<DocumentNode> &rankVec, uint32_t batch_size);

    void OutputResult(const string& query, vector<DocNode> &rankVec, uint32_t hit);
    void DeDuplicate(vector<DocNode> &docVec);

private:
    std::string mDebugInfo;
    std::string mQTCNode;
    uint32_t mCutoffNum = 1000;

private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(XueshuRankProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_KNOWLEDGE_XUESHU_RANK_PROCESSOR_H
