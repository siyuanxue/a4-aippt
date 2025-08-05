#ifndef AGG_WEB2_BIZ_RECALL_INDEX_PROCESSOR_H
#define AGG_WEB2_BIZ_RECALL_INDEX_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>
#include <A4/util/string_tokenizer.h>
#include <agg2/common/qrs/qrs_response.h>
#include <agg_web2/common/common.h>
#include <agg2/wrapper/agg_summary_wrapper.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class RecallIndexProcessor : public BaseProcessor
{
public:
    RecallIndexProcessor();
    virtual ~RecallIndexProcessor();
    
public:
    virtual bool DoInit(const String2String & params) override;
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    virtual BaseProcessor * DoClone() override;
    virtual void DoDestroy() override;
    virtual int DoProcess(AggContext * aggContext) override;

public:
    // register UDF MethodFunc
    virtual bool RegisterMethods() override;

public:
    // register UDF actor
    virtual bool RegisterActors() override;
    
private:

    std::string GetFormatTime(int Currenttime);
    string GetCollegeAuthorityData(const std::string& name, std::string& college);

    // 生成Sub Context
    bool GenSubContextByQrsClause(const std::string& key, const std::string& subContextName);
    bool RegisterSubContext(std::vector<QrsClauseMap> & qrsClauseMapVec,
            AggSubContext * & subContext, const std::string& subContextName);
    void ReleaseQrsClauseMapVec(std::vector<QrsClauseMap>& qrsClauseMapVec);

    // 发送请求
    bool SendQrsRequestByTag(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec, const std::string& indexNames,
        int queryId, const std::string& category, int hitNum, const std::string& name);
    bool SendSubContextQrsRequestByTag(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec,
        AggSubContext* aggSubContext, const std::vector<std::string>& indexNameSet,
        int queryId, const std::string& category, int hitNum, const std::string& name);
    
    bool SendQrsRequestByHost(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec, const std::string& indexNames,
        int queryId, const std::string& category, int hitNum, const std::string& name);
    bool SendSubContextQrsRequestByHost(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec,
        AggSubContext* aggSubContext, const std::vector<std::string>& indexNameSet,
        int queryId, const std::string& category, int hitNum, const std::string& name);
    
    bool SendQrsRequestByUrls(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec, const std::string& indexNames,
        int queryId, const std::string& category, int hitNum, const std::string& name);
    bool SendSubContextQrsRequestByUrls(std::vector<QrsResponsePtr>& qrsResponseVec, 
        std::vector<std::string>& nameVec, std::vector<AggSubContext*>& subContextVec,
        AggSubContext* aggSubContext, const std::vector<std::string>& indexNameSet,
        int queryId, const std::string& category, int hitNum, const std::string& name);

    // 获取QRS结果
    void GetQrsResult(std::vector<QrsResponsePtr>& qrsResponseVec, std::vector<std::string>& nameVec, 
                      std::vector<AggSubContext*>& subContextVec, vector<LLMDocPtr>& hitQueueIndex);

    // Fetch Summary
public:
    /**
     * Node2QrsHitVec: 需要从Node获取Summary的QrsHit列表
     */
    typedef std::map<std::string, std::vector<QrsHit*> > Node2QrsHitVec;
    typedef Node2QrsHitVec::const_iterator Node2QrsHitVecConstIter;
    typedef Node2QrsHitVec::iterator Node2QrsHitVecIter;

    /**
     * Node2SummaryResponse: Summary Node的Response
     */
    typedef std::map<std::string, SummaryResponsePtr> Node2SummaryResponse;
    typedef Node2SummaryResponse::const_iterator Node2SummaryResponseConstIter;
    typedef Node2SummaryResponse::iterator Node2SummaryResponseIter;

    struct SubContextInfo
    {
        SubContextInfo()
            : _subContext(NULL)
        {}
        
        AggSubContext* _subContext;
        std::vector<QrsHit*> _attributeHitVec;
        Node2QrsHitVec _node2QrsHitVec;
        Node2SummaryResponse _node2Response;
    };

    struct NodeParam
    {
        NodeParam()
            : _type(ST_UNKNOWN)
        {}

        SummaryType _type;     // Summary Node的类型: QRS or S3
        std::string _nodeName;
    };
    
    struct SummaryNodeOptionParam
    {
        SummaryNodeOptionParam()
            : _type(ST_UNKNOWN)
        {}

        SummaryType _type;
        SummaryNodeOption _summaryNodeOption;
    };

    /**
     * NodeParamMap: key indexName, value SummaryNodeParam
     */
    typedef std::map<std::string, NodeParam> NodeParamMap;
    typedef NodeParamMap::const_iterator NodeParamMapConstIter;
    typedef NodeParamMap::iterator NodeParamMapIter;
    
    /**
     * SummaryNodeOptionMap: key summaryNodeName, value summaryNodeOptionParam
     */
    typedef std::map<std::string, SummaryNodeOptionParam> SummaryNodeOptionParamMap;
    typedef SummaryNodeOptionParamMap::const_iterator SummaryNodeOptionParamMapConstIter;
    typedef SummaryNodeOptionParamMap::iterator SummaryNodeOptionParamMapIter;

private:
    AggSummaryWrapperPtr Create(SummaryType summaryType,
                                AggSubContext * aggSubContext,
                                const std::string & server,
                                const SummaryNodeOption & summaryNodeOption,
                                const String2String & kvPairs,
                                const std::map<std::string, std::string> & clusterReplace);

    bool ParseRule(const std::string& rule);
    bool ParseNodeParam(const std::string & nodeRule,
                        std::string & phase1Node,
                        NodeParam & nodeParam,
                        SummaryNodeOption & summaryNodeOption) const;

    void FillNode2QrsHitVec(std::vector<SubContextInfo>& infoVec);
    
    void SendRequest(SubContextInfo& subContextInfo);
    void GetResponse(SubContextInfo& subContextInfo);

private:
    //std::vector<std::vector<std::string> > mQpRuleItemVec; 
    //LLMDebugInfoPtr mDebugInfoPtr;
    
    int mClusterIdx = 0;
    std::vector<QpParamElem> mQpParamElemVec;
    //std::string mChannels;
    std::string mAttributeParam;
    
    NodeParamMap mNodeParamMap;
    SummaryNodeOptionParamMap mSummaryNodeOptionParamMap;

private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(RecallIndexProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_RECALL_INDEX_PROCESSOR_H
