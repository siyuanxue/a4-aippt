#include <agg_web2/biz/academic_recall/academic_recall_processor.h>
#include <agg_web2/helper/agg_web_context_helper.h>

//#include <agg_web2/common/metrics_helper.h>
#include <agg_web2/upstream/common/common_request.h>
#include <agg_web2/upstream/common/common_result.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/util/string_tool.h>
#include <jsoncpp/json/json.h>
#include <agg_web2/upstream/qt/qt_request.h>
#include <agg_web2/upstream/qt/qt_result.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg2/helper/hit_helper.h>
#include <A4/util/string_util.h>

#include <agg2/common/qrs/qrs_clause.h>
#include <agg2/common/qrs/qrs_request.h>
#include <agg2/common/qrs/qrs_response.h>

AGG_BEGIN_NAMESPACE(academic_recall);
AGG_LOG_SETUP(agg_web2, AcademicRecallProcessor);

const std::string METHOD_GET_QUERY_EMB = "get_query_emb";
const std::string METHOD_GET_BODY = "get_body";
const std::string METHOD_GET_SUMMARY = "get_summary";

const std::string ACADEMIC_QUERY_EMB_NODE = "query_emb_node";
const std::string ACADEMIC_EVE_NODE = "eve_node";
const std::string ACADEMIC_HA3_FEATURE_NODE = "academic_ha3_feature";

const std::string ACADEMIC_EVE_NOTE_NODE = "eve_note_node";
const std::string ACADEMIC_HA3_NOTE_FEATURE_NODE = "academic_ha3_note_feature";

BEGIN_DEFINE_SERIALIZATION_FUNC(AcademicRecallProcessor)
END_DEFINE_SERIALIZATION_FUNC(AcademicRecallProcessor)

AcademicRecallProcessor::AcademicRecallProcessor()
{
}

AcademicRecallProcessor::~AcademicRecallProcessor()
{
}

bool AcademicRecallProcessor::DoInit(const String2String & params)
{
    PARSE_PROCESSOR_PARAM(params, ACADEMIC_QUERY_EMB_NODE, mQueryEmbNode);
    PARSE_PROCESSOR_PARAM(params, ACADEMIC_EVE_NODE, mBodyEmbNode);
    PARSE_PROCESSOR_PARAM(params, ACADEMIC_HA3_FEATURE_NODE, mBodyFeatureNode);

    PARSE_PROCESSOR_PARAM(params, ACADEMIC_EVE_NOTE_NODE, mNoteEmbNode);
    PARSE_PROCESSOR_PARAM(params, ACADEMIC_HA3_NOTE_FEATURE_NODE, mNoteFeatureNode);
    return true;
}

bool AcademicRecallProcessor::RegisterMethods()
{
    return RegisterMethod(METHOD_GET_QUERY_EMB, &AcademicRecallProcessor::GetQueryEmb)
           && RegisterMethod(METHOD_GET_BODY, &AcademicRecallProcessor::GetBody)
           && RegisterMethod(METHOD_GET_SUMMARY, &AcademicRecallProcessor::GetSummary);
}

int AcademicRecallProcessor::GetQueryEmb(AggContext * aggContext)
{
    std::string query = AggWebContextHelper::GetRawQuery(mAggContext);
    // RequestQueryEmbeddding(query);
    RequestQueryEmbedddingL3(query);
    return PR_SUCCESS;
}

void AcademicRecallProcessor::RequestQueryEmbeddding(const std::string &query)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    std::string use_vector;
    mAggContext->GetRequestParam("use_vector", use_vector);
    vector<double> embVec;
    if (use_vector == "0") {
        mDebugInfo += "not use vector.";
        return ;
    }

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, mQueryEmbNode);
    Json::Value root;
    root["text"] = query; 
    Json::FastWriter writer;
    string requestStr = writer.write(root);
    HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
    mQueryEmbResponse = wrapper->AsyncRequest(httpRequest);
}

