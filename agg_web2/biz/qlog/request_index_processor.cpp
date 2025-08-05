#include <agg_web2/biz/qlog/request_index_processor.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg2/util/string_tool.h>
#include <autil/StringUtil.h>
#include <autil/HashAlgorithm.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <messer_utils/json/JsonParser.h>
#include <agg_web2/common/define.h>
#include <A4/util/string_util.h>

#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/common/qrs/qrs_request.h>
#include <agg2/common/qrs/qrs_response.h>
using namespace std;
AGG_BEGIN_NAMESPACE(agg_web2)
AGG_LOG_SETUP(agg_web2, RequestIndexProcessor);
AGG_LOG_SETUP_WITH_NAME(displayLog, displayLog, RequestIndexProcessor);

BEGIN_DEFINE_SERIALIZATION_FUNC(RequestIndexProcessor)
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
END_DEFINE_SERIALIZATION_FUNC(RequestIndexProcessor)

bool RequestIndexProcessor::DoInit(const String2String &params)
{
    String2StringConstIter iter = params.find("common");
    QrsClauseParam commonQrsClauseParam;
    if (iter != params.end()) {
        if (!ParseConfig(iter->second, commonQrsClauseParam)) {
            AGG_LOG(ERROR, "parse common field failed");
            return false;
        }
    }

    for (iter = params.begin(); iter != params.end(); ++iter) {
        if (iter->first.find("index") == std::string::npos) {
            continue;
        }
        QrsClauseParam& qrsClauseParam = mQrsClauseParamMap[iter->first];
        qrsClauseParam = commonQrsClauseParam;
        if (!ParseConfig(iter->second, qrsClauseParam)) {
            AGG_LOG(ERROR, "parse index field failed");
            return false;
        }
    }
    return true;
}

bool RequestIndexProcessor::ParseConfig(const std::string& configStr,
                                     QrsClauseParam& qrsClauseParam)
{
    StringTokenizer tokenizer;
    size_t count = tokenizer.Tokenize(configStr, "#", 
            StringTokenizer::TOKEN_IGNORE_EMPTY | StringTokenizer::TOKEN_TRIM);
    for (size_t i = 0; i < count; i++) {
        const std::string& kvStr = tokenizer[i]; 
        StringTokenizer kvTokenizer;
        size_t kvCount = kvTokenizer.Tokenize(kvStr, "=", 
                StringTokenizer::TOKEN_IGNORE_EMPTY | StringTokenizer::TOKEN_TRIM);

        if (kvCount < 2) {
            return false;
        }
        if (kvTokenizer[0] == "kv") {
            if (!ParseConfigItem(kvTokenizer[1], qrsClauseParam._kvMap)) {
                return false;
            }
        } else if (kvTokenizer[0] == "config") {
            if (!ParseConfigItem(kvTokenizer[1], qrsClauseParam._configMap)) {
                return false;
            }
        } else {
            std::string value = kvTokenizer[1];
            if (kvCount > 2) {
                for (size_t idx = 2; idx < kvCount; ++idx) {
                    value += "=";
                    value += kvTokenizer[idx];
                }
            }
            qrsClauseParam._paramMap[kvTokenizer[0]] = value;
        }
    }
    return true;    
}

bool RequestIndexProcessor::ParseConfigItem(const std::string& str, String2String& kvMap)
{
    StringTokenizer tokenizer;
    size_t count = tokenizer.Tokenize(str, ",", 
            StringTokenizer::TOKEN_IGNORE_EMPTY | StringTokenizer::TOKEN_TRIM);
    if (count == 0) {
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        const std::string& kvStr = tokenizer[i];
        StringTokenizer kvTokenizer;
        size_t kvCount = kvTokenizer.Tokenize(kvStr, ":", 
                StringTokenizer::TOKEN_IGNORE_EMPTY | StringTokenizer::TOKEN_TRIM);
        if (kvCount != 2) {
            return false;
        }
        kvMap[kvTokenizer[0]] = kvTokenizer[1];
    }

    return true;
}

