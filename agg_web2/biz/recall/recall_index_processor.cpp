#include <agg_web2/biz/recall/recall_index_processor.h>
#include "agg_web2/common/common.h"
#include <agg2/helper/agg_context_helper.h>
#include <agg2/util/string_tool.h>
#include <agg2/common/qrs/qrs_request.h>
#include <agg2/common/qrs/qrs_result.h>
#include <agg2/common/qrs/qrs_response.h>
#include <agg2/helper/qrs_hit_helper.h>
#include <agg_web2/data/key_value_data.h>
#include <agg_web2/biz/recall/dump_s3_summary_wrapper.h>
#include <agg2/wrapper/agg_summary_wrapper_factory.h>
#include <autil/HashAlgorithm.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, RecallIndexProcessor);

const std::string CHANNELS_CLUSTERS_PARAM_KEY = "channels_clusters";
const std::string ATTRIBUTE_KEY = "attribute";

const std::string PARAM_RULE = "rule";

const std::string PHASE1_NODE = "phase1_node";
const std::string PHASE2_NODE = "phase2_node";
const std::string PHASE2_TYPE = "phase2_type";
const std::string NODE_TYPE_S3 = "s3";
const std::string NODE_TYPE_QRS = "ha3";

const std::string NODE_PARAM_PROFILE = "profile";
const std::string NODE_PARAM_TIMEOUT = "timeout";
const std::string NODE_PARAM_KEY_TYPE = "key_type";
const std::string NODE_PARAM_KEEP_RANGE_ID = "keep_range_id";
const std::string CLUSTER_REPLACE = "cluster_replace";

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(RecallIndexProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(RecallIndexProcessor)

RecallIndexProcessor::RecallIndexProcessor()
{
}

RecallIndexProcessor::~RecallIndexProcessor()
{
}

bool RecallIndexProcessor::DoInit(const String2String & params)
{
    PARSE_PROCESSOR_PARAM(params, ATTRIBUTE_KEY, mAttributeParam);
    std::string value;
    PARSE_PROCESSOR_PARAM(params, CHANNELS_CLUSTERS_PARAM_KEY, value);
    StringTokenizer tokenizer;
    mClusterIdx = 1;
    size_t count = tokenizer.Tokenize(value, ";", StringTokenizer::TOKEN_TRIM);
    for (size_t i = 0; i < count; ++i) {
        std::string token = tokenizer[i];
        StringTokenizer tokenizer2;
        size_t count2 = tokenizer2.Tokenize(token, ":", StringTokenizer::TOKEN_TRIM);
        if (count2 != 3){
            AGG_LOG(ERROR, "channel and cluster count is not equal 3");
            return false;
        }
        /*if(mChannels.find(tokenizer2[1]) == std::string::npos) {
            if (mChannels != "") {
                mChannels += ",";
            }
            mChannels += tokenizer2[1];
        }*/
        QpParamElem elem;
        CHECK_FALSE_RET(elem.Init(tokenizer2.GetTokenVector()));
        mQpParamElemVec.push_back(elem);
    }
    mClusterIdx = count;
    //AGG_LOG(DEBUG, "mChannels=%s", mChannels.c_str());
    
    string ruleStr;
    PARSE_PROCESSOR_PARAM(params, PARAM_RULE, ruleStr);
    ParseRule(ruleStr);

    return true;
}

bool RecallIndexProcessor::ParseRule(const std::string& rule)
{
    int32_t flags = MESSER_UTILS_NS(util)::StringTokenizer::TOKEN_IGNORE_EMPTY |\
                    MESSER_UTILS_NS(util)::StringTokenizer::TOKEN_TRIM;
    MESSER_UTILS_NS(util)::StringTokenizer ruleTokenizer;
    size_t ruleCount = ruleTokenizer.Tokenize(rule, ";", flags);

    for (size_t i = 0; i < ruleCount; i++) {
        const std::string& ruleItem = ruleTokenizer[i];
        std::string phase1Node;
        NodeParam nodeParam;
        SummaryNodeOption summaryNodeOption;
        if (!ParseNodeParam(ruleItem, phase1Node, nodeParam, summaryNodeOption)) {
            return false;
        }
        mNodeParamMap[phase1Node] = nodeParam;
        mSummaryNodeOptionParamMap[nodeParam._nodeName]._type = nodeParam._type;
        mSummaryNodeOptionParamMap[nodeParam._nodeName]._summaryNodeOption = summaryNodeOption;
        AGG_LOG(DEBUG, "pharse1Node:%s, nodeType:%d, summary:%s, summaryNodeOption:[profile:%s, keyType:%s, timeout:%s, keepRangeId:%d]",
                phase1Node.c_str(), int(nodeParam._type), nodeParam._nodeName.c_str(), summaryNodeOption._profile.c_str(),
                summaryNodeOption._keyType.c_str(), summaryNodeOption._timeout.c_str(), int(summaryNodeOption._keepRangeId));
    }
    return true;
}

bool RecallIndexProcessor::ParseNodeParam(const std::string & nodeRule, 
                                           std::string& phase1Node, 
                                           NodeParam& nodeParam, 
                                           SummaryNodeOption & summaryNodeOption) const
{
    AGG_LOG(DEBUG, "NodeRule: %s", nodeRule.c_str());
    int32_t flags = MESSER_UTILS_NS(util)::StringTokenizer::TOKEN_IGNORE_EMPTY |\
                    MESSER_UTILS_NS(util)::StringTokenizer::TOKEN_TRIM;
    
    MESSER_UTILS_NS(util)::StringTokenizer tokenizer;
    size_t count = tokenizer.Tokenize(nodeRule, ",", flags);
    
    bool keepRangeIdSet = false;
    phase1Node.clear();
    nodeParam._type = ST_UNKNOWN;
    nodeParam._nodeName.clear();
    for (size_t i = 0; i < count; i++) {
        const std::string& kvItem = tokenizer[i];
        MESSER_UTILS_NS(util)::StringTokenizer kvItemTokenizer;
        if (kvItemTokenizer.Tokenize(kvItem, ":", flags) != 2) {
            AGG_LOG(ERROR, "bad kv [%s]", kvItem.c_str());
            return false;
        }

        const std::string& key = kvItemTokenizer[0];
        const std::string& value = kvItemTokenizer[1];
            
        if (key == PHASE1_NODE) {
            if (!phase1Node.empty()) {
                AGG_LOG(ERROR, "%s already set", PHASE1_NODE.c_str());
                return false;
            }
            phase1Node = value;
        } else if (key == PHASE2_NODE) {
            if (!nodeParam._nodeName.empty()) {
                AGG_LOG(ERROR, "%s already set", PHASE2_NODE.c_str());
                return false;
            }
            nodeParam._nodeName = value;
        } else if (key == PHASE2_TYPE) {
            if (nodeParam._type != ST_UNKNOWN) {
                AGG_LOG(ERROR, "%s already set", PHASE2_TYPE.c_str());
                return false;
            }
            if (value == SUMMARY_TYPE_QRS) {
                nodeParam._type = ST_QRS;
            } else if (value == SUMMARY_TYPE_S3) {
                nodeParam._type = ST_S3;
            } else {
                AGG_LOG(ERROR, "bad phase2 node type [%s]", value.c_str());
                return false;
            }
        } else if (key == NODE_PARAM_PROFILE) {
            if (!summaryNodeOption._profile.empty()) {
                AGG_LOG(ERROR, "%s already set", NODE_PARAM_PROFILE.c_str());
                return false;
            }
            summaryNodeOption._profile = value;
        } else if (key == NODE_PARAM_KEY_TYPE) {
            if (!summaryNodeOption._keyType.empty()) {
                AGG_LOG(ERROR, "%s already set", NODE_PARAM_KEY_TYPE.c_str());
                return false;
            }
            summaryNodeOption._keyType = value;
        } else if (key == NODE_PARAM_TIMEOUT) {
            if (!summaryNodeOption._timeout.empty()) {
                AGG_LOG(ERROR, "%s already set", NODE_PARAM_TIMEOUT.c_str());
                return false;
            }
            summaryNodeOption._timeout = value;
        } else if (key == NODE_PARAM_KEEP_RANGE_ID) {
            if (keepRangeIdSet) {
                AGG_LOG(ERROR, "%s already set", NODE_PARAM_KEEP_RANGE_ID.c_str());
                return false;
            }
            keepRangeIdSet = true;
            summaryNodeOption._keepRangeId = (value == "yes");
        }
    }

    if (phase1Node.empty()) {
        AGG_LOG(ERROR, "%s must be set", PHASE1_NODE.c_str());
        return false;
    }
    if (nodeParam._type == ST_UNKNOWN) {
        AGG_LOG(ERROR, "%s must be set", PHASE2_TYPE.c_str());
        return false;
    }
    if (nodeParam._nodeName.empty()) {
        AGG_LOG(ERROR, "%s must be set", PHASE2_NODE.c_str());
        return false;
    }
    if (nodeParam._type == ST_S3) {
        if (!keepRangeIdSet) {
            AGG_LOG(ERROR, "%s must be set", NODE_PARAM_KEEP_RANGE_ID.c_str());
            return false;
        }
        if (summaryNodeOption._keyType.empty()) {
            AGG_LOG(ERROR, "%s must be set", NODE_PARAM_KEY_TYPE.c_str());
            return false;
        }
    }
    return true;
}

void RecallIndexProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

int RecallIndexProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    
    // step 0, gen sub context
    GenSubContextByQrsClause("RAWQUERY_WEB_AGG_QRS_CLAUSE", "original");
    //GenSubContextByQrsClause("REWRITEQUERY_WEB_AGG_QRS_CLAUSE", "rewrite");//如果有改写query，就加上
    
    // step 1, 发起请求，并行
    std::vector<QrsResponsePtr> qrsResponseVec;
    std::vector<std::string> nameVec;
    std::vector<AggSubContext*> subContextVec;

    //每个TAG对应的值，详细见 https://aliyuque.antfin.com/mwkrzr/eiy88p/ryv4oyxm584nq43n?singleDoc#
    //SendQrsRequestByTag(qrsResponseVec, nameVec, subContextVec, "aigc_index", 0, "9", 20, "知乎");
    SendQrsRequestByTag(qrsResponseVec, nameVec, subContextVec, "aigc_index", 0, "4", 5, "百科");
    //SendQrsRequestByTag(qrsResponseVec, nameVec, subContextVec, "aigc_index", 0, "1", 10, "绝对权威");
    
    SendQrsRequestByHost(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, "www.gaokao.cn,m.gaokao.cn,gaokao.eol.cn", 20, "EOL");
    SendQrsRequestByHost(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, "www.dxsbb.com,m.dxsbb.com", 20, "DXSBB");
    SendQrsRequestByHost(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, "zhuanlan.zhihu.com,www.zhihu.com", 20, "ZHIHU");
    
    string pkurls;
    mAggContext->GetRequestParam("pk_urls", pkurls);
    SendQrsRequestByUrls(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, pkurls, 100, "PKURLS");

    string colldgeAuthorityUrls, collegeName;
    string query = AggContextHelper::GetRawQuery(mAggContext);
    string collegeAuthorityUrls = GetCollegeAuthorityData(query, collegeName);
    if (collegeAuthorityUrls.size()>0) {
        SendQrsRequestByUrls(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, collegeAuthorityUrls, 100, "PKURLS");
    }
    
    /* * 
     * * Query意图汇总，详见 https://aliyuque.antfin.com/cnh8gg/mzw0f7/ktueva3mm7nkf2e8
     * * 详细分类：https://aliyuque.antfin.com/cnh8gg/mzw0f7/dqbtzgkyqiyc39as
     * * 全部意图："其他-非高考", "其他-高考相关", "问信息-院校", "问信息-专业", "问信息-院校专业", "问信息-其他", "问政策-院校", "问政策-专业", "问政策-院校专业", "问政策-其他", "问观点&评价-院校", "问观点&评价-专业", "问观点&评价-院校专业", "问观点&评价-其他", "问方法-院校", "问方法-专业", "问方法-院校专业", "问方法-其他", "问原因-院校", "问原因-专业", "问原因-院校专业", "问原因-其他", "问建议&推荐-院校", "问建议&推荐-专业", "问建议&推荐-院校专业", "问建议&推荐-其他"
     * *
     * *
     * * 权威站点HostType汇总，详见 https://aliyuque.antfin.com/cnh8gg/mzw0f7/qe1oqv9hor25gn1k#UoFf
     * * HostType = 权威-gov, 政府网站，国家、教育部、考试院、公安厅等
     * * HostType = 权威-college, 院校介绍页
     * * HostType = 权威-college-zhaosheng，院校招生网
     * * HostType = 综合，EOL、大学生必备网 等
     */
    
    vector<string> queryIntents;
    aggContext->GetValue("QUERY_INTENTS", queryIntents);
    string queryIntentStr = autil::StringUtil::toString(queryIntents, ",");

    if (!queryIntentStr.empty() && (queryIntentStr.find("问政策-其他") != string::npos ||
                queryIntentStr.find("问信息-其他") != string::npos)) { //问政策，目前信号不准，待优化
        AggDataManager * aggDataManager = AggDataManager::GetInstance();
        AggDataWrapper aggDataWrapper(aggDataManager, "college_authority_data");
        const auto& kvData = aggDataWrapper.Get<KeyValueData>()->GetkvData();
    
        string match, prov;
        for(const auto& kv:kvData) {
            const std::string& name = kv.first;
            if (name.find("高考政策") == string::npos)
                continue;
            
            string _name = StringTool::ReplaceStr(name, "高考政策", "", false);
            if (query.find(_name) != string::npos) {
                if (_name.size() > prov.size()) { // 取最大匹配
                    match = kv.second;
                    prov = _name;
                }
            }
        }
        
        if (match.size() > 0) {
            SendQrsRequestByUrls(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, match, 200, "PKURLS");
        }

        SendQrsRequestByHost(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, "HostType#权威-gov", 10, "GOV");
    }

    if (!queryIntentStr.empty() && queryIntentStr.find("院校") != string::npos) { // 院校
        SendQrsRequestByHost(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, "HostType#权威-college,HostType#权威-college-zhaosheng", 10, "GOV");
    }

    SendQrsRequestByHost(qrsResponseVec, nameVec, subContextVec, "gaokao_index", 0, "HostType#综合", 10, "综合优质");

    // step 2, 获取结果
    vector<LLMDocPtr> hitQueueIndex;
    GetQrsResult(qrsResponseVec, nameVec, subContextVec, hitQueueIndex);
    
    int64_t cur_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "GetQrsResult Cost:%d", cur_time - start_time);
    
    // step 3, 获取Summary
    vector<QrsHit*> qrsHitVec;
    for(size_t i = 0; i < hitQueueIndex.size(); ++i) {
        qrsHitVec.push_back(hitQueueIndex[i]->mQrsHit);
    }

    map<AggSubContext*, size_t> subContext2InfoIdx;
    std::vector<SubContextInfo> subContextInfoVec;
    for(size_t i = 0; i < qrsHitVec.size(); ++i) {
        QrsHit* qrsHit = qrsHitVec[i];
        
        // fill SubContextInfo
        AggSubContext* subContext = QrsHitHelper::GetAggSubContext(qrsHit);
        if (subContext2InfoIdx.find(subContext) == subContext2InfoIdx.end()) {
            SubContextInfo info;
            info._subContext = subContext;
            subContextInfoVec.push_back(info);
            subContext2InfoIdx[subContext] = subContextInfoVec.size() - 1;
        }
        SubContextInfo& info = subContextInfoVec[subContext2InfoIdx[subContext]];
        info._attributeHitVec.push_back(qrsHit);
    }
    
    FillNode2QrsHitVec(subContextInfoVec);

    for (size_t i = 0; i < subContextInfoVec.size(); i++) {
        SendRequest(subContextInfoVec[i]);
    }
    
    for (size_t i = 0; i < subContextInfoVec.size(); i++) {
        GetResponse(subContextInfoVec[i]);
    }
    
    for (size_t i = 0; i < hitQueueIndex.size(); ++i) {
        QrsHit* qrsHit = hitQueueIndex[i]->mQrsHit;
        string title, body;
        qrsHit->GetContent("MainBody", body);
        if (body.empty()) {
            qrsHit->GetContent("V_MAIN_BODY", body);
        }
        body = StringTool::ReplaceStr(body, "\t", "", false);
        body = StringTool::ReplaceStr(body, "<em>", "", false);
        body = StringTool::ReplaceStr(body, "</em>", "", false);

        qrsHit->GetContent("Title", title);
        hitQueueIndex[i]->mTitle = title;
        hitQueueIndex[i]->mBody = body;
        // AGG_LOG(ERROR, "add hitQueueIndex, title=%s, body=%s",
		// 		title.c_str(), body.c_str());
    }
        
    mAggContext->SetValue("RAWQUERY_INDEX_RESULT", hitQueueIndex);

    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "RecallIndexProcessor Cost:%d", end_time - start_time);
    return PR_SUCCESS;
}

