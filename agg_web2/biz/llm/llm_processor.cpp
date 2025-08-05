#include <agg_web2/biz/llm/llm_processor.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg2/util/string_tool.h>
#include <autil/StringUtil.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg_web2/data/key_value_data.h>
#include <agg_web2/data/value_data_host.h>
#include <agg_web2/common/define.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/wrapper/agg_data_wrapper.h>
#include <autil/StringUtil.h>
#include <A4/util/hash_algorithm.h>
#include <agg_web2/helper/general_search_helper.h>
#include <agg2/upstream/llm_result.h>
#include <agg2/upstream/llm_request.h>

using namespace std;
AGG_BEGIN_NAMESPACE(agg_web2)
AGG_LOG_SETUP(agg_web2, LLMProcessor);
AGG_LOG_SETUP_WITH_NAME(displayLog, displayLog, LLMProcessor);


BEGIN_DEFINE_SERIALIZATION_FUNC(LLMProcessor)
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
END_DEFINE_SERIALIZATION_FUNC(LLMProcessor)

bool LLMProcessor::DoInit(const String2String &params)
{
    return true;
}

void LLMProcessor::DoRegisterMetric(AmonitorRegister *monitorRegister)
{
}

BaseProcessor* LLMProcessor::DoClone()
{
    return new LLMProcessor(*this);
}

void LLMProcessor::DoDestroy()
{
    delete this;
}

int LLMProcessor::DoProcess(AggContext* aggContext)
{
    INIT_CONTEXT(aggContext)

    std::vector<LLMDocPtr> hitQueueFinal;
    mAggContext->GetValue("FINAL_RESULT", hitQueueFinal);
    
    std::vector<LLMDocPtr> hitAgg, hitIndex;
    mAggContext->GetValue("RAWDATA_AGG", hitAgg); 
    mAggContext->GetValue("RAWDATA_AIGC", hitIndex);

    AGG_LOG(ERROR, "SIZE:%d, %d, %d", (int32_t)hitQueueFinal.size()
            , (int32_t)hitAgg.size(), (int32_t)hitIndex.size());
    
    set<string> uniqUrl;
    for (size_t i = 0; i < hitQueueFinal.size(); ++i) {
        string key = hitQueueFinal[i]->mRecall + "\t" + hitQueueFinal[i]->mUrl;
        uniqUrl.insert(key);
    }
    
    std::vector<LLMDocPtr> notSelected;
    for (auto docPtr: hitAgg) {
        string key = docPtr->mRecall + "\t" + docPtr->mUrl;
        if (uniqUrl.find(key) == uniqUrl.end()) {
            notSelected.push_back(docPtr);
        }
    }
    for (auto docPtr: hitIndex) {
        string key = docPtr->mRecall + "\t" + docPtr->mUrl;
        if (uniqUrl.find(key) == uniqUrl.end()) {
            notSelected.push_back(docPtr);
        }
    }

    // 1, 挑选数据
    SelectDocs(hitQueueFinal);
    
    // 2, GenSummary, LLM
    GetLLmSummary(AggContextHelper::GetRawQuery(mAggContext), hitQueueFinal); 

    // 3, Result Format 
    FormatResult(hitQueueFinal, notSelected);

    return PR_SUCCESS;
}

void LLMProcessor::SelectDocs(std::vector<LLMDocPtr>& hitQueue)
{
    // todo
    for (auto docPtr: hitQueue) {
        string url = docPtr->mUrl;
        if (url.find("baike.baidu.com") != string::npos || url.find("eol.cn") != string::npos 
                || url.find("gaokao.cn") != string::npos
                || url.find("dxsbb.com") != string::npos || url.find("gaokao.chsi.com.cn") != string::npos
                || url.find("edu.cn") != string::npos || url.find("zjzs.net")!= string::npos) {
            docPtr->mRankScore += 100.0;
        }
    }

    sort(hitQueue.begin(), hitQueue.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });

    return;
}

