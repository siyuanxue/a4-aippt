#include <agg_web2/biz/mount/mount_trigger_processor.h>
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

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, MountTriggerProcessor);

BEGIN_DEFINE_SERIALIZATION_FUNC(MountTriggerProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(MountTriggerProcessor)

MountTriggerProcessor::MountTriggerProcessor()
{
}

MountTriggerProcessor::~MountTriggerProcessor()
{
}

bool MountTriggerProcessor::DoInit(const String2String & params)
{
    return true;
}

void MountTriggerProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

int MountTriggerProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    int64_t end_time;

    std::string rawQuery;
    mAggContext->GetRequestParam("query", rawQuery);

    std::string message;
    if (!rawQuery.empty()) {
        rawQuery = StringTool::ReplaceStr(rawQuery, "\n", "");
    } else {
        message = "Request query is empty.";
        FormatEmptyResult(message);
        AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
        return PR_SUCCESS;
    }

    // 读取张雪峰观点绝影配置
    if (!GetZxfViewJyData()) {
        message = "Load zxf_view data failed.";
        FormatEmptyResult(message);
        AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
        return PR_SUCCESS;
    }

    // 绝影种子query匹配
    std::string seedView;
    double seedQt = 4.0;
    if (MatchSeedQuery(rawQuery, seedView)) {
        message = "Successful request - match seed query.";
        FormatValidResult(rawQuery, rawQuery, seedView, seedQt, message);
        AGG_LOG(INFO, "Has success result - match seed query.");
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
        return PR_SUCCESS;
    }

    // 读取高考意图一级信号
    std::vector<std::string> intents = GetQueryGaokaoIntentForMount(rawQuery);
    if (intents.size() == 0) { // 访问失败，重新访问一次，重试2次
        intents = GetQueryGaokaoIntentForMount(rawQuery);
        if (intents.size() == 0) {
            intents = GetQueryGaokaoIntentForMount(rawQuery);
        }
    }

    bool needTrigger = false;
    if (intents.size() == 0) { //TODO：空可能是获取失败
        needTrigger = false;
    } else {
        std::string intentStr = autil::StringUtil::toString(intents, ",");
        if (intentStr.find("专业") != string::npos) {
            needTrigger = true;
        }
    }
    AGG_LOG(INFO, "Intent:[%s][%s], needTrigger:[%d].", rawQuery.c_str(), autil::StringUtil::toString(intents, ",").c_str(), needTrigger);

    if (!needTrigger) {
        message = "Gaokao intent filtered.";
        FormatEmptyResult(message);
        AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
        return PR_SUCCESS;
    }

    // 专业名称硬匹配过滤
    if (!MatchMajorNameWithQuery(rawQuery)) {
        message = "Query hard match with major name failed.";
        FormatEmptyResult(message);
        AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
        return PR_SUCCESS;
    }

    // 请求QT服务
    vector<double> qts = GetQtForMount(aggContext, rawQuery, mMatchQueryVec);
    if (mZxfQueryViewRankVec.size() != qts.size()) {
        message = "QT service return size don't match.";
        FormatEmptyResult(message);
        AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
        return PR_SUCCESS;
    }
    if (mZxfQueryViewRankVec.size() <= 0) {
        message = "mZxfQueryViewRankVec is empty.";
        FormatEmptyResult(message);
        AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());
        end_time = autil::TimeUtility::currentTimeInMicroSeconds();
        AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
        return PR_SUCCESS;
    }
    for (size_t i = 0; i < mZxfQueryViewRankVec.size(); ++i) {
        mZxfQueryViewRankVec[i].mQt = qts[i];
    }
    sort(mZxfQueryViewRankVec.begin(), mZxfQueryViewRankVec.end(), [](ZxfRankNode& x, ZxfRankNode& y) {return x.mQt > y.mQt;});
    size_t limit_num = 1;
    for (size_t i = 0; i < mZxfQueryViewRankVec.size() && i < limit_num; ++i) {
        AGG_LOG(INFO, "RawQuery: [%s], MatchQuery: [%s], QT: [%f].", mZxfQueryViewRankVec[i].mQuery.c_str(), mZxfQueryViewRankVec[i].mMatchQuery.c_str(), mZxfQueryViewRankVec[i].mQt);
        // QT阈值
        if (mZxfQueryViewRankVec[i].mQt < 2.8) {
            message = "QT score lower than threshold.";
            FormatEmptyResult(message);
            AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());
            end_time = autil::TimeUtility::currentTimeInMicroSeconds();
            AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
            return PR_SUCCESS;
        } else {
            message = "Successful request.";
            FormatValidResult(mZxfQueryViewRankVec[i].mQuery, mZxfQueryViewRankVec[i].mMatchQuery, mZxfQueryViewRankVec[i].mMatchView, mZxfQueryViewRankVec[i].mQt, message);
            AGG_LOG(INFO, "Has success result.");
            end_time = autil::TimeUtility::currentTimeInMicroSeconds();
            AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
            return PR_SUCCESS;
        }
        break;
    }

    // 兜底 - 应该不会走到这里
    message = "final empty result.";
    FormatEmptyResult(message);
    AGG_LOG(ERROR, "[EmptyResult]: [%s]", message.c_str());

    end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(INFO, "ReRank Cost:%d", end_time - start_time);
    return PR_SUCCESS;
}

