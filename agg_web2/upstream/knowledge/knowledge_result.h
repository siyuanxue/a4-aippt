#ifndef AGGWEB2_UPSTREAM_KNOWLEDGE_RESULT_H
#define AGGWEB2_UPSTREAM_KNOWLEDGE_RESULT_H

#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
#include <agg2/helper/tixml_helper.h>
#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(agg_web2);

struct KnowledgeDoc
{
    std::string mTitle;
    std::string mBody;
    std::string mUrl;
    std::string mDataType;
    std::string mPublishTime;
    double mPr = 0.0;
    double mQtc = 0.0;
    int32_t mTriggerLevel = -1;
};

class KnowledgeResult : public Result
{
    DECLARE_SERIALIZATION_FUNC();
public:
    KnowledgeResult();
    virtual ~KnowledgeResult();

public:
    std::string mBody;
    std::vector<KnowledgeDoc> mDocs;
    int32_t mTriggerLevel = -1;

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

    void BodyClean(std::string& RawBody);

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(KnowledgeResult);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::KnowledgeResult);

#endif

