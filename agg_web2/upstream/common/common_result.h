#ifndef AGGWEB2_UPSTREAM_COMMON_RESULT_H
#define AGGWEB2_UPSTREAM_COMMON_RESULT_H

#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
#include <agg2/helper/tixml_helper.h>
#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(agg_web2);

struct CommonDoc
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

class CommonResult : public Result
{
    DECLARE_SERIALIZATION_FUNC();
public:
    CommonResult();
    CommonResult(const std::string& type);
    virtual ~CommonResult();

public:
    std::string mType;
    std::string mBody;
    std::vector<CommonDoc> mDocs;
    double mScore = 0.0;
    int32_t mTriggerLevel = -1;
    std::vector<std::string> mIntents;
    std::vector<double> mScores;

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

    void BodyClean(std::string& RawBody);
    void RemoveEM(std::string& content);
    std::string GetTextFromXML(TiXmlElement * node);
    bool GetPathValue(TiXmlElement *element, const std::vector<std::string> & path, std::string& res);
    std::string GetFormatTime(int Currenttime);
    std::string GetFormatTime(const std::string& CurrenttimeStr);
    bool ParseNews();
    bool ParseGaokaoIntent();

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(CommonResult);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::CommonResult);

#endif

