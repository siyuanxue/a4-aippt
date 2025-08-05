#include <agg_web2/biz/debug/xueshu_rank_processor.h>
#include "agg_web2/common/common.h"
#include <agg_web2/upstream/common/common_request.h>
#include <agg_web2/upstream/common/common_result.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <jsoncpp/json/json.h>
#include <agg_web2/data/data_type.h>
#include <agg_web2/util/QtcExtractor.h>
#include <agg_web2/upstream/qc/qc_feature_result.h>
#include <A4/util/string_util.h>
#include <agg_web2/upstream/qt/qt_request.h>
#include <agg_web2/upstream/qt/qt_result.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg2/helper/hit_helper.h>
#include <agg_web2/util/string_deduper.h>
#include <agg_web2/util/AggWebStringTool.h>
/*#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <jsoncpp/json/json.h>
#include <agg_web2/data/value_data.h>
#include <agg_web2/upstream/qt/qt_request.h>
#include <agg_web2/upstream/qt/qt_result.h>
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
*/

#include <agg2/helper/time_helper.h>

const std::string DEGRADE_EVE_RECALL_QPS = "degrade_eve_recall_qps";

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, XueshuRankProcessor);

BEGIN_DEFINE_SERIALIZATION_FUNC(XueshuRankProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(XueshuRankProcessor)
const std::string QTC_NODE_PARAM_KEY = "qtc_bert_node";

XueshuRankProcessor::XueshuRankProcessor()
{
}

XueshuRankProcessor::~XueshuRankProcessor()
{
}

bool XueshuRankProcessor::DoInit(const String2String & params)
{
    PARSE_PROCESSOR_PARAM(params, QTC_NODE_PARAM_KEY, mQTCNode);
    return true;
}

int XueshuRankProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    MetricItem* metricItem = mAggContext->GetContext()->MutableMetricItem();
    
    std::string query;
    mAggContext->GetRequestParam("query", query);

    std::string hit;
    mAggContext->GetRequestParam("hit", hit);
    AGG_LOG(ERROR, "get hit param [%s]", hit.c_str());

    mCutoffNum = StringUtil::StringToUint32(hit);
    if (mCutoffNum == INVALID_INT_VALUE || hit.empty()) {
        mCutoffNum = 1000;
    }
    AGG_LOG(ERROR, "get cutoffNum param [%d]", mCutoffNum);

    vector<DocumentNode> rankVec;
    
    std::shared_ptr<vector<DocumentNode> > eveResultVecPtr =
                     mAggContext->GetValue<vector<DocumentNode> >("eve_docs");
    if (eveResultVecPtr) {    
        for(size_t i = 0 ; i < eveResultVecPtr->size(); i++){
            DocumentNode node = eveResultVecPtr->at(i);
            rankVec.push_back(node);
        }
        AGG_LOG(INFO, "rankVec size [%d]", rankVec.size());
    } else {
        // eve recall failed,add umon metric
        metricItem->IncreaseQps(DEGRADE_EVE_RECALL_QPS);
        AGG_LOG(ERROR, "eveResultVecPtr Is NULL!");
    }

    // step 4, get ha3 recall
    GetHa3Result(rankVec); // 第二路召回：ha3引擎

    GetNoteFeature(rankVec, 50);

    // step 5, get llm score
    // GetLLMScore(query, rankVec, 128); // minicpm-2B
    GetPrScore(query, rankVec, 200); // llama3-1.5B

    // step 6, get qtc score
    GetQtcScore(query, rankVec, 16);

    // step 7, sort and merge chunk
    SortAndMerge(rankVec);

    // step 8, todo：获取每篇论文的题录信息，用于前端展示
    
    // step 9, merge doc
    vector<DocNode> docVec; 
    MergeChunks(rankVec, docVec);

    // step 10, get doc qt
    GetDocScores(query, docVec);

    DeDuplicate(docVec);
    
    // final, output
    OutputResult(query, docVec, mCutoffNum);

    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(INFO, "Rank Cost:%d", end_time - start_time);
    return PR_SUCCESS;
}

