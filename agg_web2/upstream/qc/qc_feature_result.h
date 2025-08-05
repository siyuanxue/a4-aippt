#ifndef A4_RECOMMEND_QC_FEATURE_RESULT_H
#define A4_RECOMMEND_QC_FEATURE_RESULT_H

#include <A4/common/common.h>
#include <agg2/common/agg_context.h>
#include <agg2/common/http_request.h>
#include <agg2/common/result.h>

AGG_BEGIN_NAMESPACE(agg_web2);

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wunused-private-field"

class QcFeatureRequest : public HttpRequest
{
public:
    QcFeatureRequest(AggContext * aggContext = NULL);
    virtual ~QcFeatureRequest();

public:
    virtual bool DoEncode(std::string & args, std::string & body,
                          String2String & headers);

    virtual bool InitRequestStr(const std::string& requestStr);
    void SetRequestStr(const std::string & requestStr) { mRequestStr = requestStr;}
    const std::string & GetRequestStr()  { return mRequestStr;}
    const std::vector<double> & GetQcFeature() { return mQcFeature;}

private:
    AggContext * mAggContext;
    std::vector<double> mQcFeature;
    std::string mRequestStr;
private:
    AGG_LOG_DECLARE();

};
//#pragma clang diagnostic pop
AGG_TYPEDEF_PTR(QcFeatureRequest);

class QcFeatureResult : public Result
{
public:
    QcFeatureResult();
    QcFeatureResult(AggContext * aggContext);
    virtual ~QcFeatureResult();

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

    void SetAggContext(AggContext * aggContext) { mAggContext = aggContext;}
    AggContext * GetAggContext() { return mAggContext;}
    const std::vector<std::vector<float>> & GetQcFeature() const { return mQcFeature;}
    std::vector<std::vector<float>> & GetQcFeature() {return mQcFeature;}
    const std::vector<std::vector<float>> & GetQtFeature() const { return mQtFeature;}
    std::vector<std::vector<float>> & GetQtFeature() {return mQtFeature;}
    void InitPR(const MESSER_UTILS_NS(json)::rapidjson::Value& conJson);
    void InitQTC(const MESSER_UTILS_NS(json)::rapidjson::Document& document);

protected:
    std::vector<std::vector<float>> mQcFeature;
    std::vector<std::vector<float>> mQtFeature;
    AggContext * mAggContext;

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(QcFeatureResult);

AGG_END_NAMESPACE(agg_web2);

#endif
