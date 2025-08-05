#include <agg_web2/biz/general_llm/general_llm_processor.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg2/util/string_tool.h>
#include <autil/StringUtil.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <messer_utils/json/JsonParser.h>
#include <agg_web2/common/define.h>
#include <agg2/upstream/llm_request.h>
#include <agg2/upstream/llm_cache.h>
#include <agg2/upstream/llm_result.h>

#include <agg_web2/common/llm/qa_llm_request.h>
#include <agg_web2/common/llm/pgllm_request.h>
#include <agg_web2/common/llm/pgllm_result.h>
#include <agg_web2/common/llm/prompt_tokenizer_request.h>
#include <agg_web2/common/llm/prompt_tokenizer_result.h>

#include <unistd.h>
#include <agg_web2/helper/medical_chat_helper.h>
#include <agg_web2/helper/general_search_helper.h>
#include <sstream>
#include <iomanip>
#include <agg_web2/data/rewrite_query_pool_data.h>
#include <agg_web2/data/key_value_data.h>
#include <agg_web2/data/value_data.h>
#include <agg_web2/data/host_authority_data.h>
#include <re2/re2.h>
#include <agg_web2/util/string_deduper.h>

using namespace std;
AGG_BEGIN_NAMESPACE(agg_web2)
AGG_LOG_SETUP(agg_web2, GeneralLLMProcessor);
AGG_LOG_SETUP_WITH_NAME(displayLog, displayLog, GeneralLLMProcessor);

const static string PARAM_GENERAL_LLM_SYNC_NODE = "general_llm_sync_server";
const static string PARAM_QUERY_INTENT_LLM_MODEL_NAME_NEW = "query_intent_llm_name_new";
const static string PARAM_REWRITE_LLM_MODEL_NAME = "rewrite_llm_name";
const static string PARAM_MAX_LOOP_COUNT = "max_loop_count";
const static string PARAM_REWRITE_LLM_MODEL_NAME_NEW = "rewrite_llm_name_new";
const static string PARAM_SINGLE_REWRITE_LLM_MODEL_NAME = "single_rewrite_llm_name";
const static string PARAM_ANSWER_LOCATE_LLM_MODEL_NAME = "answer_locate_llm_name";
const static string PARAM_QT_MODEL_NODE = "qt_model";
const static string PARAM_AI_VENDOR = "ai_vendor";
const static string PARAM_LLM_API_KEY = "llm_api_key";
const static string TAIR_NODE_NAME_KEY = "session_tair_node";
const static string PARAM_AGG_HOST = "agg_host";

const static string METHOD_REQUEST_AGG = "request_agg";

BEGIN_DEFINE_SERIALIZATION_FUNC(GeneralLLMProcessor)
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
    ar & BOOST_SERIALIZATION_NVP(mGeneralLLMSyncNode);
    ar & BOOST_SERIALIZATION_NVP(mQueryIntentLLMNameNew);
    ar & BOOST_SERIALIZATION_NVP(mRewriteLLMName);
    ar & BOOST_SERIALIZATION_NVP(mRewriteLLMNameNew);
    ar & BOOST_SERIALIZATION_NVP(mSingleRewriteLLMName);
    ar & BOOST_SERIALIZATION_NVP(mAnswerLocateLLMName);
    ar & BOOST_SERIALIZATION_NVP(mQTModelNode);
    ar & BOOST_SERIALIZATION_NVP(mChatRound);
    ar & BOOST_SERIALIZATION_NVP(mIsOffline);
END_DEFINE_SERIALIZATION_FUNC(GeneralLLMProcessor)

bool GeneralLLMProcessor::DoInit(const String2String &params)
{
    PARSE_PROCESSOR_PARAM(params, PARAM_GENERAL_LLM_SYNC_NODE, mGeneralLLMSyncNode);
    PARSE_PROCESSOR_PARAM(params, PARAM_QUERY_INTENT_LLM_MODEL_NAME_NEW, mQueryIntentLLMNameNew)
    PARSE_PROCESSOR_PARAM(params, PARAM_REWRITE_LLM_MODEL_NAME, mRewriteLLMName);
    PARSE_PROCESSOR_PARAM(params, PARAM_REWRITE_LLM_MODEL_NAME_NEW, mRewriteLLMNameNew);
    PARSE_PROCESSOR_PARAM(params, PARAM_SINGLE_REWRITE_LLM_MODEL_NAME, mSingleRewriteLLMName);
    PARSE_PROCESSOR_PARAM(params, PARAM_ANSWER_LOCATE_LLM_MODEL_NAME, mAnswerLocateLLMName);
    PARSE_PROCESSOR_PARAM(params, PARAM_AI_VENDOR, mAiVendor);
    PARSE_PROCESSOR_PARAM(params, PARAM_LLM_API_KEY, mApiKey);
    PARSE_PROCESSOR_PARAM(params, PARAM_QT_MODEL_NODE, mQTModelNode);
    if (mAiVendor.empty()){
        mAiVendor = "shenma_ai";
    }
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
    PARSE_PROCESSOR_PARAM(params, TAIR_NODE_NAME_KEY, mTairNodeName);
    AGG_LOG(INFO, "agg host:[%s] api key:[%s] vendor:[%s]", mAggHost.c_str(), mApiKey.c_str(), mAiVendor.c_str());
    return true;
}

bool GeneralLLMProcessor::RegisterMethods() {
    return RegisterMethod(METHOD_REQUEST_AGG, &GeneralLLMProcessor::RequestAgg);
}

BaseProcessor* GeneralLLMProcessor::DoClone() {
    return new GeneralLLMProcessor(*this);
}

void GeneralLLMProcessor::DoDestroy() {
    delete this;
}

std::string StripQueryLine(const std::string& query)
{
    return StringTool::ReplaceStr(query, "\n", "");
}

HttpResponsePtr GeneralLLMProcessor::AsyncRequestAgg(AggContext *aggContext, std::string query)
{
    string simQuery = GetSimQuery(aggContext, query);
    AGG_LOG(ERROR, "query=%s be replaceed by simQuery=%s", query.c_str(), simQuery.c_str());
    if (! simQuery.empty()) {
        query = simQuery;
    }

    return AsyncRequestAggRaw(aggContext, query);
}

HttpResponsePtr GeneralLLMProcessor::AsyncRequestAggRaw(AggContext *aggContext, std::string query)
{
    AggWebRequest * aggRequest = new AggWebRequest(aggContext, query);
    aggRequest->SetHttpMethod(HM_GET);
    aggRequest->SetHost(mAggHost);
    HttpRequestPtr httpRequest(aggRequest);
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "agg_web_server/agg");
    HttpResponsePtr aggResponse = wrapper->AsyncRequest(httpRequest);
    aggRequest->GetRequestStr(mAggRequestStr);
    AGG_LOG(ERROR,"agg web query=%s, request=%s", query.c_str(), mAggRequestStr.c_str());
    return aggResponse;
}

HttpResponsePtr GeneralLLMProcessor::AsyncRequestKnowledge(AggContext *aggContext, std::string query)
{
    //AGG_LOG(ERROR,"AsyncRequestKnowledge start");
    string osr_shield;
    mAggContext->GetRequestParam("osr_shield", osr_shield);
    HttpRequestPtr httpRequest(new KnowledgeRequest(query, osr_shield));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "knowledge_server/knowledge_search");
    HttpResponsePtr aggResponse = wrapper->AsyncRequest(httpRequest);// 异步请求
    if (NULL == aggResponse) {
        AGG_LOG(ERROR,"AsyncRequestKnowledge Failed");
    }
    return aggResponse;
}

HttpResponsePtr GeneralLLMProcessor::AsyncRequestCommon(AggContext *aggContext, std::string query, const std::string& type)
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

AggWebResultPtr GeneralLLMProcessor::WaitAgg(AggContext *aggContext,
        String2String&  cacheMap,
        HttpResponsePtr aggResponse,
        HttpResponsePtr knowledgeResponse,
        HttpResponsePtr newsResponse)
{
    AggWebResultPtr aggResultPtr;

    bool isAnquanMode = false;
    string osr_shield;
    mAggContext->GetRequestParam("osr_shield", osr_shield);
    if (osr_shield == "f1" || osr_shield == "f3") {
        AGG_LOG(INFO, "now query hit osr_shield [%s]", osr_shield.c_str());
        isAnquanMode = true;
    }

    aggResultPtr = SafeGetAggWeb(cacheMap, aggResponse, isAnquanMode);
    if (!aggResultPtr)
        return aggResultPtr;

    bool needKnowledge = false;
    if (aggResultPtr) {// 法律、历史、财经 问答触发
        if (aggResultPtr->mIsQaTrigger &&
                (aggResultPtr->mDomain == "law" || aggResultPtr->mDomain == "history" ||
                 aggResultPtr->mDomain == "finance" || aggResultPtr->mDomain == "policy")) {
            needKnowledge = true;
        }

        if (aggResultPtr->mFresh == 4) {// 高时效不触发
            needKnowledge = false;
        }

        if (aggResultPtr->mIsSCMode) { // 触发SC的不触发
            if (aggResultPtr->mDocPtrVec.size() > 0
                    && aggResultPtr->mDocPtrVec[0]->mUrl.find("wenda_selected") != std::string::npos) {
                // 问答结果，还是出知识索引
                // TODO：需要区分下是否精选问答
                // AGG_LOG(ERROR, "do nothings");
            } else {
                needKnowledge = false;
            }
        }

        // if (isAnquanMode) {// 走安全模式的不触发
        //    needKnowledge = false;
        // }
        AGG_LOG(ERROR, "needKnowledge:%d, qa:%d, domain:%s, fresh:%d, sc:%d, anquan:%d", (int32_t)needKnowledge
                , (int32_t)aggResultPtr->mIsQaTrigger
                , aggResultPtr->mDomain.c_str(), aggResultPtr->mFresh, (int32_t)aggResultPtr->mIsSCMode
                , (int32_t)isAnquanMode);
    }
    AGG_LOG(INFO, "needKnowledge status: [%d]", (int32_t)needKnowledge);

    //if (needKnowledge && knowledgeResponse && aggResultPtr) {
    //    ResultPtr knowledgeResult(new KnowledgeResult());
    //    if (knowledgeResponse->GetResult(knowledgeResult)) {
    //        KnowledgeResultPtr knowledgePtr = dynamic_pointer_cast<KnowledgeResult>(knowledgeResult);
    if (needKnowledge) {
        KnowledgeResultPtr knowledgePtr = SafeGetKnowledge(cacheMap, knowledgeResponse);
        if (knowledgePtr) {
            vector<LLMDocPtr> knowledgeVec;
            for (size_t i = 0; i < knowledgePtr->mDocs.size(); ++i) {
                if (knowledgePtr->mDocs[i].mTriggerLevel < 7) // 只取触发等级是7和10的部分
                    continue;

                LLMDocPtr docPtr(new LLMDoc(knowledgePtr->mDocs[i].mTitle,
                                            knowledgePtr->mDocs[i].mBody,
                                            "knowledge " + knowledgePtr->mDocs[i].mUrl, i,
                                            knowledgePtr->mDocs[i].mQtc, 0.0,
                                            knowledgePtr->mDocs[i].mPr,
                                            knowledgePtr->mDocs[i].mPublishTime, true));
                docPtr->mRankScore = 1000.0 - (double)i * 20.0;
                knowledgeVec.emplace_back(docPtr);

                if (knowledgeVec.size() >= 3)
                    break;
            }

            AGG_LOG(INFO, "Add Knowldege Result[%d]", (int32_t)knowledgeVec.size());
            if (knowledgeVec.size() > 0) {
                aggResultPtr->mDocPtrVec.insert(aggResultPtr->mDocPtrVec.begin(), knowledgeVec.begin(), knowledgeVec.end());
            }
        } else {
            AGG_LOG(ERROR, "get NEWS data failed");
        }
    }

    bool needNews = false;
    if (aggResultPtr) {
        if (aggResultPtr->mDomain == "sports" || aggResultPtr->mDomain == "game" || aggResultPtr->mDomain == "ent" ||
                aggResultPtr->mDomain == "3c" || aggResultPtr->mDomain == "business" || aggResultPtr->mDomain == "finance" || aggResultPtr->mDomain == "internet")
        {
            needNews = true;
        }
        vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
        for (size_t i=0; i < docPtrVec.size(); ++i) {
            auto docPtr = docPtrVec[i];
            // if (docPtr->mUrl.find(".news.") != std::string::npos || docPtr->mUrl.find(".news/") != std::string::npos || docPtr->mUrl.find("/news/") != std::string::npos || docPtr->mUrl.find("/news.") != std::string::npos) {
            if (docPtr->mUrl.find("news") != std::string::npos) {
                needNews = true;
                break;
            }
        }
        if (aggResultPtr->mIsSCMode) { // 触发SC的不触发
            needNews = false;
        }
        if (isAnquanMode) { // 走安全模式的不触发
            needNews = false;
        }
    }
    AGG_LOG(INFO, "needNews status: [%d]", (int32_t)needNews);

    //if (needNews && newsResponse && aggResultPtr) {
    //    ResultPtr newsResult(new CommonResult("news"));
    //    if (newsResponse->GetResult(newsResult)) {
    //        CommonResultPtr newsPtr = dynamic_pointer_cast<CommonResult>(newsResult);
    if (needNews) {
        CommonResultPtr newsPtr = SafeGetNews(cacheMap, newsResponse);
        if (newsPtr) {
            vector<LLMDocPtr> newsVec;
            for (size_t i = 0; i < newsPtr->mDocs.size(); ++i) {
                LLMDocPtr docPtr(new LLMDoc(newsPtr->mDocs[i].mTitle,
                                            newsPtr->mDocs[i].mBody,
                                            "news " + newsPtr->mDocs[i].mUrl, i,
                                            newsPtr->mDocs[i].mQtc, 0.0,
                                            newsPtr->mDocs[i].mPr,
                                            newsPtr->mDocs[i].mPublishTime, true));
                docPtr->mRankScore += 2;
                newsVec.emplace_back(docPtr);

                if (newsVec.size() >= 3) {
                    break;
                }
            }

            AGG_LOG(INFO, "Add News Result[%d]", (int32_t)newsVec.size());
            if (newsVec.size() > 0) {
                aggResultPtr->mDocPtrVec.insert(aggResultPtr->mDocPtrVec.end(), newsVec.begin(), newsVec.end());
            }
        } else {
            AGG_LOG(ERROR, "get NEWS data failed");
        }
    }

    if (aggResultPtr) {
        auto &docPtrVec = aggResultPtr->mDocPtrVec;
        for (size_t i = 0; i < docPtrVec.size(); ++i) {
            docPtrVec[i]->mRawPR = docPtrVec[i]->mPR;
        }
    }

    if (!isAnquanMode && !aggResultPtr->mIsSCMode) {
        AGG_LOG(INFO, "TO RERANK");
        int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
        if (mBucket.find("rerank=0") != std::string::npos) {
            ReRankOld(aggResultPtr); // 已废弃，可以去除
        }
        else {
            ReRankNew(aggResultPtr);
        }
        int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank COST:%d", end_time - start_time);

        AGG_LOG(INFO, " TO DEDUP");
        start_time = autil::TimeUtility::currentTimeInMicroSeconds();
        DeDuplicate(aggResultPtr);
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "DeDuplicate COST:%d", end_time - start_time);
    } else {
        AGG_LOG(INFO, "no need TO RERANK");
    }

    AGG_LOG(ERROR,"agg web response succ");
    return aggResultPtr;
}