void XueshuRankProcessor::GetNoteFeature(vector<DocumentNode>& rankVec, uint32_t batch_size)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    if (rankVec.size() == 0)
        return;

    vector<uint32_t> idxVec;
    vector<HttpResponsePtr> responsePtrVec;
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "academic_note_index");
    
    uint32_t multi_count = (uint32_t)rankVec.size() / batch_size + 1;
    for (size_t mi = 0; mi < multi_count; ++mi) { 
        uint32_t start_idx = mi * batch_size;
        
        for (uint32_t i = start_idx; i < start_idx + batch_size && i < rankVec.size(); ++i) {
            if (rankVec[i].mRecall.find("_note") != string::npos) { // 跳过单独召回的题录数据
                continue;
            }
            //for (uint32_t i = 0; i < rankVec.size(); ++i) {
            //string requestStr = "cluster=vip&&attribute=chunk,embSource&&config=format:xml,hit:60,timeout:5000,no_summary:no&&query=chk:";
            //requestStr += rankVec[i].mChunkId;
        
            string requestStr = "cluster=vip&&config=hit:10,timeout:1000&&query=pk:";
            requestStr += rankVec[i].mID;

            HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
            HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
            if (response) {
                responsePtrVec.push_back(response);
                idxVec.push_back(i);
                // AGG_LOG(ERROR, "get note succeed.[%s]", requestStr.c_str());
            } else {
                AGG_LOG(ERROR, "get note failed.[%s]", requestStr.c_str());
            }
        }
        
        for (size_t i = 0; i < responsePtrVec.size(); ++i) {
            DocumentNode &rankNode = rankVec[idxVec[i]];
            if (!responsePtrVec[i])
                continue;
            ResultPtr result(new CommonResult("ha3_feature"));
            if (responsePtrVec[i]->GetResult(result)) {
                CommonResultPtr aResult = std::dynamic_pointer_cast<CommonResult>(result);
                string body = aResult->mBody;
                string raw_data = getHa3Value(body, "raw_data");
                rankNode.mNote = raw_data;
                string abstract = getHa3Value(body, "Abstract");
                rankNode.mSummary = abstract;
                string acadamicType = getHa3Value(body, "AcadamicType");
                rankNode.mAcadamicType = acadamicType;
                string fileType = getHa3Value(body, "FileType");
                rankNode.mFileType = fileType;
                string sourceType = getHa3Value(body, "Source");
                rankNode.mSourceType = sourceType;
                string subSourceType = getHa3Value(body, "SubSource");
                rankNode.mSubSourceType = subSourceType;
                string rawDocUrl = getHa3Value(body, "RawDocUrl");
                rankNode.mRawDocUrl = rawDocUrl;
                string copyright = getHa3Value(body, "CopyrightAuthorized");
                rankNode.mCopyright = copyright;
                string docId = getHa3Value(body, "DocId");
                rankNode.mDocID = docId;
                string PKUC = getHa3Value(body, "PekingUniCore2020");
                rankNode.mPKUC = PKUC;
                string CSSCI = getHa3Value(body, "CSSCI2021-2022");
                rankNode.mCSSCI = CSSCI;
                string CSCD = getHa3Value(body, "CSCD2023-2024");
                rankNode.mCSCD = CSCD;
                
                string fileSize = getHa3Value(body, "FileSize");
                uint32_t fs = INVALID_INT_VALUE;
                fs = StringUtil::StringToUint32(fileSize);
                if (fs != INVALID_INT_VALUE) {
                    rankNode.mFileSize= fs;
                }

                string contentId = getHa3Value(body, "ContentId");
                if (rankNode.mContentId == "null" && contentId != "null" && contentId.size()>0) {
                    rankNode.mContentId = contentId;
                    // AGG_LOG(ERROR, "replace contentID.[%s]", contentId.c_str());
                }
            }
        }
    }
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    mDebugInfo += ", GetNoteFeature Cost:" + autil::StringUtil::toString(end_time - start_time);  
    AGG_LOG(ERROR, "GetNoteFeature Cost:%d", end_time - start_time);
}

std::string XueshuRankProcessor::getHa3Value(const string & body, const string &key)
{
    string s_key = "<"+key+"><![CDATA[";
    string e_key = "]]></"+key+">";
    size_t pos = body.find(s_key);
    if (pos != string::npos) {
        pos += s_key.size();
        size_t pos2 = body.find(e_key, pos);
        if (pos2 != string::npos) {
            return body.substr(pos, pos2 - pos);
        }
    }
    return "";
}

