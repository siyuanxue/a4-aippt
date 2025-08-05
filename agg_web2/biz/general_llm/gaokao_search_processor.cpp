#include <agg_web2/biz/general_llm/gaokao_search_processor.h>
#include <agg_web2/data/value_data.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/helper/agg_context_helper.h>
#include <jsoncpp/json/json.h>
#include <agg2/helper/time_helper.h>

using namespace std;
AGG_BEGIN_NAMESPACE(agg_web2)
AGG_LOG_SETUP(agg_web2, GaoKaoSearchProcessor);
AGG_LOG_SETUP_WITH_NAME(displayLog, displayLog, GaoKaoSearchProcessor);

//const static string PARAM_GENERAL_LLM_SYNC_NODE = "general_llm_sync_server";
//const static string PARAM_ANSWER_LOCATE_LLM_MODEL_NAME = "answer_locate_llm_name";
const static string PARAM_AGG_HOST = "agg_host";

BEGIN_DEFINE_SERIALIZATION_FUNC(GaoKaoSearchProcessor)
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
    //ar & BOOST_SERIALIZATION_NVP(mGeneralLLMSyncNode);
    //ar & BOOST_SERIALIZATION_NVP(mAnswerLocateLLMName);
END_DEFINE_SERIALIZATION_FUNC(GaoKaoSearchProcessor)

bool GaoKaoSearchProcessor::DoInit(const String2String &params)
{
    GeneralLLMProcessor::DoInit(params);

    //PARSE_PROCESSOR_PARAM(params, PARAM_GENERAL_LLM_SYNC_NODE, mGeneralLLMSyncNode);
    //PARSE_PROCESSOR_PARAM(params, PARAM_ANSWER_LOCATE_LLM_MODEL_NAME, mAnswerLocateLLMName);
    
    std::string tmpHost;
    PARSE_PROCESSOR_PARAM(params, PARAM_AGG_HOST,tmpHost);
    std::string::size_type pos = tmpHost.find(":");
    if (pos != std::string::npos) {
        mAggHost = tmpHost.substr(0,pos);
    }
    else
    {
        mAggHost = tmpHost;
    }
    AGG_LOG(INFO, "DoInit GaoKaoSearchProcessor Succ.");
    return true;
}

bool GaoKaoSearchProcessor::RegisterMethods() {
    return true;
}

BaseProcessor* GaoKaoSearchProcessor::DoClone() {
    return new GaoKaoSearchProcessor(*this);
}

void GaoKaoSearchProcessor::DoDestroy() {
    delete this;
}

std::string GaoKaoSearchProcessor::GetFormatTimeStr(int32_t Currenttime)
{
    if (Currenttime < 1600000000) {
        return std::string("");
    }

    char outStr[256];
    time_t t = Currenttime;
    struct tm *tmp = localtime(&t);
    strftime(outStr, sizeof(outStr), "%Y-%m-%d %H:%M:%S", tmp);
    return std::string(outStr);
}

int32_t GaoKaoSearchProcessor::GetFormatTimeInt32(std::string timestr)
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

int GaoKaoSearchProcessor::DoProcess(AggContext * aggContext)
{
    mDebugInfoPtr = LLMDebugInfoPtr(new LLMDebugInfo);
    string query = AggContextHelper::GetRawQuery(aggContext);
    if (!query.empty())
        query = StringTool::ReplaceStr(query, "\n", "");
    
    // step 1, 获取agg web和news的结果
    AggWebResultPtr aggResultPtr = GetQueryResult(aggContext, query);
    
    // step 2, 输出结果
    RequestAggFormatResult(aggContext, aggResultPtr);
    
    return PR_SUCCESS;
}