bool GeneralLLMProcessor::ConstructRewriteInput(AGG::AggContext *aggContext,
        const std::string& query, std::string& prompt, size_t round_limit)
{
    stringstream ss;
    ss << "\n\nHuman: ";
    LLMCachePtr llmCachePtr = mAggContext->GetValue<LLMCache>(LLM_CACHE_AGG_KEY);
    if (llmCachePtr) {
        LLMHistoryVec & historyVec = llmCachePtr->GetHistory();
        // 轮数限制，默认3轮。取n-1轮历史对话和当前用户输入送改写模型
        size_t start_idx = (size_t)std::max((int)(historyVec.size()-2*(round_limit-1)), 0);
        // 构建改写模型输入
        for (auto i = start_idx; i < historyVec.size(); ++i) {
            auto history = historyVec[i];
            if (history->role == "user") {
                ss << "输入:" << history->content << "\n";
            } else if ((history->role == "assistant")) {
                ss << "输出:" << history->content << "\n";
            }
        }
    }
    ss << "输入:" << query << "\n\nAssistant:";
    prompt = ss.str();
    ChatResponsePtr responsePtr = AggWebContextHelper::GetChatResponse(aggContext);
    responsePtr->debug_data.rewrite_prompt = prompt;
    return true;
}

std::string GeneralLLMProcessor::GetLatestHistoryQuery(AGG::AggContext *aggContext)
{
    LLMCachePtr llmCachePtr = mAggContext->GetValue<LLMCache>(LLM_CACHE_AGG_KEY);
    if (llmCachePtr) {
        LLMHistoryVec & historyVec = llmCachePtr->GetHistory();
        for (auto i = historyVec.size(); i > 0;) {
            -- i;
            auto history = historyVec[i];
            if (history->role == "user") {
                return history->content;
            }
        }
    }
    return std::string("");
}

std::string GeneralLLMProcessor::RequestQueryIntent(AGG::AggContext *aggContext, const std::string& query)
{
    std::string intent = GetQueryIntentMakeup(query);
    if (! intent.empty()) {
        mDebugInfoPtr->AddDebug("[makeup[" + query + "] => " + intent + "]");
        AGG_LOG(ERROR, "RequestQueryIntent match makeup intent=%s", intent.c_str());
        return intent;
    }

    std::string modelName = mQueryIntentLLMNameNew;
    std::string prompt = "\n\nHuman: 请判断下面指令的意图。\n指令：\"" + query + "\"\n\nAssistant: ";
    LLMRequestPtr llmRequest = LLMRequestPtr(new LLMRequest(aggContext, modelName));
    llmRequest->SetApiKey(mApiKey);
    llmRequest->SetPrompt(prompt);
    llmRequest->SetMaxNewTokens(1024);
    llmRequest->SetTemperature(0.1);
    llmRequest->SetOriQuery(AggContextHelper::GetRawQuery(mAggContext));

    AGG_LOG(INFO, "prompt=%s, modelName=%s, mGeneralLLMSyncNode=%s",
            prompt.c_str(), modelName.c_str(), mGeneralLLMSyncNode.c_str());

    auto httpWrapper = GetHttpWrapper(aggContext, mGeneralLLMSyncNode);
    ResultPtr result = ResultPtr(new LLMResult());
    if (!httpWrapper->SyncRequest(llmRequest, result)) {
        AGG_LOG(ERROR, "RequestQueryIntent SyncRequest error");
        return string("");
    }

    LLMResultPtr llmResultPtr = std::dynamic_pointer_cast<LLMResult>(result);
    string queryIntent = llmResultPtr->mContent;
    AGG_LOG(ERROR, "RequestQueryIntent query=%s, queryIntent=%s, prompt=%s",
            query.c_str(), queryIntent.c_str(), prompt.c_str());
    return queryIntent;
}

std::string GeneralLLMProcessor::RequestQueryIntentNew(AGG::AggContext *aggContext, const std::string& query, const std::string& intentVersion)
{
    std::string intent = GetQueryIntentMakeup(query);
    if (! intent.empty()) {
        mDebugInfoPtr->AddDebug("[makeup[" + query + "] => " + intent + "]");
        AGG_LOG(ERROR, "RequestQueryIntent match makeup intent=%s", intent.c_str());
        return intent + "_MAKEUP";
    }

    vector<string> historys;
    LLMCachePtr llmCachePtr = mAggContext->GetValue<LLMCache>(LLM_CACHE_AGG_KEY);
    if (llmCachePtr) {
        LLMHistoryVec & historyVec = llmCachePtr->GetHistory();
        int32_t round_limit = 5;
        // 轮数限制，默认5轮。取n-1轮历史对话和当前用户输入送改写模型
        size_t start_idx = (size_t)std::max((int)(historyVec.size()-2*round_limit), 0);
        // 构建改写模型输入
        for (auto i = start_idx; i < historyVec.size(); ++i) {
            auto history = historyVec[i];
            if (history->role == "user") {
                historys.push_back(history->content);
            }
        }
    }

    HttpRequestPtr httpRequest(new IntentRequest(query, historys, intentVersion));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "intent_model");
    ResultPtr result(new IntentResult);
    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
        return "";
    }
    IntentResultPtr intentResult = std::dynamic_pointer_cast<IntentResult>(result);
    if (intentResult->mIntents.size() > 0) {
        // v0版本是单标签, 只取第一个标签即可
        // return intentResult->mIntents[0];
        // v1版本是多标签, 拼成字符串的形式;
        vector<string> intentResultVec;
        for (size_t i = 0; i < intentResult->mIntents.size(); ++i) {
            intentResultVec.push_back(intentResult->mIntents[i]);
        }
        std::string intentResultStr = autil::StringUtil::toString(intentResultVec, ";");
        AGG_LOG(ERROR, "RequestQueryIntentNew intent_result=%s", intentResultStr.c_str());
        return intentResultStr;
    }
    return "";
}


bool GeneralLLMProcessor::RequestSummary(AGG::AggContext *aggContext, const std::string& query
                                                , const std::string& title, const std::string& passage
                                                , std::string& summary, double& prscore)
{
    summary = "";
    prscore = 0.0;
    HttpRequestPtr httpRequest(new AeRequest(query, title, passage));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "ae_model");
    ResultPtr result(new AeResult);
    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
        return false;
    }
    AeResultPtr aeResult = std::dynamic_pointer_cast<AeResult>(result);
    summary = aeResult->mAnswer;
    prscore = aeResult->mScore;

    return true;
}

HttpResponsePtr GeneralLLMProcessor::AsyncRequestSummaryNew(AGG::AggContext *aggContext, const std::string& query
                                                , const std::string& title, const std::string& passage)
{
    string doc = title + "\n" + passage;
    string indexed_passage = "";
    vector<string> sentences;
    vector<pair<int, int> > sent_group;
    GetIndexedPassage(doc, indexed_passage, sentences, sent_group);
    std::string prompt = "请从正文中选择能回答问题的句子，输出句子的序号；若无法回答，输出“无答案”。问题：" + query + "；正文：" + indexed_passage;
    std::string prompt_str = "<|im_start|>▁user\n"+ prompt + "<|im_end|>\n<|im_start|>▁assistant\n";

    LLMRequestPtr llmRequest = LLMRequestPtr(new LLMRequest(aggContext, mAnswerLocateLLMName));
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

    AGG_LOG(ERROR, "al prompt=%s, mGeneralLLMSyncNode=%s", prompt.c_str(), mGeneralLLMSyncNode.c_str());

    auto httpWrapper = GetHttpWrapper(aggContext, mGeneralLLMSyncNode);
    HttpResponsePtr alResponse = httpWrapper->AsyncRequest(llmRequest);

    /* HttpRequestPtr httpRequest(new AlRequest(query, title, passage)); */
    /* AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "al_model"); */
    /* HttpResponsePtr alResponse = wrapper->AsyncRequest(httpRequest); */
    /* AGG_LOG(ERROR, "Async request al, query=%s, title=%s, passage=%s", query.c_str(), title.c_str(), passage.c_str()); */

    return alResponse;
}


bool GeneralLLMProcessor::RequestSummaryQdoc(AGG::AggContext *aggContext, const std::string& query
                                                , const std::string& title, const std::string& passage
                                                , std::string& summary, double& prscore)
{
//    // summary = "";
//    prscore = 0.0;
//    HttpRequestPtr httpRequest(new QdRequest(query, title, passage));
//    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "qd_correlation_model");
//    ResultPtr result(new QdResult);
//    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
//        return false;
//    }
//    QdResultPtr qdResult = std::dynamic_pointer_cast<QdResult>(result);
//    // summary = aeResult->mAnswer;
//    prscore = qdResult->mScore;
//
//    return true;
    double prscore_old = prscore;

    HttpRequestPtr httpRequest(new QdRequest(query, title, passage));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "qd_correlation_model");
    ResultPtr result(new QdResult);
    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
        AGG_LOG(ERROR, "request qd_correlation_model failed");
        return false;
    } else {
        QdResultPtr qdResult = std::dynamic_pointer_cast<QdResult>(result);
        prscore = qdResult->mScore;
        AGG_LOG(INFO, "PRscore replace the old [%f] with the new [%f]", prscore_old, prscore);
    }

    return true;
}


bool GeneralLLMProcessor::RequestKnowledgeGradeResult(AggContext* aggContext, const std::string& query, const std::string& doc, std::vector<std::string>& labels, std::vector<double>& scores)
{
    vector<double> empty_scores;
    vector<std::string> empty_lables;
    labels = empty_lables;
    scores = empty_scores;

    HttpRequestPtr httpRequest(new KnowledgeGradeRequest(query, doc));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "knowledge_grade_model");
    ResultPtr result(new KnowledgeGradeResult);
    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
        AGG_LOG(ERROR, "[KG_DEBUG] Get knowledge grade score failed");
        return false;
    }
    KnowledgeGradeResultPtr knowledgeGradeResult = std::dynamic_pointer_cast<KnowledgeGradeResult>(result);

    labels = knowledgeGradeResult -> mLabels;
    scores = knowledgeGradeResult -> mScores;
    return true;
}


