#include <agg_web2/upstream/risk/risk_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, RiskResult);

using namespace std;

BEGIN_DEFINE_SERIALIZATION_FUNC(RiskResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(RiskResult)

bool RiskResult::Serialize(std::string & bytes)
{
    return true;
}

// 夸克文本风险识别服务， https://aliyuque.antfin.com/dda523/rwzd55/eam66mu6qqkoa0gl?singleDoc#
bool CheckShield(const std::string& shield)
{
    return shield == "正常";
}
bool CheckPhyClassify(const std::string& phy_classify)
{
    return phy_classify == "非生理";
}
bool CheckEnglishPolitical(const std::string& english_political)
{
    return english_political == "正常";
}
bool CheckNegativeRecognize(const std::string& negative_recognize)
{
    return negative_recognize == "正常";
}

std::string GetJsonStr(const Json::Value &value, const std::string& key)
{
    if (value.isMember(key) && value[key].isString()) {
        return value[key].asString();
    }
    return "";
}

bool RiskResult::Deserialize(const std::string & bytes)
{
    AGG_LOG(ERROR, "RiskResult::Deserialize [%s]", bytes.c_str());
    bool status = Parse(bytes);
    if (! status) {
        mDebugInfoPtr->AddDebug("final summary has risk info");
    }
    return status;
}

bool RiskResult::Parse(const std::string & bytes)
{
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(bytes, root)) {
        AGG_LOG(ERROR, "parse json string failed [%s]", bytes.c_str());
        return false;
    }
    if (!root.isObject()) {
        AGG_LOG(ERROR, "Risk detect [%s]", bytes.c_str());
        return false;
    }

    bool success = false;
    if (root.isMember("success") && root["success"].isBool()) {
        success = root["success"].asBool();
    }
    if (! success) {
        AGG_LOG(ERROR, "Risk detect [%s], success=%d", bytes.c_str(), success);
        return false;
    }

    if (! root.isMember("data") || ! root["data"].isObject()) {
        AGG_LOG(ERROR, "Risk detect [%s], data", bytes.c_str());
        return false;
    }

    const Json::Value &dataValue = root["data"];
    if (! dataValue.isMember("result") || ! dataValue["result"].isObject()) {
        AGG_LOG(ERROR, "Risk detect [%s], result", bytes.c_str());
        return false;
    }

    const Json::Value &result = dataValue["result"];
    if (result.isMember("error")) {
        AGG_LOG(ERROR, "Risk detect [%s], has error", bytes.c_str());
        return false;
    }

    if (result.isMember("shield") && result["shield"].isString()) {
        string shield = result["shield"].asString();
        if (! CheckShield(shield)) {
            AGG_LOG(ERROR, "Risk detect [%s], shield=%s", bytes.c_str(), shield.c_str());
            return false;
        }
    }

    if (result.isMember("abnormal_sentence") && result["abnormal_sentence"].isArray()) {
        const auto& abnormal_sentence = result["abnormal_sentence"];
        for (size_t i=0; i<abnormal_sentence.size(); ++ i) {
            const auto& sentence = abnormal_sentence[int(i)];
            if (sentence.isArray() && sentence.size() == 2) {
                const auto& info = sentence[1];
                if (! CheckShield(GetJsonStr(info, "shield"))) {
                    AGG_LOG(ERROR, "Risk detect [%s], shield", bytes.c_str());
                    return false;
                }
                if (! CheckNegativeRecognize(GetJsonStr(info, "negative_recognize"))) {
                    AGG_LOG(ERROR, "Risk detect [%s], negative_recognize", bytes.c_str());
                    return false;
                }
                if (! CheckEnglishPolitical(GetJsonStr(info, "english_political"))) {
                    AGG_LOG(ERROR, "Risk detect [%s], english_political", bytes.c_str());
                    return false;
                }
                if (! CheckPhyClassify(GetJsonStr(info, "phy_classify"))) {
                    AGG_LOG(ERROR, "Risk detect [%s], phy_classify", bytes.c_str());
                    return false;
                }
            }
        }
    }
    AGG_LOG(ERROR, "Risk misNormal is true");
    misNormal = true;
    return true;
}
AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::RiskResult);

