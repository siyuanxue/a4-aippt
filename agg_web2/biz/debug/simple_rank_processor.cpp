#include <agg_web2/biz/debug/simple_rank_processor.h>
#include "agg_web2/common/common.h"
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <jsoncpp/json/json.h>
#include <agg_web2/data/value_data.h>
#include <agg_web2/upstream/qt/qt_request.h>
#include <agg_web2/upstream/qt/qt_result.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg_web2/upstream/common/common_request.h>
#include <agg_web2/upstream/common/common_result.h>
#include <agg2/util/string_tool.h>
#include <agg_web2/data/key_value_data.h>

#include <agg_web2/upstream/qc/qc_feature_result.h>
#include <agg_web2/util/QtcExtractor.h>
#include <agg_web2/data/data_type.h>
#include <agg2/wrapper/agg_data_wrapper.h>
#include <autil/StringUtil.h>
#include <A4/util/hash_algorithm.h>
#include <agg_web2/helper/general_search_helper.h>
#include <agg_web2/util/string_deduper.h>
#include <agg2/helper/time_helper.h>
#include <agg_web2/upstream/qdoc_correlation/qd_request.h>
#include <agg_web2/upstream/qdoc_correlation/qd_result.h>

#include <agg2/upstream/llm_result.h>
#include <agg2/upstream/llm_request.h>
#include <agg_web2/util/AggWebStringTool.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, SimpleRankProcessor);

BEGIN_DEFINE_SERIALIZATION_FUNC(SimpleRankProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(SimpleRankProcessor)
const std::string QTC_NODE_PARAM_KEY = "qtc_bert_node";
const static string PARAM_GENERAL_LLM_SYNC_NODE = "general_llm_sync_server";
const static string PARAM_ANSWER_LOCATE_LLM_MODEL_NAME = "answer_locate_llm_name";
const static string PARAM_LLM_API_KEY = "llm_api_key";

SimpleRankProcessor::SimpleRankProcessor()
{
}

SimpleRankProcessor::~SimpleRankProcessor()
{
}

bool SimpleRankProcessor::DoInit(const String2String & params)
{
    PARSE_PROCESSOR_PARAM(params, QTC_NODE_PARAM_KEY, mQTCNode);
    PARSE_PROCESSOR_PARAM(params, PARAM_GENERAL_LLM_SYNC_NODE, mGeneralLLMSyncNode);
    PARSE_PROCESSOR_PARAM(params, PARAM_ANSWER_LOCATE_LLM_MODEL_NAME, mAnswerLocateLLMName);
    PARSE_PROCESSOR_PARAM(params, PARAM_LLM_API_KEY, mApiKey);
    return true;
}

void SimpleRankProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

int SimpleRankProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    int64_t end_time;
    
    std::string query, title, body;
    mAggContext->GetRequestParam("query", query);
    mAggContext->GetRequestParam("title", title);
    mAggContext->GetRequestParam("body", body);
    
    std::vector<LLMDocPtr> hitQueueAgg;
    if (!title.empty() && !body.empty()) {
        hitQueueAgg.emplace_back(new LLMDoc(title, body, "", 0, 0.0, 0.0, 0.0, "", false));
    }
    
    if (true) {
        std::vector<LLMDocPtr> tmpAgg;
        mAggContext->GetValue("RAWQUERY_WEB_AGG_RESULT", tmpAgg);
        if (!tmpAgg.empty()) {
            //hitQueueAgg.insert(hitQueueAgg.end(), tmpAgg.begin(), tmpAgg.end());
        }

        tmpAgg.clear();
        mAggContext->GetValue("RAWQUERY_INDEX_RESULT", tmpAgg);
        if (!tmpAgg.empty()) {
            hitQueueAgg.insert(hitQueueAgg.end(), tmpAgg.begin(), tmpAgg.end());
        }

        /*std::vector<LLMDocPtr> hitFilter;
        for (auto docPtr : hitQueueAgg) {
            if (docPtr->mUrl.find("baike.com") != string::npos) {
                hitFilter.push_back(docPtr);
            }
        }
        hitQueueAgg.clear();
        hitQueueAgg = hitFilter;*/
    }

    GetAggResult(hitQueueAgg);

    ReRank(hitQueueAgg);

    //RequestAnswerLocateModel(hitQueueAgg, 10);

    FormatValidResult(query, hitQueueAgg);

    end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
    return PR_SUCCESS;
}