void GeneralLLMProcessor::RequestRewriteModel(AGG::AggContext *aggContext)
{
    // 获取改写模型输入
    std::string rewritePrompt = "";
    std::string query = AggContextHelper::GetRawQuery(aggContext);
    mRewriteStr = AggContextHelper::GetRequestParam(aggContext, "query_rewrite");
    if (mRewriteStr.empty()) {
        ConstructRewriteInput(aggContext, query, rewritePrompt);
        AGG_LOG(ERROR, "rewritePrompt=%s", rewritePrompt.c_str());

        // 请求模型服务
        string reWriteName = mRewriteLLMName;
        //std::string mBucket;
        //aggContext->GetRequestParam("bucket", mBucket);
        //if (mBucket.find("llmrewrite=7") != std::string::npos) {
        reWriteName = mRewriteLLMNameNew;
        //}
        AGG_LOG(DEBUG, "[CJQ-LOG] RewriteLLM Name:%s", reWriteName.c_str());

        LLMRequestPtr rewriteRequest = LLMRequestPtr(new LLMRequest(aggContext, reWriteName));
        rewriteRequest->SetApiKey(mApiKey);
        rewriteRequest->SetPrompt(rewritePrompt);
        rewriteRequest->SetTemperature(0.01);
        rewriteRequest->SetOriQuery(AggContextHelper::GetRawQuery(mAggContext));

        auto httpWrapper = GetHttpWrapper(aggContext, mGeneralLLMSyncNode);
        ResultPtr result;
        result.reset(new LLMResult());
        if(!httpWrapper->SyncRequest(rewriteRequest, result)){
            AGG_LOG(ERROR, "[CJQ-LOG] RewriteLLM request error.");
        }

        // 解析
        LLMResultPtr llmResultPtr = std::dynamic_pointer_cast<LLMResult>(result);
        mRewriteStr = llmResultPtr->mContent;
        AGG_LOG(INFO, "[CJQ-LOG] RewriteLLM ReWriteName[%s], ReqId:[%s], return code:[%d].",
                reWriteName.c_str(), llmResultPtr->mRequestId.c_str(), llmResultPtr->mErrorCode);
    }
    ChatResponsePtr responsePtr = AggWebContextHelper::GetChatResponse(aggContext);
    responsePtr->debug_data.rewrite_response = mRewriteStr;
    // 后处理
    mQueryRewriteVec = autil::StringUtil::split(mRewriteStr, "<sep>");
    mAggUseRawQuery = GeneralSearchHelper::RewriteQueryNeedRecover(query, mQueryRewriteVec);
}

void GeneralLLMProcessor::AddSearchDoc(Json::Value& root, AggWebResultPtr aggResultPtr,
        const std::string& postfix, const std::string& intent)
{
    GetDebugInfoPostfix(aggResultPtr);

    string query = AggContextHelper::GetRawQuery(mAggContext);
    if (mQueryRewriteVec.size() > 0) {
        query = mQueryRewriteVec[0];
    }

    bool needSummary = false;
    string osr_shield;
    mAggContext->GetRequestParam("osr_shield", osr_shield);
    if (osr_shield == "f1" ||osr_shield == "f3") {
        needSummary = true;
    }

    bool isShowAll = false;
    if (mBucket.find("debug_show_all=1") != std::string::npos) {
        isShowAll = true;
    }

    double qa_intent = 0;
    int fresh = 0;
    Json::Value search_doc = Json::Value(Json::arrayValue);
    Json::Value search_summary = Json::Value(Json::arrayValue);
    if (aggResultPtr) {
        fresh = aggResultPtr->mFresh;
        if (! aggResultPtr->mDocPtrVec.empty()) {
            int REF_QUOTA = 15;
            qa_intent = 1;
            const vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
            for(size_t i = 0; i < docPtrVec.size() && (isShowAll || (i < size_t(REF_QUOTA))); ++i) {
                auto docPtr = docPtrVec[i];
                search_doc.append(Json::Value());
                Json::Value & item = search_doc[search_doc.size() - 1];

                if (docPtr->mSummary.size() == 0) {
                    docPtr->mSummary = docPtr->mBody;
                }
                string _title   = AggWebStringTool::FilterInvalidChar(docPtr->mTitle);
                string _body    = AggWebStringTool::FilterInvalidChar(docPtr->mBody);
                string _summary = AggWebStringTool::FilterInvalidChar(docPtr->mSummary);

                if (_title != docPtr->mTitle) {
                    docPtr->mTitle = _title;
                    AGG_LOG(ERROR, "has invalid char, title[%s][%s]", docPtr->mTitle.c_str(), _title.c_str());
                }
                
                if (_body != docPtr->mBody) {
                    docPtr->mBody = _body;
                    AGG_LOG(ERROR, "has invalid char, body[%s][%s]", docPtr->mTitle.c_str(), _body.c_str());
                }
                
                if (_summary != docPtr->mSummary) {
                    docPtr->mSummary = _summary;
                    AGG_LOG(ERROR, "has invalid char, summary[%s][%s]", docPtr->mSummary.c_str(), _summary.c_str());
                }

                item["title"] = docPtr->mTitle;
                item["body"] = docPtr->mBody;
                item["link"] = docPtr->mUrl;
                item["source"] = "web";
                item["hostname"] = docPtr->mHostname;
                item["correlation"] = docPtr->mQtc;
                item["passage_ranking_label"] = docPtr->mPR;
                //item["raw_pr"] = docPtr->mRawPR;
                item["rank_score"] = docPtr->mRankScore;
                //item["new_rank_score"] = docPtr->mNewRankScore;
                item["publishTime"] = docPtr->mPublishTime;
                item["rankpos"] = docPtr->mAggRank;
                item["kg_scores"] = docPtr->mKgScoreStr;
                item["debug_info"] = docPtr->mDebugInfo;
                item["summary"] = docPtr->mSummary;
                mSummaryModelSelection = true;

                if (needSummary && docPtr->mBody.size() > 0 && docPtr->mTitle.size() && query.size() >0) {
                    string passage = docPtr->mBody;
                    passage = StringTool::ReplaceStr(passage, "\n", "`");
                    passage = StringTool::ReplaceStr(passage, "“", "\"");
                    passage = StringTool::ReplaceStr(passage, "”", "\"");
                    passage = StringTool::RemoveStr(passage, "　");
                    passage = StringTool::RemoveStr(passage, " ");
                    passage = StringTool::RemoveStr(passage, "\t");

                    AGG_LOG(INFO, "passage=[%s]", passage.c_str());
                    double score = 0.0;
                    string summary;
                    RequestSummary(mAggContext, query, docPtr->mTitle, passage, summary, score);
                    //item["summary_pr"] = score;
                    if ((score > 0.21 && docPtr->mQtc > 0.5) ||
                            (osr_shield == "f1" && score > 0.06 && docPtr->mQtc > 0.5)) {
                        search_summary.append(Json::Value());
                        Json::Value & summary_item = search_summary[search_summary.size()-1];
                        summary_item["query"] = query;
                        summary_item["summary"] = StringTool::ReplaceStr(summary, "`", " ");
                        summary_item["pr"] = score;
                        summary_item["title"] = docPtr->mTitle;
                        summary_item["link"] = docPtr->mUrl;
                        summary_item["source"] = "web";
                        summary_item["hostname"] = docPtr->mHostname;
                    }
                }
            }
        }
    }
    root["query_intent" + postfix] = intent;
    root["qa_intent" + postfix] = qa_intent;
    root["search_doc" + postfix] = search_doc;
    root["search_summary" + postfix] = search_summary;
    root["fresh" + postfix] = fresh;
    root["osr_shield"] = osr_shield;
}

void GeneralLLMProcessor::RequestAggFormatResult(AGG::AggContext *aggContext, AggWebResultPtr aggResultPtr)
{
    std::string uid;
    std::string sessionId;
    std::string reqid;
    mAggContext->GetRequestParam("uid", uid);
    mAggContext->GetRequestParam("sessionid", sessionId);
    aggContext->GetRequestParam("reqid", reqid);

    Json::Value root;

    Json::Value communication;
    communication["uid"] = uid;
    communication["sessionid"] = sessionId;
    communication["reqid"] = reqid;
    communication["resid"] = 0;
    root["communication"] = communication;

    root["is_med"] = false;
    root["query"] = AggContextHelper::GetRawQuery(mAggContext);

    // 改写
    root["query_rewrite_status"] = ! mAggUseRawQuery;
    Json::Value search_query = Json::Value(Json::arrayValue);
    for (const auto& q: mQuerySearchVec) {
        search_query.append(Json::Value(q));
    }
    search_query.append(Json::Value(std::to_string(mUseRewriteAggResultPtr))); // 临时抓取评测使用，末尾0/1表示是否最终使用搜索改写的结果
    root["search_query"] = search_query;
    root["use_rewrite_agg"] = mUseRewriteAggResultPtr; // 透传给quarkman下游, 判断是否使用搜索改写的agg结果;

    root["target"] = "respond";
    root["version"] = 1;

    uint64_t sessionStartTime = mContext->GetStartProcessTime();
    root["timestamp"] = autil::StringUtil::toString<uint64_t>(sessionStartTime);

    if ((! mRawQueryAggResultPtr || mRawQueryAggResultPtr->mDocPtrVec.empty())
            && (! mRewriteQueryAggResultPtr || mRewriteQueryAggResultPtr->mDocPtrVec.empty())) {
        root["error_code"] = 10001;
        root["error_msg"] = "agg result get failed";
    } else {
        root["error_code"] = 0;
        root["error_msg"] = "";
    }

    AddSearchDoc(root, mRawQueryAggResultPtr, "", mRawQueryIntent);
    AddSearchDoc(root, mRewriteQueryAggResultPtr, "_rewrite", mRewriteQueryIntent);

    root["debug_info"] = GetDebugInfoPrefix("plan2") + mReferInfo;

    Json::FastWriter writer;
    std::string resContent = writer.write(root);

    AGG_LOG(ERROR, "resContent=%s", resContent.c_str());

    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

static int GetQueryIntent(const std::string& intent)
{
    // 负值意图严格不触发改写
    if (autil::StringUtil::startsWith(intent, "主")) {
        return 4;
    } else if (autil::StringUtil::startsWith(intent, "其")) {
        return 3;
    } else if (autil::StringUtil::startsWith(intent, "名")) {
        return 2;
    } else if (autil::StringUtil::startsWith(intent, "意")) {
        return 1;
    }  else if (autil::StringUtil::startsWith(intent, "闲")) {
        return -1;
    } else if (autil::StringUtil::startsWith(intent, "创")) {
        return -2;
    } else {
        return 0;
    }
}

bool GeneralLLMProcessor::GetNewQueryIntent(AGG::AggContext *aggContext, std::string &query, std::string &mBucket, std::string &mRawQueryIntent, int &status, std::string intentVersion)
{
    // v0: 初版单标签版本, 融合原始大搜意图模型进行判断
    if (intentVersion == "v0") {
        //if (mBucket.find("llmrewrite=7") != std::string::npos) {
            mRawQueryIntent = RequestQueryIntentNew(aggContext, query, intentVersion);
            AGG_LOG(ERROR, "query intent new v0 version: [%s]", mRawQueryIntent.c_str());
            if (mRawQueryIntent.find("_MAKEUP") != std::string::npos) {
                // do nothing, makup的结果保持不变
            } else if (status == 1) {
                // do nothing, 意图不明的保持不变
            } else if (mRawQueryIntent.find("知识类-意图不明") != std::string::npos) {
                status = 1;
            } else if (mRawQueryIntent.find("知识类") != std::string::npos) {
                if (status != 2) { // 名词短语的保持不变
                    status = 3;
                }
            } else if (!mRawQueryIntent.empty()) {
                status = -1;
            }
        //}
    } else if (intentVersion == "v1") {
        // v1: 新版多标签版本, 不使用原始大模型意图
        //if (mBucket.find("llmrewrite=7") != std::string::npos) {
            mRawQueryIntent = RequestQueryIntentNew(aggContext, query, intentVersion);
            AGG_LOG(ERROR, "query intent new v1 version: [%s]", mRawQueryIntent.c_str());
            if (mRawQueryIntent.find("_MAKEUP") != std::string::npos) {
                if (mRawQueryIntent.find("其它") != std::string::npos) {
                    status = 3;
                }
                // 其它的类型 do nothing, makup的结果保持不变
            } else if (mRawQueryIntent.find("知识类-意图不明") != std::string::npos) {
                status = 1;
            } else if (mRawQueryIntent.find("名词类") != std::string::npos) {
                status = 2; // 这里可能要结合知识类, 进一步调整策略
            } else if (mRawQueryIntent.find("知识类") != std::string::npos) {
                status = 3;
            } else {
                status = -1;
            }
        //}
    }

    //10个字以上Query & 安全模式：对 文创、其他 类型的query进行扩召回
    if (query.size() > 30 && mRawQueryIntent.size() > 0) {
        string osr_shield, problem_code;
        aggContext->GetRequestParam("osr_shield", osr_shield);
        aggContext->GetRequestParam("problem_code", problem_code);
        //AGG_LOG(ERROR, "osr_shield:[%s], problem_code:[%s]", osr_shield.c_str(), problem_code.c_str());
        if (osr_shield == "f1" && problem_code == "politics") {
            if (mRawQueryIntent.find("文创") != std::string::npos ||
                    mRawQueryIntent.find("其他") != std::string::npos) {
                status = 3;
            }
        }
    }

    return true;
}

static bool StripQuery(std::string& query, const std::string& p1)
{
    bool flag = false;
    if (autil::StringUtil::startsWith(query, p1)) {
        static const string p2 = "?";
        if (autil::StringUtil::endsWith(query, p2)) {
            query = query.substr(0, query.size() - p2.size());
            flag = true;
        }

        static const string p3 = "？";
        if (autil::StringUtil::endsWith(query, p3)) {
            query = query.substr(0, query.size() - p3.size());
            flag = true;
        }

        static const string p4 = "吗";
        if (autil::StringUtil::endsWith(query, p4)) {
            query = query.substr(0, query.size() - p4.size());
            flag = true;
        }

        if (flag) {
            query = query.substr(p1.size());
        }
    }
    return flag;
}

static bool StripQuery(std::string& query)
{
    return StripQuery(query, "你知道")
        || StripQuery(query, "请问，")
        || StripQuery(query, "请问你知道")
        || StripQuery(query, "请问");
}

// 去掉收尾空格
void GeneralLLMProcessor::ClearHeadTailSpace(std::string& input_str)
{
    if (input_str.empty()) {
        return;
    }
    input_str.erase(0, input_str.find_first_not_of(" "));
    input_str.erase(input_str.find_last_not_of(" ") + 1);
    return;
}

std::vector<double> GeneralLLMProcessor::GetQT(AggContext* aggContext, const std::string& query,
        const std::vector<std::string>& titles)
{
    HttpRequestPtr httpRequest(new QtRequest(query, titles));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, mQTModelNode);
    ResultPtr result(new QtResult);
    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
        AGG_LOG(ERROR, "GetSimQuery qt failed");
        vector<double> empty;
        return empty;
    }
    QtResultPtr qtResult = std::dynamic_pointer_cast<QtResult>(result);
    return qtResult->mQtScores;
}

