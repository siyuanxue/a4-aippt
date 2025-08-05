#ifndef AGG_COMMON_AGG_WEB_USER_CONTEXT_H
#define AGG_COMMON_AGG_WEB_USER_CONTEXT_H

#include <agg2/common/agg_user_context.h>
#include <agg2/common/hit_queue.h>
#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(common);

class AggContext;

// 重组队列的每个队列命名
enum QueueType {
    QUEUETYPE_RELEVANCE = 0,
    QUEUETYPE_CLICK,
    QUEUETYPE_AUTHORITY,
    QUEUETYPE_TIMELINESS,
    QUEUETYPE_QLOG,
    QUEUETYPE_FAISS,
    QUEUETYPE_CONTENT,

    QUEUETYPE_NUM
};

const std::string QUEUE_NAME[] = {"relevance","click", "authority","timeliness","qlog","faiss","content"};

const std::string QUEUE_MODEL_SCORE_KEY[] = {
    "FISRT_PARAL_RELEVANCE_SCORE",
    "FISRT_PARAL_CLICK_SCORE",
    "FISRT_PARAL_AUTHORITY_SCORE",
    "FISRT_PARAL_TIMELINESS_SCORE",
    "FISRT_PARAL_QLOG_SCORE",
    "FISRT_PARAL_FAISS_SCORE",
    "FISRT_PARAL_CONTENT_SCORE"
};
typedef std::map<std::string, HitQueuePtr> HitQueueMap;
typedef HitQueueMap::iterator HitQueueMapIter;
typedef HitQueueMap::const_iterator HitQueueMapConstIter;

typedef std::map<std::string, String2String> KStringMap;
typedef KStringMap::iterator KStringMapIter;
typedef KStringMap::const_iterator KStringMapConstIter;

/* 定义agg_web中主上下文扩展存储内容，包括：
 * 1. 多队列合并的中间重组队列
 * 2. 合并的所有sc hit
 */
class AggWebUserContext: public AggUserContext
{
public:
    AggWebUserContext();
    virtual ~AggWebUserContext();

public:
    HitQueuePtr  GetHitQueue(int queue_type) const;

    void SetScHitQueue(std::string name, HitQueuePtr hitQueue);
    HitQueuePtr GetScHitQueue(std::string name);

    bool IsScHitQueueEmpty(std::string name);
    void DelScHitQueue(std::string name);
    HitQueueMap& GetScHitQueueMap() { return mScHitQueueMap; }

    void SetFinalScHitQueue(HitQueuePtr hitQueue);
    HitQueuePtr GetFinalScHitQueue() { return mFinalScHitQueue; }

    void SetGeneralInfo(std::string name, String2String &kString);
    String2String& GetGeneralInfo(std::string name);

    bool IsGeneralInfoEmpty(std::string name);
    void DelGeneralInfo(std::string name);
    KStringMap& GetGeneralInfoMap() { return mGeneralInfoMap; }

    void SetQueryExtend(std::vector<std::string> queryExtend);
    std::vector<std::string> GetQueryExtend() { return mQueryExtend; }

    void SetSearchQuery(std::vector<std::string> searchQuery);
    std::vector<std::string> GetSearchQuery() { return mSearchQuery; }

    void SetChatResponse(ChatResponsePtr& responsePtr);
    ChatResponsePtr& GetChatResponse() { return mChatResponsePtr; }

    void SetMedChatFilterTag(std::string filterTag);
    std::string GetMedChatFilterTag() { return mChatFilterTag; }
    
private:
    // 重组后的多队列
    std::vector<HitQueuePtr> mMultiHitQueues;

    //sc多队列。
    HitQueueMap mScHitQueueMap;
    HitQueuePtr mFinalScHitQueue;
    KStringMap  mGeneralInfoMap;   

    // 交互搜索上下文特征
    std::vector<std::string> mQueryExtend;
    std::vector<std::string> mSearchQuery;

    ChatResponsePtr mChatResponsePtr;
    std::string mChatFilterTag;

private:
    AGG_LOG_DECLARE();

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(AggWebUserContext);

AGG_END_NAMESPACE(common);
BOOST_CLASS_EXPORT_KEY(AGG_NS(common)::AggWebUserContext);

#endif //AGG_COMMON_AGG_WEB_USER_CONTEXT_H
