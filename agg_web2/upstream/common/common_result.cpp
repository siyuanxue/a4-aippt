#include <agg_web2/upstream/common/common_result.h>
#include <messer_utils/util/ostringstream.h>
#include <autil/TimeUtility.h>
#include <autil/StringUtil.h>
#include <algorithm>
#include <agg2/util/string_tool.h>
#include <iomanip>
#include <cctype>
#include <agg_web2/data/sc_template_data.h>
#include <agg2/wrapper/agg_data_wrapper.h>
#include <agg2/helper/time_helper.h>
#include <agg_web2/helper/general_search_helper.h>
#include <agg_web2/data/key_value_data.h>
#include <agg_web2/data/value_data.h>
#include <re2/re2.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, CommonResult);

using namespace std;

BEGIN_DEFINE_SERIALIZATION_FUNC(CommonResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(CommonResult)

CommonResult::CommonResult()
{
}

CommonResult::CommonResult(const std::string& type)
{
    mType = type;
}

CommonResult::~CommonResult()
{

}

bool CommonResult::Serialize(std::string & bytes)
{
    return true;
}

std::string CommonResult::GetFormatTime(int Currenttime)
{
    if (Currenttime < 1600000000) {
        return std::string("");
    }

    char outStr[256];
    time_t t = Currenttime;
    struct tm *tmp = localtime(&t);
    strftime(outStr, sizeof(outStr), "%Y-%m-%d %H:%M:%S", tmp);
    return std::string(outStr);
}

std::string CommonResult::GetFormatTime(const std::string& CurrenttimeStr)
{
    int Currenttime = 0;
    SAFE_TYPE_CHANGE(CurrenttimeStr, Currenttime);
    return GetFormatTime(Currenttime);
}

void CommonResult::BodyClean(std::string& RawBody)
{
    AggDataManager * aggDataManager = AggDataManager::GetInstance(); // 正则过滤
    AggDataWrapper aggDataWrapper(aggDataManager, "source_bodyclean_data");
    const auto& kvData = aggDataWrapper.Get<KeyValueData>()->GetkvData();

    MESSER_UTILS_NS(util)::ostringstream oss;
    std::vector<std::string> lines = autil::StringUtil::split(RawBody, "\n");
    for (auto& line: lines) {
        line = StringTool::ReplaceStr(line, "\u200b", " ");
        GeneralSearchHelper::LStripStrInPlace(line, {" ", "\t"});
        GeneralSearchHelper::RStripStrInPlace(line, {" ", "\t"});
        int choose = 1;
        AGG_LOG(DEBUG, "BodyClean line=%s", line.c_str());
        for(const auto& kv:kvData) {
            const std::string& pattern = kv.first;
            if (RE2::FullMatch(line, pattern)) {
                if(kv.second=="break") {
                    choose = -1;
                } else if(kv.second=="continue") {
                    choose = 0;
                }
                break;
            }
        }
        if (choose == -1) {
            break;
        }
        if (choose == 1) {
            oss << line << "\n";
        }
    }
    RawBody = oss.str();

    // 截断过长body
    int cutLen = 800 * 3;
    std::string::size_type pos = RawBody.find('\n', cutLen);
    if(pos != std::string::npos) {
        RawBody = RawBody.substr(0, pos+1);
    }
}

void CommonResult::RemoveEM(std::string& content)
{
    content = StringTool::ReplaceStr(content, "<em>", "");
    content = StringTool::ReplaceStr(content, "</em>", "");
    content = StringTool::ReplaceStr(content, "\t", "");
}

std::string CommonResult::GetTextFromXML(TiXmlElement * node)
{
    if (node && node->GetText()) {
        return node->GetText();
    }
    return std::string("");
}

bool CommonResult::GetPathValue(TiXmlElement *element, const std::vector<std::string> & path, std::string& res)
{
    for (const auto& key: path) {
        TiXmlElement * ele;
        CHECK_FINDFIRSTNODE_RET(element, key, ele);
        if (ele == NULL) {
            AGG_LOG(ERROR, "GetCommonResult error, key=%s", key.c_str());
            return false;
        }
        element = ele;
    }
    res = GetTextFromXML(element);
    return ! res.empty();
}

bool CommonResult::Deserialize(const std::string & bytes)
{
    // AGG_LOG(ERROR, "%s CommonResult::Deserialize [%s]", mType.c_str(), bytes.c_str());

    mBody = bytes;
    mTriggerLevel = -1;

    if (mType == "news") {
        ParseNews();
    }

    if (mType == "gaokao_intent") {
        ParseGaokaoIntent();
    }

    if (mType == "llm_emb") {
        // {"success": true, "data": {"result": [0.04521, 0.07337,
        mScores.clear();
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(mBody, root) && root.isObject() && root.isMember("data") 
                && root["data"].isObject() && root["data"].isMember("result") && root["data"]["result"].isArray()) {
            Json::Value &value = root["data"]["result"];
            for (auto one : value) {
                if (one.isDouble()) {
                    mScores.push_back(one.asDouble());
                }
            }
        }
    }

    if (mType == "llm_emb_l3") {
        mScores.clear();
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(mBody, root) && root.isObject() && root.isMember("data") && root["data"].isObject()) {
            const Json::Value &data = root["data"];
            if (data.isMember("choices") && data["choices"].isArray() && data["choices"].size() > 0) {
                const Json::Value & choice = data["choices"][0];
                if (choice.isMember("raw_outputs") && choice["raw_outputs"].isArray() && choice["raw_outputs"].size() >= 1024 ) {
                    const Json::Value &value = choice["raw_outputs"];
                    for (uint32_t i = 0; i < 1024; ++i) {
                        if (value[i].isDouble()) {
                            mScores.push_back(value[i].asDouble());
                        }
                    }
                }
            }
        }
    }

    if (mType == "sgs_pr") {
        mScore = 0.0;
        Json::Reader reader;
        Json::Value root;
        // {"pr":"7.15255856562317e-05","prob":"0.999950463605473,2.75472033966446e-05,2.19891911297936e-05"}
        if (reader.parse(mBody, root) && root.isObject() && root.isMember("pr") && root["pr"].isString()) {
            string pr = root["pr"].asString();
            mScore = autil::StringUtil::strToDoubleWithDefault(pr.c_str(), 0.0);
        }
    }

    if (mType == "llm_pr") {
        vector<double> logits;
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(mBody, root) && root.isObject() && root.isMember("data") && root["data"].isObject()) {
            const Json::Value &data = root["data"];
            if (data.isMember("choices") && data["choices"].isArray() && data["choices"].size() > 0) {
                const Json::Value & choice = data["choices"][0];
                if (choice.isMember("raw_outputs") && choice["raw_outputs"].isArray() && choice["raw_outputs"].size() >= 5 ) {
                    const Json::Value &value = choice["raw_outputs"];
                    for (uint32_t i = 0; i < value.size(); ++i) {
                        if (value[i].isDouble()) {
                            logits.push_back(value[i].asDouble());
                        }
                    }
                }
            }
        }
        if (logits.size() >= 5) {
            double total = 0.0;
            for (auto logit: logits) {
                total += exp(logit);
            }
            if (total < 0.00001) {
                total = 0.00001;
            }
            double prob_0 = exp(logits[0]) / total;
            double prob_1 = exp(logits[1]) / total;
            double prob_2 = exp(logits[2]) / total;
            double prob_3 = exp(logits[3]) / total;
            double prob_4 = exp(logits[4]) / total;
            // mScore = prob_0 * 0 + prob_1 * 1 + prob_2 * 2;
            mScore = prob_1 + 2*prob_2+1.5*prob_4+0.5*prob_3;
        }
    }

    if (mType == "llm_minicpm") {
        // {"success": true, "data": {"result": "[{\"query\": \"复旦大学的占地面积有多少\", \"passage\": \"复旦大学_百度百科\\n不清楚复旦大学的面积有多少，可以去官网看看\", \"prob\": 0.030860427767038345}]"}, "metrics": {"arrive_time_ms": 1722309317470, "total_ms": 102, "process_ms": 102}}
        
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(mBody, root) && root.isObject() && root.isMember("data") && root["data"].isObject()
                && root["data"].isMember("result") && root["data"]["result"].isString()) {
            string result = root["data"]["result"].asString();
            Json::Reader reader2;
            Json::Value root2;
            if (reader2.parse(result, root2) && root2.isArray() && root2.size() > 0) {
                if (root2[0].isObject() && root2[0].isMember("prob") && root2[0]["prob"].isDouble()) {
                    mScore = root2[0]["prob"].asDouble();
                }
            }
        }
    }
    return true;
}