std::string GeneralLLMProcessor::GetSimQuery(AggContext* aggContext, const std::string& query)
{
    AGG_LOG(ERROR, "query=%s, GetSimQuery", query.c_str());
    AggDataWrapper data("rewrite_query_pool_data");
    auto rewriteQueryPoolDataPtr = data.Get<RewriteQueryPoolData>();
    if (rewriteQueryPoolDataPtr) {
        vector<string> simQuerys;
        vector<double> qt_thresholds;
        double qt_threshold = 0;
        double sim_threshold = 0.4;
        int max_len = 100;
        int topn = 10;
        std::vector<pair<std::string, double>> sim_querys;
        if (rewriteQueryPoolDataPtr->GetMostSimQuery(query, sim_querys, max_len, sim_threshold, topn)) {
            for (const pair<std::string, double>& p : sim_querys) {
                simQuerys.push_back(p.first);
                qt_thresholds.push_back(p.second);
            }
            if (simQuerys.empty()) {
                return "";
            }
            vector<double> qt_results = GetQT(aggContext, query, simQuerys);
            if (qt_results.size() == simQuerys.size()) {
                string maxSimQuery;
                double maxQtScore = 0;
                for (size_t i = 0; i < qt_results.size(); ++i) {
                    if (qt_results[i] + 1e-5 >= qt_thresholds[i] && qt_results[i] + 1e-5 >= maxQtScore) {
                        maxQtScore = qt_results[i];
                        maxSimQuery = simQuerys[i];
                    }
                }
                if (! maxSimQuery.empty()) {
                    mDebugInfoPtr->AddDebug("[检索增强makeup[" + query + "] => [" + maxSimQuery
                            + "] qt=" + std::to_string(maxQtScore) + "]");
                }
                return maxSimQuery;
            } else {
                AGG_LOG(ERROR, "GetSimQuery qt_results.size()=%u, simQuerys.size()=%u",
                        qt_results.size(), simQuerys.size());
            }
        }
    }
    return "";
}

std::string GeneralLLMProcessor::GetQueryIntentMakeup(const std::string& query)
{
    AggDataWrapper data("query_rewrite_data");
    auto queryRewriteDataPtr = data.Get<KeyValueData>();
    if (queryRewriteDataPtr) {
        string intent = queryRewriteDataPtr->GetValue(query);
        AGG_LOG(ERROR, "query=%s, intent=%s", query.c_str(), intent.c_str());
        return intent;
    } else {
        AGG_LOG(ERROR, "query_rewrite_data failed");
    }
    return "";
}

int32_t GeneralLLMProcessor::GetQueryIntentStatus(AggContext *aggContext, std::string &query, std::string& queryIntent)
{
    /*LLMCachePtr llmCachePtr = aggContext->GetValue<LLMCache>(LLM_CACHE_AGG_KEY);
    if (llmCachePtr && llmCachePtr->GetHistory().size() > 0) {
        mChatRound = llmCachePtr->GetHistory().size();
    }*/

    // 默认是v1版本
    std::string intentVersion = "v1";
    if (mBucket.find("intentbucket=v0") != std::string::npos) {
        intentVersion = "v0";
    }

    int32_t status = -1;
    if (intentVersion == "v0") {
        queryIntent = RequestQueryIntent(aggContext, query);
        status = GetQueryIntent(queryIntent);
        AGG_LOG(ERROR, "================== query=%s, mChatRound=%d, status=%d", query.c_str(), mChatRound, status);
        GetNewQueryIntent(aggContext, query, mBucket, queryIntent, status, intentVersion);
        AGG_LOG(ERROR, "================== intentVersion v0: query=%s, mChatRound=%d, status=%d", query.c_str(), mChatRound, status);
    } else if (intentVersion == "v1") {
        GetNewQueryIntent(aggContext, query, mBucket, queryIntent, status, intentVersion);
        AGG_LOG(ERROR, "================== intentVersion v1: query=%s, mChatRound=%d, status=%d", query.c_str(), mChatRound, status);
    }

    string _query = StripQueryLine(AggContextHelper::GetRawQuery(aggContext));
    bool helloPattern = StripQuery(_query);
    if (mChatRound == 0 || helloPattern) { // 单轮
        if (status <= 1) { // 意图不明等
            status = -1;
        }
    }
    return status;
}

bool GeneralLLMProcessor::ConstructSingleRewriteInput(AGG::AggContext *aggContext,
        const std::string& query, std::string& prompt)
{
    stringstream ss;
    // ss << "\n\nHuman: " << query << "\n\nAssistant:";
    ss << "<|im_start|>user\n" << query << "<|im_end|>\n<|im_start|>assistant\n";
    prompt = ss.str();
    ChatResponsePtr responsePtr = AggWebContextHelper::GetChatResponse(aggContext);
    AGG_LOG(ERROR, "Construct Single Rewrite Input = [%s]", prompt.c_str());
    return true;
}

void GeneralLLMProcessor::RequestSingleRewriteModel(AGG::AggContext *aggContext, std::string query)
{
    // 获取改写模型输入
    std::string rewritePrompt = "";

    mSingleRewriteStr = "";
    if (mSingleRewriteStr.empty()) {
        ConstructSingleRewriteInput(aggContext, query, rewritePrompt);
        AGG_LOG(ERROR, "single rewrite prompt = %s", rewritePrompt.c_str());

        // 请求模型服务
        string reWriteName = mSingleRewriteLLMName;
        AGG_LOG(DEBUG, "Single Rewrite LLM Name: %s", reWriteName.c_str());
        LLMRequestPtr rewriteRequest = LLMRequestPtr(new LLMRequest(aggContext, reWriteName));
        rewriteRequest->SetApiKey(mApiKey);
        rewriteRequest->SetPrompt(rewritePrompt);
        rewriteRequest->SetTemperature(0.01);
        rewriteRequest->SetOriQuery(AggContextHelper::GetRawQuery(mAggContext));

        auto httpWrapper = GetHttpWrapper(aggContext, mGeneralLLMSyncNode);
        ResultPtr result;
        result.reset(new LLMResult());
        if (!httpWrapper->SyncRequest(rewriteRequest, result)) {
            AGG_LOG(ERROR, "Single Rewrite LLM request error.");
        }

        // 解析
        LLMResultPtr llmResultPtr = std::dynamic_pointer_cast<LLMResult>(result);
        mSingleRewriteStr = llmResultPtr->mContent;
        AGG_LOG(INFO, "SingleRewriteLLM ReWriteName[%s], ReqId:[%s], return code:[%d].",
                reWriteName.c_str(), llmResultPtr->mRequestId.c_str(), llmResultPtr->mErrorCode);
    }
    AGG_LOG(ERROR, "single rewrite response = %s", mSingleRewriteStr.c_str());

    /*if (mSingleRewriteStr.size() > 0 && mSingleRewriteStr.find("服务异常") != string::npos) {
        return "";
    }
    return mSingleRewriteStr;
    */

    // 后处理
    mQuerySingleRewriteVec.clear();
    mQuerySingleRewriteVec = autil::StringUtil::split(mSingleRewriteStr, "<sep>");
    mAggNotUseSingleRewrite = GeneralSearchHelper::SingleRewriteQueryNeedRecover(query, mQuerySingleRewriteVec);
}

void GeneralLLMProcessor::GetCacheMap(String2String& cacheMap, std::string &query)
{
    cacheMap.clear();
    cacheMap["cache_query"] = query;

    if (mBucket.find("use_agg_cache=1") == std::string::npos) {
        return;
    }

    AggCacheWrapperPtr wrapper = GetCacheWrapper(mAggContext, mTairNodeName);

    if (!wrapper)
        return;

    std::string sessionid = "12345";
    //mAggContext->GetRequestParam("sessionid", sessionid);

    GetCacheMap(wrapper, cacheMap, sessionid, "agg_web", query);
    GetCacheMap(wrapper, cacheMap, sessionid, "knowledge", query);
    GetCacheMap(wrapper, cacheMap, sessionid, "news", query);
}

void GeneralLLMProcessor::GetCacheMap(AggCacheWrapperPtr& wrapper,
                                      String2String& cacheMap,
                                      const std::string& sessionid,
                                      const std::string& type,
                                      const std::string& query)
{
    if (!wrapper)
        return;

    int status = 0;
    std::string cacheKey = "quarkman_websearch_cache_" + sessionid + "_"+type+"_" + query;
    std::string cacheVal;
    if (wrapper->Get(cacheKey, cacheVal, status)) {
        cacheMap[type] = cacheVal;
        AGG_LOG(ERROR, "GetCache:%s:[%s]", type.c_str(), cacheVal.c_str());
    }
}

void GeneralLLMProcessor::SetCache(const std::string& type, const std::string& query, const std::string& val)
{
    if (mBucket.find("use_agg_cache=1") == std::string::npos) {
        return;
    }

    if (val.empty())
        return;

    AggCacheWrapperPtr wrapper = GetCacheWrapper(mAggContext, mTairNodeName);

    if (!wrapper)
        return;

    std::string sessionid = "12345";
    //mAggContext->GetRequestParam("sessionid", sessionid);
    std::string cacheKey = "quarkman_websearch_cache_" + sessionid + "_" + type + "_" + query;
    uint32_t time = 3600;
    int status = 0;
    if (!wrapper->Set(cacheKey, val, time, status)) {
        AGG_LOG(ERROR, "Set Tair Faile.[%s]", cacheKey.c_str());
    }
}

HttpResponsePtr GeneralLLMProcessor::SafeRequest(AggContext *aggContext, String2String& cacheMap,
                                                 std::string query, const std::string& type)
{
    HttpResponsePtr response = nullptr;
    // 命中Cache，就不再访问
    if (cacheMap.find(type) != cacheMap.end())
        return response;

    if (type == "agg_web") {
        response = AsyncRequestAgg(aggContext, query); // 异步请求agg
    } else if (type == "knowledge") {
        response = AsyncRequestKnowledge(aggContext, query);
    } else if (type == "news") {
        response = AsyncRequestCommon(aggContext, query, "news");
    }
    return response;
}

AggWebResultPtr GeneralLLMProcessor::SafeGetAggWeb(String2String& cacheMap, HttpResponsePtr& aggResponse, bool isAnquanMode)
{
    AggWebResultPtr aggResultPtr = nullptr;
    if (cacheMap.find("agg_web") != cacheMap.end()) {
        ResultPtr result(new AggWebResult(mDebugInfoPtr, mIsOffline, isAnquanMode));
        result->Deserialize(cacheMap["agg_web"]);
        aggResultPtr = dynamic_pointer_cast<AggWebResult>(result);
        AGG_LOG(ERROR, "agg_web use cache succ.");
    } else {
        if (! aggResponse) {
            AGG_LOG(ERROR,"agg web request failed");
            return aggResultPtr;
        }

        ResultPtr result(new AggWebResult(mDebugInfoPtr, mIsOffline, isAnquanMode));
        if (!aggResponse->GetResult(result)) {
            AGG_LOG(ERROR, "agg web response failed");
            return aggResultPtr;
        }
        aggResultPtr = dynamic_pointer_cast<AggWebResult>(result);

        if (!aggResultPtr->mBytes.empty()) {
            SetCache("agg_web", cacheMap["cache_query"], aggResultPtr->mBytes);
        }
    }
    return aggResultPtr;
}