void AcademicRecallProcessor::GetQueryEmbeddding(vector<double> &queryEmb)
{
    ResultPtr result(new CommonResult("llm_emb"));
    if (mQueryEmbResponse != nullptr && mQueryEmbResponse->GetResult(result)) {
        CommonResultPtr aResult = std::dynamic_pointer_cast<CommonResult>(result);
        queryEmb = aResult->mScores;
    }else{
        AGG_LOG(ERROR, "get llm_emb failed.");
    }
 }

void AcademicRecallProcessor::RequestQueryEmbedddingL3(const std::string &query)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    std::string use_vector;
    mAggContext->GetRequestParam("use_vector", use_vector);
    vector<double> embVec;
    if (use_vector == "0") {
        mDebugInfo += "not use vector.";
        return ;
    }

    string prompt_str = "query:" + query + "<|im_end|>";
    // AGG_LOG(ERROR, "llama3 pr prompt_str = %s", prompt_str.c_str());

    Json::Value root;
    root["request_id"] = "test-123";
    Json::Value prompts = Json::Value(Json::arrayValue);
    prompts.append(prompt_str);
    root["prompts"] = prompts; 
    root["max_new_tokens"] = 2048;
    Json::Value stop_texts = Json::Value(Json::arrayValue);
    stop_texts.append("<|end_of_text|>");
    stop_texts.append("<|im_end|>");
    stop_texts.append("<|endoftext|>");
    root["stop"] = stop_texts;
    root["reply_mode"] = "sync";
    root["do_sample"] = false;

    Json::FastWriter writer;
    std::string requestStr = writer.write(root);
    
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "llm_emb_model_l3");
    HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
    mQueryEmbResponse = wrapper->AsyncRequest(httpRequest);
}

void AcademicRecallProcessor::GetQueryEmbedddingL3(vector<double> &queryEmb)
{
    ResultPtr result(new CommonResult("llm_emb_l3"));
    if (mQueryEmbResponse != nullptr && mQueryEmbResponse->GetResult(result)) {
        CommonResultPtr aResult = std::dynamic_pointer_cast<CommonResult>(result);
        queryEmb = aResult->mScores;
    } else{
        AGG_LOG(ERROR, "get llm_emb_l3 failed.");
    }
    MESSER_UTILS_NS(util)::ostringstream ss;
    for (auto a: queryEmb) {
        ss << a << "`1";
    }
    // AGG_LOG(INFO,"query emb: %s", ss.str().c_str());
}

int AcademicRecallProcessor::GetBody(AggContext * aggContext)
{
    std::string vecDocNumStr;
    mAggContext->GetRequestParam("vector_doc_num", vecDocNumStr);
    uint32_t vecDocNum = INVALID_INT_VALUE;
    vecDocNum = StringUtil::StringToUint32(vecDocNumStr);
    if (vecDocNum == INVALID_INT_VALUE || vecDocNumStr.empty()) {
        vecDocNum = 100;
    }
    AGG_LOG(ERROR, "get vector_doc_num param [%d]", vecDocNum);
    mHitDocNum = vecDocNum;

    std::string vecNoteNumStr;
    mAggContext->GetRequestParam("vector_note_num", vecNoteNumStr);
    uint32_t vecNoteNum = INVALID_INT_VALUE;
    vecNoteNum = StringUtil::StringToUint32(vecNoteNumStr);
    if (vecNoteNum == INVALID_INT_VALUE || vecNoteNumStr.empty()) {
        vecNoteNum = 0;
    }
    AGG_LOG(ERROR, "get vector_note_num param [%d]", vecNoteNum);
    mHitNoteNum = vecNoteNum;

    vector<double> queryEmb;
    GetQueryEmbedddingL3(queryEmb);
    if(queryEmb.size() != 0){
        GetMainTextRecall(queryEmb, vecDocNum);
        // GetNoteTextRecall(queryEmb, vecNoteNum);
    }else{
        AGG_LOG(ERROR, "get query emb failed.");
    }

    return PR_SUCCESS;
}