bool CommonResult::ParseNews()
{
    TiXmlDocument doc;
    TiXmlElement *rootElement = NULL;
    doc.Parse(mBody.c_str(), 0);
    if (doc.Error() || (rootElement = doc.RootElement()) == NULL) {
        AGG_LOG(ERROR, "GetNewsResult root error");
        return false;
    }

    TiXmlElement * engineElement;
    CHECK_FINDFIRSTNODE_RET(rootElement, "engine", engineElement);
    if (engineElement == NULL) {
        AGG_LOG(ERROR, "GetNewsResult engine error");
        return false;
    }

    TiXmlElement * hitsElement;
    CHECK_FINDFIRSTNODE_RET(engineElement, "hits", hitsElement);
    if (hitsElement == NULL) {
        AGG_LOG(ERROR, "GetNewsResult hits error");
        return false;
    }

    size_t idx;
    TiXmlElement * hitElement;
    for (idx=0, hitElement = hitsElement->FirstChildElement("hit");
            hitElement != NULL; idx+=1 , hitElement = hitElement->NextSiblingElement("hit")) {
        AGG_LOG(DEBUG, "news hit[%s]", TiXmlHelper::Print(hitElement).c_str());
        CommonDoc oneDoc;

        if (GetPathValue(hitElement, {"fields", "NormalizedUrl"}, oneDoc.mUrl)
                && GetPathValue(hitElement, {"fields", "title"}, oneDoc.mTitle)
                && GetPathValue(hitElement, {"fields", "ORI_MAIN_BODY"}, oneDoc.mBody))
        {
            RemoveEM(oneDoc.mTitle);
            RemoveEM(oneDoc.mBody);
            BodyClean(oneDoc.mBody);

            std::string publishTime;
            GetPathValue(hitElement, {"fields", "publish_time"}, publishTime);
            oneDoc.mPublishTime = GetFormatTime(publishTime);

            string qtcStr;
            GetPathValue(hitElement, {"fields", "qtc_model_score"}, qtcStr);
            double qtc = 0;
            SAFE_TYPE_CHANGE(qtcStr, qtc);
            oneDoc.mQtc = qtc;

            oneDoc.mPr = oneDoc.mQtc / 2;

            AGG_LOG(ERROR, "news hit with title=%s, qtc=%f, pr=%f, url=%s, time=%s, body=%s", oneDoc.mTitle.c_str(), oneDoc.mQtc, oneDoc.mPr, oneDoc.mUrl.c_str(), oneDoc.mPublishTime.c_str(), oneDoc.mBody.c_str());
            mDocs.push_back(oneDoc);
        }
    }
    return true;
}

