#ifndef AGGWEB2_UPSTREAM_WEB_AGG_WEB_RESULT_H
#define AGGWEB2_UPSTREAM_WEB_AGG_WEB_RESULT_H

#include <agg2/common/agg_common.h>
#include <agg2/common/result.h>
#include <agg2/helper/tixml_helper.h>
#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class AggWebResult : public Result
{
    DECLARE_SERIALIZATION_FUNC();
public:
    AggWebResult();
    AggWebResult(LLMDebugInfoPtr debugInfoPtr, bool isOffline, bool isAnquan=false);
    virtual ~AggWebResult();

private:
    bool ParseWeather(TiXmlElement * hitsElement, const std::string& name, int idx);
    bool ParseOw(TiXmlElement * hitsElement, int idx);  //[zisen add]
    bool ParseHoliday(TiXmlElement * hitsElement, const std::string& name, int idx); //[zisen add]
    bool ParseNewsSc(TiXmlElement * hitsElement, const std::string& name, int idx);
    bool ParseScByConfig(TiXmlElement * hitsElement, const std::string& name,
            const std::string& query, int idx);
    bool ParseFilmList(TiXmlElement * hitsElement, const std::string& name, int idx);
    bool ParseCustomerPhone(TiXmlElement * root, const std::string& name, int idx);
    bool ParseWendaSelected(TiXmlElement * root, int idx);

    bool ParseMakeupdata();

    bool GetPathValue(TiXmlElement *element, const std::vector<std::string> & path, std::string& res);
    TiXmlElement * GetPathNode(TiXmlElement *element, const std::vector<std::string> & path);
    void GetQuery(TiXmlElement * rootElement);
    int CheckPrEnable(const std::string & bytes);
    int GetTimeliness(TiXmlElement * rootElement);
    void BodyClean(std::string& RawBody);
    int GetFirstScFilterStatus(const std::string& name);
    std::string GetQueryIntent(TiXmlElement *rootElement);

    bool ParseWenChuang(TiXmlElement * hitsElement);
    bool ParseGaoKao(TiXmlElement * hitsElement);

public:
    void SetOnlyScResult();
    void SetWenChuangMode();
    void SetGaoKaoMode();

public:
    LLMDebugInfoPtr mDebugInfoPtr;
    std::vector<LLMDocPtr> mDocPtrVec;
    std::vector<LLMDocPtr> mScPtrVec;  // [zisen add] 用于存储全部触发的SC，并排序
    std::string mScAnswer; // sc能直接回答
    std::string mBaikeAnswer; // 百度百科、夸克百科能直接回答
    std::string mQuery; // query
    std::map<std::string, double> mUrl2Pr; // url -> pr
    int mFresh = 0; // 时效性
    bool isWeatherSC = false;
    bool isNewsSC = false;
    bool isNewsFlag = false;
    bool isFilmSC = false;
    bool mIsOffline = false; // 离线刷库
    bool mIsAnquanMode = false; // 走安全模型，抽取摘要
    bool mIsWenChuang = false; // 是否文创场景
    bool mIsGaoKao = false; // 是否高考场景
    bool mHitGaokoaSC = false; // 是否触发高考SC
    int mHitGoodHost = 0; //命中高考权威站点的doc数量
    bool mIsSCMode = false;  //是否触发SC
    double mMaxPr = 0.0;
    std::string mDomain; //行业信号
    bool mIsQaTrigger = false;
    std::string mBytes;

    std::string mQrsClause;

public:
    virtual bool Serialize(std::string & bytes) override;
    virtual bool Deserialize(const std::string & bytes) override;

private:
    AGG_LOG_DECLARE();
};
AGG_TYPEDEF_PTR(AggWebResult);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::AggWebResult);

#endif //AGG_COMMON_ARBITER_ARBITER_RESULT_H

