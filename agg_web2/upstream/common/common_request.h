#ifndef AGG_WEB2_UPSTREAM_COMMON_REQUEST_H
#define AGG_WEB2_UPSTREAM_COMMON_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>

AGG_BEGIN_NAMESPACE(arbiter);

class CommonRequest : public HttpRequest
{
    DECLARE_SERIALIZATION_FUNC();

public:
    CommonRequest();
    CommonRequest(const std::string& query, const std::string& type);
    virtual ~CommonRequest();

public:
    bool DoEncode(std::string& uriQuery, std::string & body, String2String & header) override;

private:
    std::string mQuery;
    std::string mType;

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(CommonRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::CommonRequest);

#endif

