#include <algorithm>
#include <agg_web2/helper/medical_chat_helper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg_web2/common/agg_web_user_context.h>
#include <A4/util/string_tokenizer.h>
#include <A4/util/string_util.h>
#include <jsoncpp/json/json.h>
#include <messer_utils/util/ostringstream.h>
#include <agg2/upstream/llm_cache.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, MedicalChatHelper);
using namespace std;

std::string MedicalChatHelper::ChatResponseToJson(const ChatResponse & response,bool needDebug)
{
    AGG_LOG(ERROR, "medical chat test ...");

    Json::Value root;
    root["version"] = response.version;
    root["target"] = response.target;

    root["error_code"] = response.error_code;
    root["error_msg"] = response.error_msg;
    root["timestamp"] = response.timestamp;

    Json::Value communication;
    communication["uid"] = response.communication.uid;
    communication["sessionid"] = response.communication.sessionid;
    communication["reqid"] = response.communication.reqid;
    communication["resid"] = response.communication.resid;
    root["communication"] = communication;

    Json::Value data;
    data["message"]["is_med"] = response.data.isMed;
    data["message"]["query"] = response.data.query;
    data["message"]["query_type"] = response.data.query_type;
    data["message"]["query_prompt"] = response.data.query_prompt;
    
    Json::Value query_extend;
    for (const std::string& q : response.data.query_extend) {
        query_extend.append(q);
    } 
    data["message"]["query_extend"] = query_extend;

    Json::Value history_query;
    for (const std::string& q : response.data.history_query) {
        history_query.append(q);
    } 
    data["message"]["history_query"] = history_query;


    data["message"]["general_content"] = response.data.general_content;
    if (! response.data.general_content_cache.empty()) {
        data["message"]["general_content_cache"] = response.data.general_content_cache;
    }
    data["message"]["general_status"] = response.data.general_status;
    data["message"]["medical_question"] = response.data.medical_question;
    data["message"]["general_content_evdoff"] = response.data.general_content_evdoff;

    // // 临时使用string
    // data["message"]["medical_evidence"] = response.data.medical_evidence;
    try
    {
        Json::Reader reader;
        Json::Value root;

        // int returnCount = -1;
        if (reader.parse(response.data.medical_evidence, root))
        {
            data["message"]["medical_evidence"] = root;
        }
    }
    catch(...)
    {
        AGG_LOG(ERROR, "Init Evidence:[%s] Failed", response.data.medical_evidence.c_str());
    }

    Json::Value medical_origin_links;
    for (const auto& link : response.data.medical_origin_links) {
        Json::Value item;
        for (auto it = link.begin(); it != link.end(); ++it) {
            item[it->first] = it->second;
        }
        medical_origin_links.append(item);
    }
    data["message"]["medical_origin_links"] = medical_origin_links;

    if(needDebug) {
        Json::Value search_doc = Json::Value(Json::arrayValue);
        for (const auto& link : response.debug_data.medical_search_content) {
            Json::Value item;
            for (auto it = link.begin(); it != link.end(); ++it) {
                item[it->first] = it->second;
            }
            search_doc.append(item);
        }
        data["debug"]["search_doc"] = search_doc;
        data["debug"]["query_prompt"] = response.debug_data.query_prompt;
        data["debug"]["recommend_prompt"] = response.debug_data.recommend_prompt;
        data["debug"]["summary_prompt"] = response.debug_data.summary_prompt;

        for (const auto& kv: response.debug_data.debug_kvs) {
            data["debug"][kv.first] = kv.second;
        }
        data["debug"]["rewrite_prompt"] = response.debug_data.rewrite_prompt;
        data["debug"]["rewrite_response"] = response.debug_data.rewrite_response;
    }

    data["message"]["status"] = response.data.status;
    data["message"]["progress"] = response.data.progress;
    root["data"] = data;

    Json::FastWriter writer;
    std::string output = writer.write(root);
    return output;
}


bool MedicalChatHelper::SendMessageToClient(AggContext* aggContext)
{
    AGG_LOG(INFO, "response SendMessageToClient ");
    std::string debug = "";
    aggContext->GetRequestParam("debug", debug);
    bool debugFlag = ! debug.empty();
    AggUserContext* useContext = aggContext->GetAggUserContext();
    AggWebUserContext* aggWebUserContext =
            dynamic_cast<AggWebUserContext*>(useContext);
    ChatResponsePtr chatResponsePtr = aggWebUserContext->GetChatResponse();
    string msgData = ChatResponseToJson(*chatResponsePtr,debugFlag);

    string debugKey;
    if(aggContext->GetRequestParam("nowsk",debugKey)){
        AGG_LOG(INFO, "debug response Info:[%s] ",msgData.c_str());
        return true;
    }
    if (IsSSERequest(aggContext)) {
        std::string event;
        if (chatResponsePtr->data.status == "complete") {
            event = "complete";
        }
        return SendEventData(aggContext,msgData,event);
    }
    return false;
}

