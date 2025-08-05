#ifndef AGG_WEB_COMMON_DEFINE_H
#define AGG_WEB_COMMON_DEFINE_H

#include <agg2/debuger/define.h>

#define LOG_IS_DEBUG __builtin_expect(_logger->isLevelEnabled(alog::LOG_LEVEL_DEBUG), 0)

// 初始化 processor里面的mContext 和 mAggContext
#define ADJUST_BEGIN_IN_CONTEXT(aggContext, key) {                      \
    std::set<std::string> * adjusterOffSetPtr = AggWebContextHelper::GetAdjusterOffset(aggContext); \
    if (adjusterOffSetPtr == NULL || adjusterOffSetPtr->find(key) == adjusterOffSetPtr->end()) { \

#define ADJUST_END_IN_CONTEXT() }}

#define ADJUST_BEGIN_IN_SUBCONTEXT(aggSubContext, key) {                \
    std::set<std::string> * adjusterOffSetPtr = AggWebSubContextHelper::GetAdjusterOffset(aggSubContext); \
    if (adjusterOffSetPtr == NULL || adjusterOffSetPtr->find(key) == adjusterOffSetPtr->end()) { \

#define ADJUST_END_IN_SUBCONTEXT() }}

// debug输出usercontext中的多队列信息
#define PK_POS_CHECK_FOR_USER_CONTEXT(aggContext)  do {                      \
    bool __pkDebugStatus = false;                                   \
    GET_PK_DEBUG_INFO(aggContext, __pkDebugStatus);                 \
    if (__pkDebugStatus){                                           \
        int __pkPosContext = AggContextHelper::GetContextHitPosPkDebuger(aggContext, __pkDebuger, -1); \
        ADD_PK_DEBUG_INFO_BASIC("#####################################################"); \
        ADD_PK_DEBUG_INFO_BASIC("POS CHECK:[" << __pkPosContext << "]\tSEARCH_NAME:[" << __pkDebuger->GetSearchName() << "]\tCLASS_NAME:[" << mName << "]"); \
    }                                                               \
    if (__debugInfo){                                               \
        const std::string & methodName = aggContext->GetMethod();   \
        std::string key = std::string(AGG_CONTEXT_NAME) + "" + GetName(); \
        if (!methodName.empty()) {                                  \
            key.append("@").append(methodName).append("_");         \
        }                                                           \
        for (int i = 0; i < QUEUETYPE_NUM; ++i) {                   \
            auto hitQueue = AggWebContextHelper::GetMultiHitQueue(aggContext, i);  \
            string key_sub = key + QUEUE_NAME[i];                      \
            for (size_t j = 0;j < hitQueue->GetHitCount(); ++j){        \
                Hit * hit = hitQueue->GetHit(j);                        \
                __debugInfo->InsertProcessHit(key_sub, hit);            \
                AGG_LOG(DEBUG, "%s, %u", key_sub.c_str(), j);                      \
            }                                                           \
        }                                                               \
    }                                                                   \
} while(0)



#endif //AGG_WEB_COMMON_DEFINE_H