KnowledgeResultPtr GeneralLLMProcessor::SafeGetKnowledge(String2String& cacheMap, HttpResponsePtr& knowledgeResponse)
{
    KnowledgeResultPtr knowledgePtr = nullptr;
    if (cacheMap.find("knowledge") != cacheMap.end()) {
        ResultPtr result(new KnowledgeResult());
        result->Deserialize(cacheMap["knowledge"]);
        knowledgePtr = dynamic_pointer_cast<KnowledgeResult>(result);
        AGG_LOG(ERROR, "knowledge use cache succ.");
    }
    else {
        if (!knowledgeResponse) {
            return knowledgePtr;
        }
        ResultPtr knowledgeResult(new KnowledgeResult());
        if (!knowledgeResponse->GetResult(knowledgeResult)) {
            return knowledgePtr;
        }

        knowledgePtr = dynamic_pointer_cast<KnowledgeResult>(knowledgeResult);

        if (!knowledgePtr->mBody.empty()) {
            SetCache("knowledge", cacheMap["cache_query"], knowledgePtr->mBody);
        }
    }
    return knowledgePtr;
}

CommonResultPtr GeneralLLMProcessor::SafeGetNews(String2String& cacheMap, HttpResponsePtr& newsResponse)
{
    CommonResultPtr newsPtr = nullptr;
    if (cacheMap.find("news") != cacheMap.end()) {
        ResultPtr newsResult(new CommonResult("news"));
        newsResult->Deserialize(cacheMap["news"]);
        newsPtr = dynamic_pointer_cast<CommonResult>(newsResult);
        AGG_LOG(ERROR, "news use cache succ.");
    }
    else {
        if (!newsResponse) {
            return newsPtr;
        }

        ResultPtr newsResult(new CommonResult("news"));
        if (!newsResponse->GetResult(newsResult)) {
            return newsPtr;
        }

        newsPtr = dynamic_pointer_cast<CommonResult>(newsResult);

        if (!newsPtr->mBody.empty()) {
            SetCache("news", cacheMap["cache_query"], newsPtr->mBody);
        }
    }
    return newsPtr;
}

AggWebResultPtr GeneralLLMProcessor::GetQueryResult(AggContext *aggContext, std::string &query)
{
    AggWebResultPtr nullResult;
    if (query.size() == 0) {
        return nullResult;
    }

    String2String cacheMap;
    GetCacheMap(cacheMap, query);
    HttpResponsePtr aggResponse       = SafeRequest(aggContext, cacheMap, query, "agg_web");
    HttpResponsePtr knowledgeResponse = SafeRequest(aggContext, cacheMap, query, "knowledge");
    HttpResponsePtr newsResponse      = SafeRequest(aggContext, cacheMap, query, "news");

    //HttpResponsePtr aggResponse = AsyncRequestAgg(aggContext, query); // 异步请求agg
    //HttpResponsePtr knowledgeResponse = AsyncRequestKnowledge(aggContext, query);
    //HttpResponsePtr newsResponse = AsyncRequestCommon(aggContext, query, "news");

    // 访问Query搜索简化后的结果
    string querySingleRewrite;
    if (mBucket.find("singlerewrite=0") == std::string::npos) {
        RequestSingleRewriteModel(aggContext, query);
        if (!mAggNotUseSingleRewrite) {
            querySingleRewrite = mQuerySingleRewriteVec[0];
            ClearHeadTailSpace(querySingleRewrite);
            if (querySingleRewrite.size() > 0 && querySingleRewrite.find("服务异常") != string::npos) {
                querySingleRewrite = "";
            }
        }
    }
    AggWebResultPtr rewriteAggResultPtr;
    if (querySingleRewrite.size() > 0 && querySingleRewrite != query) {
        String2String _cacheMap;
        GetCacheMap(_cacheMap, querySingleRewrite);
        HttpResponsePtr _aggResponse       = SafeRequest(aggContext, _cacheMap, querySingleRewrite, "agg_web");
        HttpResponsePtr _knowledgeResponse = SafeRequest(aggContext, _cacheMap, querySingleRewrite, "knowledge");
        HttpResponsePtr _newsResponse      = SafeRequest(aggContext, _cacheMap, querySingleRewrite, "news");

        //HttpResponsePtr _aggResponse = AsyncRequestAgg(aggContext, querySingleRewrite); // 异步请求agg
        //HttpResponsePtr _knowledgeResponse = AsyncRequestKnowledge(aggContext, querySingleRewrite);
        //HttpResponsePtr _newsResponse = AsyncRequestCommon(aggContext, querySingleRewrite, "news");

        rewriteAggResultPtr = WaitAgg(aggContext, _cacheMap, _aggResponse, _knowledgeResponse, _newsResponse);
        mQuerySearchVec.push_back(querySingleRewrite);
    }

    // 如何进行混排，默认先进行替换
    AggWebResultPtr rawAggResultPtr = WaitAgg(aggContext, cacheMap, aggResponse, knowledgeResponse, newsResponse);
    mUseRewriteAggResultPtr = false;
    if (!rawAggResultPtr || rawAggResultPtr->mDocPtrVec.size() == 0) {
        rawAggResultPtr = rewriteAggResultPtr;
        mUseRewriteAggResultPtr = true;
    }

    return rawAggResultPtr;
}

AggWebResultPtr GeneralLLMProcessor::ChatPlan(AggContext *aggContext)
{
    /*stringstream debug;
    debug << "this = " << this << ", mDebugInfoPtr = " << mDebugInfoPtr
        << ", mSummaryModelSelection = " << (&mSummaryModelSelection)
        << ", mSummaryModelSelection value = " << mSummaryModelSelection << endl;
    AGG_LOG(DEBUG, "%s", debug.str().c_str()); */

    string query = StripQueryLine(AggContextHelper::GetRawQuery(aggContext));
    string tmpQuery = query;
    bool helloPattern = StripQuery(query);
    AGG_LOG(ERROR, "query=%s, stripquery=%s, helloPattern=%d", tmpQuery.c_str(), query.c_str(), helloPattern);

    LLMCachePtr llmCachePtr = aggContext->GetValue<LLMCache>(LLM_CACHE_AGG_KEY);
    if (llmCachePtr && llmCachePtr->GetHistory().size() > 0) {
        mChatRound = llmCachePtr->GetHistory().size();
    }

    AggWebResultPtr nullResult;

    bool rawagg = true;
    std::string rawQuery = query;
    std::string rewriteQuery;

    /* step 1. 意图识别模块
     * status :
     * -1 : 文创类、人设类、其他类 等非知识类Query
     *  0 : 默认值
     *  1 : 知识类 - 意图不明
     *  2 : 知识类 - 名词短语
     *  >=3 : 知识类
     * */

    int32_t status = GetQueryIntentStatus(aggContext, query, mRawQueryIntent);
    if (status <= 0) { // 闲聊等意图，直接退出
        return nullResult;
    }

    if (status == 1) { // 意图不明, 多轮，访问Query改写模型
        RequestRewriteModel(aggContext); // 请求query改写
        if (! mAggUseRawQuery && ! mQueryRewriteVec.empty() && ! mQueryRewriteVec[0].empty()) { // 需要改写
            rewriteQuery = mQueryRewriteVec[0];
            ClearHeadTailSpace(rewriteQuery);
            mQuerySearchVec.push_back(rewriteQuery);
        }

        if (rewriteQuery.size() > 0) {
            static const string patt = "呢";
            bool flag = autil::StringUtil::endsWith(rewriteQuery, patt);
            if (flag) {
                rewriteQuery = rewriteQuery.substr(0, rewriteQuery.size() - patt.size());
            }
            if (rewriteQuery.find("无需改写") != std::string::npos || rewriteQuery.find("无需检索") != std::string::npos) {
                rewriteQuery = "";
            }

            std::string latestHistoryQuery = StripQueryLine(GetLatestHistoryQuery(aggContext));
            if (latestHistoryQuery.size() > 0 && rewriteQuery.size() > 0
                    && GeneralSearchHelper::CheckSimilarity(rewriteQuery, latestHistoryQuery)) { // 和上一轮Query语义一致
                rewriteQuery = "";
            }

            if (rewriteQuery == rawQuery) {
                rewriteQuery = "";
            }
        }
        if (rewriteQuery.size() > 0) {
            int32_t rewriteStatus = GetQueryIntentStatus(aggContext, rewriteQuery, mRewriteQueryIntent);
            if (rewriteStatus <= 2) { // 改写后的，非知识类的，不访问
                rewriteQuery = "";
            }
        }
    }

    /* step 2. 获取最终结果
     * 分为两种情况：1. 原始Query访问 2. 改写query访问
     * 当前逻辑：只会存在一种情况，后续如果1、2同时存在，需要改写成并行访问
     * */

    AggWebResultPtr aggResultPtr;
    if (status >= 2) {
        mQuerySearchVec.push_back(rawQuery);
        aggResultPtr = GetQueryResult(aggContext, rawQuery); // 获取原始Query的最终增强结果
        if (mChatRound > 0 && !helloPattern && status == 2 && aggResultPtr) { // 多轮，名词短语
            // ToDo：是否要保留该逻辑
            aggResultPtr->SetOnlyScResult(); // 原始Q，只保留sc结果
        }
        mQuerySelection = rawQuery;
    }

    AggWebResultPtr rewriteAggResultPtr;
    if (rewriteQuery.size() >= 0) {
        rewriteAggResultPtr = GetQueryResult(aggContext, rewriteQuery); // 获取改写Query的最终增强结果
        mQuerySelection = rewriteQuery;
    }

    /* step 3. 二次查询
     * 当 RawQuery和rewriteQuery都没有结果时，触发二次查询
     * */
    AggWebResultPtr secondSearchAggResultPtr; // 二次查询Query的最终增强结果
    if ((aggResultPtr && aggResultPtr->mDocPtrVec.size() > 0) ||
            (rewriteAggResultPtr && rewriteAggResultPtr->mDocPtrVec.size() > 0)) {
        // 有结果，do nothings
    } else {
        // 无结果，Query拆解 或者 其他，待优化
        // mQuerySearchVec.push_back(secondSearchQuery);
    }

    /* step 4. 最终结果混排/选择
     * 当前逻辑：优先使用原Query的
     * Todo：二次查询结果进行插入
     * */
    if (aggResultPtr && aggResultPtr->mDocPtrVec.size() > 0) {
        mRawQueryAggResultPtr = aggResultPtr;
        AGG_LOG(ERROR, "================== query=%s, mChatRound=%d", query.c_str(), mChatRound);
        if (aggResultPtr) {
            mQuerySelection = rawQuery;
        }
    } else if (rewriteAggResultPtr) {
        mRewriteQueryAggResultPtr = rewriteAggResultPtr;
        AGG_LOG(ERROR, "================== query=%s, mChatRound=%d", query.c_str(), mChatRound);
        if (rewriteAggResultPtr) {
            mQuerySelection = rewriteQuery;
        }
    }

    if (secondSearchAggResultPtr && secondSearchAggResultPtr->mDocPtrVec.size() > 0) {
        // 二次查询有结果，do somethings
    }

    return nullResult;
}

// 方案3：请求agg拿到agg信息，返回
int GeneralLLMProcessor::RequestAgg(AGG::AggContext *aggContext)
{
    mDebugInfoPtr = LLMDebugInfoPtr(new LLMDebugInfo);
    aggContext->GetRequestParam("bucket", mBucket);

    string query = StripQueryLine(AggContextHelper::GetRawQuery(aggContext));

    // 1.构建请求，获取结果
    AggWebResultPtr aggResultPtr = ChatPlan(aggContext);

    // 2. 封装结果
    RequestAggFormatResult(aggContext, aggResultPtr);

    bool isaggdebug = mBucket.find("llmcachewrite=1") != std::string::npos;
    if (isaggdebug) {
        MedicalChatHelper::AddLLMHistory(aggContext, query, "");

        //if (mBucket.find("llmmultidebug=1") != std::string::npos) {
        //    AggCacheWrapperPtr wrapper = GetCacheWrapper(aggContext,mTairNodeName);
        //    MedicalChatHelper::UpdateLLMCache(aggContext,wrapper);
        //}
    }

    return PR_REQUEST_EXIT;
}