void XueshuRankProcessor::GetHa3Result(vector<DocumentNode> &rankVec)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    std::string use_ha3;
    mAggContext->GetRequestParam("use_ha3", use_ha3);

    if (use_ha3 == "0")
        return;

    std::string use_ha3_note;
    mAggContext->GetRequestParam("use_ha3_note", use_ha3_note);
    if (use_ha3_note.empty()) {
        use_ha3_note = "1";
    }

    std::vector<QrsHit*>qrsHitVec;
    GetQrsResult(mAggContext, qrsHitVec);
    for (auto& qrsHit : qrsHitVec) {
        if (qrsHit->GetQrsResult()) {
            string indexName = qrsHit->GetQrsResult()->GetIndexName();
            // AGG_LOG(ERROR, "get index name:%s", indexName.c_str());

            if (indexName.find("xueshu") != string::npos) {
                string title, chunk_id, body, contentID, ID, pageInfo;
                qrsHit->GetContent("articleTitle", title);
                qrsHit->GetContent("chunk_id", chunk_id);
                qrsHit->GetContent("body", body);
                qrsHit->GetContent("ID", ID);
                qrsHit->GetContent("ContentId", contentID);
                qrsHit->GetContent("PageInfo", pageInfo);

                DocumentNode rankNode;
                rankNode.mRecall = "ha3";
                rankNode.mTitle = title;
                rankNode.mChunkId = chunk_id;
                rankNode.mChunk = body;
                rankNode.mID = ID;
                rankNode.mContentId = contentID;
                rankNode.mLayout = pageInfo;
                Json::Reader reader;
                Json::Value root;
                if (reader.parse(pageInfo, root)) {
                    if (root.isArray() && root.size() > 0) {
                        if (root[0].isMember("page")) {
                            int ref_page_num = root[0]["page"].asInt();
                            rankNode.mRefPageNum = ref_page_num;
                            // AGG_LOG(ERROR, "get page [%d] from pageinfo [%s]", ref_page_num, pageInfo.c_str());
                        }
                        else if (root[0].isMember("page_idx")) {
                            int ref_page_num = root[0]["page_idx"].asInt();
                            rankNode.mRefPageNum = ref_page_num;
                            // AGG_LOG(ERROR, "get page_idx [%d] from pageinfo [%s]", ref_page_num, pageInfo.c_str());
                        }
                    }
                }
                else {
                    AGG_LOG(ERROR, "parse pageInfo fail");
                }

                rankVec.push_back(rankNode);
            }
            else if (use_ha3_note == "1" && indexName.find("note") != string::npos) {
                string raw_data, abstract, acadamicType, fileType, subSourceType, sourceType, rawDocUrl, fileSize, contentID, ID, title, copyright, docID, PKUC, CSSCI, CSCD;
                qrsHit->GetContent("raw_data", raw_data);
                qrsHit->GetContent("Abstract", abstract);
                qrsHit->GetContent("AcadamicType", acadamicType);
                qrsHit->GetContent("FileType", fileType);
                qrsHit->GetContent("SubSource", subSourceType);
                qrsHit->GetContent("Source", sourceType);
                qrsHit->GetContent("RawDocUrl", rawDocUrl);
                qrsHit->GetContent("FileSize", fileSize);
                qrsHit->GetContent("ContentId", contentID);
                qrsHit->GetContent("ID", ID);
                qrsHit->GetContent("title", title);
                qrsHit->GetContent("CopyrightAuthorized", copyright);
                if (copyright == "0" && rawDocUrl.empty()) { // 过滤没有url的外部题录数据
                    continue; 
                }
                qrsHit->GetContent("DocId", docID);
                qrsHit->GetContent("PekingUniCore2020", PKUC);
                qrsHit->GetContent("CSSCI2021-2022", CSSCI);
                qrsHit->GetContent("CSCD2023-2024", CSCD);

                // AGG_LOG(ERROR, "get note ha3 doc:%s", raw_data.c_str());

                DocumentNode rankNode;
                rankNode.mRecall = "ha3_note";
                rankNode.mNote = raw_data;
                rankNode.mSummary = abstract;
                rankNode.mChunk = abstract;
                rankNode.mAcadamicType = acadamicType;
                rankNode.mFileType = fileType;
                rankNode.mSubSourceType = subSourceType;
                rankNode.mSourceType = sourceType;
                rankNode.mRawDocUrl = rawDocUrl;
                uint32_t fs = INVALID_INT_VALUE;
                fs = StringUtil::StringToUint32(fileSize);
                if (fs != INVALID_INT_VALUE) {
                    rankNode.mFileSize= fs;
                }
                rankNode.mContentId = contentID;
                rankNode.mID = ID;
                rankNode.mTitle = title;
                rankNode.mCopyright = copyright;
                rankNode.mDocID = docID;
                rankNode.mPKUC = PKUC;
                rankNode.mCSSCI = CSSCI;
                rankNode.mCSCD = CSCD;

                rankVec.push_back(rankNode);
            }
        }
    }
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    mDebugInfo += ", GetHa3Result Cost:" + autil::StringUtil::toString(end_time - start_time);  
}

void XueshuRankProcessor::GetQrsResult(AggContext * aggContext, std::vector<QrsHit*>& qrsHitVec)
{
    size_t count = aggContext->GetSubContextCount();
    AGG_LOG(ERROR, "subContextCount:[%d]", count);
    if (count == 0) {
        AGG_LOG(ERROR, "subcontext count is 0");
        return;
    }
    
    for (size_t s = 0; s < count; s++) {
        AggSubContext* subContext = aggContext->GetSubContext(s);
        size_t resultCount = subContext->GetQrsResultCount();//查询引擎的次数
        
        if (resultCount == 0) {
            AGG_LOG(DEBUG, "subcontext has no qrs result");
            continue;
        }
        AGG_LOG(ERROR, "resultCount is:%d", resultCount);

        /* 收集最初数据 */
        for (size_t j = 0; j < resultCount; j++) {
            QrsResultPtr qrsResult = subContext->GetQrsResult(j);
            size_t hitCount = qrsResult->GetHitCount();
            AGG_LOG(ERROR, "hitCount=[%d]", hitCount);
            if (hitCount == 0) {
                AGG_LOG(INFO, "[%s] has no qrs hit", qrsResult->GetIndexName().c_str());
            }
            for (size_t k = 0; k < hitCount; k++) {
                QrsHit* qrsHit = qrsResult->GetHit(k);
                qrsHitVec.push_back(qrsHit);
            }
        }
    }
}

