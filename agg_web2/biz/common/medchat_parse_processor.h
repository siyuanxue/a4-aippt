#ifndef AGG_WEB2_BIZ_COMMON_MEDCHAT_PARSE_PROCESSOR_H
#define AGG_WEB2_BIZ_COMMON_MEDCHAT_PARSE_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>

AGG_BEGIN_NAMESPACE(biz);

class MedChatParseProcessor : public BaseProcessor
{
public:
    MedChatParseProcessor();
    virtual ~MedChatParseProcessor();

public:
    virtual bool DoInit(const String2String& params) override;
    
    /**
     * RequestParseProcessor中生成AggContext等上下文信息
     * 需要重载基类中的Process成员函数
     */
    virtual int Process(Context *context) override final;
    
    virtual BaseProcessor* DoClone() override;
    
    virtual void DoDestroy() override;

public:
    /**
     * @func: 自定义Metric指标注册
     */
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    
    /**
     * RequestParseProcessor的子类可重载DoProcess成员函数
     * 用于实现子类自定义的处理逻辑
     */
    virtual int DoProcess(AggContext * aggContext) override;

protected:
    bool Parse(Context* context, String2String& params);
    bool GetBucketGroup(Context* context, std::string& bucketGroupStr);
    bool ParseJson(std::string &msg, std::map<std::string, std::string> &params);
    void InitResponse(AggContext *aggContext);
private:
    bool mMustQuery;
    bool mBtsGroupSwitch;
    std::map<std::string,std::string> mExtraParams;
private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(biz);

#endif //AGG_WEB2_BIZ_COMMON_MEDCHAT_PARSE_PROCESSOR_H
