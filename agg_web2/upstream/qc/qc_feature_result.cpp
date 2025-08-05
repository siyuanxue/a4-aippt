#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg_web2/upstream/qc/qc_feature_result.h>
#include <agg2/helper/json_helper.h>
#include <jsoncpp/json/json.h>
#include <messer_utils/json/rapidjson/document.h>
#include <messer_utils/json/rapidjson/writer.h>
#include <agg_web2/util/LatencyCounter.h>
#include <agg_web2/proto/tf_response.pb.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, QcFeatureRequest);
AGG_LOG_SETUP(agg_web2, QcFeatureResult);

QcFeatureRequest::QcFeatureRequest(AggContext * aggContext)
    : mAggContext(aggContext)
{
    SetHttpMethod(HM_POST);
}

QcFeatureRequest::~QcFeatureRequest()
{
}

bool QcFeatureRequest::InitRequestStr(const std::string& requestStr)
{
    mRequestStr = requestStr;
    return true;
}

bool QcFeatureRequest::DoEncode(std::string & args,
                                std::string & body,
                                String2String & headers)
{
    body = mRequestStr;
    // LoadBalance参数
    if (mAggContext != NULL) {
        AddParam(LB_HASH_KEY, mAggContext->GetAggRequest()->_query);
    }
    return true;
}

// result

QcFeatureResult::QcFeatureResult()
{
    mAggContext = NULL;
}

QcFeatureResult::QcFeatureResult(AggContext * aggContext) : mAggContext(aggContext) 
{
}

bool QcFeatureResult::Serialize(std::string & bytes)
{
    //TODO:next 待梳理
    return true;
}


bool QcFeatureResult::Deserialize(const std::string & str)
{
    //LatencyCounter counter("QcFeatureResult::Deserialize");
    // AGG_LOG(DEBUG, "mAggContext:%p", mAggContext);
   
    A4::AGG::proto::qtc_result::TfResponse pbResult;
    if(pbResult.ParseFromString(str)) {
        AGG_LOG(DEBUG, "QcFeatureResult::Deserialize protobuf");
        int tensor_size = pbResult.tensors_size();
        for(int i = 0; i < tensor_size; ++i) {
            const A4::AGG::proto::qtc_result::TensorPair& tensor_pair = pbResult.tensors(i);
            const string& name = tensor_pair.name();
            AGG_LOG(DEBUG, "QcFeatureResult::Deserialize tensor name [%s]", name.c_str());
            const A4::AGG::proto::qtc_result::TensorProto& tensor_proto = tensor_pair.tensor(); 
            if(!tensor_proto.has_tensor_shape() || !tensor_proto.has_dtype() || \
               tensor_proto.dtype() != A4::AGG::proto::qtc_result::DataType::DT_FLOAT) {
                AGG_LOG(DEBUG, "QcFeatureResult::Deserialize tensor no shape or datatype not float");
                return false;
            }
            const A4::AGG::proto::qtc_result::TensorShapeProto& tensor_shape = tensor_proto.tensor_shape();
            int shapeSize = tensor_shape.dim_size();
            if(shapeSize != 2) {
                AGG_LOG(DEBUG, "QcFeatureResult::Deserialize shapeSize[%d] != 2", shapeSize);
                return false;
            }
            int batch_size = tensor_shape.dim(0).size();
            int dim = tensor_shape.dim(1).size();
            AGG_LOG(DEBUG, "QcFeatureResult::Deserialize batchsize[%d] dim[%d]", batch_size, dim);
            if(batch_size * dim != tensor_proto.float_val_size()) {
                AGG_LOG(DEBUG, "QcFeatureResult::Deserialize float_val_size[%] != [%d]", \
                  tensor_proto.float_val_size(), batch_size * dim);
                return false;
            }
            if(batch_size * dim != tensor_proto.float_val().size()) {
                AGG_LOG(DEBUG, "QcFeatureResult::Deserialize tensor_proto float_val size[%d] != [%d]", \
                  tensor_proto.float_val().size(), batch_size * dim);
                return false;
            }
            if(mQcFeature.empty()) {
                mQcFeature.resize(batch_size);
            }
            if(static_cast<int>(mQcFeature.size()) != batch_size) {
                AGG_LOG(DEBUG, "QcFeatureResult::Deserialize mQcFeature[%d] != batch_size[%d]", \
                  mQcFeature.size(), batch_size);
                return false;
            }
            for(int i = 0; i < batch_size; ++i) {
                vector<float>& feature = mQcFeature[i];
                size_t ori_size = feature.size();
                feature.resize(ori_size + dim);
                void* dst = reinterpret_cast<void*>(feature.data() + ori_size);
                const void* src = reinterpret_cast<const void*>(tensor_proto.float_val().data() + i * dim);
                memcpy(dst, src, sizeof(float) * dim);
            }
        }
        if(mQcFeature.empty()) {
            AGG_LOG(DEBUG, "QcFeatureResult::Deserialize mQcFeature is empty");
            return false;
        }
        AGG_LOG(DEBUG, "QcFeatureResult::Deserialize mQcFeature[%d][%d]", mQcFeature.size(), mQcFeature[0].size());
        return true;
    }
    AGG_LOG(DEBUG, "QcFeatureResult::Deserialize json");
    MESSER_UTILS_NS(json)::rapidjson::Document document;
    document.Parse(str.c_str()); 
    if (document.HasParseError()) {
        AGG_LOG(ERROR, "Json parse error, in=[%s]", str.c_str());
        return false;
    }
    if (!document.IsObject()) {
        AGG_LOG(ERROR, "Json not object, in=[%s]", str.c_str());
        return false;
    }

    const auto& conJson = document["scores"]["content"];
    const auto& conIntentJson = document["wenda_intent_prob"]["content"];
    const auto& conPr12LJson = document["answerable_prob"]["content"];
    if (conJson.IsArray()) {
        InitPR(conJson);
    } else if (conIntentJson.IsArray()) {
        InitPR(conIntentJson);
    } else if (conPr12LJson.IsArray()) {
        InitPR(conPr12LJson);
    } else {
        InitQTC(document);
    }
    return true;
}

