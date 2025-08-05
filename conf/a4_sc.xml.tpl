<A4>
    <main>
        <listen>0.0.0.0:${global_http_port}</listen>
        <net_io_thread_count>4</net_io_thread_count>
        <thread_count>256</thread_count>
        <task_queue_count>1000</task_queue_count>
        <amonitor_service_name>${kvParam_amonitor_service_name_on_ude}</amonitor_service_name>
        <allstar_server_spec>42.156.215.219:8080/setgraph</allstar_server_spec>
        <amonitor_agent_port>10086</amonitor_agent_port>
        <workdir>${A4_INSTALL_ROOT}/usr/local/var/</workdir>
        <control_cmd_location>/cmd</control_cmd_location>
        <control_task_queue_count>1</control_task_queue_count>
        <close_std_fd>off</close_std_fd>
    </main>

    <threadpools>
        <threadpool name="interleaving_thread_pool">
            <thread_count>${kvParam_thread_count_interleaving}</thread_count>
            <task_queue_count>1000</task_queue_count>
        </threadpool>
    </threadpools>

    <plugin>
        <modules dlclose="on">
            <module name="a4_ns_plugin" path="${A4_INSTALL_ROOT}/usr/local/lib64/liba4_ns_plugin.so">
                <params>
                    <zk_spec>${kvParam_cm_zk_spec}</zk_spec>
                    <zk_path>${kvParam_cm_zk_path}</zk_path>
                    <timeout>10</timeout>
                    <sub_type>st_part</sub_type>
                    <compress_type>ct_snappy</compress_type>
                    <sub_cluster_lbs>${kvParam_lbs}</sub_cluster_lbs>
                    <sub_cluster_shield>${kvParam_shield}</sub_cluster_shield>
                    <sub_cluster_multi_exp_bert>${kvParam_zeus_multi_exp_bert}</sub_cluster_multi_exp_bert>
                    <sub_cluster_multi_exp_bert_v2>${kvParam_zeus_multi_exp_bert_v2}</sub_cluster_multi_exp_bert_v2>
                    <sub_cluster_dct_imp>${kvParam_zeus_dct_imp}</sub_cluster_dct_imp>
                    <sub_cluster_clickG_predict>${kvParam_zeus_clickG_predict}</sub_cluster_clickG_predict>
                    <sub_cluster_clickG_predict_v2>${kvParam_zeus_clickG_predict_v2}</sub_cluster_clickG_predict_v2>
                    <sub_cluster_click_rerank>${kvParam_zeus_click_rerank}</sub_cluster_click_rerank>
                    <sub_cluster_query_rewrite>${kvParam_query_rewrite}</sub_cluster_query_rewrite>
                    <sub_cluster_qlog_rewrite_recall>${kvParam_zeus_qlog_rewrite}</sub_cluster_qlog_rewrite_recall>
                    <sub_cluster_qlog_rewrite_recall_v2>${kvParam_zeus_qlog_rewrite_v2}</sub_cluster_qlog_rewrite_recall_v2>
                    <sub_cluster_uads>${kvParam_uads}</sub_cluster_uads>
                    <sub_cluster_nest_click>${kvParam_ha3_nest_click}</sub_cluster_nest_click>
                    <sub_cluster_ha3_pv>${kvParam_ha3_pv}</sub_cluster_ha3_pv>
                    <sub_cluster_ha3_qlog>${kvParam_ha3_qlog}</sub_cluster_ha3_qlog>
                    <sub_cluster_ha3_qlog_v2>${kvParam_ha3_qlog_v2}</sub_cluster_ha3_qlog_v2>
                    <sub_cluster_ha3_sm_click_server_v3>${kvParam_ha3_sm_click_server_v3}</sub_cluster_ha3_sm_click_server_v3>
                    <sub_cluster_qp>${kvParam_qp}</sub_cluster_qp>
                    <sub_cluster_mainindex>${kvParam_ha3_mainindex}</sub_cluster_mainindex>
                    <sub_cluster_rtindex>${kvParam_ha3_webrt}</sub_cluster_rtindex>
                    <sub_cluster_qaindex>${kvParam_ha3_qa_mainindex}</sub_cluster_qaindex>
                    <sub_cluster_bigindex>${kvParam_ha3_bigindex}</sub_cluster_bigindex>
                    <sub_cluster_s3_mainindex>${kvParam_s3_mainindex}</sub_cluster_s3_mainindex>
                    <sub_cluster_s3_bigindex>${kvParam_s3_bigindex}</sub_cluster_s3_bigindex>
                    <sub_cluster_qa_agg>${kvParam_qa_agg}</sub_cluster_qa_agg>
                    <sub_cluster_qa_ss>${kvParam_qa_ss_node_ns}</sub_cluster_qa_ss>
                    <sub_cluster_quark_baike>${kvParam_quark_baike_node_ns}</sub_cluster_quark_baike>
                    <sub_cluster_edu_signal>${kvParam_edu_signal}</sub_cluster_edu_signal>
                    <sub_cluster_prior_edu_signal>${kvParam_prior_edu_signal}</sub_cluster_prior_edu_signal>
                    <sub_cluster_query_prior_intention>${kvParam_query_prior_intention}</sub_cluster_query_prior_intention>
                    <sub_cluster_query_poster_intention>${kvParam_query_poster_intention}</sub_cluster_query_poster_intention>
                    <sub_cluster_s3_kvindex>${kvParam_s3_kvindex}</sub_cluster_s3_kvindex>
                    <sub_cluster_query_ner_service>${kvParam_query_ner_service}</sub_cluster_query_ner_service>
                    <sub_cluster_sc_pos_model>${kvParam_sc_pos_model}</sub_cluster_sc_pos_model>
                    <sub_cluster_sc_relevance>${kvParam_sc_relevance}</sub_cluster_sc_relevance>
                    <sub_cluster_sc_aol_caculate>${kvParam_sc_aol_caculate}</sub_cluster_sc_aol_caculate>
                    <sub_cluster_medical_intention>${kvParam_medical_intention}</sub_cluster_medical_intention>
                    <sub_cluster_medical_vertical>${kvParam_medical_vertical}</sub_cluster_medical_vertical>
                    <sub_cluster_medical_sc_trigger>${kvParam_medical_sc_trigger}</sub_cluster_medical_sc_trigger>
                    <sub_cluster_arbiter>${kvParam_arbiter}</sub_cluster_arbiter>
                    <sub_cluster_ugc_video>${kvParam_ugc_video}</sub_cluster_ugc_video>
                    <!--<sub_cluster_recommend_web>$(recommend_web)</sub_cluster_recommend_web>-->
                    <sub_cluster_relative_search>${kvParam_relative_search}</sub_cluster_relative_search>
                    <sub_cluster_spl>${kvParam_zeus_agg_specific_logic}</sub_cluster_spl>
                    <sub_cluster_timeliness>${kvParam_timeliness}</sub_cluster_timeliness>
                    <sub_cluster_freshment_rerank>${kvParam_freshment_rerank}</sub_cluster_freshment_rerank>
                    <sub_cluster_ufs_agg_adjuster>${kvParam_ufs_agg_adjuster_model}</sub_cluster_ufs_agg_adjuster>
                    <sub_cluster_feed_replace>${kvParam_feed_replace}</sub_cluster_feed_replace>
                    <sub_cluster_feed_search_gpu_service>${kvParam_feed_search_gpu_service}</sub_cluster_feed_search_gpu_service>
                    <sub_cluster_feed_search_cpu_service>${kvParam_feed_search_cpu_service}</sub_cluster_feed_search_cpu_service>
                    <sub_cluster_relevance>${kvParam_relevance}</sub_cluster_relevance>
                    <sub_cluster_sifter>${kvParam_sifter}</sub_cluster_sifter>
                    <sub_cluster_summary_modify>${kvParam_summary_modify}</sub_cluster_summary_modify>
                    <sub_cluster_makeup>${kvParam_makeup}</sub_cluster_makeup>
                    <sub_cluster_config_server>${kvParam_config_server}</sub_cluster_config_server>
                    <sub_cluster_agg_model_ufs>${kvParam_zeus_ufs_app_agg}</sub_cluster_agg_model_ufs>
                    <sub_cluster_authority_model_ufs>${kvParam_ufs_agg_authority_model}</sub_cluster_authority_model_ufs>
                    <sub_cluster_content_model_ufs>${kvParam_zeus_ufs_app_content}</sub_cluster_content_model_ufs>
                    <sub_cluster_qc_v2_feature_bert>${kvParam_qc_v2_feature_bert}</sub_cluster_qc_v2_feature_bert>
                    <sub_cluster_qc_feature_bert>${kvParam_qc_feature_bert}</sub_cluster_qc_feature_bert>
                    <sub_cluster_qc_feature_bert_p100>${kvParam_qc_feature_bert_p100}</sub_cluster_qc_feature_bert_p100>
                    <sub_cluster_qtc_student_v2>${kvParam_qtc_student_v2}</sub_cluster_qtc_student_v2>
                    <sub_cluster_qc_feature_bert_p2080>${kvParam_qc_feature_bert_2080}</sub_cluster_qc_feature_bert_p2080>
                    <sub_cluster_sc_qtc_minibert>${kvParam_sc_qtc_minibert}</sub_cluster_sc_qtc_minibert>
                    <sub_cluster_brain_agg_qt_semantic>${kvParam_qt_semantic}</sub_cluster_brain_agg_qt_semantic>
                    <sub_cluster_agg_feature>${kvParam_zeus_agg_feature}</sub_cluster_agg_feature>
                    <sub_cluster_agg_feature_ufs>${kvParam_agg_feature_ufs}</sub_cluster_agg_feature_ufs>
                    <sub_cluster_ufs_spell_check_model>${kvParam_zeus_ufs_app_spell_check}</sub_cluster_ufs_spell_check_model>
                    <sub_cluster_qu>${kvParam_qu_node_ns}</sub_cluster_qu>
                    <sub_cluster_ufs_app_querylog>${kvParam_zeus_ufs_app_querylog}</sub_cluster_ufs_app_querylog>
                    <sub_cluster_cms>${kvParam_cms}</sub_cluster_cms>
                    <sub_cluster_hhm_model>${kvParam_zeus_hhm_model}</sub_cluster_hhm_model>
                    <sub_cluster_authority_nn_model>${kvParam_zeus_authority_nn_model}</sub_cluster_authority_nn_model>
                    <sub_cluster_acnn_feature_bert>${kvParam_zeus_acnn_feature_bert}</sub_cluster_acnn_feature_bert>
                    <sub_cluster_vip_index_router>${kvParam_zeus_semantic_index}</sub_cluster_vip_index_router>
                    <sub_cluster_semantic_zeus_service>${kvParam_semantic_zeus_service}</sub_cluster_semantic_zeus_service>
                    
                    <sub_cluster_ip_limit_0>multi_tab</sub_cluster_ip_limit_0>
                    <sub_cluster_ip_limit_1>sm-quark</sub_cluster_ip_limit_1>
                    <sub_cluster_ip_limit_2>a4_agg_union</sub_cluster_ip_limit_2>
                    <sub_cluster_ip_limit_3>a4_agg_zzd_search</sub_cluster_ip_limit_3>
                    <sub_cluster_ip_limit_4>zzd_search</sub_cluster_ip_limit_4>
                    <sub_cluster_ip_limit_5>pe_tools</sub_cluster_ip_limit_5>
                    <sub_cluster_ip_limit_6>php_web</sub_cluster_ip_limit_6>
                    <sub_cluster_ip_limit_7>php_web_offline</sub_cluster_ip_limit_7>
                    <sub_cluster_ip_limit_8>arrival_access_web</sub_cluster_ip_limit_8>
                    <sub_cluster_ip_limit_9>search_qa_deepqa</sub_cluster_ip_limit_9>

                    <!-- for migrate agg_feature Fetch ha3 forward index -->
                    <sub_cluster_aliws_forward_index>${kvParam_ha3_nest_feature}</sub_cluster_aliws_forward_index>

                    <!-- agg_web1 -->
                    <!--include_xml type="params">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/ns_params.xml</include_xml-->
                </params>
            </module>
            <module name="agg_web2_module" path="${A4_INSTALL_ROOT}/usr/local/lib64/liba4_agg_web2.so">
                <params>
                    <query_url_pk_file/>
                    <pk_index>mainindex_pk</pk_index>
                    <domain_suffix_file_path>${A4_INSTALL_ROOT}/usr/local/conf/domain_suffix.conf</domain_suffix_file_path>
                    <mix_dump>0</mix_dump>
                    <online_dump>0</online_dump>
                    <l3_dump>0</l3_dump>
                    <basic_index_set>fast_index;main_index;qa_index</basic_index_set>
                    <main_index>mainindex:${kvParam_vip_cluster},${kvParam_galaxy_cluster}</main_index>
                    <qa_index>qaindex:${kvParam_qaindex_cluster}</qa_index>
                    <big_index>bigindex:${kvParam_bigindex_cluster}</big_index>
                    <fast_index>fastindex:${kvParam_fastindex_cluster}</fast_index>
                    <rt_index>rtindex:${kvParam_rtindex_cluster}</rt_index>
                    <at_index>mainindex_at:${kvParam_vip_cluster},${kvParam_galaxy_cluster}</at_index>
                    <local_ip>${kvParam_local_ip}</local_ip>
                    
                    <host_config_datas>multi_site,news_site,adult_site,bbs_site,mobile_friendly_site</host_config_datas>
                    <config_collector>
                        sc_config:sc_config,intention_template_config:intention_template_config,timeliness_config:timeliness_config,online_dedup_config:multi_online_dedup_config,adjuster_config:adjuster_config
                    </config_collector>
                </params>
            </module>

            <!-- agg_web1 module -->
            <!--include_xml type="modules">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/modules.xml</include_xml-->
        </modules>

        <processors>
            <!-- debug -->
            <processor name="__buildin_debug_processor" module_name="agg_web2_module" class_name="__buildin_debug_processor">
                <params>
                    <debug_processor_file_serving_root>${A4_INSTALL_ROOT}/usr/local/var/data/</debug_processor_file_serving_root>
                    <debug_processor_package_dir>${A4_INSTALL_ROOT}/var/ainst/packages/</debug_processor_package_dir>
                </params>
            </processor>
            <!-- control -->
            <processor name="ns_ip_limit_processor" module_name="a4_ns_plugin" class_name="NsIpLimitProcessor">
                <params>
                    <access_ctl_data>acl_data</access_ctl_data>
                </params>
            </processor>
            <processor name="request_limit_processor" module_name="" class_name="RequestLimitProcessor">
                <params>
                    <max_qps>500</max_qps>
                </params>
            </processor>
            <processor name="file_processor" module_name="" class_name="FileProcessor">
                <params>
                    <file_dir>${A4_INSTALL_ROOT}/usr/local/var/data</file_dir>
                </params>
            </processor>

            <!-- interleaving -->
            <processor name="interleaving_parallel_execute_processor" module_name="agg_web2_module" class_name="InterleavingParallelExecuteProcessor">
                <params>
                    <default_app>agg_web</default_app>
                    <default_interleaving_app>interleaving</default_interleaving_app>
                    <interleaving_app/>
                    <default_interleaving_test/>
                    <default_interleaving_value/>
                    <timeout>2000</timeout>
                    <rc_for_split_failed>PR_PHASE_EXIT</rc_for_split_failed>
                    <rc_for_merge_failed>PR_PHASE_EXIT</rc_for_merge_failed>
                    <rc_for_dispatch_failed>PR_PHASE_EXIT</rc_for_dispatch_failed>
                </params>
            </processor>
            <processor name="interleaving_supplement_processor" module_name="agg_web2_module" class_name="SupplementProcessor">
                <params>
                    <max_start_hit>50</max_start_hit>
                </params>
            </processor>
            <processor name="interleaving_post_treat_processor" module_name="agg_web2_module" class_name="InterleavingPostTreatProcessor">
                <params>
                </params>
            </processor>
            <processor name="interleaving_log_processor" module_name="agg_web2_module" class_name="LogProcessor">
                <params>
                    <log_actor_set>display,remote,local</log_actor_set>
                    <remote_log_node>remote_log/third_party</remote_log_node>
                    <remote_log_header>Host:backlog.sm.alibaba-inc.com</remote_log_header>
                </params>
            </processor>
            <processor name="interleaving_result_format_processor" module_name="agg_web2_module" class_name="ResultFormatProcessor">
                <params>
                    <field_blacklist>ORI_MAIN_BODY_NOREVISE,predict_feature_map,HHM_MODEL_FEATURE,ORI_Main_Body,OriTitle,MAIN_BODY_NEW</field_blacklist>
                </params>
            </processor>
            <processor name="interleaving_latency_calculate_processor" module_name="agg_web2_module" class_name="LatencyCalculateProcessor">
                <params>
                    <session_latency_key>session_latency_in_parallel</session_latency_key>
                </params>
            </processor>

            <!-- normal -->
            <processor name="request_parse_processor" module_name="agg_web2_module" class_name="RequestParseProcessor">
                <params>
                </params>
            </processor>
            <processor name="geo_info_processor" module_name="agg_web2_module" class_name="GeoInfoProcessor">
                <params>
                    <lbs>lbs/coordinate</lbs> 
                </params>
            </processor>
            <processor name="query_filter_processor" module_name="agg_web2_module" class_name="QueryFilterProcessor">
                <params>
                    <query_filter>query_filter/shield</query_filter>
                    <default_app_id>web_query</default_app_id>
                    <default_bid>aggweb</default_bid>
                    <shield_router>from#wh10331:uc_toutiao|feed_search_union,wh10339:uc_toutiao|feed_search_union,uc_iflow_video:uc_toutiao|feed_search_union,kkyouth:kk_youth_query|kk_youth</shield_router>
                    <default_result>fi:3</default_result>
                </params>
            </processor>
            <processor name="agg_web_query_filter_processor" module_name="agg_web2_module" class_name="AggWebQueryFilterProcessor">
                <params>
                    <query_filter>query_filter/shield</query_filter>
                    <default_app_id>web_query</default_app_id>
                    <default_bid>aggweb</default_bid>
                    <shield_router>from#wh10331:uc_toutiao|feed_search_union,wh10339:uc_toutiao|feed_search_union,uc_iflow_video:uc_toutiao|feed_search_union,kkyouth:kk_youth_query|kk_youth</shield_router>
                    <default_result>fi:3</default_result>
                </params>
            </processor>

            <processor name="identity_processor" module_name="agg_web2_module" class_name="IdentityProcessor">
                <params>
                    <uads_server>uads/uads/uaapi</uads_server>
                    <uads_header>Host:uads.ucweb.com</uads_header>
                </params>
            </processor>
            <processor name="result_filter_processor" module_name="agg_web2_module" class_name="ResultFilterProcessor">
                <params>
                    <result_filter>result_filter/shield</result_filter>
                    <default_app_id>web_result</default_app_id>
                    <default_bid>aggweb_result</default_bid>
                    <shield_router>from#wh10331:web_result|feed_search_union_result,wh10339:web_result|feed_search_union_result,uc_iflow_video:web_result|feed_search_union_result,kkyouth:kk_youth_query|kk_youth</shield_router>
                    <default_filter>false</default_filter>
                    <max_word_count>100</max_word_count>
                    <url_key_ex>tinyapp_url,uc_tinyapp_url,quark_tinyapp_url</url_key_ex>
                </params>
            </processor>
            <processor name="query_rewrite_processor" module_name="agg_web2_module" class_name="QueryRewriteProcessor">
                <params>
                    <query_rewrite_node>gig/query_rewrite</query_rewrite_node>
                    <querylog_cache_node>querylog_cache</querylog_cache_node>
                    <query_rewrite_tf_node>qlog_rewrite_recall/query_rewrite?_chain=v1_proxy&amp;topk=5</query_rewrite_tf_node>
                    <query_rewrite_tf_v2_node>qlog_rewrite_recall_v2/query_rewrite?_chain=v2_proxy&amp;topk=5</query_rewrite_tf_v2_node>
                </params>
            </processor>
            <processor name="click_feedback_processor" module_name="agg_web2_module" class_name="ClickFeedbackProcessor">
                <params>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};bigindex:web_big_index:${kvParam_bigindex_cluster};rtindex:web_rt_index:${kvParam_rtindex_cluster},${kvParam_fastindex_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster};mainindex_at:web_at_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster}
                    </channels_clusters>
                    <rt_clickserver>rt_clickserver</rt_clickserver>
                    <main_clickserver>main_clickserver</main_clickserver> 
                    <main_pvserver>main_pvserver</main_pvserver>
                    <cluster_name>click_server</cluster_name>
                    <basic_config_item>metrics_src:click,format:protobuf,start:0,hit:20,query_pv_threshold:10,query_doc_pv_threshold:10</basic_config_item>
                    <config_item>metrics_src:click,format:protobuf,start:0,hit:50,query_pv_threshold:10,query_doc_pv_threshold:10</config_item>
                    <official_site_dict_server>official_site_dict</official_site_dict_server>
                    <similar_doc_pair_node>similar_click_doc_pair</similar_doc_pair_node>
                    <similar_doc_pair_v2_node>similar_click_doc_pair_v2</similar_doc_pair_v2_node>
                    <click_server_node>ha3_sm_click_server_v3</click_server_node>
                    <clickg_score_cache_node>clickg_score_cache</clickg_score_cache_node>
                    <click_server_cluster_name>qlog</click_server_cluster_name>
                    <click_server_config_item>format:protobuf,start:0,hit:10,query_pv_threshold:10,query_doc_pv_threshold:10</click_server_config_item>
                    <clickG_score_tf_node>clickG_score_node/clickG_predict?_chain=clickG_v1_117</clickG_score_tf_node>
                    <clickG_score_tf_node_v2>clickG_score_node_v2/clickG_predict?_chain=clickG_v2_169</clickG_score_tf_node_v2>
                    <click_rerank_nn_tf_node>click_rerank_nn_node/clickRerank?_chain=tf</click_rerank_nn_tf_node>
                </params>
            </processor>
            <processor name="query_analysis_processor" module_name="agg_web2_module" class_name="QueryAnalysisProcessor">
                <params>
                    <disable_site_syntax>1</disable_site_syntax>
                    <multi_exp_node>multi_exp_bert</multi_exp_node>
                    <multi_exp_v2_node>multi_exp_bert_v2</multi_exp_v2_node>
                    <dct_imp_node>dct_imp</dct_imp_node>
                    <acnn_cache_node>acnn_cache</acnn_cache_node>
                    <acnn_feature_node>acnn_feature_bert/?_chain=autobatch_bert_l4_nn_782_query</acnn_feature_node>
                    <acnn_v2_feature_node>acnn_feature_bert/?_chain=l3_colbert_query</acnn_v2_feature_node>
                    <qtcnn_feature_node>gig/qc_feature_bert/qtc_repre_minibert_model_query_embed?_chain=L3_QTC_REPRE_V18_QUERY</qtcnn_feature_node>
                    <qtcnn_v2_feature_node>gig/qc_v2_feature_bert/qtc_repre_12Lbert_v30_query_embed?_chain=l3_v2_qtc_repre_v30_avg_query</qtcnn_v2_feature_node>
                    <qtcnn_cache_node>qc_score_cache</qtcnn_cache_node>
                    <!-- L2NN params -->
                    <l2nn_feature_node>gig/qc_v2_feature_bert/qtc_l2_v6_s128_e128_query?_chain=l2_v6_s128_e128_kd_query</l2nn_feature_node>
                    <l2nn_v2_feature_node>gig/qc_v2_feature_bert/qtc_l2_v32_s128_e128_query?_chain=l2_v32_s128_e128_kd_query</l2nn_v2_feature_node>
                    <l2nn_cache_node>qc_score_cache</l2nn_cache_node>
                    <!-- L2NN params end -->
                    <qp_node>gig/qp/web</qp_node>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};bigindex:web_big_index:${kvParam_bigindex_cluster};rtindex:web_rt_index:${kvParam_rtindex_cluster},${kvParam_fastindex_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster};mainindex_at:web_at_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster}
                    </channels_clusters>
                    <kv_item>fr:xxx,ip:0.0.0.0,st:wap,tl:54,sl:100,adult_demote:255,sp:on,mode:xxx,or_logic:v2</kv_item>
                    <config_item>start:0,hit:,navigationalQuery:on,filtermode:11</config_item>
                    <prior_edu_signal_node>prior_edu_signal/?_chain=prior</prior_edu_signal_node>
                    <query_prior_intention_node>query_prior_intention</query_prior_intention_node>
                    <s3_kvindex_node>s3_kvindex</s3_kvindex_node>
                    <query_ner_service_node>query_ner_service</query_ner_service_node>
                </params>
            </processor>
            <processor name="index_processor" module_name="agg_web2_module" class_name="IndexProcessor">
                <params>
                    <!-- actor info_map, index_processor使用的Actor列表, key为actor的name, value为actor的类型 -->
                    <actor_info_map>check_index_degrade_actor:CheckIndexDegradeActor,qrs_clause_actor:QrsClauseActor,read_cache_actor:ReadCacheActor,request_index_actor:RequestIndexActor,basic_rerank_actor:BasicRerankActor,fetch_summary_actor:FetchSummaryActor,update_cache_actor:UpdateCacheActor,senior_pre_rerank_actor:SeniorPreRerankActor,senior_rerank_actor:SeniorRerankActor,senior_rerank_exec_actor:SeniorRerankExecActor</actor_info_map>

                    <!-- check index degrade -->
                    <degrade_param_config_file>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_degrade_param_config_file}</degrade_param_config_file>
                    <degrade_param_map>mainindex:mainindex,qaindex:qaindex,rtindex:rtindex</degrade_param_map>
                    <degrade_ref_index>mainindex</degrade_ref_index>
                    
                    <!-- gen qrs clause -->
                    <common>
                        kv=topnAcFeatureThrehold:500#distinct=dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2#config=format:protobuf,no_summary:yes,proto_format_option:pb_matchdoc_format,timeout:1000,result_compress:snappy
                    </common>
                    <index_list>mainindex_at,mainindex,bigindex,qaindex,rtindex</index_list>
                    <mainindex_at>
                        config=searcher_return_hits:10,hit:100,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,IndustryStat,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL,ContentQuality,LandingPageCategory,LandingPageScore
                    </mainindex_at>
                    <mainindex>
                        config=searcher_return_hits:10,hit:500,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,IndustryStat,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL,ContentQuality,LandingPageCategory,LandingPageScore
                    </mainindex>
                    <bigindex>
                        config=searcher_return_hits:10,hit:500,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,PQQualityScore,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL,ContentQuality,LandingPageCategory,LandingPageScore
                    </bigindex>
                    <qaindex>
                        distinct=dist_key:V_HOST_HASH,dist_count:10;dist_key:V_HOST_HASH,dist_count:6#kv=sc_summary_conf:1`1280`1`1`1#config=searcher_return_hits:10,hit:50,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,IndustryStat,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL,ContentQuality,LandingPageCategory,LandingPageScore
                    </qaindex>
                    <rtindex>
                        config=searcher_return_hits:10,hit:500,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,PQQualityScore,DNN_AT_ID_ATTR,DNN_TITLE_ID_ATTR,FirstIndexedTime,IndustryStat,SiteValue,TimeSensitive,V_SECURITY_LEVEL,ContentQuality,LandingPageCategory,LandingPageScore
                    </rtindex>
                    <ranker_bucket>
                        bucket_2=mainindex:BucketProfile,qaindex:BucketProfile,bigindex:BucketProfile#bucket_3=mainindex:DefaultProfile,qaindex:DefaultProfile,bigindex:DefaultProfile
                    </ranker_bucket>
                    <ranker_bucket_key>ranker</ranker_bucket_key>
                    <ranker_interleaving_key>ltrmodelexp</ranker_interleaving_key>
                    <ranker_interleaving>ltrmodelexp=mainindex:BucketProfile,qaindex:BucketProfile</ranker_interleaving>
                    <interleaving_qrs_config>${kvParam_interleaving_qrs_config}</interleaving_qrs_config>
                    <abtest_qrs_config>${kvParam_abtest_qrs_config}</abtest_qrs_config>
                    <l1lr_ranker_profile>mainindex:HJProfile,qaindex:HJProfile,bigindex:HJProfile</l1lr_ranker_profile>

                    <!-- cache -->
                    <max_start_hit>50</max_start_hit>
                    <main_cache_node>main_cache</main_cache_node>
                    <quark_main_cache_node>quark_main_cache</quark_main_cache_node>
                    <main_cache_compress_type>snappy</main_cache_compress_type>
                    <rt_cache_node>rt_cache</rt_cache_node>
                    <rt_cache_compress_type>snappy</rt_cache_compress_type>

                    <!-- read cache -->
                    <cache_key_with_app_name>0</cache_key_with_app_name>
                    <main_cache_key_channel>mainindex</main_cache_key_channel>
                    <main_magic_number>agg_web2_senior_123456789</main_magic_number>
                    <main_cache_index_set>mainindex,qaindex</main_cache_index_set>
                    <rt_cache_key_channel>rtindex</rt_cache_key_channel>
                    <rt_magic_number>agg_web2_senior_123456789</rt_magic_number>
                    <rt_cache_index_set>rtindex</rt_cache_index_set>
                    <demote_shared_key>adult_demote_shared_key,political_demote_shared_key</demote_shared_key>
                    
                    <!-- request index -->
                    <baidu_querylog_node>baidu_querylog</baidu_querylog_node>
                    <at_request_rate>0.15</at_request_rate>
                    <need_atindex_totalhitcount>5000</need_atindex_totalhitcount>
                    <need_atindex_firstpagescore>400</need_atindex_firstpagescore>
                    <need_atindex_maxpv>50</need_atindex_maxpv>
                    <need_bigindex_totalhitcount>10000</need_bigindex_totalhitcount>
                    <need_bigindex_firstpagescore>80</need_bigindex_firstpagescore>
                    <need_secondrequest_firstpagescore>200</need_secondrequest_firstpagescore>
                    <second_request_rate>0.15</second_request_rate>
                    <extend_query_index_name_map/>
                    <degrade_index_set>mainindex,qaindex,rtindex</degrade_index_set>
                    
                    <!-- fetch summary -->
                    <fetch_count>100</fetch_count>
                    <rule>phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3,profile:MainBodyProfile;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_semantic,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:qaindex_semantic,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex_semantic,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK</rule>
                    <!-- update cache -->
                    <main_cache_index_list>mainindex,bigindex,qaindex</main_cache_index_list>
                    <main_cache_effective_time>${kvParam_main_cache_effective_time}</main_cache_effective_time>
                    <main_cache_compress_type>snappy</main_cache_compress_type>
                    <rt_cache_index_list>rtindex</rt_cache_index_list>
                    <rt_cache_effective_time>${kvParam_rt_cache_effective_time}</rt_cache_effective_time>
                    <rt_cache_compress_type>snappy</rt_cache_compress_type>

                    <!-- senior rerank -->
                    <brain_agg_qt_semantic_node>gig/qt_semantic/ts</brain_agg_qt_semantic_node>
                    <aliws_forward_index_node>http/aliws_forward_index/</aliws_forward_index_node>

                    <qtc_feature_node_online_t4>gig/qc_feature_bert</qtc_feature_node_online_t4>
                    <qtc_feature_node_online_p100>gig/qc_feature_bert_p100</qtc_feature_node_online_p100>
                    <qtc_feature_node_online_2080>gig/qc_feature_bert_2080</qtc_feature_node_online_2080>

                    <qc_score_cache_node>qc_score_cache</qc_score_cache_node>

                    <agg_feature_node>brain_agg_feature</agg_feature_node>
                    <agg_feature_ufs_node>agg_feature_ufs</agg_feature_ufs_node>

                    <distinct_param>dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2</distinct_param>
                    <sc_aol_node>gig/sc_aol</sc_aol_node>

                    <!-- senior rerank exec -->
                    <brain_agg_model_QTC_EXAM_ONLINE_node>brain_agg_model_ufs</brain_agg_model_QTC_EXAM_ONLINE_node>
                </params>
            </processor>
            <processor name="exp_index_processor" module_name="agg_web2_module" class_name="ExpIndexProcessor">
                <params>
                    <!-- fetch summary mainbody -->
                    <max_start_hit>50</max_start_hit>
                    <fetch_count>100</fetch_count>
                    <rule>
                        phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3,profile:MainBodyProfile;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK
                    </rule>
                </params>
            </processor>
            <processor name="ae_index_processor" module_name="agg_web2_module" class_name="AeIndexProcessor">
                <params>
                    <!-- ae fetch summary -->
                    <max_start_hit>10</max_start_hit>
                    <fetch_count>20</fetch_count>
                    <!--rule>
                        phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK
                    </rule-->
                    <rule>
                        phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK
                    </rule>
                </params>
            </processor>
            <processor name="simdoc_processor" module_name="agg_web2_module" class_name="SimDocProcessor">
                <params>
                    <similar_doc_pair_node>similar_doc_pair</similar_doc_pair_node>
                    <similar_doc_pair_offline_node>similar_doc_pair_offline</similar_doc_pair_offline_node>
                    <wk_similar_doc_pair_offline_node>wk_similar_doc_pair_offline</wk_similar_doc_pair_offline_node>
                    <simdoc_replace_step>2</simdoc_replace_step>
                </params>
            </processor>
           
            <processor name="exp_simdoc_processor" module_name="agg_web2_module" class_name="ExpSimDocProcessor">
                <params>
                    <similar_doc_pair_node>similar_doc_pair</similar_doc_pair_node>
                    <similar_doc_pair_offline_node>similar_doc_pair_offline</similar_doc_pair_offline_node>
                    <simdoc_replace_step>1</simdoc_replace_step>
                </params>
            </processor>

            <processor name="qa_agg_processor" module_name="agg_web2_module" class_name="QaAggProcessor">
                <params>
                    <qa_agg_node>gig/qa_agg</qa_agg_node>
                    <qa_ss_node>qa_ss_node/hqqrs_baike_summary</qa_ss_node>
                    <ae_node>qa_ss_node/web_summary</ae_node>
                    <multi_source_node>qa_ss_node/multi_source_summary</multi_source_node>
                    <max_num>20</max_num>
                    
                    <arrival_summary_cache_node>arrival_summary_cache</arrival_summary_cache_node>
                    <effective_time>${kvParam_rt_cache_effective_time}</effective_time>
                </params>
            </processor>
            
            <processor name="deep_summary_processor" module_name="agg_web2_module" class_name="DeepSummaryProcessor">
                <params>
                    <ae_node>qa_ss_node/deep_summary</ae_node>
                    <max_num>10</max_num>
                </params>
            </processor>
             
            <processor name="quark_baike_processor" module_name="agg_web2_module" class_name="QuarkBaikeProcessor">
                <params>
                    <quark_baike_node>quark_baike_node/baike_search</quark_baike_node>
                </params>
            </processor>
 
            <processor name="intention_processor" module_name="agg_web2_module" class_name="IntentionProcessor">
                <params>
                    <query_aol_node>gig/query_aol/intention</query_aol_node>
                    <content_intention_node>content_intention</content_intention_node>
                </params>
            </processor>

            <processor name="medical_processor" module_name="agg_web2_module" class_name="MedicalProcessor">
                <params>
                    <medical_intention_server>medical_intention/classify</medical_intention_server>
                    <medical_vertical_server>medical_vertical/agg</medical_vertical_server>
                    <medical_rerank_model_server>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_medical&amp;chain=medical_rerank</medical_rerank_model_server>
                    <medical_auth_rerank_model_server>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_medical&amp;chain=medical_auth_rerank</medical_auth_rerank_model_server>
                    <medical_sc_trigger_server>medical_sc_trigger/default_chain</medical_sc_trigger_server>
                </params>
            </processor>

            <processor name="sc_processor" module_name="agg_web2_module" class_name="ScProcessor">
                <params>
                    <max_start_hit>50</max_start_hit>
                    <default_vendor>100002</default_vendor>
                    <arbiter_node>gig/arbiter</arbiter_node>
                    <retrigger_arbiter_node>arbiter_retrigger</retrigger_arbiter_node>
                    <sc_aol_node>gig/sc_aol</sc_aol_node>
                    <edu_signal_node>edu_signal/?_chain=poster</edu_signal_node>
                    <!-- <qtc_student_node>gig/qc_feature_bert_2080/qtc_minibert_model?_chain=qtc_v22_warm_qt_qc_ckpt_1206256</qtc_student_node> -->
                    <qtc_student_node>sc_qtc_minibert/qtc_minibert_model?_chain=qtc_v22_warm_qt_qc_ckpt_1206256</qtc_student_node>
                    <query_poster_intention_node>query_poster_intention</query_poster_intention_node>
                    <intention_tair_node>intention_tair</intention_tair_node>
                </params>
            </processor>

            <processor name="qtc_processor" module_name="agg_web2_module" class_name="QtcProcessor">
                <params>
                    <qc_score_cache_node>qc_score_cache</qc_score_cache_node>
                    <qtc_teacher_node>gig/qtc_student_v2</qtc_teacher_node>
                    <qtc_student_node>gig/qc_feature_bert_2080</qtc_student_node>

                    <!-- fetch summary -->
                    <rule>phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3,profile:MainBodyProfile;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_semantic,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:qaindex_semantic,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex_semantic,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK</rule>
                    <cluster_replace>galaxy:galaxy2</cluster_replace>
                </params>
            </processor>

            <processor name="querylog_processor" module_name="agg_web2_module" class_name="QueryLogProcessor">
                <params>
                    <baidu_querylog_node>baidu_querylog</baidu_querylog_node>
                    <tt_querylog_node>tt_querylog</tt_querylog_node>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};bigindex:web_big_index:${kvParam_bigindex_cluster};rtindex:web_rt_index:${kvParam_rtindex_cluster},${kvParam_fastindex_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster};mainindex_at:web_at_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster}
                    </channels_clusters>
                    <insert_model_v2_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v2</insert_model_v2_node>
                    <insert_model_v3_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v3</insert_model_v3_node>
                    <insert_model_v4_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v4</insert_model_v4_node>
                    <insert_model_v5_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v5</insert_model_v5_node>
                    <insert_model_v6_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v6</insert_model_v6_node>
                    <insert_model_v7_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v7</insert_model_v7_node>
                    <clickG_score_tf_node>clickG_score_node/clickG_predict?_chain=clickG_v1_117</clickG_score_tf_node>
                    <qlog_score_tf_node>qlog_score_node/qlog_score?_chain=tf</qlog_score_tf_node>
                    <ha3_qlog_node>ha3_qlog</ha3_qlog_node>
                    <ha3_qlog_v2_node>ha3_qlog_v2</ha3_qlog_v2_node>
                    <cluster_name>qlog</cluster_name>
                    <config_item>format:protobuf,start:0,hit:10,query_pv_threshold:10,query_doc_pv_threshold:10</config_item>
                </params>
            </processor>
            <processor name="video_processor" module_name="agg_web2_module" class_name="VideoProcessor">
                <params>
                    <ugc_video_server>ugc_video/yisou_video_external</ugc_video_server>
                </params>
            </processor>

            <!--
            <processor name="recommend_web_processor" module_name="agg_web2_module" class_name="RecommendWebProcessor">
                <params>
                    <recommend_web_server>gig/recommend_web/web</recommend_web_server>
                </params>
            </processor>
            -->

            <processor name="rs_processor" module_name="agg_web2_module" class_name="RsProcessor">
                <params>
                    <rs_server>gig/relative_search/aggpb</rs_server>
                    <rs_union_server>gig/relative_search/feedsearch</rs_union_server>
                </params>
            </processor>

            <processor name="novel_processor" module_name="agg_web2_module" class_name="NovelProcessor">
                <params>
                    <novel_update_server>novel_update</novel_update_server>
                    <spl_server>zeus_agg_specific_logic/work</spl_server>
                </params>
            </processor>
            
            <processor name="timeliness_processor" module_name="agg_web2_module" class_name="TimelinessProcessor">
                <params>
                    <timeliness_server>timeliness/freshment</timeliness_server>
                    <timeliness_rerank_model_server>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_timeliness&amp;chain=timeliness_predict</timeliness_rerank_model_server>
                    <timeliness_trigger_model_server>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_timeliness&amp;chain=timeliness_trigger</timeliness_trigger_model_server>
                </params>
            </processor>

            <processor name="feed_processor" module_name="agg_web2_module" class_name="FeedProcessor">
                <params>
                    <feed_replace_server>feed_replace/agg</feed_replace_server>
                    <abcnn_server>feed_search_gpu_service/abcnn_trt</abcnn_server>
                    <abcnn_feature_format_info>q_term_id_list@@#@@i_title_term_id_list@@#@@[dat]@@#@@=0:[[@@#@@]]</abcnn_feature_format_info>
                    <distcnn_server>feed_search_cpu_service/distcnn</distcnn_server>
                    <distcnn_feature_format_info>query@@#@@title_pos@@#@@[dat]@@#@@=0:@@#@@</distcnn_feature_format_info>
                </params>
            </processor>

            <processor name="orion_processor" module_name="agg_web2_module" class_name="OrionProcessor">
                <params>
                    <cms_server>cms/cms/query_res</cms_server>
                    <cms_host>${kvParam_cms_host}</cms_host>
                    <cms_secret>${kvParam_cms_secret}</cms_secret>
                    <filter_param>bucket,city,partner_id,prov,query_words,result_amount,sc,ts,uid,ver</filter_param>
                </params>
            </processor>

            <processor name="authority_processor" module_name="agg_web2_module" class_name="AuthorityProcessor">
                <params>
                    <brain_agg_qt_semantic_node>gig/qt_semantic/ts</brain_agg_qt_semantic_node>
                    <actor_info_map>authority_rerank_actor:AuthorityRerankActor</actor_info_map>
                    <authority_rerank_model_node>authority_agg_rerank_model_ufs/?_chain=ufs_app&amp;scene=agg_web&amp;chain=authority_rerank_model</authority_rerank_model_node>    
                </params>
            </processor>

            <processor name="content_processor" module_name="agg_web2_module" class_name="ContentProcessor">
                <params>
                    <content_model_node>content_model_ufs/?_chain=ufs_app&amp;scene=content_based_rank&amp;chain=v1</content_model_node>
                    <content_model_node_v2>content_model_ufs/?_chain=ufs_app&amp;scene=content_based_rank_v4&amp;chain=v1</content_model_node_v2>
                </params>
            </processor>

            <processor name="baike_processor" module_name="agg_web2_module" class_name="BaikeProcessor">
                <params>
                </params>
            </processor>

            <processor name="tieba_processor" module_name="agg_web2_module" class_name="TiebaProcessor">
                <params>
                </params>
            </processor>

            <processor name="special_host_processor" module_name="agg_web2_module" class_name="SpecialHostProcessor">
                <params>
                    <relevance_server>relevance_service</relevance_server>
                </params>
            </processor>

            <processor name="internal_merge_processor" module_name="agg_web2_module" class_name="InternalMergeProcessor">
                <params>
                    <sc_pos_model_node>sc_pos_model</sc_pos_model_node>
                </params>
            </processor>

            <processor name="internal_rerank_processor" module_name="agg_web2_module" class_name="InternalRerankProcessor">
                <params>
                </params>
            </processor>

            <processor name="demote_processor" module_name="agg_web2_module" class_name="DemoteProcessor">
                <params>
                    <!-- demote qrsclause  -->
                    <adult_demote_key>adult_demote</adult_demote_key>
                    <political_demote_key>political_demote</political_demote_key>
                    <adult_demote_shared_key>adult_demote_shared_key:no_adult</adult_demote_shared_key>
                    <political_demote_shared_key>political_demote_shared_key:no_political</political_demote_shared_key>
                    <cid_demote_rule>7:1</cid_demote_rule>

                    <!-- sifter -->
                    <sifter_node>sifter</sifter_node>
                    
                    <!-- sifter attribute -->
                    <max_sifter_attribute_count>500</max_sifter_attribute_count>
                    
                    <!-- sifter summary -->
                    <max_sifter_summary_count>50</max_sifter_summary_count>
                    <max_start_hit>50</max_start_hit>
                    
                    <!-- host_demote -->
                </params>
            </processor>
            
            <processor name="dump_processor" module_name="agg_web2_module" class_name="DumpProcessor">
                <params>
                </params>
            </processor>

            <processor name="quark_processor" module_name="agg_web2_module" class_name="QuarkProcessor">
                <params>
                    <wenda_summary_server>wenda_summary</wenda_summary_server>
                    <node_option>profile:WendaProfile,keep_range_id:no,key_type:RAW_PK</node_option>
                </params>
            </processor>

            <processor name="summary_modify_processor" module_name="agg_web2_module" class_name="SummaryModifyProcessor">
                <params>
                    <summary_modify_server>summary_modify</summary_modify_server>
                </params>
            </processor>

            <processor name="backup_processor" module_name="agg_web2_module" class_name="BackupProcessor">
                <params>
                </params>
            </processor>

            <processor name="external_merge_processor" module_name="agg_web2_module" class_name="ExternalMergeProcessor">
                <params>
                    <brain_agg_model_speck_adjust_node_ufs_v0525>spell_check_model_ufs/?_chain=ufs_app&amp;scene=speck&amp;chain=agg_speck_0525</brain_agg_model_speck_adjust_node_ufs_v0525>
                    <brain_agg_model_speck_adjust_simple_node_ufs_v0525>spell_check_model_ufs/?_chain=ufs_app&amp;scene=speck&amp;chain=agg_speck_simple_0525</brain_agg_model_speck_adjust_simple_node_ufs_v0525>
                    <brain_agg_model_speck_adjust_node_ufs_v0909>spell_check_model_ufs/?_chain=ufs_app&amp;scene=speck&amp;chain=agg_speck_0909</brain_agg_model_speck_adjust_node_ufs_v0909>
                    <brain_agg_model_speck_adjust_simple_node_ufs_v0909>spell_check_model_ufs/?_chain=ufs_app&amp;scene=speck&amp;chain=agg_speck_0909_simple</brain_agg_model_speck_adjust_simple_node_ufs_v0909>
                    <hhm_model_node>hhm_model_zeus_tfs/hhm_model</hhm_model_node>
                </params>
            </processor>
            <processor name="qu_processor" module_name="agg_web2_module" class_name="QuProcessor">
                <params>
                    <qu_node>gig/qu_node</qu_node>
                    <chain>query_understanding</chain>
                </params>
            </processor>
            <processor name="edu_processor" module_name="agg_web2_module" class_name="EduProcessor">
                <params>
                    <intention_tair_node>intention_tair</intention_tair_node>
                </params>
            </processor>
            <processor name="dedup_processor" module_name="agg_web2_module" class_name="DedupProcessor">
                <params>
                    <online_dedup_num>40</online_dedup_num>
                    <basic_dedup_num>30</basic_dedup_num>
                    <url_dedup_num>20</url_dedup_num>
                    <diversity_dedup_num>15</diversity_dedup_num>
                </params>
            </processor>


            <!--rujun.grj-->
            <processor name="qt_processor" module_name="agg_web2_module" class_name="QtProcessor">
                <params>
                    <brain_agg_qt_semantic_node>gig/qt_semantic/ts</brain_agg_qt_semantic_node>
                    <aliws_forward_index_node>http/aliws_forward_index/</aliws_forward_index_node>
                    <agg_feature_node>brain_agg_feature</agg_feature_node>
                    <agg_feature_ufs_node>agg_feature_ufs</agg_feature_ufs_node>

                    <!-- fetch summary -->
                    <rule>phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3,profile:MainBodyProfile;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_semantic,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:qaindex_semantic,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex_semantic,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK</rule>
                    <cluster_replace>galaxy:galaxy2</cluster_replace>
                </params>
            </processor>
           
            <processor name="filter_processor" module_name="agg_web2_module" class_name="FilterProcessor">
                <params>
                </params>
            </processor>

            <processor name="makeup_processor" module_name="agg_web2_module" class_name="MakeupProcessor">
                <params>
                    <makeup_server>makeup/operation</makeup_server>
                </params>
            </processor>

            <processor name="post_processor" module_name="agg_web2_module" class_name="PostProcessor">
                <params>
                    <!-- SitelinkActor -->
                    <url_field>RawUrl,NormalizedUrl,OriUrl</url_field>
                    <data>site_link</data>
                    <bucket_data>bucket_1:site_link,bucket_2:site_link2,bucket_3:site_link3,bucket_4:site_link2</bucket_data>

                    <!-- CheckQuarkStatusActor -->
                    <config_server>config_server/web</config_server>
                </params>
            </processor>
            <processor name="pan_demand_processor" module_name="agg_web2_module" class_name="PanDemandProcessor">
                <params>
                </params>
            </processor>
            <processor name="aggregator_processor" module_name="agg_web2_module" class_name="AggregatorProcessor">
                <params>
                    <upvote_tair_node>upvote</upvote_tair_node>
                </params>
            </processor>
            <processor name="supplement_processor" module_name="agg_web2_module" class_name="SupplementProcessor">
                <params>
                    <max_start_hit>50</max_start_hit>
                </params>
            </processor>
            <processor name="log_processor" module_name="agg_web2_module" class_name="LogProcessor">
                <params>
                    <log_actor_set>display,remote,local</log_actor_set>
                    <remote_log_node>remote_log/third_party</remote_log_node>
                    <remote_log_header>Host:backlog.sm.alibaba-inc.com</remote_log_header>
                </params>
            </processor>
            <processor name="result_format_processor" module_name="agg_web2_module" class_name="ResultFormatProcessor">
                <params>
                    <field_blacklist>ORI_MAIN_BODY_NOREVISE,predict_feature_map,HHM_MODEL_FEATURE,ORI_Main_Body,OriTitle,MAIN_BODY_NEW</field_blacklist>
                </params>
            </processor>
            <processor name="latency_calculate_processor" module_name="agg_web2_module" class_name="LatencyCalculateProcessor">
                <params>
                    <session_latency_cache_hit_key>session_latency_cache_hit_in_default</session_latency_cache_hit_key>
                    <session_latency_cache_miss_key>session_latency_cache_miss_in_default</session_latency_cache_miss_key>
                    <session_latency_key>session_latency_in_default</session_latency_key>
                </params>
            </processor>
            <processor name="bangbangda_processor" module_name="agg_web2_module" class_name="BangbangdaProcessor">
                <params>
                    <debug>0</debug>
                </params>
            </processor>
            <processor name="faiss_processor" module_name="agg_web2_module" class_name="FaissProcessor">
                <params>
                    <!-- request -->
                    <node_name>gig/semantic_zeus_service</node_name>
                    <model_name>representation_v28</model_name>
                    <chain_name>l3_rank</chain_name>
                    <chain_name_iter>l3_rank_iter</chain_name_iter>

                    <!-- trigger model-->
                    <feature_stat_info>aol_app:1.0,-1.0,0.0859508408151,0.242503111947;aol_baike:0.999998,-1.0,0.118770225198,0.256661291839;aol_map:1.0,-1.0,0.0941284645492,0.254510773026;aol_novel:1.0,-1.0,0.0889701266336,0.277750410047;aol_pic:1.0,-1.0,0.0679781556902,0.219931981981;aol_shopping:0.999959,-1.0,0.0475556749903,0.180392917441;aol_video:0.999999,-1.0,0.134111591713,0.288269328036;count:150.0,2.0,20.5762423927,14.3949978051;normpv:18.395406,-1.0,6.75521302137,7.06574279369;pv:24854600.0,-1.0,9116.81007257,174648.689656;ac_score_min:60350.886718,-2230.061279,-115.730509062,1253.50089393;ac_score_max:111656.445312,-1181.756347,952.883451317,4398.28198634;ac_score_std:46063.3298394,0.0,264.686066875,1058.21806936;lightgbmscore_min:555.998718,158.699783,331.51663514,57.1757519074;lightgbmscore_max:755.473937,230.210067,469.606077486,47.562488076;lightgbmscore_std:209.58111553,0.0,37.6862126216,14.6303847671;anchorscore_max:183.217697,0.0,7.98907501299,16.9783838793;textscore_std:104.426213925,0.0,21.7702357708,11.8509128415;oldtextscore_max:306.541687,0.0,132.058824593,25.3166454395;oldtextscore_std:148.614151282,0.0,28.7385754983,15.2792416958;oldtextscore_median:227.908546,0.0,100.881351279,37.3632178335;mixacscore_max:21.3235,-5.0,5.3651244368,2.76297510955;mixacscore_mean:13.94033,-5.0,2.15882372513,1.96534491138;mixacscore_std:12.9553629575,0.0,1.82112245472,0.804550317672;mixacscore_median:16.8502,-5.0,2.11160918231,2.07761376648;mlauthorityscore_min:6.016335,-8.69028,-4.28866327857,0.884151603352;mlauthorityscore_median:6.4162685,-6.285617,-3.00108616202,0.835544527796;pqqualityscore_max:5.372019,0.336331,3.35413588436,0.515042080211;pqqualityscore_mean:4.8009495,0.153137714286,2.71805629626,0.292858384113;pqqualityscore_std:2.76776867663,0.0,0.408927086919,0.216114435213;pqqualityscore_median:4.8458095,-0.163983,2.77261054427,0.272448003853;bertl4ac_max:93.063,-5.0,77.4935325808,7.18465302375;bertl4ac_mean:87.685,-5.0,71.0524451197,7.19178061142;bertl4ac_median:88.0545,-5.0,71.5562279951,7.15440164238;bertcls_mean:-5.0,-76.7011363636,-71.5118230362,6.25743989936;bertcls_median:-5.0,-77.4155,-71.9243522963,6.13058592945;bertmse_max:9.598,-7.032,1.42558061173,2.06068860127;bertmse_mean:6.247,-8.74084210526,-1.0806244834,1.86178348016;bertmse_std:9.11531351628,0.0,1.56702408652,0.646093645137;bertmse_median:6.927,-9.236,-1.03139312358,1.97736249813</feature_stat_info>
                <feature_weights>2.76960083e-03,4.59356763e-01,7.50892054e-01,-1.66193921e+00,2.09633569e-01,5.38906220e-01,-7.00180331e-01,-3.32622264e+00,-4.12563885e-01,-5.33507396e-02,2.08272778e+00,-6.07708781e+00,3.55337659e+00,2.48207008e+00,-2.44463060e+00,3.56770974e-01,-2.53753661e+00,7.57546885e-01,-8.88924804e-02,-7.90347582e-01,-5.71720769e-01,-8.49280448e-01,2.72661788e-01,3.95155973e-01,-5.81321482e-01,5.61125455e-01,1.17377151e+00,-6.57448330e-01,6.90519887e-01,2.83327925e-02,-1.49108054e+00,-1.73014968e-01,-1.63003999e+00,-1.21652099e+00,-2.53407754e+00,-7.27389612e-01,-1.50213920e+00,-3.20108291e+00,-4.65555498e-01,-2.67883810e-01</feature_weights>
                <model_bias>9.5600443</model_bias>

                <!-- faiss makeup -->
                  <makeup_feature_stat>nosug_norm:1.0,0.0,0.0280017962109,0.164939252886;ishqadult_norm:1.0,0.0,0.00197902069899,0.0444414908099;bert_cls_norm:-68.2731,-75.572,-72.7316925364,1.43694367561;mainbodylength_norm:5010.0,0.0,656.023147084,999.157793664;lastdocclicks_ac_norm:1023.0,0.0,48.9891448688,177.656578677;aol_travel_norm:1.0,0.0,0.0133007495314,0.0980868358175;timeincontentscore_norm:146.0,-16.0,21.4461530615,33.0707649918;at_hit_variance_ac_norm:10.0,0.0,2.13147351034,3.0982925125;strongentitynum_norm:4.0,0.0,0.753346567698,0.893689991762;qwtrcos_ac_norm:1.0,0.0,0.716582286622,0.258326479761;isnews_norm:1.0,0.0,0.869884981003,0.336423867854;contdnn_ac_norm:1.0,0.0,0.365304737002,0.269788956971;new_ctr_cqr_ac_norm:1.0,0.0,0.39132186207,0.290004247017;anchorctr_ac_norm:1.0,0.0,0.173367324507,0.325110884041;bertl4acscore_norm:83.7848,46.5534,70.800129323,5.74720740983;authorityscore_norm:58.4978,0.0,21.9464263288,14.0702585222;entitynum_norm:4.0,0.0,0.788533758111,0.916526473101;v28_score_norm:0.0,-10.1994,-5.02257174846,1.86314910853;aol_fanyi_norm:1.0,0.0,3.77142502995e-07,2.58897288768e-06;at_start_pos_ac_norm:32.0,0.0,5.50616999455,9.65926168257;pv_norm:179634.0,0,5255.97189762,22459.7343835;atprox1_ac_norm:1.0,0.0,0.883115698981,0.318785962052;ctrcqr_ac_norm:1.0,0.0,0.551456920609,0.314161862312;ctrscore_norm:1.0,0.0,0.623852636773,0.314644192457;andtermcount_norm:6.0,0.0,2.15760826002,1.3826859275;semanticmiss_ac_norm:1.0,0.0,0.795537541577,0.276868220343;isvideo_norm:1.0,0.0,0.00398537270834,0.0630038906358;mlauthorityscore_norm:2.7814,-6.4294,-2.9172261518,1.50961148728;aol_app_norm:1.0,0.0,0.0870242845684,0.241130103078;anchorscore_ac_norm:22.7301,0.0,0.992626840999,3.43192412658;aol_pic_norm:1.0,0.0,0.071073986288,0.221740637151;new_cqr2_ac_norm:1.0,0.0,0.4817999602,0.287304734767;isadultquery_ac_norm:1.0,0.0,0.0425063699049,0.201664007182;aol_weather_norm:1.0,0.0,0.00637600334099,0.0781339477283;querypv_ac_norm:174424.0,0.0,5122.73307557,21821.2732814;aol_video_norm:1.0,0.0,0.132490301609,0.283412825537;docmatchcqr_ac_norm:1.0,0.0,0.792957818578,0.263150564319;atctr_ac_norm:1.0,0.0,0.570201789179,0.331123258413;bert_mse_norm:4.3269,-6.9412,-1.52118417644,2.36641229063;aol_song_norm:1.0,0.0,0.0133609445307,0.0936960029414;new_cqr_ac_norm:1.0,0.0,0.525314904949,0.272129423078;omit4ratio_ac_norm:1.0,0.0,0.806740688379,0.293187545235;cmauthscore_norm:3019999.0,0.0,245393.419603,616936.377162;ctrcqromit_ac_norm:1.0,0.0,0.52595493265,0.324105779728;ngramscore_norm:653.06,0.0,60.2546350143,158.24851744;aol_lottery_norm:1.0,0.0,0.00133663268416,0.0341276907507;new_ctr_ac_norm:1.0,0.0,0.607187275478,0.30617844157;rank_norm:20.0,0,9.49828738869,6.57622762764;ltr_body_miss_loc_ac_norm:5.0,0.0,3.09320153792,1.67004186188;orderproxinanchor_ac_norm:1.0,0.0,0.0221972707281,0.142556646625;aol_dict_norm:1.0,0.0,0.012290382776,0.101529907797;ltr_title_miss_loc_ac_norm:5.0,0.0,3.70634989658,1.06651150887;new_omit_ratio_ac_norm:1.0,0.0,0.665202641251,0.341046580175;aol_gaokao_norm:1.0,0.0,0.003164415434,0.0450656649557;aolauthscore_norm:4.1891,-7.5738,-1.01563936694,1.45505539747;mixacscore_norm:9.26,-7.19,2.0984462339,2.69169320381;timesensitivescore_norm:3.145,-3.0,0.675671175019,1.76078035196;distcnn_ac_norm:3.7432,-4.0,0.878320198575,1.3409085412;title_start_pos_ac_norm:32.0,0.0,4.25013450206,7.66735022958;aol_map_norm:1.0,0.0,0.0962192703954,0.253539117214;isqualitytieba_norm:1.0,0.0,0.035840505002,0.185892359964;isspam_norm:1.0,0.0,0.00528592023066,0.0725119316736;ispureentity_norm:1.0,0.0,0.0611684008936,0.239542567578;isgambling_norm:1.0,0.0,0.00225158671218,0.0473878594721;docclicks_ac_norm:1023.0,0.0,59.9686230786,196.547564716;isqualitybbs_norm:1.0,0.0,0.000276673510696,0.0166312059939;anchorscore_norm:22.77,0.0,0.998671033646,3.44199021224;docmatchctr_ac_norm:1.0,0.0,0.626703879424,0.305449466874;ranktermcount_norm:12.0,0.0,2.86797108691,2.78368933342;normpv_norm:16.25,0.0,7.17009809868,6.62492719563;bodyctr_ac_norm:1.0,0.0,0.439953177794,0.355612394137;isliststruct_norm:1.0,0.0,0.156266979202,0.363104060462;aol_manga_norm:1.0,0.0,0.00177798891625,0.0355449765911;ishq_norm:1.0,0.0,0.429469664551,0.494887825676;clickscore_norm:96.48,0.0,10.8087011202,21.7278063651;aol_novel_norm:1.0,0.0,0.0882286507452,0.273306540282;ishqwenda_norm:1.0,0.0,0.0130107538625,0.113320238633;aol_auto_norm:1.0,0.0,0.00806928249292,0.0812066831528;isbaike_norm:1.0,0.0,0.0178368479046,0.132350432839;title_hit_variance_ac_norm:13.5,0.0,2.22152339438,3.06906430988;ltr_is_home_page_ac_norm:1.0,0.0,0.0210917465111,0.143690013854;querylogscore_norm:104.8751,0.0,9.42731989334,21.3352545468;isnavi_norm:1.0,0.0,0.00137295398545,0.0370062121978;avgretrievetokenlen_norm:4.5,1.0,1.7612466682,0.521151608092;pv_ac_norm:2124.0,0.0,34.9155472311,236.810593993;distdnn_ac_norm:3.2385,-4.0,0.699698513104,1.20572795086;dmvscore_norm:1.0,0.0,0.627668415515,0.299463283355;titleprox1_ac_norm:1.0,0.0,0.932141585596,0.248428986904;lastdocctr_ac_norm:1.0,0.0,0.0629954374608,0.091464890415;statdomainauthorityscore_norm:2.6118,0.0,1.30830861813,0.551904535101;aol_stock_norm:1.0,0.0,0.00457484838162,0.0618599118707;ltr_is_news_tag_ac_norm:1.0,0.0,0.0673032747726,0.250544131569;at_avg_pos_ac_norm:32.0,0.0,9.3789948571,9.08319514763;aol_sport_norm:1.0,0.0,0.0013380677991,0.0319788611796;sitevaluescore_norm:5.0958,0.0,3.60569383335,0.966888437115;aol_astro_norm:1.0,0.0,0.00210132666181,0.0418775968908;anchorprox1_ac_norm:1.0,0.0,0.261448806264,0.439006949753;bodyprox1_ac_norm:1.0,0.0,0.73630260265,0.439157200074;acscore_norm:778.5,-828.34,330.539341254,241.307487134;proxscore_norm:1.0,0.0,0.695443412554,0.30868298267;isadultbad_norm:1.0,0.0,0.00143214946194,0.0378166454114;uv_ac_norm:657.01,0.0,11.3861552047,74.3249409876;adultlevel_norm:7.0,0.0,0.269948395883,1.30584247732;ortermcount_norm:3.0,0.0,0.692524565386,1.01609873948;anchorbm251_ac_norm:1.0,0.0,0.0120952051199,0.032361938903;domainauthorityscore_norm:1.0,0.0,0.110096781772,0.16214262833;bodybm251_ac_norm:1.0,0.0,0.0810357445239,0.142014640162;titlectr_ac_norm:1.0,0.0,0.601187240546,0.315193568857;aol_shopping_norm:1.0,0.0,0.045111787551,0.171762783236;pqqualityscore_norm:4.0978,0.0,2.72417800986,0.501080692769;lightgbmscore_norm:520.4832,160.03,397.362780801,57.1282457672;iswendaquery_norm:1.0,0.0,0.225057725392,0.417565950943;docctr_ac_norm:1.0,0.0,0.092911178021,0.128611680225;qa_score_ac_norm:3.462,0.173,1.09616696946,0.496056573408;titlebodybm25_ac_norm:2.8029,0.0,1.97414340694,0.681623715383;retrievetokensize_norm:11.0,1.0,4.91452891347,2.21721140098;isnavquery_norm:1.0,0.0,0.0144530003097,0.119288943316;iscontentstruct_norm:1.0,0.0,0.813821224899,0.389246667074;oldtextscore_norm:150.0,0.0,95.8950515473,44.9068029131;urldepth_norm:6.0,1.0,3.16884801887,0.894106337972;prhostrankscore_norm:8.0,0.0,4.65345937664,2.67064771473;isadult_norm:1.0,0.0,0.00527791126061,0.0724572692624;aol_baike_norm:1.0,0.0,0.12248580004,0.262878232703;new_ctr_cqr_omit_ac_norm:1.0,0.0,0.328145456973,0.300155313681;mixtsscore_norm:18.9713,-7.2357,4.39939041017,5.20643466719;ownclicklogscore_ac_norm:96.48,0.0,10.7634307245,21.6850150184;totalclickscore_ac_norm:205.3204,0.0,14.9466922182,34.5347816859;missqwt2_ac_norm:1.0,0.0,0.155278474611,0.303330048869;semantictokensize_norm:10.0,1.0,4.26096925966,2.09782108489;aol_traffic_norm:1.0,0.0,0.00265908873203,0.04522854529;istieba_norm:1.0,0.0,0.00488619981652,0.0697303789817;title_avg_pos_ac_norm:32.0,0.0,7.73096099966,7.57588335125;orderproxintitle_ac_norm:1.0,0.0,0.690340286364,0.31567061763;ltr_is_bbs_flag_ac_norm:1.0,0.0,0.0617556815148,0.240711243155;isnovelquery_norm:1.0,0.0,0.119997160141,0.32490105836;hostrankscore_norm:9743.8398,0.0,622.508459024,1885.42191327;qanchorscore_ac_norm:109.154,0.0,22.5270085214,28.2386886303;orderproxinbody_ac_norm:1.0,0.0,0.481001139601,0.377585865098;textscore_norm:150.0,0.0,80.1303063083,37.1149292202;elapseddays_norm:4983.0,0.0,1169.83527058,1043.38008861;authorityscore_ac_norm:58.4285,-24.5899,21.236586828,15.0098997142;bertl4mergemse_norm:5.1195,-2.0,1.52623773274,1.38643676874;bertl4mergecls_norm:5.3454,-17.1609,-9.29273969349,5.36417728827</makeup_feature_stat>
                  <makeup_weights>pv_norm:0.217608310691,normpv_norm:0.141948138176,isnavquery_norm:-0.0102525456574,isnovelquery_norm:-0.000974904410491,isgambling_norm:-0.368816950673,iswendaquery_norm:0.0640504114604,ispureentity_norm:-0.0204546261038,retrievetokensize_norm:0.0154745063033,semantictokensize_norm:0.0531167456954,avgretrievetokenlen_norm:0.116351807954,entitynum_norm:-0.0728324973651,strongentitynum_norm:0.011081996407,adultlevel_norm:-0.843999478266,nosug_norm:-0.169576022958,andtermcount_norm:-0.0227380127092,ortermcount_norm:-0.0381780510438,ranktermcount_norm:-0.0435330002134,aol_app_norm:-0.0699553079031,aol_astro_norm:-0.172851727948,aol_auto_norm:0.0313035093842,aol_baike_norm:-0.0138089031787,aol_dict_norm:-0.0338582216273,aol_fanyi_norm:0.290080698051,aol_gaokao_norm:-0.314514744076,aol_lottery_norm:-0.147917544614,aol_manga_norm:-0.0569998884718,aol_map_norm:-0.11954345805,aol_novel_norm:0.528178039962,aol_pic_norm:-0.0468092434837,aol_shopping_norm:0.00246423826555,aol_song_norm:0.100474738979,aol_sport_norm:-0.0771152504477,aol_stock_norm:0.0555699974163,aol_traffic_norm:0.0127944098566,aol_travel_norm:-0.0512471451579,aol_video_norm:-0.0031523602955,aol_weather_norm:0.351167824595,acscore_norm:0.0079036548558,lightgbmscore_norm:0.911067484677,textscore_norm:0.192784623238,oldtextscore_norm:0.238166618732,ctrscore_norm:-0.15096663999,dmvscore_norm:-0.190106975874,proxscore_norm:-0.0761392104191,ngramscore_norm:-0.0635847522556,mixacscore_norm:0.241399542998,mixtsscore_norm:0.0839027678237,authorityscore_norm:0.0991997104627,mlauthorityscore_norm:0.000964274455005,domainauthorityscore_norm:0.0242570160094,pqqualityscore_norm:0.228929900258,hostrankscore_norm:-0.0809826078897,prhostrankscore_norm:-0.0628623563217,sitevaluescore_norm:0.0508706359219,aolauthscore_norm:0.0285760191742,cmauthscore_norm:-0.0120028335893,timeincontentscore_norm:-1.80033819728,elapseddays_norm:1.70275284923,timesensitivescore_norm:-0.0121425182568,urldepth_norm:-0.0157073995887,mainbodylength_norm:0.0127859730721,clickscore_norm:0.177295548103,querylogscore_norm:-0.0321435960161,isnavi_norm:1.36600912578,isvideo_norm:0.12913513244,isnews_norm:-0.0770988037844,isadult_norm:0.153974025797,isbaike_norm:0.0027429350576,istieba_norm:0.00583835822803,isspam_norm:-0.000907972097344,ishq_norm:-0.0362002780107,ishqwenda_norm:-0.0408873211615,isadultbad_norm:-0.164615899238,ishqadult_norm:-0.0903338289822,isqualitybbs_norm:-0.00375867784611,isqualitytieba_norm:-0.000286152016786,isliststruct_norm:-0.192699497257,iscontentstruct_norm:-0.159968838774,distdnn_ac_norm:-0.197565547506,contdnn_ac_norm:-0.0699093201227,isadultquery_ac_norm:0.747086008823,qa_score_ac_norm:0.0971111885206,pv_ac_norm:0.140861500334,uv_ac_norm:-0.0625541160097,anchorscore_ac_norm:0.0642281749762,authorityscore_ac_norm:-0.117718540331,qanchorscore_ac_norm:-0.110005751573,querypv_ac_norm:-0.0373524716647,docclicks_ac_norm:-0.0573508067421,docctr_ac_norm:-0.258324632715,lastdocctr_ac_norm:0.120228460278,atctr_ac_norm:0.163065858778,bodyctr_ac_norm:0.0331672215741,anchorctr_ac_norm:-0.0510783671378,atprox1_ac_norm:0.051904995961,titleprox1_ac_norm:-0.228420711887,bodyprox1_ac_norm:0.0154053323478,bodybm251_ac_norm:-0.0155913214299,anchorbm251_ac_norm:-0.0212603742565,omit4ratio_ac_norm:0.0173809174715,semanticmiss_ac_norm:-0.0795045677822,docmatchctr_ac_norm:-0.261859737959,docmatchcqr_ac_norm:-0.212074515024,ctrcqromit_ac_norm:0.360848124352,qwtrcos_ac_norm:-0.115591435634,missqwt2_ac_norm:-0.0407995981701,titlebodybm25_ac_norm:-0.0749638447346,orderproxinbody_ac_norm:-0.0412331933792,orderproxinanchor_ac_norm:-0.0178818702203,at_start_pos_ac_norm:-0.011665184844,at_avg_pos_ac_norm:0.230551263778,at_hit_variance_ac_norm:0.0337008771413,title_start_pos_ac_norm:0.000947357911332,title_avg_pos_ac_norm:-0.10546115487,title_hit_variance_ac_norm:-0.0448311619297,new_omit_ratio_ac_norm:-0.103656256713,new_ctr_ac_norm:0.0528679416742,new_cqr_ac_norm:-0.0804334723098,new_cqr2_ac_norm:0.0599606655262,new_ctr_cqr_omit_ac_norm:0.105705450096,ltr_is_news_tag_ac_norm:0.00481365945563,ltr_title_miss_loc_ac_norm:0.0485676046448,ltr_body_miss_loc_ac_norm:-0.00809118364537,ltr_is_home_page_ac_norm:0.0447792509482,ltr_is_bbs_flag_ac_norm:0.060226655252,distcnn_ac_norm:-0.590527159921,bert_mse_norm:0.467469135784,bert_cls_norm:0.00251567159449,bertl4acscore_norm:0.4417782893,bertl4mergecls_norm:0.605660735504,bertl4mergemse_norm:1.42535501782</makeup_weights>  
                  <makeup_bias>-1.01518151</makeup_bias> 
                  <makeup_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l4_rank</makeup_service_spec_node>
                  <!-- yg284091 add for faiss with qtc -->
                  <makeup_with_qtc_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l4_rank_add_qtc</makeup_with_qtc_service_spec_node>
                  <makeup_f133_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l4_rank_fea133</makeup_f133_service_spec_node>
                  <!-- nayu.zq vec l4 qtc v2 -->
                  <makeup_with_qtc_v2_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vec_qtc_v2_exam_v0</makeup_with_qtc_v2_service_spec_node>
                  <makeup_l4_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l4_rank_v1_ss</makeup_l4_service_spec_node>
                  <makeup_l5_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l5_rank_v1</makeup_l5_service_spec_node>
                  <makeup_l4_v2_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l4_rank_v2</makeup_l4_v2_service_spec_node>
                  <makeup_l5_v2_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l5_rank_v2</makeup_l5_v2_service_spec_node>
                  <makeup_l4_v3_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l4_rank_v37_1</makeup_l4_v3_service_spec_node>
                  <makeup_l5_v3_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l5_rank_v37_1</makeup_l5_v3_service_spec_node>
                  <makeup_model_type>LIGHTGBM</makeup_model_type>

                  <!-- faiss_external_merge -->
                  <merge_feature_stat>nosug_norm:1.0,0.0,0.0280017962109,0.164939252886;ishqadult_norm:1.0,0.0,0.00197902069899,0.0444414908099;bert_cls_norm:-68.2731,-75.572,-72.7316925364,1.43694367561;mainbodylength_norm:5010.0,0.0,656.023147084,999.157793664;lastdocclicks_ac_norm:1023.0,0.0,48.9891448688,177.656578677;aol_travel_norm:1.0,0.0,0.0133007495314,0.0980868358175;timeincontentscore_norm:146.0,-16.0,21.4461530615,33.0707649918;at_hit_variance_ac_norm:10.0,0.0,2.13147351034,3.0982925125;strongentitynum_norm:4.0,0.0,0.753346567698,0.893689991762;qwtrcos_ac_norm:1.0,0.0,0.716582286622,0.258326479761;isnews_norm:1.0,0.0,0.869884981003,0.336423867854;contdnn_ac_norm:1.0,0.0,0.365304737002,0.269788956971;new_ctr_cqr_ac_norm:1.0,0.0,0.39132186207,0.290004247017;anchorctr_ac_norm:1.0,0.0,0.173367324507,0.325110884041;bertl4acscore_norm:83.7848,46.5534,70.800129323,5.74720740983;authorityscore_norm:58.4978,0.0,21.9464263288,14.0702585222;entitynum_norm:4.0,0.0,0.788533758111,0.916526473101;v28_score_norm:0.0,-10.1994,-5.02257174846,1.86314910853;aol_fanyi_norm:1.0,0.0,3.77142502995e-07,2.58897288768e-06;at_start_pos_ac_norm:32.0,0.0,5.50616999455,9.65926168257;pv_norm:179634.0,0,5255.97189762,22459.7343835;atprox1_ac_norm:1.0,0.0,0.883115698981,0.318785962052;ctrcqr_ac_norm:1.0,0.0,0.551456920609,0.314161862312;ctrscore_norm:1.0,0.0,0.623852636773,0.314644192457;andtermcount_norm:6.0,0.0,2.15760826002,1.3826859275;semanticmiss_ac_norm:1.0,0.0,0.795537541577,0.276868220343;isvideo_norm:1.0,0.0,0.00398537270834,0.0630038906358;mlauthorityscore_norm:2.7814,-6.4294,-2.9172261518,1.50961148728;aol_app_norm:1.0,0.0,0.0870242845684,0.241130103078;anchorscore_ac_norm:22.7301,0.0,0.992626840999,3.43192412658;aol_pic_norm:1.0,0.0,0.071073986288,0.221740637151;new_cqr2_ac_norm:1.0,0.0,0.4817999602,0.287304734767;isadultquery_ac_norm:1.0,0.0,0.0425063699049,0.201664007182;aol_weather_norm:1.0,0.0,0.00637600334099,0.0781339477283;querypv_ac_norm:174424.0,0.0,5122.73307557,21821.2732814;aol_video_norm:1.0,0.0,0.132490301609,0.283412825537;docmatchcqr_ac_norm:1.0,0.0,0.792957818578,0.263150564319;atctr_ac_norm:1.0,0.0,0.570201789179,0.331123258413;bert_mse_norm:4.3269,-6.9412,-1.52118417644,2.36641229063;aol_song_norm:1.0,0.0,0.0133609445307,0.0936960029414;new_cqr_ac_norm:1.0,0.0,0.525314904949,0.272129423078;omit4ratio_ac_norm:1.0,0.0,0.806740688379,0.293187545235;cmauthscore_norm:3019999.0,0.0,245393.419603,616936.377162;ctrcqromit_ac_norm:1.0,0.0,0.52595493265,0.324105779728;ngramscore_norm:653.06,0.0,60.2546350143,158.24851744;aol_lottery_norm:1.0,0.0,0.00133663268416,0.0341276907507;new_ctr_ac_norm:1.0,0.0,0.607187275478,0.30617844157;rank_norm:20.0,0,9.49828738869,6.57622762764;ltr_body_miss_loc_ac_norm:5.0,0.0,3.09320153792,1.67004186188;orderproxinanchor_ac_norm:1.0,0.0,0.0221972707281,0.142556646625;aol_dict_norm:1.0,0.0,0.012290382776,0.101529907797;ltr_title_miss_loc_ac_norm:5.0,0.0,3.70634989658,1.06651150887;new_omit_ratio_ac_norm:1.0,0.0,0.665202641251,0.341046580175;aol_gaokao_norm:1.0,0.0,0.003164415434,0.0450656649557;aolauthscore_norm:4.1891,-7.5738,-1.01563936694,1.45505539747;mixacscore_norm:9.26,-7.19,2.0984462339,2.69169320381;timesensitivescore_norm:3.145,-3.0,0.675671175019,1.76078035196;distcnn_ac_norm:3.7432,-4.0,0.878320198575,1.3409085412;title_start_pos_ac_norm:32.0,0.0,4.25013450206,7.66735022958;aol_map_norm:1.0,0.0,0.0962192703954,0.253539117214;isqualitytieba_norm:1.0,0.0,0.035840505002,0.185892359964;isspam_norm:1.0,0.0,0.00528592023066,0.0725119316736;ispureentity_norm:1.0,0.0,0.0611684008936,0.239542567578;isgambling_norm:1.0,0.0,0.00225158671218,0.0473878594721;docclicks_ac_norm:1023.0,0.0,59.9686230786,196.547564716;isqualitybbs_norm:1.0,0.0,0.000276673510696,0.0166312059939;anchorscore_norm:22.77,0.0,0.998671033646,3.44199021224;docmatchctr_ac_norm:1.0,0.0,0.626703879424,0.305449466874;ranktermcount_norm:12.0,0.0,2.86797108691,2.78368933342;normpv_norm:16.25,0.0,7.17009809868,6.62492719563;bodyctr_ac_norm:1.0,0.0,0.439953177794,0.355612394137;isliststruct_norm:1.0,0.0,0.156266979202,0.363104060462;aol_manga_norm:1.0,0.0,0.00177798891625,0.0355449765911;ishq_norm:1.0,0.0,0.429469664551,0.494887825676;clickscore_norm:96.48,0.0,10.8087011202,21.7278063651;aol_novel_norm:1.0,0.0,0.0882286507452,0.273306540282;ishqwenda_norm:1.0,0.0,0.0130107538625,0.113320238633;aol_auto_norm:1.0,0.0,0.00806928249292,0.0812066831528;isbaike_norm:1.0,0.0,0.0178368479046,0.132350432839;title_hit_variance_ac_norm:13.5,0.0,2.22152339438,3.06906430988;ltr_is_home_page_ac_norm:1.0,0.0,0.0210917465111,0.143690013854;querylogscore_norm:104.8751,0.0,9.42731989334,21.3352545468;isnavi_norm:1.0,0.0,0.00137295398545,0.0370062121978;avgretrievetokenlen_norm:4.5,1.0,1.7612466682,0.521151608092;pv_ac_norm:2124.0,0.0,34.9155472311,236.810593993;distdnn_ac_norm:3.2385,-4.0,0.699698513104,1.20572795086;dmvscore_norm:1.0,0.0,0.627668415515,0.299463283355;titleprox1_ac_norm:1.0,0.0,0.932141585596,0.248428986904;lastdocctr_ac_norm:1.0,0.0,0.0629954374608,0.091464890415;statdomainauthorityscore_norm:2.6118,0.0,1.30830861813,0.551904535101;aol_stock_norm:1.0,0.0,0.00457484838162,0.0618599118707;ltr_is_news_tag_ac_norm:1.0,0.0,0.0673032747726,0.250544131569;at_avg_pos_ac_norm:32.0,0.0,9.3789948571,9.08319514763;aol_sport_norm:1.0,0.0,0.0013380677991,0.0319788611796;sitevaluescore_norm:5.0958,0.0,3.60569383335,0.966888437115;aol_astro_norm:1.0,0.0,0.00210132666181,0.0418775968908;anchorprox1_ac_norm:1.0,0.0,0.261448806264,0.439006949753;bodyprox1_ac_norm:1.0,0.0,0.73630260265,0.439157200074;acscore_norm:778.5,-828.34,330.539341254,241.307487134;proxscore_norm:1.0,0.0,0.695443412554,0.30868298267;isadultbad_norm:1.0,0.0,0.00143214946194,0.0378166454114;uv_ac_norm:657.01,0.0,11.3861552047,74.3249409876;adultlevel_norm:7.0,0.0,0.269948395883,1.30584247732;ortermcount_norm:3.0,0.0,0.692524565386,1.01609873948;anchorbm251_ac_norm:1.0,0.0,0.0120952051199,0.032361938903;domainauthorityscore_norm:1.0,0.0,0.110096781772,0.16214262833;bodybm251_ac_norm:1.0,0.0,0.0810357445239,0.142014640162;titlectr_ac_norm:1.0,0.0,0.601187240546,0.315193568857;aol_shopping_norm:1.0,0.0,0.045111787551,0.171762783236;pqqualityscore_norm:4.0978,0.0,2.72417800986,0.501080692769;lightgbmscore_norm:520.4832,160.03,397.362780801,57.1282457672;iswendaquery_norm:1.0,0.0,0.225057725392,0.417565950943;docctr_ac_norm:1.0,0.0,0.092911178021,0.128611680225;qa_score_ac_norm:3.462,0.173,1.09616696946,0.496056573408;titlebodybm25_ac_norm:2.8029,0.0,1.97414340694,0.681623715383;retrievetokensize_norm:11.0,1.0,4.91452891347,2.21721140098;isnavquery_norm:1.0,0.0,0.0144530003097,0.119288943316;iscontentstruct_norm:1.0,0.0,0.813821224899,0.389246667074;oldtextscore_norm:150.0,0.0,95.8950515473,44.9068029131;urldepth_norm:6.0,1.0,3.16884801887,0.894106337972;prhostrankscore_norm:8.0,0.0,4.65345937664,2.67064771473;isadult_norm:1.0,0.0,0.00527791126061,0.0724572692624;aol_baike_norm:1.0,0.0,0.12248580004,0.262878232703;new_ctr_cqr_omit_ac_norm:1.0,0.0,0.328145456973,0.300155313681;mixtsscore_norm:18.9713,-7.2357,4.39939041017,5.20643466719;ownclicklogscore_ac_norm:96.48,0.0,10.7634307245,21.6850150184;totalclickscore_ac_norm:205.3204,0.0,14.9466922182,34.5347816859;missqwt2_ac_norm:1.0,0.0,0.155278474611,0.303330048869;semantictokensize_norm:10.0,1.0,4.26096925966,2.09782108489;aol_traffic_norm:1.0,0.0,0.00265908873203,0.04522854529;istieba_norm:1.0,0.0,0.00488619981652,0.0697303789817;title_avg_pos_ac_norm:32.0,0.0,7.73096099966,7.57588335125;orderproxintitle_ac_norm:1.0,0.0,0.690340286364,0.31567061763;ltr_is_bbs_flag_ac_norm:1.0,0.0,0.0617556815148,0.240711243155;isnovelquery_norm:1.0,0.0,0.119997160141,0.32490105836;hostrankscore_norm:9743.8398,0.0,622.508459024,1885.42191327;qanchorscore_ac_norm:109.154,0.0,22.5270085214,28.2386886303;orderproxinbody_ac_norm:1.0,0.0,0.481001139601,0.377585865098;textscore_norm:150.0,0.0,80.1303063083,37.1149292202;elapseddays_norm:4983.0,0.0,1169.83527058,1043.38008861;authorityscore_ac_norm:58.4285,-24.5899,21.236586828,15.0098997142;bertl4mergemse_norm:5.1195,-2.0,1.52623773274,1.38643676874;bertl4mergecls_norm:5.3454,-17.1609,-9.29273969349,5.36417728827</merge_feature_stat>
                  <merge_weights>pv_norm:0.217608310691,normpv_norm:0.141948138176,isnavquery_norm:-0.0102525456574,isnovelquery_norm:-0.000974904410491,isgambling_norm:-0.368816950673,iswendaquery_norm:0.0640504114604,ispureentity_norm:-0.0204546261038,retrievetokensize_norm:0.0154745063033,semantictokensize_norm:0.0531167456954,avgretrievetokenlen_norm:0.116351807954,entitynum_norm:-0.0728324973651,strongentitynum_norm:0.011081996407,adultlevel_norm:-0.843999478266,nosug_norm:-0.169576022958,andtermcount_norm:-0.0227380127092,ortermcount_norm:-0.0381780510438,ranktermcount_norm:-0.0435330002134,aol_app_norm:-0.0699553079031,aol_astro_norm:-0.172851727948,aol_auto_norm:0.0313035093842,aol_baike_norm:-0.0138089031787,aol_dict_norm:-0.0338582216273,aol_fanyi_norm:0.290080698051,aol_gaokao_norm:-0.314514744076,aol_lottery_norm:-0.147917544614,aol_manga_norm:-0.0569998884718,aol_map_norm:-0.11954345805,aol_novel_norm:0.528178039962,aol_pic_norm:-0.0468092434837,aol_shopping_norm:0.00246423826555,aol_song_norm:0.100474738979,aol_sport_norm:-0.0771152504477,aol_stock_norm:0.0555699974163,aol_traffic_norm:0.0127944098566,aol_travel_norm:-0.0512471451579,aol_video_norm:-0.0031523602955,aol_weather_norm:0.351167824595,acscore_norm:0.0079036548558,lightgbmscore_norm:0.911067484677,textscore_norm:0.192784623238,oldtextscore_norm:0.238166618732,ctrscore_norm:-0.15096663999,dmvscore_norm:-0.190106975874,proxscore_norm:-0.0761392104191,ngramscore_norm:-0.0635847522556,mixacscore_norm:0.241399542998,mixtsscore_norm:0.0839027678237,authorityscore_norm:0.0991997104627,mlauthorityscore_norm:0.000964274455005,domainauthorityscore_norm:0.0242570160094,pqqualityscore_norm:0.228929900258,hostrankscore_norm:-0.0809826078897,prhostrankscore_norm:-0.0628623563217,sitevaluescore_norm:0.0508706359219,aolauthscore_norm:0.0285760191742,cmauthscore_norm:-0.0120028335893,timeincontentscore_norm:-1.80033819728,elapseddays_norm:1.70275284923,timesensitivescore_norm:-0.0121425182568,urldepth_norm:-0.0157073995887,mainbodylength_norm:0.0127859730721,clickscore_norm:0.177295548103,querylogscore_norm:-0.0321435960161,isnavi_norm:1.36600912578,isvideo_norm:0.12913513244,isnews_norm:-0.0770988037844,isadult_norm:0.153974025797,isbaike_norm:0.0027429350576,istieba_norm:0.00583835822803,isspam_norm:-0.000907972097344,ishq_norm:-0.0362002780107,ishqwenda_norm:-0.0408873211615,isadultbad_norm:-0.164615899238,ishqadult_norm:-0.0903338289822,isqualitybbs_norm:-0.00375867784611,isqualitytieba_norm:-0.000286152016786,isliststruct_norm:-0.192699497257,iscontentstruct_norm:-0.159968838774,distdnn_ac_norm:-0.197565547506,contdnn_ac_norm:-0.0699093201227,isadultquery_ac_norm:0.747086008823,qa_score_ac_norm:0.0971111885206,pv_ac_norm:0.140861500334,uv_ac_norm:-0.0625541160097,anchorscore_ac_norm:0.0642281749762,authorityscore_ac_norm:-0.117718540331,qanchorscore_ac_norm:-0.110005751573,querypv_ac_norm:-0.0373524716647,docclicks_ac_norm:-0.0573508067421,docctr_ac_norm:-0.258324632715,lastdocctr_ac_norm:0.120228460278,atctr_ac_norm:0.163065858778,bodyctr_ac_norm:0.0331672215741,anchorctr_ac_norm:-0.0510783671378,atprox1_ac_norm:0.051904995961,titleprox1_ac_norm:-0.228420711887,bodyprox1_ac_norm:0.0154053323478,bodybm251_ac_norm:-0.0155913214299,anchorbm251_ac_norm:-0.0212603742565,omit4ratio_ac_norm:0.0173809174715,semanticmiss_ac_norm:-0.0795045677822,docmatchctr_ac_norm:-0.261859737959,docmatchcqr_ac_norm:-0.212074515024,ctrcqromit_ac_norm:0.360848124352,qwtrcos_ac_norm:-0.115591435634,missqwt2_ac_norm:-0.0407995981701,titlebodybm25_ac_norm:-0.0749638447346,orderproxinbody_ac_norm:-0.0412331933792,orderproxinanchor_ac_norm:-0.0178818702203,at_start_pos_ac_norm:-0.011665184844,at_avg_pos_ac_norm:0.230551263778,at_hit_variance_ac_norm:0.0337008771413,title_start_pos_ac_norm:0.000947357911332,title_avg_pos_ac_norm:-0.10546115487,title_hit_variance_ac_norm:-0.0448311619297,new_omit_ratio_ac_norm:-0.103656256713,new_ctr_ac_norm:0.0528679416742,new_cqr_ac_norm:-0.0804334723098,new_cqr2_ac_norm:0.0599606655262,new_ctr_cqr_omit_ac_norm:0.105705450096,ltr_is_news_tag_ac_norm:0.00481365945563,ltr_title_miss_loc_ac_norm:0.0485676046448,ltr_body_miss_loc_ac_norm:-0.00809118364537,ltr_is_home_page_ac_norm:0.0447792509482,ltr_is_bbs_flag_ac_norm:0.060226655252,distcnn_ac_norm:-0.590527159921,bert_mse_norm:0.467469135784,bert_cls_norm:0.00251567159449,bertl4acscore_norm:0.4417782893,bertl4mergecls_norm:0.605660735504,bertl4mergemse_norm:1.42535501782</merge_weights>  
                  <merge_bias>-1.01518151</merge_bias> 
                  <merge_service_spec_node>ufs_agg_adjuster_model/?_chain=ufs_app&amp;scene=adjuster_vector&amp;chain=vector_l4_rank</merge_service_spec_node>
                  <merge_model_type>LIGHTGBM</merge_model_type>
                </params>
            </processor>

            <!-- dump -->
            <processor name="dump_qp_processor" module_name="agg_web2_module" class_name="QueryAnalysisProcessor">
                <params>
                    <multi_exp_node>multi_exp_bert</multi_exp_node>
                    <multi_exp_v2_node>multi_exp_bert_v2</multi_exp_v2_node>
                    <dct_imp_node>dct_imp</dct_imp_node>
                    <acnn_cache_node>acnn_cache</acnn_cache_node>
                    <acnn_feature_node>acnn_feature_bert/?_chain=autobatch_bert_l4_nn_782_query</acnn_feature_node>
                    <acnn_v2_feature_node>acnn_feature_bert/?_chain=l3_colbert_query</acnn_v2_feature_node>
                    <qtcnn_feature_node>gig/qc_feature_bert/qtc_repre_minibert_model_query_embed?_chain=L3_QTC_REPRE_V18_QUERY</qtcnn_feature_node>
                    <qtcnn_v2_feature_node>gig/qc_v2_feature_bert/qtc_repre_12Lbert_v30_query_embed?_chain=l3_v2_qtc_repre_v30_avg_query</qtcnn_v2_feature_node>
                    <qtcnn_cache_node>qc_score_cache</qtcnn_cache_node>
                    <!-- L2NN params -->
                    <l2nn_feature_node>gig/qc_v2_feature_bert/qtc_l2_v6_s128_e128_query?_chain=l2_v6_s128_e128_kd_query</l2nn_feature_node>
                    <l2nn_v2_feature_node>gig/qc_v2_feature_bert/qtc_l2_v32_s128_e128_query?_chain=l2_v32_s128_e128_kd_query</l2nn_v2_feature_node>
                    <l2nn_cache_node>qc_score_cache</l2nn_cache_node>
                    <!-- L2NN params end -->
                    <qp_node>gig/qp/web</qp_node>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};bigindex:web_big_index:${kvParam_bigindex_cluster};rtindex:web_rt_index:${kvParam_rtindex_cluster},${kvParam_fastindex_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster};mainindex_pk:web_small_index:${kvParam_galaxy_cluster}
                    </channels_clusters>
                    <kv_item>fr:xxx,ip:0.0.0.0,st:wap,tl:54,sl:100,adult_demote:255,sp:on,mode:xxx,or_logic:v2</kv_item>
                    <config_item>start:0,hit:,navigationalQuery:on,filtermode:11</config_item>
                </params>
            </processor>
            <processor name="dump_querylog_processor" module_name="agg_web2_module" class_name="QueryLogProcessor">
                <params>
                    <baidu_querylog_node>baidu_querylog</baidu_querylog_node>
                    <tt_querylog_node>tt_querylog</tt_querylog_node>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};bigindex:web_big_index:${kvParam_bigindex_cluster};rtindex:web_rt_index:${kvParam_rtindex_cluster},${kvParam_fastindex_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster};mainindex_pk:web_small_index:${kvParam_galaxy_cluster}
                    </channels_clusters>
                    <insert_model_v2_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v2</insert_model_v2_node>
                    <insert_model_v3_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v3</insert_model_v3_node>
                    <insert_model_v4_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v4</insert_model_v4_node>
                    <insert_model_v5_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v5</insert_model_v5_node>
                    <insert_model_v6_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v6</insert_model_v6_node>
                    <insert_model_v7_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v7</insert_model_v7_node>
                    <clickG_score_tf_node>clickG_score_node/clickG_predict?_chain=clickG_v1_117</clickG_score_tf_node>
                    <qlog_score_tf_node>qlog_score_node/qlog_score?_chain=tf</qlog_score_tf_node>
                    <ha3_qlog_node>ha3_qlog</ha3_qlog_node>
                    <ha3_qlog_v2_node>ha3_qlog_v2</ha3_qlog_v2_node>
                    <cluster_name>qlog</cluster_name>
                    <config_item>format:protobuf,start:0,hit:10,query_pv_threshold:10,query_doc_pv_threshold:10</config_item>
                </params>
            </processor>
            <processor name="dump_index_processor" module_name="agg_web2_module" class_name="DumpIndexProcessor">
                <params>
                    <!-- gen qrs clause params -->
                    <index_list>mainindex_pk</index_list>
                    <common>
                        kv=topnAcFeatureThrehold:500#distinct=dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2#config=format:protobuf,no_summary:yes,proto_format_option:pb_matchdoc_format,timeout:1000,result_compress:snappy
                    </common>
                    <mainindex_pk>
                        config=searcher_return_hits:100,hit:500,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL
                    </mainindex_pk>
                    <ranker_bucket>
                        bucket_2=mainindex:BucketProfile,qaindex:BucketProfile,bigindex:BucketProfile#bucket_3=mainindex:DefaultProfile,qaindex:DefaultProfile,bigindex:DefaultProfile
                    </ranker_bucket>
                    <ranker_bucket_key>ranker</ranker_bucket_key>
                    <ranker_interleaving_key>ltrmodelexp</ranker_interleaving_key>
                    <ranker_interleaving>ltrmodelexp=mainindex:BucketProfile,qaindex:BucketProfile</ranker_interleaving>
                    <interleaving_qrs_config>${kvParam_interleaving_qrs_config}</interleaving_qrs_config>
                    <abtest_qrs_config>${kvParam_abtest_qrs_config}</abtest_qrs_config>
                    <l1lr_ranker_profile/>

                    <!-- request index params -->
                    <dump_index>mainindex_pk</dump_index>

                    <!-- fetch summary params -->
                    <max_start_hit>50</max_start_hit>
                    <fetch_count>50</fetch_count>
                    <rule>
                        phase1_node:mainindex_pk,phase2_node:mainindex_summary_pk,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK
                    </rule>
                    <cluster_replace>galaxy:galaxy2</cluster_replace>

                    <!-- senior rerank -->
                    <brain_agg_qt_semantic_node>gig/qt_semantic/ts</brain_agg_qt_semantic_node>
                    <aliws_forward_index_node>http/aliws_forward_index/</aliws_forward_index_node>

                    <qtc_feature_node_online_t4>gig/qc_feature_bert</qtc_feature_node_online_t4>
                    <qtc_feature_node_online_p100>gig/qc_feature_bert_p100</qtc_feature_node_online_p100>
                    <qtc_feature_node_online_2080>gig/qc_feature_bert_2080</qtc_feature_node_online_2080>

                    <qc_score_cache_node>qc_score_cache</qc_score_cache_node>

                    <agg_feature_node>brain_agg_feature</agg_feature_node>
                    <agg_feature_ufs_node>agg_feature_ufs</agg_feature_ufs_node>

                    <distinct_param>dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2</distinct_param>
                    <sc_aol_node>gig/sc_aol</sc_aol_node>
                   
                    <!-- senior rerank exec --> 
                    <brain_agg_model_QTC_EXAM_ONLINE_node>brain_agg_model_ufs</brain_agg_model_QTC_EXAM_ONLINE_node>
                
                    <!-- dump error cutoff -->
                    <context_failed_check_nodes/>
                    <subcontext_failed_check_nodes/>
                </params>
            </processor>

            <!-- agg_web_arrival-->
            <processor name="arrival_query_analysis_processor" module_name="agg_web2_module" class_name="QueryAnalysisProcessor">
                <params>
                    <multi_exp_node>multi_exp_bert</multi_exp_node>
                    <multi_exp_v2_node>multi_exp_bert_v2</multi_exp_v2_node>
                    <dct_imp_node>dct_imp</dct_imp_node>
                    <acnn_cache_node>acnn_cache</acnn_cache_node>
                    <acnn_feature_node>acnn_feature_bert/?_chain=autobatch_bert_l4_nn_782_query</acnn_feature_node>
                    <acnn_v2_feature_node>acnn_feature_bert/?_chain=l3_colbert_query</acnn_v2_feature_node>
                    <qtcnn_feature_node>gig/qc_feature_bert/qtc_repre_minibert_model_query_embed/?_chain=L3_QTC_REPRE_V18_QUERY</qtcnn_feature_node>
                    <qtcnn_v2_feature_node>gig/qc_v2_feature_bert/qtc_repre_12Lbert_v30_query_embed?_chain=l3_v2_qtc_repre_v30_avg_query</qtcnn_v2_feature_node>
                    <qtcnn_cache_node>qc_score_cache</qtcnn_cache_node>
                    <!-- L2NN params -->
                    <l2nn_feature_node>gig/qc_v2_feature_bert/qtc_l2_v6_s128_e128_query?_chain=l2_v6_s128_e128_kd_query</l2nn_feature_node>
                    <l2nn_v2_feature_node>gig/qc_v2_feature_bert/qtc_l2_v32_s128_e128_query?_chain=l2_v32_s128_e128_kd_query</l2nn_v2_feature_node>
                    <l2nn_cache_node>qc_score_cache</l2nn_cache_node>
                    <!-- L2NN params end -->
                    <qp_node>gig/qp/web</qp_node>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster}
                    </channels_clusters>
                    <kv_item>fr:xxx,ip:0.0.0.0,st:wap,tl:54,sl:100,adult_demote:255,sp:on,mode:xxx,or_logic:v2</kv_item>
                    <config_item>start:0,hit:,navigationalQuery:on,filtermode:11</config_item>
                </params>
            </processor>
            <processor name="arrival_querylog_processor" module_name="agg_web2_module" class_name="QueryLogProcessor">
                <params>
                    <baidu_querylog_node>baidu_querylog</baidu_querylog_node>
                    <tt_querylog_node>tt_querylog</tt_querylog_node>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster}
                    </channels_clusters>
                    <insert_model_v2_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v2</insert_model_v2_node>
                    <insert_model_v3_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v3</insert_model_v3_node>
                    <insert_model_v4_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v4</insert_model_v4_node>
                    <insert_model_v5_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v5</insert_model_v5_node>
                    <insert_model_v6_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v6</insert_model_v6_node>
                    <insert_model_v7_node>querylog_model_ufs/?_chain=ufs_app&amp;scene=querylog_makeup&amp;chain=insert_model_v7</insert_model_v7_node>
                    <clickG_score_tf_node>clickG_score_node/clickG_predict?_chain=clickG_v1_117</clickG_score_tf_node>
                    <ha3_qlog_node>ha3_qlog</ha3_qlog_node>
                    <ha3_qlog_v2_node>ha3_qlog_v2</ha3_qlog_v2_node>
                    <cluster_name>qlog</cluster_name>
                    <config_item>format:protobuf,start:0,hit:10,query_pv_threshold:10,query_doc_pv_threshold:10</config_item>
                </params>
            </processor>
            <processor name="arrival_processor" module_name="agg_web2_module" class_name="ArrivalProcessor">
                <params>
                    <!-- basic gen qrs clause -->
                    <index_list>mainindex,qaindex</index_list>
                    <common>
                        distinct=dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2#config=rank_size:100000,rerank_size:1,searcher_return_hits:10,hit:200,start:0,format:protobuf,no_summary:yes,proto_format_option:pb_matchdoc_format,timeout:1000,result_compress:snappy
                    </common>
                    <mainindex>
                        rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,IndustryStat,PQQualityScore,V_SECURITY_LEVEL
                    </mainindex>
                    <qaindex>
                        distinct=dist_key:V_HOST_HASH,dist_count:10;dist_key:V_HOST_HASH,dist_count:6#kv=sc_summary_conf:1`1280`1`1`1#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,IndustryStat,PQQualityScore,V_SECURITY_LEVEL
                    </qaindex>
                    <summary_config>2`15000`1&lt;em&gt;`1&lt;/em&gt;`1....</summary_config>
                    <l1lr_ranker_profile/>
                    
                    <!-- cache -->
                    <cache_max_start_hit>50</cache_max_start_hit>
                    <cache_node>main_cache</cache_node>
                    <cache_compress_type>snappy</cache_compress_type>

                    <!-- read cache -->
                    <cache_key_channel>mainindex</cache_key_channel>
                    <cache_magic_number>agg_web2_arrival_12345</cache_magic_number>
                    
                    <!-- request index -->

                    <!-- rerank -->

                    <!-- fetch summary -->
                    <max_start_hit>10</max_start_hit>
                    <fetch_count>20</fetch_count>
                    <rule>
                        phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK
                    </rule>

                    <!-- update cache -->
                    <cache_index_list>mainindex,qaindex,rtindex</cache_index_list>
                    <cache_effective_time>7200</cache_effective_time>
                    
                    <!-- internal merge  -->

                    <!-- external merge -->
                </params>
            </processor>
            <processor name="arrival_dedup_processor" module_name="agg_web2_module" class_name="ArrivalDedupProcessor">
                <params>
                    <max_start_hit>50</max_start_hit>
                    <exact_dedup>dedup_field:RawUrl,</exact_dedup>
                </params>
            </processor>
            <processor name="arrival_log_processor" module_name="agg_web2_module" class_name="LogProcessor">
                <params>
                    <log_actor_set>display</log_actor_set>
                </params>
            </processor>

            <!-- agg_web_arrival_summary -->
            <processor name="arrival_summary_processor" module_name="agg_web2_module" class_name="ArrivalProcessor">
                <params>
                    <!-- basic gen qrs clause -->
                    <index_list>mainindex,qaindex</index_list>
                    <common>
                        distinct=dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2#config=rank_size:50000,rerank_size:20,searcher_return_hits:10,hit:60,start:0,format:protobuf,no_summary:yes,proto_format_option:pb_matchdoc_format,timeout:10000,result_compress:snappy
                    </common>
                    <mainindex>
                        rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,IndustryStat,PQQualityScore,V_SECURITY_LEVEL
                    </mainindex>
                    <qaindex>
                        distinct=dist_key:V_HOST_HASH,dist_count:10;dist_key:V_HOST_HASH,dist_count:6#kv=sc_summary_conf:1`1280`1`1`1#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,IndustryStat,PQQualityScore,V_SECURITY_LEVEL
                    </qaindex>
                    <summary_config>2`15000`1&lt;em&gt;`1&lt;/em&gt;`1....</summary_config>
                    
                    <!-- cache -->
                    <cache_max_start_hit>50</cache_max_start_hit>
                    <cache_node>main_cache</cache_node>
                    <cache_compress_type>snappy</cache_compress_type>

                    <!-- read cache -->
                    <cache_key_channel>mainindex</cache_key_channel>
                    <cache_magic_number>agg_web2_arrival_summary_12345</cache_magic_number>
                    
                    <!-- request index -->

                    <!-- rerank -->

                    <!-- fetch summary -->
                    <max_start_hit>10</max_start_hit>
                    <fetch_count>20</fetch_count>
                    <rule>
                        phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:MainBodyProfile,keep_range_id:no,key_type:RAW_PK
                    </rule>

                    <!-- update cache -->
                    <cache_index_list>mainindex,qaindex,rtindex</cache_index_list>
                    <cache_effective_time>7200</cache_effective_time>
                    
                    <!-- internal merge  -->

                    <!-- external merge -->
                </params>
            </processor>
            
            <!-- agg_web_qrs -->
            <processor name="qrs_query_analysis_processor" module_name="agg_web2_module" class_name="QueryAnalysisProcessor">
                <params>
                    <multi_exp_node>multi_exp_bert</multi_exp_node>
                    <multi_exp_v2_node>multi_exp_bert_v2</multi_exp_v2_node>
                    <dct_imp_node>dct_imp</dct_imp_node>
                    <acnn_cache_node>acnn_cache</acnn_cache_node>
                    <acnn_feature_node>acnn_feature_bert/?_chain=autobatch_bert_l4_nn_782_query</acnn_feature_node>
                    <acnn_v2_feature_node>acnn_feature_bert/?_chain=l3_colbert_query</acnn_v2_feature_node>
                    <qtcnn_feature_node>gig/qc_feature_bert/qtc_repre_minibert_model_query_embed/?_chain=L3_QTC_REPRE_V18_QUERY</qtcnn_feature_node>
                    <qtcnn_v2_feature_node>gig/qc_v2_feature_bert/qtc_repre_12Lbert_v30_query_embed?_chain=l3_v2_qtc_repre_v30_avg_query</qtcnn_v2_feature_node>
                    <qtcnn_cache_node>qc_score_cache</qtcnn_cache_node>
                    <!-- L2NN params -->
                    <l2nn_feature_node>gig/qc_v2_feature_bert/qtc_l2_v6_s128_e128_query?_chain=l2_v6_s128_e128_kd_query</l2nn_feature_node>
                    <l2nn_v2_feature_node>gig/qc_v2_feature_bert/qtc_l2_v32_s128_e128_query?_chain=l2_v32_s128_e128_kd_query</l2nn_v2_feature_node>
                    <l2nn_cache_node>qc_score_cache</l2nn_cache_node>
                    <!-- L2NN params end -->
                    <qp_node>gig/qp/web</qp_node>
                    <channels_clusters>
                        mainindex:web_small_index:${kvParam_vip_cluster},${kvParam_galaxy_cluster};rtindex:web_rt_index:${kvParam_rtindex_cluster},${kvParam_fastindex_cluster};qaindex:web_qa_index:${kvParam_qaindex_cluster}
                    </channels_clusters>
                    <kv_item>fr:xxx,ip:0.0.0.0,st:wap,tl:54,sl:100,adult_demote:255,sp:on,mode:xxx,or_logic:v2</kv_item>
                    <config_item>start:0,hit:,navigationalQuery:on,filtermode:11</config_item>
                </params>
            </processor>
            <processor name="qrs_index_processor" module_name="agg_web2_module" class_name="QrsIndexProcessor">
                <params>
                    <!-- basic gen qrs clause -->
                    <index_list>mainindex_at,mainindex,bigindex,qaindex,rtindex</index_list>
                    <common>
                        distinct=dist_key:V_HOST_HASH,dist_count:5;dist_key:V_HOST_HASH,dist_count:2#config=format:protobuf,no_summary:yes,proto_format_option:pb_matchdoc_format,timeout:1000,result_compress:snappy
                    </common>
                    <mainindex_at>
                        config=searcher_return_hits:10,hit:100,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,IndustryStat,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL
                    </mainindex_at>
                    <mainindex>
                        config=searcher_return_hits:10,hit:500,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,IndustryStat,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL
                    </mainindex>
                    <bigindex>
                        config=searcher_return_hits:10,hit:500,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,FirstIndexedTime,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL
                    </bigindex>
                    <qaindex>
                        distinct=dist_key:V_HOST_HASH,dist_count:10;dist_key:V_HOST_HASH,dist_count:6#kv=sc_summary_conf:1`1280`1`1`1#config=searcher_return_hits:10,hit:50,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,OfflineQuality,V_TOPIC_ID_0,V_TOPIC_WEIGHT_0,V_TOPIC_ID_1,V_TOPIC_WEIGHT_1,IndustryStat,PQQualityScore,SiteValue,TimeSensitive,V_SECURITY_LEVEL
                    </qaindex>
                    <rtindex>
                        config=searcher_return_hits:10,hit:500,start:0#rank=rank_profile:DefaultProfile#attribute=V_MAIN_BODY_LENGTH,CmauthStat,NormalizedUrl,V_TIME,V_TIME_TYPE,V_URL_HASH,V_URL_DEPTH,V_HOST_HASH,V_HOST_RANK,V_HOST_G_PR,V_DOMAIN_HASH,PageIntention,PageCategory,ContentCategory,PageAttributeTag,PageQuality,PQQualityScore,DNN_AT_ID_ATTR,DNN_TITLE_ID_ATTR,FirstIndexedTime,IndustryStat,SiteValue,TimeSensitive,V_SECURITY_LEVEL
                    </rtindex>
                    <ranker_bucket>
                        bucket_2=mainindex:BucketProfile,qaindex:BucketProfile,bigindex:BucketProfile#bucket_3=mainindex:DefaultProfile,qaindex:DefaultProfile,bigindex:DefaultProfile
                    </ranker_bucket>
                    <ranker_bucket_key>ranker</ranker_bucket_key>
                    <ranker_interleaving_key>ltrmodelexp</ranker_interleaving_key>
                    <ranker_interleaving>ltrmodelexp=mainindex:BucketProfile,qaindex:BucketProfile</ranker_interleaving>
                    <interleaving_qrs_config>${kvParam_interleaving_qrs_config}</interleaving_qrs_config>
                    <abtest_qrs_config>${kvParam_abtest_qrs_config}</abtest_qrs_config>
                    <l1lr_ranker_profile/>
                    
                    <!-- cache -->
                    <max_start_hit>50</max_start_hit>
                    <main_cache_node>main_cache</main_cache_node>
                    <quark_main_cache_node>quark_main_cache</quark_main_cache_node>
                    <main_cache_compress_type>snappy</main_cache_compress_type>
                    <rt_cache_node>rt_cache</rt_cache_node>
                    <rt_cache_compress_type>snappy</rt_cache_compress_type>

                    <!-- read cache -->
                    <cache_key_with_app_name>1</cache_key_with_app_name>
                    <main_cache_key_channel>mainindex</main_cache_key_channel>
                    <main_magic_number>agg_web2_qrs_123456789</main_magic_number>
                    <main_cache_index_set>mainindex,qaindex</main_cache_index_set>
                    <rt_cache_key_channel>rtindex</rt_cache_key_channel>
                    <rt_magic_number>agg_web2_qrs_123456789</rt_magic_number>
                    <rt_cache_index_set>rtindex</rt_cache_index_set>
                    <demote_shared_key>adult_demote_shared_key,political_demote_shared_key</demote_shared_key>
                    
                    <!-- request index -->

                    <!-- rerank -->

                    <!-- fetch summary -->
                    <fetch_count>60</fetch_count>
                    <rule>
                        phase1_node:query_log,phase2_node:query_log_summary,phase2_type:s3,profile:DefaultProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex,phase2_node:mainindex_summary,phase2_type:s3,profile:DefaultProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:rtindex,phase2_node:rtindex_summary,phase2_type:ha3;phase1_node:qaindex,phase2_node:qaindex_summary,phase2_type:s3,profile:DefaultProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:bigindex,phase2_node:bigindex_summary,phase2_type:s3,profile:DefaultProfile,keep_range_id:no,key_type:RAW_PK;phase1_node:mainindex_at,phase2_node:mainindex_summary,phase2_type:s3,profile:DefaultProfile,keep_range_id:no,key_type:RAW_PK
                    </rule>

                    <!-- update cache -->
                    <main_cache_index_list>mainindex,bigindex,qaindex</main_cache_index_list>
                    <main_cache_effective_time>${kvParam_main_cache_effective_time}</main_cache_effective_time>
                    <rt_cache_index_list>rtindex</rt_cache_index_list>
                    <rt_cache_effective_time>${kvParam_rt_cache_effective_time}</rt_cache_effective_time>
                </params>
            </processor>
            <processor name="qrs_log_processor" module_name="agg_web2_module" class_name="LogProcessor">
                <params>
                    <log_actor_set>display</log_actor_set>
                </params>
            </processor>

            <!-- agg_web1 processors -->
            <!--include_xml type="processors">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/processors.xml</include_xml-->
        </processors>

        <selectors>
            <selector name="agg_web_selector" module_name="agg_web2_module" class_name="CombineSelector">
                <params>
                    <split_threshold_qps>250</split_threshold_qps>
                    <limit_threshold_qps>300</limit_threshold_qps>
                    <basic_app>agg_web</basic_app>
                    <degraded_app>agg_web_hp</degraded_app>
                    <bucket_app/>
                </params>
            </selector>
            <selector name="dump_selector" module_name="" class_name="DefaultAppSelector">
                <params>
                    <app_name>dump</app_name>
                </params>
            </selector>
            <selector name="service_debug_selector" module_name="" class_name="DefaultAppSelector">
                <params>
                    <app_name>service_debug</app_name>
                </params>
            </selector>
            <selector name="feature_selector" module_name="" class_name="DefaultAppSelector">
                <params>
                    <app_name>feature</app_name>
                </params>
            </selector>
            <selector name="check_status_selector" module_name="" class_name="DefaultAppSelector">
                <params>
                    <app_name>checkstatus</app_name>
                </params>
            </selector>
            <selector name="union_selector" module_name="agg_web2_module" class_name="QpsSelector">
                <params>
                    <split_threshold_qps>250</split_threshold_qps>
                    <limit_threshold_qps>300</limit_threshold_qps>
                    <basic_app>agg_web</basic_app>
                    <degraded_app>agg_web_hp</degraded_app>
                </params>
            </selector>
            <selector name="qrs_selector" module_name="" class_name="DefaultAppSelector">
                <params>
                    <app_name>agg_web_qrs</app_name>
                </params>
            </selector>
            <selector name="arrival_selector" module_name="" class_name="DefaultAppSelector">
                <params>
                    <app_name>agg_web_arrival</app_name>
                </params>
            </selector>
            <selector name="arrival_summary_selector" module_name="" class_name="DefaultAppSelector">
                <params>
                    <app_name>agg_web_arrival_summary</app_name>
                </params>
            </selector>

            <!-- agg_web1 selector -->
            <!--include_xml type="selectors">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/selectors.xml</include_xml-->
        </selectors>

        <loaders>
            <loader name="access_ctl_data_loader" module_name="a4_ns_plugin" class_name="AccessCtlDataLoader">
                <params/>
            </loader>
            <loader name="city_external_merge_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>CityExternalMergeData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="site_authority_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SiteAuthorityData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="domain_authority_id_align_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>DomainAuthorityIdAlignData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="sc_config_loader" module_name="agg_web2_module" class_name="ScConfigLoader">
                <params>
                </params>
            </loader>
            <loader name="sc_rule_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScRuleData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="sc_qtc_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScQtcData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="sc_app_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScAppData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="stopword_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SetUserData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="index_interleaving_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>IndexInterleavingData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="multi_site_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>MultiSiteData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="specifical_site_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SpecificalSiteData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="intention_template_config_loader" module_name="agg_web2_module" class_name="IntentionTemplateConfigLoader">
                <params>
                </params>
            </loader>
            <loader name="timeliness_config_loader" module_name="agg_web2_module" class_name="TimelinessConfigLoader">
                <params>
                </params>
            </loader>
            <loader name="online_dedup_config_loader" module_name="agg_web2_module" class_name="OnlineDedupConfigLoader">
                <params>
                </params>
            </loader>
            <loader name="adjuster_config_loader" module_name="agg_web2_module" class_name="AdjusterConfigLoader">
                <params>
                </params>
            </loader>
            <loader name="sc_type_mapping_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>MapUserData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="type_sc_mapping_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>MapUserData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="sc_list_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScListData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="releate_sc_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>MapUserData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="type_cutoff_threshold_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>TypeCutoffThresholdData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="sc_cutoff_threshold_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScCutoffThresholdData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="filter_query_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>FilterQueryData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="agg_querylog_model_v2_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>Boosting</data_type>
                    <data_param>type:gbdt</data_param>
                </params>
            </loader>
            <loader name="gbdt_model_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>GbdtModel</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="gbrank_svm_model_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>GBRankSvmModel</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="entity_quality_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EntityQualityData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="novel_pattern_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>PatternMatchData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="novel_adjust_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SetUserData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="query_prior_intention_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScPriorModelIdxData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="query_poster_intention_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScPosterModelIdxData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="app_white_list_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SetUserData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="novel_filter_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SetUserData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="novel_shield_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>HashSetUserData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="sitelink_loader" module_name="agg_web2_module" class_name="SitelinkDictLoader">
                <params>
                </params>
            </loader>
            <loader name="general_timeliness_regex_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>GeneralTimelinessRegexData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="general_timeliness_term_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>GeneralTimelinessTermData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="lr_model_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>LrModelData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="kv_string_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>KvStringData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="kv_string_data2_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>KvStringData2</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="wordid_mapping_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>WordIdMappingData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="idword_mapping_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>IdWordMappingData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="timeliness_site_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>TimelinessSiteData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="gbrank_svm_model_news_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>GBRankSvmModelNews</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="feed_makeup_stop_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>FeedMakeupStoper</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="feed_makeup_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>FeedMakeuper</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="host_icon_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>HostIconData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="site_aggregator_pattern_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SiteAggregatorPatternData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="spread_makeup_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SpreadMakeUpData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="domain_authority_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>DomainAuthorityData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="sc_dedup_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ScDedupData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="url_dedup_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>UrlDedupData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="qc_bert_token_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>StringIntData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="qc_term_weight_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>StringFloatData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="qc_stop_word_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SetUserData</data_type>
                    <data_param>type:string</data_param>
                </params>
            </loader>
            <loader name="xishuashua_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>XishuashuaData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="feed_tag_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>FeedTagData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="host_intention_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>HostIntentionData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="detail_intention_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>DetailIntentionData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="url_demote_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>UrlDemoteData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="host_weight_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>HostWeightData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="entity_recommend_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EntityRecommendData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="entity_recommend_black_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EntityRecommendBlackData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="entity_recommend_black_query_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EntityRecommendBlackQueryData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="domain_quality_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>DomainQualityData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="novel_noncoop_url_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>NovelNoncoopUrlData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="timeliness_model_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>Boosting</data_type>
                    <data_param>type:gbdt</data_param>
                </params>
            </loader>
            <loader name="agg_qrclick_model_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>Boosting</data_type>
                    <data_param>type:gbdt</data_param>
                </params>
            </loader>
            <loader name="agg_querylog_model_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>Boosting</data_type>
                    <data_param>type:gbdt</data_param>
                </params>
            </loader>
            <loader name="agg_at_model_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>Boosting</data_type>
                    <data_param>type:gbdt</data_param>
                </params>
            </loader>
            <loader name="agg_tt_querylog_model_v1_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>Boosting</data_type>
                    <data_param>type:gbdt</data_param>
                </params>
            </loader>
            <loader name="agg_tt_querylog_model_v2_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>Boosting</data_type>
                    <data_param>type:gbdt</data_param>
                </params>
            </loader>
            <loader name="high_quality_host_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>HighQualityHostData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="url_pattern_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>UrlPatternData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="speed_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SpeedData</data_type>
                    <data_param/>
                </params>
            </loader>
            <!-- TODO:next gbdt的loader统一成一个 -->
            <loader name="news_host_quality_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>NewsHostQualityData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="edu_opt_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EduOptData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="edu_sc_temp_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EduOptData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="edu_diversion_trigger_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EduDiversionTriggerData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="host_demote_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>HostDemoteData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="spm_feature_discretize_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SpmFeatureDiscretizeData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="spm_predict_id_info_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SpmPredictIdInfoData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="edu_query_offi_mark_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EduQueryOffiMarkData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="edu_query_hit_pos_data_loader" module_name="agg_web2_module" class_name="AggWebHttpDataLoader">
                <params>
                    <data_type>EduQueryHitPosData</data_type>
                    <data_param/>
                <data_dir>${A4_INSTALL_ROOT}/usr/local/var/data/</data_dir></params>
            </loader>
            <loader name="old_sc_offline_data_loader" module_name="agg_web2_module" class_name="AggWebHttpDataLoader">
                <params>
                    <data_type>OldScOfflineData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="intention_threshold_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>IntentionThresholdData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="makeup_followup_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>MakeupFollowupData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="summary_makeup_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SummaryMakeupData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="tool_sc_name_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>ToolScNameData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="summary_rank_pos_data_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>SummaryRankPosData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="bert_dict_hashmap_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                 <params>
                     <data_type>BertDictHashMap</data_type>
                     <data_param/>
                 </params>
             </loader>
            <loader name="jxwd_merge_model_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>EnsembleGBDTData</data_type>
                    <data_param/>
                </params>
            </loader>

            <loader name="official_document_online_loader" module_name="agg_web2_module" class_name="AggWebDataLoader">
                <params>
                    <data_type>OfficialDocumentOnlineData</data_type>
                    <data_param/>
                </params>
            </loader>
            <loader name="aliws_token_id_loader" module_name="agg_web2_module" class_name="AliwsTokenIdLoader">
                <params>
                </params>
            </loader>
             <!-- agg_web1 loaders -->
             <!--include_xml type="loaders">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/loaders.xml</include_xml-->

        </loaders>

        <loadbalances>
            <loadbalance name="hash_strategy" module_name="" class_name="HashLoadBalanceStrategy"/>
            <loadbalance name="rr_strategy" module_name="" class_name="RrLoadBalanceStrategy"/>
            <loadbalance name="ns_rr_strategy" module_name="a4_ns_plugin" class_name="CmRrStrategy"/>
            <loadbalance name="ns_hash_strategy" module_name="a4_ns_plugin" class_name="CmHashStrategy"/>
            <loadbalance name="ns_ip_hash_strategy" module_name="a4_ns_plugin" class_name="CmIpHashStrategy"/>
        </loadbalances>

    </plugin>

    <nodes>
        <node name="lbs" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_lbs_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_lbs}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        <node name="query_filter" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_query_filter_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_shield}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        <node name="result_filter" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_result_filter_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_shield}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="query_rewrite" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/query_rewrite.json</gig_config>
            <biz>${kvParam_query_rewrite}</biz>
            <strategy>query_rewrite</strategy>
            <hash_key>hash</hash_key>
            <max_qps>0</max_qps>
        </node>
        
        <node name="uads" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_uads_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_uads}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="main_clickserver" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_main_clickserver_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_ha3_nest_click}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="main_pvserver" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_main_pvserver_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_ha3_pv}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="ha3_qlog" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_ha3_qlog_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_ha3_qlog}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        <node name="ha3_qlog_v2" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_ha3_qlog_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_ha3_qlog_v2}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        <node name="ha3_sm_click_server_v3" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_ha3_sm_click_server_v3_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_ha3_sm_click_server_v3}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        <node name="qp" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qp.json</gig_config>
            <biz>${kvParam_qp}</biz>
            <strategy>qp</strategy>
            <max_qps>0</max_qps>
        </node>
        
        <node name="mainindex" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_mainindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="mainindex_at" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_mainindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="mainindex_doc_pk" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_mainindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="mainindex_summary" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_mainindex_summary_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_s3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="query_log_summary" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_mainindex_summary_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_s3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="mainindex_pk" type="http">
            <conn_pool_size>200</conn_pool_size>
            <timeout>10000</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="mainindex_summary_pk" type="http">
            <conn_pool_size>200</conn_pool_size>
            <timeout>10000</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <!-- <name>ha3_mainindex</name> -->
                        <name>${kvParam_s3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="rtindex" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_rtindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_webrt}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="rtindex_summary" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_rtindex_summary_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_webrt}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="fastindex" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_rtindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_webrt}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="fastindex_summary" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_rtindex_summary_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_webrt}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="qaindex" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_qaindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_qa_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="qaindex_doc_pk" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_qaindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_qa_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="qaindex_summary" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_qaindex_summary_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_s3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="bigindex" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_bigindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_bigindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="bigindex_doc_pk" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_bigindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_bigindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="bigindex_summary" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_bigindex_summary_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_s3_bigindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="qa_agg" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qa_agg.json</gig_config>
            <biz>${kvParam_qa_agg}</biz>
            <strategy>qa_agg</strategy>
            <max_qps>0</max_qps>
            <hash_key>hash</hash_key>
        </node>
        
        <node name="query_aol" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/query_aol.json</gig_config>
            <biz>${kvParam_sc_aol_caculate}</biz>
            <strategy>query_aol</strategy>
            <max_qps>0</max_qps>
        </node>
         
        <node name="content_intention" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_content_intention_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">${kvParam_content_intention_vip}</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">${kvParam_content_intention_vip}</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">${kvParam_content_intention_vip}</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">${kvParam_content_intention_vip}</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">${kvParam_content_intention_vip}</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">${kvParam_content_intention_vip}</spec>
                </specs>
            </upstream>
        </node>

        <node name="medical_intention" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_medical_intention_timeout}</timeout>
            <max_qps>0</max_qps>
            <enable_retry>true</enable_retry>
            <retry_timeout>10</retry_timeout>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_medical_intention}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="medical_vertical" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_medical_vertical_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_medical_vertical}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="medical_sc_trigger" type="http">
          <conn_pool_size>200</conn_pool_size>
          <timeout>10000</timeout>
          <max_qps>0</max_qps>
          <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_medical_sc_trigger}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
          </upstream>
        </node>
        
        <node name="arbiter" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/arbiter.json</gig_config>
            <biz>${kvParam_arbiter}</biz>
            <strategy>arbiter</strategy>
            <max_qps>0</max_qps>
        </node>

        <node name="arbiter_retrigger" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_arbiter_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_arbiter}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="sc_aol" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/sc_aol.json</gig_config>
            <biz>${kvParam_sc_aol_caculate}</biz>
            <strategy>sc_aol</strategy>
            <max_qps>0</max_qps>
        </node>

        <node name="edu_signal" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_edu_signal_timeout}</timeout>
             <max_qps>0</max_qps>
             <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_edu_signal}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.80.167:5175</spec>
                </specs-->
            </upstream>
        </node>

        <node name="prior_edu_signal" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_prior_edu_signal_timeout}</timeout>
             <max_qps>0</max_qps>
             <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_prior_edu_signal}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--lb_strategy name="rr_strategy">
                                         <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.80.167:5175</spec>
                </specs-->
            </upstream>
        </node>
        
        <node name="sc_pos_model" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_sc_pos_model_timeout}</timeout>
             <max_qps>0</max_qps>
             <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_sc_pos_model}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--lb_strategy name="rr_strategy">
                                         <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.80.167:5175</spec>
                </specs-->
            </upstream>
        </node>
        
        <node name="query_prior_intention" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_query_prior_intention_timeout}</timeout>
             <max_qps>0</max_qps>
             <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_query_prior_intention}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--lb_strategy name="rr_strategy">
                                         <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.84.177:6410</spec>
                </specs-->
            </upstream>
        </node>

        <node name="query_poster_intention" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_query_poster_intention_timeout}</timeout>
             <max_qps>0</max_qps>
             <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_query_poster_intention}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="s3_kvindex" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_s3_kvindex_timeout}</timeout>
             <max_qps>0</max_qps>
             <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_s3_kvindex}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="query_ner_service" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_query_ner_service_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
               <lb_strategy name="ns_rr_strategy">
                   <params>
                       <name>${kvParam_query_ner_service}</name>
                       <interval>5000</interval>
                   </params>
               </lb_strategy>
               <!--lb_strategy name="rr_strategy">
                                          <params>
                    </params>
               </lb_strategy>
               <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.84.177:6410</spec>
               </specs-->
           </upstream>
        </node>

        <node name="sc_relevance" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>20</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_sc_relevance}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.26.158.53:6642</spec>
                </specs-->
            </upstream>
        </node>


        <node name="ugc_video" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_ugc_video_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ugc_video}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <!--
        <node name="recommend_web" type="gig">
            <gig_config>$(AINST__INSTALLROOT)/conf/a4_agg_medchat/$(deploy_env)/recommend_web.json</gig_config>
            <biz>$(recommend_web)</biz>
            <strategy>recommend_web</strategy>
            <max_qps>0</max_qps>
        </node>
        -->

        <node name="intention_tair" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_intention_model_tair_effective_time}</effective_time>
            <port>${kvParam_intention_model_tair_port}</port>
            <group>${kvParam_intention_model_tair_group}</group>
            <area>${kvParam_intention_model_tair_area}</area>
            <master>${kvParam_intention_model_tair_master}</master>
            <slave>${kvParam_intention_model_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/intention_model_tair.log</log_path>
            <log_level>warn</log_level>
        </node>
       
        <node name="relative_search" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/relative_search.json</gig_config>
            <biz>${kvParam_relative_search}</biz>
            <strategy>relative_search</strategy>
            <max_qps>0</max_qps>
        </node>
        
        <node name="zeus_agg_specific_logic" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_agg_specific_logic_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_agg_specific_logic}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="timeliness" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_timeliness_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_timeliness}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="freshment_ac_ranker_tf_serving" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_agg_feature_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_freshment_rerank}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="ufs_agg_adjuster_model" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_ufs_agg_adjuster_model_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_ufs_agg_adjuster_model}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="feed_replace" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_feed_replace_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_feed_replace}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="feed_search_gpu_service" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_feed_search_gpu_service_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_feed_search_gpu_service}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="feed_search_cpu_service" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_feed_search_cpu_service_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_feed_search_cpu_service}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="cms" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_cms_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_cms}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="relevance_service" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_relevance_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_relevance}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="sifter" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_sifter_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_ip_hash_strategy">
                    <params>
                        <name>${kvParam_sifter}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                        <hot_threshold>20</hot_threshold>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="wenda_summary" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_wenda_summary_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_s3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="summary_modify" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_summary_modify_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_summary_modify}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="makeup" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_makeup_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_makeup}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="config_server" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_config_server_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_config_server}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="brain_agg_model_ufs" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_ufs_app_agg_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_ufs_app_agg}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="authority_agg_rerank_model_ufs" type="http">
          <conn_pool_size>200</conn_pool_size>
          <timeout>80</timeout>
          <max_qps>0</max_qps>
          <upstream>
            <lb_strategy name="ns_rr_strategy">
              <params>
                <name>${kvParam_ufs_agg_authority_model}</name>
                <interval>5000</interval>
              </params>
            </lb_strategy>
          </upstream>
        </node>

        <node name="content_model_ufs" type="http">
            <conn_pool_size>${kvParam_thread_count}</conn_pool_size>
            <timeout>${kvParam_zeus_ufs_app_content_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_ufs_app_content}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.23.18:7529</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.23.18:7529</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.23.18:7529</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.23.18:7529</spec>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.23.18:7529</spec>
                </specs>
                -->
            </upstream>
        </node>

        <node name="multi_exp_bert" type="http">
            <conn_pool_size>5000</conn_pool_size>
            <timeout>${kvParam_zeus_multi_exp_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_multi_exp_bert}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="multi_exp_bert_v2" type="http">
            <conn_pool_size>5000</conn_pool_size>
            <timeout>${kvParam_zeus_multi_exp_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_multi_exp_bert_v2}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="dct_imp" type="http">
            <conn_pool_size>5000</conn_pool_size>
            <timeout>${kvParam_zeus_dct_imp_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_dct_imp}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="qc_v2_feature_bert" type="gig">
             <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qc_v2_feature_bert.json</gig_config>
             <biz>${kvParam_qc_v2_feature_bert}</biz>
             <strategy>qc_v2_feature_bert</strategy>
             <max_qps>0</max_qps>
        </node>


        <node name="qc_feature_bert" type="gig">
             <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qc_feature_bert.json</gig_config>
             <biz>${kvParam_qc_feature_bert}</biz>
             <strategy>qc_feature_bert</strategy>
             <max_qps>0</max_qps>
        </node>

        <node name="qc_feature_bert_p100" type="gig">
             <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qc_feature_bert_p100.json</gig_config>
             <biz>${kvParam_qc_feature_bert_p100}</biz>
             <strategy>qc_feature_bert_p100</strategy>
             <max_qps>0</max_qps>
        </node>

        <node name="qtc_student_v2" type="gig">
             <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qtc_student_v2.json</gig_config>
             <biz>${kvParam_qtc_student_v2}</biz>
             <strategy>qtc_student_v2</strategy>
             <max_qps>0</max_qps>
        </node>

        <node name="qc_feature_bert_2080" type="gig">
             <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qc_feature_bert_2080.json</gig_config>
             <biz>${kvParam_qc_feature_bert_2080}</biz>
             <strategy>qc_feature_bert_2080</strategy>
             <max_qps>0</max_qps>
        </node>

        <node name="sc_qtc_minibert" type="http">                                                                
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_sc_qtc_minibert_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_sc_qtc_minibert}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="qt_semantic" type="gig">
             <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qt_semantic.json</gig_config>
             <biz>${kvParam_qt_semantic}</biz>
             <strategy>qt_semantic</strategy>
             <max_qps>0</max_qps>
        </node>
 
        <node name="brain_agg_feature" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_agg_feature_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_agg_feature}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="agg_feature_ufs" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_agg_feature_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_agg_feature_ufs}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
            <!--
             <gig_config>$(AINST__INSTALLROOT)/conf/a4_agg_medchat/$(deploy_env)/agg_feature_ufs.json</gig_config>
             <biz>$(agg_feature_ufs)</biz>
             <strategy>agg_feature_ufs</strategy>
             <max_qps>0</max_qps>
            -->
        </node>
        
        <node name="spell_check_model_ufs" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_ufs_app_spell_check_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_ufs_app_spell_check}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
        
        <node name="qu_node" type="gig">
            <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/qu.json</gig_config>
            <biz>${kvParam_qu_node_ns}</biz>
            <strategy>qu</strategy>
            <max_qps>0</max_qps>
            <hash_key>hash</hash_key>
        </node>

        <node name="querylog_model_ufs" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_ufs_app_querylog_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_ufs_app_querylog}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="qlog_rewrite_recall" type="http">
            <conn_pool_size>5000</conn_pool_size>
            <timeout>${kvParam_qlog_rewrite_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_qlog_rewrite}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.76.39:9719</spec>
                </specs>
                -->
            </upstream>
        </node>

        <node name="qlog_rewrite_recall_v2" type="http">
            <conn_pool_size>5000</conn_pool_size>
            <timeout>${kvParam_qlog_rewrite_timeout_v2}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_qlog_rewrite_v2}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                
                <!--
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.29.136.37:9985</spec>
                </specs>
                -->
            </upstream>
        </node>

        <node name="remote_log" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_remote_log_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="2" max_fails="30" fail_timeout="10000">backlog.sm.alibaba-inc.com:80</spec>
                    <spec group="1" weight="2" max_fails="30" fail_timeout="10000">backlog.sm.alibaba-inc.com:80</spec>
                    <spec group="1" weight="2" max_fails="30" fail_timeout="10000">backlog.sm.alibaba-inc.com:80</spec>
                    <spec group="1" weight="2" max_fails="30" fail_timeout="10000">backlog.sm.alibaba-inc.com:80</spec>
                    <spec group="1" weight="2" max_fails="30" fail_timeout="10000">backlog.sm.alibaba-inc.com:80</spec>
                    <spec group="1" weight="2" max_fails="30" fail_timeout="10000">backlog.sm.alibaba-inc.com:80</spec>
                </specs>
            </upstream>
        </node>
        
        

        <!-- tair cache  -->
        <node name="main_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_main_cache_effective_time}</effective_time>
            <port>${kvParam_main_cache_tair_port}</port>
            <group>${kvParam_main_cache_tair_group}</group>
            <area>${kvParam_main_cache_tair_area}</area>
            <master>${kvParam_main_cache_tair_master}</master>
            <slave>${kvParam_main_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="quark_main_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_quark_main_cache_effective_time}</effective_time>
            <port>${kvParam_quark_main_cache_tair_port}</port>
            <group>${kvParam_quark_main_cache_tair_group}</group>
            <area>${kvParam_quark_main_cache_tair_area}</area>
            <master>${kvParam_quark_main_cache_tair_master}</master>
            <slave>${kvParam_quark_main_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="rt_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_rt_cache_effective_time}</effective_time>
            <port>${kvParam_rt_cache_tair_port}</port>
            <group>${kvParam_rt_cache_tair_group}</group>
            <area>${kvParam_rt_cache_tair_area}</area>
            <master>${kvParam_rt_cache_tair_master}</master>
            <slave>${kvParam_rt_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/tair.log</log_path>
            <log_level>warn</log_level>
        </node>
        
        <node name="baidu_querylog" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_baidu_querylog_tair_port}</port>
            <group>${kvParam_baidu_querylog_tair_group}</group>
            <area>${kvParam_baidu_querylog_tair_area}</area>
            <master>${kvParam_baidu_querylog_tair_master}</master>
            <slave>${kvParam_baidu_querylog_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/baidu_querylog_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="tt_querylog" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_tt_querylog_tair_port}</port>
            <group>${kvParam_tt_querylog_tair_group}</group>
            <area>${kvParam_tt_querylog_tair_area}</area>
            <master>${kvParam_tt_querylog_tair_master}</master>
            <slave>${kvParam_tt_querylog_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/tt_querylog_tair.log</log_path>
            <log_level>warn</log_level>
        </node>
        
        <node name="querylog_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>43200</effective_time>
            <port>${kvParam_querylog_cache_tair_port}</port>
            <group>${kvParam_querylog_cache_tair_group}</group>
            <area>${kvParam_querylog_cache_tair_area}</area>
            <master>${kvParam_querylog_cache_tair_master}</master>
            <slave>${kvParam_querylog_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/querylog_cache_tair.log</log_path>
            <log_level>warn</log_level>
        </node>
        
        <node name="rt_clickserver" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_rt_clickserver_effective_time}</effective_time>
            <port>${kvParam_rt_clickserver_tair_port}</port>
            <group>${kvParam_rt_clickserver_tair_group}</group>
            <area>${kvParam_rt_clickserver_tair_area}</area>
            <master>${kvParam_rt_clickserver_tair_master}</master>
            <slave>${kvParam_rt_clickserver_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/rt_clickserver_tair.log</log_path>
            <log_level>warn</log_level>
        </node>
        
        <node name="novel_update" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>864000</effective_time>
            <port>${kvParam_novel_update_tair_port}</port>
            <group>${kvParam_novel_update_tair_group}</group>
            <area>${kvParam_novel_update_tair_area}</area>
            <master>${kvParam_novel_update_tair_master}</master>
            <slave>${kvParam_novel_update_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/novel_update_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="qc_score_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_qc_score_cache_effective_time}</effective_time>
            <port>${kvParam_qc_score_cache_tair_port}</port>
            <group>${kvParam_qc_score_cache_tair_group}</group>
            <area>${kvParam_qc_score_cache_tair_area}</area>
            <master>${kvParam_qc_score_cache_tair_master}</master>
            <slave>${kvParam_qc_score_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/qc_score_cache_tair.log</log_path>
            <log_level>warn</log_level>
        </node>
        
        <node name="similar_doc_pair" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_similar_doc_pair_tair_port}</port>
            <group>${kvParam_similar_doc_pair_tair_group}</group>
            <area>${kvParam_similar_doc_pair_tair_area}</area>
            <master>${kvParam_similar_doc_pair_tair_master}</master>
            <slave>${kvParam_similar_doc_pair_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/similar_doc_pair_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="clickg_score_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_clickg_score_cache_effective_time}</effective_time>
            <port>${kvParam_clickg_score_cache_tair_port}</port>
            <group>${kvParam_clickg_score_cache_tair_group}</group>
            <area>${kvParam_clickg_score_cache_tair_area}</area>
            <master>${kvParam_clickg_score_cache_tair_master}</master>
            <slave>${kvParam_clickg_score_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/clickg_score_cache_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="similar_doc_pair_offline" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_similar_doc_pair_offline_tair_port}</port>
            <group>${kvParam_similar_doc_pair_offline_tair_group}</group>
            <area>${kvParam_similar_doc_pair_offline_tair_area}</area>
            <master>${kvParam_similar_doc_pair_offline_tair_master}</master>
            <slave>${kvParam_similar_doc_pair_offline_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/similar_doc_pair_offline_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="wk_similar_doc_pair_offline" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_wk_similar_doc_pair_offline_tair_port}</port>
            <group>${kvParam_wk_similar_doc_pair_offline_tair_group}</group>
            <area>${kvParam_wk_similar_doc_pair_offline_tair_area}</area>
            <master>${kvParam_wk_similar_doc_pair_offline_tair_master}</master>
            <slave>${kvParam_wk_similar_doc_pair_offline_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/wk_similar_doc_pair_offline_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="similar_click_doc_pair" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_similar_click_doc_pair_tair_port}</port>
            <group>${kvParam_similar_click_doc_pair_tair_group}</group>
            <area>${kvParam_similar_click_doc_pair_tair_area}</area>
            <master>${kvParam_similar_click_doc_pair_tair_master}</master>
            <slave>${kvParam_similar_click_doc_pair_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat//similar_click_doc_pair_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="similar_click_doc_pair_v2" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_similar_click_doc_pair_tair_v2_port}</port>
            <group>${kvParam_similar_click_doc_pair_tair_v2_group}</group>
            <area>${kvParam_similar_click_doc_pair_tair_v2_area}</area>
            <master>${kvParam_similar_click_doc_pair_tair_v2_master}</master>
            <slave>${kvParam_similar_click_doc_pair_tair_v2_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat//similar_click_doc_pair_tair_v2.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="upvote" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>0</effective_time>
            <port>${kvParam_usefull_tair_port}</port>
            <group>${kvParam_usefull_tair_group}</group>
            <area>${kvParam_usefull_tair_area}</area>
            <master>${kvParam_usefull_tair_master}</master>
            <slave>${kvParam_usefull_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/usefull_tair.log</log_path>
            <log_level>warn</log_level>
        </node>
        
        <node name="official_site_dict" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_official_site_dict_tair_effective_time}</effective_time>
            <port>${kvParam_official_site_dict_tair_port}</port>
            <group>${kvParam_official_site_dict_tair_group}</group>
            <area>${kvParam_official_site_dict_tair_area}</area>
            <master>${kvParam_official_site_dict_tair_master}</master>
            <slave>${kvParam_official_site_dict_tair_slave}</slave>
            <log_path>/apsarapangu/disk1/xonline_runtime/agg_web_installroot_new_base/logs/a4_agg_medchat/tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="hhm_model_zeus_tfs" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_hhm_model_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_hhm_model}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="arrival_summary_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>${kvParam_rt_cache_effective_time}</effective_time>
            <port>${kvParam_arrival_summary_cache_tair_port}</port>
            <group>${kvParam_arrival_summary_cache_tair_group}</group>
            <area>${kvParam_arrival_summary_cache_tair_area}</area>
            <master>${kvParam_arrival_summary_cache_tair_master}</master>
            <slave>${kvParam_arrival_summary_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="uni_dedup_relation" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_uni_dedup_relation_tair_port}</port>
            <group>${kvParam_uni_dedup_relation_tair_group}</group>
            <area>${kvParam_uni_dedup_relation_tair_area}</area>
            <master>${kvParam_uni_dedup_relation_tair_master}</master>
            <slave>${kvParam_uni_dedup_relation_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/uni_dedup_relation_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="acnn_cache" type="tair">
            <timeout>${kvParam_tair_timeout}</timeout>
            <effective_time>172800</effective_time>
            <port>${kvParam_acnn_cache_tair_port}</port>
            <group>${kvParam_acnn_cache_tair_group}</group>
            <area>${kvParam_acnn_cache_tair_area}</area>
            <master>${kvParam_acnn_cache_tair_master}</master>
            <slave>${kvParam_acnn_cache_tair_slave}</slave>
            <log_path>${A4_INSTALL_ROOT}/logs/a4_agg_medchat/acnn_cache_tair.log</log_path>
            <log_level>warn</log_level>
        </node>

        <node name="acnn_feature_bert" type="http">
            <conn_pool_size>5000</conn_pool_size>
            <timeout>${kvParam_acnn_feature_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_acnn_feature_bert}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="qa_ss_node" type="http">
            <conn_pool_size>500</conn_pool_size>
            <timeout>${kvParam_qa_ss_node_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_qa_ss_node_ns}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="quark_baike_node" type="http">
            <conn_pool_size>500</conn_pool_size>
            <timeout>${kvParam_quark_baike_node_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_quark_baike_node_ns}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

     
        <!-- 对应faiss旧模型在线和调研索引 -->
        <node name="query_vec" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_query_vec_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.180.61.198:5327</spec>
                </specs>
            </upstream>
        </node>
        
        <!-- 对应faiss全量索引新模型 -->
        <node name="bert_query_vec_test" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_query_vec_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.251.180.26:5416</spec>
                </specs>
            </upstream>
        </node>
            
        <!-- 对应faiss调研索引新模型cold1500 --> 
        <node name="bert_query_vec_iter_cold1500" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_query_vec_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.80.221:5321</spec>
                </specs>
            </upstream>
        </node>
            
        <!-- 对应faiss调研索引新模型top2k --> 
        <node name="bert_query_vec_iter_top2k" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_query_vec_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.91.152:5608</spec>
                </specs>
            </upstream>
        </node>
            
        <!-- 对应faiss调研索引新模型top2k_new --> 
        <node name="bert_query_vec_iter_top2k_new" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_query_vec_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.180.64.161:5690</spec>
                </specs>
            </upstream>
        </node>
            
        <node name="clickG_score_node" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_clickG_predict_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_clickG_predict}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                -->
            </upstream>
        </node>

        <node name="clickG_score_node_v2" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_clickG_predict_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_clickG_predict_v2}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                -->
            </upstream>
        </node>

        <node name="click_rerank_nn_node" type="http">
            <conn_pool_size>5000</conn_pool_size>
            <timeout>${kvParam_click_rerank_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_zeus_click_rerank}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
                <!--
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.251.179.65:10805</spec>
                </specs>
                -->
            </upstream>
        </node>
            
        <node name="semantic_index" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.180.62.235:8773</spec>
                </specs>
            </upstream>
        </node>
        
        <node name="semantic_index_iter" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.81.9:5375</spec>
                </specs>
            </upstream>
        </node>
     
        <!-- faiss全量索引新模型 --> 
        <node name="bert_semantic_index" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
              <lb_strategy name="ns_rr_strategy">
                <params>
                  <name>${kvParam_zeus_semantic_index}</name>
                  <interval>5000</interval>
                </params>
              </lb_strategy>
            </upstream>
         </node>

        <!-- faiss全量索引新模型 --> 
        <node name="bert_semantic_index_test" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.76.39:5515</spec>
                </specs>
            </upstream>
         </node>
         
         <!-- faiss新模型调研索引cold1500 -->
         <node name="bert_semantic_index_iter_cold1500" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.251.180.129:5326</spec>
                </specs>
            </upstream>
         </node>

         <!-- faiss全量索引新模型top2k base --> 
         <node name="bert_semantic_index_iter_top2k" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.86.74:5603</spec>
                </specs>
            </upstream>
         </node>

         <!-- faiss全量索引新模型top2k new --> 
         <node name="bert_semantic_index_iter_top2k_new" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="rr_strategy">
                    <params>
                    </params>
                </lb_strategy>
                <specs>
                    <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.81.32:5698</spec>
                </specs>
            </upstream>
         </node>

         <!-- faiss全量索引新模型top2k new --> 
         <node name="bert_semantic_index_column" type="http">
             <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
             <timeout>${kvParam_zeus_semantic_index_timeout}</timeout>
             <max_qps>0</max_qps>
             <upstream>
                 <lb_strategy name="rr_strategy">
                     <params>
                     </params>
                 </lb_strategy>
                 <specs>
                     <spec group="1" weight="1" max_fails="10" fail_timeout="10000">11.9.86.74:5421</spec>
                 </specs>
            </upstream>
         </node>

        <node name="mainindex_semantic" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_mainindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="qaindex_semantic" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>${kvParam_qaindex_timeout}</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_hash_strategy">
                    <params>
                        <name>${kvParam_ha3_mainindex}</name>
                        <interval>5000</interval>
                        <hash_key>hash</hash_key>
                    </params>
                </lb_strategy>
            </upstream>
        </node>

        <node name="semantic_zeus_service" type="gig">
             <gig_config>${A4_INSTALL_ROOT}/usr/local/conf/${kvParam_deploy_env}/semantic_zeus_service.json</gig_config>
             <biz>${kvParam_semantic_zeus_service}</biz>
             <strategy>semantic_zeus_service</strategy>
             <max_qps>0</max_qps>
        </node>
    <!-- faiss add node end -->
        
        <!-- agg_web1 nodes -->
        <!--include_xml type="nodes">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/nodes.xml</include_xml-->
        <node name="aliws_forward_index" type="http">
            <conn_pool_size>${kvParam_conn_pool_size}</conn_pool_size>
            <timeout>500</timeout>
            <max_qps>0</max_qps>
            <upstream>
                <lb_strategy name="ns_rr_strategy">
                    <params>
                        <name>${kvParam_ha3_nest_feature}</name>
                        <interval>5000</interval>
                    </params>
                </lb_strategy>
            </upstream>
        </node>
    </nodes>

    <datas>
        <data name="acl_data" path="${kvParam_ip_limit_rule}" loader="access_ctl_data_loader" update_interval="5000"/>
        <data name="index_interleaving_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/index_interleaving.json" loader="index_interleaving_loader" update_interval="0"/>
        <data name="stopword" path="${A4_INSTALL_ROOT}/usr/local/var/data/stopword.dat" loader="stopword_data_loader" update_interval="0"/>
        <data name="city_external_merge" path="${A4_INSTALL_ROOT}/usr/local/var/data/city_external_merge_data.dat" loader="city_external_merge_data_loader" update_interval="0"/>
        <data name="site_authority" path="${A4_INSTALL_ROOT}/usr/local/var/data/site_timeliness_authority.dat" loader="site_authority_data_loader" update_interval="0"/>
        <data name="domain_authority" path="${A4_INSTALL_ROOT}/usr/local/var/data/domain_authority.dat" loader="domain_authority_data_loader" update_interval="0"/>
        <data name="domain_authority_new" path="${A4_INSTALL_ROOT}/usr/local/var/data/domain_authority_new.dat" loader="domain_authority_data_loader" update_interval="0"/>
        <data name="domain_authority_id_align" path="${A4_INSTALL_ROOT}/usr/local/var/data/qc_pi_align.dat" loader="domain_authority_id_align_data_loader" update_interval="0"/>
        <data name="domain_authority_id_align_new" path="${A4_INSTALL_ROOT}/usr/local/var/data/qc_pi_align_new.dat" loader="domain_authority_id_align_data_loader" update_interval="0"/>
        <data name="domain_authority_id_align_2" path="${A4_INSTALL_ROOT}/usr/local/var/data/qc_pi_align_timeliness_authority.dat" loader="domain_authority_id_align_data_loader" update_interval="0"/>
        <data name="sc_config" path="${A4_INSTALL_ROOT}/usr/local/var/data/sc.conf" loader="sc_config_loader" update_interval="0"/>
        <data name="bert_word_id_mapping_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/bert_word_id_mapping_data" loader="wordid_mapping_data_loader" update_interval="0"/>
        <data name="bert_segment_unicode_punc_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/bert_segment_unicode_punc_data" loader="stopword_data_loader" update_interval="0"/>
        <data name="bert_segment_unicode_ctrl_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/bert_segment_unicode_ctrl_data" loader="stopword_data_loader" update_interval="0"/>
        <data name="faiss_qu_label_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/faiss_qu_label_data" loader="kv_string_data2_loader" update_interval="0"/>
        <data name="tool_sc_name_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_3238.data" loader="tool_sc_name_data_loader" update_interval="0"/>
        <data name="summary_rank_pos_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_3046.data" loader="summary_rank_pos_data_loader" update_interval="0"/>
        <data name="bert_dict_hashmap_term" path="${A4_INSTALL_ROOT}/usr/local/var/data/bert_dict_hashmap_term.txt" loader="bert_dict_hashmap_loader" update_interval="0"/>
        <data name="bert_dict_hashmap_punc" path="${A4_INSTALL_ROOT}/usr/local/var/data/bert_dict_hashmap_punc.txt" loader="bert_dict_hashmap_loader" update_interval="0"/>
        <data name="bert_dict_hashmap_control" path="${A4_INSTALL_ROOT}/usr/local/var/data/bert_dict_hashmap_control.txt" loader="bert_dict_hashmap_loader" update_interval="0"/>
        <data name="sc_wenda_evade_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/sc_wenda_evade.txt" loader="sc_rule_data_loader" update_interval="0"/>
        <data name="sc_qtc_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_2bc8a9bdbc535c909917235f199bde70.data" loader="sc_qtc_data_loader" update_interval="0"/>
        <data name="sc_qtc_data_v2" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_837f8854d9ac6a1a701590160351a2de.data" loader="sc_qtc_data_loader" update_interval="0"/>
        <data name="sc_app_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/sc_app_info.dat" loader="sc_app_data_loader" update_interval="0"/>
        <data name="summary_makeup_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_3303.data" loader="summary_makeup_data_loader" update_interval="0"/>
        <data name="multi_site" path="${A4_INSTALL_ROOT}/usr/local/var/data/host.dat" loader="multi_site_data_loader" update_interval="0"/>
        <data name="news_site" path="${A4_INSTALL_ROOT}/usr/local/var/data/newshost.dat" loader="specifical_site_data_loader" update_interval="0"/>
        <data name="adult_site" path="${A4_INSTALL_ROOT}/usr/local/var/data/adulthost.dat" loader="specifical_site_data_loader" update_interval="0"/>
        <data name="bbs_site" path="${A4_INSTALL_ROOT}/usr/local/var/data//bbshost.dat" loader="specifical_site_data_loader" update_interval="0"/>
        <data name="mobile_friendly_site" path="${A4_INSTALL_ROOT}/usr/local/var/data/mobilehost.dat" loader="specifical_site_data_loader" update_interval="0"/>
        <data name="intention_template_config" path="${A4_INSTALL_ROOT}/usr/local/var/data/intention_template.conf" loader="intention_template_config_loader" update_interval="0"/>
        <data name="timeliness_config" path="${A4_INSTALL_ROOT}/usr/local/var/data/timeliness.conf" loader="timeliness_config_loader" update_interval="0"/>
        <data name="multi_online_dedup_config" path="${A4_INSTALL_ROOT}/usr/local/var/data/multi_online_dedup_v2.conf" loader="online_dedup_config_loader" update_interval="0"/>
        <data name="adjuster_config" path="${A4_INSTALL_ROOT}/usr/local/var/data/adjuster.conf" loader="adjuster_config_loader" update_interval="0"/>
        <data name="sc_type_mapping_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/sc_type_mapping.dat" loader="sc_type_mapping_data_loader" update_interval="0"/>
        <data name="type_sc_mapping_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/type_sc_mapping.dat" loader="type_sc_mapping_data_loader" update_interval="0"/>
        <data name="sc_list_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/sc_list.dat" loader="sc_list_data_loader" update_interval="0"/>
        <data name="releate_sc_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/releate_sc.dat" loader="releate_sc_data_loader" update_interval="0"/>
        <data name="type_cutoff_threshold_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/type_cutoff_threshold.dat" loader="type_cutoff_threshold_data_loader" update_interval="0"/>
        <data name="sc_cutoff_threshold_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/sc_cutoff_threshold.dat" loader="sc_cutoff_threshold_data_loader" update_interval="0"/>
        <data name="filter_query_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/filter_query_data.dat" loader="filter_query_data_loader" update_interval="0"/>
        <data name="querylog_model_v2" path="${A4_INSTALL_ROOT}/usr/local/var/data/querylog_v2.model" loader="agg_querylog_model_v2_loader" update_interval="0"/>
        <data name="news_type_classification_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/news_type_classification_model.dat" loader="gbdt_model_loader" update_interval="0"/>
        <data name="rtindex_merge_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/rtindex_merge_model.dat" loader="gbrank_svm_model_loader" update_interval="0"/>
        <data name="entity_quality" path="${A4_INSTALL_ROOT}/usr/local/var/data/entity_quality.data" loader="entity_quality_data_loader" update_interval="0"/>
        <data name="novel_pattern_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/novel_pattern.dat" loader="novel_pattern_data_loader" update_interval="0"/>
        <data name="novel_adjust_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_257.data" loader="novel_adjust_data_loader" update_interval="0"/>
        <data name="novel_filter_host" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_314.data" loader="novel_filter_data_loader" update_interval="0"/>
        <data name="site_link" path="${A4_INSTALL_ROOT}/usr/local/var/data/wap.sitelink.in" loader="sitelink_loader" update_interval="0"/>
        <data name="site_link2" path="${A4_INSTALL_ROOT}/usr/local/var/data/wap.sitelink.in.2" loader="sitelink_loader" update_interval="0"/>
        <data name="site_link3" path="${A4_INSTALL_ROOT}/usr/local/var/data/wap.sitelink.in.3" loader="sitelink_loader" update_interval="0"/>
        <data name="general_timeliness_regex" path="${A4_INSTALL_ROOT}/usr/local/var/data/general_timeliness_regex.dat" loader="general_timeliness_regex_loader" update_interval="0"/>
        <data name="general_timeliness_term" path="${A4_INSTALL_ROOT}/usr/local/var/data/general_timeliness_term.dat" loader="general_timeliness_term_loader" update_interval="0"/>
        <data name="lr_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/lr_model.data" loader="lr_model_data_loader" update_interval="0"/>
        <data name="sc_to_qcate_dict" path="${A4_INSTALL_ROOT}/usr/local/var/data/sc_to_qcate.dict" loader="kv_string_data_loader" update_interval="0"/>
        <data name="uc_cate_to_qcate_dict" path="${A4_INSTALL_ROOT}/usr/local/var/data/uc_cate_to_qcate.dict" loader="kv_string_data_loader" update_interval="0"/>
        <data name="host_id_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/host_id_data" loader="kv_string_data2_loader" update_interval="0"/>
        <data name="feed_wordid_100w" path="${A4_INSTALL_ROOT}/usr/local/var/data/word_id_mapping_100w" loader="wordid_mapping_data_loader" update_interval="0"/>
        <data name="feed_wordid_100w_reverse" path="${A4_INSTALL_ROOT}/usr/local/var/data/word_id_mapping_100w" loader="idword_mapping_data_loader" update_interval="0"/>
        <data name="feed_wordid_stopword" path="${A4_INSTALL_ROOT}/usr/local/var/data/word_id_mapping_stopword" loader="idword_mapping_data_loader" update_interval="0"/>
        <data name="timeliness_site" path="${A4_INSTALL_ROOT}/usr/local/var/data/timeliness_site.dat" loader="timeliness_site_data_loader" update_interval="0"/>
        <data name="rtindex_merge_model_news" path="${A4_INSTALL_ROOT}/usr/local/var/data/news_rerank.dat" loader="gbrank_svm_model_news_loader" update_interval="0"/>
        <data name="feed_makeup_stop_dict" path="${A4_INSTALL_ROOT}/usr/local/var/data/feed_del" loader="feed_makeup_stop_loader" update_interval="0"/>
        <data name="feed_makeup_dict" path="${A4_INSTALL_ROOT}/usr/local/var/data/feed_top" loader="feed_makeup_loader" update_interval="0"/>
        <data name="host_icon" path="${A4_INSTALL_ROOT}/usr/local/var/data/host_icon.data" loader="host_icon_data_loader" update_interval="0"/>
        <data name="site_aggregator_pattern" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_2860.data" loader="site_aggregator_pattern_loader" update_interval="0"/>
        <data name="spread_makeup" path="${A4_INSTALL_ROOT}/usr/local/var/data/spread_makeup.dat" loader="spread_makeup_data_loader" update_interval="0"/>
        <data name="sc_dedup" path="${A4_INSTALL_ROOT}/usr/local/var/data/scdedup.dat" loader="sc_dedup_data_loader" update_interval="0"/>
        <data name="url_dedup" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_748.data" loader="url_dedup_data_loader" update_interval="0"/>
        <data name="qc_bert_token" path="${A4_INSTALL_ROOT}/usr/local/var/data/bert_word_map" loader="qc_bert_token_loader" update_interval="0"/>
        <data name="qc_term_weight" path="${A4_INSTALL_ROOT}/usr/local/var/data/muda_qp_word_count_idf.dat.slim" loader="qc_term_weight_loader" update_interval="0"/>
        <data name="qc_stop_word" path="${A4_INSTALL_ROOT}/usr/local/var/data/stop_word.txt" loader="qc_stop_word_loader" update_interval="0"/>
        <data name="news_host_quality_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/news_host_quality_data" loader="news_host_quality_data_loader" update_interval="0"/>
        <data name="host_demote_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/host_demote.txt" loader="host_demote_data_loader" update_interval="0"/>
        <data name="app_white_list_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/app_white_list.dat" loader="app_white_list_data_loader" update_interval="0"/>
        <data name="query_prior_intention_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/query_prior_intention.dat" loader="query_prior_intention_data_loader" update_interval="0"/>
        <data name="query_prior_intention_data_lv3" path="${A4_INSTALL_ROOT}/usr/local/var/data/query_prior_intention_lv3.dat" loader="query_prior_intention_data_loader" update_interval="0"/>
        <data name="query_poster_intention_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/query_poster_intention.dat" loader="query_poster_intention_data_loader" update_interval="0"/>
        <data name="spm_feature_discretize_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_feature_discretize.dat" loader="spm_feature_discretize_data_loader" update_interval="0"/>
        <data name="spm_host_id_feature_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_host_id_feature.dat" loader="wordid_mapping_data_loader" update_interval="0"/>
        <data name="spm_recall_sc_id_feature_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_recall_sc_id_feature.dat" loader="wordid_mapping_data_loader" update_interval="0"/>
        <data name="spm_recall_sc_id_feature_v2_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_recall_sc_id_feature_v2.dat" loader="wordid_mapping_data_loader" update_interval="0"/>
        <data name="spm_predict_id_info_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_predict_id_info.dat" loader="spm_predict_id_info_data_loader" update_interval="0"/>
        <data name="spm_predict_id_info_new_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_predict_id_info_new.dat" loader="spm_predict_id_info_data_loader" update_interval="0"/>
        <data name="spm_predict_id_info_v2_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_predict_id_info_v2.dat" loader="spm_predict_id_info_data_loader" update_interval="0"/>
        <data name="spm_predict_id_info_v2_rc2_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/spm_predict_id_info_v2_rc2.dat" loader="spm_predict_id_info_data_loader" update_interval="0"/>
        <data name="edu_opt_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/edu_opt_data_v4" loader="edu_opt_data_loader" update_interval="0"/>
        <data name="edu_sc_temp_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/edu_sc_temp_data" loader="edu_sc_temp_data_loader" update_interval="0"/>
        <data name="edu_diversion_trigger_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/edu_diversion_trigger_data" loader="edu_diversion_trigger_data_loader" update_interval="0"/>
        <data name="edu_query_offi_mark_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/edu_query_offi_mark_data" loader="edu_query_offi_mark_data_loader" update_interval="0"/>
        <data name="edu_query_hit_pos_data" path="${kvParam_data_edu_query_hit_pos_data}" loader="edu_query_hit_pos_data_loader" update_interval="60000"/>
        <data name="old_sc_offline_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/old_sc_offline_data_v1" loader="old_sc_offline_data_loader" update_interval="0"/>
        <data name="intention_threshold_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/intention_threshold_data" loader="intention_threshold_data_loader" update_interval="0"/>
        <data name="makeup_followup" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_3339.data" loader="makeup_followup_loader" update_interval="0"/>
        <data name="jxwd_merge_model_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/jxwd_merge_gbdt_model" loader="jxwd_merge_model_loader" update_interval="0"/>
        <data name="jxwd_type_ctr_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/jxwd_type_ctr_data" loader="kv_string_data2_loader" update_interval="0"/>
        <data name="domain_authority_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/domain_authority_data" loader="kv_string_data2_loader" update_interval="0"/>
        <data name="data_aliws_token_id" path="${A4_INSTALL_ROOT}/usr/local/var/data/dnn_word_map" loader="aliws_token_id_loader" update_interval="0"/>
        <data name="quark_baike_evade" path="${A4_INSTALL_ROOT}/usr/local/var/data/quark_baike_evade" loader="kv_string_data2_loader" update_interval="0"/>

        <!-- 应该不再使用的数据, 后续逐步考虑删除 -->
        <data name="xishuashua_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/xishuashua.dat" loader="xishuashua_loader" update_interval="0"/>
        <data name="feed_tag_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/query_tag_only_reco_feed_data" loader="feed_tag_loader" update_interval="0"/>
        <data name="host_intention" path="${A4_INSTALL_ROOT}/usr/local/var/data/host_intention.dat" loader="host_intention_data_loader" update_interval="0"/>
        <data name="detail_intention" path="${A4_INSTALL_ROOT}/usr/local/var/data/detail_intention.dat" loader="detail_intention_data_loader" update_interval="0"/>
        <data name="url_demote" path="${A4_INSTALL_ROOT}/usr/local/var/data/click_badcase.dat" loader="url_demote_data_loader" update_interval="0"/>
        <data name="host_weight" path="${A4_INSTALL_ROOT}/usr/local/var/data/host_weight_new.dat" loader="host_weight_data_loader" update_interval="0"/>
        <data name="entity_recommend" path="${A4_INSTALL_ROOT}/usr/local/var/data/entity_recommend_new.data" loader="entity_recommend_data_loader" update_interval="0"/>
        <data name="entity_recommend_black" path="${A4_INSTALL_ROOT}/usr/local/var/data/entity_recommend_black.data" loader="entity_recommend_black_data_loader" update_interval="0"/>
        <data name="entity_recommend_black_query" path="${A4_INSTALL_ROOT}/usr/local/var/data/entity_recommend_black_query.data" loader="entity_recommend_black_query_data_loader" update_interval="0"/>
        <data name="domain_quality" path="${A4_INSTALL_ROOT}/usr/local/var/data/domain_quality.dat" loader="domain_quality_data_loader" update_interval="0"/>
        <data name="novel_noncoop_url" path="${A4_INSTALL_ROOT}/usr/local/var/data/novel_noncoop_url.dat" loader="novel_noncoop_url_data_loader" update_interval="0"/>
        <data name="timeliness_trigger_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/timeliness_trigger.model" loader="timeliness_model_loader" update_interval="0"/>
        <data name="timeliness_trigger_model_new" path="${A4_INSTALL_ROOT}/usr/local/var/data/timeliness_trigger.model.new" loader="timeliness_model_loader" update_interval="0"/>
        <data name="timeliness_rerank_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/timeliness_new_rerank.model" loader="timeliness_model_loader" update_interval="0"/>
        <data name="timeliness_rerank_model_new" path="${A4_INSTALL_ROOT}/usr/local/var/data/timeliness_new_rerank.model.new" loader="timeliness_model_loader" update_interval="0"/>
        <data name="auth_trigger_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/auth_trigger.model" loader="timeliness_model_loader" update_interval="0"/>
        <data name="auth_rerank_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/auth_rerank.model" loader="timeliness_model_loader" update_interval="0"/>
        <data name="medical_rerank_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/med_rerank_2.model" loader="timeliness_model_loader" update_interval="0"/>
        <data name="medical_auth_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/med_auth_rerank_4.model" loader="timeliness_model_loader" update_interval="0"/>
        <data name="agg_qrclick_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/agg_qrclick.model" loader="agg_qrclick_model_loader" update_interval="0"/>
        <data name="querylog_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/querylog.model" loader="agg_querylog_model_loader" update_interval="0"/>
        <data name="at_model" path="${A4_INSTALL_ROOT}/usr/local/var/data/at_v1.model" loader="agg_at_model_loader" update_interval="0"/>
        <data name="tt_querylog_model_v1" path="${A4_INSTALL_ROOT}/usr/local/var/data/tt_querylog_v1.model" loader="agg_tt_querylog_model_v1_loader" update_interval="0"/>
        <data name="tt_querylog_model_v2" path="${A4_INSTALL_ROOT}/usr/local/var/data/tt_querylog_v2.model" loader="agg_tt_querylog_model_v2_loader" update_interval="0"/>
        <data name="political_news_site" path="${A4_INSTALL_ROOT}/usr/local/var/data/political_news_site.dat" loader="stopword_data_loader" update_interval="0"/>
        <data name="high_quality_host" path="${A4_INSTALL_ROOT}/usr/local/var/data/high_quality_host.dat" loader="high_quality_host_loader" update_interval="0"/>
        <data name="url_pattern" path="${A4_INSTALL_ROOT}/usr/local/var/data/url_pattern.dat" loader="url_pattern_loader" update_interval="0"/>
        <data name="speed_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/application_2030.data" loader="speed_data_loader" update_interval="0"/>
        <data name="novel_shield_data" path="${A4_INSTALL_ROOT}/usr/local/var/data/novel_shield.dat" loader="novel_shield_data_loader" update_interval="0"/>
        <data name="official_document_online" path="${A4_INSTALL_ROOT}/usr/local/var/data/official_document_online_daily.dat" loader="official_document_online_loader" update_interval="120000"/>

        <!-- agg_web1 datas -->
        <!--include_xml type="datas">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/datas.xml</include_xml-->
    </datas>

    <apps>
        <!-- agg_web主链 -->
        <app name="agg_web">
            <phase name="access-limit">
                <processor name="ns_ip_limit_processor" method="" desc="访问控制"/>
                <processor name="request_limit_processor" method="" desc="流量控制"/>
            </phase>
            <phase name="interleaving">
                <processor name="interleaving_parallel_execute_processor" method="" desc="Interleaving"/>
                <processor name="interleaving_supplement_processor" method="truncate_final_hits" desc="FinalHitQueue截断"/>
                <processor name="interleaving_post_treat_processor" method="" desc="收尾处理"/>
                <processor name="interleaving_log_processor" method="normal_log" desc="日志处理"/>
                <processor name="interleaving_result_format_processor" method="" desc=""/>
                <processor name="interleaving_latency_calculate_processor" method="" desc="Latency指标统计并结束处理"/>
            </phase>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <processor name="query_rewrite_processor" method="request"/>
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <!-- 1. query分析 -->
                <processor name="identity_processor" method="request_uads"/>
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="agg_web_query_filter_processor" method="parse_spellcheck" desc="改写query过qf"/>
                <processor name="qu_signal_processor" method="request" desc="请求qu"/>
                <processor name="click_feedback_processor" method="trigger_click_recall" desc="触发click召回队列"/>

                <!-- 2.  业务触发&异步请求逻辑 -->
                <processor name="intention_processor" method="trigger"/>
                <processor name="timeliness_processor" method="trigger" desc="触发请求时效性服务"/>
                <processor name="medical_processor" method="request_intention"/>
                <processor name="identity_processor" method="get_uads_result"/>
                <processor name="click_feedback_processor" method="trigger"/>
                <processor name="intention_processor" method="search"/>
                <processor name="medical_processor" method="process_intention"/>
                <processor name="timeliness_processor" method="search" desc="获取时效性服务结果"/>
                <processor name="qu_signal_processor" method="get" desc="获取qu返回结果"/>
                <processor name="query_analysis_processor" method="request_prior_intent" desc="获取先验意图"/>
                <processor name="qa_agg_processor" method="search"/>
                <processor name="quark_baike_processor" method="search"/>
                <processor name="query_analysis_processor" method="recheck_query_location" desc=""/>
                <processor name="medical_processor" method="request_vertical"/>
                <processor name="medical_processor" method="request_sc_trigger"/>
                <processor name="querylog_processor" method="pre_trigger"/>
                <processor name="sc_processor" method="request_arbiter" desc="请求Arbiter-sc"/>
                <processor name="rs_processor" method="request_rs" desc="触发请求RelativeSearch"/>
                <processor name="makeup_processor" method="request" desc="触发请求Makeup"/>
                <processor name="video_processor" method="request_ugc_video" desc="触发请求UGC-Video"/>
                <processor name="query_rewrite_processor" method="get_result"/>
                <processor name="querylog_processor" method="trigger"/>
                <processor name="demote_processor" method="demote_before_index" desc="涉黄涉证打压"/>
            </phase>
            <phase name="3">
                <!-- 1. 召回准备工作 -->
                <processor name="index_processor" method="check_index_degrade" desc="检测是否需要索引降级"/>
                <processor name="index_processor" method="gen_qrs_clause"/>
                <processor name="intention_recall_processor" method="extend"/>
                <processor name="faiss_recall_processor" method="create_faiss_subcontext"/>
                <processor name="query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>

                <!-- 2. 召回：关键词索引 + 向量 -->
                <processor name="index_processor" method="read_cache"/>
                <processor name="faiss_recall_processor" method="request_faiss"/>
                <processor name="index_processor" method="request_index"/>
                <processor name="simdoc_processor" method="fetch_simdoc"/>
                <processor name="faiss_recall_processor" method="get_faiss"/>
                <processor name="index_processor" method="basic_rerank"/>

                <!-- 3. 过滤、摘要、特征请求&aggmodel，穿插基于搜索召回的后验逻辑（query后验信号和行业后验逻辑） -->
                <!--processor name="special_host_processor" method="backup" desc="特殊站点Hit备份"/-->
                <processor name="demote_processor" method="sifter_attribute" desc="sifter过滤"/>
                <processor name="dump_processor" method="random_shuffle_qrs" desc="L3Dump时随机打乱AttributeQrsHit顺序"/>
                <processor name="index_processor" method="fetch_summary" desc="获取摘要"/>
                <processor name="ae_index_processor" method="fetch_ae_summary" desc="获取AE摘要"/>
                <processor name="qa_agg_processor" method="request_ae" desc="请求AE结果"/>
                <processor name="timeliness_processor" method="internal_dedup" desc="时效性Summary队列去重"/>
                <processor name="index_processor" method="senior_pre_rerank" desc="SeniorRerank预处理"/>
                <processor name="sc_processor" method="get_intention_tair" desc="请求tair获取sc_aol缓存信息"/>
                <processor name="sc_processor" method="request_sc_aol" desc="请求ScAol, 置于FetchSummary之后SeniorRerank之前, 作为统一的ScAol访问, SeniorRerank中会用到ScAol的结果"/>
                <processor name="index_processor" method="senior_rerank" desc="SeniorRerank"/>
                <processor name="sc_processor" method="get_sc_aol" desc="获取ScAol结果"/>
                <processor name="index_processor" method="senior_rerank_exec" desc="SeniorRerankExec, AggModel算分排序"/>
                <processor name="content_processor" method="get_content_score" desc="获取内容化分数，依赖bert v2分数，因此置于senior_rerank之后"/>
                <processor name="faiss_processor" method="merge_for_cache"/>
                <processor name="index_processor" method="update_cache" desc="更新Cache"/>
            </phase>
            <phase name="4">
                <!-- 1. SC&行业异步获取结果, 请求逻辑参见phase2的step2 -->
                <processor name="faiss_processor" method="split_for_cache"/>
                <processor name="quark_processor" method="fetch_summary" desc="Quark获取问答摘要"/>
                <processor name="feed_processor" method="request_feed_replace" desc="触发请求FeedReplace"/>
                <processor name="summary_modify_processor" method="request" desc="触发请求SummaryModify"/>
                <processor name="summary_modify_processor" method="get_and_modify" desc="获取SummaryModify结果并调整SumamryQrsHit"/>
                <processor name="sc_processor" method="get_arbiter" desc="获取Aribiter-sc结果"/>
                <processor name="rs_processor" method="get_rs" desc="获取RelativeSearch推荐结果"/>
                <processor name="video_processor" method="get_ugc_video" desc="获取UGC-Video推荐结果"/>
                <processor name="backup_processor" method="split_backup" desc="将backupHits从SummaryQrsHitQueue中剥离"/>
                <processor name="sc_processor" method="second_request_arbiter" desc="二次请求Arbiter-sc"/>
                <processor name="video_processor" method="request_retrigger_ugc_video" desc="二次请求UGC-Video"/>
                <processor name="sc_processor" method="second_get_arbiter" desc="获取二次请求Arbiter-sc结果"/>
                <processor name="video_processor" method="get_retrigger_ugc_video" desc="获取二次请求UGC-Video的结果"/>
                <processor name="demote_processor" method="sifter_over_5_pages" desc="5页之后请求结果sifter过滤"/>
                <processor name="orion_processor" method="request" desc="触发请求投放orion-cms服务"/>
                
                <!-- 2. InternalMerge, 包含自然结果和SC行业结果 -->
                <processor name="internal_merge_processor" method="merge_qrs" desc="自然结果队列内Merge"/>
                <processor name="click_feedback_processor" method="request_clickG" desc="请求clickG模型"/>
                <processor name="timeliness_processor" method="internal_rerank" desc="周期性调整"/>
                <processor name="internal_merge_processor" method="merge_sc" desc="sc队列内merge"/>
                <processor name="internal_merge_processor" method="merge_retrigger_sc" desc="合并sc二查结果"/>
                <processor name="sc_processor" method="internal_rerank" desc="队列内合并时调整Sc结果"/>
                <processor name="baike_processor" method="internal_rerank" desc="队列内合并时调整baike结果"/>
                <processor name="tieba_processor" method="internal_rerank" desc="队列内合并并调整tieba结果"/>
                <processor name="sc_processor" method="internal_rerank_step2" desc="队列内合并时调整Sc结果-Step2"/>
                <processor name="internal_rerank_processor" method="internal_rerank" desc="队列内合并时的Rerank逻辑"/>
                <processor name="novel_processor" method="internal_rerank" desc="队列内合并时调整小说结果"/>
                <processor name="internal_merge_processor" method="merge_extend" desc="Merge意图扩展结果"/>
                <processor name="demote_processor" method="sifter_summary" desc="sifter过滤"/>
                <processor name="querylog_processor" method="merge" desc="Qlog并入主队列"/>
                <processor name="click_feedback_processor" method="trigger_click_merge" desc="Click并入主队列"/>
                <processor name="click_feedback_processor" method="internal_final_rerank" desc="队列内点调"/>

                <!-- 3. FinalRerank, 包含自然结果和SC行业结果 -->
                <processor name="special_host_processor" method="sync_req_relevance" desc="Relevance模型预测"/>
                <processor name="baike_processor" method="internal_final_rerank" desc="队列内百科调整"/>
                <processor name="tieba_processor" method="internal_final_rerank" desc="队列内贴吧调整"/>
                <processor name="timeliness_processor" method="internal_final_rerank" desc="队列内时效性调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step1" desc="队列内RerankStep1"/>
                <processor name="authority_processor" method="internal_final_rerank" desc="队列内权威性调整"/>
                <processor name="authority_processor" method="authority_rerank" desc="权威性子目标排序"/>
                <processor name="medical_processor" method="internal_final_rerank" desc="队列内医疗调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step2" desc="队列内RerankStep2"/>
                <processor name="novel_processor" method="internal_final_rerank" desc="队列内小说调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step3" desc="队列内RerankStep3"/>
                
                <!-- 官网调权 原始-->
                <processor name="click_feedback_processor" method="internal_final_rerank_navi" desc="导航和官网点调"/>
                <processor name="click_feedback_processor" method="rerank_clickrecall" desc="点击队列rerank"/>
                <processor name="sc_processor" method="internal_final_rerank" desc="队列内sc调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step4" desc="队列内RerankStep4"/>
                <processor name="timeliness_processor" method="internal_final_rerank_step2" desc="队列内时效性调整STEP2"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step5" desc="队列内RerankStep5"/>
                <processor name="special_host_processor" method="internal_final_rerank" desc="队列内特殊站点调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step6" desc="队列内RerankStep6"/>
                <processor name="feed_processor" method="feed_back_up" desc="插入feed内容化结果到backup"/>
                <processor name="content_processor" method="content_insert" desc="队列内内容化优质结果插入"/>
            </phase>
            <phase name="5">  <!-- TODO 整体看这部分逻辑强依赖，耦合严重，整理难度大 -->
                <!-- 1. 多队列合并 -->
                <processor name="external_merge_processor" method="" desc="多队列合并"/>

                <!-- 2. 业务前置逻辑：主要是插入逻辑 -->
                <processor name="deep_summary_processor" method="request" desc="使用top3的正文请求deep_summary服务"/>
                <processor name="novel_processor" method="request_spl" desc="触发请求小说SPL"/>
                <processor name="novel_processor" method="process_spl" desc="获取SPL结果并Rerank"/>
                <processor name="orion_processor" method="get" desc="获取投放orion-cms结果"/>
                <processor name="video_processor" method="merge_video" desc="合并并调整UGC-Video推荐结果"/>
                <processor name="feed_processor" method="feed_replace" desc="获取并替换Feed结果"/>
                <processor name="feed_processor" method="feed_merge" desc="合并Feed-HQC结果"/>
                <processor name="feed_processor" method="request_abcnn" desc="Abcnn模型预测"/>
                <processor name="feed_processor" method="request_distcnn" desc="Distcnn模型预测"/>
                <processor name="click_feedback_processor" method="rank_with_basic"/>
                <processor name="querylog_processor" method="makeup"/>
                <processor name="faiss_processor" method="makeup_faiss"/>
                <processor name="feed_processor" method="cutoff" desc="FeedCutOff, 获取ABCNN和DISTCNN分数并对Feed结果截断调整"/>
                <processor name="dedup_processor" method="external_distinct" desc="自然结果DedupByHost"/>
                <processor name="qt_processor" method="qt_fetch_summary_merge" desc="获取需要重排的自然结果的t"/>
                <processor name="qt_processor" method="rerank_merge" desc="利用qt得分进行调序"/>

                <!-- 3. ExternalFilter -->
                <processor name="filter_processor" method="external_filter_step1" desc="结果过滤"/>
                <processor name="novel_processor" method="external_filter" desc="小说结果过滤"/>
                <processor name="medical_processor" method="external_filter" desc="医疗结果过滤"/>
                <processor name="filter_processor" method="external_filter_step2" desc="结果过滤"/>
                <processor name="sc_processor" method="external_filter" desc="SC结果过滤"/>
                <processor name="filter_processor" method="external_filter_step3" desc="结果过滤"/>

                <!-- 4. 业务后置逻辑：主要是rerak逻辑 -->
                <processor name="sc_processor" method="external_rerank" desc="队列外ScRerank"/>
                <processor name="authority_processor" method="external_rerank" desc="top15权威性调整"/>
                <processor name="makeup_processor" method="get_and_insert_result" desc="获取并插入Makeup hit"/>
                <processor name="click_feedback_processor" method="adjust_by_nav_click" desc="调整NavUrl位置"/>
                <processor name="sc_processor" method="newshot_dedup_in_makeup" desc="新闻sc去重"/>
                <processor name="novel_processor" method="rerank_in_makeup" desc="小说站点屏蔽"/>
                <processor name="medical_processor" method="external_rerank" desc="医疗场景调序"/>
                <processor name="baike_processor" method="rerank_with_makeup" desc="百科调序"/>
                <processor name="quark_baike_processor" method="rank" desc="获取quark_baike结果并排序"/>
                <processor name="special_host_processor" method="rerank_in_makeup" desc="教育场景调序"/>
                <processor name="sc_processor" method="dedup_after_makeup" desc="过期sc批量删除"/>
                <processor name="pan_demand_processor" method="" desc="nav sc场景调整sc"/>
                <processor name="exp_simdoc_processor" method="replace_simdoc" desc="问答场景替换baidu自然结果"/>

                <processor name="click_feedback_processor" method="rank_with_basic_clickG"/>
                
                <processor name="querylog_processor" method="qlog_makeup" desc="qlog ufs模型调序"/>
                <processor name="faiss_processor" method="post_cutoff" desc="faiss adjuster by qlog"/>

                <processor name="qtc_processor" method="qtc_fetch_summary_merge" desc="重新获取自然结果的tc"/>
                <processor name="qtc_processor" method="rerank_merge" desc="利用qtc进行规则调序"/>

                <processor name="aggregator_processor" method="normal_aggregate" desc="通用聚合"/>
                <processor name="aggregator_processor" method="ucbanner_adjust" desc="ucbanner调整"/>
                <processor name="timeliness_processor" method="external_rerank" desc="时效性调整"/>
                <processor name="aggregator_processor" method="medical_aggregate" desc="医疗聚合"/>
                <processor name="qa_agg_processor" method="rank" desc="获取QA结果并排序"/>
                <processor name="deep_summary_processor" method="merge" desc="获取deep_summary服务,并merge回content中"/>

                <processor name="click_feedback_processor" method="official_online_adjuster" desc="导航和官网点调"/>
                
                <processor name="medical_processor" method="process_vertical" desc="合并并Rerank医疗结果"/>
                <processor name="medical_processor" method="process_trigger" desc="样式增强信号处理"/>
                <processor name="click_feedback_processor" method="desc_click_sc" desc="CTR 低的sc 打压"/>
                <processor name="sc_processor" method="request_sc_sort_model" desc="请求sc排序模型"/>
                <processor name="dedup_processor" method="external_dedup" desc="主队列Hit去重"/>
                <processor name="dedup_processor" method="news_dedup" desc="新闻去重"/>
                <processor name="timeliness_processor" method="external_rerank2" desc="时效性第二阶段模型过旧信息或时间调整"/>
                <processor name="demote_processor" method="host_demote" desc="资源成本站点打压"/>
                <processor name="sc_processor" method="rerank_for_video_query" desc="长视频Query下调序"/>
                <processor name="sc_processor" method="rerank_for_doc_query" desc="doc Query下调序"/>
                <processor name="sc_processor" method="check_nav_sc" desc="nav-sc调整到首位"/>
                <processor name="sc_processor" method="followup" desc="sc跟随配置调整"/>
                <processor name="qtc_processor" method="qtc_fetch_summary_qrs" desc="重新获取自然结果的tc"/>
                <processor name="qtc_processor" method="rerank_qrs" desc="利用qtc进行规则调序"/>

                <!-- 原先处于phase6的处理, 高优业务调序 -->
                <processor name="baike_processor" method="rerank_with_recyle" desc="百科结果去重"/>
                <processor name="feed_processor" method="rerank" desc="小说场景下，feed不出在前5"/>
                <processor name="simdoc_processor" method="replace_simdoc" desc="替换baidu结果"/>
                <processor name="simdoc_processor" method="replace_wk_simdoc" desc="替换wukong结果"/>
                <processor name="edu_processor" method="edu_opt" desc="教育场景调序"/>
                <processor name="bangbangda_processor" method="insert" desc="插入帮帮答sc，商业化需求"/>
                <processor name="medical_processor" method="medical_makeup" desc="医疗导流的位置保护"/>
                <processor name="sc_processor" method="intention_rerank" desc="sc意图需求排序调整"/>
                <processor name="sc_processor" method="rerank_app_sc" desc="app sc排序调整"/>
                <processor name="click_feedback_processor" method="click_final_rerank" desc="最终点调"/>
                <processor name="sc_processor" method="sc_pos_model_final_rerank" desc="sc pos model最终排序调整"/>
                <processor name="sc_processor" method="news_hot_rerank" desc="news hot排序调整"/>
                <processor name="sc_processor" method="sc_qtc_rerank" desc="qtc sc排序调整"/>
                <processor name="dedup_processor" method="external_dedup_new" desc="主队列Hit去重"/>
                <processor name="sc_processor" method="sc_pos_dedup" desc="sc结果去重"/>
                <processor name="demote_processor" method="badcase_cutoff" desc="兜底策略处理badcase"/>
            </phase>
            <phase name="6"> <!-- 所有业务调序逻辑结束, 仅限补充输出字段、结果回收、运营介入、分页截断、封禁 -->
                <!-- 1.结果回收，兜底过滤，运营介入 -->
                <processor name="supplement_processor" method="recyle_deleted_hits" desc="重新召回DeletedHits,扩容结果"/>
                <processor name="filter_processor" method="external_filter_step4" desc="兜底过滤"/>
                <processor name="makeup_processor" method="rerank_makeup_result" desc="运营介入"/>
                
                <!-- 2.截断前信息收集、输出处理，单页截断，封禁 -->
                <processor name="aggregator_processor" method="spread_aggregate" desc="合并蓝关sc,蓝光字段处理"/>
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
                <processor name="query_analysis_processor" method="check_doc_fitler" desc="涉政敏感词内容过滤"/>
                <processor name="result_filter_processor" method="" desc="网页结果封禁过滤"/>

                <!-- 3.输出字段处理 -->
                <processor name="deep_summary_processor" method="add_bkt_exp" desc="根据最后的排序，确定deep_summary逻辑是否命中"/>
                <processor name="post_processor" method="" desc="字段改写、输出补充"/>
            </phase>
            <phase name="7">
                <processor name="log_processor" method="intention_log" desc="输出intention log"/>
                <processor name="log_processor" method="normal_log" desc="display/local/remote日志处理"/>
                <processor name="result_format_processor" method=""/>
                <processor name="latency_calculate_processor" method="" desc="latency指标统计"/>
            </phase>
        </app>

        <!-- interleaving实验用处理链 -->
        <app name="interleaving">
            <threadpool>interleaving_thread_pool</threadpool>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <processor name="query_rewrite_processor" method="request"/>
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <!-- 1. query分析 -->
                <processor name="identity_processor" method="request_uads"/>
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="agg_web_query_filter_processor" method="parse_spellcheck" desc="改写query过qf"/>
                <processor name="qu_signal_processor" method="request" desc="请求qu"/>
                <processor name="click_feedback_processor" method="trigger_click_recall" desc="触发click召回队列"/>

                <!-- 2.  业务触发&异步请求逻辑 -->
                <processor name="intention_processor" method="trigger"/>
                <processor name="timeliness_processor" method="trigger" desc="触发请求时效性服务"/>
                <processor name="medical_processor" method="request_intention"/>
                <processor name="identity_processor" method="get_uads_result"/>
                <processor name="click_feedback_processor" method="trigger"/>
                <processor name="intention_processor" method="search"/>
                <processor name="medical_processor" method="process_intention"/>
                <processor name="timeliness_processor" method="search" desc="获取时效性服务结果"/>
                <processor name="qu_signal_processor" method="get" desc="获取qu返回结果"/>
                <processor name="qa_agg_processor" method="search"/>
                <processor name="quark_baike_processor" method="search"/>
                <processor name="query_analysis_processor" method="recheck_query_location" desc=""/>
                <processor name="medical_processor" method="request_vertical"/>
                <processor name="medical_processor" method="request_sc_trigger"/>
                <processor name="sc_processor" method="request_arbiter" desc="请求Arbiter-sc"/>
                <processor name="rs_processor" method="request_rs" desc="触发请求RelativeSearch"/>
                <processor name="makeup_processor" method="request" desc="触发请求Makeup"/>
                <processor name="video_processor" method="request_ugc_video" desc="触发请求UGC-Video"/>
                <processor name="querylog_processor" method="trigger"/>
                <processor name="query_rewrite_processor" method="get_result"/>
                <processor name="demote_processor" method="demote_before_index" desc="涉黄涉证打压"/>
            </phase>
            <phase name="3">
                <!-- 1. 召回准备工作 -->
                <processor name="index_processor" method="check_index_degrade" desc="检测是否需要索引降级"/>
                <processor name="index_processor" method="gen_qrs_clause"/>
                <processor name="intention_recall_processor" method="extend"/>
                <processor name="faiss_recall_processor" method="create_faiss_subcontext"/>
                <processor name="query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>

                <!-- 2. 召回：关键词索引 + 向量 -->
                <processor name="index_processor" method="read_cache"/>
                <processor name="faiss_recall_processor" method="request_faiss"/>
                <processor name="index_processor" method="request_index"/>
                <processor name="simdoc_processor" method="fetch_simdoc"/>
                <processor name="faiss_recall_processor" method="get_faiss"/>
                <processor name="index_processor" method="basic_rerank"/>

                <!-- 3. 过滤、摘要、特征请求&aggmodel，穿插基于搜索召回的后验逻辑（query后验信号和行业后验逻辑） -->
                <!--processor name="special_host_processor" method="backup" desc="特殊站点Hit备份"/-->
                <processor name="demote_processor" method="sifter_attribute" desc="sifter过滤"/>
                <processor name="dump_processor" method="random_shuffle_qrs" desc="L3Dump时随机打乱AttributeQrsHit顺序"/>
                <processor name="index_processor" method="fetch_summary" desc="获取摘要"/>
                <processor name="ae_index_processor" method="fetch_ae_summary" desc="获取AE摘要"/>
                <processor name="qa_agg_processor" method="request_ae" desc="请求AE结果"/>
                <processor name="timeliness_processor" method="internal_dedup" desc="时效性Summary队列去重"/>
                <processor name="index_processor" method="senior_pre_rerank" desc="SeniorRerank预处理"/>
                <processor name="sc_processor" method="get_intention_tair" desc="请求tair获取sc_aol缓存信息"/>
                <processor name="sc_processor" method="request_sc_aol" desc="请求ScAol, 置于FetchSummary之后SeniorRerank之前, 作为统一的ScAol访问, SeniorRerank中会用到ScAol的结果"/>
                <processor name="index_processor" method="senior_rerank" desc="SeniorRerank"/>
                <processor name="sc_processor" method="get_sc_aol" desc="获取ScAol结果"/>
                <processor name="index_processor" method="senior_rerank_exec" desc="SeniorRerankExec, AggModel算分排序"/>
                <processor name="content_processor" method="get_content_score" desc="获取内容化分数，依赖bert v2分数，因此置于senior_rerank之后"/>
                <processor name="faiss_processor" method="merge_for_cache"/>
                <processor name="index_processor" method="update_cache" desc="更新Cache"/>
            </phase>
            <phase name="4">
                <!-- 1. SC&行业异步获取结果, 请求逻辑参见phase2的step2 -->
                <processor name="faiss_processor" method="split_for_cache"/>
                <processor name="quark_processor" method="fetch_summary" desc="Quark获取问答摘要"/>
                <processor name="feed_processor" method="request_feed_replace" desc="触发请求FeedReplace"/>
                <processor name="summary_modify_processor" method="request" desc="触发请求SummaryModify"/>
                <processor name="summary_modify_processor" method="get_and_modify" desc="获取SummaryModify结果并调整SumamryQrsHit"/>
                <processor name="sc_processor" method="get_arbiter" desc="获取Aribiter-sc结果"/>
                <processor name="rs_processor" method="get_rs" desc="获取RelativeSearch推荐结果"/>
                <processor name="video_processor" method="get_ugc_video" desc="获取UGC-Video推荐结果"/>
                <processor name="backup_processor" method="split_backup" desc="将backupHits从SummaryQrsHitQueue中剥离"/>
                <processor name="sc_processor" method="second_request_arbiter" desc="二次请求Arbiter-sc"/>
                <processor name="video_processor" method="request_retrigger_ugc_video" desc="二次请求UGC-Video"/>
                <processor name="sc_processor" method="second_get_arbiter" desc="获取二次请求Arbiter-sc结果"/>
                <processor name="video_processor" method="get_retrigger_ugc_video" desc="获取二次请求UGC-Video的结果"/>
                <processor name="demote_processor" method="sifter_over_5_pages" desc="5页之后请求结果sifter过滤"/>
                <processor name="orion_processor" method="request" desc="触发请求投放orion-cms服务"/>
                
                <!-- 2. InternalMerge, 包含自然结果和SC行业结果 -->
                <processor name="internal_merge_processor" method="merge_qrs" desc="自然结果队列内Merge"/>
                <processor name="click_feedback_processor" method="request_clickG" desc="请求clickG模型"/>
                <processor name="timeliness_processor" method="internal_rerank" desc="周期性调整"/>
                <processor name="internal_merge_processor" method="merge_sc" desc="sc队列内merge"/>
                <processor name="internal_merge_processor" method="merge_retrigger_sc" desc="合并sc二查结果"/>
                <processor name="sc_processor" method="internal_rerank" desc="队列内合并时调整Sc结果"/>
                <processor name="baike_processor" method="internal_rerank" desc="队列内合并时调整baike结果"/>
                <processor name="tieba_processor" method="internal_rerank" desc="队列内合并并调整tieba结果"/>
                <processor name="sc_processor" method="internal_rerank_step2" desc="队列内合并时调整Sc结果-Step2"/>
                <processor name="internal_rerank_processor" method="internal_rerank" desc="队列内合并时的Rerank逻辑"/>
                <processor name="novel_processor" method="internal_rerank" desc="队列内合并时调整小说结果"/>
                <processor name="internal_merge_processor" method="merge_extend" desc="Merge意图扩展结果"/>
                <processor name="demote_processor" method="sifter_summary" desc="sifter过滤"/>
                <processor name="querylog_processor" method="merge" desc="Qlog并入主队列"/>
                <processor name="click_feedback_processor" method="trigger_click_merge" desc="Click并入主队列"/>
                <processor name="click_feedback_processor" method="internal_final_rerank" desc="队列内点调"/>

                <!-- 3. FinalRerank, 包含自然结果和SC行业结果 -->
                <processor name="special_host_processor" method="sync_req_relevance" desc="Relevance模型预测"/>
                <processor name="baike_processor" method="internal_final_rerank" desc="队列内百科调整"/>
                <processor name="tieba_processor" method="internal_final_rerank" desc="队列内贴吧调整"/>
                <processor name="timeliness_processor" method="internal_final_rerank" desc="队列内时效性调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step1" desc="队列内RerankStep1"/>
                <processor name="authority_processor" method="internal_final_rerank" desc="队列内权威性调整"/>
                <processor name="authority_processor" method="authority_rerank" desc="权威性子目标排序"/>
                <processor name="medical_processor" method="internal_final_rerank" desc="队列内医疗调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step2" desc="队列内RerankStep2"/>
                <processor name="novel_processor" method="internal_final_rerank" desc="队列内小说调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step3" desc="队列内RerankStep3"/>

                <!-- 官网调权 原始-->
                <processor name="click_feedback_processor" method="internal_final_rerank_navi" desc="导航和官网点调"/>
                <processor name="click_feedback_processor" method="rerank_clickrecall" desc="点击队列rerank"/>
                <processor name="sc_processor" method="internal_final_rerank" desc="队列内sc调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step4" desc="队列内RerankStep4"/>
                <processor name="timeliness_processor" method="internal_final_rerank_step2" desc="队列内时效性调整STEP2"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step5" desc="队列内RerankStep5"/>
                <processor name="special_host_processor" method="internal_final_rerank" desc="队列内特殊站点调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step6" desc="队列内RerankStep6"/>
                <processor name="feed_processor" method="feed_back_up" desc="插入feed内容化结果到backup"/>
                <processor name="content_processor" method="content_insert" desc="队列内内容化优质结果插入"/>
            </phase>
            <phase name="5">
                <!-- 1. 多队列合并 -->
                <processor name="external_merge_processor" method="" desc="多队列合并"/>

                <!-- 2. 业务前置逻辑：主要是插入逻辑 -->
                <processor name="deep_summary_processor" method="request" desc="使用top3的正文请求deep_summary服务"/>
                <processor name="novel_processor" method="request_spl" desc="触发请求小说SPL"/>
                <processor name="novel_processor" method="process_spl" desc="获取SPL结果并Rerank"/>
                <processor name="orion_processor" method="get" desc="获取投放orion-cms结果"/>
                <processor name="video_processor" method="merge_video" desc="合并并调整UGC-Video推荐结果"/>
                <processor name="feed_processor" method="feed_replace" desc="获取并替换Feed结果"/>
                <processor name="feed_processor" method="feed_merge" desc="合并Feed-HQC结果"/>
                <processor name="feed_processor" method="request_abcnn" desc="Abcnn模型预测"/>
                <processor name="feed_processor" method="request_distcnn" desc="Distcnn模型预测"/>
                <processor name="click_feedback_processor" method="rank_with_basic"/>
                <processor name="querylog_processor" method="makeup"/>
                <processor name="faiss_processor" method="makeup_faiss"/>
                <processor name="feed_processor" method="cutoff" desc="FeedCutOff, 获取ABCNN和DISTCNN分数并对Feed结果截断调整"/>
                <processor name="dedup_processor" method="external_distinct" desc="自然结果DedupByHost"/>
                <processor name="qt_processor" method="qt_fetch_summary_merge" desc="获取需要重排的自然结果的t"/>
                <processor name="qt_processor" method="rerank_merge" desc="利用qt得分进行调序"/>

                <!-- 3. ExternalFilter -->
                <processor name="filter_processor" method="external_filter_step1" desc="结果过滤"/>
                <processor name="novel_processor" method="external_filter" desc="小说结果过滤"/>
                <processor name="medical_processor" method="external_filter" desc="医疗结果过滤"/>
                <processor name="filter_processor" method="external_filter_step2" desc="结果过滤"/>
                <processor name="sc_processor" method="external_filter" desc="SC结果过滤"/>
                <processor name="filter_processor" method="external_filter_step3" desc="结果过滤"/>

                <!-- 4. 业务后置逻辑：主要是rerak逻辑 -->
                <processor name="sc_processor" method="external_rerank" desc="队列外ScRerank"/>
                <processor name="authority_processor" method="external_rerank" desc="top15权威性调整"/>
                <processor name="makeup_processor" method="get_and_insert_result" desc="获取并插入Makeup hit"/>
                <processor name="click_feedback_processor" method="adjust_by_nav_click" desc="调整NavUrl位置"/>
                <processor name="sc_processor" method="newshot_dedup_in_makeup" desc="新闻sc去重"/>
                <processor name="novel_processor" method="rerank_in_makeup" desc="小说站点屏蔽"/>
                <processor name="medical_processor" method="external_rerank" desc="医疗场景调序"/>
                <processor name="baike_processor" method="rerank_with_makeup" desc="百科调序"/>
                <processor name="quark_baike_processor" method="rank" desc="获取quark_baike结果并排序"/>
                <processor name="special_host_processor" method="rerank_in_makeup" desc="教育场景调序"/>
                <processor name="sc_processor" method="dedup_after_makeup" desc="过期sc批量删除"/>
                <processor name="pan_demand_processor" method="" desc="nav sc场景调整sc"/>
                <processor name="exp_simdoc_processor" method="replace_simdoc" desc="问答场景替换baidu自然结果"/>

                <processor name="click_feedback_processor" method="rank_with_basic_clickG"/>
                
                <processor name="querylog_processor" method="qlog_makeup" desc="qlog ufs模型调序"/>
                <processor name="faiss_processor" method="post_cutoff" desc="faiss adjuster by qlog"/>

                <processor name="qtc_processor" method="qtc_fetch_summary_merge" desc="重新获取自然结果的tc"/>
                <processor name="qtc_processor" method="rerank_merge" desc="利用qtc进行规则调序"/>

                <processor name="aggregator_processor" method="normal_aggregate" desc="通用聚合"/>
                <processor name="aggregator_processor" method="ucbanner_adjust" desc="ucbanner调整"/>
                <processor name="timeliness_processor" method="external_rerank" desc="时效性调整"/>
                <processor name="aggregator_processor" method="medical_aggregate" desc="医疗聚合"/>
                <processor name="qa_agg_processor" method="rank" desc="获取QA结果并排序"/>
                <processor name="deep_summary_processor" method="merge" desc="获取deep_summary服务,并merge回content中"/>

                <processor name="click_feedback_processor" method="official_online_adjuster" desc="导航和官网点调"/>
                
                <processor name="medical_processor" method="process_vertical" desc="合并并Rerank医疗结果"/>
                <processor name="medical_processor" method="process_trigger" desc="样式增强信号处理"/>
                <processor name="click_feedback_processor" method="desc_click_sc" desc="CTR 低的sc 打压"/>
                <processor name="sc_processor" method="request_sc_sort_model" desc="请求sc排序模型"/>
                <processor name="dedup_processor" method="external_dedup" desc="主队列Hit去重"/>
                <processor name="dedup_processor" method="news_dedup" desc="新闻去重"/>
                <processor name="demote_processor" method="host_demote" desc="资源成本站点打压"/>
                <processor name="sc_processor" method="rerank_for_video_query" desc="长视频Query下调序"/>
                <processor name="sc_processor" method="rerank_for_doc_query" desc="doc Query下调序"/>
                <processor name="sc_processor" method="check_nav_sc" desc="nav-sc调整到首位"/>
                <processor name="sc_processor" method="followup" desc="sc跟随配置调整"/>
                <processor name="qtc_processor" method="qtc_fetch_summary_qrs" desc="重新获取自然结果的tc"/>
                <processor name="qtc_processor" method="rerank_qrs" desc="利用qtc进行规则调序"/>

                <!-- 原先处于phase6的处理, 高优业务调序 -->
                <processor name="baike_processor" method="rerank_with_recyle" desc="百科结果去重"/>
                <processor name="feed_processor" method="rerank" desc="小说场景下，feed不出在前5"/>
                <processor name="simdoc_processor" method="replace_simdoc" desc="替换baidu结果"/>
                <processor name="simdoc_processor" method="replace_wk_simdoc" desc="替换wukong结果"/>
                <processor name="edu_processor" method="edu_opt" desc="教育场景调序"/>
                <processor name="bangbangda_processor" method="insert" desc="插入帮帮答sc，商业化需求"/>
                <processor name="medical_processor" method="medical_makeup" desc="医疗导流的位置保护"/>
                <processor name="sc_processor" method="intention_rerank" desc="sc意图需求排序调整"/>
                <processor name="sc_processor" method="rerank_app_sc" desc="app sc排序调整"/>
                <processor name="click_feedback_processor" method="click_final_rerank" desc="最终点调"/>
                <processor name="sc_processor" method="sc_pos_model_final_rerank" desc="sc pos model最终排序调整"/>
                <processor name="sc_processor" method="news_hot_rerank" desc="news hot排序调整"/>
                <processor name="sc_processor" method="sc_qtc_rerank" desc="qtc sc排序调整"/>
                <processor name="dedup_processor" method="external_dedup_new" desc="主队列Hit去重"/>
                <processor name="sc_processor" method="sc_pos_dedup" desc="sc结果去重"/>
            </phase>
            <phase name="6"> <!-- 所有业务调序逻辑结束, 仅限补充输出字段、结果回收、运营介入、分页截断、封禁 -->
                <!-- 1.结果回收，兜底过滤 -->
                <processor name="supplement_processor" method="recyle_deleted_hits" desc="重新召回DeletedHits,扩容结果"/>
                <processor name="filter_processor" method="external_filter_step4" desc="兜底过滤"/>
                
                <!-- 2.运营介入，单页截断，封禁 -->
                <processor name="makeup_processor" method="rerank_makeup_result" desc="运营介入"/>
                <processor name="aggregator_processor" method="spread_aggregate" desc="蓝光字段处理"/>
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
                <processor name="query_analysis_processor" method="check_doc_fitler" desc="涉政敏感词内容过滤"/>
                <processor name="result_filter_processor" method="" desc="网页结果封禁过滤"/>

                <!-- 3.输出字段处理 -->
                <processor name="deep_summary_processor" method="add_bkt_exp" desc="根据最后的排序，确定deep_summary逻辑是否命中"/>
                <processor name="post_processor" method="" desc="字段改写、输出补充"/>
            </phase>
 
        </app>

        <!-- 降级chain -->
        <app name="agg_web_hp" desc="agg_web的降级chain">
            <phase name="access-limit">
                <processor name="ns_ip_limit_processor" method="" desc="访问控制"/>
                <processor name="request_limit_processor" method="" desc="流量控制"/>
            </phase>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="click_feedback_processor" method="trigger"/>
                <processor name="query_analysis_processor" method="recheck_query_location" desc=""/>
                <processor name="query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>
                <processor name="sc_processor" method="request_arbiter" desc="请求Arbiter-sc"/>
                <processor name="rs_processor" method="request_rs" desc="触发请求RelativeSearch"/>
                <processor name="makeup_processor" method="request" desc="触发请求Makeup"/>
                <processor name="demote_processor" method="demote_before_index" desc="涉黄涉证打压"/>
            </phase>
            <phase name="3">
                <processor name="index_processor" method="gen_qrs_clause"/>
                <!--processor name="index_processor" method="read_cache"/-->
                <processor name="index_processor" method="request_index"/>
                <processor name="simdoc_processor" method="fetch_simdoc"/>
                <processor name="index_processor" method="basic_rerank"/>
                <!--processor name="special_host_processor" method="backup" desc="特殊站点Hit备份"/-->
                <processor name="demote_processor" method="sifter_attribute" desc="sifter过滤"/>
                <processor name="index_processor" method="fetch_summary" desc="获取摘要"/>
                <!--processor name="exp_index_processor" method="fetch_summary_mainbody" desc="实验逻辑, 获取摘要"/-->
                <processor name="timeliness_processor" method="internal_dedup" desc="时效性Summary队列去重"/>
                <processor name="index_processor" method="senior_pre_rerank" desc="SeniorRerank预处理"/>
                <processor name="sc_processor" method="get_intention_tair" desc="请求tair获取sc_aol缓存信息"/>
                <processor name="sc_processor" method="request_sc_aol" desc="请求ScAol, 置于FetchSummary之后SeniorRerank之前, 作为统一的ScAol访问, SeniorRerank中会用到ScAol的结果"/>
                <processor name="index_processor" method="senior_rerank" desc="SeniorRerank"/>
                <processor name="sc_processor" method="get_sc_aol" desc="获取ScAol结果"/>
                <processor name="index_processor" method="senior_rerank_exec" desc="SeniorRerankExec, AggModel算分排序"/>
                <!--processor name="index_processor" method="update_cache" desc="更新Cache"/-->
            </phase>
            <phase name="4">
                <processor name="sc_processor" method="get_arbiter" desc="获取Aribiter-sc结果"/>
                <processor name="rs_processor" method="get_rs" desc="获取RelativeSearch推荐结果"/>
                <processor name="demote_processor" method="sifter_over_5_pages" desc="5页之后请求结果sifter过滤"/>
                
                <!-- InternalMerge -->
                <processor name="internal_merge_processor" method="merge_qrs" desc="自然结果队列内Merge"/>
                <processor name="timeliness_processor" method="internal_rerank" desc="周期性调整"/>
                <processor name="internal_merge_processor" method="merge_sc" desc="sc队列内merge"/>
                <processor name="internal_merge_processor" method="merge_retrigger_sc" desc="合并sc二查结果"/>
                <processor name="sc_processor" method="internal_rerank" desc="队列内合并时调整Sc结果"/>
                <processor name="baike_processor" method="internal_rerank" desc="队列内合并时调整baike结果"/>
                <processor name="tieba_processor" method="internal_rerank" desc="队列内合并并调整tieba结果"/>
                <processor name="sc_processor" method="internal_rerank_step2" desc="队列内合并时调整Sc结果-Step2"/>
                <processor name="internal_rerank_processor" method="internal_rerank" desc="队列内合并时的Rerank逻辑"/>
                <processor name="novel_processor" method="internal_rerank" desc="队列内合并时调整小说结果"/>
                <processor name="internal_merge_processor" method="merge_extend" desc="Merge意图扩展结果"/>

                <processor name="demote_processor" method="sifter_summary" desc="sifter过滤"/>

                <processor name="querylog_processor" method="merge" desc="Qlog并入主队列"/>
                <processor name="click_feedback_processor" method="trigger_click_merge" desc="Click并入主队列"/>
                <processor name="click_feedback_processor" method="internal_final_rerank" desc="队列内点调"/>

                <!-- FinalRerank -->
                <processor name="special_host_processor" method="sync_req_relevance" desc="Relevance模型预测"/>
                <processor name="baike_processor" method="internal_final_rerank" desc="队列内百科调整"/>
                <processor name="tieba_processor" method="internal_final_rerank" desc="队列内贴吧调整"/>
                <processor name="timeliness_processor" method="internal_final_rerank" desc="队列内时效性调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step1" desc="队列内RerankStep1"/>
                <processor name="authority_processor" method="internal_final_rerank" desc="队列内权威性调整"/>
                <processor name="medical_processor" method="internal_final_rerank" desc="队列内医疗调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step2" desc="队列内RerankStep2"/>
                <processor name="novel_processor" method="internal_final_rerank" desc="队列内小说调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step3" desc="队列内RerankStep3"/>
                <processor name="click_feedback_processor" method="internal_final_rerank_navi" desc="导航和官网点调"/>
                <processor name="click_feedback_processor" method="rerank_clickrecall" desc="点击队列rerank"/>
                <processor name="sc_processor" method="internal_final_rerank" desc="队列内sc调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step4" desc="队列内RerankStep4"/>
                <processor name="timeliness_processor" method="internal_final_rerank_step2" desc="队列内时效性调整STEP2"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step5" desc="队列内RerankStep5"/>
                <processor name="special_host_processor" method="internal_final_rerank" desc="队列内特殊站点调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step6" desc="队列内RerankStep6"/>
            </phase>
            <phase name="5">
                <processor name="external_merge_processor" method="" desc="多队列合并"/>
                <processor name="click_feedback_processor" method="rank_with_basic"/>
                <processor name="querylog_processor" method="makeup"/>
                <processor name="dedup_processor" method="external_distinct" desc="自然结果DedupByHost"/>
                <!--ExternalFilter-->
                <processor name="filter_processor" method="external_filter_step1" desc="结果过滤"/>
                <processor name="novel_processor" method="external_filter" desc="小说结果过滤"/>
                <processor name="medical_processor" method="external_filter" desc="医疗结果过滤"/>
                <processor name="filter_processor" method="external_filter_step2" desc="结果过滤"/>
                <processor name="sc_processor" method="external_filter" desc="SC结果过滤"/>
                <processor name="filter_processor" method="external_filter_step3" desc="结果过滤"/>

                <processor name="sc_processor" method="external_rerank" desc="队列外ScRerank"/>
                <processor name="authority_processor" method="external_rerank" desc="权威性调整,原makeup_process.AdjustAuthority()"/>
                <processor name="makeup_processor" method="get_and_insert_result" desc="获取并插入Makeup数据,原makeup_process.InsertMakeUpResult()"/>
                <processor name="click_feedback_processor" method="adjust_by_nav_click" desc="调整NavUrl位置,原makeup_process.AdjustByNavClick()"/>
                <processor name="sc_processor" method="newshot_dedup_in_makeup" desc="DedupByNewsHot,原makeup_process.DedupByNewsHot()"/>
                <processor name="novel_processor" method="rerank_in_makeup" desc="小说站点屏蔽 makeupS5,原makeup_process.AdjustForNovelEmergency()"/>
                <processor name="medical_processor" method="external_rerank" desc="MedicalExternalRerank makeupS6,原makeup_process.AdjustMedical()"/>
                <processor name="baike_processor" method="rerank_with_makeup" desc="原MakeUp中Baike调整 makeupS7,原makeup_process.AdjustBaike()"/>
                <processor name="special_host_processor" method="rerank_in_makeup" desc="原MakeUp中Xyst调整 makeupS8,原makeup_process.XystAdjust()"/>
                <processor name="sc_processor" method="dedup_after_makeup" desc="ScDedup ScRemoveAdjust,原makeup_process.ScRemoveAdjust()"/>
                <processor name="pan_demand_processor" method="" desc="娱乐盘点, 暂时没地方放, 后续归结到biz/nav中"/>
                <processor name="querylog_processor" method="qlog_makeup" desc="原QlogMakeupProcessor"/>
                <processor name="aggregator_processor" method="normal_aggregate" desc="聚合"/>
                <processor name="timeliness_processor" method="external_rerank" desc="时效性调整"/>
                <processor name="aggregator_processor" method="medical_aggregate" desc="医疗聚合"/>
                <processor name="qa_agg_processor" method="rank" desc="获取QA结果并排序"/>
                <processor name="click_feedback_processor" method="desc_click_sc" desc="CTR 低的sc 打压"/>
                <processor name="dedup_processor" method="external_dedup" desc="主队列Hit去重"/>
                <processor name="demote_processor" method="host_demote" desc="资源成本站点打压"/>
                <processor name="sc_processor" method="check_nav_sc" desc="nav-sc调整到首位"/>
                <processor name="baike_processor" method="rerank_with_recyle" desc="百科结果去重"/>
                <processor name="sc_processor" method="sc_pos_dedup" desc="sc结果去重"/>
            </phase>
            <phase name="6">
                <processor name="supplement_processor" method="recyle_deleted_hits" desc="重新召回DeletedHits,扩容结果"/>
                <processor name="filter_processor" method="external_filter_step4" desc="结果过滤"/>
                
                <processor name="makeup_processor" method="rerank_makeup_result" desc="调整运营Makeup数据位置"/>
                <processor name="aggregator_processor" method="spread_aggregate" desc="蓝光字段处理"/>
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
                <processor name="query_analysis_processor" method="check_doc_fitler" desc="涉政敏感词内容过滤"/>
                <processor name="result_filter_processor" method="" desc="网页结果封禁过滤"/>
                
                <processor name="post_processor" method="" desc="字段改写、输出补充"/>
            </phase>
            <phase name="7">
                <processor name="log_processor" method="normal_log" desc="display/local/remote日志处理"/>
                <processor name="result_format_processor" method=""/>
                <processor name="latency_calculate_processor" method="" desc="latency指标统计"/>
            </phase>
        </app>
        
        <!-- dump -->
        <app name="dump">
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
            </phase>
            <phase name="2">
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="dump_qp_processor" method="qp_rerank" desc="qp"/>
                <processor name="dump_qp_processor" method="report_metric" desc="report metric: SubContextCount"/>
                <processor name="dump_querylog_processor" method="trigger" desc="querylog"/>
                <processor name="click_feedback_processor" method="trigger_click_recall" desc="触发click召回队列"/>
                <processor name="click_feedback_processor" method="trigger" desc="subcontext-clickfeedback"/>
                <processor name="intention_processor" method="trigger" desc="request intention"/>
                <processor name="timeliness_processor" method="trigger" desc="request timeliness"/>
                <processor name="medical_processor" method="request_intention" desc=""/>
                <processor name="intention_processor" method="search" desc="get intention result"/>
                <processor name="timeliness_processor" method="search" desc="get timeliness result"/>
                <processor name="medical_processor" method="process_intention" desc=""/>

                <!-- index -->
                <processor name="dump_index_processor" method="gen_qrs_clause" desc=""/>
                <processor name="dump_index_processor" method="request_index" desc=""/>
                <processor name="dump_index_processor" method="basic_rerank" desc=""/>
                <processor name="dump_processor" method="random_shuffle_qrs" desc=""/>
                <processor name="dump_index_processor" method="fetch_summary" desc=""/>
                <processor name="dump_index_processor" method="senior_pre_rerank" desc=""/>
                <processor name="sc_processor" method="get_intention_tair" desc="请求tair获取sc_aol缓存信息"/>
                <processor name="sc_processor" method="request_sc_aol" desc="request sc aol in aggcontext"/>
                <processor name="dump_index_processor" method="senior_rerank" desc=""/>
                <processor name="sc_processor" method="get_sc_aol" desc="get sc aol in aggcontext"/>
                <processor name="dump_index_processor" method="senior_rerank_exec" desc="SeniorRerankExec, AggModel算分排序"/>
                <processor name="dump_index_processor" method="error_cutoff" desc=""/>
            </phase>
        </app>

        <app name="agg_web_arrival">
            <phase name="access-limit">
                <processor name="ns_ip_limit_processor" method="" desc="访问控制"/>
                <processor name="request_limit_processor" method="" desc="流量控制"/>
            </phase>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="arrival_query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="arrival_query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="arrival_query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="arrival_querylog_processor" method="trigger"/>
            </phase>
            <phase name="3">
                <processor name="arrival_processor" method="gen_qrs_clause"/>
                <processor name="arrival_query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>
                <processor name="arrival_processor" method="read_cache"/>
                <processor name="arrival_processor" method="request_index"/>
                <processor name="arrival_processor" method="rerank"/>
                <processor name="arrival_processor" method="fetch_summary" desc="获取摘要"/>
                <processor name="arrival_processor" method="update_cache" desc="更新Cache"/>
            </phase>
            <phase name="4">
                <processor name="arrival_processor" method="internal_merge" desc="队列内Merge"/>
            </phase>
            <phase name="5">
                <processor name="arrival_processor" method="external_merge" desc="多队列Merge"/>
            </phase>
            <phase name="6">
                <processor name="arrival_dedup_processor" method="dedup" desc="去重"/>
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
                <processor name="result_filter_processor" method="" desc="网页结果封禁过滤"/>
                <processor name="post_processor" method="" desc="字段改写、输出补充"/>
            </phase>
            <phase name="7">
                <processor name="arrival_log_processor" method="normal_log" desc="display日志处理"/>
                <processor name="result_format_processor" method=""/>
            </phase>
        </app>
        
        <app name="agg_web_arrival_summary">
            <phase name="access-limit">
                <processor name="ns_ip_limit_processor" method="" desc="访问控制"/>
                <processor name="request_limit_processor" method="" desc="流量控制"/>
            </phase>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="arrival_query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="arrival_query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="arrival_query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="arrival_querylog_processor" method="trigger"/>
            </phase>
            <phase name="3">
                <processor name="arrival_summary_processor" method="gen_qrs_clause"/>
                <processor name="arrival_query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>
                <processor name="arrival_summary_processor" method="read_cache"/>
                <processor name="arrival_summary_processor" method="request_index"/>
                <processor name="arrival_summary_processor" method="rerank"/>
                <processor name="arrival_summary_processor" method="fetch_summary" desc="获取摘要"/>
                <processor name="arrival_summary_processor" method="update_cache" desc="更新Cache"/>
            </phase>
            <phase name="4">
                <processor name="arrival_summary_processor" method="internal_merge" desc="队列内Merge"/>
            </phase>
            <phase name="5">
                <processor name="arrival_summary_processor" method="external_merge" desc="多队列Merge"/>
            </phase>
            <phase name="6">
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
                <processor name="result_filter_processor" method="" desc="网页结果封禁过滤"/>
                <processor name="post_processor" method="" desc="字段改写、输出补充"/>
            </phase>
            <phase name="7">
                <processor name="arrival_log_processor" method="normal_log" desc="display日志处理"/>
                <processor name="result_format_processor" method=""/>
            </phase>
        </app>
        
        <app name="agg_web_qrs">
            <phase name="access-limit">
                <processor name="ns_ip_limit_processor" method="" desc="访问控制"/>
                <processor name="request_limit_processor" method="" desc="流量控制"/>
            </phase>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="qrs_query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="qrs_query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="qrs_query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="querylog_processor" method="trigger"/>
                <processor name="click_feedback_processor" method="trigger"/>
                <processor name="demote_processor" method="demote_before_index" desc="涉黄涉证打压"/>
            </phase>
            <phase name="3">
                <processor name="qrs_index_processor" method="gen_qrs_clause" desc=""/>
                <processor name="qrs_query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>
                <processor name="qrs_index_processor" method="read_cache" desc=""/>
                <processor name="qrs_index_processor" method="request_index" desc=""/>
                <processor name="qrs_index_processor" method="rerank" desc=""/>
                <processor name="qrs_index_processor" method="fetch_summary" desc=""/>
                <processor name="qrs_index_processor" method="update_cache" desc=""/>
            </phase>
            <phase name="4">
                <!-- InternalMerge -->
                <processor name="internal_merge_processor" method="merge_qrs" desc="自然结果队列内Merge"/>
                <processor name="timeliness_processor" method="internal_rerank" desc="周期性调整"/>
                <processor name="internal_merge_processor" method="merge_sc" desc="sc队列内merge"/>
                <processor name="internal_merge_processor" method="merge_retrigger_sc" desc="合并sc二查结果"/>
                <processor name="sc_processor" method="internal_rerank" desc="队列内合并时调整Sc结果"/>
                <processor name="baike_processor" method="internal_rerank" desc="队列内合并时调整baike结果"/>
                <processor name="tieba_processor" method="internal_rerank" desc="队列内合并并调整tieba结果"/>
                <processor name="sc_processor" method="internal_rerank_step2" desc="队列内合并时调整Sc结果-Step2"/>
                <processor name="internal_rerank_processor" method="internal_rerank" desc="队列内合并时的Rerank逻辑"/>
                <processor name="novel_processor" method="internal_rerank" desc="队列内合并时调整小说结果"/>
                <processor name="internal_merge_processor" method="merge_extend" desc="Merge意图扩展结果"/>
                
                <processor name="external_merge_processor" method="" desc="多队列合并"/>
                <processor name="dedup_processor" method="external_dedup" desc="主队列Hit去重"/>
                <processor name="dedup_processor" method="external_distinct" desc="自然结果DedupByHost"/>
            </phase>
            <phase name="5">
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
                <processor name="post_processor" method="" desc="字段改写、输出补充"/>
                <processor name="qrs_log_processor" method="normal_log" desc="display日志处理"/>
                <processor name="result_format_processor" method=""/>
            </phase>
        </app>
        
        <app name="agg_web_site">
            <phase name="access-limit">
                <processor name="ns_ip_limit_processor" method="" desc="访问控制"/>
                <processor name="request_limit_processor" method="" desc="流量控制"/>
            </phase>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <processor name="query_rewrite_processor" method="request"/>
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <processor name="identity_processor" method="request_uads"/>
                <processor name="query_rewrite_processor" method="get_result"/>
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="querylog_processor" method="trigger"/>
                <processor name="intention_processor" method="trigger"/>
                <processor name="timeliness_processor" method="trigger" desc="触发请求时效性服务"/>
                <processor name="medical_processor" method="request_intention"/>
                <processor name="identity_processor" method="get_uads_result"/>
                <processor name="click_feedback_processor" method="trigger"/>
                <processor name="intention_processor" method="search"/>
                <processor name="medical_processor" method="process_intention"/>
                <processor name="timeliness_processor" method="search" desc="获取时效性服务结果"/>
                <processor name="query_analysis_processor" method="recheck_query_location" desc=""/>
                <processor name="demote_processor" method="demote_before_index" desc="涉黄涉证打压"/>
            </phase>
            <phase name="3">
                <processor name="index_processor" method="check_index_degrade" desc="检测是否需要索引降级"/>
                <processor name="index_processor" method="gen_qrs_clause"/>
                <processor name="query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>
                <processor name="index_processor" method="read_cache"/>
                <processor name="index_processor" method="request_index"/>
                <processor name="simdoc_processor" method="fetch_simdoc"/>
                <processor name="index_processor" method="basic_rerank"/>
                <!--processor name="special_host_processor" method="backup" desc="特殊站点Hit备份"/-->
                <processor name="demote_processor" method="sifter_attribute" desc="sifter过滤"/>
                <processor name="index_processor" method="fetch_summary" desc="获取摘要"/>
                <processor name="ae_index_processor" method="fetch_ae_summary" desc="获取AE摘要"/>
                <processor name="qa_agg_processor" method="request_ae" desc="请求AE结果"/>
                <processor name="timeliness_processor" method="internal_dedup" desc="时效性Summary队列去重"/>
                <processor name="index_processor" method="senior_pre_rerank" desc="SeniorRerank预处理"/>
                <processor name="sc_processor" method="request_sc_aol" desc="请求ScAol, 置于FetchSummary之后SeniorRerank之前, 作为统一的ScAol访问, SeniorRerank中会用到ScAol的结果"/>
                <processor name="index_processor" method="senior_rerank" desc="SeniorRerank"/>
                <processor name="sc_processor" method="get_sc_aol" desc="获取ScAol结果"/>
                <processor name="index_processor" method="senior_rerank_exec" desc="SeniorRerankExec, AggModel算分排序"/>
                <processor name="index_processor" method="update_cache" desc="更新Cache"/>
            </phase>
            <phase name="4">
                <processor name="summary_modify_processor" method="request" desc="触发请求SummaryModify"/>
                <processor name="summary_modify_processor" method="get_and_modify" desc="获取SummaryModify结果并调整SumamryQrsHit"/>
                <processor name="demote_processor" method="sifter_over_5_pages" desc="5页之后请求结果sifter过滤"/>
                <processor name="orion_processor" method="request" desc="触发请求投放orion-cms服务"/>
                
                <!-- InternalMerge -->
                <processor name="internal_merge_processor" method="merge_qrs" desc="自然结果队列内Merge"/>
                <processor name="timeliness_processor" method="internal_rerank" desc="周期性调整"/>
                <processor name="internal_merge_processor" method="merge_sc" desc="sc队列内merge"/>
                <processor name="internal_merge_processor" method="merge_retrigger_sc" desc="合并sc二查结果"/>
                <processor name="sc_processor" method="internal_rerank" desc="队列内合并时调整Sc结果"/>
                <processor name="baike_processor" method="internal_rerank" desc="队列内合并时调整baike结果"/>
                <processor name="tieba_processor" method="internal_rerank" desc="队列内合并并调整tieba结果"/>
                <processor name="sc_processor" method="internal_rerank_step2" desc="队列内合并时调整Sc结果-Step2"/>
                <processor name="internal_rerank_processor" method="internal_rerank" desc="队列内合并时的Rerank逻辑"/>
                <processor name="novel_processor" method="internal_rerank" desc="队列内合并时调整小说结果"/>
                <processor name="internal_merge_processor" method="merge_extend" desc="Merge意图扩展结果"/>

                <processor name="demote_processor" method="sifter_summary" desc="sifter过滤"/>

                <processor name="click_feedback_processor" method="internal_final_rerank" desc="队列内点调"/>

                <!-- FinalRerank -->
                <processor name="special_host_processor" method="sync_req_relevance" desc="Relevance模型预测"/>
                <processor name="baike_processor" method="internal_final_rerank" desc="队列内百科调整"/>
                <processor name="tieba_processor" method="internal_final_rerank" desc="队列内贴吧调整"/>
                <processor name="timeliness_processor" method="internal_final_rerank" desc="队列内时效性调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step1" desc="队列内RerankStep1"/>
                <processor name="authority_processor" method="internal_final_rerank" desc="队列内权威性调整"/>
                <processor name="medical_processor" method="internal_final_rerank" desc="队列内医疗调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step2" desc="队列内RerankStep2"/>
                <processor name="novel_processor" method="internal_final_rerank" desc="队列内小说调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step3" desc="队列内RerankStep3"/>
                <processor name="click_feedback_processor" method="internal_final_rerank_navi" desc="导航和官网点调"/>
                <processor name="sc_processor" method="internal_final_rerank" desc="队列内sc调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step4" desc="队列内RerankStep4"/>
                <processor name="timeliness_processor" method="internal_final_rerank_step2" desc="队列内时效性调整STEP2"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step5" desc="队列内RerankStep5"/>
                <processor name="special_host_processor" method="internal_final_rerank" desc="队列内特殊站点调整"/>
                <processor name="internal_rerank_processor" method="internal_final_rerank_step6" desc="队列内RerankStep6"/>
            </phase>
            <phase name="5">
                <processor name="external_merge_processor" method="" desc="多队列合并"/>
                <processor name="novel_processor" method="request_spl" desc="触发请求小说SPL"/>
                <processor name="novel_processor" method="process_spl" desc="获取SPL结果并Rerank"/>
                <processor name="orion_processor" method="get" desc="获取投放orion-cms结果"/>
                <processor name="click_feedback_processor" method="rank_with_basic"/>
                <processor name="dedup_processor" method="external_distinct" desc="自然结果DedupByHost"/>
                <!--ExternalFilter-->
                <processor name="filter_processor" method="external_filter_step1" desc="结果过滤"/>
                <processor name="novel_processor" method="external_filter" desc="小说结果过滤"/>
                <processor name="medical_processor" method="external_filter" desc="医疗结果过滤"/>
                <processor name="filter_processor" method="external_filter_step2" desc="结果过滤"/>
                <processor name="sc_processor" method="external_filter" desc="SC结果过滤"/>
                <processor name="filter_processor" method="external_filter_step3" desc="结果过滤"/>

                <processor name="authority_processor" method="external_rerank" desc="权威性调整,原makeup_process.AdjustAuthority()"/>
                <processor name="makeup_processor" method="get_and_insert_result" desc="获取并插入Makeup数据,原makeup_process.InsertMakeUpResult()"/>
                <processor name="click_feedback_processor" method="adjust_by_nav_click" desc="调整NavUrl位置,原makeup_process.AdjustByNavClick()"/>
                <processor name="sc_processor" method="newshot_dedup_in_makeup" desc="DedupByNewsHot,原makeup_process.DedupByNewsHot()"/>
                <processor name="novel_processor" method="rerank_in_makeup" desc="小说站点屏蔽 makeupS5,原makeup_process.AdjustForNovelEmergency()"/>
                <processor name="medical_processor" method="external_rerank" desc="MedicalExternalRerank makeupS6,原makeup_process.AdjustMedical()"/>
                <processor name="baike_processor" method="rerank_with_makeup" desc="原MakeUp中Baike调整 makeupS7,原makeup_process.AdjustBaike()"/>
                <processor name="special_host_processor" method="rerank_in_makeup" desc="原MakeUp中Xyst调整 makeupS8,原makeup_process.XystAdjust()"/>
                <processor name="sc_processor" method="dedup_after_makeup" desc="ScDedup ScRemoveAdjust,原makeup_process.ScRemoveAdjust()"/>
                <processor name="dedup_processor" method="external_dedup" desc="主队列Hit去重"/>
                <processor name="sc_processor" method="check_nav_sc" desc="nav-sc调整到首位"/>
            </phase>
            <phase name="6">
                <processor name="supplement_processor" method="recyle_deleted_hits" desc="重新召回DeletedHits,扩容结果"/>
                <processor name="filter_processor" method="external_filter_step4" desc="结果过滤"/>
                <processor name="makeup_processor" method="rerank_makeup_result" desc="调整运营Makeup数据位置"/>
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
                <processor name="query_analysis_processor" method="check_doc_fitler" desc="涉政敏感词内容过滤"/>
                <processor name="result_filter_processor" method="" desc="网页结果封禁过滤"/>
                <processor name="post_processor" method="" desc="字段改写、输出补充"/>
            </phase>
            <phase name="7">
                <processor name="log_processor" method="normal_log" desc="display/local/remote日志处理"/>
                <processor name="result_format_processor" method=""/>
                <processor name="latency_calculate_processor" method="" desc="latency指标统计"/>
            </phase>
        </app>

        <app name="feature">
            <phase name="access-limit">
                <processor name="ns_ip_limit_processor" method="" desc="访问控制"/>
                <processor name="request_limit_processor" method="" desc="流量控制"/>
            </phase>
            <phase name="1">
                <processor name="request_parse_processor" method=""/>
                <!--processor name="query_rewrite_processor" method="request"/-->
                <processor name="geo_info_processor" method=""/>
                <processor name="query_filter_processor" method=""/>
                <processor name="query_analysis_processor" method="check_lbs_adult_switch" desc="check lbs adult"/>
                <processor name="query_analysis_processor" method="check_query_filter" desc="check query filter"/>
            </phase>
            <phase name="2">
                <!-- 1. query分析 -->
                <!--processor name="identity_processor" method="request_uads"/-->
                <processor name="click_feedback_processor" method="trigger_with_basic"/>
                <processor name="query_analysis_processor" method="qp_rerank" desc="访问QP"/>
                <processor name="qu_signal_processor" method="request" desc="请求qu"/>
                <processor name="click_feedback_processor" method="trigger_click_recall" desc="触发click召回队列"/>

                <!-- 2.  业务触发&异步请求逻辑 -->
                <processor name="intention_processor" method="trigger"/>
                <processor name="timeliness_processor" method="trigger" desc="触发请求时效性服务"/>
                <processor name="medical_processor" method="request_intention"/>
                <!--processor name="identity_processor" method="get_uads_result"/-->
                <processor name="click_feedback_processor" method="trigger"/>
                <processor name="intention_processor" method="search"/>
                <processor name="medical_processor" method="process_intention"/>
                <processor name="timeliness_processor" method="search" desc="获取时效性服务结果"/>
                <processor name="qu_signal_processor" method="get" desc="获取qu返回结果"/>
                <!--processor name="qa_agg_processor" method="search"/-->
                <!--processor name="quark_baike_processor" method="search"/-->
                <processor name="query_analysis_processor" method="recheck_query_location" desc=""/>
                <!--processor name="medical_processor" method="request_vertical"/-->
                <!--processor name="medical_processor" method="request_sc_trigger"/-->
                <!--processor name="querylog_processor" method="pre_trigger"/-->
                <!--processor name="sc_processor" method="request_arbiter" desc="请求Arbiter-sc"/-->
                <!--processor name="rs_processor" method="request_rs" desc="触发请求RelativeSearch"/-->
                <!--processor name="makeup_processor" method="request" desc="触发请求Makeup"/-->
                <!--processor name="video_processor" method="request_ugc_video" desc="触发请求UGC-Video"/-->
                <!--processor name="query_rewrite_processor" method="get_result"/-->
                <!--processor name="querylog_processor" method="trigger"/-->
                <!--processor name="demote_processor" method="demote_before_index" desc="涉黄涉证打压"/-->
            </phase>
            <phase name="3">
                <!-- 1. 召回准备工作 -->
                <processor name="index_processor" method="check_index_degrade" desc="检测是否需要索引降级"/>
                <processor name="index_processor" method="gen_qrs_clause"/>
                <processor name="intention_recall_processor" method="extend"/>
                <!--processor name="faiss_processor" method="create_faiss_subcontext"/-->
                <processor name="query_analysis_processor" method="report_metric" desc="report metric: SubContextCount"/>

                <!-- 2. 召回：关键词索引 + 向量 -->
                <!--processor name="index_processor" method="read_cache"/-->
                <!--processor name="faiss_processor" method="request_faiss"/-->
                <processor name="index_processor" method="request_index"/>
                <!--processor name="simdoc_processor" method="fetch_simdoc"/-->
                <!--processor name="faiss_processor" method="get_faiss"/-->
                <processor name="index_processor" method="basic_rerank"/>

                <!-- 3. 过滤、摘要、特征请求&aggmodel，穿插基于搜索召回的后验逻辑（query后验信号和行业后验逻辑） -->
                <!--processor name="special_host_processor" method="backup" desc="特殊站点Hit备份"/-->
                <processor name="demote_processor" method="sifter_attribute" desc="sifter过滤"/>
                <processor name="dump_processor" method="random_shuffle_qrs" desc="L3Dump时随机打乱AttributeQrsHit顺序"/>
                <processor name="index_processor" method="fetch_summary" desc="获取摘要"/>
                <!--processor name="ae_index_processor" method="fetch_ae_summary" desc="获取AE摘要"/-->
                <!--processor name="qa_agg_processor" method="request_ae" desc="请求AE结果"/-->
                <processor name="timeliness_processor" method="internal_dedup" desc="时效性Summary队列去重"/>
                <processor name="index_processor" method="senior_pre_rerank" desc="SeniorRerank预处理"/>
                <processor name="sc_processor" method="get_intention_tair" desc="请求tair获取sc_aol缓存信息"/>
                <processor name="sc_processor" method="request_sc_aol" desc="请求ScAol, 置于FetchSummary之后SeniorRerank之前, 作为统一的ScAol访问, SeniorRerank中会用到ScAol的结果"/>
                <processor name="index_processor" method="senior_rerank" desc="SeniorRerank"/>
                <processor name="sc_processor" method="get_sc_aol" desc="获取ScAol结果"/>
                <processor name="index_processor" method="senior_rerank_exec" desc="SeniorRerankExec, AggModel算分排序"/>
                <processor name="content_processor" method="get_content_score" desc="获取内容化分数，依赖bert v2分数，因此置于senior_rerank之后"/>
                <!--processor name="faiss_processor" method="merge_for_cache"/-->
                <!--processor name="index_processor" method="update_cache" desc="更新Cache"/-->
            </phase>
            <phase name="4">
                <!-- 2. InternalMerge, 包含自然结果和SC行业结果 -->
                <processor name="internal_merge_processor" method="merge_qrs" desc="自然结果队列内Merge"/>
            </phase>
            <phase name="5">  <!-- TODO 整体看这部分逻辑强依赖，耦合严重，整理难度大 -->
                <!-- 1. 多队列合并 -->
                <processor name="external_merge_processor" method="" desc="多队列合并"/>
            </phase>
            <phase name="6">
                <!-- 所有业务调序逻辑结束, 仅限补充输出字段、结果回收、运营介入、分页截断、封禁 -->
                <processor name="supplement_processor" method="truncate_final_hits" desc="截断FinalQueue"/>
            </phase>
            <phase name="7">
                <!--processor name="log_processor" method="normal_log" desc="display/local/remote日志处理"/-->
                <processor name="result_format_processor" method=""/>
                <processor name="latency_calculate_processor" method="" desc="latency指标统计"/>
            </phase>
        </app>

        <!-- service相关的app-chain -->
        <app name="checkstatus">
            <phase name="1">
                <processor name="request_limit_processor"/>
                <processor name="file_processor"/>
            </phase>
        </app>

        <app name="service_debug">
            <phase name="1">
                <processor name="request_limit_processor"/>
                <processor name="__buildin_debug_processor" method="" desc="debug"/>
            </phase>
        </app>


        <!-- agg_web1 apps -->
        <!--include_xml type="apps">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/apps.xml</include_xml-->
    </apps>

    <locations>
        <location name="/agg" selector="agg_web_selector"/>

        <location name="/agg_union" selector="union_selector"/>
        <location name="/qrs" selector="qrs_selector"/>
        <location name="/arrival" selector="arrival_selector"/>
        <location name="/arrival_summary" selector="arrival_summary_selector"/>

        <location name="/dump" selector="dump_selector"/>
        <location name="/service_debug" selector="service_debug_selector"/>
        <location name="/feature" selector="feature_selector"/>
        <location name="/status" selector="check_status_selector"/>
        <location name="*" selector="check_status_selector"/>


        <!--- agg_web1 location -->
        <!--include_xml type="locations">$(AINST__INSTALLROOT)/conf/a4_agg_medchat/agg_web1/locations.xml</include_xml-->
    </locations>
</A4>
