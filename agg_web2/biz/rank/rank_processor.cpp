#include <agg_web2/biz/rank/rank_processor.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg2/util/string_tool.h>
#include <autil/StringUtil.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg_web2/data/key_value_data.h>
#include <agg_web2/data/value_data_host.h>
#include <agg_web2/common/define.h>
#include <agg_web2/upstream/qc/qc_feature_result.h>
#include <agg_web2/util/QtcExtractor.h>
#include <agg_web2/data/data_type.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/wrapper/agg_data_wrapper.h>
#include <autil/StringUtil.h>
#include <A4/util/hash_algorithm.h>
#include <agg_web2/helper/general_search_helper.h>
#include <agg_web2/data/value_data.h>
#include <agg_web2/util/string_deduper.h>
#include <agg2/helper/time_helper.h>
#include <agg_web2/upstream/qdoc_correlation/qd_request.h>
#include <agg_web2/upstream/qdoc_correlation/qd_result.h>
#include <agg2/upstream/llm_result.h>
#include <agg2/upstream/llm_request.h>

using namespace std;
AGG_BEGIN_NAMESPACE(agg_web2)
AGG_LOG_SETUP(agg_web2, RankProcessor);
AGG_LOG_SETUP_WITH_NAME(displayLog, displayLog, RankProcessor);


BEGIN_DEFINE_SERIALIZATION_FUNC(RankProcessor)
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
    ar & BOOST_SERIALIZATION_NVP(mGeneralLLMSyncNode);
    ar & BOOST_SERIALIZATION_NVP(mAnswerLocateLLMName);
END_DEFINE_SERIALIZATION_FUNC(RankProcessor)

const std::string QTC_NODE_PARAM_KEY = "qtc_bert_node";
const std::string PR_NODE_PARAM_KEY = "pr_node";
const static string PARAM_GENERAL_LLM_SYNC_NODE = "general_llm_sync_server";
const static string PARAM_ANSWER_LOCATE_LLM_MODEL_NAME = "answer_locate_llm_name";
const static string PARAM_LLM_API_KEY = "llm_api_key";

bool RankProcessor::DoInit(const String2String &params)
{
    PARSE_PROCESSOR_PARAM(params, QTC_NODE_PARAM_KEY, mQTCNode);
    PARSE_PROCESSOR_PARAM(params, PR_NODE_PARAM_KEY, mPrNode);
    PARSE_PROCESSOR_PARAM(params, PARAM_GENERAL_LLM_SYNC_NODE, mGeneralLLMSyncNode);
    PARSE_PROCESSOR_PARAM(params, PARAM_ANSWER_LOCATE_LLM_MODEL_NAME, mAnswerLocateLLMName);
    PARSE_PROCESSOR_PARAM(params, PARAM_LLM_API_KEY, mApiKey);
    return true;
}

void RankProcessor::DoRegisterMetric(AmonitorRegister *monitorRegister)
{
}

BaseProcessor* RankProcessor::DoClone()
{
    return new RankProcessor(*this);
}

void RankProcessor::DoDestroy()
{
    delete this;
}

void RankProcessor::GetRerankResult(std::vector<LLMDocPtr>& docPtrVec, std::string& Source)
{
    SplitDoc2Psg(docPtrVec);
    AGG_LOG(ERROR, "rerank recall size=%d", docPtrVec.size());

    GetScores(docPtrVec);
    AGG_LOG(ERROR, "rerank recall size=%d", docPtrVec.size());
    
    mAggContext->SetValue("RAWDATA_" + Source, docPtrVec);

    SelectDocs(docPtrVec, Source);
    AGG_LOG(ERROR, "rerank recall size=%d", docPtrVec.size());
}

bool RankProcessor::GetAggResult(std::vector<LLMDocPtr>& hitQueueAgg)
{
    mAggContext->GetValue("RAWQUERY_WEB_AGG_RESULT", hitQueueAgg);
    AGG_LOG(DEBUG, "AGG start recall size=%d", hitQueueAgg.size());

    // DeDuplicate(hitQueueAgg, 0.99);

    std::string Source = "AGG";
    GetRerankResult(hitQueueAgg, Source);
    //mAggContext->SetValue(HIT_QUEUE_AGG, hitQueueAgg);
    //AGG_LOG(DEBUG, "AGG recall size=%d", hitQueueAgg.size());

    return true;
}

bool RankProcessor::GetAigcResult(std::vector<LLMDocPtr>& hitQueueAigc)
{
    mAggContext->GetValue("RAWQUERY_INDEX_RESULT", hitQueueAigc);
    AGG_LOG(ERROR, "aigc recall size=%d", hitQueueAigc.size());

    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    DeDuplicate(hitQueueAigc, 0.99, false);
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "DeDuplicate Aigc COST:%d", end_time - start_time);

    std::string Source = "AIGC";
    GetRerankResult(hitQueueAigc, Source);

    // 写agg结果到agg队列中
    //mAggContext->SetValue(HIT_QUEUE_AIGC, hitQueueAigc);

    return ! hitQueueAigc.empty();
}

/*void RankProcessor::MergeFinalQueue(std::vector<LLMDocPtr>& hitQueueAgg,
        std::vector<LLMDocPtr>& hitQueueAigc)
{
    LLMTraceInfoPtr traceInfoPtr;
    mAggContext->GetValue(TRACE_INFO, traceInfoPtr);
    AggDataManager * aggDataManager = AggDataManager::GetInstance();
    AggDataWrapper aggDataWrapper(aggDataManager, "host_name_data");
    traceInfoPtr->AddTrace("final agg recall size=" + std::to_string(hitQueueAgg.size()));
    traceInfoPtr->AddTrace("final aigc recall size=" + std::to_string(hitQueueAigc.size()));
    AGG_LOG(DEBUG, "final agg recall size=%llu", hitQueueAgg.size());
    AGG_LOG(DEBUG, "final aigc recall size=%llu", hitQueueAigc.size());

    std::vector<LLMDocPtr> hitQueueFinal;
    hitQueueFinal.insert(hitQueueFinal.begin(), hitQueueAgg.begin(), hitQueueAgg.end());
    hitQueueFinal.insert(hitQueueFinal.begin(), hitQueueAigc.begin(), hitQueueAigc.end());
    if (hitQueueFinal.size() < size_t(2)) {
        AGG_LOG(ERROR, "docPtrVec.size < 2");
        traceInfoPtr->AddTrace("filter by: hitQueueFinal.size < 2");
        hitQueueFinal.clear();
    }

    // 考虑站点权威性，url去重，样本去重等
    // 按得分降序，从分高到分低保序
    sort(hitQueueFinal.begin(), hitQueueFinal.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore < y->mRankScore;
    });
    set<string> dedup;
    for (size_t i=hitQueueFinal.size(); i>0; --i) {
        auto docPtr = hitQueueFinal[i - 1];
        string bodyhash = std::to_string(HashAlgorithm::HashString64(docPtr->mBody.c_str()));
        if (dedup.find(docPtr->mUrl) != dedup.end()
                || dedup.find(bodyhash) != dedup.end()) {
            hitQueueFinal.erase(hitQueueFinal.begin() + i - 1);
        }
        dedup.insert(docPtr->mUrl);
        dedup.insert(bodyhash);
    }

    // 统一混排，从分高到分低保序
    sort(hitQueueFinal.begin(), hitQueueFinal.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });
    if (hitQueueFinal.size()>0) {
        AGG_LOG(DEBUG, "hitQueueFinal first qtc=%f, pr=%f, authorityscore=%f",
                hitQueueFinal[0]->mQtc, hitQueueFinal[0]->mPR, hitQueueFinal[0]->mAuthorityScore);

        double maxPrScore = 0.0;
        auto docPtrMaxPr = hitQueueFinal[0]; 
        if (hitQueueFinal[0]->mQtc < 1 || hitQueueFinal[0]->mPR < 0.85
                || hitQueueFinal[0]->mAuthorityScore < 2.5) {
            for (size_t i=0; i<hitQueueFinal.size(); ++i) {
                auto docPtr = hitQueueFinal[i];
                if (docPtr->mQtc >= 1 && docPtr->mPR >= 0.85 && docPtr->mAuthorityScore >= 2.5) {
                    if(docPtr->mPR > maxPrScore){
                        maxPrScore = docPtr->mPR;
                        docPtrMaxPr = docPtr;
                    }
                }
            }
            if(maxPrScore > 0.85){
                docPtrMaxPr->mRankScore = docPtrMaxPr->mRankScore + 100;
            }
        }
    }
    dedup.clear();
    for (size_t i=0; i<hitQueueFinal.size(); ++i) {
        auto docPtr = hitQueueFinal[i];
        string host = GetHost(docPtr->mUrl);
        string mHostName = aggDataWrapper.Get<ValueDataHost>()->GetHostName(docPtr->mUrl);
        if (mHostName != host){
            docPtr->mHostname = mHostName;
        }else if (mHostName == host && (docPtr->mHostname=="None" || docPtr->mHostname.empty())) {
            docPtr->mHostname = host;
        }
        string mHostNameDedup = docPtr->mHostname;
        AGG_LOG(DEBUG, "host_name=%s", host.c_str());
        AGG_LOG(DEBUG, "host_name_zh=%s", mHostName.c_str());

        if (host.find("baike.baidu.com") != std::string::npos ||
                host.find("baike.sogou.com") != std::string::npos || 
                host.find("baike.so.com") != std::string::npos ||
                host.find("baike.quark.cn")!= std::string::npos){
            mHostNameDedup = "百科";
        } else if (host.find("www.zhihu.com") != std::string::npos
                || host.find("zhuanlan.zhihu.com") != std::string::npos){
            mHostNameDedup = "知乎";
        } else if (host.find("m.sohu.com") != std::string::npos
                || host.find("www.sohu.com") != std::string::npos){
            mHostNameDedup = "搜狐";
        }
        if (dedup.find(mHostNameDedup) != dedup.end()) {
            docPtr->mRankScore -= 30; // 站点去重降权
        }
        AGG_LOG(DEBUG, "host_name_final=%s", host.c_str());
        dedup.insert(mHostNameDedup);
    }

    // 最终混排
    sort(hitQueueFinal.begin(), hitQueueFinal.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });
    
    mAggContext->SetValue(HIT_QUEUE_FINAL, hitQueueFinal);
} */