AggWebResultPtr GaoKaoSearchProcessor::GetQueryResult(AggContext *aggContext, std::string &query)
{
    HttpResponsePtr aggResponse = AsyncRequestAgg(aggContext, query); // 异步请求agg
    HttpResponsePtr newsResponse = AsyncRequestCommon(aggContext, query, "news");
    
    AggWebResultPtr aggResultPtr = nullptr;
    
    if (!aggResponse) {
        AGG_LOG(ERROR,"agg web request failed");
        return aggResultPtr;
    }

    ResultPtr result(new AggWebResult(mDebugInfoPtr, false, false));
    aggResultPtr = dynamic_pointer_cast<AggWebResult>(result);
    aggResultPtr->SetGaoKaoMode(); //高考定制的agg结果解析模式

    if (!aggResponse->GetResult(result)) {
        AGG_LOG(ERROR, "agg web response failed");
        return nullptr; 
    }
    //aggResultPtr = dynamic_pointer_cast<AggWebResult>(result);
        
    if (newsResponse) {
        ResultPtr newsResult(new CommonResult("news"));
        if (newsResponse->GetResult(newsResult)) {
            CommonResultPtr newsPtr = dynamic_pointer_cast<CommonResult>(newsResult);
            vector<LLMDocPtr> newsVec;
            for (size_t i = 0; i < newsPtr->mDocs.size(); ++i) {
                //if (newsPtr->mDocs[i].mQtc < 0.5)
                //    continue;
                
                LLMDocPtr docPtr(new LLMDoc(newsPtr->mDocs[i].mTitle,
                                            newsPtr->mDocs[i].mBody,
                                            "news " + newsPtr->mDocs[i].mUrl, i,
                                            newsPtr->mDocs[i].mQtc, 2.0,
                                            newsPtr->mDocs[i].mPr,
                                            newsPtr->mDocs[i].mPublishTime, true));
                //docPtr->mRankScore += 2;
                newsVec.emplace_back(docPtr);
                
                //if (newsVec.size() >= 3) {
                //    break;
                //}
            }

            AGG_LOG(INFO, "Add News Result[%d]", (int32_t)newsVec.size());
            if (newsVec.size() > 0) {
                aggResultPtr->mDocPtrVec.insert(aggResultPtr->mDocPtrVec.end(), newsVec.begin(), newsVec.end());
            }
        }
    }
    
    if (!aggResultPtr || aggResultPtr->mDocPtrVec.size() == 0) {
        return aggResultPtr;
    }

    sort(aggResultPtr->mDocPtrVec.begin(), aggResultPtr->mDocPtrVec.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });
    
    DeDuplicate(aggResultPtr);
    
    double filter_qtc = 0.5;
    std::string filter_qtc_str;
    mAggContext->GetRequestParam("filter_qtc", filter_qtc_str);
    if (!filter_qtc_str.empty()) {
        filter_qtc = autil::StringUtil::strToDoubleWithDefault(filter_qtc_str.c_str(), 0.5);
    }

    vector<RankNode> rankVec;
    vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
    for (size_t i = 0; i < docPtrVec.size(); ++i) {
        if (docPtrVec[i]->mUrl.find("news_uchq") == 0 && docPtrVec[i]->mAggRank < 3) {//前3位的资讯卡片，强行保护
            if (docPtrVec[i]->mQtc < 1.0) {
                docPtrVec[i]->mQtc = 1.0;
            }
        }

        if (docPtrVec[i]->mBody.empty() || docPtrVec[i]->mQtc < filter_qtc) {
            continue;
        }
        
        RankNode rankNode;
        rankNode.mDocPtr = docPtrVec[i];
        rankNode.mRankScore = docPtrVec[i]->mRankScore;
        rankNode.mPr = docPtrVec[i]->mPR;

        //if (docPtrVec[i]->mUrl.find("wenda_selected ") != std::string::npos) {
        //    rankNode.mNeedRank = false;
        //}
        
        // 前3位的资讯卡片，强行保护
        if (docPtrVec[i]->mUrl.find("news_uchq") == 0 && docPtrVec[i]->mAggRank < 3) {
            rankNode.mRankScore += 5000.0;
        }

        rankVec.push_back(rankNode);
    }

    // 重置PR分
    ReSetPR(rankVec, aggResultPtr->mQuery);
    
    ReRankByHost(rankVec);
    
    vector<RankNode> newRankVec;
    for (size_t i=0; i < rankVec.size(); ++i) { //不相关截断
        double pr = rankVec[i].mPr;
        double qtc = rankVec[i].mDocPtr->mQtc;
        if (pr < 0.1 || (qtc <= 1.7 && pr < 0.2) || (qtc <= 0.7 && pr <= 0.35)) {
            rankVec[i].mRankScore -= 10000.0;
        }
       
        // 时间格式转换
        if (rankVec[i].mDocPtr->mUrl.find("news_uchq") == 0 && rankVec[i].mDocPtr->mPublishTime != "") {
            int32_t tmp;
            if (autil::StringUtil::strToInt32(rankVec[i].mDocPtr->mPublishTime.c_str(), tmp)) {
                rankVec[i].mDocPtr->mPublishTime = GetFormatTimeStr(tmp); 
            }
        }

        int32_t curTime = (int32_t)TimeHelper::GetSystemCurrentTimeSec();
        int32_t docTime = GetFormatTimeInt32(rankVec[i].mDocPtr->mPublishTime);
        int32_t days = (curTime - docTime) / 3600 / 24;

        // 时效query下，对30天内结果
        if (aggResultPtr->mFresh > 0 && days < 30) { //30天内结果前提权 
            rankVec[i].mRankScore += 2000.0;
        }

        // 时效query下，对360天内结果
        if (aggResultPtr->mFresh > 0 && days < 360) { // 360天结果调权
            rankVec[i].mRankScore += (1 + (360 - days) / 180.0) * (0.3 + 0.7 * pr) * 5.0;
        }

        if (rankVec[i].mRankScore > 0) {
            newRankVec.push_back(rankVec[i]);
        }
    }
    rankVec = newRankVec;
    
    // 最终顺序
    auto cmp = [](RankNode& x, RankNode& y) {
        return x.mRankScore > y.mRankScore;
    };
    sort(rankVec.begin(), rankVec.end(), cmp);

    // 请求答案定位模型
    RequestAnswerLocateModel(rankVec, aggResultPtr->mQuery, 6);
    
    vector<LLMDocPtr> docPtrVecFinal;
    for (size_t i=0; i < rankVec.size(); ++i) {
        auto &docPtr = rankVec[i].mDocPtr;
        docPtr->mPR = rankVec[i].mPr;
        docPtr->mRankScore = rankVec[i].mRankScore;
        docPtr->mDebugInfo = rankVec[i].toString();
        
        int32_t curTime = (int32_t)TimeHelper::GetSystemCurrentTimeSec();
        int32_t docTime = GetFormatTimeInt32(docPtr->mPublishTime);
        int32_t days = (curTime - docTime) / 3600 / 24;
        docPtr->mDebugInfo += ", days: " + autil::StringUtil::toString(days);
        
        if (docPtr->mUrl.find("news_uchq") == 0 && docPtr->mAggRank < 3 && docPtr->mSummary == "无答案") {
            docPtr->mSummary = "";
        }

        if (docPtr->mSummary == "无答案") {
            docPtr->mRankScore = 0.0;
        }

        if (docPtr->mRankScore < 10.0 || docPtr->mPR < 0.1 || (docPtr->mQtc <= 1.7 && docPtr->mPR < 0.2) || (docPtr->mQtc <= 0.7 && docPtr->mPR <= 0.35) || (docPtr->mQtc <= 0.9 && docPtr->mQt <= 0.9)) {
            docPtr->mRankScore = 0.0;
        } else {
            docPtrVecFinal.push_back(docPtr);
        }
    }

    aggResultPtr->mDocPtrVec = docPtrVecFinal;
        
    DeDuplicate(aggResultPtr);

    return aggResultPtr;
}