void AcademicRecallProcessor::GetMainTextRecall(vector<double> &queryEmb, int32_t topn)
{
    std::shared_ptr<vector<DocumentNode> > eveResultPtr = 
                    mAggContext->GetValue<vector<DocumentNode>>("eve_docs");
    if (!eveResultPtr) {
        eveResultPtr.reset(new std::vector<DocumentNode>());
    }

    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    if (queryEmb.size() == 0)
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, mBodyEmbNode);
    Json::Value root;
    root["topk"] = topn;
    
    Json::Value vectors = Json::Value(Json::arrayValue);
    Json::Value vector  = Json::Value(Json::arrayValue);
    for (auto a: queryEmb) {
        vector.append(a);
    }
    vectors.append(vector);
    root["vectors"] = vectors;

    Json::Value attributes = Json::Value(Json::arrayValue);
    attributes.append("chunk_id");
    attributes.append("ID");
    attributes.append("ContentId");
    root["attributes"] = attributes;

    Json::Value config = Json::Value();
    config["probe_num"] = 1000;
    config["timeout"] = 10000;
    config["max_scan_num"] = 500000;
    root["config"] = config;

    Json::FastWriter writer;
    string requestStr = writer.write(root);
    // AGG_LOG(ERROR, "vector recall requestStr:%s", requestStr.c_str());
    HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
    HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
    ResultPtr result(new CommonResult("llm_recall"));
    if (response && response->GetResult(result)) {
        CommonResultPtr aResult = std::dynamic_pointer_cast<CommonResult>(result);
        Json::Reader reader;
        Json::Value root;
        // AGG_LOG(ERROR, "get vector recall [%s]", (aResult->mBody).c_str());
        if (reader.parse(aResult->mBody, root) && root.isObject() && root.isMember("data")
                && root["data"].isArray() && root["data"].size() > 0 && root["data"][0].isArray()) {
            for (auto one: root["data"][0]) {
                DocumentNode node;
                node.mRecall = "vector";
                Json::Value fields = one["fields"];
                if (fields["chunk_id"].isString()) {
                    node.mChunkId = fields["chunk_id"].asString();
                }
                if (fields["ID"].isString()) {
                    node.mID = fields["ID"].asString();
                }
                if (fields["ContentId"].isString()) {
                    node.mContentId = fields["ContentId"].asString();
                }
                if (one["pk"].isString()) {
                    node.mPK = one["pk"].asString();
                }
                if (one["score"].isDouble()) {
                    node.mVectorScore = one["score"].asDouble();
                }
                node.mRecall = "vector";
                eveResultPtr->emplace_back(node);
            }
        }
    }else{
        AGG_LOG(ERROR, "get vector recall failed.");
    }
    mAggContext->SetValue("eve_docs", eveResultPtr);
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    mDebugInfo += ", VectorRecall Cost:" + autil::StringUtil::toString(end_time - start_time); 
    AGG_LOG(INFO, "MainText VectorRecall Cost: %ld", end_time - start_time);
    return ;
}