int RankProcessor::DoProcess(AggContext* aggContext)
{
    INIT_CONTEXT(aggContext)

    // 1. 获取agg结果
    std::vector<LLMDocPtr> hitQueueAgg;
    if (! GetAggResult(hitQueueAgg)) {
        AGG_LOG(DEBUG, "AGG final recall size=%d", hitQueueAgg.size());
        hitQueueAgg.clear();
    }

    // 2. 获取aigc索引结果
    std::vector<LLMDocPtr> hitQueueAigc;
    if (! GetAigcResult(hitQueueAigc)) {
        hitQueueAigc.clear();
    }

    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    DeDuplicate(hitQueueAgg, 0.9, false);
    DeDuplicate(hitQueueAigc, 0.9, true);
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "DeDuplicate all COST:%d", end_time - start_time);

    AGG_LOG(ERROR, "AGG final recall size=%lu", hitQueueAgg.size());
    AGG_LOG(ERROR, "AIGC final recall size=%lu", hitQueueAigc.size());

    // 3. 排序 & 合并
    /*if (hitQueueAgg.size() + hitQueueAigc.size() < size_t(2)) {
        AGG_LOG(ERROR, "docPtrVec.size < 2");
        hitQueueAgg.clear();
        hitQueueAigc.clear();
    } else {
        MergeFinalQueue(hitQueueAgg, hitQueueAigc);
    }*/

    for (size_t i = 0; i < hitQueueAigc.size(); ++i) {
        AGG_LOG(ERROR, "AIGC LIST[%s]", hitQueueAigc[i]->mUrl.c_str());
    }
    
    std::vector<LLMDocPtr> hitQueueFinal;
    hitQueueFinal.insert(hitQueueFinal.end(), hitQueueAgg.begin(), hitQueueAgg.end());
    hitQueueFinal.insert(hitQueueFinal.end(), hitQueueAigc.begin(), hitQueueAigc.end());

    ReRankByHost(hitQueueFinal); // 优质站点调权

    ReRankByTime(hitQueueFinal); // 时效性调权
    sort(hitQueueFinal.begin(), hitQueueFinal.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });

    ReRankBySource(hitQueueFinal); // 数据来源调权
    
    sort(hitQueueFinal.begin(), hitQueueFinal.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });

    RequestAnswerLocateModel(hitQueueFinal, 10);
    
    // for (size_t i = 0; i < hitQueueFinal.size(); ++i) {
    //     AGG_LOG(ERROR, "FINAL LIST[%s]", hitQueueFinal[i]->mUrl.c_str());
    // }

    //std::vector<LLMDocPtr> hitQueueFinal;
    mAggContext->SetValue("FINAL_RESULT", hitQueueFinal);

    return PR_SUCCESS;
}

void RankProcessor::SplitDoc2Psg(std::vector<LLMDocPtr>& LLMDocPtrQueue)
{
    size_t sampleId = 0;
    for (auto& LLMDocPtr : LLMDocPtrQueue) {
        LLMDocPtr->mBodyLines = autil::StringUtil::split(LLMDocPtr->mBody, "\n");
        size_t idx = 0;
        size_t num = 0;
        while (idx < LLMDocPtr->mBodyLines.size()) {
            ++ num;
            string psg;
            size_t next = LLMDocPtr->mBodyLines.size();
            for (size_t i=idx; i<LLMDocPtr->mBodyLines.size(); ++i) {
                psg += LLMDocPtr->mBodyLines[i] + "\n";
                if (psg.size() >= size_t(200 * 3)) {
                    next = min(next, i + 1);
                }
                if (psg.size() >= size_t(450 * 3)) {
                    break;
                }
            }
            AGG_LOG(DEBUG, "SplitDoc2Psg idx=%llu, next=%llu, num=%llu, url=%s, title=%s, psg=%s",
                    idx, next, num,
                    LLMDocPtr->mUrl.c_str(), LLMDocPtr->mTitle.c_str(), psg.c_str());
            if (psg.size() >= size_t(20 * 3)) {
                LLMPsgPtr LLMPsgPtr(new LLMPsg(idx, ++sampleId, LLMDocPtr->mTitle, psg));
                LLMDocPtr->mPsgVec.emplace_back(LLMPsgPtr);
            }
            idx = next; // overlap方式，取200个字后的passage
        }
        // if (LLMDocPtr->mRecall != "WEB_AGG") {
        //     AGG_LOG(ERROR, "SplitDoc2Psg total psg=%llu, source=%s, body=%s", num, LLMDocPtr->mRecall.c_str(), LLMDocPtr->mBody.c_str());
        // }
    }
    AGG_LOG(DEBUG, "SplitDoc2Psg total psg=%llu", sampleId);
}