void XueshuRankProcessor::GetLLMScore(const string& query, vector<DocumentNode> &rankVec, uint32_t batch_size)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    if (rankVec.size() == 0)
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "llm_rank_model/process");
    //uint32_t batch_size = 50;
    uint32_t multi_count = (uint32_t)rankVec.size() / batch_size + 1;
    
    for (size_t mi = 0; mi < multi_count; ++mi) { 
        uint32_t start_idx = mi * batch_size;
        
        vector<uint32_t> idxVec;
        vector<HttpResponsePtr> responsePtrVec;
        for (uint32_t idx = start_idx; idx < start_idx + batch_size && idx < rankVec.size(); ++idx) {
            if (rankVec[idx].mEmbSource.size() < 10 * 3 && rankVec[idx].mChunk.size() < 10 * 3)
                continue;

            Json::Value root;
            root["cutoff_layers"] = "40";
            root["query"] = query; 
            if (rankVec[idx].mRecall.find("_note") != string::npos) {
                root["passage"] = rankVec[idx].mTitle + "\n" + rankVec[idx].mSummary;
            }
            else {
                if (rankVec[idx].mEmbSource.size() >= 10 * 3) {
                    root["passage"] = rankVec[idx].mTitle + "\n" + rankVec[idx].mEmbSource;
                } else {
                    root["passage"] = rankVec[idx].mTitle + "\n" + rankVec[idx].mChunk;
                }
            }

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
            ResultPtr result(new CommonResult("llm_rank"));
            if (!responsePtrVec[i]->GetResult(result)) {
                continue;
            }

            CommonResultPtr aResult = std::dynamic_pointer_cast<CommonResult>(result);
            Json::Reader reader;
            Json::Value root;
            if (reader.parse(aResult->mBody, root) && root.isObject() && root.isMember("data")
                    && root["data"].isObject() && root["data"].isMember("result")) {
                rankVec[idxVec[i]].mLLMScore = root["data"]["result"]["prob"].asDouble(); 
            }
        }
    }
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    mDebugInfo += ", GetLLMScore Cost:" + autil::StringUtil::toString(end_time - start_time);  
}

void XueshuRankProcessor::GetPrScore(const string& query, vector<DocumentNode> &rankVec, uint32_t batch_size)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    if (rankVec.size() == 0) {
        AGG_LOG(ERROR, "rankVec is empty");
        return;
    }

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "llm_pr_model");
    uint32_t multi_count = (uint32_t)rankVec.size() / batch_size + 1;
    
    for (size_t mi = 0; mi < multi_count; ++mi) { 
        uint32_t start_idx = mi * batch_size;
        
        vector<uint32_t> idxVec;
        vector<HttpResponsePtr> responsePtrVec;
        for (uint32_t idx = start_idx; idx < start_idx + batch_size && idx < rankVec.size(); ++idx) {
            if (rankVec[idx].mEmbSource.size() < 10 * 3 && rankVec[idx].mChunk.size() < 10 * 3 && rankVec[idx].mSummary.size() < 10 * 3)
                continue;

            string passage;
            string title = rankVec[idx].mTitle;
            string content;
            if (rankVec[idx].mRecall.find("_note") != string::npos) {
                passage = rankVec[idx].mTitle + "\n" + rankVec[idx].mSummary;
                content = rankVec[idx].mSummary;
            }
            else {
                if (rankVec[idx].mEmbSource.size() >= 10 * 3) {
                    passage = rankVec[idx].mTitle + "\n" + rankVec[idx].mEmbSource;
                    content = rankVec[idx].mEmbSource;
                } else {
                    passage = rankVec[idx].mTitle + "\n" + rankVec[idx].mChunk;
                    content = rankVec[idx].mChunk;
                }
            }

            string prompt_str= "你是一个检索相关的专家，请你对给出的query和其检索到的doc的相关性进行打分，在打分时需要考虑\n1.query的实体和doc的实体是完全一致部分一致还是不一致\n2.query的意图和doc的意图是完全一致部分一致还是不一致\n综合以上两个条件进行考虑，各个分数的具体说明如下\n0分：query实体和doc完全不相关（包括与query的实体是同层级的情况，比如iPhone15和iPhone16）\n1分:doc和query部分相关，且query实体和doc实体是上下级关系\n2分:doc的实体和意图与query均一致且能够回答问题的doc\n3分:doc中没有内容与query相关，但是实体或意图部分匹配\n4分:doc的实体和query完全匹配，但是doc的意图和query的意图只是部分相关。\nquery:"+query+"\ndoc:"+title + "\n" + content;

            // string prompt_str = "请判断提供的回复能否回答问题，输出0表示不能回答，1表示可以部分回答，2表示可以完全回答。\n问题：";
            // prompt_str += query;
            // prompt_str += "\n回复：" + passage;
            
            prompt_str = "<|im_start|>user\n" + prompt_str + "<|im_end|>\n<|im_start|>assistant\n";
            // AGG_LOG(ERROR, "llama3 pr prompt_str = %s", prompt_str.c_str());

            Json::Value root;
            root["request_id"] = "test-123";
            Json::Value prompts = Json::Value(Json::arrayValue);
            prompts.append(prompt_str);
            root["prompts"] = prompts; 
            root["max_new_tokens"] = 100;
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
                continue;
            }

            CommonResultPtr prResult = std::dynamic_pointer_cast<CommonResult>(result);
            Json::Reader reader;
            Json::Value root;
            rankVec[idxVec[i]].mLLMScore = prResult->mScore; 
            // AGG_LOG(ERROR, "llama3 pr score = %f", prResult->mScore);
        }
    }
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    mDebugInfo += ", GetLLMScore Cost:" + autil::StringUtil::toString(end_time - start_time);  
}

