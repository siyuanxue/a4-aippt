#include <agg_web2/upstream/ae/ae_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, AeResult);

BEGIN_DEFINE_SERIALIZATION_FUNC(AeResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(AeResult)


AeResult::AeResult()
{
}

AeResult::~AeResult() 
{ 
}

bool AeResult::Serialize(std::string & bytes)
{

    return true;
}

bool AeResult::Deserialize(const std::string & bytes)
{
    AGG_LOG(ERROR, "AeResult::Deserialize [%s]", bytes.c_str());
    
    mScore = -1.0;
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

    if (!root.isMember("data") || !root["data"].isObject()) {
        return false;
    }

    const Json::Value &dataValue = root["data"];

    if (!dataValue.isObject() || 
            !dataValue.isMember("final_answer") || !dataValue["final_answer"].isString() ||
            !dataValue.isMember("answer") || !dataValue["answer"].isString() ||
            !dataValue.isMember("prscore") || !dataValue["prscore"].isDouble()) {
        return false;
    }
    mAnswer = dataValue["final_answer"].asString();
    mOriAnswer = dataValue["answer"].asString();
    mScore = dataValue["prscore"].asDouble();

    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::AeResult);