void RankProcessor::ReRankByHost(std::vector<LLMDocPtr>& LLMDocPtrQueue)
{
    // host过滤
    AggDataManager * aggDataManager = AggDataManager::GetInstance();
    AggDataWrapper filterHostWrapper(aggDataManager, "gaokao_filter_host_data");
    AggDataWrapper goodHostWrapper(aggDataManager, "gaokao_good_host_data");
    if (filterHostWrapper.Get<ValueData>() == NULL ||
            goodHostWrapper.Get<ValueData>() == NULL) {
        AGG_LOG(ERROR, "Get GaoKao Filter/Good Host failed.");
        return;
    }

    const std::vector<std::string>& hostblacklist = filterHostWrapper.Get<ValueData>()->GetValue();
    const std::vector<std::string>& hostgoodlist = goodHostWrapper.Get<ValueData>()->GetValue();
    string goodhosts = autil::StringUtil::toString(hostgoodlist, ";");
    /* AGG_LOG(ERROR, "Good Hosts [%s]", goodhosts.c_str()); */
    for (size_t i = 0; i < LLMDocPtrQueue.size(); ++i) {
        int32_t host_level = 0;
        string url = LLMDocPtrQueue[i]->mUrl;
        string host;
        size_t pos = url.find("://");
        if (pos == string::npos) {
            host = url;
            continue;
        }
        
        host = url.substr(pos+3); // 原始Host
        size_t pos2 = host.find("/");
        if (pos2 != string::npos) {
            host = host.substr(0, pos2);
        }

        if (std::find(hostblacklist.begin(), hostblacklist.end(), host) != hostblacklist.end()) { //命中黑名单
            host_level -= 100;
        }
        if (std::find(hostgoodlist.begin(), hostgoodlist.end(), host) != hostgoodlist.end()) { //命中优质名单
            host_level += 100;
        }
        
        AGG_LOG(DEBUG, "Raw Host[%s][%d]", host.c_str(), host_level);
        if (host == "page.sm.cn") {// 自建站点特殊处理
            pos = url.find("?h=");
            if (pos == string::npos) {
                pos = url.find("&h=");
            }
            if (pos != string::npos) {
                host = url.substr(pos+3);
                pos2 = host.find("&");
                if (pos2 != string::npos) {
                    host = host.substr(0, pos2);
                }
            }
            
            if (std::find(hostblacklist.begin(), hostblacklist.end(), host) != hostblacklist.end()) { //命中黑名单
                host_level -= 1000;
            }
            if (std::find(hostgoodlist.begin(), hostgoodlist.end(), host) != hostgoodlist.end()) { //命中优质名单
                host_level += 1000;
            }
            AGG_LOG(ERROR, "sm.cn Host[%s][%d]", host.c_str(), host_level);
        }

        string first_host = host;
        pos = host.find(".");
        if (pos != string::npos) {// 去除一级站点限制
            host = host.substr(pos+1);
            pos2 = host.find(".");
            if (pos2 != string::npos) {
                if (std::find(hostblacklist.begin(), hostblacklist.end(), host) != hostblacklist.end()) { //命中黑名单
                    host_level -= 10;
                }
                if (std::find(hostgoodlist.begin(), hostgoodlist.end(), host) != hostgoodlist.end()) { //命中优质名单
                    host_level += 10;
                }
                /* AGG_LOG(ERROR, "Second Host[%s][%d]", host.c_str(), host_level); */
            }
        }

        if (host_level > 0) {
            double pr = LLMDocPtrQueue[i]->mPR;
            double hostWeight = 1000.0 * pr;
            LLMDocPtrQueue[i]->mRankScore += hostWeight;
            LLMDocPtrQueue[i]->mHostLevel = 1;
            LLMDocPtrQueue[i]->mDebugInfo += "host=" + first_host+"+"+to_string(hostWeight)+"\n ";
            // AGG_LOG(ERROR, "Hit Good Host[%s]", url.c_str());
        }
        if (host_level < 0) {
            LLMDocPtrQueue[i]->mRankScore -= 1000;
        }
    }
}

void RankProcessor::ReRankBySource(std::vector<LLMDocPtr>& LLMDocPtrQueue)
{
    vector<string> queryIntents;
    mAggContext->GetValue("QUERY_INTENTS", queryIntents);
    string queryIntentStr = autil::StringUtil::toString(queryIntents, ",");
    AGG_LOG(ERROR, "QUERY_INTENTS [%s]", queryIntentStr.c_str());

    bool nonPolicy = false;
    if (!queryIntentStr.empty() && (queryIntentStr.find("问观点") != string::npos || queryIntentStr.find("问建议") != string::npos || queryIntentStr.find("问方法") != string::npos || queryIntentStr.find("问原因") != string::npos)) { //非政策类
        nonPolicy = true;
    }
    
    std::map<std::string, int> scourceLimit;
    scourceLimit["百科"] = 1;
    scourceLimit["EOL"] = 2;
    scourceLimit["DXSBB"] = 1;
    scourceLimit["PKURLS"] = 2;

    for (size_t i = 0; i < LLMDocPtrQueue.size(); ++i) {
        string source = LLMDocPtrQueue[i]->mRecall;
        // AGG_LOG(ERROR, "time [%s]", LLMDocPtrQueue[i]->mPublishTime.c_str());
        if (source == "百科" && scourceLimit["百科"]>0) {
            double sourceWeight = 1000 * LLMDocPtrQueue[i]->mPR;
            LLMDocPtrQueue[i]->mRankScore += sourceWeight;
            LLMDocPtrQueue[i]->mDebugInfo += "source=" + source+"+"+to_string(sourceWeight)+"\n ";
            scourceLimit["百科"] -= 1;
            AGG_LOG(ERROR, "add weights to [%s]", source.c_str());
        }
        else if (source == "EOL" && scourceLimit["EOL"]>0)
        {
            double sourceWeight = 1000 * LLMDocPtrQueue[i]->mPR;
            LLMDocPtrQueue[i]->mRankScore += sourceWeight;
            LLMDocPtrQueue[i]->mDebugInfo += "source=" + source+"+"+to_string(sourceWeight)+"\n ";
            scourceLimit["EOL"] -= 1;
            AGG_LOG(ERROR, "add weights to [%s]", source.c_str());
        }
        else if (source == "DXSBB" && scourceLimit["DXSBB"]>0)
        {
            double sourceWeight = 1000 * LLMDocPtrQueue[i]->mPR;
            LLMDocPtrQueue[i]->mRankScore += sourceWeight;
            LLMDocPtrQueue[i]->mDebugInfo += "source=" + source+"+"+to_string(sourceWeight)+"\n ";
            scourceLimit["DXSBB"] -= 1;
            AGG_LOG(ERROR, "add weights to [%s]", source.c_str());
        }
        else if (source == "PKURLS" && scourceLimit["PKURLS"]>0)
        {
            double sourceWeight = 1000 * LLMDocPtrQueue[i]->mPR;
            LLMDocPtrQueue[i]->mRankScore += sourceWeight;
            LLMDocPtrQueue[i]->mDebugInfo += "source=" + source+"+"+to_string(sourceWeight)+"\n ";
            scourceLimit["PKURLS"] -= 1;
            AGG_LOG(ERROR, "add weights to [%s]", source.c_str());
        }

        if (nonPolicy && LLMDocPtrQueue[i]->mUrl.find("zhihu")!=string::npos) {
            double sourceWeight = 100 * LLMDocPtrQueue[i]->mPR;
            LLMDocPtrQueue[i]->mRankScore += sourceWeight;
            LLMDocPtrQueue[i]->mDebugInfo += "source=zhihu+"+to_string(sourceWeight)+"\n ";
            AGG_LOG(ERROR, "add weights to [zhihu]");
        }
    }
}

std::string RankProcessor::GetFormatTimeStr(int32_t Currenttime)
{
    if (Currenttime < 1000000000) {
        return std::string("");
    }

    char outStr[256];
    time_t t = Currenttime;
    struct tm *tmp = localtime(&t);
    strftime(outStr, sizeof(outStr), "%Y-%m-%d %H:%M:%S", tmp);
    return std::string(outStr);
}