void AcademicRecallProcessor::GetNoteTextRecall(vector<double> &queryEmb, int32_t topn)
{
    std::shared_ptr<vector<DocumentNode> > eveResultPtr = 
                    mAggContext->GetValue<vector<DocumentNode>>("eve_docs");
    if (!eveResultPtr) {
        eveResultPtr.reset(new std::vector<DocumentNode>());
    }else{
        int nMainTextNum = eveResultPtr->size();
        AGG_LOG(INFO, "already get main text eve result, nMainTextNum = %d", nMainTextNum);
    }

    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    if (queryEmb.size() == 0)
        return;

    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, mNoteEmbNode);
    Json::Value root;
    root["topk"] = topn;
    
    Json::Value vectors = Json::Value(Json::arrayValue);
    Json::Value vector  = Json::Value(Json::arrayValue);
    for (auto a: queryEmb) {
        vector.append(a);
    }
    vectors.append(vector);
    root["vectors"] = vectors;

    Json::Value attributes = Json::Value(Json::arrayValue);
    //attributes.append("chunk_id");
    attributes.append("ID");
    attributes.append("ContentId");
    root["attributes"] = attributes;

    Json::Value config = Json::Value();
    config["probe_num"] = 1000;
    config["timeout"] = 10000;
    config["max_scan_num"] = 500000;
    root["config"] = config;

    Json::FastWriter writer;
    string requestStr = writer.write(root);
    HttpRequestPtr httpRequest(new CommonRequest(requestStr, "common_post"));
    HttpResponsePtr response = wrapper->AsyncRequest(httpRequest);
    ResultPtr result(new CommonResult("llm_recall"));
    if (response && response->GetResult(result)) {
        CommonResultPtr aResult = std::dynamic_pointer_cast<CommonResult>(result);
        Json::Reader reader;
        Json::Value root;
        // AGG_LOG(ERROR, "get vector recall [%s]", (aResult->mBody).c_str());
        if (reader.parse(aResult->mBody, root) && root.isObject() && root.isMember("data")
                && root["data"].isArray() && root["data"].size() > 0 && root["data"][0].isArray()) {
            for (auto one: root["data"][0]) {
                DocumentNode node;
                node.mRecall = "vector";
                Json::Value fields = one["fields"];
                /*if (fields["chunk_id"].isString()) {
                    node.mChunkId = fields["chunk_id"].asString();
                }*/
                if (fields["ID"].isString()) {
                    node.mID = fields["ID"].asString();
                }
                if (fields["ContentId"].isString()) {
                    node.mContentId = fields["ContentId"].asString();
                }
                if (one["pk"].isString()) {
                    node.mPK = one["pk"].asString();
                }
                if (one["score"].isDouble()) {
                    node.mVectorScore = one["score"].asDouble();
                }
                node.mRecall = "vector_note";
                eveResultPtr->emplace_back(node);
            }
        }
    }else{
        AGG_LOG(ERROR, "get vector recall failed.");
    }
    mAggContext->SetValue("eve_docs", eveResultPtr);
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    mDebugInfo += ", VectorRecall Cost:" + autil::StringUtil::toString(end_time - start_time); 
    AGG_LOG(INFO, "Note VectorRecall Cost: %ld", end_time - start_time);
    return ;
}