// 有效返回结果
void SimpleRankProcessor::FormatValidResult(const std::string rawQuery, std::vector<LLMDocPtr>& hitQueueAgg) 
{
    Json::Value root;
    Json::Value search_doc = Json::Value(Json::arrayValue);
    for (auto docPtr:hitQueueAgg) {
        search_doc.append(Json::Value());
        Json::Value & item = search_doc[search_doc.size() - 1];
        
        item["summary"] = docPtr->mSummary;
        //item["body"] = docPtr->mBody;
        item["url"] = docPtr->mUrl;
        item["title"] = docPtr->mTitle;
        item["recall"] = docPtr->mRecall;
        item["doc_pr"] = docPtr->mPR;
        item["doc_qtc"] = docPtr->mQtc;
        item["doc_llm"] = docPtr->mLLMScore;
        item["time_str"] = docPtr->mPublishTime;
        Json::Value psg = Json::Value(Json::arrayValue);
        for (size_t j = 0; j < docPtr->mPsgVec.size(); ++j) {
            LLMPsgPtr curPsg = docPtr->mPsgVec[j];
            psg.append(Json::Value());
            Json::Value & _item = psg[psg.size() - 1];
            _item["sample_id"] = (uint32_t)curPsg->mSampleId;
            _item["pr"] = curPsg->mPR;
            _item["prv0"] = curPsg->mPRV0;
            _item["qtc"] = curPsg->mQtc;
            _item["llm_score"] = curPsg->mLLMScore;
            _item["body"] = curPsg->mPsg;

        }
        //item["z_psg"] = psg;
        Json::Value maxPsg;
        if (docPtr->mPsgVec.size() > 0) {
            LLMPsgPtr curPsg = docPtr->mPsgVec[docPtr->mMaxId];
            Json::Value & _item = maxPsg; 
            _item["sample_id"] = (uint32_t)curPsg->mSampleId;
            _item["pr"] = curPsg->mPR;
            _item["prv0"] = curPsg->mPRV0;
            _item["qtc"] = curPsg->mQtc;
            _item["llm_score"] = curPsg->mLLMScore;
            _item["body"] = curPsg->mPsg;

        }
        item["best"] = maxPsg;
    }

    root["query"] = rawQuery;
    root["message"] = "succ";
    root["data"] = search_doc;

    Json::FastWriter writer;
    std::string resContent = writer.write(root);

    AGG_LOG(INFO, "resContent=%s", resContent.c_str());
    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

// 空结果返回
void SimpleRankProcessor::FormatEmptyResult(const std::string message)
{
    Json::Value root;
    Json::Value search_doc = Json::Value(Json::arrayValue);
    root["search_doc"] = search_doc;

    std::string rawQuery;
    mAggContext->GetRequestParam("query", rawQuery);
    if (!rawQuery.empty())
        rawQuery = StringTool::ReplaceStr(rawQuery, "\n", "");
    root["query"] = rawQuery;
    root["message"] = message;

    Json::FastWriter writer;
    std::string resContent = writer.write(root);
    AGG_LOG(INFO, "resContent=%s", resContent.c_str());
    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

// 请求QT服务
std::vector<double> SimpleRankProcessor::GetQtScores(AggContext* aggContext, const std::string& query,
        const std::vector<std::string>& titles)
{
    HttpRequestPtr httpRequest(new QtRequest(query, titles));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(aggContext, "qt_model/ts");
    ResultPtr result(new QtResult);
    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
        AGG_LOG(ERROR, "GetSimQuery qt failed");
        vector<double> empty;
        return empty;
    }
    QtResultPtr qtResult = std::dynamic_pointer_cast<QtResult>(result);
    return qtResult->mQtScores;
}

bool SimpleRankProcessor::GetAggResult(std::vector<LLMDocPtr>& hitQueueAgg)
{
    //mAggContext->GetValue("RAWQUERY_WEB_AGG_RESULT", hitQueueAgg);
    //AGG_LOG(DEBUG, "AGG start recall size=%d", hitQueueAgg.size());

    // DeDuplicate(hitQueueAgg, 0.99);

    std::string Source = "AGG";
    GetRerankResult(hitQueueAgg, Source);
    //mAggContext->SetValue(HIT_QUEUE_AGG, hitQueueAgg);
    //AGG_LOG(DEBUG, "AGG recall size=%d", hitQueueAgg.size());

    return true;
}

void SimpleRankProcessor::GetRerankResult(std::vector<LLMDocPtr>& docPtrVec, std::string& Source)
{
    SplitDoc2Psg(docPtrVec);
    AGG_LOG(ERROR, "rerank recall size=%d", docPtrVec.size());

    GetScores(docPtrVec);
    AGG_LOG(ERROR, "rerank recall size=%d", docPtrVec.size());
    
    //mAggContext->SetValue("RAWDATA_" + Source, docPtrVec);

    SelectDocs(docPtrVec, Source);
    AGG_LOG(ERROR, "rerank recall size=%d", docPtrVec.size());
    
    string llm_mode;
    mAggContext->GetRequestParam("llm_mode", llm_mode);
    string query = AggContextHelper::GetRawQuery(mAggContext);

    if (llm_mode == "minicpm") {
        FilterRequestPrMiniCpm(docPtrVec, query, false);
    } else if (llm_mode == "none") { 
        // do nothings
    } else {
        FilterRequestPrLLM(docPtrVec, query, false);
    }
}

void SimpleRankProcessor::SplitDoc2Psg(std::vector<LLMDocPtr>& LLMDocPtrQueue)
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
            if (psg.size() >= size_t(3 * 3)) {
                LLMPsgPtr LLMPsgPtr(new LLMPsg(idx, ++sampleId, LLMDocPtr->mTitle, psg));
                LLMDocPtr->mPsgVec.emplace_back(LLMPsgPtr);
            }
            idx = next; // overlap方式，取200个字后的passage
        }
        // if (LLMDocPtr->mRecall != "WEB_AGG") {
        //     AGG_LOG(ERROR, "SplitDoc2Psg total psg=%llu, source=%s, body=%s", num, LLMDocPtr->mRecall.c_str(), LLMDocPtr->mBody.c_str());
        // }
    }
    AGG_LOG(ERROR, "SplitDoc2Psg total psg=%llu", sampleId);
}

