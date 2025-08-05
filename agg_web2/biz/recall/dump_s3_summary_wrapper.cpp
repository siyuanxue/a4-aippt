#include <agg_web2/biz/recall/dump_s3_summary_wrapper.h>
//#include <agg_web2/helper/agg_web_qrs_hit_helper.h>
#include <agg2/helper/qrs_hit_helper.h>
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/common/http_response.h>
#include <agg2/common/qrs/qrs_clause.h>
#include <agg2/common/summary/s3_summary_response.h>
#include <A4/util/string_util.h>

const std::string HA3_QUERY_STRING = "ha3_querystring";
const std::string NORMALIZED_URL = "NormalizedUrl";
const std::string KEY_RAW_TYPE = "RAW_PK";


AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2,DumpS3SummaryWrapper);

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(DumpS3SummaryWrapper)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(DumpS3SummaryWrapper)

DumpS3SummaryWrapper::DumpS3SummaryWrapper() :
    AggSummaryWrapper()
{
}

DumpS3SummaryWrapper::DumpS3SummaryWrapper(AggSubContext * aggSubContext,
                                   const std::string & server,
                                   const SummaryNodeOption & summaryNodeOption,
                                   const String2String & kvPairs,
                                   const std::map<std::string, std::string> & clusterReplace) : 
    AggSummaryWrapper(aggSubContext, server, summaryNodeOption, kvPairs), 
    mClusterReplace(clusterReplace)
{
}

DumpS3SummaryWrapper::~DumpS3SummaryWrapper()
{
}

SummaryResponsePtr DumpS3SummaryWrapper::AsyncRequest(const std::vector<QrsHit*> & qrsHitVec)
{
    std::map<size_t, S3RequestPtr> requestMap;
    std::map<std::string, std::vector<size_t>> indexMap;
    if (!BuildRequestMap(qrsHitVec, requestMap, indexMap) || requestMap.empty()
            || indexMap.empty()) {
        AGG_LOG(ERROR, "build request map failed, request[%d], index[%d], hit[%d],"
                " requestQuery[%s]", int(requestMap.size()), int(indexMap.size()),
                int(qrsHitVec.size()), mRequestQuery.c_str());
        return {};
    }
    
    std::vector<S3SummaryResponse::ResponseItem> responseItemVec;
    AggHttpWrapper wrapper(mAggSubContext->GetContext(), mServer);
    for (auto iter = requestMap.begin(); iter != requestMap.end(); ++iter) {
        HttpResponsePtr httpResponse = wrapper.AsyncRequest(iter->second);
        if (!httpResponse) {
            AGG_LOG(ERROR, "request s3 summary failed");
            continue;
        }
        responseItemVec.emplace_back(
                S3SummaryResponse::ResponseItem(iter->first, httpResponse));
    }
    if (responseItemVec.empty()) {
        AGG_LOG(ERROR, "s3 summary responseItemVec is empty");
        return {};
    }
    S3SummaryResponsePtr response(
            new S3SummaryResponse(mAggSubContext, indexMap, responseItemVec));
    AGG_LOG(DEBUG, "s3 summary wrapper async request:%s success", mServer.c_str());
    return response;
}

bool DumpS3SummaryWrapper::BuildRequestMap(const std::vector<QrsHit*> & qrsHitVec,
                                       std::map<size_t, S3RequestPtr> & requestMap, 
                                       std::map<std::string, std::vector<size_t>> & indexMap) const
{
    bool suc = true;
    requestMap.clear();
    indexMap.clear();

    size_t count = qrsHitVec.size();
    for (size_t i = 0; i < count; i++) {
        QrsHit* qrsHit = qrsHitVec[i];
        QrsResult* qrsResult = qrsHit->GetQrsResult();
        if (qrsResult == NULL) {
            AGG_LOG(ERROR, "get qrs result failed [%s]", mRequestQuery.c_str());
            suc = false;
            break;
        }

        std::string hitKey = GetHitKey(qrsHit);
        if (hitKey.empty()) {
            AGG_LOG(ERROR, "get key failed [%s]", mRequestQuery.c_str());
            suc = false;
            break;
        }

        S3Request::KeyInfo keyInfo;
        if (!BuildKeyInfo(qrsHit, keyInfo)) {
            AGG_LOG(ERROR, "build key failed [%s]", mRequestQuery.c_str());
            suc = false;
            break;
        }

        S3RequestPtr request;
        size_t requestKey = (size_t)qrsResult->GetQrsClause();
        std::string flag = "";
        if (qrsHit->GetContent("sim_doc", flag) && flag == "1") {
            requestKey = requestKey + 1000;
        }
        std::map<size_t, S3RequestPtr>::iterator iter = requestMap.find(requestKey);
        if (iter != requestMap.end()) {
            request = iter->second;
            assert(request != NULL);
        } else {
            request = CreateBaseS3Request(qrsHit);
            if (request == NULL) {
                AGG_LOG(ERROR, "init request failed [%s]", mRequestQuery.c_str());
                suc = false;
                break;
            }
            requestMap[requestKey] = request;
        }

        MESSER_UTILS_NS(util)::ostringstream ss;
        ss << requestKey << "|" << hitKey;
        std::string qrsHitId = ss.str(); 
        indexMap[qrsHitId].push_back(i);   // 允许QrsHitVec中有重复的QrsHit
        if (indexMap[qrsHitId].size() > 1) {
            AGG_LOG(DEBUG, "duplicate QrsHit, hitKey:%s, pos:%d", hitKey.c_str(), (int)i);
        } else {
            request->AddKeyInfo(keyInfo);
        }
    }

    if (!suc) {
        requestMap.clear();
        indexMap.clear();
    }
    return suc;
}