void LLMProcessor::FormatResult(std::vector<LLMDocPtr>& hitQueue, std::vector<LLMDocPtr>& notSelected)
{
    std::string uid;
    std::string sessionId;
    std::string reqid;
    std::string bucket;
    mAggContext->GetRequestParam("uid", uid);
    mAggContext->GetRequestParam("sessionid", sessionId);
    mAggContext->GetRequestParam("reqid", reqid);
    mAggContext->GetRequestParam("bucket", bucket);
    AGG_LOG(DEBUG, "bucket=%s", bucket.c_str());
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

    std::vector<std::string> queryIntents;
    mAggContext->GetValue("QUERY_INTENTS", queryIntents);
    root["intent"] = autil::StringUtil:: toString(queryIntents, ",");
    
    root["target"] = "respond";
    root["version"] = 1;

    uint64_t sessionStartTime = mContext->GetStartProcessTime();
    root["timestamp"] = autil::StringUtil::toString<uint64_t>(sessionStartTime);

    if (hitQueue.empty()) {
        root["error_code"] = 10001;
        root["error_msg"] = "agg result get failed";
    } else {
        root["error_code"] = 0;
        root["error_msg"] = "";
    }

    Json::Value search_doc = Json::Value(Json::arrayValue);

    int hitGoodHost = 0; //命中优质站点数量
    for(size_t i = 0; i < hitQueue.size(); ++i) {
        auto docPtr = hitQueue[i];
        if (docPtr->mHostLevel>0) {
            hitGoodHost += 1;
        }
    }

    for (size_t i = 0; i < hitQueue.size() /*&& i<10*/; ++i) {
        search_doc.append(Json::Value());
        Json::Value & item = search_doc[search_doc.size() - 1];
        auto docPtr = hitQueue[i];
        item["title"] = docPtr->mTitle;
        item["body"] = docPtr->mBody;
        item["link"] = docPtr->mUrl;
        item["source"] = "web";
        item["hostname"] = docPtr->mHostname;
        item["correlation"] = docPtr->mQtc;
        item["passage_ranking_label"] = docPtr->mPR;
        item["rank_score"] = docPtr->mRankScore;
        item["publishTime"] = docPtr->mPublishTime;
        item["rankpos"] = docPtr->mAggRank;
        item["recall"] = docPtr->mRecall;
        item["hostlevel"] = docPtr->mHostLevel;
        //item["kg_scores"] = docPtr->mKgScoreStr;
        item["debug_info"] = docPtr->mDebugInfo;
        if (docPtr->mSummary.size() == 0) {
            docPtr->mSummary = docPtr->mBody;
        }
        item["summary"] = AggWebStringTool::FilterInvalidChar(docPtr->mSummary);
    }
    
    root["search_doc"] = search_doc;
    root["fresh"] = 0;
    root["is_news"] = 0;
    root["hit_good_host"] = hitGoodHost;
    
    stringstream oss;
    oss << "not seleted:";
    for (auto docPtr: notSelected) {
        oss << docPtr->GetDebugString() << endl;
    }
    
    root["debug_info"] = oss.str();//mReferInfo;
    
    root["llm_result"] = mSummary; 

    Json::FastWriter writer;
    std::string resContent = writer.write(root);

    AGG_LOG(INFO, "resContent=%s", resContent.c_str());

    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

std::string LLMProcessor::GetLLmSummary(const std::string& query, std::vector<LLMDocPtr>& hitQueue)
{
    if (hitQueue.size() == 0)
        return "";

    stringstream ss;
    ss << "你是一个优秀的高考问答助手，下面是搜索引擎搜到的结果，请根据这些搜索结果，同时完成两个任务：\n";
    ss << "第一个任务：先对用户输入query的意图进行分类，包括主要意图、意图实体和细分意图，用json进行输出\n";
    ss << "第二个任务：基于用户的query意图、搜索资料以及意图对应的回答范式进行答案生成，需要有条理性，字数在400-600字左右。\n";
    ss << "\n搜索到的资料如下：\n";
    ss << "[\n";
    /*ss << "{\n";
    ss << "\"hostname\": \"知乎\",\n";
    ss << "\"title\": \"口腔医学专业专升本难吗？ - 知乎\",\n";
    ss << "\"body\": \"比较难\n1.千军万马过独木桥（院校选择少）\n全国范围来看，开设口腔医学本科专业的院校不在少数，但每个专科院校对口升学的本科院校缺少之又少，大多数院校可以参与专升本的数量基本不会超过三所，所以对于广大考生来说并不友好。\n2.必须潜心学习（录取分数较高）\n这里说的录取分数较高，也不绝对，全国各省份院校每年的分数线也是有波动的，但是综合所述，口腔医学专业的分数线一定远远高于其他普通专业。如果要靠运气基本不可能成功，所以还是要潜下心来好好备考。不要等到录取分数线出来，就个位数分差失之交臂。\n3.专业特殊性（招生计划不多）\n口腔医学这个专业相对于其他普通专业来说，比较特殊，属于国控专业。各省份对于国控专业的招生计划数量层次不齐，拿山东为例，多一点的院校招生计划可以达到100+，但是同期开始看，比如重庆的院校，招生计划数可能只有个位数。既然是国控专业，那么计划数总量上一定是无法和普通专业数量做比较的。\n除去以上三点，口腔医学本来的学习难度，课程安排都远远大于普通专业，如果没有本科学历，想要依靠专升本考试提升，难度还是比较大的，希望所有看到这篇文章的朋友考的全会，蒙的全对，金榜题名时，口腔名校见~\"\n}\n";*/
    
    int32_t ii = 0;
    for (auto docPtr:hitQueue) {
        ii ++;
        if (ii > 6)
            break;

        ss << "\"hostname\": \"" << docPtr->mHostname << "\",\n";
        ss << "\"title\": \"" << docPtr->mTitle << "\",\n";
        ss << "\"body\": \"" << docPtr->mBody << "\",\n";
    }

    ss << "]\n";
    ss << "用户输入：\n";
    ss << query;
    ss << "\n输出：";

    std::string prompt_str = ss.str();
    
    LLMRequestPtr llmRequest = LLMRequestPtr(new LLMRequest(mAggContext, "quarkl2-h32-gaokao-rag-sgs"));
    llmRequest->SetApiKey("Bearer QEDtobN7LA4niif70j0Yv8tN");
    llmRequest->SetContent(prompt_str);
    llmRequest->SetMaxNewTokens(4096);
    llmRequest->SetTemperature(0.1);
    llmRequest->SetOriQuery(AggContextHelper::GetRawQuery(mAggContext));
    llmRequest->SetRole("user");
    llmRequest->SetModel("quarkl2-h32-gaokao-rag-sgs");
    llmRequest->SetSessionId("12345");
    llmRequest->SetRequestId("gaokao_llm_test");

    auto httpWrapper = GetHttpWrapper(mAggContext, "gaokao_llm_service/v1/chat/completions"); 
    HttpResponsePtr llmResponse = httpWrapper->AsyncRequest(llmRequest);
    if (llmResponse) {
        ResultPtr result = ResultPtr(new LLMResult());
        if (llmResponse->GetResult(result)) {
            LLMResultPtr llmResultPtr = std::dynamic_pointer_cast<LLMResult>(result);
            mSummary = llmResultPtr->mContent;
            return mSummary; 
        }
    }

    return "failed";
}


AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(aigc)::LLMProcessor);