std::string GeneralLLMProcessor::GetDebugInfoPrefix(const std::string& plan)
{
    MESSER_UTILS_NS(util)::ostringstream oss;
    oss << "[检索增强=" << std::to_string(mSummaryModelSelection) << "]\n\n"
        << "[" << plan << "]\n\n"
        << "[原Query意图=" << mRawQueryIntent << "]\n\n"
        << "[触发改写=" << std::to_string(!mAggUseRawQuery)
        << ", 改写结果=" << StringTool::ReplaceStr(mRewriteStr, "<sep>", " - ") << "]\n\n";
    if (! mRewriteQueryIntent.empty()) {
        oss << "[改写后首Q意图=" << mRewriteQueryIntent << "]\n\n";
    }
    if (! mSingleRewriteStr.empty()) {
        oss << "[单Query精简后=" << mSingleRewriteStr << "]\n\n";
    }
    oss << "[使用搜索改写的agg结果=" << std::to_string(mUseRewriteAggResultPtr) << "]\n\n";
    oss << "[最终检索增强Q=" << mQuerySelection << "]\n\n";
    for (const auto& debug: mDebugInfoPtr->GetDebugInfo()) {
        oss << debug << "\n\n";
    }
    return oss.str();
}

void GeneralLLMProcessor::GetDebugInfoPostfix(AggWebResultPtr aggResultPtr)
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
}