int AcademicRecallProcessor::GetSummary(AggContext * aggContext)
{
    std::string pkSearchQuery;
    std::string responseKey_str;

    std::shared_ptr<vector<DocumentNode> > eveResultVecPtr =
                     mAggContext->GetValue<vector<DocumentNode> >("eve_docs");

    if (!eveResultVecPtr) {    
        AGG_LOG(ERROR, "eveResultVecPtr Is NULL!");
        return PR_SUCCESS;
    } 

    //========================get maintext feature ====================
    // 获取maintext feature
    for (auto it = eveResultVecPtr->begin(); it != eveResultVecPtr->end(); ++it) {
        DocumentNode &node = *it;
        if(node.mRecall == "vector_note"){
            continue;
        }

        if (it != eveResultVecPtr->begin()) pkSearchQuery += " OR ";
        pkSearchQuery += "pk:'" + node.mPK + "'";
    }
    AGG_LOG(DEBUG, "[GetSummary MainText]\t%s", pkSearchQuery.c_str());

    std::unordered_map<std::string, FeatureNode> pk_map;
    responseKey_str = "academic_maintext_feature_response";
    SendPkQrsRequest(aggContext,responseKey_str,pkSearchQuery,mHitDocNum,mBodyFeatureNode);
    GetMainTextPkQrsResult(aggContext,responseKey_str,pk_map);

    // 整合feature字段
    for (auto it = eveResultVecPtr->begin(); it != eveResultVecPtr->end(); ++it) {
        DocumentNode &node = *it;
        if(node.mRecall == "vector_note"){
            continue;
        }
        if (pk_map.find(node.mPK) != pk_map.end()) {
            FeatureNode &featureInfo = pk_map[node.mPK];

            //copy to node
            node.mTitle = featureInfo.mTitle;
            node.mChunk = featureInfo.mChunk;
            node.mEmbSource = featureInfo.mEmbSource;
            node.mRefPageNum = featureInfo.mRefPageNum;
            node.mLayout = featureInfo.mLayout;
        }else{
            AGG_LOG(ERROR, "pk_map not find the key = %s", node.mPK.c_str());
        }
    }


    //========================get note feature ====================
    /*// 获取note feature
    pkSearchQuery = "";
    for (auto it = eveResultVecPtr->begin(); it != eveResultVecPtr->end(); ++it) {
        DocumentNode &node = *it;
        if(node.mRecall == "vector"){
            continue;
        }

        if (it != eveResultVecPtr->begin()) pkSearchQuery += " OR ";
        pkSearchQuery += "pk:'" + node.mID + "'";
    }
    // 去除第一个OR
    if (pkSearchQuery.size() > 4) {
        pkSearchQuery = pkSearchQuery.substr(4);
    }
    AGG_LOG(DEBUG, "[GetSummary NoteText]\t%s", pkSearchQuery.c_str());

    std::unordered_map<std::string, DocumentNode> pk_map_note;
    responseKey_str = "academic_notetext_feature_response";
    SendPkQrsRequest(aggContext,responseKey_str,pkSearchQuery,mHitNoteNum,mNoteFeatureNode);
    GetNoteTextPkQrsResult(aggContext,responseKey_str,pk_map_note);

    // 整合feature字段
    for (auto it = eveResultVecPtr->begin(); it != eveResultVecPtr->end(); ++it) {
        DocumentNode &node = *it;
        if(node.mRecall == "vector"){
            continue;
        }
        if (pk_map_note.find(node.mID) != pk_map_note.end()) {
            DocumentNode &featureInfo = pk_map_note[node.mID];

            //copy to node
            node.mNote = featureInfo.mNote;
            node.mSummary = featureInfo.mSummary;
            node.mChunk = featureInfo.mSummary;
            node.mAcadamicType = featureInfo.mAcadamicType;
            node.mFileType = featureInfo.mFileType;
            node.mSubSourceType = featureInfo.mSubSourceType;
            node.mSourceType = featureInfo.mSourceType;
            node.mRawDocUrl = featureInfo.mRawDocUrl;
            node.mCopyright = featureInfo.mCopyright;
            node.mDocID = featureInfo.mDocID;
            node.mFileSize = featureInfo.mFileSize;
            node.mContentId = featureInfo.mContentId;
            node.mID = featureInfo.mID;
            node.mTitle = featureInfo.mTitle;
            node.mPKUC = featureInfo.mPKUC;
            node.mCSSCI = featureInfo.mCSSCI;
            node.mCSCD = featureInfo.mCSCD;
        }else{
            AGG_LOG(ERROR, "note pk_map not find the key = %s", node.mID.c_str());
        }
    }*/
    
    return PR_SUCCESS;
}

