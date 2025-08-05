#ifndef AGG_WEB2_COMMON_LLM_QA_LLM_REQUEST_H
#define AGG_WEB2_COMMON_LLM_QA_LLM_REQUEST_H

#include <agg2/common/agg_common.h>
#include <agg2/common/http_request.h>
#include <agg2/common/agg_context.h>

AGG_BEGIN_NAMESPACE(agg_web2);


class QaLLMRequest : public HttpRequest {
    DECLARE_SERIALIZATION_FUNC();

public:
    QaLLMRequest();
    QaLLMRequest(AggContext *aggContext,const std::string &model);
    virtual ~QaLLMRequest();
    
public:
    virtual bool DoEncode(std::string &uriQuery, std::string &body,
                  String2String &header) override;
public:
    const std::string& GetModel() const { return mModel; }
    void SetModel(const std::string& model) { mModel = model;}
    void SetRemoveHistory() {
        mNeedHistory = false;
    }
    int GetMaxNewTokens() const { return mMaxNewTokens; }
    void SetMaxNewTokens(int maxTokens) { mMaxNewTokens = maxTokens;}
    const std::string& GetRequestId() const { return mRequestId; }
    void SetRequestId(const std::string& request_id) { mRequestId = request_id;} 
    const std::string& GetContent() const { return mContent; }
    void SetContent(const std::string& content) { mContent = content;} 
    const std::string& GetRole() const { return mRole; }
    void SetRole(const std::string& role) { mRole = role;}  
    const std::string& GetProtoFormat() const { return mProtoFormat; }
    void SetProtoFormat(const std::string& format) { mProtoFormat = format;}
    const std::string& GetPrompt() const { return mPrompt; }
    void SetPrompt(const std::string& prompt) { mPrompt = prompt;}
    const std::string& GetApiKey() const { return mApiKey; }
    void SetApiKey(const std::string& key) { mApiKey = key;}
    float GetTemperature() const { return mTemperature; }
    void SetTemperature(float temperature) { mTemperature = temperature;}

public:
    void SetPaddingHistory() {
        mNeedHistory = true;
    }
protected:
    AggContext * mAggContext;
    std::string mModel;
    std::string mProtoFormat;
    std::string mPrompt;
    std::string mContent;
    std::string mRole;
    std::string mRequestId;
    std::string mApiKey;
    //extra_args
    int mMaxNewTokens;
    float mTemperature;
    bool mNeedHistory = true;
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(QaLLMRequest);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(agg_web2)::QaLLMRequest);

#endif

