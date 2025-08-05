#ifndef AGG_WEB2_UPSTREAM_QD_REQUEST_H
#define AGG_WEB2_UPSTREAM_QD_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>

AGG_BEGIN_NAMESPACE(arbiter);

class QdRequest : public HttpRequest
{
    DECLARE_SERIALIZATION_FUNC();

public:
    QdRequest();
    QdRequest(const std::string& query, const std::string& title, const std::string& passage);
    virtual ~QdRequest();

public:
    bool Encode() override;
    bool DoEncode(std::string& uriQuery, std::string & body, String2String & header) override;

private:
    std::string mQuery;
    std::string mTitle;
    std::string mPassage;
private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(QdRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::QdRequest);

#endif