void XueshuRankProcessor::GetQtcScore(const string& query, vector<DocumentNode> &rankVec, uint32_t batch_size)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    if (rankVec.size() == 0)
        return;
    
    size_t batch = 16;
    vector<vector<size_t>> idxVecs;
    vector<size_t> idxVec;
    vector<vector<DocumentNode>> psgSampleVec;
    vector<HttpResponsePtr> qtcRespVec;
    vector<DocumentNode> cands;
    for (size_t idx =0; idx<rankVec.size(); ++idx) {
        string psg = "";
        if (rankVec[idx].mEmbSource.size() < 10 * 3 && rankVec[idx].mChunk.size() < 10 * 3  && rankVec[idx].mSummary.size() < 10 * 3)
            continue;
        // 
        // LLMPsgPtr LLMPsgPtr(new LLMPsg(0, 0, rankVec[idx].mTitle, psg));
        cands.emplace_back(rankVec[idx]);
        idxVec.emplace_back(idx);
        
        if (cands.size() >= batch) {
            BatchRequestQTC(query, cands, qtcRespVec);
            idxVecs.emplace_back(idxVec);
            cands.clear();
            idxVec.clear();
        }
    }
    if (! cands.empty()) {
		BatchRequestQTC(query, cands, qtcRespVec);
        idxVecs.emplace_back(idxVec);
		cands.clear();
        idxVec.clear();
	}

    // 异步获取qtc得分
	for (size_t i=0; i<qtcRespVec.size(); ++i) {
		vector<double> qtcScores = GetQTC(qtcRespVec[i]);
		vector<size_t>& psgSample = idxVecs[i];
		if (qtcScores.size() == psgSample.size()*2) {
			for (size_t j=0; j<psgSample.size(); ++j) {
                rankVec[psgSample[j]].mQtcScore = qtcScores[j];
                rankVec[psgSample[j]].mQtScore = qtcScores[j+psgSample.size()];
                // AGG_LOG(ERROR, "qtc score = %f", qtcScores[j]);
			}
		}
	}
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    mDebugInfo += ", GetQtcScore Cost:" + autil::StringUtil::toString(end_time - start_time); 
}

void XueshuRankProcessor::BatchRequestQTC(const std::string& query,
        std::vector<DocumentNode>& cands,
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
        std::string t = cands[i].mTitle;
        std::string c = "";
        if (cands[i].mRecall.find("_note") != string::npos) {
            c = cands[i].mSummary;
        }
        else {
            if (cands[i].mEmbSource.size() >= 10 * 3) {
                c = cands[i].mEmbSource;
            } else {
                c = cands[i].mChunk;
            }
        }

        std::string single_qtc_request_dat;
        QtcExtractor::extractQtcStdkv(wordIdInBertQC, query, t, c, single_qtc_request_dat);
        qtc_request_dat_vec.emplace_back(single_qtc_request_dat);
    }
    std::string qtc_request_dat = autil::StringUtil::toString(qtc_request_dat_vec, "\n");
    
    // batch请求qtc、pr
    qtcRespVec.emplace_back(RequestQTC(qtc_request_dat));
}

HttpResponsePtr XueshuRankProcessor::RequestQTC(std::string& qtc_request_dat)
{
    QcFeatureRequestPtr qcFeatureRequest = std::make_shared<QcFeatureRequest>(mAggContext);
    qcFeatureRequest->InitRequestStr(qtc_request_dat);
    AggHttpWrapper wrapper(mAggContext, mQTCNode);
    //AGG_LOG(DEBUG, "qtc mRequestStr=%s", qcFeatureRequest->GetRequestStr().c_str());
    return wrapper.AsyncRequest(qcFeatureRequest);
}

