#ifndef AGG_WEB_HELPER_MEDICAL_CHAT_HELPER_H
#define AGG_WEB_HELPER_MEDICAL_CHAT_HELPER_H

#include <agg_web2/common/common.h>
#include <agg2/wrapper/agg_cache_wrapper.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class MedicalChatHelper
{
public:
    MedicalChatHelper(){};
    ~MedicalChatHelper(){};

public:
    static std::string ChatResponseToJson(const ChatResponse & response,bool needDebug = false);

public:
    static bool SendMessageToClient(AggContext* aggContext);
    static bool IsSSERequest(AggContext* aggContext);
    static bool SendEventData(AggContext* aggContext, const std::string &data, const std::string &event = "");
    static void SendErrorResponsePacket(AggContext* aggContext,uint32_t code, const std::string& body, 
                                  const String2String& headers);
    static bool AddMedicalHistory(AggContext* aggContext, const std::string &query,const std::string &generate_text);
    static bool AddLLMHistory(AggContext* aggContext, const std::string &query,const std::string &generate_text);
    static bool UpdateLLMCache(AggContext* aggContext,AggCacheWrapperPtr wrapper);
private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);
#endif //A4_PROCESSOR_TEST_PROCESSOR_H