bool CommonResult::ParseGaokaoIntent()
{
    //{"data":{"gaokao":{"gaokao_primary":["问信息-院校"],"gaokao_primary_prob":[0.9634103661096334],"gaokao_second":["否"],"gaokao_second_prob":[0.9836978255133652],"gaokao_third":["是"],"gaokao_third_prob":[0.9626998878905192]}},"debug":{},"message":"succ","status":200}
    mIntents.clear(); 
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(mBody, root)) {
        AGG_LOG(ERROR, "parse json string failed [%s]", mBody.c_str());
        return false;
    }
    if (!root.isObject()) {
        AGG_LOG(ERROR, "invalid json object [%s]", mBody.c_str());
        return false;
    }

    if (!root.isMember("data") || !root["data"].isObject()) {
        return false;
    }
    const Json::Value &dataValue = root["data"];
    if (!dataValue.isMember("gaokao") || !dataValue["gaokao"].isObject()) {
        return false;
    }
    const Json::Value &gaokao = dataValue["gaokao"];
    if (!gaokao.isMember("gaokao_primary") || !gaokao["gaokao_primary"].isArray()) {
        return false;
    }
    
    const Json::Value &resultValue = gaokao["gaokao_primary"]; 
    for (uint32_t i = 0; i < resultValue.size(); ++i) {
        if (resultValue[i].isString()) {
            mIntents.push_back(resultValue[i].asString());
        }
    }

    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::CommonResult);

