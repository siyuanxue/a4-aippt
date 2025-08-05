#ifndef AGG2_HELPER_AGG_WEB_CONTEXT_HELPER_H
#define AGG2_HELPER_AGG_WEB_CONTEXT_HELPER_H

#include <A4/common/define.h>
#include <A4/common/common.h>
#include <A4/util/thread_pool_manager.h>
#include <agg2/common/agg_log.h>
#include <agg2/common/define.h>
#include <agg2/common/agg_context.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg2/common/recommend/recommend_result.h>
#include <agg2/common/click/click_feedback_result.h>
#include <agg2/processor/base_processor.h>
#include "agg_web2/common/define.h"
#include "agg_web2/common/common.h"
#include <agg_web2/common/agg_web_user_context.h>


A4_USE_NAMESPACE(A4);
AGG_BEGIN_NAMESPACE(helper);

typedef std::map<uint64_t, std::string> QrsResultInfoMap;
AGG_TYPEDEF_PTR(QrsResultInfoMap);

class AggWebContextHelper : public AggContextHelper
{
public:
    static void SetChatResponse(AggContext * aggContext, ChatResponsePtr& responsePtr);
    static ChatResponsePtr GetChatResponse(AggContext * aggContext);
    //static void SetMidPageResult(AggContext * aggContext, MidPageResult& midPageResult);
    //static MidPageResult *GetMidPageResultPtr(AggContext * aggContext);
    static void SetMedChatFilterTag(AggContext * aggContext, std::string filterTag);
    static std::string GetMedChatFilterTag(AggContext * aggContext);
    static void SetQrsResultInfo(AggContext * aggContext, const QrsResultPtr& qrsResult, const std::string & info);
    static bool GetQrsResultInfo(AggContext * aggContext, const QrsResultPtr& qrsResult, std::string & info);

private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(helper);

#endif //AGG2_HELPER_AGG_WEB_CONTEXT_HELPER_H