int32_t RankProcessor::GetFormatTimeInt32(std::string timestr)
{
    if (timestr.empty() || autil::StringUtil::split(timestr, " ").size() != 2 ||
            autil::StringUtil::split(timestr, "-").size() != 3 || autil::StringUtil::split(timestr, ":").size() != 3) {
        return 0;
    }

    tm tm_;
    char buf[128]= {0};
    
    //strcpy(buf, "2012-01-01 14:00:00");
    strcpy(buf, timestr.c_str());
    strptime(buf, "%Y-%m-%d %H:%M:%S", &tm_); //将字符串转换为tm时间
    tm_.tm_isdst = -1;
    time_t t_ = mktime(&tm_); //将tm时间转换为秒时间
    
    return (int32_t)t_;
}

std::vector<int> RankProcessor::extractYears(const std::string& text) {
    std::vector<int> years;
    int length = text.length();

    for(int i = 0; i <= length - 4; ++i) {
        if(std::isdigit(text[i]) && std::isdigit(text[i+1]) && 
           std::isdigit(text[i+2]) && std::isdigit(text[i+3])) {
            int year = std::stoi(text.substr(i, 4));

            if(year >= 2015 && year <= 2030) {
                years.push_back(year);
                i += 3;
            }
        }
    }
    return years;
}

void RankProcessor::ReRankByTime(std::vector<LLMDocPtr>& LLMDocPtrQueue)
{

    for (size_t i=0; i < LLMDocPtrQueue.size(); ++i) {
        double pr = LLMDocPtrQueue[i]->mPR;
        double qtc = LLMDocPtrQueue[i]->mQtc;

        // 时间格式转换
        if (LLMDocPtrQueue[i]->mUrl.find("news_uchq") == 0 && LLMDocPtrQueue[i]->mPublishTime != "") {
            int32_t tmp;
            if (autil::StringUtil::strToInt32(LLMDocPtrQueue[i]->mPublishTime.c_str(), tmp)) {
                LLMDocPtrQueue[i]->mPublishTime = GetFormatTimeStr(tmp); 
            }
        }

        int32_t curTime = (int32_t)TimeHelper::GetSystemCurrentTimeSec();
        int32_t docTime = GetFormatTimeInt32(LLMDocPtrQueue[i]->mPublishTime);
        // AGG_LOG(ERROR, "publish time [%s], doctime [%d]", LLMDocPtrQueue[i]->mPublishTime.c_str(), docTime);

        //获取网页中的年份信息
        vector<int> yearsInTitle = extractYears(LLMDocPtrQueue[i]->mTitle);
        vector<int> yearsInBody = extractYears(LLMDocPtrQueue[i]->mBody);
        if (yearsInTitle.size() > 0) {
            int max_year = *max_element(yearsInTitle.begin(), yearsInTitle.end()); 
            string new_date = to_string(max_year) + "-12-31 23:59:59";
            int32_t titleTime = GetFormatTimeInt32(new_date);
            if (titleTime<docTime || docTime==0) {
                AGG_LOG(ERROR, "time not match, bodytime is [%s], publishtime is [%s], body is [%s]", new_date.c_str(), LLMDocPtrQueue[i]->mPublishTime.c_str(), LLMDocPtrQueue[i]->mBody.c_str());
                docTime = titleTime;
            }
        }
        else if (yearsInBody.size() > 0) {
            int max_year = *max_element(yearsInBody.begin(), yearsInBody.end()); 
            string new_date = to_string(max_year) + "-12-31 23:59:59";
            int32_t bodyTime = GetFormatTimeInt32(new_date);
            if (bodyTime<docTime || docTime==0) {
                AGG_LOG(ERROR, "time not match, bodytime is [%s], publishtime is [%s], body is [%s]", new_date.c_str(), LLMDocPtrQueue[i]->mPublishTime.c_str(), LLMDocPtrQueue[i]->mBody.c_str());
                docTime = bodyTime;
            }
        }

        if (docTime <= 0) {
            LLMDocPtrQueue[i]->mDebugInfo += "days=null\n ";
            continue;
        }

        int32_t days = (curTime - docTime) / 3600 / 24;

        // 时效query下，对30天内结果
        if (days < 30 && days>0) { //30天内结果前提权 
            double timeWeight = 100.0 * (qtc/2+pr)/2;
            LLMDocPtrQueue[i]->mRankScore += timeWeight;
            LLMDocPtrQueue[i]->mDebugInfo += "days=" + to_string(days) +"+"+ to_string(timeWeight)+"\n ";
        }
        // 时效query下，对360天内结果
        else if (days < 360 && days>0) { // 360天结果调权
            double timeWeight = (1 + (360 - days) / 180.0) * (0.3 + 0.7 * pr) * 5.0;
            LLMDocPtrQueue[i]->mRankScore += timeWeight;
            LLMDocPtrQueue[i]->mDebugInfo += "days=" + to_string(days) +"+"+ to_string(timeWeight)+"\n ";
        }

        // 对时效性差的数据降权
        int32_t outdatedTime = GetFormatTimeInt32("2022-12-31 23:59:59");
        if (docTime <= outdatedTime) {
            double timeWeight = days * (1-pr); 
            LLMDocPtrQueue[i]->mRankScore -= timeWeight;
            AGG_LOG(ERROR, "outdated Doc, doctime is [%s], publishtime is [%s], minus weight [%lf]", GetFormatTimeStr(docTime).c_str(), LLMDocPtrQueue[i]->mPublishTime.c_str(), timeWeight);
            LLMDocPtrQueue[i]->mDebugInfo += "days=" + to_string(days) +"-"+ to_string(timeWeight)+"\n ";
        }
    }
}

