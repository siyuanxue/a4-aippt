#ifndef AGG_WEB2_BIZ_KNOWLEDGE_SGS_RERANK_PROCESSOR_H
#define AGG_WEB2_BIZ_KNOWLEDGE_SGS_RERANK_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class SgsRerankProcessor : public BaseProcessor
{
public:
    SgsRerankProcessor();
    virtual ~SgsRerankProcessor();
    
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
    void GetQrsResult(AggContext * aggContext, std::vector<QrsHit*>& qrsHitVec);
    std::vector<double> GetQT(AggContext* aggContext, const std::string& query,
        const std::vector<std::string>& titles);

private:
    struct RankNode {
        std::string mQuery;
        std::string mRawData;
        std::string mIsLBS;
        std::string mProvince;
        double mQt;
    };
    
    void RequestQrsFormatResult(std::vector<RankNode>& rankVec, uint32_t hit);

private:
    std::string mDebugInfo;

private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(SgsRerankProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_KNOWLEDGE_SGS_RERANK_PROCESSOR_H
