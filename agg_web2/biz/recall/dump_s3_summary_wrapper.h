#ifndef AGG_WEB_DUMP_DUMP_S3_SUMMARY_WRAPPER_H
#define AGG_WEB_DUMP_DUMP_S3_SUMMARY_WRAPPER_H

#include <agg_web2/common/common.h>
#include <agg2/common/s3/s3_request.h>
#include <agg2/wrapper/agg_summary_wrapper.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class DumpS3SummaryWrapper : public AggSummaryWrapper
{
public:
    DumpS3SummaryWrapper();
    DumpS3SummaryWrapper(AggSubContext * aggSubContext,
                     const std::string & server,
                     const SummaryNodeOption & summaryNodeOption,
                     const String2String & kvPair,
                     const std::map<std::string, std::string> & clusterReplace);
    virtual ~DumpS3SummaryWrapper();
public:
    virtual SummaryResponsePtr AsyncRequest(const std::vector<QrsHit*> & qrsHitVec) override;
    
private:
    /**
     * @param QrsHitVec 待获取Summary的QrsHit列表
     * @param out: requestMap
     * @param out: QrsHit key与pos的映射, 兼容支持QrsHit中存在重复QrsHit的情况
     */
    bool BuildRequestMap(const std::vector<QrsHit*> & qrsHitVec,
                         std::map<size_t, S3RequestPtr> & requestMap,
                         std::map<std::string, std::vector<size_t>> & indexMap) const;

    std::string GetHitKey(QrsHit * qrsHit) const;
    std::string GetDocKey(QrsHit * qrsHit) const;

    bool BuildKeyInfo(QrsHit * qrsHit, S3Request::KeyInfo & keyInfo) const;

    S3RequestPtr CreateBaseS3Request(QrsHit* qrsHit) const;
    
    void SetClusterReplace(const std::map<std::string, std::string> & clusterReplace) 
    {
        mClusterReplace = clusterReplace;
    }
    
    std::string GetClusterName(QrsHit * qrsHit) const;

private:
    std::map<std::string, std::string> mClusterReplace;
    
private:
    AGG_LOG_DECLARE();

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(DumpS3SummaryWrapper);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(dump)::DumpS3SummaryWrapper);

#endif //AGG_WEB_DUMP_DUMP_S3_SUMMARY_WRAPPER_H
