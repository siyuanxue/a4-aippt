#include <agg_web2/common/agg_web_user_context.h>
//#include <search_lib/upstream/sc/sc_request.h>

AGG_BEGIN_NAMESPACE(common);
AGG_LOG_SETUP(common, AggWebUserContext);

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(AggWebUserContext)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(AggWebUserContext)

AggWebUserContext::AggWebUserContext()
{
    mScHitQueueMap.clear();
    mMultiHitQueues.resize(QUEUETYPE_NUM);
    for (size_t i = 0;i < QUEUETYPE_NUM; ++i) {
        mMultiHitQueues[i] = HitQueuePtr(new HitQueue);
    }
    mFinalScHitQueue = HitQueuePtr(new HitQueue);
}

AggWebUserContext::~AggWebUserContext()
{
}

HitQueuePtr AggWebUserContext::GetHitQueue(int queue_type) const
{
    assert(queue_type >= 0);
    assert(queue_type < QUEUETYPE_NUM);
    return mMultiHitQueues[queue_type];
}

void AggWebUserContext::SetScHitQueue(std::string name, HitQueuePtr hitQueue) 
{
    mScHitQueueMap.insert(make_pair(name, hitQueue));    
}

HitQueuePtr AggWebUserContext::GetScHitQueue(std::string name)
{
    if (mScHitQueueMap.find(name) == mScHitQueueMap.end()) {
        mScHitQueueMap[name] = HitQueuePtr(new HitQueue);
    }
    return mScHitQueueMap[name];
}

bool AggWebUserContext::IsScHitQueueEmpty(std::string name)
{
    if (mScHitQueueMap.find(name) == mScHitQueueMap.end()) {
        return true;
    } else {
        return false;
    }
}

void AggWebUserContext::DelScHitQueue(std::string name)
{
    if (mScHitQueueMap.find(name) != mScHitQueueMap.end()) {
        mScHitQueueMap.erase(name);
    }
}

void AggWebUserContext::SetQueryExtend(std::vector<std::string> queryExtend) {
    mQueryExtend = queryExtend;
}

void AggWebUserContext::SetSearchQuery(std::vector<std::string> searchQuery) {
    mSearchQuery = searchQuery;
}

void AggWebUserContext::SetChatResponse(ChatResponsePtr& responsePtr) {
    mChatResponsePtr = responsePtr;
}

void AggWebUserContext::SetMedChatFilterTag(std::string filterTag) {
    mChatFilterTag = filterTag;
}

void AggWebUserContext::SetFinalScHitQueue(HitQueuePtr hitQueue)
{
    mFinalScHitQueue = hitQueue;
}

void AggWebUserContext::SetGeneralInfo(std::string name, String2String &kString)
{
    mGeneralInfoMap.insert(make_pair(name, kString));
}

String2String& AggWebUserContext::GetGeneralInfo(std::string name)
{
    if (mGeneralInfoMap.find(name) == mGeneralInfoMap.end()) {
        mGeneralInfoMap[name] = String2String();
    }
    return mGeneralInfoMap[name];
}

bool AggWebUserContext::IsGeneralInfoEmpty(std::string name)
{
    if (mGeneralInfoMap.find(name) == mGeneralInfoMap.end()) {
        return true;
    } else {
        return false;
    }
}

void AggWebUserContext::DelGeneralInfo(std::string name)
{
    if (mGeneralInfoMap.find(name) != mGeneralInfoMap.end()) {
        mGeneralInfoMap.erase(name);
    }
}

AGG_END_NAMESPACE(common);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(common)::AggWebUserContext);