bool AcademicRecallProcessor::SendPkQrsRequest(AggContext * aggContext, 
                                               const std::string& responseKey_str,
                                               const std::string &pk_str,
                                               int hitNum,
                                               std::string nodeName)
{
    // add pk clause in qrsClause
    std::string query = AggWebContextHelper::GetRawQuery(mAggContext);
    QrsClause* qrsClause = new QrsClause;
    qrsClause->SetClusterName("vip");
    qrsClause->SetConfig("format","protobuf");
    qrsClause->SetConfig("result_compress","snappy");
    qrsClause->SetConfig("hit", std::to_string(hitNum));
    qrsClause->SetConfig("timeout", "5000");
    qrsClause->SetQuery(pk_str);

    AGG_LOG(DEBUG, "[PDF_PK_DOC] send pk request\t%s\t%s", 
            query.c_str(), pk_str.c_str());
    std::string qrsEncodeQuery;
    qrsClause->Encode(qrsEncodeQuery);
    AGG_LOG(INFO, "[PDF_PK_DOC]\t%s", qrsEncodeQuery.c_str());

    // 存入上下文,用于释放
    MESSER_UTILS_NS(util)::ostringstream qrsClauseKeySS;
    qrsClauseKeySS << responseKey_str << "_qrs_clause";
    aggContext->SetValue<QrsClause>(qrsClauseKeySS.str(), qrsClause);

    const std::string & requestQuery = mAggContext->GetAggRequest()->_query;
    QrsRequestPtr qrsRequest(new QrsRequest(requestQuery, qrsClause));

    AggHttpWrapper qrsWrapper(mAggContext, nodeName);
    HttpResponsePtr httpResponse = qrsWrapper.AsyncRequest(qrsRequest);

    {
        std::string bytes;
        qrsClause->Encode(bytes);
        AGG_LOG(DEBUG, "[PDF_PK_DOC]subContext:[%s], qrsClause:[%s], indexName:[%s]",
                aggContext->GetName().c_str(),
                bytes.c_str(), nodeName.c_str());
    }

    if (!httpResponse) {
        AGG_LOG(ERROR, "Send Post Request For %s Failed", nodeName.c_str());
        return false;
    }
    std::string indexName = nodeName;

    if (mAggContext->GetSubContextCount() == 0) {
        AGG_LOG(ERROR,"all subconetxt is null");
        return false;
    }
    AggSubContext * aggSubContext = mAggContext->GetSubContext(0);

    QrsResponsePtr qrsResponse(new QrsResponse(aggSubContext, indexName, qrsRequest, httpResponse));
    aggContext->SetValue(responseKey_str, qrsResponse);

    return true;
}

bool AcademicRecallProcessor::GetMainTextPkQrsResult(AggContext * aggContext, 
                                            const std::string& responseKey_str,
                                            std::unordered_map<std::string, FeatureNode> &pk_map)
{
    QrsResponsePtr qrsResponse = aggContext->GetValue<QrsResponse>(responseKey_str);
    if (!qrsResponse) {
        AGG_LOG(ERROR, "qrsResponse is NULL");
        return false;
    }

    QrsResultPtr qrsPKResult = qrsResponse->GetResult();;
    if (!qrsPKResult) {
        AGG_LOG(ERROR, "Get qrsPKResult Failed");
        return false;
    }

    std::string query = AggWebContextHelper::GetRawQuery(mAggContext);
    for (size_t i = 0; i < qrsPKResult->GetHitCount(); ++i) {
        QrsHit* qrsHit = qrsPKResult->GetHit(i);
        if(qrsHit == NULL){
            AGG_LOG(ERROR, "qrsHit is NULL");
            continue;
        }
        std::string PK = HitHelper::GetContentVar<std::string>(qrsHit, "PK","");
        std::string title = HitHelper::GetContentVar<std::string>(qrsHit, "title","");
        std::string chunk = HitHelper::GetContentVar<std::string>(qrsHit, "chunk","");
        std::string embSource = HitHelper::GetContentVar<std::string>(qrsHit, "embSource","");
        std::string pageInfo = HitHelper::GetContentVar<std::string>(qrsHit, "PageInfo","");
        int ref_page_num = 0;
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(pageInfo, root)) {
            if (root.isArray() && root.size() > 0) {
                if (root[0].isMember("page")) {
                    ref_page_num = root[0]["page"].asInt();
                    // AGG_LOG(ERROR, "get page [%d] from pageinfo [%s]", ref_page_num, pageInfo.c_str());
                }
                else if (root[0].isMember("page_idx")) {
                    ref_page_num = root[0]["page_idx"].asInt();
                    // AGG_LOG(ERROR, "get page_idx [%d] from pageinfo [%s]", ref_page_num, pageInfo.c_str());
                }
            }
        }
        else {
            AGG_LOG(ERROR, "parse pageInfo fail");
        }

        FeatureNode featureNode;
        featureNode.mPK = PK;
        featureNode.mTitle = title;
        featureNode.mChunk = chunk;
        featureNode.mEmbSource = embSource;
        featureNode.mRefPageNum = ref_page_num;
        featureNode.mLayout = pageInfo;
        pk_map[PK] = featureNode;
        //AGG_LOG(INFO, "[PDF_PK_DOC]\t%s\t%s", query.c_str(), PK.c_str());
    }
    AGG_LOG(INFO,"GetMainTextPkQrsResult size:%d", pk_map.size());

    return true;
}