void RankProcessor::DeDuplicate(std::vector<LLMDocPtr>& LLMDocPtrQueue, float sim_threshold, bool bySource=false) //TODO 保留时间戳最新的、rankscore大的
{
    if (LLMDocPtrQueue.size() <= 1) {
        AGG_LOG(INFO, "no need to dedup");
        return;
    }
    AGG_LOG(ERROR, "items before dedup count [%d]", LLMDocPtrQueue.size());

    // prepare data
    vector<vector<string>> tokenVecVec;
    vector<set<string>> tokenSetVec;
    for (size_t i=0; i < LLMDocPtrQueue.size(); ++i) {
        // string body = LLMDocPtrQueue[i]->mBody;
        string body = AggWebStringTool::SubStrByUtf8Size(LLMDocPtrQueue[i]->mBody, 100); //全文计算耗时严重，先用前100字，后续可考虑minhash算法：https://code.alibaba-inc.com/sitao.cst/a4_agg_7u/blob/master/agg/util/dedup/simhash_deduper.cpp
        if (body.size() == 0) {
            body = "empty";
        }
        // AGG_LOG(ERROR, "prepare token data, size[%d]", body.size());
        vector<string> tokens;
        StringDeduper::SplitToWords(body, tokens, true, true);
        tokenVecVec.push_back(tokens);
        vector<string> tg = StringDeduper::getNgrams(tokens, 2);
        set<string> ts = StringDeduper::transVec2Set(tg);
        tokenSetVec.push_back(ts);
    }
    // AGG_LOG(ERROR, "prepare DeDuplicate data");

    // to dedup
    std::set<size_t> removeSet;
    std::vector<size_t> keepSet;
    bool flag = false;
    for (size_t i=0; i < LLMDocPtrQueue.size(); ++i) {
        flag = false;
        for (size_t j=0; j < keepSet.size(); ++j) {
            size_t curIdx = keepSet[j];
            float jac_sim = StringDeduper::jaccard(tokenSetVec[i], tokenSetVec[curIdx]);
            if (jac_sim >= sim_threshold || LLMDocPtrQueue[i]->mUrl==LLMDocPtrQueue[curIdx]->mUrl) {
                if ((bySource && LLMDocPtrQueue[i]->mRecall==LLMDocPtrQueue[curIdx]->mRecall) || (!bySource)) {
                    // AGG_LOG(ERROR, "item [%d]-[%s] is similar with before-item [%d]-[%s] by jac_sim [%f]", i, LLMDocPtrQueue[i]->mUrl.c_str(), j, LLMDocPtrQueue[curIdx]->mUrl.c_str(), jac_sim);
                    // AGG_LOG(ERROR, "[%s]", LLMDocPtrQueue[i]->mBody.c_str());
                    // AGG_LOG(ERROR, "[%s]", LLMDocPtrQueue[curIdx]->mBody.c_str());
                    int32_t docTime1 = GetFormatTimeInt32(LLMDocPtrQueue[i]->mPublishTime);
                    int32_t docTime2 = GetFormatTimeInt32(LLMDocPtrQueue[curIdx]->mPublishTime);
                    // AGG_LOG(ERROR, "docTime1 [%s], docTime2 [%s], time1[%d], time2[%d]", LLMDocPtrQueue[i]->mPublishTime.c_str(), LLMDocPtrQueue[curIdx]->mPublishTime.c_str(), docTime1, docTime2);
                    if (docTime1<docTime2) { //保留时效性最新的
                        removeSet.insert(i);
                    }
                    else {
                        removeSet.insert(curIdx); 
                        keepSet[j] = i;
                    }
                    // AGG_LOG(ERROR, "[%s]", LLMDocPtrQueue[i]->mRecall.c_str());
                    flag = true;
                    break;
                }
            }
        }
        if (!flag) {
            keepSet.emplace_back(i);
        }
    }

    // output
    vector<LLMDocPtr> docPtrVecFinal;
    for (size_t i=0; i < LLMDocPtrQueue.size(); ++i) {
        auto docPtr = LLMDocPtrQueue[i];
        if (removeSet.count(i)) {
            AGG_LOG(DEBUG, "***dedup remove doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
        } else {
            AGG_LOG(DEBUG, "***dedup output doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
            docPtrVecFinal.push_back(docPtr);
        }
    }

    LLMDocPtrQueue = docPtrVecFinal;
    AGG_LOG(ERROR, "items after dedup count [%d]", LLMDocPtrQueue.size());
}

/*
double RankProcessor::GetAuthorityWeight(const LLMDocPtr docPtr)
{
    // 1. 优先使用绝影配置的权重
    AggDataManager * aggDataManager = AggDataManager::GetInstance();
    AggDataWrapper aggDataWrapper(aggDataManager, "host_name_filter_data");
    const auto& hostWeightMap = aggDataWrapper.Get<KeyValueData>()->GetkvData();
    const auto& url = docPtr->mUrl;
    for (const auto& kv: hostWeightMap) {
        if (url.find(kv.first) != std::string::npos) {
            double weight = 0;
            SAFE_TYPE_CHANGE(kv.second, weight);
            AGG_LOG(DEBUG, "url=%s, host=%s, weight=%lf", url.c_str(), kv.first.c_str(), weight);
            return weight;
        }
    }

    // 2. 基于aigc独立索引，配置默认权重
    if (docPtr->mRecall.find("百科") != string::npos) {
        return 4.5;
    }
    if (docPtr->mRecall.find("知乎") != string::npos) { // 知乎
        // 点赞数、评论数
        return 4;
    }
    if (docPtr->mRecall.find("汽车") != string::npos ||
            docPtr->mRecall.find("游戏") != string::npos ||
            docPtr->mRecall.find("法律") != string::npos ||
            docPtr->mRecall.find("运动") != string::npos ||
            docPtr->mRecall.find("3c") != string::npos) { // 汽车、游戏、法律、运动、3c
        return 3;
    }

    // 3. 来源大搜
    if (docPtr->mRecall.find(RECALL_NAME_AGG) != string::npos) {
        return docPtr->mAuthorityScore;
    }

    // 4. 来源其它aigc独立索引，默认权重
    return 2.5;
}*/

void RankProcessor::SelectDocs(std::vector<LLMDocPtr>& LLMDocPtrQueue, std::string& Source)
{
    std::vector<LLMDocPtr> hitQueueAigc;
    for (auto docPtr: LLMDocPtrQueue) {
        LLMPsgPtr maxPsg;
        double maxScore = 0;
        bool isSelected = false;
        for (size_t j = 0; j < docPtr->mPsgVec.size(); ++j) {
            LLMPsgPtr curPsg = docPtr->mPsgVec[j];

            bool match = false;
            if (Source.find("AGG") != string::npos) {
                match = curPsg->mQtc >= 1.2 || curPsg->mPR >=0.8 || (curPsg->mQtc >= 0.7 && curPsg->mPR >= 0.5);
            } else {
                // match = curPsg->mPR >= 0. && curPsg->mQtc >= 1.06;
                match = curPsg->mPR >= 0.5 && curPsg->mQtc >= 0.5; //调低阈值
            }

            if (docPtr->mRecall == "PKURLS") {
                AGG_LOG(DEBUG, "%d recall=%s, url=%s, title=%s, pr=%lf, qtc=%lf, psg=[%s]",
                        (int32_t)match, docPtr->mRecall.c_str(), docPtr->mUrl.c_str(), docPtr->mTitle.c_str(),
                        curPsg->mPR, curPsg->mQtc, curPsg->mPsg.c_str()); 
            }

            // TODO: 加入权威性、质量分、时效性得分
            double score = curPsg->mPR * 20 + curPsg->mQtc * 10;
            if (score > maxScore) {
                maxScore = score;
                maxPsg = curPsg;
            }
            if (match) {
                isSelected = true;
            }
            // AGG_LOG(ERROR, "SelectDocs doc mPR=%lf, mQTC=%lf, score=%lf, match=%d, recall=%s", curPsg->mPR, curPsg->mQtc, score, int(match), docPtr->mRecall.c_str());
        }
        //AGG_LOG(DEBUG, "SelectDocs doc qtcnn=%lf, qtnn=%lf, url=%s, title=%s, maxScore=%lf",
        //        docPtr->mQtcnn, docPtr->mQtnn, docPtr->mUrl.c_str(), docPtr->mTitle.c_str(), maxScore);

        if (maxPsg) { //body不为空都会有maxPsg
            docPtr->mPR = maxPsg->mPR;
            docPtr->mQtc = maxPsg->mQtc;
            //docPtr->mAuthorityScore = 0.0;//GetAuthorityWeight(docPtr);
            docPtr->mRankScore = docPtr->mPR * 20 + docPtr->mQtc * 10;// + docPtr->mAuthorityScore * 10;
            size_t idx = maxPsg->mIdx;
            size_t accLen = maxPsg->mPsg.size();
            while (idx >= 1 && idx + 4 >= maxPsg->mIdx) { // 最多往前拓展两行
                if (accLen + docPtr->mBodyLines[idx - 1].size() < size_t(600 * 3)) {
                    accLen += docPtr->mBodyLines[idx - 1].size(); // 修复Bug
                    -- idx;
                } else {
                    break;
                }
            }
            docPtr->mBody.clear();
            for (size_t i=idx; i<docPtr->mBodyLines.size(); ++i) {
                if (i != idx) docPtr->mBody += "\n";
                docPtr->mBody += docPtr->mBodyLines[i];
                if (docPtr->mBody.size() >= size_t(1800 * 3)) { //700字改到1800字，再交由AL抽取，以保障答案完整性
                    break;
                }
            }
            if (isSelected) {
                hitQueueAigc.emplace_back(docPtr);
            }

            AGG_LOG(DEBUG, "recall=%s, url=%s, title=%s, pr=%lf, qtc=%lf, rankscore=%lf, body=%s",
                    docPtr->mRecall.c_str(), docPtr->mUrl.c_str(), docPtr->mTitle.c_str(),
                    maxPsg->mPR, maxPsg->mQtc, docPtr->mRankScore, docPtr->mBody.c_str());

            if (docPtr->mRecall == "PKURLS") {
                AGG_LOG(DEBUG, "final raw psg[%s]", maxPsg->mPsg.c_str());
                AGG_LOG(DEBUG, "final recall=%s, url=%s, title=%s, pr=%lf, qtc=%lf, rankscore=%lf, body=%s",
                        docPtr->mRecall.c_str(), docPtr->mUrl.c_str(), docPtr->mTitle.c_str(),
                        maxPsg->mPR, maxPsg->mQtc, docPtr->mRankScore, docPtr->mBody.c_str());
            }
        }
        else {
            if (maxPsg) {
                docPtr->mPR = maxPsg->mPR;
                docPtr->mQtc = maxPsg->mQtc;
                docPtr->mRankScore = docPtr->mPR * 20 + docPtr->mQtc * 10;// + docPtr->mAuthorityScore * 10;
                AGG_LOG(ERROR, "not select doc [%s], source [%s], url [%s], pr=%lf, qtc=%lf, rankscore=%lf", docPtr->mTitle.c_str(), docPtr->mRecall.c_str(), docPtr->mUrl.c_str(), docPtr->mPR, docPtr->mQtc, docPtr->mRankScore);
            }
            else {
                AGG_LOG(ERROR, "not select doc [%s], source [%s], url [%s], body [%s]", docPtr->mTitle.c_str(), docPtr->mRecall.c_str(), docPtr->mUrl.c_str(), docPtr->mBody.c_str());
            }
        }
    }

    // 根据rankscore排序
    sort(hitQueueAigc.begin(), hitQueueAigc.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });

    LLMDocPtrQueue.swap(hitQueueAigc);
}

void RankProcessor::FilterRequestPr(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter){
    size_t batch = 4;
    vector<vector<LLMPsgPtr>> psgSampleVec;
    vector<HttpResponsePtr> prRespVec;
    vector<LLMPsgPtr> cands;
	// 异步请求pr得分
	for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
		for (size_t j = 0; j < psgVec.size(); ++j) {
			LLMPsgPtr curPsg = psgVec[j];
			if(!NeedFilter ||  curPsg->mQtc >= 0.1){
				cands.emplace_back(curPsg);
			}
			if (cands.size() >= batch) {
				BatchRequestPR(query, cands, psgSampleVec, prRespVec);
				cands.clear();
			}
		}
	}
	if (! cands.empty()) {
		BatchRequestPR(query, cands, psgSampleVec, prRespVec);
		cands.clear();
	}

	// 异步获取pr得分
	for (size_t i=0; i<psgSampleVec.size(); ++i) {
		vector<double> prScores = GetPr(prRespVec[i]);
		vector<LLMPsgPtr>& psgSample = psgSampleVec[i];
		if (prScores.size() == psgSample.size()) {
			for (size_t j=0; j<psgSample.size(); ++j) {
				if(false && prScores[j] < 0.1){
					psgSample[j]->mPR = 0.0;
					psgSample[j]->mQtc = 0.0;
				}else{
					psgSample[j]->mPR = prScores[j];
				}
			}
		}
	}
}