std::vector<double> XueshuRankProcessor::GetQTC(HttpResponsePtr httpResponse)
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
    std::vector<float> qt_scores = qcFeatureResult->GetQtFeature()[0];
    std::vector<double> qtc_scores_double(qtc_scores.begin(), qtc_scores.end());
    qtc_scores_double.insert(qtc_scores_double.end(), qt_scores.begin(), qt_scores.end());
    return qtc_scores_double;
}

//正文数据合并相同chunk、补充没有召回过正文的摘要数据
void XueshuRankProcessor::SortAndMerge(vector<DocumentNode> &rankVec)
{
    for (auto &rankNode: rankVec) {
        rankNode.mRankScore = rankNode.mLLMScore;
    }
    sort(rankVec.begin(), rankVec.end(), [](DocumentNode& x, DocumentNode& y) {
        return x.mRankScore > y.mRankScore;
    });
    
    // step 6.1 按照sort的顺序，进行chunk合并 
    if (rankVec.size() > 0) {
        map<string, DocumentNode> rankMap;
        vector<string> chunkVec;
        vector<string> docVec;
        for (auto &rankNode: rankVec) {
            if (rankNode.mRecall.find("note") != string::npos) { // 题录召回的数据需要跟正文召回的互斥
                continue;
            }
            string key = rankNode.mID + "\t" + rankNode.mChunkId;
            if (std::find(chunkVec.begin(), chunkVec.end(), key) != chunkVec.end()) {
                rankMap[key].mRecall += "," + rankNode.mRecall;
                if (rankMap[key].mEmbSource.empty()) {
                    rankMap[key].mEmbSource = rankNode.mEmbSource; 
                }
                if (rankMap[key].mPK.empty()) {
                    rankMap[key].mPK = rankNode.mPK; 
                }
                continue;
            }
            rankMap[key] = rankNode;
            chunkVec.push_back(key);
            docVec.push_back(rankNode.mID);
        }

        for (auto &rankNode: rankVec) {
            if (rankNode.mRecall.find("note") == string::npos) { // 题录召回的数据需要跟正文召回的互斥
                continue;
            }
            string key = rankNode.mID;
            if (rankMap.find(key) != rankMap.end()) {
                rankMap[key].mRecall += "," + rankNode.mRecall;
                if (rankMap[key].mEmbSource.empty()) {
                    rankMap[key].mEmbSource = rankNode.mEmbSource; 
                }
                if (rankMap[key].mPK.empty()) {
                    rankMap[key].mPK = rankNode.mPK; 
                }
                continue;
            }
            else if (std::find(docVec.begin(), docVec.end(), key) == docVec.end()) { //正文没有召回过的doc
                rankMap[key] = rankNode;
                docVec.push_back(key);
            }
        }

        rankVec.clear();
        for (auto &item: rankMap) {
            rankVec.push_back(item.second);
        }
    }
}
    
void XueshuRankProcessor::MergeChunks(vector<DocumentNode> &rankVec, vector<DocNode> &docVec)
{
    map<string, DocNode> docMap;
    vector<string> docList;
    for (auto &rankNode: rankVec) {
        string key = rankNode.mID;
        if (std::find(docList.begin(), docList.end(), key) != docList.end()) {
            docMap[key].mChunks.push_back(rankNode);
            continue;
        }
        DocNode docNode;
        docNode.mChunks.push_back(rankNode);
        docMap[key] = docNode; 
        docList.push_back(key);
    }
    
    for (auto key: docList) {
        docVec.push_back(docMap[key]);
    }
    
    for (auto &docNode: docVec) {
        sort(docNode.mChunks.begin(), docNode.mChunks.end(), [](DocumentNode& x, DocumentNode& y) {
            return x.mRankScore > y.mRankScore;
        });
        docNode.mBestChunkIdx = 0;
        docNode.mBestChunkLLMScore = docNode.mChunks[0].mLLMScore;
        docNode.mBestChunkQtcScore = docNode.mChunks[0].mQtcScore;
        docNode.mBestChunkQtScore  = docNode.mChunks[0].mQtScore;

        docNode.mTitle = docNode.mChunks[0].mTitle;
        docNode.mID = docNode.mChunks[0].mID;
        docNode.mRankScore = docNode.mBestChunkLLMScore;
    }

    sort(docVec.begin(), docVec.end(), [](DocNode& x, DocNode& y) {
            return x.mRankScore > y.mRankScore;
    });
}

std::vector<double> XueshuRankProcessor::GetQT(const std::string& query,
        const std::vector<std::string>& titles)
{
    HttpRequestPtr httpRequest(new QtRequest(query, titles));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "qt_model/ts");
    ResultPtr result(new QtResult);
    if (! wrapper->SyncRequest(httpRequest, result) || ! result) {
        AGG_LOG(ERROR, "Get qt failed");
        vector<double> empty;
        return empty;
    }
    QtResultPtr qtResult = std::dynamic_pointer_cast<QtResult>(result);
    return qtResult->mQtScores;
}
    