void SimpleRankProcessor::GetScores(std::vector<LLMDocPtr>& LLMDocPtrQueue)
{
    // 构造请求串并请求，获取段落级别的qtc、pr分数
    //std::string query = StripQueryLine(AggContextHelper::GetRawQuery(mAggContext));
    string query = AggContextHelper::GetRawQuery(mAggContext);
    if (!query.empty())
        query = StringTool::ReplaceStr(query, "\n", "");

    AGG_LOG(ERROR, "query=%s", query.c_str());
	int hashQueryNum = (int)HashAlgorithm::HashString64(query.c_str());
    
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    FilterRequestQtc(LLMDocPtrQueue, query, false);
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "Wait QTC COST:%d", end_time - start_time);
    
    start_time = end_time; 
    FilterRequestPr(LLMDocPtrQueue, query, false);
    FilterRequestPrOld(LLMDocPtrQueue, query, false);
    //FilterRequestPrLLM(LLMDocPtrQueue, query, false);
    end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "Wait PR COST:%d", end_time - start_time);

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

void SimpleRankProcessor::FilterRequestQtc(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter){
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
				if(false && qtcScores[j] < 0.1) {
					psgSample[j]->mPR = 0.0;
					psgSample[j]->mQtc = 0.0;
				}else{
					psgSample[j]->mQtc = qtcScores[j];
				}
			}
		}
	}
}