string RecallIndexProcessor::GetCollegeAuthorityData(const std::string& query, std::string& college)
{
    AggDataManager * aggDataManager = AggDataManager::GetInstance();
    AggDataWrapper aggDataWrapper(aggDataManager, "college_authority_data");
    // auto kvData = aggDataWrapper.Get<KeyValueData>();
    // if (kvData) {
    //     std::string value = kvData->GetValue(name);
    //     AGG_LOG(ERROR, "GetCollegeAuthorityData name=%s, value=%s", name.c_str(), value.c_str());
    //     return value;
    // }
    // else {
    //     AGG_LOG(ERROR, "GetCollegeAuthorityData failed");
    // }
    const auto& kvData = aggDataWrapper.Get<KeyValueData>()->GetkvData();

    string policy;
    mAggContext->GetRequestParam("policy", policy);
    
    string match;
    for(const auto& kv:kvData) {
        string name = kv.first;
        if (policy == "1") {
            name = StringTool::ReplaceStr(name, "高考政策", "");
        }
        if (query.find(name)!=string::npos) {
            //std::string value = kv.second;
            //AGG_LOG(ERROR, "GetCollegeAuthorityData name=%s, value=%s", name.c_str(), value.c_str());
            //return value;
            if (name.size() > college.size()) { // 取最大匹配
                match = kv.second;
                college = name;
            }
        }
    }
    //AGG_LOG(ERROR, "GetCollegeAuthorityData failed");
    return match; 
}
bool RecallIndexProcessor::GenSubContextByQrsClause(const std::string& key, const std::string& subContextName)
{
    string qrsQuery;
    mAggContext->GetValue(key, qrsQuery);
    if (qrsQuery.empty()) {
        AGG_LOG(ERROR, "qrsQuery empty");
        return false;
    }
    
    QrsClauseMap qrsClauseMap;
    for (int i = 0; i < mClusterIdx; ++i) {
        const std::string& qrsKey = mQpParamElemVec[i].mQrsKey;
        std::string qpKey = "HA2" + mQpParamElemVec[i].mQpKey;
        const std::string& clusterName = mQpParamElemVec[i].mClusterName;

        QrsClause * qrsClause = new QrsClause();
        qrsClause->Decode(qrsQuery);
        qrsClause->SetClusterName(clusterName);
        qrsClause->SetAttributeParam(mAttributeParam);
        //qrsClause->RemoveConfig("acnn_embedding");
        //qrsClause->RemoveConfig("qtcnn_embedding");
        std::string qrsEncodeQuery;
        qrsClause->Encode(qrsEncodeQuery);
        qrsClauseMap[qrsKey] = qrsClause;

        // AGG_LOG(ERROR, "clusterName=%s, qrsKey=%s, qpKey=%s, qrsEncodeQuery=%s",
        //         clusterName.c_str(), qrsKey.c_str(), qpKey.c_str(), qrsEncodeQuery.c_str());
    }
    
    std::vector<QrsClauseMap> qrsClauseMapVec;
    qrsClauseMapVec.emplace_back(qrsClauseMap);

    //string subContextName = "original";
    AggSubContext * aggSubContext;
    if (!RegisterSubContext(qrsClauseMapVec, aggSubContext, subContextName)){
        ReleaseQrsClauseMapVec(qrsClauseMapVec);
        //traceInfoPtr->AddTrace("RegisterSubContext failed");
        AGG_LOG(ERROR, "RegisterSubContext Failed!");
        return false;
    }

    return true;
}