bool RequestIndexProcessor::RegisterMethods() {
    //return RegisterMethod(METHOD_GEN_QRS_CLAUSE, &RequestIndexProcessor::GenQrsClause);
    
    return RegisterMethod("gen_qrs_clause", &RequestIndexProcessor::GenQrsClause) &&
           RegisterMethod("read_cache", &RequestIndexProcessor::ReadCache) &&
           RegisterMethod("request_index", &RequestIndexProcessor::RequestIndex) &&
           //RegisterMethod("rerank", &RequestIndexProcessor::Rerank) &&
           //RegisterMethod("fetch_summary", &RequestIndexProcessor::FetchSummary) &&
           RegisterMethod("update_cache", &RequestIndexProcessor::UpdateCache);
}

BaseProcessor* RequestIndexProcessor::DoClone() {
    return new RequestIndexProcessor(*this);
}

void RequestIndexProcessor::DoDestroy() {
    delete this;
}

int RequestIndexProcessor::GenQrsClause(AGG::AggContext *aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    size_t count = aggContext->GetSubContextCount();
    if (count == 0) {
        AGG_LOG(ERROR, "subcontext count is 0");
        return PR_SUCCESS;
    }

    std::vector<AggSubContext*> subContextVec;
    for (size_t i = 0; i < count; i++) {
        AggSubContext* subContext = aggContext->GetSubContext(i);
        CacheInfo* cacheInfo = subContext->GetCacheInfo();
        if (cacheInfo == NULL ||
            cacheInfo->_cacheResultCollector == NULL) {
            subContextVec.push_back(subContext);
        }
    }

    size_t contextInfoVecCount = subContextVec.size();
    if (contextInfoVecCount == 0) {
        return PR_SUCCESS;
    }
    
    for (size_t i = 0; i < contextInfoVecCount; i++) {
        AggSubContext* subContext = subContextVec[i];
        QueryCollector* collector = subContext->GetQueryCollector();
        if (collector == NULL) {
            continue;
        }
        size_t qrsClauseMapCount = collector->GetQrsClauseMapCount();
        for (size_t j = 0; j < qrsClauseMapCount; j++) {
            QrsClauseMap qrsClauseMap;
            if (!collector->GetQrsClauseMap(j, qrsClauseMap)) {
                continue;
            }
            SetQrsClauseMapByParamMap(mQrsClauseParamMap, qrsClauseMap);
        }
    }
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "GenQrsClause Cost:%d", end_time - start_time);
    
    //std::string debuginfo = "RequestIndexProcessor GenQrsClause Cost:" + autil::StringUtil::toString(end_time - start_time);
    //AggWebContextHelper::AppendProcessorDebugInfo(aggContext, debuginfo);

    return PR_SUCCESS;
}

void RequestIndexProcessor::SetQrsClauseMapByParamMap(
        const QrsClauseParamMap& paramMap, QrsClauseMap& qrsClauseMap)
{
    if (paramMap.size() == 0) {
        return;
    }

    for (QrsClauseMapIter iter = qrsClauseMap.begin();
         iter != qrsClauseMap.end(); ++iter) {
        QrsClauseParamMapConstIter paramIter = paramMap.find(iter->first);
        if (paramIter == paramMap.end()) {
            continue;
        }

        QrsClause* qrsClause = iter->second;
        const QrsClauseParam& qrsClauseParam = paramIter->second;
        for (String2StringConstIter configIter = qrsClauseParam._configMap.begin();
             configIter != qrsClauseParam._configMap.end(); ++configIter) {
            qrsClause->SetConfig(configIter->first, configIter->second);
        }

        for (String2StringConstIter kvIter = qrsClauseParam._kvMap.begin();
             kvIter != qrsClauseParam._kvMap.end(); ++kvIter) {
            qrsClause->SetKvPair(kvIter->first, kvIter->second);
        }

        for (String2StringConstIter paramIter = qrsClauseParam._paramMap.begin();
             paramIter != qrsClauseParam._paramMap.end(); ++paramIter) {
            SetQrsClauseParam(qrsClause, paramIter->first, paramIter->second);   
        }
    }
}

