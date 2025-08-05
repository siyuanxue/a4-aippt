#include <agg_web2/upstream/qt/qt_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, QtResult);

BEGIN_DEFINE_SERIALIZATION_FUNC(QtResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(QtResult)


QtResult::QtResult()
{
}

QtResult::~QtResult()
{

}

bool QtResult::Serialize(std::string & bytes)
{

    return true;
}

bool QtResult::Deserialize(const std::string & bytes)
{
    AGG_LOG(DEBUG, "QtResult::Deserialize [%s]", bytes.c_str());

    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(bytes, root)) {
        AGG_LOG(ERROR, "parse json string failed [%s]", bytes.c_str());
        return false;
    }
    if (!root.isObject()) {
        AGG_LOG(ERROR, "invalid json object [%s]", bytes.c_str());
        return false;
    }
    if (root.isMember("code") && root["code"].isInt()) {
        int code = root["code"].asInt();
        if (code != 0) {
            AGG_LOG(ERROR, "invalid json object [%s], code=%d", bytes.c_str(), code);
            return false;
        }
    } else {
        return false;
    }
    if (! root.isMember("data") || ! root["data"].isObject()) {
        AGG_LOG(ERROR, "invalid json object [%s], data", bytes.c_str());
        return false;
    }
    const Json::Value &dataValue = root["data"];
    if (! dataValue.isMember("result") || ! dataValue["result"].isArray()) {
        AGG_LOG(ERROR, "invalid json object [%s], result", bytes.c_str());
        return false;
    }
    const Json::Value &resultValue = dataValue["result"];
    for (size_t i = 0; i < resultValue.size(); ++i) {
        const Json::Value& doc = resultValue[int(i)];
        if (! doc.isMember("result") || ! doc["result"].isObject()) {
            AGG_LOG(ERROR, "invalid json object [%s], result", bytes.c_str());
            return false;
        }
        const Json::Value &result = doc["result"];
        if (! result.isMember("qt_12l_ipu_v2") || ! result["qt_12l_ipu_v2"].isObject()) {
            AGG_LOG(ERROR, "invalid json object [%s], qt_12l_ipu_v2", bytes.c_str());
            return false;
        }
        const Json::Value & qt_12l_ipu_v2 = result["qt_12l_ipu_v2"];
        if (qt_12l_ipu_v2.isMember("qt_12l_ipu_score") && qt_12l_ipu_v2["qt_12l_ipu_score"].isArray()
            && qt_12l_ipu_v2["qt_12l_ipu_score"].size() == 4
            && qt_12l_ipu_v2["qt_12l_ipu_score"][3].isDouble()) {
            mQtScores.push_back(qt_12l_ipu_v2["qt_12l_ipu_score"][3].asDouble());
        } else {
            AGG_LOG(ERROR, "invalid json object [%s], qt_12l_ipu_v2", bytes.c_str());
            return false;
        }
    }
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::QtResult);

