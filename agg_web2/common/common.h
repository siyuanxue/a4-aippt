#ifndef AGG_WEB_COMMON_COMMON_H
#define AGG_WEB_COMMON_COMMON_H

#include <A4/common/http_packet_factory.h>
#include <A4/util/string_tokenizer.h>
#include <agg2/common/agg_common.h>
#include <agg2/common/qrs/qrs_hit.h>
#include <messer_utils/xml/pugixml/pugixml.hpp>
#include <GigUtil/dpm/degrade_config.h>
#include <agg_web2/common/constants.h>
#include <agg2/common/recommend/recommend_hit.h>

AGG_BEGIN_NAMESPACE(common);

class Hit;
// medical chat gpt output struct
struct Communication
{
    std::string uid;
    std::string sessionid;
    std::string reqid;
    int resid;
};

struct ChatMessage {
    bool isMed;
    std::string query;
    std::vector<std::string> query_extend;
    std::vector<std::string> history_query;
    std::string query_type;
    std::string query_prompt;
    std::string general_content;
    std::string general_content_cache;
    std::string general_status;
    std::string medical_question;
    std::string status;
    std::string progress;
    std::string general_content_evdoff;
    std::string medical_evidence;
    std::vector<std::unordered_map<std::string, std::string>> medical_origin_links;    
};

struct ChatDebugMessage
{
    std::string query_prompt;
    std::string recommend_prompt;
    std::string summary_prompt;
    std::string rewrite_prompt;
    std::string rewrite_response;
    std::vector<std::unordered_map<std::string, std::string>> medical_search_content;
    std::map<std::string, std::string> debug_kvs;
};

struct ChatResponse
{
    int version;
    std::string target;
    int error_code;
    std::string error_msg;
    std::string timestamp;
    Communication communication;
    ChatMessage data;
    ChatDebugMessage debug_data;
};

// reward sc config info, use in sc_rerank_processor
struct RewardConfigInfo
{
    std::string scName;
    std::string scStype;
    std::string scFullName;

    std::map<std::string, double> scTypeInfo;
    std::map<std::string, double> scInfo;
    std::vector<std::string> scProtectInfo;
    std::string oriInfoStr;
};

struct SubtopicInfo {
    std::string index;
    double score;
    std::vector<std::string> qlist;
    std::vector<std::string> ulist;
    std::vector<std::string> sclist;
    std::string ex;
};

struct QueryLogInfoV2
{
    QueryLogInfoV2(std::string url);
    QueryLogInfoV2() {};
    std::string mNormalizedUrl;
    std::vector<uint64_t> mCrawlTimeVec;
    std::vector<uint32_t> mPosVec;
    uint32_t mLastPos;
    uint32_t mMinPos;
    uint32_t mMaxPos;
    double mAvgPos;
    uint32_t mCrawlNum;
    uint64_t mLastCrawlTime;
    std::string mUniqId;
    std::string mQuery;
};
AGG_TYPEDEF_PTR(QueryLogInfoV2);
typedef std::vector<QueryLogInfoV2Ptr> QueryLogV2Vec;
AGG_TYPEDEF_PTR(QueryLogV2Vec);
AGG_TYPEDEF_PTR(ChatResponse);


struct OfficialInfo
{
    OfficialInfo(std::string &url);
    OfficialInfo() {};
    std::string mNormalizedUrl;
    std::string mCertainty;
    uint32_t mIntention;
    std::string mSources;
    double mAnchor;
    int64_t mValueCnt;
    double mKkCtr;
    int64_t mKkVpt;
    int64_t mKkClk;
    size_t mIdx;
};
AGG_TYPEDEF_PTR(OfficialInfo);
typedef std::vector<OfficialInfoPtr> OfficialInfoVec;
AGG_TYPEDEF_PTR(OfficialInfoVec);

typedef std::vector<double> QueueScoreSortVec;
AGG_TYPEDEF_PTR(QueueScoreSortVec);


// used for index param degrade
struct DegradeInfo {
    DegradeInfo() : _isDegrade(false),_strategy("normal")
    {}

    bool _isDegrade;
    std::string _strategy;
    GIGUTIL::DegradeParam _degradeParam;
};

typedef std::map<std::string, DegradeInfo> DegradeInfoMap;
typedef DegradeInfoMap::const_iterator DegradeInfoMapConstIter;
typedef DegradeInfoMap::iterator DegradeInfoMapIter;

typedef std::map<std::string, std::string> SimDocPair;
AGG_TYPEDEF_PTR(SimDocPair);
typedef std::map<std::string, double> SimDocScorePair;
AGG_TYPEDEF_PTR(SimDocScorePair);

typedef std::set<std::string> SiteMappingSet;
AGG_TYPEDEF_PTR(SiteMappingSet);

typedef std::vector<QrsHit *> SemanticHitVec;
AGG_TYPEDEF_PTR(SemanticHitVec);

typedef std::vector<QrsHit *> Ha3HitVec;
AGG_TYPEDEF_PTR(Ha3HitVec);

typedef std::vector<std::pair<uint32_t, std::string> > QueryLogSCInfoVec;
typedef std::vector<QrsHit * > AuthorityUnitVec;
typedef std::vector<QrsHit * > OfficialHitVec;
AGG_TYPEDEF_PTR(OfficialHitVec);
typedef std::set<std::string> OfficialSubLinkSet;
AGG_TYPEDEF_PTR(OfficialSubLinkSet);
typedef std::vector<RecommendHit * > FeedBackUpHitVec;

