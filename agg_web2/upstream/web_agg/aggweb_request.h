#ifndef AGG_WEB2_UPSTREAM_AGG_WEB_AGG_WEB_REQUEST_H
#define AGG_WEB2_UPSTREAM_AGG_WEB_AGG_WEB_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>

AGG_BEGIN_NAMESPACE(arbiter);

class AggWebRequest : public HttpRequest
{
    DECLARE_SERIALIZATION_FUNC();

public:
    AggWebRequest();
    AggWebRequest(AggContext *aggContext, const std::string& query);
    virtual ~AggWebRequest();

public:
    bool DoEncode(std::string& uriQuery, std::string & body, String2String & header) override;
public:
    const std::string& GetHost() const { return mHost; }
    void SetHost(const std::string& host) { mHost = host;}
    bool GetRequestStr(std::string& req) {
        if (mAggRequestStr.empty()) {
            return false;
        }
        req = mAggRequestStr;
        return true;
    }
private:
    AggContext *mAggContext;
    std::string mHost;
    std::string mAggRequestStr;
    std::string mQuery;
private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(AggWebRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::AggWebRequest);

#endif //AGG_COMMON_ARBITER_ARBITER_REQUEST_H

