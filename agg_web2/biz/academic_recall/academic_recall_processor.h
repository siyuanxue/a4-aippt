#ifndef AGG_RECALL_ACADEMIC_RECALL_PROCESSOR_H
#define AGG_RECALL_ACADEMIC_RECALL_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg_web2/common/common.h>
#include <agg2/processor/base_processor.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class AcademicRecallProcessor: public BaseProcessor
{
public:
    AcademicRecallProcessor();
    virtual ~AcademicRecallProcessor();
public:
    virtual bool DoInit(const String2String & params) override;
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    virtual BaseProcessor* DoClone() override;
    virtual void DoDestroy() override;

public:
    struct FeatureNode {
        std::string mPK;
        std::string mTitle;
        std::string mChunk;
        std::string mEmbSource;
        std::string mLayout;
        int mRefPageNum;
    };

public:
    int GetQueryEmb(AggContext * aggContext);
    void RequestQueryEmbeddding(const std::string &query);
    void GetQueryEmbeddding(vector<double> &queryEmb);

    // new query embedding
    void RequestQueryEmbedddingL3(const std::string &query);
    void GetQueryEmbedddingL3(vector<double> &queryEmb);

    int GetBody(AggContext * aggContext);
    void GetMainTextRecall(vector<double> &queryEmb, int32_t topn);
    void GetNoteTextRecall(vector<double> &queryEmb, int32_t topn);

    int GetSummary(AggContext * aggContext);
    bool SendPkQrsRequest(AggContext * aggContext, 
                            const std::string& responseKey_str,
                            const std::string &pk_str,
                            int hitNum,
                            std::string nodeName);
    bool GetMainTextPkQrsResult(AggContext * aggContext, 
                            const std::string& responseKey_str,
                            std::unordered_map<std::string, FeatureNode> &pk_map);
    bool GetNoteTextPkQrsResult(AggContext * aggContext, 
                            const std::string& responseKey_str,
                            std::unordered_map<std::string, DocumentNode> &pk_map);
public:
    // register UDF MethodFunc
    virtual bool RegisterMethods() override;

public:
    // register UDF actor
    virtual bool RegisterActors() override;

private:
    std::string mQueryEmbNode;
    std::string mBodyEmbNode;
    std::string mBodyFeatureNode;

    std::string mNoteEmbNode;
    std::string mNoteFeatureNode;

    std::string mDebugInfo;
    HttpResponsePtr mQueryEmbResponse;
    int mHitDocNum;
    int mHitNoteNum;

private:
    AGG_LOG_DECLARE();

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(AcademicRecallProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_RECALL_ACADEMIC_RECALL_PROCESSOR_H
