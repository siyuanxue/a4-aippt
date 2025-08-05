#include <agg_web2/common/llm/qa_llm_result.h>
#include <messer_utils/util/ostringstream.h>
#include <A4/common/url_encoder.h>
#include <agg2/common/rapidjson.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(common);
AGG_LOG_SETUP(agg_web2, QaLLMResult);

bool QaLLMResult::Serialize(std::string &bytes) {
    AGG_LOG(ERROR, "QaLLMResult Serialize implementation");
    return true;
}

bool QaLLMResult::Deserialize(const std::string &bytes) {
    Json::Reader reader;
    Json::Value root;
    
    AGG_LOG(INFO, "llm result:[%s]", bytes.c_str());
    if(!reader.parse(bytes, root)) {
        AGG_LOG(ERROR, "parse json string failed [%s]", bytes.c_str());
        return false;
    }
    if(!root.isObject()) {
        AGG_LOG(ERROR, "invalid json object [%s]", bytes.c_str());
        return false;
    }
   if(root.isMember("code") && root["code"].isInt()){
       mCode = root["code"].asInt();
   }
   if(root.isMember("message") && root["message"].isString()){
       mMessage = root["message"].asString();
   }
   if(root.isMember("data") && root["data"].isObject()){
       const Json::Value &dataValue = root["data"];
       if(dataValue.isMember("choices") && dataValue["choices"].isArray()){
           const Json::Value& choicesArr = dataValue["choices"];
           if(choicesArr.size() >= 1 && choicesArr[0].isObject()){
               const Json::Value choicesData = choicesArr[0];
               if(choicesData.isMember("finish_reason") and choicesData["finish_reason"].isString()){
                   mFinishReason = choicesData["finish_reason"].asString();
               }
               if(choicesData.isMember("message") and choicesData["message"].isObject()){
                   const Json::Value messageObj = choicesData["message"];
                   if(messageObj.isMember("content") and messageObj["content"].isString()){
                       mContent = messageObj["content"].asString();
                   }
               }
           }
       }
       if(dataValue.isMember("debug") && dataValue["debug"].isObject()){
           const Json::Value debugValue = dataValue["debug"];
           if(debugValue.isMember("prompt") and debugValue["prompt"].isString()){
                mPrompt = debugValue["prompt"].asString();
           }
       }
       if(dataValue.isMember("request_id") && dataValue["request_id"].isString()){
           mRequestId = dataValue["request_id"].asString();
       }
   }

   return true;
}


AGG_END_NAMESPACE(common);