void SimpleRankProcessor::BatchRequestQTC(const std::string& query,
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

HttpResponsePtr SimpleRankProcessor::RequestQTC(std::string& qtc_request_dat)
{
    QcFeatureRequestPtr qcFeatureRequest = std::make_shared<QcFeatureRequest>(mAggContext);
    qcFeatureRequest->InitRequestStr(qtc_request_dat);
    AggHttpWrapper wrapper(mAggContext, mQTCNode);
    //AGG_LOG(DEBUG, "qtc mRequestStr=%s", qcFeatureRequest->GetRequestStr().c_str());
    return wrapper.AsyncRequest(qcFeatureRequest);
}

std::vector<double> SimpleRankProcessor::GetQTC(HttpResponsePtr httpResponse)
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

void SimpleRankProcessor::FilterRequestPr(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter)
{
    if (query.empty())
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "sgs_pr_model");

    vector<LLMPsgPtr> psgSampleVec;
    for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
        //AGG_LOG(ERROR, "psg vec:%d", (int32_t)psgVec.size());
		for (size_t j = 0; j < psgVec.size(); ++j) {
			LLMPsgPtr curPsg = psgVec[j];
			if(!NeedFilter ||  curPsg->mQtc >= 0.1) {
				psgSampleVec.emplace_back(curPsg);
			}
		}
	}
    
    AGG_LOG(ERROR, "FilterRequestPr size:%d", (int32_t)psgSampleVec.size());

    // 异步请求pr得分
    uint32_t batch_size = 300;
    uint32_t multi_count = (uint32_t)psgSampleVec.size() / batch_size + 1;
    
    for (size_t mi = 0; mi < multi_count; ++mi) { 
        uint32_t start_idx = mi * batch_size;
        
        vector<uint32_t> idxVec;
        vector<HttpResponsePtr> responsePtrVec;
        for (uint32_t idx = start_idx; idx < start_idx + batch_size && idx < psgSampleVec.size(); ++idx) {
            
            Json::Value root;
            root["query"] = query;
            root["title"] = psgSampleVec[idx]->mTitle;
            root["content"] = psgSampleVec[idx]->mPsg;
            Json::FastWriter writer;
            std::string requestStr = writer.write(root);
            
            HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
            HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
            if (response) {
                responsePtrVec.push_back(response);
                idxVec.push_back(idx);
            }
        }
        
        for (size_t i = 0; i < responsePtrVec.size(); ++i) {
            if (!responsePtrVec[i])
                continue;
            ResultPtr result(new CommonResult("sgs_pr"));
            if (!responsePtrVec[i]->GetResult(result)) {
                AGG_LOG(ERROR, "get pr score failed.");
                continue;
            }

            CommonResultPtr qdResult = std::dynamic_pointer_cast<CommonResult>(result);
            psgSampleVec[idxVec[i]]->mPR = qdResult->mScore; 
        }
    }
}

void SimpleRankProcessor::FilterRequestPrLLM(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter)
{
    if (query.empty())
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "llm_pr_model");

    vector<LLMPsgPtr> psgSampleVec;
    for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
        //AGG_LOG(ERROR, "psg vec:%d", (int32_t)psgVec.size());
		for (size_t j = 0; j < psgVec.size(); ++j) {
            if (j != LLMDocPtrQueue[i]->mMaxId)//限制只用最优的Chunk计算 
                continue;

			LLMPsgPtr curPsg = psgVec[j];
			if(!NeedFilter ||  curPsg->mQtc >= 0.1) {
				psgSampleVec.emplace_back(curPsg);
			}
		}
	}
    
    AGG_LOG(ERROR, "Visit LLM size:%d", (int32_t)psgSampleVec.size());

    // 异步请求pr得分
    uint32_t batch_size = 1;
    uint32_t multi_count = (uint32_t)psgSampleVec.size() / batch_size + 1;
    
    for (size_t mi = 0; mi < multi_count; ++mi) { 
        uint32_t start_idx = mi * batch_size;
        
        vector<uint32_t> idxVec;
        vector<HttpResponsePtr> responsePtrVec;
        for (uint32_t idx = start_idx; idx < start_idx + batch_size && idx < psgSampleVec.size(); ++idx) {
            
            string prompt_str = "请判断下文中提供的参考内容能否回答给定的问题，问题：";
            prompt_str += query;
            prompt_str += "，参考内容：" + psgSampleVec[idx]->mTitle + "\n" + psgSampleVec[idx]->mPsg;
            prompt_str = "<|im_start|>user\n" + prompt_str + "<|im_end|>\n<|im_start|>assistant\n";
            
            Json::Value root;
            root["request_id"] = "test-123";
            Json::Value prompts = Json::Value(Json::arrayValue);
            prompts.append(prompt_str);
            root["prompts"] = prompts; 
            root["max_new_tokens"] = 100;
            //root["stop_texts"] = ["<|im_start|>", "<|im_end|>", "<|endoftext|>", "\n\nHuman: "];
            Json::Value stop_texts = Json::Value(Json::arrayValue);
            stop_texts.append("<|im_start|>");
            stop_texts.append("<|im_end|>");
            stop_texts.append("<|endoftext|>");
            stop_texts.append("\n\nHuman: ");
            root["stop_texts"] = stop_texts;
            root["reply_mode"] = "sync";
            root["do_sample"] = false;
            root["skip_special_tokens"] = false;

            Json::FastWriter writer;
            std::string requestStr = writer.write(root);
            
            HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
            HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
            if (response) {
                responsePtrVec.push_back(response);
                idxVec.push_back(idx);
            }
        }
        
        for (size_t i = 0; i < responsePtrVec.size(); ++i) {
            if (!responsePtrVec[i])
                continue;
            ResultPtr result(new CommonResult("llm_pr"));
            if (!responsePtrVec[i]->GetResult(result)) {
                AGG_LOG(ERROR, "get pr score failed.");
                continue;
            }

            CommonResultPtr qdResult = std::dynamic_pointer_cast<CommonResult>(result);
            psgSampleVec[idxVec[i]]->mLLMScore = qdResult->mScore; 
        }
    }
   
    // 赋值到docPtr 
    for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
        if (LLMDocPtrQueue[i]->mMaxId >= psgVec.size())
            continue;
        LLMDocPtrQueue[i]->mLLMScore = psgVec[LLMDocPtrQueue[i]->mMaxId]->mLLMScore;
    }
}