bool RecallIndexProcessor::RegisterSubContext(std::vector<QrsClauseMap> & qrsClauseMapVec,
        AggSubContext * & subContext, const std::string& subContextName)
{
    QueryCollector* collector = new QueryCollector;
    if (!collector->Init(qrsClauseMapVec)) {
        AGG_LOG(ERROR, "InitQueryCollector Failed!");
        A4_DELETE_AND_SET_NULL(collector);
        return false;
    }

    subContext = new AggSubContext;
    subContext->SetQueryCollector(collector);
    subContext->SetContext(mAggContext);
    AggSubContextHelper::SetSubContextName(subContext, subContextName);
    mAggContext->AddSubContext(subContext);

    return true;
}

void RecallIndexProcessor::ReleaseQrsClauseMapVec(std::vector<QrsClauseMap>& qrsClauseMapVec)
{
    for (size_t i = 0; i < qrsClauseMapVec.size(); i++) {
        QrsClauseMap& qrsClauseMap = qrsClauseMapVec[i];
        for (QrsClauseMapIter iter = qrsClauseMap.begin(); iter != qrsClauseMap.end(); ++iter) {
            A4_DELETE_AND_SET_NULL(iter->second);
        }
    }
}

bool RecallIndexProcessor::SendQrsRequestByTag(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec, const std::string& indexNames,
        int queryId, const std::string& category, int hitNum, const std::string& name)
{
    vector<string> indexNameSet = autil::StringUtil::split(indexNames.c_str(), ",");

    for (size_t i = 0; i < mAggContext->GetSubContextCount(); ++i) {
        AggSubContext* aggSubContext = mAggContext->GetSubContext(i);
        SendSubContextQrsRequestByTag(qrsResponseVec, nameVec, subContextVec, 
                aggSubContext, indexNameSet, queryId, category, hitNum, name);
    }

    return true;
}