// TODO:发现有几个机器时延异常，需要排查
void RankProcessor::FilterRequestPrNew(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter)
{
    if (query.empty())
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "qd_correlation_model");

    vector<LLMPsgPtr> psgSampleVec;
    for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
		for (size_t j = 0; j < psgVec.size(); ++j) {
			LLMPsgPtr curPsg = psgVec[j];
			if(!NeedFilter ||  curPsg->mQtc >= 0.1) {
				psgSampleVec.emplace_back(curPsg);
			}
		}
	}
    
    AGG_LOG(ERROR, "FilterRequestPrNew size:%d", (int32_t)psgSampleVec.size());

    // 异步请求pr得分
    uint32_t batch_size = 18; //服务每个集群部署了18个实例
    uint32_t multi_count = (uint32_t)psgSampleVec.size() / batch_size + 1;
    
    for (size_t mi = 0; mi < multi_count; ++mi) { 
        uint32_t start_idx = mi * batch_size;
        
        vector<uint32_t> idxVec;
        vector<HttpResponsePtr> responsePtrVec;
        for (uint32_t idx = start_idx; idx < start_idx + batch_size && idx < psgSampleVec.size(); ++idx) {
            string title = psgSampleVec[idx]->mTitle; 
            string passage = psgSampleVec[idx]->mPsg;
            
            HttpRequestPtr httpRequest(new QdRequest(query, title, passage));
            HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
            if (response) {
                responsePtrVec.push_back(response);
                idxVec.push_back(idx);
            }
        }
        
        for (size_t i = 0; i < responsePtrVec.size(); ++i) {
            if (!responsePtrVec[i])
                continue;
            ResultPtr result(new QdResult);
            if (!responsePtrVec[i]->GetResult(result)) {
                AGG_LOG(ERROR, "get pr score failed.");
                continue;
            }

            // to do:访问失败的比例，以及访问失败和原来的混用会不会有问题
            QdResultPtr qdResult = std::dynamic_pointer_cast<QdResult>(result);
            psgSampleVec[idxVec[i]]->mPR = qdResult->mScore; 
        }
    }
}


void RankProcessor::FilterRequestQtc(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter){
    size_t batch = 4;
    vector<vector<LLMPsgPtr>> psgSampleVec;
    vector<HttpResponsePtr> qtcRespVec;
    vector<LLMPsgPtr> cands;
	// 异步请求qtc得分
	for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
		for (size_t j = 0; j < psgVec.size(); ++j) {
			LLMPsgPtr curPsg = psgVec[j];
			if(!NeedFilter || curPsg->mPR >= 0.1){
				cands.emplace_back(curPsg);
			}
			if (cands.size() >= batch) {
				BatchRequestQTC(query, cands, psgSampleVec, qtcRespVec);
				cands.clear();
			}
		}
	}
	if (! cands.empty()) {
		BatchRequestQTC(query, cands, psgSampleVec, qtcRespVec);
		cands.clear();
	}

	// 异步获取qtc得分
	for (size_t i=0; i<psgSampleVec.size(); ++i) {
		vector<double> qtcScores = GetQTC(qtcRespVec[i]);
		vector<LLMPsgPtr>& psgSample = psgSampleVec[i];
		if (qtcScores.size() == psgSample.size()) {
			for (size_t j=0; j<psgSample.size(); ++j) {
				if(false && qtcScores[j] < 0.1){
					psgSample[j]->mPR = 0.0;
					psgSample[j]->mQtc = 0.0;
				}else{
					psgSample[j]->mQtc = qtcScores[j];
				}
			}
		}
	}
}

void RankProcessor::GetScores(std::vector<LLMDocPtr>& LLMDocPtrQueue)
{
    // 构造请求串并请求，获取段落级别的qtc、pr分数
    //std::string query = StripQueryLine(AggContextHelper::GetRawQuery(mAggContext));
    string query = AggContextHelper::GetRawQuery(mAggContext);
    if (!query.empty())
        query = StringTool::ReplaceStr(query, "\n", "");

    AGG_LOG(ERROR, "query=%s", query.c_str());
	int hashQueryNum = (int)HashAlgorithm::HashString64(query.c_str());
    
    string isPrNew;
    mAggContext->GetRequestParam("is_new_pr", isPrNew);

    if (isPrNew != "0") {
		FilterRequestQtc(LLMDocPtrQueue, query, false);
        int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
		FilterRequestPrNew(LLMDocPtrQueue, query, true);
        int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(ERROR, "Wait PR COST:%d", end_time - start_time);
    } else {

	if(hashQueryNum % 2 == 1){
		// 异步请求qtc、pr得分
		FilterRequestQtc(LLMDocPtrQueue, query, false);
		FilterRequestPr(LLMDocPtrQueue, query, true);
		
	}else{
		FilterRequestPr(LLMDocPtrQueue, query, false);
		FilterRequestQtc(LLMDocPtrQueue, query, true);
	}

    }

	for (size_t i=0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
		for (size_t j = 0; j < psgVec.size(); ++j) {
			LLMPsgPtr curPsg = psgVec[j];
			AGG_LOG(DEBUG, "recall=%s, pr=%lf, qtc=%lf, query=%s, title=%s, psg=%s",
				LLMDocPtrQueue[i]->mRecall.c_str(), curPsg->mPR, curPsg->mQtc,
				query.c_str(), curPsg->mTitle.c_str(), curPsg->mPsg.c_str());
		}
	}
}

