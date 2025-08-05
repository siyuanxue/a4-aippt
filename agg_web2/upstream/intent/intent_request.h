#ifndef AGG_WEB2_UPSTREAM_INTENT_REQUEST_H
#define AGG_WEB2_UPSTREAM_INTENT_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>

AGG_BEGIN_NAMESPACE(arbiter);

class IntentRequest : public HttpRequest
{
    DECLARE_SERIALIZATION_FUNC();

public:
    IntentRequest();
    IntentRequest(const std::string& query, const vector<std::string>& historys, const std::string& version);
    virtual ~IntentRequest();

public:
    bool DoEncode(std::string& uriQuery, std::string & body, String2String & header) override;

private:
    std::string mQuery;
    vector<std::string> mHistorys;
    std::string mVersion;
private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(IntentRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::IntentRequest);

#endif