bool RecallIndexProcessor::SendSubContextQrsRequestByTag(
        std::vector<QrsResponsePtr>& qrsResponseVec, std::vector<std::string>& nameVec,
        std::vector<AggSubContext*>& subContextVec, AggSubContext* aggSubContext,
        const std::vector<std::string>& indexNameSet,
        int queryId, const std::string& category, int hitNum, const std::string& name) 
{
    QueryCollector * collector = aggSubContext->GetQueryCollector();
    CHECK_NULL_RET(collector);

    QrsClauseMap qrsClauseMap;
    if (!collector->GetQrsClauseMap(queryId, qrsClauseMap)) {
        AGG_LOG(ERROR, "Can't Find queryMap for query:%d", (int)queryId);
        return false;
    }

    const std::string & requestQuery = mAggContext->GetAggRequest()->_query;

    for (std::vector<std::string>::const_iterator it = indexNameSet.begin();
            it != indexNameSet.end(); ++it) {

        const std::string indexName = *it;

        AGG_LOG(DEBUG, "SendSubContextQrsRequest indexName[%s].", indexName.c_str());

        if (qrsClauseMap.find(indexName) == qrsClauseMap.end()) {
            AGG_LOG(ERROR, "qrsClauseMap.find(%s) failed", indexName.c_str());
            continue;
        }

        QrsClause* qrsClause = qrsClauseMap[indexName];
        qrsClause->SetConfig("hit", std::to_string(hitNum));
        qrsClause->SetConfig("timeout", "5000");
        qrsClause->SetRankParam("rank_profile:DefaultProfile");
        qrsClause->SetKvPair("feature_service", "qa_good");
        //qrsClause->SetConfig("rank_size", mDefaultRankSize);

        std::string oldSearchQuery;
        qrsClause->GetQuery(oldSearchQuery);
        string searchQuery = oldSearchQuery;
		searchQuery = StringTool::ReplaceStr(searchQuery, "phrase_at:", "phrase:", false);
        if (! category.empty()) {
            searchQuery = "(" + searchQuery + ") AND tag:" + category;
            qrsClause->SetQuery(searchQuery);
        }
        
        AGG_LOG(DEBUG, "oldSearchQuery=%s", oldSearchQuery.c_str());
        AGG_LOG(DEBUG, "searchQuery=%s", searchQuery.c_str());

        QrsRequestPtr qrsRequest(new QrsRequest(requestQuery, qrsClause));
        qrsRequest->SetHttpMethod(HM_POST);

        AggHttpWrapper qrsWrapper(mAggContext, indexName);
        std::string qrsString;
        qrsClause->Encode(qrsString);

        // AGG_LOG(ERROR, "indexName[%s], QrsRequest [%s]", indexName.c_str(), qrsString.c_str());

        HttpResponsePtr httpResponse = qrsWrapper.AsyncRequest(qrsRequest);
        qrsClause->SetQuery(oldSearchQuery);
        if (!httpResponse) {
            continue;
        }
        QrsResponsePtr qrsResponse(new QrsResponse(aggSubContext, indexName, qrsRequest, httpResponse));
        //std::string responseId = GET_RESPONSE_KEY(indexName, queryId, category);
        //aggSubContext->SetValue(responseId, qrsResponse);
        qrsResponseVec.emplace_back(qrsResponse);
        nameVec.push_back(name);
        subContextVec.emplace_back(aggSubContext);
    }
    return true;
}