void RankProcessor::BatchRequestQTC(const std::string& query,
        std::vector<LLMPsgPtr>& cands,
        std::vector<std::vector<LLMPsgPtr>>& psgSampleVec,
        std::vector<HttpResponsePtr>& qtcRespVec)
{
    // 获取词表映射
    AggDataWrapper dataWrapper("qc_bert_token"); 
    QcBertTokenData * qcBertTokenData = dataWrapper.Get<QcBertTokenData>();
    if (qcBertTokenData == nullptr) {
        AGG_LOG(ERROR, "get bert tokenizer dict failed!");
        return;
    }
    const std::map<std::string, int>& wordIdInBertQC = qcBertTokenData->GetData();
    std::vector<std::string> qtc_request_dat_vec;
    for (size_t i=0; i<cands.size(); ++i) {
        std::string t = cands[i]->mTitle;
        std::string c = cands[i]->mPsg;
        std::string single_qtc_request_dat;
        QtcExtractor::extractQtcStdkv(wordIdInBertQC, query, t, c, single_qtc_request_dat);
        qtc_request_dat_vec.emplace_back(single_qtc_request_dat);
    }
    std::string qtc_request_dat = autil::StringUtil::toString(qtc_request_dat_vec, "\n");
    
    // batch请求qtc、pr
    psgSampleVec.emplace_back(cands);
    qtcRespVec.emplace_back(RequestQTC(qtc_request_dat));
}

void RankProcessor::BatchRequestPR(const std::string& query,
        std::vector<LLMPsgPtr>& cands,
        std::vector<std::vector<LLMPsgPtr>>& psgSampleVec,
        std::vector<HttpResponsePtr>& prRespVec)
{
    // 获取词表映射
    AggDataWrapper dataWrapper("qc_bert_token"); 
    QcBertTokenData * qcBertTokenData = dataWrapper.Get<QcBertTokenData>();
    if (qcBertTokenData == nullptr) {
        AGG_LOG(ERROR, "get bert tokenizer dict failed!");
        return;
    }
    const std::map<std::string, int>& wordIdInBertQC = qcBertTokenData->GetData();
    std::vector<std::string> pr_request_dat_vec;
    for (size_t i=0; i<cands.size(); ++i) {
        std::string t = cands[i]->mTitle;
        std::string c = cands[i]->mPsg;
        std::string single_pr_request_dat;
        QtcExtractor::extractPrStdkv(wordIdInBertQC, query, t, c, single_pr_request_dat);
        pr_request_dat_vec.emplace_back(single_pr_request_dat);
    }
    std::string pr_request_dat = autil::StringUtil::toString(pr_request_dat_vec, "\n");

    // batch请求qtc、pr
    psgSampleVec.emplace_back(cands);
    prRespVec.emplace_back(RequestPr(pr_request_dat));
}

HttpResponsePtr RankProcessor::RequestQTC(std::string& qtc_request_dat)
{
    QcFeatureRequestPtr qcFeatureRequest = std::make_shared<QcFeatureRequest>(mAggContext);
    qcFeatureRequest->InitRequestStr(qtc_request_dat);
    AggHttpWrapper wrapper(mAggContext, mQTCNode);
    //AGG_LOG(DEBUG, "qtc mRequestStr=%s", qcFeatureRequest->GetRequestStr().c_str());
    return wrapper.AsyncRequest(qcFeatureRequest);
}

std::vector<double> RankProcessor::GetQTC(HttpResponsePtr httpResponse)
{
    std::vector<double> empty;
    if (!httpResponse) {
        AGG_LOG(ERROR, "get qtc httpResponse failed!");
        return empty;
    }
    ResultPtr result(new QcFeatureResult);
    if (!httpResponse->GetResult(result)) {
        AGG_LOG(ERROR, "get qtc httpResponse result failed!");
        return empty;
    }
    QcFeatureResultPtr qcFeatureResult = std::dynamic_pointer_cast<QcFeatureResult>(result);
    std::vector<float> qtc_scores = qcFeatureResult->GetQcFeature()[0];
    std::vector<double> qtc_scores_double(qtc_scores.begin(), qtc_scores.end());
    return qtc_scores_double;
}

HttpResponsePtr RankProcessor::RequestPr(std::string& pr_request_dat)
{
    QcFeatureRequestPtr qcFeatureRequest = std::make_shared<QcFeatureRequest>(mAggContext);
    qcFeatureRequest->InitRequestStr(pr_request_dat);
    AggHttpWrapper wrapper(mAggContext, mPrNode);
    //AGG_LOG(DEBUG, "pr mRequestStr=%s", qcFeatureRequest->GetRequestStr().c_str());
    return wrapper.AsyncRequest(qcFeatureRequest);
}

std::vector<double> RankProcessor::GetPr(HttpResponsePtr httpResponse)
{
    std::vector<double> empty;
    if (!httpResponse) {
        AGG_LOG(ERROR, "get pr httpResponse failed!");
        return empty;
    }
    ResultPtr result(new QcFeatureResult);
    if (!httpResponse->GetResult(result)) {
        AGG_LOG(ERROR, "get pr httpResponse failed!");
        return empty;
    }
    QcFeatureResultPtr qcFeatureResult = std::dynamic_pointer_cast<QcFeatureResult>(result);
    std::vector<float> pr_scores = qcFeatureResult->GetQcFeature()[0];
    std::vector<double> pr_scores_double(pr_scores.begin(), pr_scores.end());
    return pr_scores_double;
}

void RankProcessor::RequestAnswerLocateModel(vector<LLMDocPtr> &LLMDocPtrQueue, uint32_t topN)
{
    string query = AggContextHelper::GetRawQuery(mAggContext);
    if (query.empty())
        return;

    vector<uint32_t> idxVec;
    vector<HttpResponsePtr> alResponsePtrVec;
    for (uint32_t idx = 0; idx < LLMDocPtrQueue.size() && idx<topN; ++idx) {
        string title = LLMDocPtrQueue[idx]->mTitle;
        string body = LLMDocPtrQueue[idx]->mBody;

        if (body.size() > 0) {
            HttpResponsePtr alResponse = AsyncRequestSummary(query, title, body);
            if (alResponse) {
                alResponsePtrVec.emplace_back(alResponse);
                idxVec.emplace_back(idx);
            }
        }
    }

    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    for (size_t i = 0; i < alResponsePtrVec.size(); ++i) {
        if (!alResponsePtrVec[i])
            continue;
        string summaryNew;
        auto &docPtr = LLMDocPtrQueue[idxVec[i]];
        ResultPtr result = ResultPtr(new LLMResult());
        if (alResponsePtrVec[i]->GetResult(result)) {
            LLMResultPtr llmResultPtr = std::dynamic_pointer_cast<LLMResult>(result);
            string alResult = llmResultPtr->mContent;
            // AGG_LOG(ERROR, "get alResult succeed, query=%s, title=%s, passage=%s, alResult=%s", query.c_str(), docPtr->mTitle.c_str(), docPtr->mBody.c_str(), alResult.c_str());

            ReCatAnswer(alResult, query, docPtr->mTitle, docPtr->mBody, summaryNew);
            docPtr->mSummary = summaryNew;
            if (summaryNew.size() == 0) {
                docPtr->mSummary = docPtr->mBody;
            }
        }
        else {
            summaryNew = "fail_req";
            AGG_LOG(ERROR, "get alResult failed, query=%s, title=%s, passage=%s", query.c_str(), docPtr->mTitle.c_str(), docPtr->mBody.c_str());
            docPtr->mSummary = docPtr->mBody;
            continue;
        }
    }
    // 未请求AL的doc，用body填充summary
    for (uint32_t idx = 0; idx < LLMDocPtrQueue.size(); ++idx) {
        auto &docPtr = LLMDocPtrQueue[idx];
        string summaryNew = docPtr->mSummary;
        if (summaryNew.size() == 0) {
            docPtr->mSummary = docPtr->mBody;
        }
    }
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "Wait AL COST:%d", end_time - start_time);

    return;

}

