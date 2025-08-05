#include <agg_web2/upstream/knowledge/knowledge_result.h>
#include <jsoncpp/json/json.h>

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

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, KnowledgeResult);

BEGIN_DEFINE_SERIALIZATION_FUNC(KnowledgeResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(KnowledgeResult)


KnowledgeResult::KnowledgeResult()
{
}

KnowledgeResult::~KnowledgeResult() 
{ 

}

bool KnowledgeResult::Serialize(std::string & bytes)
{

    return true;
}

void KnowledgeResult::BodyClean(std::string& RawBody)
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

bool KnowledgeResult::Deserialize(const std::string & bytes)
{
    AGG_LOG(ERROR, "KnowledgeResult::Deserialize [%s]", bytes.c_str());
    
    mBody = bytes;
    mTriggerLevel = -1;

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

    
    if (root.isMember("trigger_level") && root["trigger_level"].isInt()) {
        mTriggerLevel = root["trigger_level"].asInt();
    }

    if (root.isMember("result") && root["result"].isArray()) {
        const Json::Value &resultValue = root["result"]; 
        for (uint32_t i = 0; i < resultValue.size(); ++i) {
            const Json::Value &doc = resultValue[i];
            if (!doc.isObject())
                continue;
            
            KnowledgeDoc onedoc;
            if (doc.isMember("doc_trigger_level") && doc["doc_trigger_level"].isString()) {
                onedoc.mTriggerLevel = autil::StringUtil::strToInt32WithDefault(doc["doc_trigger_level"].asString().c_str(), -1);
            }
            if (doc.isMember("doc_trigger_level") && doc["doc_trigger_level"].isInt()) {
                onedoc.mTriggerLevel = doc["doc_trigger_level"].asInt();
            }

            if (doc.isMember("body") && doc["body"].isString()) {
                onedoc.mBody = doc["body"].asString(); 
            }
            
            if (doc.isMember("title") && doc["title"].isString()) {
                std::string cluster;
                if (doc.isMember("cluster") && doc["cluster"].isString()){
                    cluster = doc["cluster"].asString();
                }
                std::string prefix = "";
                if (cluster == "book"){
                    prefix = "以下结果来自书籍：\n";
                } else if(cluster == "law_provision"){
                    prefix = "以下结果来自法条：\n";
                }
                onedoc.mTitle = prefix + doc["title"].asString();
            }
            
            if (doc.isMember("data_type") && doc["data_type"].isString()) {
                onedoc.mDataType = doc["data_type"].asString();
            }

            if (doc.isMember("normalized_url") && doc["normalized_url"].isString()) {
                onedoc.mUrl = doc["normalized_url"].asString();
            }

            if (doc.isMember("QtcTeacherQtc") && doc["QtcTeacherQtc"].isString()) {
                onedoc.mQtc = autil::StringUtil::strToDoubleWithDefault(doc["QtcTeacherQtc"].asString().c_str(), 0.0);
            }
            
            if (doc.isMember("PrScore") && doc["PrScore"].isString()) {
                onedoc.mPr = autil::StringUtil::strToDoubleWithDefault(doc["PrScore"].asString().c_str(), 0.0);
            }
            
            if (doc.isMember("published_time") && doc["published_time"].isString()) {
                onedoc.mPublishTime = doc["published_time"].asString();
            }
        
            BodyClean(onedoc.mBody);

            AGG_LOG(ERROR, "parse json[%s][%s][%s]", onedoc.mUrl.c_str(), onedoc.mDataType.c_str(), onedoc.mTitle.c_str());
            mDocs.push_back(onedoc);
        }
    }
    
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::KnowledgeResult);