bool RecallIndexProcessor::SendQrsRequestByHost(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec, const std::string& indexNames,
        int queryId, const std::string& host, int hitNum, const std::string& name)
{
    vector<string> indexNameSet = autil::StringUtil::split(indexNames.c_str(), ",");

    for (size_t i = 0; i < mAggContext->GetSubContextCount(); ++i) {
        AggSubContext* aggSubContext = mAggContext->GetSubContext(i);
        SendSubContextQrsRequestByHost(qrsResponseVec, nameVec, subContextVec, 
                aggSubContext, indexNameSet, queryId, host, hitNum, name);
    }

    return true;
}

bool RecallIndexProcessor::SendSubContextQrsRequestByHost(
        std::vector<QrsResponsePtr>& qrsResponseVec, std::vector<std::string>& nameVec,
        std::vector<AggSubContext*>& subContextVec, AggSubContext* aggSubContext,
        const std::vector<std::string>& indexNameSet,
        int queryId, const std::string& host, int hitNum, const std::string& name) 
{
    QueryCollector * collector = aggSubContext->GetQueryCollector();
    CHECK_NULL_RET(collector);

    QrsClauseMap qrsClauseMap;
    if (!collector->GetQrsClauseMap(queryId, qrsClauseMap)) {
        AGG_LOG(ERROR, "Can't Find queryMap for query:%d", (int)queryId);
        return false;
    }

    const std::string & requestQuery = mAggContext->GetAggRequest()->_query;

    for (std::vector<std::string>::const_iterator it = indexNameSet.begin();
            it != indexNameSet.end(); ++it) {

        const std::string indexName = *it;

        AGG_LOG(DEBUG, "SendSubContextQrsRequest indexName[%s].", indexName.c_str());

        if (qrsClauseMap.find(indexName) == qrsClauseMap.end()) {
            AGG_LOG(ERROR, "qrsClauseMap.find(%s) failed", indexName.c_str());
            continue;
        }

        QrsClause* qrsClause = qrsClauseMap[indexName];
        qrsClause->SetConfig("hit", std::to_string(hitNum));
        qrsClause->SetConfig("timeout", "5000");
        qrsClause->SetRankParam("rank_profile:DefaultProfile");
        qrsClause->SetKvPair("feature_service", "sgs-gaokao");

        //qrsClause->SetKvPair("feature_service", "qa_good");
        //qrsClause->SetConfig("rank_size", mDefaultRankSize);
        
        std::string oldSearchQuery;
        qrsClause->GetQuery(oldSearchQuery);
        string searchQuery = oldSearchQuery;
		//searchQuery = StringTool::ReplaceStr(searchQuery, "phrase_at:", "phrase:", false);
        //if (! category.empty()) {
        //    searchQuery = "(" + searchQuery + ") AND tag:" + category;
            qrsClause->SetQuery(searchQuery);
        //}

        std::string oldFilterStr;
        qrsClause->GetFilterParam(oldFilterStr);
        string hostFilter;
        vector<string> hosts = autil::StringUtil::split(host.c_str(), ",", true);
        for (size_t ii = 0; ii < hosts.size(); ++ii) {
            if (hosts[ii].find("HostType#") == 0) { // HostType模式
                if (!hostFilter.empty())
                    hostFilter += " OR ";
                hostFilter += "HostType=\"" + hosts[ii].substr(9) +"\"";
                continue;    
            }
            string host_hash = autil::StringUtil::toString((int64_t)autil::HashAlgorithm::hashString64(hosts[ii].c_str()));
            if (!hostFilter.empty())
                hostFilter += " OR ";
            hostFilter += "V_HOST_HASH=" + host_hash;
        } 
                    
        string timeFilter = "V_TIME>";
        int64_t curtime = autil::TimeUtility::currentTimeInSeconds();
        curtime -= 365 * 24 * 3600 * 2;// 2年内
        string curtime_str;
        SAFE_TYPE_CHANGE(curtime, curtime_str);
        timeFilter.append(curtime_str);
        timeFilter.append(" AND (" + hostFilter + ")");
        
        qrsClause->SetFilterParam(timeFilter);
 
        AGG_LOG(DEBUG, "oldSearchQuery=%s", oldSearchQuery.c_str());
        AGG_LOG(DEBUG, "searchQuery=%s", searchQuery.c_str());

        QrsRequestPtr qrsRequest(new QrsRequest(requestQuery, qrsClause));
        qrsRequest->SetHttpMethod(HM_POST);

        AggHttpWrapper qrsWrapper(mAggContext, indexName);
        std::string qrsString;
        qrsClause->Encode(qrsString);

        // AGG_LOG(ERROR, "indexName[%s], QrsRequest [%s]", indexName.c_str(), qrsString.c_str());

        HttpResponsePtr httpResponse = qrsWrapper.AsyncRequest(qrsRequest);
        qrsClause->SetQuery(oldSearchQuery) ; // 还原
        qrsClause->SetFilterParam(oldFilterStr); // 还原
        if (!httpResponse) {
            continue;
        }
        QrsResponsePtr qrsResponse(new QrsResponse(aggSubContext, indexName, qrsRequest, httpResponse));
        //std::string responseId = GET_RESPONSE_KEY(indexName, queryId, category);
        //aggSubContext->SetValue(responseId, qrsResponse);
        qrsResponseVec.emplace_back(qrsResponse);
        nameVec.push_back(name);
        subContextVec.emplace_back(aggSubContext);
    }
    return true;
}