void MountTriggerProcessor::RequestFormatResult()
{
    Json::Value root;
    Json::Value search_doc = Json::Value(Json::arrayValue);

    search_doc.append(Json::Value());
    Json::Value & item = search_doc[search_doc.size() - 1];
    item["content"] = "关于机械类专业，张雪峰在多次直播中表达了自己的观点。 \n\
首先，机械类专业想要学好，需要高中物理力学基础好一些，同时机械的力学基础是高等数学和线性代数，也需要一定的数学功底。高中阶段物理的力学、数学中的几何和代数学得好，动手能力强的孩子，更适合学机械类专业。\n\
其次，机械类考研专业课共四门：理论力学，材料力学，然后机械原理，机械设计。\n\
再次，机械专业不同层次学校差别不大，985的学生和211的学生可能在一样的岗位工作。下限能保证工作和温饱，上限也不高，想要通过这个岗位发财，可能性也不大。根本原因是，这个领域的工作，企业的主要投资都在设备上，人只是大型设备的附属。 \n\
\n\
此外，张雪峰老师还在直播中给出过很多具体的建议：\n\
1. 判断一个学科是否是机械的相关专业，就看他的考研专业课科目，是不是也是这四门课。所以航空航天、汽车都是机械类的相关专业。\n\
2. 机械类专业的分数普遍低于电气类专业，如机械、土木、水利、航空航天等专业的分数通常低于电子信息、电气工程等专业。\n\
3. 如果分数不够航空航天专业，可以先选择普通机械类专业，以后考研时再申请航空航天方向的研究生。\n\
4. 在机械类专业中，建议将机械电子工程专业放在首位，车辆工程专业放在第二位，工业设计专业也还可以，然后将其他机械专业随意排列，最后将材料成型及控制工程和过程装备及控制工程专业放在最后。";

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

// 有效返回结果
void MountTriggerProcessor::FormatValidResult(const std::string rawQuery, const std::string matchQuery, const std::string matchView, const double qt_score, const std::string message)
{
    Json::Value root;
    Json::Value search_doc = Json::Value(Json::arrayValue);

    search_doc.append(Json::Value());
    Json::Value & item = search_doc[search_doc.size() - 1];
    item["content"] = matchView;
    item["matchQuery"] = matchQuery;
    item["score"] = qt_score;
    root["search_doc"] = search_doc;
    root["query"] = rawQuery;
    root["message"] = message;

    Json::FastWriter writer;
    std::string resContent = writer.write(root);

    AGG_LOG(INFO, "resContent=%s", resContent.c_str());
    mContext->SetResponsePacket(HttpPacketFactory::CreateResponse(200, resContent, String2String()));
}

// 空结果返回
void MountTriggerProcessor::FormatEmptyResult(const std::string message)
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

// 读取意图信号函数
std::vector<std::string> MountTriggerProcessor::GetQueryGaokaoIntentForMount(std::string& query)
{
    vector<string> intents;
    HttpRequestPtr httpRequest(new CommonRequest(query, "gaokao_intent"));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "gaokao_intent_service");
    HttpResponsePtr aggResponse = wrapper->AsyncRequest(httpRequest);
    if (aggResponse) {
        ResultPtr intentResult(new CommonResult("gaokao_intent"));
        if (aggResponse->GetResult(intentResult)) {
            CommonResultPtr intentPtr = dynamic_pointer_cast<CommonResult>(intentResult);
            intents = intentPtr->mIntents;
        }
    }
    return intents;
}