HttpResponsePtr RankProcessor::AsyncRequestSummary(const std::string& query, const std::string& title, const std::string& passage)
{
    string doc = title + "\n" + passage;
    string indexed_passage = "";
    vector<string> sentences;
    vector<pair<int, int> > sent_group;
    GetIndexedPassage(doc, indexed_passage, sentences, sent_group);
    std::string prompt = "请从正文中选择能回答问题的句子，输出句子的序号；若无法回答，输出“无答案”。问题：" + query + "；正文：" + indexed_passage;
    std::string prompt_str = "<|im_start|>▁user\n"+ prompt + "<|im_end|>\n<|im_start|>▁assistant\n";

    LLMRequestPtr llmRequest = LLMRequestPtr(new LLMRequest(mAggContext, mAnswerLocateLLMName));
    llmRequest->SetApiKey(mApiKey);
    llmRequest->SetPrompt(prompt_str);
    llmRequest->SetMaxNewTokens(100);
    llmRequest->SetTemperature(0.1);
    llmRequest->SetOriQuery(AggContextHelper::GetRawQuery(mAggContext));
    string reqid = llmRequest->GetRequestId();
    /* AGG_LOG(ERROR, "al reqid=%s", reqid.c_str()); */
    llmRequest->SetRequestId("al_model_req");
    reqid = llmRequest->GetRequestId();
    /* AGG_LOG(ERROR, "al reqid=%s", reqid.c_str()); */

    // AGG_LOG(ERROR, "al prompt=%s", prompt.c_str());

    auto httpWrapper = GetHttpWrapper(mAggContext, mGeneralLLMSyncNode);
    HttpResponsePtr alResponse = httpWrapper->AsyncRequest(llmRequest);

    return alResponse;
}

bool RankProcessor::ReCatAnswer(const string &alResult, const string& query, const string& title, const string& passage, string& answer) {
    if (alResult.find("无")!=string::npos) {
        answer = "无答案";
        return true;
    }

    string doc = title + "\n" + passage;
    string indexed_passage = "";
    vector<string> sentences;
    vector<pair<int, int> > sent_group;
    GetIndexedPassage(doc, indexed_passage, sentences, sent_group);
    // AGG_LOG(ERROR, "alResult=%s", alResult.c_str());

    vector<int> alNumResVec;
    try {
        vector<string> alResVec;
        alResVec = autil::StringUtil::split(alResult, "、");
        for (size_t i = 0; i<alResVec.size(); i++) {
            if (alResVec[i].find("~")!= string::npos) {
                vector<string> range_vec;
                range_vec = autil::StringUtil::split(alResVec[i], "~");
                if (range_vec.size() == 2) {
                    for (int j = std::stoi(range_vec[0]); j <= std::stoi(range_vec[1]); j++) {
                        alNumResVec.emplace_back((j));
                    }
                }
            }
            else {
                alNumResVec.emplace_back(std::stoi(alResVec[i]));
            }
        }
    }
    catch(exception& e) {
        AGG_LOG(ERROR, "alResult format error, alResult=%s", alResult.c_str());
    }

    // 获取句子粒度答案
    for (size_t i = 0; i < alNumResVec.size(); i++) {
        if (alNumResVec[i]-1 >= 0 && alNumResVec[i]-1 < int(sentences.size())) {
            answer += sentences[alNumResVec[i]-1]; // 句子粒度答案
        }
    }
    // 获取段落粒度答案
    std::set<int> st;
    for (size_t i = 0; i < alNumResVec.size(); i++) {
        for (size_t j = 0; j < sent_group.size(); j++) {
            if (alNumResVec[i] >= sent_group[j].first && alNumResVec[i] <= sent_group[j].second) {
                st.insert(j);
            }
        }
    }
    string paragraph_answer = "";
    for (auto it = st.begin(); it!= st.end(); it++) {
        for (int i = sent_group[*it].first; i <= sent_group[*it].second; i++) {
            paragraph_answer += sentences[i-1];
        }
        paragraph_answer += "\n";
    }
    paragraph_answer.erase(0, paragraph_answer.find_first_not_of(" \n\r\t"));  // strip left
    paragraph_answer.erase(paragraph_answer.find_last_not_of(" \n\r\t") + 1); // strip right
    answer = paragraph_answer; // 段落粒度答案
    return true;
}

bool RankProcessor::GetIndexedPassage(const string &doc, string &indexed_passage, vector<string> &sentences, vector<pair<int, int> > &sent_group) {
    stringstream ss(doc);
    string psg;
    vector<string> passages;
    while (getline(ss, psg, '\n')) {
        passages.push_back(psg);
    } // 按照换行符划分

    int cur_cnt = 0;
    for(size_t idx = 0; idx < passages.size(); ++idx) {
        const string& passage = passages[idx];
        if(passage.empty()) {
            continue;
        }
        else {
            size_t start = 0, end = 0;
            size_t offset = 0;
            size_t char_num[6] = {4,6,6,3,3,1};
            int sent_cnt = 0;

            while (end != string::npos) {
                vector<size_t> end_pos;
                size_t end1 = passage.find("?”", start);
                size_t end2 = passage.find("？”", start);
                size_t end3 = passage.find("。”", start);
                size_t end4 = passage.find("。", start);
                size_t end5 = passage.find("？", start);
                size_t end6 = passage.find("?", start);

                end_pos.emplace_back(end1);
                end_pos.emplace_back(end2);
                end_pos.emplace_back(end3);
                end_pos.emplace_back(end4);
                end_pos.emplace_back(end5);
                end_pos.emplace_back(end6);

                auto min_it = min_element(end_pos.begin(),end_pos.end());
                end = *min_it;

                size_t min_idx = distance(std::begin(end_pos), min_it);
                offset = char_num[min_idx];

                if (end != start && end != string::npos) {
                    string tmp = passage.substr(start, end - start +offset);
                    tmp.erase(0, tmp.find_first_not_of(" \n\r\t"));  // strip left
                    tmp.erase(tmp.find_last_not_of(" \n\r\t") + 1); // strip right
                    if(tmp.length() > 0) {
                        sentences.push_back(tmp);
                        sent_cnt += 1;
                    }
                }
                if (end != string::npos) {
                    start = end + offset;
                }
            }
            if (start < passage.size()) {
                string tmp = passage.substr(start);
                tmp.erase(0, tmp.find_first_not_of(" \n\r\t"));  // strip left
                tmp.erase(tmp.find_last_not_of(" \n\r\t") + 1); // strip right
                if(tmp.length() > 0) {
                    sentences.push_back(tmp);
                    sent_cnt += 1;
                }
            }
            sent_group.emplace_back(make_pair(cur_cnt+1, cur_cnt+sent_cnt));
            cur_cnt += sent_cnt;
        }
    }

    int passage_len = 0;
    indexed_passage = "";
    for(size_t idx = 0; idx < sentences.size(); idx++){
        string new_sent = to_string(idx+1) + "：" + sentences[idx] + "\n";
        passage_len += GetUtf8LetterNumber(new_sent);
        if (passage_len<1800) {
            indexed_passage += new_sent;
        }
        else {
            break;
        }
    }
    indexed_passage = AggWebStringTool::FilterInvalidChar(indexed_passage);

    return true;
}

int RankProcessor::GetUtf8LetterNumber(const string &s)
{
    int j = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        if ((s[i] & 0xc0) != 0x80) j++;
    }
    return j;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(aigc)::RankProcessor);

