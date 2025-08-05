#include <agg_web2/upstream/knowledge_grade/knowledge_grade_result.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, KnowledgeGradeResult);

BEGIN_DEFINE_SERIALIZATION_FUNC(KnowledgeGradeResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(KnowledgeGradeResult)


KnowledgeGradeResult::KnowledgeGradeResult()
{
}

KnowledgeGradeResult::~KnowledgeGradeResult() 
{ 
}

bool KnowledgeGradeResult::Serialize(std::string & bytes)
{
    return true;
}

bool KnowledgeGradeResult::Deserialize(const std::string & bytes)
{
    AGG_LOG(ERROR, "[KG_DEBUG] KnowledgeGradeResult::Deserialize [%s]", bytes.c_str());

    // {"data":{"text_scene":{"label_names":["逻辑性:举例阐述","逻辑性:解释说明"],"text_scene_prob":[0.9064784139479148,0.987079411414588]}},"debug":{},"message":"succ","status":200}
    
    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(bytes, root)) {
        AGG_LOG(ERROR, "[KG_DEBUG] parse json string failed [%s]", bytes.c_str());
        return false;
    }
    if (!root.isObject()) {
        AGG_LOG(ERROR, "[KG_DEBUG] invalid json object [%s]", bytes.c_str());
        return false;
    }

    if (!root.isMember("data") || !root["data"].isObject()) {
        AGG_LOG(ERROR, "[KG_DEBUG] invalid response data, error key=`data`");
        return false;
    }

    const Json::Value &tmpDataValue = root["data"];
    if (!tmpDataValue.isMember("text_scene") || !tmpDataValue["text_scene"].isObject())
    {
        AGG_LOG(ERROR, "[KG_DEBUG] invalid response data, error key=`text_scene`");
        return false;
    }

    const Json::Value &dataValue = tmpDataValue["text_scene"];
    if (
        !dataValue.isObject() 
        || !dataValue.isMember("label_names") || !dataValue["label_names"].isArray()
        || !dataValue.isMember("text_scene_prob") || !dataValue["text_scene_prob"].isArray()
        ) 
    {
        AGG_LOG(ERROR, "[KG_DEBUG] invalid text_scene data");
        return false;
    }

    const Json::Value &tmpLabels = dataValue["label_names"]; 
    const Json::Value &tmpScores = dataValue["text_scene_prob"]; 
    for (uint32_t i = 0; i < tmpLabels.size(); ++i) {
        if (tmpLabels[i].isString() && tmpScores[i].isDouble()) {
            mLabels.push_back(tmpLabels[i].asString());
            mScores.push_back(tmpScores[i].asDouble());
        } else {
            AGG_LOG(ERROR, "[KG_DEBUG] invalid label_names or text_scene_prob, got label_names='%s', text_scene_prob='%f'", tmpLabels[i].asString().c_str(), tmpScores[i].asDouble());
            return false;
        }
    }

    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::KnowledgeGradeResult);