void XueshuRankProcessor::GetDocScores(const string& query, vector<DocNode> &docVec)
{
    map<uint32_t, uint32_t> indexMap;
    map<string, uint32_t> titleMap;

    size_t valid_doc_num = 0;
    for (auto &docNode: docVec) { // 缩放题录召回数据的分数
        if (docNode.mChunks.size()==0) {
            continue;
        }
        auto rankNode = docNode.mChunks[0];
        if (docNode.mBestChunkLLMScore > 1.2 && rankNode.mRecall.find("note")==string::npos) { // pr大于阈值的正文doc,llmscore不作调整
            valid_doc_num +=1;
            continue;
        }
        if (rankNode.mRecall.find("note")!=string::npos) { // 召回的题录数据，llmscore scale到阈值以下
            docNode.mRankScore = docNode.mBestChunkLLMScore / 2.0 * 1.2;
        }
    }

    for (auto &docNode: docVec) { // qtc调权
        if (docNode.mRankScore > 1.8) {
            docNode.mRankScore += (docNode.mBestChunkQtcScore + docNode.mBestChunkQtScore)/2.0/5; //0-0.4
        }
    }

    std::string authorityRank;
    mAggContext->GetRequestParam("authority_rank", authorityRank);
    AGG_LOG(ERROR, "get authority_rank param [%s]", authorityRank.c_str());
    if (authorityRank != "0") {
        for (auto &docNode: docVec) { // 权威特征调权
            if (docNode.mChunks.size()==0) {
                continue;
            }
            auto rankNode = docNode.mChunks[0];
            if (rankNode.mPKUC == "1") {
                docNode.mRankScore += 0.1;
            }
            if (rankNode.mCSSCI == "1") {
                docNode.mRankScore += 0.1;
            }
            if (rankNode.mCSCD == "1") {
                docNode.mRankScore += 0.1;
            }
        }
    }

    sort(docVec.begin(), docVec.end(), [](DocNode& x, DocNode& y) {
            return x.mRankScore > y.mRankScore;
    });
}

void XueshuRankProcessor::DeDuplicate(vector<DocNode> &docVec)
{
    if (docVec.size() <= 1) {
        AGG_LOG(INFO, "no need to dedup");
        return;
    }
    AGG_LOG(INFO, "items before dedup count [%d]", docVec.size());

    std::set<size_t> removeSet;
    set<string> dedupSet;
    for (size_t i=0; i < docVec.size(); ++i) {
        if (docVec[i].mChunks.size() == 0) {
            continue;
        }
        auto rankNode = docVec[i].mChunks[0];
        string summary = rankNode.mSummary;
        string title = rankNode.mTitle;
        
        string writer;
        Json::Reader reader;
        Json::Value note;
        if (reader.parse(rankNode.mNote, note) && note.isObject() && note.isMember("writer")) {
            writer = note["writer"].asString();
        }
        if (title.empty() || writer.empty()) {
            continue;
        }

        // string dedupStr = title+writer+summary;
        string dedupStr = title+writer;
        string synbols[] = {",", "?", "!", "，", "！", "？", "。", "\n", ";", "；", ":", "：", " "};
        for (auto synbol: synbols) {
            dedupStr = StringTool::ReplaceStr(dedupStr, synbol, "");
        }
        
        // dedupStr = AggWebStringTool::SubStrByUtf8Size(dedupStr, 50);
        if (dedupSet.find(dedupStr) != dedupSet.end()) {
            removeSet.insert(i);
            AGG_LOG(INFO, "duplicated [%s]", dedupStr.c_str());
            continue;
        }
        dedupSet.insert(dedupStr);
    }

    // output
    vector<DocNode> docVecFinal;
    for (size_t i=0; i < docVec.size(); ++i) {
        auto doc = docVec[i];
        if (removeSet.count(i)) {
            AGG_LOG(ERROR, "***dedup remove doc*** [%i]", i);
        } else {
            // AGG_LOG(ERROR, "***dedup output doc*** [%i]", i);
            docVecFinal.push_back(doc);
        }
    }
    docVec.clear();
    docVec = docVecFinal;
    AGG_LOG(INFO, "items after dedup count [%d]", docVecFinal.size());
}

