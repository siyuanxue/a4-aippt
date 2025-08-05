#include <agg_web2/upstream/qdoc_correlation/qd_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, QdResult);

BEGIN_DEFINE_SERIALIZATION_FUNC(QdResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(QdResult)


QdResult::QdResult()
{
}

QdResult::~QdResult() 
{ 
}

bool QdResult::Serialize(std::string & bytes)
{

    return true;
}

bool QdResult::Deserialize(const std::string & bytes)
{
    AGG_LOG(INFO, "QdResult::Deserialize [%s]", bytes.c_str());

    mScore = -1.0;
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(bytes, root)) {
        AGG_LOG(ERROR, "QdResult::parse json string failed [%s]", bytes.c_str());
        return false;
    }
    if (!root.isObject()) {
        AGG_LOG(ERROR, "QdResult::invalid json object [%s]", bytes.c_str());
        return false;
    }

    if (!root.isMember("data") || !root["data"].isObject()) {
        AGG_LOG(ERROR, "QdResult::invalid json object [%s]", bytes.c_str());
        return false;
    }

    const Json::Value &dataValue = root["data"];

    if (!dataValue.isObject() ||
            !dataValue.isMember("prob") || !dataValue["prob"].isString()) {
        AGG_LOG(ERROR, "QdResult::invalid json object [%s]", bytes.c_str());
        return false;
    }
    mScore = autil::StringUtil::strToDoubleWithDefault(dataValue["prob"].asString().c_str(), mScore);

    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::QdResult);

