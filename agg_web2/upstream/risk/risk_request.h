#ifndef AGG_WEB2_UPSTREAM_RISK_REQUEST_H
#define AGG_WEB2_UPSTREAM_RISK_REQUEST_H
#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>
AGG_BEGIN_NAMESPACE(arbiter);
class RiskRequest : public HttpRequest
{
    DECLARE_SERIALIZATION_FUNC();
public:
    RiskRequest();
    RiskRequest(const std::string& summary);
    virtual ~RiskRequest();
public:
    bool DoEncode(std::string& uriQuery, std::string & body, String2String & header) override;
private:
    std::string mSummary;
private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(RiskRequest);
AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::RiskRequest);
#endif
