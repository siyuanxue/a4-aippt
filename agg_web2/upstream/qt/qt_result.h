#ifndef AGGWEB2_UPSTREAM_QT_RESULT_H
#define AGGWEB2_UPSTREAM_QT_RESULT_H

#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
#include <agg2/helper/tixml_helper.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class QtResult : public Result
{
    DECLARE_SERIALIZATION_FUNC();
public:
    QtResult();
    virtual ~QtResult();

public:
    std::vector<double> mQtScores;

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(QtResult);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::QtResult);

#endif