bool RecallIndexProcessor::SendQrsRequestByUrls(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec, const std::string& indexNames,
        int queryId, const std::string& urls, int hitNum, const std::string& name)
{
    vector<string> indexNameSet = autil::StringUtil::split(indexNames.c_str(), ",");

    for (size_t i = 0; i < mAggContext->GetSubContextCount(); ++i) {
        AggSubContext* aggSubContext = mAggContext->GetSubContext(i);
        SendSubContextQrsRequestByUrls(qrsResponseVec, nameVec, subContextVec, 
                aggSubContext, indexNameSet, queryId, urls, hitNum, name);
    }

    return true;
}

bool RecallIndexProcessor::SendSubContextQrsRequestByUrls(
        std::vector<QrsResponsePtr>& qrsResponseVec, std::vector<std::string>& nameVec,
        std::vector<AggSubContext*>& subContextVec, AggSubContext* aggSubContext,
        const std::vector<std::string>& indexNameSet,
        int queryId, const std::string& urls, int hitNum, const std::string& name) 
{
    if (urls.empty())
        return true;

    QueryCollector * collector = aggSubContext->GetQueryCollector();
    CHECK_NULL_RET(collector);

    QrsClauseMap qrsClauseMap;
    if (!collector->GetQrsClauseMap(queryId, qrsClauseMap)) {
        AGG_LOG(ERROR, "Can't Find queryMap for query:%d", (int)queryId);
        return false;
    }

    const std::string & requestQuery = mAggContext->GetAggRequest()->_query;

    for (std::vector<std::string>::const_iterator it = indexNameSet.begin();
            it != indexNameSet.end(); ++it) {

        const std::string indexName = *it;

        AGG_LOG(DEBUG, "SendSubContextQrsRequest indexName[%s].", indexName.c_str());

        if (qrsClauseMap.find(indexName) == qrsClauseMap.end()) {
            AGG_LOG(ERROR, "qrsClauseMap.find(%s) failed", indexName.c_str());
            continue;
        }

        QrsClause* qrsClause = qrsClauseMap[indexName];
        qrsClause->SetConfig("hit", std::to_string(hitNum));
        qrsClause->SetConfig("timeout", "5000");
        qrsClause->SetRankParam("rank_profile:DefaultProfile");
        //qrsClause->SetKvPair("feature_service", "qa_good");
        //qrsClause->SetConfig("rank_size", mDefaultRankSize);

        std::string oldSearchQuery;
        qrsClause->GetQuery(oldSearchQuery);
        string searchQuery = oldSearchQuery;
		//searchQuery = StringTool::ReplaceStr(searchQuery, "phrase_at:", "phrase:", false);
        //if (! category.empty()) {
        //    searchQuery = "(" + searchQuery + ") AND tag:" + category;
            //qrsClause->SetQuery(searchQuery);
        //}
        
        string pkStr;
        vector<string> urlsVec = autil::StringUtil::split(urls.c_str(), ",", true);
        for (size_t ii = 0; ii < urlsVec.size(); ++ii) {
            if (!pkStr.empty())
                pkStr += " OR ";
            pkStr += "pk:\"" + urlsVec[ii] + "\"";
        }
        
        pkStr = "("+pkStr+")";

        searchQuery = StringTool::ReplaceStr(searchQuery, "(", "", false);
        searchQuery = StringTool::ReplaceStr(searchQuery, ")", "", false);
        searchQuery = StringTool::ReplaceStr(searchQuery, " OR ", "\t", false);
        searchQuery = StringTool::ReplaceStr(searchQuery, " AND ", "\t", false);
        searchQuery = StringTool::ReplaceStr(searchQuery, " RANK ", "\t", false);
        set<string> uniq;
        vector<string> tokens = autil::StringUtil::split(searchQuery, "\t", true);
        for (size_t ii = 0; ii < tokens.size(); ++ii) {
            if (uniq.find(tokens[ii]) != uniq.end())
                continue;
            pkStr += " RANK " + tokens[ii];
            uniq.insert(tokens[ii]);
        }
        qrsClause->SetQuery(pkStr);

        //qrsClause->SetFilterParam(hostFilter);
 
        AGG_LOG(DEBUG, "oldSearchQuery=%s", oldSearchQuery.c_str());
        AGG_LOG(DEBUG, "searchQuery=%s", searchQuery.c_str());

        QrsRequestPtr qrsRequest(new QrsRequest(requestQuery, qrsClause));
        qrsRequest->SetHttpMethod(HM_POST);

        AggHttpWrapper qrsWrapper(mAggContext, indexName);
        std::string qrsString;
        qrsClause->Encode(qrsString);

        // AGG_LOG(ERROR, "indexName[%s], QrsRequest [%s]", indexName.c_str(), qrsString.c_str());

        HttpResponsePtr httpResponse = qrsWrapper.AsyncRequest(qrsRequest);
        qrsClause->SetQuery(oldSearchQuery);
        if (!httpResponse) {
            continue;
        }
        QrsResponsePtr qrsResponse(new QrsResponse(aggSubContext, indexName, qrsRequest, httpResponse));
        //std::string responseId = GET_RESPONSE_KEY(indexName, queryId, category);
        //aggSubContext->SetValue(responseId, qrsResponse);
        qrsResponseVec.emplace_back(qrsResponse);
        nameVec.push_back(name);
        subContextVec.emplace_back(aggSubContext);
    }
    return true;
}

