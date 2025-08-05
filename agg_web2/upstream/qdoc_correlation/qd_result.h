#ifndef AGGWEB2_UPSTREAM_QD_RESULT_H
#define AGGWEB2_UPSTREAM_QD_RESULT_H

#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
#include <agg2/helper/tixml_helper.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class QdResult : public Result
{
    DECLARE_SERIALIZATION_FUNC();
public:
    QdResult();
    virtual ~QdResult();

public:
    double mScore;

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(QdResult);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::QdResult);

#endif

