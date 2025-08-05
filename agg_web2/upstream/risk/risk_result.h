#ifndef AGGWEB2_UPSTREAM_RISK_RESULT_H
#define AGGWEB2_UPSTREAM_RISK_RESULT_H

#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
#include <agg2/helper/tixml_helper.h>
#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class RiskResult : public Result
{
    DECLARE_SERIALIZATION_FUNC();

public:
    RiskResult() : mDebugInfoPtr(new LLMDebugInfo) {}
    RiskResult(LLMDebugInfoPtr debugInfoPtr) : mDebugInfoPtr(debugInfoPtr) {}
    virtual ~RiskResult() {}

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;
    bool misNormal = true;

private:
    bool Parse(const std::string & bytes);
    LLMDebugInfoPtr mDebugInfoPtr;

    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(RiskResult);
AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::RiskResult);
#endif