void GaoKaoSearchProcessor::ReRankByHost(std::vector<RankNode>& rankVec)
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
    for (size_t i = 0; i < rankVec.size(); ++i) {
        int32_t host_level = 0;
        string url = rankVec[i].mDocPtr->mUrl;
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
        
        AGG_LOG(ERROR, "Raw Host[%s][%d]", host.c_str(), host_level);
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
            rankVec[i].mRankScore += 1000;
            rankVec[i].mDocPtr->mHostLevel = 1;
            AGG_LOG(ERROR, "Hit Good Host[%s]", url.c_str());
        }
        if (host_level < 0) {
            rankVec[i].mRankScore -= 1000;
        }
    }
}

HttpResponsePtr GaoKaoSearchProcessor::AsyncRequestAgg(AggContext *aggContext, std::string query)
{
    AggWebRequest * aggRequest = new AggWebRequest(aggContext, query);
    aggRequest->SetHttpMethod(HM_GET);
    aggRequest->SetHost(mAggHost);
    HttpRequestPtr httpRequest(aggRequest);
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "agg_web_server/agg");
    HttpResponsePtr aggResponse = wrapper->AsyncRequest(httpRequest);
    
    string aggRequestStr;
    aggRequest->GetRequestStr(aggRequestStr);
    AGG_LOG(INFO,"agg web query=%s, request=%s", query.c_str(), aggRequestStr.c_str());
    return aggResponse;
}

HttpResponsePtr GaoKaoSearchProcessor::AsyncRequestCommon(AggContext *aggContext, std::string query, const std::string& type)
{
    HttpRequestPtr httpRequest(new CommonRequest(query, type));
    string server_name;
    if (type == "news") {
        server_name = "news_server";
    }
    if (server_name.empty()) {
        return nullptr;
    }

    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, server_name);
    HttpResponsePtr aggResponse = wrapper->AsyncRequest(httpRequest);
    if (NULL == aggResponse) {
        AGG_LOG(ERROR, "AsyncRequestCommon %s failed", type.c_str());
    }
    return aggResponse;
}