// 获取张雪峰观点绝影配置
bool MountTriggerProcessor::GetZxfViewJyData()
{
    AggDataWrapper data("zxf_view_data");
    auto zxfViewDataPtr = data.Get<KeyValueData>();
    if (zxfViewDataPtr) {
        mZxfViewKvMap = zxfViewDataPtr->GetkvData();
        AGG_LOG(INFO, "load zxf_view_data success.");
        return true;
    } else {
        AGG_LOG(ERROR, "load zxf_view_data failed.");
        return false;
    }
}

// 绝影种子query匹配
bool MountTriggerProcessor::MatchSeedQuery(const std::string query, std::string& seedView)
{
    bool match_flag = false;
    for (std::unordered_map<std::string, std::string>::iterator iter = mZxfViewKvMap.begin(); iter != mZxfViewKvMap.end(); ++iter) {
        string major_key = iter->first;
        vector<string> majorKeyVec;
        majorKeyVec = autil::StringUtil::split(major_key, "###");
        if (majorKeyVec.size() != 2) {
            AGG_LOG(ERROR, "major_key size not match [%s].", major_key.c_str());
            continue;
        }
        if (query == majorKeyVec[1]) {
            seedView = iter->second;
            match_flag = true;
            AGG_LOG(INFO, "match seed query.");
            break;
        }
    }
    return match_flag;
}

// 专业类别学科关键词匹配
bool MountTriggerProcessor::MatchMajorNameWithQuery(const std::string query)
{
    mMatchQueryVec.clear();
    for (std::unordered_map<std::string, std::string>::iterator iter = mZxfViewKvMap.begin(); iter != mZxfViewKvMap.end(); ++iter) {
        string major_key = iter->first;
        vector<string> majorKeyVec;
        majorKeyVec = autil::StringUtil::split(major_key, "###");
        if (majorKeyVec.size() != 2) {
            AGG_LOG(ERROR, "major_key size not match [%s].", major_key.c_str());
            continue;
        }
        if (query.find(majorKeyVec[0]) != string::npos) {
            mMatchQueryVec.push_back(majorKeyVec[1]);
            ZxfRankNode zxfRankNodeTmp;
            zxfRankNodeTmp.mQuery = query;
            zxfRankNodeTmp.mMatchQuery = majorKeyVec[1];
            zxfRankNodeTmp.mMatchView = iter->second;
            zxfRankNodeTmp.mQt = 0.0;
            mZxfQueryViewRankVec.push_back(zxfRankNodeTmp);
            AGG_LOG(INFO, "match major_name: [%s], major_query: [%s].", majorKeyVec[0].c_str(), majorKeyVec[1].c_str());
        }
    }

    if (mMatchQueryVec.size() > 0) {
        AGG_LOG(INFO, "Match query success. mMatchQueryVec size: [%d].", mMatchQueryVec.size());
        return true;
    } else {
        AGG_LOG(INFO, "Match query fail.");
        return false;
    }
}

// 请求QT服务
std::vector<double> MountTriggerProcessor::GetQtForMount(AggContext* aggContext, const std::string& query,
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

BaseProcessor * MountTriggerProcessor::DoClone()
{
    return new MountTriggerProcessor(*this);
}

void MountTriggerProcessor::DoDestroy()
{
    delete this;
}

bool MountTriggerProcessor::RegisterMethods()
{
    return true;
}

bool MountTriggerProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);