void QcFeatureResult::InitPR(const MESSER_UTILS_NS(json)::rapidjson::Value& conJson)
{
    // 解析passage ranking
    mQcFeature.clear();
    std::vector<float> feature;
    for (size_t i = 0; i < conJson.Size(); i++) {
        if (!conJson[i].IsDouble()) {
            AGG_LOG(ERROR, "Content Json Index[%d] is not double.", i);
            return;
        }
        feature.push_back(conJson[i].GetDouble());
    }
    mQcFeature.push_back(feature);
}

void QcFeatureResult::InitQTC(const MESSER_UTILS_NS(json)::rapidjson::Document& document)
{
    const auto& qtProbJson = document["prob"]["content"];
    const auto& qtShapeJson = document["prob"]["shape"];
    if(qtProbJson.IsArray() && qtShapeJson.IsArray()
            && qtShapeJson.Size()==2
            && (qtShapeJson[1]==5 || qtShapeJson[1]==6)) {

        AGG_LOG(DEBUG, "Processing qtc scores");
        mQcFeature.clear();
        std::vector<float> feature;
        for (size_t i = 0; i < qtProbJson.Size(); i++) {
            //qtc只取第一个
            if (i % qtShapeJson[1].GetInt() != 0) {
                continue;
            }
            if (!qtProbJson[i].IsDouble()) {
                AGG_LOG(ERROR, "Content Json Index[%d] is not double.", i);
                return;
            }
            AGG_LOG(DEBUG, "qtcsoces in pr_result_collector is [%f]",
                    qtProbJson[i].GetDouble());
            feature.push_back(qtProbJson[i].GetDouble());
        }
        mQcFeature.push_back(feature);

        mQtFeature.clear();
        std::vector<float> qtFeature;
        for (size_t i = 0; i < qtProbJson.Size(); i++) {
            //取qt分，index=1 (merge,  qt,  qc,  mixqt,  mixqc,   ts_merge)
            if (i % qtShapeJson[1].GetInt() != 1) {
                continue;
            }
            if (!qtProbJson[i].IsDouble()) {
                AGG_LOG(ERROR, "Content Json Index[%d] is not double.", i);
                return;
            }
            AGG_LOG(DEBUG, "qtsoces in pr_result_collector is [%f]",
                    qtProbJson[i].GetDouble());
            qtFeature.push_back(qtProbJson[i].GetDouble());
        }
        mQtFeature.push_back(qtFeature);
    }
}

QcFeatureResult::~QcFeatureResult() 
{ 
}

AGG_END_NAMESPACE(agg_web2);