void SimpleRankProcessor::FilterRequestPrMiniCpm(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter)
{
    if (query.empty())
        return;

    //AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "llm_minicpm_model/api?service_name=KkqSnCXG3z6y6xBJvM3Ud6&__profile_python__=False");
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "llm_minicpm_model/process");

    vector<LLMPsgPtr> psgSampleVec;
    for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
        //AGG_LOG(ERROR, "psg vec:%d", (int32_t)psgVec.size());
		for (size_t j = 0; j < psgVec.size(); ++j) {
            if (j != LLMDocPtrQueue[i]->mMaxId)//限制只用最优的Chunk计算 
                continue;

			LLMPsgPtr curPsg = psgVec[j];
			if(!NeedFilter ||  curPsg->mQtc >= 0.1) {
				psgSampleVec.emplace_back(curPsg);
			}
		}
	}
    
    AGG_LOG(ERROR, "Visit LLM size:%d", (int32_t)psgSampleVec.size());

    // 异步请求pr得分
    uint32_t batch_size = 5;
    uint32_t multi_count = (uint32_t)psgSampleVec.size() / batch_size + 1;
    
    for (size_t mi = 0; mi < multi_count; ++mi) { 
        uint32_t start_idx = mi * batch_size;
        
        vector<uint32_t> idxVec;
        vector<HttpResponsePtr> responsePtrVec;
        for (uint32_t idx = start_idx; idx < start_idx + batch_size && idx < psgSampleVec.size(); ++idx) {
            Json::Value root;
            root["cutoff_layers"] = "40";
            root["query"] = query; 
            root["passage"] = psgSampleVec[idx]->mTitle + "\n" + psgSampleVec[idx]->mPsg; 

            Json::FastWriter writer;
            std::string requestStr = writer.write(root);
            
            HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
            //httpRequest->SetUri("service_name=KkqSnCXG3z6y6xBJvM3Ud6&__profile_python__=False");
            HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
            if (response) {
                responsePtrVec.push_back(response);
                idxVec.push_back(idx);
            }
        }
        
        for (size_t i = 0; i < responsePtrVec.size(); ++i) {
            if (!responsePtrVec[i])
                continue;
            ResultPtr result(new CommonResult("llm_minicpm"));
            if (!responsePtrVec[i]->GetResult(result)) {
                AGG_LOG(ERROR, "get pr score failed.");
                continue;
            }

            CommonResultPtr qdResult = std::dynamic_pointer_cast<CommonResult>(result);
            psgSampleVec[idxVec[i]]->mLLMScore = qdResult->mScore; 
        }
    }
   
    // 赋值到docPtr 
    for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
        if (LLMDocPtrQueue[i]->mMaxId >= psgVec.size())
            continue;
        LLMDocPtrQueue[i]->mLLMScore = psgVec[LLMDocPtrQueue[i]->mMaxId]->mLLMScore;
    }
}

void SimpleRankProcessor::FilterRequestPrOld(std::vector<LLMDocPtr>& LLMDocPtrQueue, const std::string& query, bool NeedFilter)
{
    if (query.empty())
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "qd_correlation_model");

    vector<LLMPsgPtr> psgSampleVec;
    for (size_t i =0; i<LLMDocPtrQueue.size(); ++i) {
		std::vector<LLMPsgPtr>& psgVec = LLMDocPtrQueue[i]->mPsgVec;
        //AGG_LOG(ERROR, "psg vec:%d", (int32_t)psgVec.size());
		for (size_t j = 0; j < psgVec.size(); ++j) {
			LLMPsgPtr curPsg = psgVec[j];
			if(!NeedFilter ||  curPsg->mQtc >= 0.1) {
				psgSampleVec.emplace_back(curPsg);
			}
		}
	}
    
    AGG_LOG(ERROR, "FilterRequestPrOld size:%d", (int32_t)psgSampleVec.size());

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
            psgSampleVec[idxVec[i]]->mPRV0 = qdResult->mScore; 
        }
    }
}