std::string DumpS3SummaryWrapper::GetHitKey(QrsHit* qrsHit) const  
{
    if (qrsHit == NULL) {
        return "";
    }
    std::string docKey = GetDocKey(qrsHit);
    if (docKey.empty()) {
        return "";
    }

    MESSER_UTILS_NS(util)::ostringstream ss;
    ss << GetClusterName(qrsHit) << "|" << docKey;

    return ss.str();
}

std::string DumpS3SummaryWrapper::GetDocKey(QrsHit* qrsHit) const
{
    if (qrsHit == NULL) {
        return "";
    }
    std::string key = QrsHitHelper::GetOriNormalizedUrlAttr(qrsHit);;
    if (mSummaryNodeOption._keyType == KEY_RAW_TYPE) {
        if (key != "") {
            AGG_LOG(DEBUG, "get qrshit attr normalized_url[%s]", key.c_str());
            return key;
        } else {
            AGG_LOG(ERROR, "get qrshit attr normalized_url failed");
            return "";
        }
    }
    
    uint64_t pkHigher = qrsHit->GetPkHigher();
    uint64_t pkLower = qrsHit->GetPkLower();
    const size_t bufSize = 35;

    char pkBuf[bufSize] = {0};
    size_t len = 0;
    if (pkHigher != 0 && pkLower != 0) {
        len = snprintf(pkBuf, bufSize, "0x%lx%016lx", pkHigher, pkLower);
    } else if (pkLower != 0) {
        len = snprintf(pkBuf, bufSize, "0x%lx", pkLower);
    } else {
        AGG_LOG(ERROR, "bad pk [%lx] [%lx]", pkHigher, pkLower);
        return "";
    }
    
    if (len == 0 || len >= bufSize) {
        return "";
    }

    key.assign(pkBuf, len);
    return key;
}

bool DumpS3SummaryWrapper::BuildKeyInfo(QrsHit* qrsHit, S3Request::KeyInfo& keyInfo) const
{
    if (qrsHit == NULL) {
        return "";
    }
    std::string docKey = GetDocKey(qrsHit);
    if(docKey.empty()) {
        AGG_LOG(ERROR, "get key failed, original pk [0x%lx%016lx]",
                qrsHit->GetPkHigher(), qrsHit->GetPkLower());
        return false;
    }

    keyInfo._key = docKey;
    keyInfo._cluster = GetClusterName(qrsHit);

    if (mSummaryNodeOption._keepRangeId) {
        keyInfo._rangeId = qrsHit->GetHashId();
    }

    return true;
}

S3RequestPtr DumpS3SummaryWrapper::CreateBaseS3Request(QrsHit* qrsHit) const
{
    if (qrsHit == NULL) {
        return {};
    }
    QrsResult* qrsResult = qrsHit->GetQrsResult();
    if (qrsResult == NULL) {
        AGG_LOG(ERROR, "get qrs result failed [%s]", mRequestQuery.c_str());
        return {};
    }

    QrsClause* qrsClause = qrsResult->GetQrsClause();
    if (qrsClause == NULL) {
        AGG_LOG(ERROR, "get qrs clause failed [%s] [%s]",
                qrsResult->GetIndexName().c_str(), mRequestQuery.c_str());
        return {};
    }

    String2String kvPairMap;
    qrsClause->GetKvPairMap(kvPairMap);

    std::string query;
    qrsClause->GetQuery(query);
    if (!query.empty()) {
        kvPairMap[HA3_QUERY_STRING] = query;
    }
    for (auto & iter: mKvPair) {
        kvPairMap[iter.first] = iter.second;
    }

    S3RequestPtr request = std::make_shared<S3Request>();
    request->SetKvPair(kvPairMap);
    if (!mSummaryNodeOption._keyType.empty()) {
        request->SetKeyType(mSummaryNodeOption._keyType);
    }
    if (!mSummaryNodeOption._timeout.empty()) {
        int32_t timeout = StringUtil::StringToUint32(mSummaryNodeOption._timeout);
        if (timeout > 0) {
            request->SetTimeout(timeout);
        }
    }
    if (!mSummaryNodeOption._profile.empty()) {
        request->SetSummaryProfile(mSummaryNodeOption._profile);
    }

    // HashLoadBalance, 以RequestQuery为依据进行HashLoadBalance
    request->AddParam("hash", mRequestQuery);

    return request;
}

std::string DumpS3SummaryWrapper::GetClusterName(QrsHit * qrsHit) const
{
    std::string clusterName = qrsHit->GetClusterName();
    AGG_LOG(DEBUG, "mServer = %s", mServer.c_str());
    if (mServer.find("_pk") != std::string::npos || mServer == "feedindex_summary" || mServer == "feedindex_summary_bucket") {
        std::map<std::string, std::string>::const_iterator it = mClusterReplace.find(clusterName);
        if (it != mClusterReplace.end()) {
            clusterName = it->second;
        }
    }
    return clusterName;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(dump)::DumpS3SummaryWrapper);