void RequestIndexProcessor::SetQrsClauseParam(QrsClause* qrsClause, 
        const std::string& key, const std::string& value)
{
    if (key == "distinct") {
        qrsClause->SetDistinctParam(value);
    } else if (key == "rank") {
        qrsClause->SetRankParam(value);
    } else if (key == "attribute") {
        qrsClause->SetAttributeParam(value);
    } else if (key == "filter") {
        qrsClause->SetFilterParam(value);
    } else {
        AGG_LOG(ERROR, "illegal qrs clause key[%s]", key.c_str());
    }
}

int RequestIndexProcessor::ReadCache(AGG::AggContext *aggContext)
{
    return PR_SUCCESS;
}

int RequestIndexProcessor::UpdateCache(AGG::AggContext *aggContext)
{
    return PR_SUCCESS;
}

int RequestIndexProcessor::RequestIndex(AGG::AggContext *aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    
    string rawQuery = AggContextHelper::GetRawQuery(aggContext);
    if (!rawQuery.empty()) {
        rawQuery = StringTool::ReplaceStr(rawQuery, "\n", "");
    }

    size_t count = mAggContext->GetSubContextCount();
    if (count == 0) {
        AGG_LOG(ERROR, "subcontext count is 0");
        return PR_SUCCESS;
    }
    
    std::vector<SubContextInfo> subContextInfoVec;
    for (size_t i = 0; i < count; i++) {
        AggSubContext* subContext = mAggContext->GetSubContext(i);
        CacheInfo* cacheInfo = subContext->GetCacheInfo();
        if (cacheInfo == NULL ||
            cacheInfo->_cacheResultCollector == NULL) {
            subContextInfoVec.push_back(SubContextInfo(subContext));
        }
    }
    AGG_LOG(ERROR, "debug, sub context size:%d ", (int32_t)subContextInfoVec.size());
    
    size_t contextInfoVecCount = subContextInfoVec.size();
    for (size_t i = 0; i < contextInfoVecCount; i ++) {
        SubContextInfo& subContextInfo = subContextInfoVec[i];
        AggSubContext* subContext = subContextInfo._subContext;
        const std::string & subContextName = subContext->GetName();

        QueryCollector* collector = subContext->GetQueryCollector();
        count = collector->GetQrsClauseMapCount();
        if (count == 0) {
            continue;
        }

        //备注：只选择了第一条qp返回的结果访问
        QrsClauseMap qrsClauseMap;
        collector->GetQrsClauseMap(0, qrsClauseMap);
        
        std::vector<QrsResponsePtr>& qrsResponseVec = subContextInfo._qrsResponseVec;
        for (QrsClauseMapIter iter = qrsClauseMap.begin();
             iter != qrsClauseMap.end(); ++iter) {
            const std::string& indexName = iter->first;
            AGG_LOG(ERROR, "qrsClause indexName: [%s]", indexName.c_str());
            QrsClause* qrsClause = iter->second; 

            if (indexName.find("xueshu") != string::npos) {
                std::string ha3DocNumStr;
                mAggContext->GetRequestParam("ha3_doc_num", ha3DocNumStr);
                uint32_t ha3DocNum = INVALID_INT_VALUE;
                ha3DocNum = StringUtil::StringToUint32(ha3DocNumStr);
                if (ha3DocNum == INVALID_INT_VALUE || ha3DocNumStr.empty()) {
                    ha3DocNumStr = "50";
                }
                AGG_LOG(ERROR, "get ha3_doc_num param [%s], ha3DocNum [%d]", ha3DocNumStr.c_str(), ha3DocNum);
                qrsClause->SetConfig("hit", ha3DocNumStr);

            }
            else if (indexName.find("note") != string::npos) {
                std::string ha3NoteNumStr;
                mAggContext->GetRequestParam("ha3_note_num", ha3NoteNumStr);
                uint32_t ha3NoteNum = INVALID_INT_VALUE;
                ha3NoteNum = StringUtil::StringToUint32(ha3NoteNumStr);
                if (ha3NoteNum == INVALID_INT_VALUE || ha3NoteNumStr.empty()) {
                    ha3NoteNumStr = "50";
                }
                AGG_LOG(ERROR, "get ha3_note_num param [%s], ha3NoteNum [%d]", ha3NoteNumStr.c_str(), ha3NoteNum);
                qrsClause->SetConfig("hit", ha3NoteNumStr);
            }
            
            {// raw
                string finalRequest = "";
                qrsClause->Encode(finalRequest);
                AGG_LOG(ERROR, "Final QrsRequest:: [%s][%s]",indexName.c_str(), finalRequest.c_str());
                
                QrsRequestPtr qrsRequest = std::make_shared<QrsRequest>(rawQuery, qrsClause);
                AggHttpWrapper wrapper(mAggContext, indexName);
                HttpResponsePtr httpResponse = wrapper.AsyncRequest(qrsRequest);
                if (!httpResponse) {
                    AGG_LOG(ERROR, "async request index: %s failed, subContext: %s",
                            indexName.c_str(), subContextName.c_str());
                    continue;
                }
                QrsResponsePtr qrsResponse = std::make_shared<QrsResponse>(subContext,
                                                    indexName, qrsRequest, httpResponse);

                qrsResponseVec.push_back(qrsResponse);
            }
        }
    }

    for (size_t i = 0; i < contextInfoVecCount; i ++) {
        SubContextInfo& subContextInfo = subContextInfoVec[i];
        AggSubContext* subContext = subContextInfo._subContext;
        const std::string & subContextName = subContext->GetName();

        std::vector<QrsResponsePtr>& qrsResponseVec = subContextInfo._qrsResponseVec;
        count = qrsResponseVec.size();
        AGG_LOG(ERROR, "debug, total qrs res:%d ", (int32_t)qrsResponseVec.size());
        for (size_t j = 0; j < count; j++) {
            QrsResultPtr qrsResult = qrsResponseVec[j]->GetResult();
            if (!qrsResult) {
                AGG_LOG(ERROR, "get qrs result failed, subContext: %s", subContextName.c_str());
                continue;
            }
            subContext->AddQrsResult(qrsResult);
            qrsResult->SetSubContext(subContext);
            size_t hitCount = qrsResult->GetHitCount();
            AGG_LOG(ERROR, "ADD QrsHit:%d", (int32_t)hitCount);
            for (size_t j = 0; j < hitCount; j++) {
                QrsHit* qrsHit = qrsResult->GetHit(j);
                if (qrsHit->GetSortValueCount() > 0) {
                    qrsHit->SetScore(qrsHit->GetSortValue(0));
                }
            }
        }
    }

    CalculateTotalHits(subContextInfoVec);
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "RequestIndex Cost:%d", end_time - start_time);
    
    //std::string debuginfo = "RequestIndexProcessor RequestIndex Cost:" + autil::StringUtil::toString(end_time - start_time);
    //AggWebContextHelper::AppendProcessorDebugInfo(aggContext, debuginfo);
    
    return PR_SUCCESS;
}

void RequestIndexProcessor::CalculateTotalHits(const std::vector<SubContextInfo>& subContextInfoVec)
{
    size_t contextInfoVecCount = subContextInfoVec.size();
    for (size_t i = 0; i < contextInfoVecCount; i++) { 
        const SubContextInfo& subContextInfo = subContextInfoVec[i];
        AggSubContext* subContext = subContextInfo._subContext;

        size_t totalHits = 0;
        size_t resultCount = subContext->GetQrsResultCount();
        for (size_t j = 0; j < resultCount; j++) {
            QrsResultPtr qrsResult = subContext->GetQrsResult(j);
            totalHits += qrsResult->GetTotalHits();            
        }
        subContext->SetTotalHits(totalHits);
                
        AGG_LOG(INFO, "subContext Total Hit:%d", (int32_t)totalHits);
    }
    return;
}

AGG_END_NAMESPACE(agg_web2)
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(medical)::RequestIndexProcessor)