void GaoKaoSearchProcessor::RequestAggFormatResult(AGG::AggContext *aggContext, AggWebResultPtr aggResultPtr)
{
    std::string uid;
    std::string sessionId;
    std::string reqid;
    std::string bucket;
    mAggContext->GetRequestParam("uid", uid);
    mAggContext->GetRequestParam("sessionid", sessionId);
    aggContext->GetRequestParam("reqid", reqid);
    aggContext->GetRequestParam("bucket", bucket);
    AGG_LOG(ERROR, "bucket=%s", bucket.c_str());
    bool onlyGoodHost = false;
    if (bucket.find("onlygoodhost=1") != std::string::npos) {
        onlyGoodHost = true;
    }

    Json::Value root;

    Json::Value communication;
    communication["uid"] = uid;
    communication["sessionid"] = sessionId;
    communication["reqid"] = reqid;
    communication["resid"] = 0;
    root["communication"] = communication;

    //root["is_med"] = false;
    root["query"] = AggContextHelper::GetRawQuery(mAggContext);
    
    root["target"] = "respond";
    root["version"] = 1;

    uint64_t sessionStartTime = mContext->GetStartProcessTime();
    root["timestamp"] = autil::StringUtil::toString<uint64_t>(sessionStartTime);

    if (!aggResultPtr || aggResultPtr->mDocPtrVec.empty()) {
        root["error_code"] = 10001;
        root["error_msg"] = "agg result get failed";
    } else {
        root["error_code"] = 0;
        root["error_msg"] = "";
    }

    Json::Value search_doc = Json::Value(Json::arrayValue);
    if (aggResultPtr) {
        if (! aggResultPtr->mDocPtrVec.empty()) {
            const vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
            bool hitGoodHost = false;
            for(size_t i = 0; i < docPtrVec.size(); ++i) {
                auto docPtr = docPtrVec[i];
                if (docPtr->mHostLevel>0) {
                    hitGoodHost = true;
                    break;
                }
            }

            int docNum = 0;
            for(size_t i = 0; i < docPtrVec.size() && docNum < 6; ++i) {
                auto docPtr = docPtrVec[i];
                if (onlyGoodHost && hitGoodHost && docPtr->mHostLevel==0) { // 命中优质站的query仅保留优质站数据
                    continue;
                }
                search_doc.append(Json::Value());
                Json::Value & item = search_doc[search_doc.size() - 1];
                item["title"] = AggWebStringTool::FilterInvalidChar(docPtr->mTitle);
                item["body"] = AggWebStringTool::FilterInvalidChar(docPtr->mBody);
                item["link"] = docPtr->mUrl;
                item["source"] = "web";
                item["hostname"] = docPtr->mHostname;
                item["correlation"] = docPtr->mQtc;
                item["passage_ranking_label"] = docPtr->mPR;
                item["rank_score"] = docPtr->mRankScore;
                item["publishTime"] = docPtr->mPublishTime;
                item["rankpos"] = docPtr->mAggRank;
                //item["kg_scores"] = docPtr->mKgScoreStr;
                item["debug_info"] = docPtr->mDebugInfo;
                if (docPtr->mSummary.size() == 0) {
                    docPtr->mSummary = docPtr->mBody;
                }
                item["summary"] = AggWebStringTool::FilterInvalidChar(docPtr->mSummary);
                aggResultPtr->mHitGoodHost += docPtr->mHostLevel;
                docNum += 1;
            }
        }
    }
    
    root["search_doc"] = search_doc;
    root["fresh"] = 0;
    root["is_news"] = 0;
    root["hit_good_host"] = 0;
    if (aggResultPtr) {
        root["fresh"] = aggResultPtr->mFresh;
        root["hit_good_host"] = aggResultPtr->mHitGoodHost;
        if (aggResultPtr->isNewsSC && aggResultPtr->isNewsFlag) {
            root["is_news"] = 1;
        }
    }
    
    GetDebugInfoPostfix(aggResultPtr);
    root["debug_info"] = mReferInfo;

    Json::FastWriter writer;
    std::string resContent = writer.write(root);

    AGG_LOG(INFO, "resContent=%s", resContent.c_str());

    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

void GaoKaoSearchProcessor::GetDebugInfoPostfix(AggWebResultPtr aggResultPtr)
{
    if (! aggResultPtr) {
        return;
    }
    const vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
    if (mReferInfo.empty()) {
        mReferInfo = "[参考来源]\n";
    } else {
        mReferInfo += "\n\n[改写后参考来源]\n";
    }

    int doc = 0;
    for(size_t i = 0; i < docPtrVec.size(); ++i) {
        auto docPtr = docPtrVec[i];
        if (doc > 0) {
            mReferInfo += "\n";
        }
        ++ doc;
        mReferInfo += std::to_string(doc) + ". " + docPtr->GetDebugString();
    }
    mReferInfo += "\nmIsQaTrigger:" + to_string(aggResultPtr->mIsQaTrigger);
    mReferInfo += "\nmHitGaokoaSC:" + to_string(aggResultPtr->mHitGaokoaSC);
}

AGG_END_NAMESPACE(agg_web2)
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(medical)::GaoKaoSearchProcessor)