typedef std::map<std::string, uint32_t> QueryLogMonitorInfoMap;
AGG_TYPEDEF_PTR(QueryLogMonitorInfoMap);


AGG_TYPEDEF_PTR(QueryLogSCInfoVec);
AGG_TYPEDEF_PTR(FeedBackUpHitVec);

// 从base_external_merge_util.h移过来的
const size_t MAX_SUB_CONTEXT_RESULT = 10000000;

enum EXTERNAL_SUBCONTEXT_TYPE {
  NORMAL = 0,
  CITY_QRS = 100,
  QP_RECUT = 200
};

enum TimelinessSensitive
{
    NONE_TS,
    YEAR_TS,
    MONTH_YEAR_TS,
    DAY_WEEK_TS,
    SPECIFICAL_TS
};

class LLMDebugInfo
{
public:
    LLMDebugInfo() {}
    void AddDebug(const std::string& info) {
        mDebugInfoVec.emplace_back(info);
    }
    const std::vector<std::string> GetDebugInfo() {
        return mDebugInfoVec;
    }

private:
    std::vector<std::string> mDebugInfoVec;
};

AGG_TYPEDEF_PTR(LLMDebugInfo);

class LLMPsg {
public:
    LLMPsg(size_t idx, size_t sampleId, const std::string& title, const std::string& psg)
            : mIdx(idx), mSampleId(sampleId), mTitle(title), mPsg(psg) {
    }
    size_t mIdx = 0;
    size_t mSampleId = 0;
    double mQtc = 0;
    double mPR = 0;
    double mPRV0 = 0;
    double mLLMScore = 0;
    std::string mTitle;
    std::string mPsg;
};
AGG_TYPEDEF_PTR(LLMPsg);

class LLMDoc {
public:
    LLMDoc(const std::string& title, const std::string& body, const std::string& url, int idx,
            double qtc, double qt, double pr, const std::string& publishTime, bool isSc)
            : mTitle(title), mBody(body), mUrl(url), mAggRank(idx), mQtc(qtc), mQt(qt), mPR(pr),
              mPublishTime(publishTime), mIsSc(isSc) {
        mSummary="";
        mRankScore = mQtc * 10 + mQt * 5 + mPR * 20;
    }

    LLMDoc(const std::string& title, const std::string& body, const std::string& url, int idx,
            double qtc, double qt, double pr, const std::string& publishTime, bool isSc,
            const std::string& Hostname, const std::string& Hostlogo)
            : LLMDoc(title, body, url, idx, qtc, qt, pr, publishTime, isSc) {
        if (!Hostname.empty()) mHostname = Hostname;
        if (!Hostlogo.empty()) mHostlogo = Hostlogo;
    }

    const std::string GetDebugString() const {
        return std::string("是否模型输入=") + (mSelection ? "是" : "否")
            + "，rankscore=" + std::to_string(mRankScore)
            + ", recall=" + mRecall
            + "，大搜排序=" + std::to_string(mAggRank)
            + "，标题=" + mTitle
            + "，qtc=" + std::to_string(mQtc)
            + "，qt=" + std::to_string(mQt)
            + "，pr=" + std::to_string(mPR)
            + "，publishtime=" + mPublishTime
            + "，url=" + mUrl
            + "，hostlevel=" + std::to_string(mHostLevel)
            + "，bodylen=" + std::to_string(mBody.size());
    }
    QrsHit *    mQrsHit = NULL;
    std::string mTitle;
    std::string mBody;
    std::string mUrl;
    std::string mSummary;
    std::string mHostname = "None";     // 刷入直达页的默认值
    std::string mHostlogo = "None";     // 刷入直达页的默认值
    std::string mRecall;
    int mAggRank = 0; // agg第几位
    std::string mKgScoreStr = "";
    double mQtc = 0;
    double mQt = 0;
    double mPR = 0;
    double mLLMScore = 0.0;
    double mSummaryPR = 0.0;
    std::string mPublishTime;
    bool mIsSc = false;
    bool mSelection = false;
    double mRankScore = 0;
    double mNewRankScore = 0.0;
    double mRawPR = 0;
    int mHostLevel = 0;
    std::string mDebugInfo;
    
    std::vector<std::string> mBodyLines;
    std::vector<LLMPsgPtr> mPsgVec;
    uint32_t mMaxId = 0;
};
AGG_TYPEDEF_PTR(LLMDoc);


struct DocumentNode {
    std::string mChunkId;
    std::string mID; // 与题录的关联id
    std::string mContentId; //正文id
    std::string mDocID;
    std::string mPK;
    std::string mTitle;
    std::string mChunk;
    std::string mEmbSource;
    std::string mRecall;
    std::string mNote;
    std::string mSummary;
    std::string mAcadamicType;
    std::string mFileType;
    std::string mSubSourceType;
    std::string mSourceType;
    std::string mRawDocUrl;
    std::string mCopyright;
    std::string mLayout;
    std::string mPKUC;
    std::string mCSSCI;
    std::string mCSCD;
    uint32_t mFileSize = 0;
    int mRefPageNum = 0;
    double mVectorScore = 0.0;
    double mLLMScore = 0.0;
    double mQtcScore = 0.0;
    double mQtScore = 0.0;
    double mRankScore = 0.0;
    std::string toString() {
        std::ostringstream ss;
        return ss.str();
    }
};

AGG_END_NAMESPACE(common);
#endif //AGG_WEB_COMMON_COMMON_H