void MedicalChatHelper::SendErrorResponsePacket(AggContext* aggContext,uint32_t code, const std::string& body, 
                                  const String2String& headers)
{
    if (IsSSERequest(aggContext)) {
        aggContext->GetContext()->SetSessionStatus(SS_INTERNAL_ERROR);
        ChatResponsePtr responsePtr = AggWebContextHelper::GetChatResponse(aggContext);
        if (responsePtr) {
            responsePtr->data.status = "complete";
            responsePtr->error_code = code;
            responsePtr->error_msg = body;
            std::string result = MedicalChatHelper::ChatResponseToJson(*responsePtr,true);
            SendEventData(aggContext,result,"complete");
            UserConnection* connect = aggContext->GetContext()->GetConnection();
            if (connect && !connect->IsClosed()) {
                connect->Close();
            }            
            AGG_LOG(ERROR, "send_error_response,code:%d,msg:%s",code,body.c_str());
            return;
        }
        else
        {
            UserConnection* connect = aggContext->GetContext()->GetConnection();
            if (connect && !connect->IsClosed()) {
                connect->Close();
            }
            AGG_LOG(ERROR, "connect close because error,code:%d,msg:%s",code,body.c_str());
            return;
        }
    }  
    aggContext->GetContext()->SetResponsePacket(HttpPacketFactory::CreateResponse(code, body, headers));
    return;
}

bool MedicalChatHelper::SendEventData(AggContext* aggContext, const std::string &data, const std::string &event) {
    bool ret = false;
    std::string realData;
    AGG_LOG(INFO, "SendEvent data:[%s] event:[%s]", data.c_str(), event.c_str());
    if (!data.empty() && data[data.size()-1] == '\n') {
        //rapidjson bug,it will append a n last
        AGG_LOG(ERROR, "last data is n,remove...");
        ret = AggContextHelper::SendEventData(aggContext,data.substr(0,data.size()-1),event);
    }
    else
    {
        ret = AggContextHelper::SendEventData(aggContext,data,event);
    }

    return ret;
}

bool MedicalChatHelper::IsSSERequest(AggContext* aggContext) {
    HttpPacket* requestPacket = aggContext->GetContext()->GetRequestPacket();
    if (requestPacket == NULL) {
        AGG_LOG(ERROR,"request packet null");
        return false;
    }
    std::string value;
    if (requestPacket->GetHeader("Accept", value)) {
        if (value.find("event-stream") != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool MedicalChatHelper::AddMedicalHistory(AggContext* aggContext, const std::string &query,const std::string &generate_text) {
    LLMCachePtr llmCachePtr = aggContext->GetValue<LLMCache>("llm_cache");
    if (llmCachePtr) {
        LLMHistoryVec historyVec;
        LLMHistoryPtr humanHistory(new LLMHistory());
        LLMHistoryPtr assistHistory(new LLMHistory());
        humanHistory->role = "Human";
        humanHistory->content = query;
        assistHistory->role = "Assistant";
        assistHistory->content = generate_text;
        historyVec.push_back(humanHistory);
        historyVec.push_back(assistHistory);
        llmCachePtr->AddHistory(historyVec);  //must be pair
        return true;
    }
    else
    {
        AGG_LOG(ERROR,"can not find llmcache!");
        return false;
    }
}

bool MedicalChatHelper::AddLLMHistory(AggContext* aggContext, const std::string &query,const std::string &generate_text) {
    LLMCachePtr llmCachePtr = aggContext->GetValue<LLMCache>("llm_cache");
    if (llmCachePtr) {
        LLMHistoryVec historyVec;
        LLMHistoryPtr userHistory(new LLMHistory());
        LLMHistoryPtr assistHistory(new LLMHistory());
        userHistory->role = "user";
        userHistory->content = query;
        assistHistory->role = "assistant";
        assistHistory->content = generate_text;
        historyVec.push_back(userHistory);
        historyVec.push_back(assistHistory);
        llmCachePtr->AddHistory(historyVec);  //must be pair
        return true;
    }
    else
    {
        AGG_LOG(ERROR,"can not find llmcache!");
        return false;
    }
}

bool MedicalChatHelper::UpdateLLMCache(AggContext* aggContext,AggCacheWrapperPtr wrapper) {
    LLMCachePtr llmCachePtr = aggContext->GetValue<LLMCache>(LLM_CACHE_AGG_KEY);
    if (llmCachePtr && wrapper) {
        std::string cacheVal;
        llmCachePtr->Serialize(cacheVal);
        if (!cacheVal.empty()) {
            std::string sessionid;
            aggContext->GetRequestParam("sessionid", sessionid);
            std::string cacheKey = LLM_CACHE_KEY_PREFIX+sessionid;
            uint32_t time = 43200;
            int status = 0;
            if (!wrapper->Set(cacheKey, cacheVal, time, status)) {
                AGG_LOG(ERROR, "Set sessionid[%s]  status[%d] Failed.", sessionid.c_str(), status);
                return false;
            }
        }
        return true;
    }
    return false;
}

AGG_END_NAMESPACE(agg_web2);