void GeneralLLMProcessor::ReRankNew(AggWebResultPtr& aggResultPtr)
{
    if (!aggResultPtr) {
        return;
    }

    mDebugInfoPtr->AddDebug("[domain]" + aggResultPtr->mDomain + "\n");

    vector<RankNode> rankVec;
    vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
    for (size_t i = 0; i < docPtrVec.size(); ++i) {
        if (docPtrVec[i]->mBody.empty()) {
            continue;
        }
        RankNode rankNode;
        rankNode.mDocPtr = docPtrVec[i];
        rankNode.mRankScore = docPtrVec[i]->mRankScore;
        rankNode.mPr = docPtrVec[i]->mPR;

        if (docPtrVec[i]->mUrl.find("wenda_selected ") != std::string::npos) {
            rankNode.mNeedRank = false;
        }

        rankVec.push_back(rankNode);
    }

    // 重置PR分
    ReSetPR(rankVec, aggResultPtr->mQuery);

    // 正文长度调权
    ReRankByBodyLen(rankVec);

    // 时效调权
    ReRankByTime(rankVec);

    // 站点调权
    ReRankByHost(rankVec, aggResultPtr->mDomain);

    // 知识分级调权
    ReRankByKnowledgeGrade(rankVec, aggResultPtr->mQuery);

    for (size_t i = 0; i < rankVec.size(); ++i) {
        AGG_LOG(ERROR, "RankVec:%d, %s", (int32_t)i, rankVec[i].toString().c_str());
        rankVec[i].mDocPtr->mNewRankScore = rankVec[i].mRankScore;
    }

    //return;

    // 最终顺序
    auto cmp = [](RankNode& x, RankNode& y) {
        return x.mRankScore > y.mRankScore;
    };
    sort(rankVec.begin(), rankVec.end(), cmp);

    // 请求答案定位模型
    RequestAnswerLocateModel(rankVec, aggResultPtr->mQuery, 10);

    // 截断
    int validDocNum = 0;
    bool flag = false;
    vector<LLMDocPtr> docPtrVecFinal;
    for (size_t i=0; i < rankVec.size(); ++i) {
        auto &docPtr = rankVec[i].mDocPtr;
        docPtr->mPR = rankVec[i].mPr;
        docPtr->mRankScore = rankVec[i].mRankScore;
        docPtr->mDebugInfo = rankVec[i].toString();

        if (docPtr->mRankScore < 10.0 || docPtr->mPR < 0.1 || (docPtr->mQtc <= 1.7 && docPtr->mPR < 0.2) || (docPtr->mQtc <= 0.7 && docPtr->mPR <= 0.35)) {
            AGG_LOG(INFO, "***rerank remove doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
            docPtr->mRankScore = 0.0;
        } else {
            if (docPtr->mSummary=="无答案") {
                AGG_LOG(INFO, "***rerank al remove doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
                if (validDocNum<3) {
                    flag = true;
                }
            }
            else {
                AGG_LOG(INFO, "***rerank output doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
                docPtrVecFinal.push_back(docPtr);
                validDocNum += 1;
            }
        }
    }

    if (flag) {
        for (size_t i=0; i < docPtrVecFinal.size(); ++i) {
            auto docPtr = docPtrVecFinal[i];
            docPtr->mUrl = "newStrategy " + docPtr->mUrl;
        }
    }

    if (mBucket.find("debug_show_all=1") != std::string::npos) {
        docPtrVecFinal.clear();
        for (size_t i=0; i < rankVec.size(); ++i) {
            auto &docPtr = rankVec[i].mDocPtr;
            docPtrVecFinal.push_back(docPtr);
        }
    }

    aggResultPtr->mDocPtrVec = docPtrVecFinal;
}

void GeneralLLMProcessor::RequestAnswerLocateModel(vector<RankNode> &rankVec, const string &query, uint32_t topN)
{
    if (query.empty())
        return;

    vector<uint32_t> idxVec;
    vector<HttpResponsePtr> alResponsePtrVec;
    for (uint32_t idx = 0; idx < rankVec.size() && idx<topN; ++idx) {
        string title = rankVec[idx].mDocPtr->mTitle;
        string body = rankVec[idx].mDocPtr->mBody;

        if (body.size() > 0) {
            HttpResponsePtr alResponse = AsyncRequestSummaryNew(mAggContext, query, title, body);
            if (alResponse) {
                alResponsePtrVec.emplace_back(alResponse);
                idxVec.emplace_back(idx);
            }
        }
        else {
            HttpResponsePtr alResponse = AsyncRequestSummaryNew(mAggContext, query, title, "1");
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
        auto &docPtr = rankVec[idxVec[i]].mDocPtr;
        ResultPtr result = ResultPtr(new LLMResult());
        if (alResponsePtrVec[i]->GetResult(result)) {
            LLMResultPtr llmResultPtr = std::dynamic_pointer_cast<LLMResult>(result);
            string alResult = llmResultPtr->mContent;
            /* AGG_LOG(ERROR, "get alResult succeed, query=%s, title=%s, passage=%s, alResult=%s", query.c_str(), docPtr->mTitle.c_str(), docPtr->mBody.c_str(), alResult.c_str()); */

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
        /* if (summaryNew != "无答案") { */
            /* docPtr->mBody = summaryNew; //测试，用summary替代body */
        /* } */
        /* AGG_LOG(ERROR, "summaryNew [%s]", summaryNew.c_str()); */

    }
    // 未请求AL的doc，用body填充summary
    for (uint32_t idx = 0; idx < rankVec.size(); ++idx) {
        auto &docPtr = rankVec[idx].mDocPtr;
        string summaryNew = docPtr->mSummary;
        if (summaryNew.size() == 0) {
            docPtr->mSummary = docPtr->mBody;
        }
    }
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "Wait AL COST:%d", end_time - start_time);

    return;

}

void GeneralLLMProcessor::ReSetPR(vector<RankNode> &rankVec, const string &query)
{
    if (query.empty())
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "qd_correlation_model");

    vector<uint32_t> idxVec;
    vector<HttpResponsePtr> responsePtrVec;
    for (uint32_t idx = 0; idx < rankVec.size(); ++idx) {
        string title = rankVec[idx].mDocPtr->mTitle;
        string passage = rankVec[idx].mDocPtr->mBody;
        PassageClean(passage);

        //AGG_LOG(ERROR, "POST PR NEW[%s][%s][%s][%s]", query.c_str(), rankVec[idx].mDocPtr->mUrl.c_str(), title.c_str(), passage.c_str());

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
        double pr_score = qdResult->mScore;
        rankVec[idxVec[i]].mRankScore -= rankVec[idxVec[i]].mDocPtr->mPR * 20;
        rankVec[idxVec[i]].mPr = pr_score;
        rankVec[idxVec[i]].mRankScore += pr_score * 10;
        //AGG_LOG(ERROR, "GET PR NEW[%s][%f]", rankVec[idxVec[i]].mDocPtr->mUrl.c_str(), pr_score);
    }

    return;
}

void GeneralLLMProcessor::ReRankByBodyLen(vector<RankNode> &rankVec)
{
    vector<int> bodySizeVec;
    for (size_t i = 0; i < rankVec.size(); ++i) {
        bodySizeVec.push_back((int)rankVec[i].mDocPtr->mBody.size());
    }
    sort(bodySizeVec.begin(), bodySizeVec.end(), greater<int>());
    int sumTopBodySize = 0;
    for (size_t i=1; i < bodySizeVec.size() && i <= 5; ++i) {
        sumTopBodySize += bodySizeVec[i];
    }
    double avgTopBodySize = 0.0;
    if (bodySizeVec.size() > 1) {
        avgTopBodySize = (double) sumTopBodySize / std::min(5, (int)bodySizeVec.size() - 1);
    }

    for (size_t i = 0; i < rankVec.size(); ++i) {
        if (!rankVec[i].mNeedRank)
            continue;

        auto& docPtr = rankVec[i].mDocPtr;
        double& mRankScore = rankVec[i].mRankScore;
        double oriRankScore = mRankScore;
        double qtc = docPtr->mQtc;
        double pr  = rankVec[i].mPr;

        int bodySize = docPtr->mBody.size();
        if (bodySize < avgTopBodySize / 3.0) {
            AGG_LOG(INFO, "punish for too short for 0.6 [%s]", docPtr->mUrl.c_str());
            mRankScore -= 5;
            if (qtc <= 1.8 && pr <= 0.9) {
                mRankScore -= 3;
            }
        } else if (bodySize < avgTopBodySize / 2.0) {
            AGG_LOG(INFO, "punish for too short for 0.8 [%s]", docPtr->mUrl.c_str());
            mRankScore -= 3;
            if (qtc <= 1.8 && pr <= 0.9) {
                mRankScore -= 2;
            }
        } else if (bodySize < avgTopBodySize / 1.5) {
            AGG_LOG(INFO, "punish for too short for 0.9 [%s]", docPtr->mUrl.c_str());
            mRankScore -= 1;
            if (qtc <= 1.8 && pr <= 0.9) {
                mRankScore -= 2;
            }
        }

        rankVec[i].mDeltaBodyLen = mRankScore - oriRankScore;
    }
}

void GeneralLLMProcessor::ReRankByTime(vector<RankNode> &rankVec)
{
    for (size_t i = 0; i < rankVec.size(); ++i) {
        if (!rankVec[i].mNeedRank)
            continue;

        auto& docPtr = rankVec[i].mDocPtr;
        double& mRankScore = rankVec[i].mRankScore;
        double oriRankScore = mRankScore;
        double qtc = docPtr->mQtc;
        double pr  = rankVec[i].mPr;

        string time_format = docPtr->mPublishTime;
        if (time_format.find("2023") != std::string::npos
                || time_format.find("2024") != std::string::npos) {
            mRankScore += 2;
            if (qtc > 1.0 && pr > 0.7) {
                mRankScore += 3;
            }
        } else if (time_format.find("2022") != std::string::npos) {
            mRankScore += 1;
            if (qtc > 1.0 && pr > 0.7) {
                mRankScore += 2;
            }
        } else if (time_format.find("2021") != std::string::npos) {
            ;
        } else if (time_format.find("2020") != std::string::npos) {
            mRankScore -= 1;
            if (qtc <= 1.8 && pr <= 0.9) {
                mRankScore -= 2;
            }
        } else {
            mRankScore -= 2;
            if (qtc <= 1.8 && pr <= 0.9) {
                mRankScore -= 2;
            }
        }

        rankVec[i].mDeltaTime = mRankScore - oriRankScore;
    }
}

void GeneralLLMProcessor::ReRankByHost(vector<RankNode> &rankVec, const string& domain)
{
    AggDataWrapper data("host_authority_data");
    auto hostAuthorityDataPtr = data.Get<HostAuthorityData>();
    if (!hostAuthorityDataPtr)
        return;

    for (size_t i = 0; i < rankVec.size(); ++i) {
        if (!rankVec[i].mNeedRank)
            continue;

        auto& docPtr = rankVec[i].mDocPtr;
        double& mRankScore = rankVec[i].mRankScore;
        double oriRankScore = mRankScore;
        double qtc = docPtr->mQtc;
        double pr  = rankVec[i].mPr;

        int weight = 1;
        string url = docPtr->mUrl;
        if (autil::StringUtil::startsWith(url, "news ")) {
            // do nothing
        } else if (hostAuthorityDataPtr && hostAuthorityDataPtr->GetValue(domain, url, weight)) {
            AGG_LOG(INFO, "succ get host weight [%d] for domain [%s] and url [%s] with old score [%f]", weight, domain.c_str(), url.c_str(), mRankScore);
            mRankScore += 3;
            if (weight == 7) {
                if (qtc > 1.0 && pr > 0.7) {
                    mRankScore += 5;
                }
            } else if (weight == 4) {
                mRankScore += 1;
                if (qtc > 1.0 && pr > 0.7) {
                    mRankScore += 2;
                }
            }
        } else if (url.find("www.zhihu.com/question") != std::string::npos || url.find(".jingyanlib.com") != std::string::npos) {
            mRankScore -= 3;
            if (qtc <= 1.8 && pr <= 0.9) {
                AGG_LOG(INFO, "succ get common low_quality host url [%s] with old score [%f]", url.c_str(), mRankScore);
                mRankScore -= 2;
                AGG_LOG(INFO, "new score [%f]", mRankScore);
            }
        } else if (url.find(".iask.sina.com") != std::string::npos || url.find("zhidao.baidu.com") != std::string::npos || url.find("wen.baidu.com") != std::string::npos) {
            mRankScore -= 1;
            if (qtc <= 1.8 && pr <= 0.9) {
                AGG_LOG(INFO, "succ get common low_quality host url [%s] with old score [%f]", url.c_str(), mRankScore);
                mRankScore -= 2;
                AGG_LOG(INFO, "new score [%f]", mRankScore);
            }
        } else if (domain=="edu" || domain=="health" || domain=="law" || domain=="history" || domain=="policy" || domain=="finance" || domain=="travel" || domain=="game" || domain=="car" || domain=="fineart" || domain=="food" || domain=="society" || domain=="shopping" || domain=="soft" || domain=="3c" || domain=="sports") {
            mRankScore -= 1;
            if (qtc <= 1.8 && pr <= 0.9) {
                AGG_LOG(INFO, "not common host url [%s] with old score [%f]", url.c_str(), mRankScore);
                mRankScore -= 2;
                AGG_LOG(INFO, "new score [%f]", mRankScore);
            }
        }

        rankVec[i].mHostWeight = weight;
        rankVec[i].mDeltaHost = mRankScore - oriRankScore;
    }
}

void GeneralLLMProcessor::ReRankByKnowledgeGrade(vector<RankNode>& rankVec, const string& query)
{
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "knowledge_grade_model");
    vector<uint32_t> idxVec;
    vector<HttpResponsePtr> responsePtrVec;

    for (size_t i = 0; i < rankVec.size(); ++i) {
        if (!rankVec[i].mNeedRank)
            continue;

        auto& docPtr = rankVec[i].mDocPtr;
        double& mRankScore = rankVec[i].mRankScore;
        double oriRankScore = mRankScore;
        double qtc = docPtr->mQtc;
        double pr  = rankVec[i].mPr;

        if (mRankScore < 10.0 || pr < 0.1 || (qtc <= 1.7 && pr < 0.2) || (qtc <= 0.7 && pr <= 0.35)) {
            continue;
        }

        HttpRequestPtr httpRequest(new KnowledgeGradeRequest(query, docPtr->mBody));
        //AGG_LOG(ERROR, "POST KG NEW[%s][%s][%s]", docPtr->mUrl.c_str(), query.c_str(), docPtr->mBody.c_str());
        HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
        if (response) {
            responsePtrVec.push_back(response);
            idxVec.push_back(i);
        }
    }

    for (size_t ii = 0; ii < idxVec.size(); ++ii) {
        size_t idx = idxVec[ii];
        auto& docPtr = rankVec[idx].mDocPtr;
        double& mRankScore = rankVec[idx].mRankScore;
        double oriRankScore = mRankScore;
        double qtc = docPtr->mQtc;
        double pr  = rankVec[idx].mPr;

        if (!responsePtrVec[ii])
            continue;

        ResultPtr result(new KnowledgeGradeResult);
        if (!responsePtrVec[ii]->GetResult(result)) {
            AGG_LOG(ERROR, "visit knowledge grade failed.");
            continue;
        }

        KnowledgeGradeResultPtr knowledgeGradeResult = std::dynamic_pointer_cast<KnowledgeGradeResult>(result);

        // 知识分级需要的变量
        std::vector<std::string> mKgLabels = knowledgeGradeResult -> mLabels;
        std::vector<double> mKgScores = knowledgeGradeResult -> mScores;

        //AGG_LOG(ERROR, "GET KG NEW[%s][%s]", docPtr->mUrl.c_str(), autil::StringUtil::toString(mKgScores).c_str());

        std::string mKgScoreStr = "";
        double kgAlpha = 0.0;  // 整体的分值权重

        // 以下参数需要后续依靠 绝影配置文件来读取，第一版本先hard code
        // 整体标签：
        // 举例阐述,解释说明,过于精简,文不对题,大量冗余,数据统计,个人观点,口语化,参考引用,资料权威,分条分步阐述,营销号推广,综合全面,明确答案,信息缺失,格式不清晰
        std::vector<int> kgOrderValidVecoter {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};  // 标签是否可用
        std::vector<double> kgPunishVecoter {0.7176, 0.7599, 0.9594, 0.7502, 0.7774, 0.8516, 0.8493, 0.7478, 0.8791, 0.9069, 0.8559, 0.7426, 0.8078, 0.8275, 0.7787, 0.6260};  // 代表了排序的可信度，暂且是GAUC，但是有些影响较大，则需要降权
        std::vector<double> kgWeightVecoter {1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 3.0, 2.0, 3.0, 1.0, 1.0, 1.0, 1.0, 2.0};  // 代表标签本身的重要程度（人为拍定）
        std::vector<int> kgPosNegVecoter {1, 1, -1, -1, -1, 1, -1, -1, 1, 1, 1, -1, 1, 1, -1, -1};  // 标签的正负性
        double maxQTC = 1.8;
        double maxPR = 0.9;
        double midQTC = 1.0;
        double midPR = 0.7;

        AGG_LOG(INFO, "[KG_DEBUG_SUCCESS] request knowledge grade service success!");

        // 根据相关性、有用性调整每个类别的影响情况：
        if (qtc >= maxQTC && pr >= maxPR) {
            // 特别相关且有用的，减分的权重要降低，减分要慎重
            kgAlpha = 3.0;
            for (size_t i = 0; i < kgWeightVecoter.size(); i++){
                if (kgPosNegVecoter[i] == -1) kgWeightVecoter[i] /= kgAlpha;
            }
        } else if ((qtc >= midQTC && pr >= midPR)) {
            // 比较相关且有用的
            kgAlpha = 3.0;
        // } else if (docPtr->mQtc >= maxQTC) {
        //     // 特别相关的
        //     kgAlpha = 3.0;
        // } else if (docPtr->mQtc >= midQTC) {
        //     // 比较相关的，有用性差一点
        //     kgAlpha = 2.0;
        } else if (pr >= midPR) {
            // 相关性都一般，有用性比较高
            kgAlpha = 1.0;
        } else {
            // 其他情况就是有用性相关性都不怎么高，那加分权重降低一半，扣分不变
            kgAlpha = 1.0;
            for (size_t i = 0; i < kgWeightVecoter.size(); i++){
                if (kgPosNegVecoter[i] == 1) kgWeightVecoter[i] /= 2.0;
            }
        }

        double kgUnionScore = 0.0;
        for (size_t i=0; i < mKgLabels.size(); i++){
            kgUnionScore += kgAlpha * double(kgOrderValidVecoter[i]) * kgPunishVecoter[i] * kgWeightVecoter[i] * double(kgPosNegVecoter[i]) * mKgScores[i];
        }

        mRankScore += kgUnionScore;

        // 将知识分级的字符串拼接起来，打日志输出用kgScoreStr
        std::string sepToken = ",";
        std::ostringstream kgScoreStr;
        for (size_t i=0; i < mKgLabels.size(); i++){
            if (i != 0) kgScoreStr << sepToken;
            std::size_t pos = mKgLabels[i].find(":");
            kgScoreStr << mKgLabels[i].substr(pos+1) << ":" << std::setprecision(3) << std::fixed<< mKgScores[i];
        }

        mKgScoreStr = kgScoreStr.str();
        docPtr->mKgScoreStr = mKgScoreStr;

        rankVec[idx].mDeltaKg = mRankScore - oriRankScore;
    }
}

void GeneralLLMProcessor::ReRankOld(AggWebResultPtr& aggResultPtr)
{
    // 重置PR分
    vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
    string query = aggResultPtr->mQuery;
    string domain = aggResultPtr->mDomain;
    vector<int> bodySizeVec;
    for (size_t i=0; i < docPtrVec.size(); ++i) {
        auto docPtr = docPtrVec[i];
        if (docPtr->mBody.size() > 0 && docPtr->mTitle.size() && query.size() > 0) {
            string passage = docPtr->mBody;
            PassageClean(passage);
            AGG_LOG(INFO, "passage=[%s]", passage.c_str());
            string summary;
            double pr_score = 0.0;
            // if (RequestSummary(mAggContext, query, docPtr->mTitle, passage, summary, pr_score)) {
            //     AGG_LOG(INFO, "replace PR_score [%f] with [%f] for old mRankScore [%f]", docPtr->mPR, pr_score, docPtr->mRankScore);
            //     docPtr->mRankScore -= docPtr->mPR * 20;
            //     docPtr->mPR = pr_score;
            //     docPtr->mRankScore += docPtr->mPR * 20;
            //     AGG_LOG(INFO, "new mRankScore [%f]", docPtr->mRankScore);
            // }
            if (RequestSummaryQdoc(mAggContext, query, docPtr->mTitle, passage, summary, pr_score)) {
                AGG_LOG(INFO, "replace PR_score [%f] with [%f] for old mRankScore [%f]", docPtr->mPR, pr_score, docPtr->mRankScore);
                //AGG_LOG(ERROR, "POST PR OLD[%s][%s][%s][%s]", query.c_str(), docPtr->mUrl.c_str(), docPtr->mTitle.c_str(), passage.c_str());
                docPtr->mRankScore -= docPtr->mPR * 20;
                docPtr->mPR = pr_score;
                docPtr->mRankScore += docPtr->mPR * 10;
                AGG_LOG(INFO, "new mRankScore [%f]", docPtr->mRankScore);
                //AGG_LOG(ERROR, "GET PR OLD[%s][%f]", docPtr->mUrl.c_str(), pr_score);
            }
            bodySizeVec.push_back((int) docPtr->mBody.size());
        }
        aggResultPtr->mMaxPr = std::max(aggResultPtr->mMaxPr, docPtr->mPR);
    }
    sort(bodySizeVec.begin(), bodySizeVec.end(), greater<int>());
    int sumTopBodySize = 0;
    for (size_t i=1; i < bodySizeVec.size() && i <= 5; ++i) {
        sumTopBodySize += bodySizeVec[i];
    }
    double avgTopBodySize = 0.0;
    if (bodySizeVec.size() > 1) {
        avgTopBodySize = (double) sumTopBodySize / std::min(5, (int)bodySizeVec.size() - 1);
    }

    // 策略调权
    AggDataWrapper data("host_authority_data");
    auto  hostAuthorityDataPtr = data.Get<HostAuthorityData>();
    for (size_t i=0; i < docPtrVec.size(); ++i) {
        auto docPtr = docPtrVec[i];
        AGG_LOG(INFO, "***rerank input doc*** [%i] with title [%s] url [%s] time [%s] rerank_sore [%f] pr_score [%f] qtc_score [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mPublishTime.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
        if (docPtr->mUrl.find("wenda_selected ") != std::string::npos) {
            continue;
        }

        // 正文长度调权
        int bodySize = docPtr->mBody.size();
        if (bodySize < avgTopBodySize / 3.0) {
            AGG_LOG(INFO, "punish for too short for 0.6 [%s]", docPtr->mUrl.c_str());
            docPtr->mRankScore -= 5;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                docPtr->mRankScore -= 3;
            }
        } else if (bodySize < avgTopBodySize / 2.0) {
            AGG_LOG(INFO, "punish for too short for 0.8 [%s]", docPtr->mUrl.c_str());
            docPtr->mRankScore -= 3;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                docPtr->mRankScore -= 2;
            }
        } else if (bodySize < avgTopBodySize / 1.5) {
            AGG_LOG(INFO, "punish for too short for 0.9 [%s]", docPtr->mUrl.c_str());
            docPtr->mRankScore -= 1;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                docPtr->mRankScore -= 2;
            }
        }
        // 时效调权
        string time_format = docPtr->mPublishTime;
        if (time_format.find("2023") != std::string::npos) {
            docPtr->mRankScore += 2;
            if (docPtr->mQtc > 1.0 && docPtr->mPR > 0.7) {
                docPtr->mRankScore += 3;
            }
        } else if (time_format.find("2022") != std::string::npos) {
            docPtr->mRankScore += 1;
            if (docPtr->mQtc > 1.0 && docPtr->mPR > 0.7) {
                docPtr->mRankScore += 2;
            }
        } else if (time_format.find("2021") != std::string::npos) {
            ;
        } else if (time_format.find("2020") != std::string::npos) {
            docPtr->mRankScore -= 1;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                docPtr->mRankScore -= 2;
            }
        } else {
            docPtr->mRankScore -= 2;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                docPtr->mRankScore -= 2;
            }
        }
        // 站点调权
        int weight = 1;
        string url = docPtr->mUrl;
        if (autil::StringUtil::startsWith(url, "news ")) {
            // do nothing
        } else if (hostAuthorityDataPtr && hostAuthorityDataPtr->GetValue(domain, url, weight)) {
            AGG_LOG(INFO, "succ get host weight [%d] for domain [%s] and url [%s] with old score [%f]", weight, domain.c_str(), url.c_str(), docPtr->mRankScore);
            docPtr->mRankScore += 3;
            if (weight == 7) {
                if (docPtr->mQtc > 1.0 && docPtr->mPR > 0.7) {
                    docPtr->mRankScore += 5;
                }
            } else if (weight == 4) {
                docPtr->mRankScore += 1;
                if (docPtr->mQtc > 1.0 && docPtr->mPR > 0.7) {
                    docPtr->mRankScore += 2;
                }
            }
        } else if (url.find("www.zhihu.com/question") != std::string::npos || url.find(".jingyanlib.com") != std::string::npos) {
            docPtr->mRankScore -= 3;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                AGG_LOG(INFO, "succ get common low_quality host url [%s] with old score [%f]", url.c_str(), docPtr->mRankScore);
                docPtr->mRankScore -= 2;
                AGG_LOG(INFO, "new score [%f]", docPtr->mRankScore);
            }
        } else if (url.find(".iask.sina.com") != std::string::npos || url.find("zhidao.baidu.com") != std::string::npos || url.find("wen.baidu.com") != std::string::npos) {
            docPtr->mRankScore -= 1;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                AGG_LOG(INFO, "succ get common low_quality host url [%s] with old score [%f]", url.c_str(), docPtr->mRankScore);
                docPtr->mRankScore -= 2;
                AGG_LOG(INFO, "new score [%f]", docPtr->mRankScore);
            }
        } else if (domain=="edu" || domain=="health" || domain=="law" || domain=="history" || domain=="policy" || domain=="finance" || domain=="travel" || domain=="game" || domain=="car" || domain=="fineart" || domain=="food" || domain=="society" || domain=="shopping" || domain=="soft" || domain=="3c" || domain=="sports") {
            docPtr->mRankScore -= 1;
            if (docPtr->mQtc <= 1.8 && docPtr->mPR <= 0.9) {
                AGG_LOG(INFO, "not common host url [%s] with old score [%f]", url.c_str(), docPtr->mRankScore);
                docPtr->mRankScore -= 2;
                AGG_LOG(INFO, "new score [%f]", docPtr->mRankScore);
            }
        }

        // 知识分级需要的变量
        std::vector<std::string> mKgLabels;
        std::vector<double> mKgScores;
        std::string mKgScoreStr = "";
        double kgAlpha = 0.0;  // 整体的分值权重

        // 以下参数需要后续依靠 绝影配置文件来读取，第一版本先hard code
        // 整体标签：
        // 举例阐述,解释说明,过于精简,文不对题,大量冗余,数据统计,个人观点,口语化,参考引用,资料权威,分条分步阐述,营销号推广,综合全面,明确答案,信息缺失,格式不清晰
        std::vector<int> kgOrderValidVecoter {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};  // 标签是否可用
        std::vector<double> kgPunishVecoter {0.7176, 0.7599, 0.9594, 0.7502, 0.7774, 0.8516, 0.8493, 0.7478, 0.8791, 0.9069, 0.8559, 0.7426, 0.8078, 0.8275, 0.7787, 0.6260};  // 代表了排序的可信度，暂且是GAUC，但是有些影响较大，则需要降权
        std::vector<double> kgWeightVecoter {1.0, 2.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 3.0, 2.0, 3.0, 1.0, 1.0, 1.0, 1.0, 2.0};  // 代表标签本身的重要程度（人为拍定）
        std::vector<int> kgPosNegVecoter {1, 1, -1, -1, -1, 1, -1, -1, 1, 1, 1, -1, 1, 1, -1, -1};  // 标签的正负性
        double maxQTC = 1.8;
        double maxPR = 0.9;
        double midQTC = 1.0;
        double midPR = 0.7;

        // 这里代表了最终的过滤策略，以下分值的数据是被过滤掉的，因此知识分级也不对其作出加分
        if (docPtr->mRankScore < 10.0 || docPtr->mPR < 0.1 || (docPtr->mQtc <= 1.7 && docPtr->mPR < 0.2) || (docPtr->mQtc <= 0.7 && docPtr->mPR <= 0.35)) {
            AGG_LOG(INFO, "[KG_DEBUG_IGNORE] don't request knowledge grade because of low quality, id=[%i], query=[%s], title=[%s], oldRankScore=[%f]", i, query.c_str(), docPtr->mTitle.c_str(), docPtr->mRankScore);
        } else {
            if (!RequestKnowledgeGradeResult(mAggContext, query, docPtr -> mBody, mKgLabels, mKgScores)){
                AGG_LOG(ERROR, "[KG_DEBUG_ERROR] request knowledge grade failed! id=[%i], query=[%s], body=[%s], oldRankScore=[%f]", i, query.c_str(), docPtr->mBody.c_str(), docPtr->mRankScore);
            } else {
                AGG_LOG(INFO, "[KG_DEBUG_SUCCESS] request knowledge grade service success!");

                // 根据相关性、有用性调整每个类别的影响情况：
                if (docPtr->mQtc >= maxQTC && docPtr->mPR >= maxPR) {
                    // 特别相关且有用的，减分的权重要降低，减分要慎重
                    kgAlpha = 3.0;
                    for (size_t i = 0; i < kgWeightVecoter.size(); i++){
                        if (kgPosNegVecoter[i] == -1) kgWeightVecoter[i] /= kgAlpha;
                    }
                } else if ((docPtr->mQtc >= midQTC && docPtr->mPR >= midPR)) {
                    // 比较相关且有用的
                    kgAlpha = 3.0;
                // } else if (docPtr->mQtc >= maxQTC) {
                //     // 特别相关的
                //     kgAlpha = 3.0;
                // } else if (docPtr->mQtc >= midQTC) {
                //     // 比较相关的，有用性差一点
                //     kgAlpha = 2.0;
                } else if (docPtr->mPR >= midPR) {
                    // 相关性都一般，有用性比较高
                    kgAlpha = 1.0;
                } else {
                    // 其他情况就是有用性相关性都不怎么高，那加分权重降低一半，扣分不变
                    kgAlpha = 1.0;
                    for (size_t i = 0; i < kgWeightVecoter.size(); i++){
                        if (kgPosNegVecoter[i] == 1) kgWeightVecoter[i] /= 2.0;
                    }
                }

                double kgUnionScore = 0.0;
                for (size_t i=0; i < mKgLabels.size(); i++){
                    kgUnionScore += kgAlpha * double(kgOrderValidVecoter[i]) * kgPunishVecoter[i] * kgWeightVecoter[i] * double(kgPosNegVecoter[i]) * mKgScores[i];
                }
                auto oldRankScore = docPtr->mRankScore;
                docPtr->mRankScore += kgUnionScore;

                // 将知识分级的字符串拼接起来，打日志输出用kgScoreStr
                std::string sepToken = ",";
                std::ostringstream kgScoreStr;
                for (size_t i=0; i < mKgLabels.size(); i++){
                    if (i != 0) kgScoreStr << sepToken;
                    std::size_t pos = mKgLabels[i].find(":");
                    kgScoreStr << mKgLabels[i].substr(pos+1) << ":" << std::setprecision(3) << std::fixed<< mKgScores[i];
                }

                AGG_LOG(
                    INFO, "[KG_DEBUG_FINAL] id=[%i], title=[%s], oldRankScore=[%f], kgUnionScore=[%f], newRankScore=[%f], mQTC=[%f], mPR=[%f], kgScoreList=[{%s}]",
                    i, docPtr->mTitle.c_str(), oldRankScore, kgUnionScore, docPtr->mRankScore, docPtr->mQtc, docPtr->mPR, kgScoreStr.str().c_str()
                );
                mKgScoreStr = kgScoreStr.str();
            }
        }
        docPtr->mKgScoreStr = mKgScoreStr;
        // 网页类型调权
    }

    // 最终顺序
    auto cmp = [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    };
    sort(docPtrVec.begin(), docPtrVec.end(), cmp);

    // 截断
    vector<LLMDocPtr> docPtrVecFinal;
    for (size_t i=0; i < docPtrVec.size(); ++i) {
        auto docPtr = docPtrVec[i];
        if (docPtr->mRankScore < 10.0 || docPtr->mPR < 0.1 || (docPtr->mQtc <= 1.7 && docPtr->mPR < 0.2) || (docPtr->mQtc <= 0.7 && docPtr->mPR <= 0.35)) {
            AGG_LOG(INFO, "***rerank remove doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
            docPtr->mRankScore = 0.0;
        } else {
            AGG_LOG(INFO, "***rerank output doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
            docPtrVecFinal.push_back(docPtr);
        }
    }
    aggResultPtr->mDocPtrVec = docPtrVecFinal;
}

void GeneralLLMProcessor::DeDuplicate(AggWebResultPtr& aggResultPtr)
{
    vector<LLMDocPtr>& docPtrVec = aggResultPtr->mDocPtrVec;
    if (docPtrVec.size() <= 1) {
        AGG_LOG(INFO, "no need to dedup");
        return;
    }
    AGG_LOG(INFO, "items before dedup count [%d]", docPtrVec.size());

    // prepare data
    vector<vector<string>> tokenVecVec;
    vector<set<string>> tokenSetVec;
    for (size_t i=0; i < docPtrVec.size(); ++i) {
        string body = docPtrVec[i]->mBody;
        vector<string> tokens;
        StringDeduper::SplitToWords(body, tokens, true, true);
        tokenVecVec.push_back(tokens);
        vector<string> tg = StringDeduper::getNgrams(tokens, 2);
        set<string> ts = StringDeduper::transVec2Set(tg);
        tokenSetVec.push_back(ts);
    }

    // to dedup
    size_t topK = 10; // 只对 topK 进行去重，避免性能损耗
    std::set<size_t> removeSet;
    for (size_t i=0; i < docPtrVec.size() && i < topK; ++i) {
        for (size_t j=0; j < i; ++j) {
            float jac_sim = StringDeduper::jaccard(tokenSetVec[i], tokenSetVec[j]);
            if (jac_sim >= 0.85) {
                AGG_LOG(INFO, "item [%d]-[%s] is similar with before-item [%d]-[%s] by jac_sim [%f]",
                        i, docPtrVec[i]->mUrl.c_str(), j, docPtrVec[j]->mUrl.c_str(), jac_sim);
                AGG_LOG(INFO, "[%s]", docPtrVec[i]->mBody.c_str());
                AGG_LOG(INFO, "[%s]", docPtrVec[j]->mBody.c_str());
                removeSet.insert(i);
                break;
            }
        }
    }

    // output
    vector<LLMDocPtr> docPtrVecFinal;
    for (size_t i=0; i < docPtrVec.size(); ++i) {
        auto docPtr = docPtrVec[i];
        if (removeSet.count(i)) {
            AGG_LOG(DEBUG, "***dedup remove doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
        } else {
            AGG_LOG(DEBUG, "***dedup output doc*** [%i] with title [%s] url [%s] rerank_sore [%f] mPR [%f] mQtc [%f]", docPtr->mAggRank, docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mRankScore, docPtr->mPR, docPtr->mQtc);
            docPtrVecFinal.push_back(docPtr);
        }
    }

    aggResultPtr->mDocPtrVec = docPtrVecFinal;
    AGG_LOG(INFO, "items after dedup count [%d]", docPtrVecFinal.size());
}

void GeneralLLMProcessor::PassageClean(std::string& passage)
{
    passage = StringTool::ReplaceStr(passage, "\n", "`");
    passage = StringTool::ReplaceStr(passage, "“", "\"");
    passage = StringTool::ReplaceStr(passage, "”", "\"");
    passage = StringTool::RemoveStr(passage, "　");
    passage = StringTool::RemoveStr(passage, " ");
    passage = StringTool::RemoveStr(passage, "\t");
}

bool GeneralLLMProcessor::GetIndexedPassage(const string &doc, string &indexed_passage, vector<string> &sentences, vector<pair<int, int> > &sent_group) {
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

bool GeneralLLMProcessor::ReCatAnswer(const string &alResult, const string& query, const string& title, const string& passage, string& answer) {
    if (alResult.find("无")!=string::npos) {
        answer = "无答案";
        return true;
    }

    string doc = title + "\n" + passage;
    string indexed_passage = "";
    vector<string> sentences;
    vector<pair<int, int> > sent_group;
    GetIndexedPassage(doc, indexed_passage, sentences, sent_group);
    AGG_LOG(ERROR, "alResult=%s", alResult.c_str());

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
        for (size_t i = 0; i < alNumResVec.size(); i++) {
            if (alNumResVec[i]-1 >= 0 && alNumResVec[i]-1 < int(sentences.size())) {
                answer += sentences[alNumResVec[i]-1]; // 句子粒度答案
            }
        }
    }
    catch(exception& e) {
        AGG_LOG(ERROR, "alResult format error, alResult=%s", alResult.c_str());
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

int GeneralLLMProcessor::GetUtf8LetterNumber(const string &s)
{
    int j = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        if ((s[i] & 0xc0) != 0x80) j++;
    }
    return j;
}

AGG_END_NAMESPACE(agg_web2)
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(medical)::GeneralLLMProcessor)