bool AcademicRecallProcessor::GetNoteTextPkQrsResult(AggContext * aggContext, 
                                            const std::string& responseKey_str,
                                            std::unordered_map<std::string, DocumentNode> &pk_map)
{
    QrsResponsePtr qrsResponse = aggContext->GetValue<QrsResponse>(responseKey_str);
    if (!qrsResponse) {
        AGG_LOG(ERROR, "qrsResponse is NULL");
        return false;
    }

    QrsResultPtr qrsPKResult = qrsResponse->GetResult();;
    if (!qrsPKResult) {
        AGG_LOG(ERROR, "Get qrsPKResult Failed");
        return false;
    }

    std::string query = AggWebContextHelper::GetRawQuery(mAggContext);
    for (size_t i = 0; i < qrsPKResult->GetHitCount(); ++i) {
        QrsHit* qrsHit = qrsPKResult->GetHit(i);
        if(qrsHit == NULL){
            AGG_LOG(ERROR, "qrsHit is NULL");
            continue;
        }

        DocumentNode featureNode;
        featureNode.mID = HitHelper::GetContentVar<std::string>(qrsHit, "ID","");
        featureNode.mNote = HitHelper::GetContentVar<std::string>(qrsHit, "raw_data","");
        featureNode.mSummary = HitHelper::GetContentVar<std::string>(qrsHit, "Abstract","");
        featureNode.mAcadamicType = HitHelper::GetContentVar<std::string>(qrsHit, "AcadamicType","");
        featureNode.mFileType = HitHelper::GetContentVar<std::string>(qrsHit, "FileType","");
        featureNode.mSubSourceType = HitHelper::GetContentVar<std::string>(qrsHit, "SubSource","");
        featureNode.mSourceType = HitHelper::GetContentVar<std::string>(qrsHit, "Source","");
        featureNode.mRawDocUrl = HitHelper::GetContentVar<std::string>(qrsHit, "RawDocUrl","");
        featureNode.mCopyright = HitHelper::GetContentVar<std::string>(qrsHit, "CopyrightAuthorized","");
        featureNode.mDocID  = HitHelper::GetContentVar<std::string>(qrsHit, "DocId","");
        featureNode.mTitle  = HitHelper::GetContentVar<std::string>(qrsHit, "title","");
        std::string fileSize = HitHelper::GetContentVar<std::string>(qrsHit, "FileSize","");
        uint32_t fs = INVALID_INT_VALUE;
        fs = StringUtil::StringToUint32(fileSize);
        if (fs != INVALID_INT_VALUE) {
            featureNode.mFileSize= fs;
        }
        std::string contentId = HitHelper::GetContentVar<std::string>(qrsHit, "ContentId","");
        if (contentId != "null" && contentId.size()>0) {
            featureNode.mContentId = contentId;
        }
        featureNode.mPKUC  = HitHelper::GetContentVar<std::string>(qrsHit, "PekingUniCore2020","");
        featureNode.mCSSCI  = HitHelper::GetContentVar<std::string>(qrsHit, "CSSCI2021-2022","");
        featureNode.mCSCD  = HitHelper::GetContentVar<std::string>(qrsHit, "CSCD2023-2024","");

        pk_map[featureNode.mID] = featureNode;
        //AGG_LOG(INFO, "[PDF_PK_DOC]\t%s\t%s", query.c_str(), PK.c_str());
    }
    AGG_LOG(INFO,"GetNoteTextPkQrsResult size:%d", pk_map.size());

    return true;
}

void AcademicRecallProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

BaseProcessor* AcademicRecallProcessor::DoClone()
{
    return new AcademicRecallProcessor(*this);
}

void AcademicRecallProcessor::DoDestroy()
{
    delete this;
}

bool AcademicRecallProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(academic_recall);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(academic_recall)::AcademicRecallProcessor);