void SimpleRankProcessor::SelectDocs(std::vector<LLMDocPtr>& LLMDocPtrQueue, std::string& Source)
{
    for (auto docPtr: LLMDocPtrQueue) {
        double maxScore = -100.0;
        for (size_t j = 0; j < docPtr->mPsgVec.size(); ++j) {
            LLMPsgPtr curPsg = docPtr->mPsgVec[j];
            double score = curPsg->mPR * 20 + curPsg->mQtc * 10;
            if (score > maxScore) {
                docPtr->mPR = curPsg->mPR;
                docPtr->mQtc = curPsg->mQtc;
                docPtr->mMaxId = j;
                maxScore = score;
            }
        }
    }
}

void SimpleRankProcessor::ReRank(std::vector<LLMDocPtr>& hitQueueAgg)
{
    string rank_type;
    mAggContext->GetRequestParam("rank_type", rank_type);
    if (rank_type.empty())
        return;

    bool needSort = false;
    if (rank_type == "qtc_pr") {
        for (auto docPtr : hitQueueAgg) {
            docPtr->mRankScore = docPtr->mQtc * 10.0 + docPtr->mPR * 20.0;
            if (docPtr->mQtc < 0.1 || docPtr->mPR < 0.1) {
                docPtr->mRankScore *= 0.1;
            }
        }
        needSort = true;
    }
    
    if (rank_type == "llm_only") {
        for (auto docPtr : hitQueueAgg) {
            docPtr->mRankScore = docPtr->mLLMScore;
        }
        needSort = true;
    }
    
    if (rank_type == "llm_qtc") {
        for (auto docPtr : hitQueueAgg) {
            docPtr->mRankScore = docPtr->mLLMScore;
            if (docPtr->mQtc < 0.1) {
                docPtr->mRankScore *= 0.1;
            }
        }
        needSort = true;
    }
    
    if (needSort) {
        sort(hitQueueAgg.begin(), hitQueueAgg.end(), [](LLMDocPtr x, LLMDocPtr y) {
            return x->mRankScore > y->mRankScore;
        });
    }
}

void SimpleRankProcessor::RequestAnswerLocateModel(vector<LLMDocPtr> &LLMDocPtrQueue, uint32_t topN)
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
                //docPtr->mSummary = docPtr->mBody;
            }
        }
        else {
            summaryNew = "fail_req";
            AGG_LOG(ERROR, "get alResult failed, query=%s, title=%s, passage=%s", query.c_str(), docPtr->mTitle.c_str(), docPtr->mBody.c_str());
            //docPtr->mSummary = docPtr->mBody;
            continue;
        }
    }
    // 未请求AL的doc，用body填充summary
    for (uint32_t idx = 0; idx < LLMDocPtrQueue.size(); ++idx) {
        auto &docPtr = LLMDocPtrQueue[idx];
        string summaryNew = docPtr->mSummary;
        if (summaryNew.size() == 0) {
            //docPtr->mSummary = docPtr->mBody;
        }
    }
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "Wait AL COST:%d", end_time - start_time);

    return;

}

HttpResponsePtr SimpleRankProcessor::AsyncRequestSummary(const std::string& query, const std::string& title, const std::string& passage)
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

bool SimpleRankProcessor::ReCatAnswer(const string &alResult, const string& query, const string& title, const string& passage, string& answer) {
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

bool SimpleRankProcessor::GetIndexedPassage(const string &doc, string &indexed_passage, vector<string> &sentences, vector<pair<int, int> > &sent_group) {
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

int SimpleRankProcessor::GetUtf8LetterNumber(const string &s)
{
    int j = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        if ((s[i] & 0xc0) != 0x80) j++;
    }
    return j;
}

BaseProcessor * SimpleRankProcessor::DoClone()
{
    return new SimpleRankProcessor(*this);
}

void SimpleRankProcessor::DoDestroy()
{
    delete this;
}

bool SimpleRankProcessor::RegisterMethods()
{
    return true;
}

bool SimpleRankProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);

