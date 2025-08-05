#include <agg_web2/common/llm/pgllm_result.h>
#include <messer_utils/util/ostringstream.h>
#include <A4/common/url_encoder.h>
#include <agg2/common/rapidjson.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(common);
AGG_LOG_SETUP(common, PgLLMResult);

bool PgLLMResult::Serialize(std::string &bytes) {
    AGG_LOG(ERROR, "PgLLMResult Serialize implementation");
    return true;
}

bool PgLLMResult::Deserialize(const std::string &bytes) {
    Json::Reader reader;
    Json::Value root;
    AGG_LOG(ERROR,"pgLlmresult:%s",bytes.c_str());
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
        if(dataValue.isMember("finish_reason") && dataValue["finish_reason"].isString()){           
            mFinishReason = dataValue["finish_reason"].asString();           
        }
        if(dataValue.isMember("message") && dataValue["message"].isString()){           
            mContent = dataValue["message"].asString();           
        }
        if(dataValue.isMember("prompt") && dataValue["prompt"].isString()){           
            mPrompt = dataValue["prompt"].asString();           
        }
        if(dataValue.isMember("request_id") && dataValue["request_id"].isString()){
           mRequestId = dataValue["request_id"].asString();
        }
    }

    return true;
}


AGG_END_NAMESPACE(common);