static const std::vector<std::string> PUBLISH_TIME_FEATURE = {
    "SS_PUBLISHDATE",
    "NEWSSC_PUBLISH_TIME",
    "VIDEO_PUBLISHDATE",
    "PublishTime",
    "V_TIME",
};

int64_t GetPublishTime(QrsHit* qrsHit)
{
    for (auto& feature: PUBLISH_TIME_FEATURE) {
        int64_t publishTime = QrsHitHelper::GetIntAttr(qrsHit, feature, 0);
        if (publishTime > 0) {
            return publishTime;
        }
    }
    return 0;
}

std::string RecallIndexProcessor::GetFormatTime(int Currenttime)
{
    //if (Currenttime < 1600000000) {
    if (Currenttime < 1000000000) {
        return std::string("");
    }

    char outStr[256];
    time_t t = Currenttime;
    struct tm *tmp = localtime(&t);
    strftime(outStr, sizeof(outStr), "%Y-%m-%d %H:%M:%S", tmp);
    return std::string(outStr);
}
    
void RecallIndexProcessor::GetQrsResult(std::vector<QrsResponsePtr>& qrsResponseVec, std::vector<std::string>& nameVec,
        std::vector<AggSubContext*>& subContextVec, vector<LLMDocPtr>& hitQueueIndex)
{
    if (qrsResponseVec.size() != nameVec.size() || qrsResponseVec.size() != subContextVec.size())
        return;

    for (size_t idx = 0; idx < qrsResponseVec.size(); ++idx) {
        string recall_name = nameVec[idx];
        QrsResponsePtr qrsResponse = qrsResponseVec[idx]; 
        AggSubContext * aggSubContext = subContextVec[idx];
        if (!qrsResponse) {
            continue;
        }

        QrsResultPtr qrsResult = qrsResponse->GetResult();
        if (! qrsResult) {
            AGG_LOG(ERROR, "Get Qrs Result Failed.");
            continue;
        }

        string debugstr;
        qrsResult->Serialize(debugstr);
        AGG_LOG(DEBUG, "AIGC Serialize=%s", debugstr.c_str());

        size_t hitCount = qrsResult->GetHitCount();
        for (size_t i = 0; i < hitCount; ++i) {
            QrsHit* qrsHit = qrsResult->GetHit(i);
            if (NULL == qrsHit)
                continue;

            string title = QrsHitHelper::GetStringAttr(qrsHit, "Title", "");
            string body = QrsHitHelper::GetStringAttr(qrsHit, "V_MAIN_BODY", "");
            string url = QrsHitHelper::GetStringAttr(qrsHit, "NormalizedUrl", "");
            // 详细打分特征 https://yuque.antfin-inc.com/chatun.wjj/zp0i8p/lgyws8?，可以根据需要选择获取
            double acnn= 0;
            double qtcnn = 0;
            double qtnn = 0;
            std::vector<double> ltrBrainFeatureList;
            qrsHit->GetDoubleVariable("BRAIN_FEATURE_LIST", ltrBrainFeatureList);
            if (ltrBrainFeatureList.size()>=6) {
                acnn = ltrBrainFeatureList[1];
                qtcnn = ltrBrainFeatureList[3];
                qtnn = ltrBrainFeatureList[4];
            }

            //string publishTime = std::to_string(GetPublishTime(qrsHit));
            string publishTime = GetFormatTime(GetPublishTime(qrsHit));
            string scName;
            string HostName = QrsHitHelper::GetStringAttr(qrsHit, "HostName", "");
            string HostLogo = QrsHitHelper::GetStringAttr(qrsHit, "HostLogo", "");
            hitQueueIndex.emplace_back(new LLMDoc(title, body, url, i, qtcnn, qtnn, 0.0,
                        publishTime, false, HostName, HostLogo));

            hitQueueIndex[hitQueueIndex.size() -1]->mQrsHit = qrsHit;
            hitQueueIndex[hitQueueIndex.size() -1]->mRecall = recall_name;
            
            // AGG_LOG(ERROR, "ADD Index[%s][%s]", url.c_str(), title.c_str());
            //AGG_LOG(DEBUG, "AIGC i=%lu, debug=%s", i, hitQueueAigc.back()->GetDebugString().c_str());
        }
        
        //totalCount += qrsResult->GetHitCount();
        //if (qrsResult->GetHitCount() == 0) {
        //    IncreaseEmptyQps();
        //} else {
        //    IncreaseUnEmptyQps();
        //}
        
        //AggSubContext * aggSubContext = qrsResponse->mAggSubContext;
        qrsResult->SetSubContext(aggSubContext);
        aggSubContext->AddQrsResult(qrsResult);
        //aggSubContext->DeleteSharedObject(responseId);
    }
}

