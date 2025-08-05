#include <agg_web2/biz/qlog/sgs_rerank_processor.h>
#include "agg_web2/common/common.h"
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <jsoncpp/json/json.h>
#include <agg_web2/data/value_data.h>
#include <agg_web2/upstream/qt/qt_request.h>
#include <agg_web2/upstream/qt/qt_result.h>
#include <agg2/wrapper/agg_http_wrapper.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, SgsRerankProcessor);

BEGIN_DEFINE_SERIALIZATION_FUNC(SgsRerankProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(SgsRerankProcessor)

SgsRerankProcessor::SgsRerankProcessor()
{
}

SgsRerankProcessor::~SgsRerankProcessor()
{
}

bool SgsRerankProcessor::DoInit(const String2String & params)
{
    return true;
}

void SgsRerankProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

int SgsRerankProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    
    // 获取Qrs结果
    std::vector<QrsHit*>qrsHitVec;
    GetQrsResult(aggContext, qrsHitVec);
    
    std::string rawQuery;
    mAggContext->GetRequestParam("query", rawQuery);

    std::string user_prov;
    mAggContext->GetRequestParam("province", user_prov);
    
    //std::string mode;
    //mAggContext->GetRequestParam("mode", mode);
    
    std::vector<RankNode> rankVec;
    std::vector<string> titles;
    std::set<string> titleUniq;
    for (auto& qrsHit : qrsHitVec) {
        string title, raw_data, islbs, province;
        qrsHit->GetContent("title", title);
        title = StringTool::ReplaceStr(title, "\n", "");
        qrsHit->GetContent("raw_data", raw_data);
        qrsHit->GetContent("isLBS", islbs);
        qrsHit->GetContent("province", province);

        if (islbs == "1") { // 带有LBS信息的
            if (province.empty() || province != user_prov)
                continue;
        }

        if (!title.empty() && !raw_data.empty()) {
            if (titleUniq.find(title) != titleUniq.end()) {// 重复的数据，取索引里面的第一条
                continue;
            }

            RankNode rankNode;
            rankNode.mQuery = title;
            rankNode.mRawData = raw_data;
            rankNode.mIsLBS = islbs;
            rankNode.mProvince = province;
            rankNode.mQt = 0.0;
            titles.push_back(title);
            rankVec.push_back(rankNode);

            titleUniq.insert(title);
        }
    }

    vector<double> qts = GetQT(aggContext, rawQuery, titles);

    AGG_LOG(DEBUG, "titles:%d, qts:%d", (int32_t)rankVec.size(), (int32_t)qts.size());

    if (rankVec.size() == qts.size()) {
        for (size_t i = 0; i < rankVec.size(); ++i) {
            rankVec[i].mQt = qts[i];
        }
    }
    
    sort(rankVec.begin(), rankVec.end(), [](RankNode& x, RankNode& y) {
        return x.mQt > y.mQt;
    });

    RequestQrsFormatResult(rankVec, 5);

    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "ReRank Cost:%d", end_time - start_time);

    return PR_SUCCESS;
}

std::vector<double> SgsRerankProcessor::GetQT(AggContext* aggContext, const std::string& query,
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

void SgsRerankProcessor::GetQrsResult(AggContext * aggContext, std::vector<QrsHit*>& qrsHitVec)
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

void SgsRerankProcessor::RequestQrsFormatResult(std::vector<RankNode>& rankVec, uint32_t hit)
{
    float qt_limit = 3.0f;
    std::string qtScoreStr;
    mAggContext->GetRequestParam("qt_score", qtScoreStr);
    if (!qtScoreStr.empty()) {
        qt_limit = autil::StringUtil::strToFloatWithDefault(qtScoreStr.c_str(), 3.0f);
    }
    if (qt_limit < 2.6) {
        qt_limit = 2.6;
    }
    
    std::string mode;
    mAggContext->GetRequestParam("mode", mode);
    if (mode == "limit") {
        qt_limit = 2.8f;
    }

    Json::Value root;
    Json::Value search_doc = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < rankVec.size() && i < hit; ++i) {
        if (rankVec[i].mQt < qt_limit)
            continue;

        search_doc.append(Json::Value());
        Json::Value & item = search_doc[search_doc.size() - 1];
        item["title"] = rankVec[i].mQuery; 
        item["province"] = rankVec[i].mProvince; 
        item["isLBS"] = rankVec[i].mIsLBS; 
        item["qt"] = rankVec[i].mQt;
        item["qt_limit"] = qt_limit;
        item["raw_data"] = rankVec[i].mRawData; 
    }
    root["search_doc"] = search_doc;
    
    std::string rawQuery;
    mAggContext->GetRequestParam("query", rawQuery);
    if (!rawQuery.empty())
        rawQuery = StringTool::ReplaceStr(rawQuery, "\n", "");

    root["query"] = rawQuery;
    
    Json::FastWriter writer;
    std::string resContent = writer.write(root);

    AGG_LOG(INFO, "resContent=%s", resContent.c_str());

    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

BaseProcessor * SgsRerankProcessor::DoClone()
{
    return new SgsRerankProcessor(*this);
}

void SgsRerankProcessor::DoDestroy()
{
    delete this;
}

bool SgsRerankProcessor::RegisterMethods()
{
    return true;
}

bool SgsRerankProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);