void XueshuRankProcessor::OutputResult(const string& query, vector<DocNode> &docVec, uint32_t hit)
{ 
    string filterEmptyNote;
    mAggContext->GetRequestParam("filter_empty_note", filterEmptyNote);
    if (filterEmptyNote.empty()) {
        filterEmptyNote = "1";
    }

    Json::Value root;
    root["query"] = query;
    Json::Value results = Json::Value(Json::arrayValue);
    uint32_t count = 0;
    uint32_t note_count = 0;
    uint32_t max_note_num = hit*3/10;
    for (auto& docNode : docVec) {
        if (docNode.mChunks.size()==0) {
            continue;
        }

        auto rankNode = docNode.mChunks[0];
        if (filterEmptyNote == "1" && rankNode.mNote.empty()) { // 过滤note为空的数据
            continue;
        }
        if (rankNode.mRecall.find("note") != string::npos && note_count>=max_note_num) { // 限制外部题录数据数量
            continue;
        }
        
        results.append(Json::Value());
        Json::Value & doc = results[results.size() - 1];

        doc["id"] = docNode.mID;
        doc["type"] = "paper_doc";
        doc["doc_id"] = rankNode.mDocID;
        doc["content_id"] = rankNode.mContentId;;
        doc["doc_type"] = rankNode.mFileType;
        doc["academic_type"] = rankNode.mAcadamicType;
        doc["title"] = docNode.mTitle;
        doc["summary"] = rankNode.mSummary;
        doc["reference"] = rankNode.mChunk;
        doc["emb_source"] = rankNode.mEmbSource;
        Json::Reader reader;
        Json::Value note;
        if (reader.parse(rankNode.mNote, note)) {
            doc["note"] = note;
        }
        else {
            doc["note"] = rankNode.mNote;
        }
        doc["ref_page_num"] = rankNode.mRefPageNum;
        Json::Value layout;
        Json::Value layout_new = Json::Value(Json::arrayValue);
        if (reader.parse(rankNode.mLayout, layout)) {
            if (layout.isArray() && layout.size() > 0) {
                // AGG_LOG(INFO, "layout.size=%d", layout.size());
                for (auto para: layout) {
                    if (para.isMember("page_idx")) {
                        // AGG_LOG(INFO, "page_idx=%d", para["page_idx"].asInt());
                        para["page"] = para["page_idx"];
                    }
                    layout_new.append(para);
                }
            }
            
            doc["layout"] = layout_new;
        }
        else {
            doc["layout"] = rankNode.mLayout;
        }
        doc["layout_sentence"] = "";
        doc["raw_url"] = rankNode.mRawDocUrl;
        doc["sub_source_type"] = rankNode.mSubSourceType;
        doc["source_type"] = rankNode.mSourceType;
        doc["file_size"] = rankNode.mFileSize;
        doc["preview_img_url"] = "";
        doc["extra"] = "";
        if (rankNode.mCopyright == "0") {
            doc["is_wenku_doc"] = false;
        }
        else {
            doc["is_wenku_doc"] = true;
        }
        doc["qtc_score"] = rankNode.mQtcScore;
        doc["qt_score"] = rankNode.mQtScore;
        doc["llm_score"] = rankNode.mLLMScore;
        doc["recall"] = rankNode.mRecall;
        if (rankNode.mRecall.find("note") != string::npos) {
            note_count += 1;
        }
        
        // Json::Value chunks = Json::Value(Json::arrayValue);
        // for (auto& rankNode : docNode.mChunks) {
        //     chunks.append(Json::Value());
        //     Json::Value & item = chunks[chunks.size() - 1];
        //     //item["title"] = rankNode.mTitle;
        //     item["chunk_id"] = rankNode.mChunkId;
        //     item["normalized_url"] = rankNode.mNormalizedUrl;
        //     item["pk"] = rankNode.mPK;
        //     item["chunk"] = rankNode.mChunk;
        //     item["emb_source"] = rankNode.mEmbSource;
        //     item["vector_score"] = rankNode.mVectorScore;
        //     item["llm_score"] = rankNode.mLLMScore;
        //     item["qtc_score"] = rankNode.mQtcScore;
        //     item["rank_score"] = rankNode.mRankScore;
        //     item["recall"] = rankNode.mRecall;
        // }
        // doc["chunks"] = chunks;
        doc["operate_fields"] = "";
        doc["rank_score"] = docNode.mRankScore;
        doc["doc_trigger_level"] = 1;
        doc["pk"] = rankNode.mPK;
        doc["authority"] = "PKUC:" + rankNode.mPKUC + " CSSCI:" + rankNode.mCSSCI + " mCSCD:" + rankNode.mCSCD;

        if (++count >= hit)
            break;
    }
    AGG_LOG(ERROR, "note_count [%d]", note_count);

    root["rank_score"] = 0.0;
    if (docVec.size() > 0) {
        root["rank_score"] = docVec[0].mRankScore;
    }
    root["ra_doc_list"] = results;
    root["message"] = "succ";
    root["debug"] = mDebugInfo;

    Json::FastWriter writer;
    std::string resContent = writer.write(root);

    AGG_LOG(INFO, "resContent=%s", resContent.c_str());
    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

BaseProcessor * XueshuRankProcessor::DoClone()
{
    return new XueshuRankProcessor(*this);
}

void XueshuRankProcessor::DoDestroy()
{
    delete this;
}

bool XueshuRankProcessor::RegisterMethods()
{
    return true;
}

bool XueshuRankProcessor::RegisterActors()
{
    return true;
}

void XueshuRankProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister){
    monitorRegister->RegisterQpsMetric(DEGRADE_EVE_RECALL_QPS);
}
AGG_END_NAMESPACE(agg_web2);