void RecallIndexProcessor::FillNode2QrsHitVec(std::vector<SubContextInfo>& infoVec)
{
    size_t infoCount = infoVec.size();
    for (size_t i = 0; i < infoCount; i++) {
        SubContextInfo& info = infoVec[i];
        size_t hitCount = info._attributeHitVec.size();
        for (size_t j = 0; j < hitCount; j++) {
            QrsHit* qrsHit = info._attributeHitVec[j];

            const std::string & clusterName = qrsHit->GetClusterName();
            QrsResult* qrsResult = qrsHit->GetQrsResult();
            if (qrsResult == NULL) {
                AGG_LOG(DEBUG, "get qrs result failed, ignore this doc, clusterName = %s", clusterName.c_str());
                continue;
            }

            if (qrsResult->GetSubContext() == NULL) {
                qrsResult->SetSubContext(info._subContext);
            }

            const std::string& phase1Node = qrsResult->GetIndexName();
            NodeParamMapConstIter iter = mNodeParamMap.find(phase1Node);

            if (iter == mNodeParamMap.end()) {
                AGG_LOG(ERROR, "no param info for node [%s]", phase1Node.c_str());
                continue;
            }

            const std::string & summaryNodeName = iter->second._nodeName;
            info._node2QrsHitVec[summaryNodeName].push_back(qrsHit);
        }
    }
}

void RecallIndexProcessor::SendRequest(SubContextInfo& info)
{
    AggContext* aggContext = info._subContext->GetContext();
    const std::string& query = aggContext->GetAggRequest()->_query;

    for (Node2QrsHitVecConstIter iter = info._node2QrsHitVec.begin();
         iter != info._node2QrsHitVec.end(); ++iter) {
        const std::string& nodeName = iter->first;
        const std::vector<QrsHit*>& qrsHitVec = iter->second;

        AGG_LOG(DEBUG, "nodeName = %s, qrsHitVec.size() = %d", nodeName.c_str(), (int)(qrsHitVec.size()));
        
        auto summaryNodeOptionParamIter = mSummaryNodeOptionParamMap.find(nodeName);
        if (summaryNodeOptionParamIter == mSummaryNodeOptionParamMap.end()) {
            AGG_LOG(ERROR, "get summary node:%s param failed", nodeName.c_str());
            continue;
        }
        const SummaryNodeOptionParam & summaryNodeOptionParam = summaryNodeOptionParamIter->second; 

        String2String kvPair;
        
        string bodyLen = "300";
        kvPair["body_summary_conf"] = "2`1" + bodyLen + "`1<em>`1</em>`1...";
        
        map<std::string, std::string> nullReplace;// todo
        AggSummaryWrapperPtr wrapper = Create(
                                    summaryNodeOptionParam._type, info._subContext, nodeName,
                                    summaryNodeOptionParam._summaryNodeOption, kvPair, 
                                    nullReplace);
        if (!wrapper) {
            AGG_LOG(ERROR, "get summary wrapper failed [%s]", query.c_str());
            continue;
        }
        SummaryResponsePtr response = wrapper->AsyncRequest(qrsHitVec);
        if (!response) {
            AGG_LOG(ERROR, "request summary failed [%s]:node[%s]",
                    query.c_str(), nodeName.c_str());
            continue;
        }
        info._node2Response[nodeName] = response;
    }
}

AggSummaryWrapperPtr RecallIndexProcessor::Create(
        SummaryType summaryType,
        AggSubContext * aggSubContext,
        const std::string & server,
        const SummaryNodeOption & summaryNodeOption,
        const String2String & kvPairs,
        const std::map<std::string, std::string> & clusterReplace)
{
    AGG_LOG(DEBUG, "summaryType:%d", int(summaryType));
    switch (summaryType) {
        case ST_QRS:
            return AggSummaryWrapperFactory::Create(ST_QRS, aggSubContext, server, summaryNodeOption, kvPairs);
        case ST_S3:
            //return AggSummaryWrapperFactory::Create(ST_S3, aggSubContext, server, summaryNodeOption, kvPairs);
            return std::make_shared<DumpS3SummaryWrapper>(aggSubContext, server,
                    summaryNodeOption, kvPairs, clusterReplace);
        default:
            AGG_LOG(ERROR, "unknown summary type:%d", int(summaryType));
            return {};
    }
}

void RecallIndexProcessor::GetResponse(SubContextInfo& info)
{
    AggContext* aggContext = info._subContext->GetContext();
    AggSubContext * aggSubContext = info._subContext;
    const std::string& query = aggContext->GetAggRequest()->_query;
    for (Node2SummaryResponseConstIter iter = info._node2Response.begin();
            iter != info._node2Response.end(); ++iter) {
        const std::string & nodeName = iter->first;
        SummaryResponsePtr response = iter->second;
        SummaryResultPtr summaryResult = response->GetResult();
        if (!summaryResult) {
            AGG_LOG(WARN, "wait failed [%s]", query.c_str());
            continue;
        }
        const std::vector<Hit*> & summaryHitVec = summaryResult->GetSummaryHitVec();
        const std::vector<QrsHit*>& attrHitVec = info._node2QrsHitVec[nodeName];
        size_t hitCount = summaryHitVec.size();
        AGG_LOG(ERROR, "nodeName = %s, summary hit count[%d], attr hit count[%d]", 
                nodeName.c_str(), (int)hitCount, (int)attrHitVec.size());

        if (attrHitVec.size() != summaryHitVec.size()) {
            continue;
        }

        bool summaryError = false;
        for (size_t j = 0; j < hitCount; j++) {
            QrsHit* attrHit = attrHitVec[j];
            Hit* summaryHit = summaryHitVec[j];

            if (summaryHit == NULL) {
                AGG_LOG(ERROR, "no summary hit found [%s]", query.c_str());
                summaryError = true;
                continue;
            }
            
            AGG_LOG(DEBUG, "++++++++++++++++++++++++++");
            for (String2StringConstIter cntIter = summaryHit->ContentBegin();
                    cntIter != summaryHit->ContentEnd(); ++cntIter) {
                AGG_LOG(DEBUG, "Set Summary[%s][%s]", cntIter->first.c_str(), cntIter->second.c_str());
                attrHit->SetContent(cntIter->first, cntIter->second);
            }
        }
    }
}

BaseProcessor * RecallIndexProcessor::DoClone()
{
    return new RecallIndexProcessor(*this);
}

void RecallIndexProcessor::DoDestroy()
{
    delete this;
}

bool RecallIndexProcessor::RegisterMethods()
{
    return true;
}

bool RecallIndexProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);

