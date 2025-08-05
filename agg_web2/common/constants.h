#ifndef AGG_WEB_COMMON_CONSTANTS_H
#define AGG_WEB_COMMON_CONSTANTS_H

AGG_BEGIN_NAMESPACE(agg_web2);

// method-name
const std::string METHOD_TRIGGER = "trigger";
const std::string METHOD_MERGE = "merge";
const std::string METHOD_SEARCH = "search";
const std::string METHOD_RANK = "rank";

// default interleaving 
const std::string DEFAULT_INTERLEAVING_TEST = "base";
const std::string DEFAULT_INTERLEAVING_VALUE = "base";

// const string variable
const std::string KEY_DEGRADE_INFO_MAP = "degrade_info_map";
const std::string KEY_IS_INDEX_DEGRADE = "is_index_degrade";

const std::string KEY_CONFIG_COLLECTOR = "config_collector";   // AggContext ConfigCollector Key

// adjust
const std::string KEY_ADJUSTER_OFFSET = "adjuster_offset";

const std::string KEY_SC_QTC_URL_VEC = "sc_qtc_url_vec";
const std::string KEY_SC_QTC_URL_MAP = "sc_qtc_url_map";
const std::string KEY_SC_QTC_HIT_VEC = "sc_qtc_hit_vec";
const std::string KEY_SC_QTC_QRS_TITLES_VEC = "sc_qtc_qrs_titles_vec";
//
const std::string KEY_NEWS_TYPE = "NEWS_TYPE";
const std::string KEY_SYN_TYPE = "SYN_TYPE"; 
const std::string KEY_TOTAL_HIT_COUNT_WITHOUT_SECOND_SEARCH = "total_hit_count_without_second_search";
const std::string KEY_NEWS_NEGATIVE_REGRESSION_VALUE = "news_negative_regression_value";
const std::string KEY_SC_AOL_RESULT_STR = "sc_aol_result_str";
const std::string KEY_QUERY_PRIOR_INTENTION_RESULT_STR = "query_prior_intention_result_str";
const std::string KEY_QUERY_PRIOR_INTENTION_RESULT_LV3_STR = "query_prior_intention_result_lv3_str";
const std::string KEY_QUERY_PRIOR_INTENTION_ARBITER_STR = "query_prior_intention_arbiter_str";
const std::string KEY_QUERY_PRIOR_INTENTION_ARBITER_LV3_STR = "query_prior_intention_arbiter_lv3_str";
const std::string KEY_QUERY_POSTER_INTENTION_RESULT_STR = "query_poster_intention_result_str";
const std::string KEY_QUERY_NER_RESULT_STR = "query_ner_result_str";
const std::string KEY_QUERY_NER_ARBITER_STR = "query_ner_arbiter_str";
const std::string KEY_QUERY_NER_COUNT = "query_ner_count";
const std::string KEY_S3_NEXT_QUERYS_RESULT_VEC = "s3_next_querys_result_vec";
const std::string KEY_S3_NEXT_QUERYS_RESULT_STR = "s3_next_querys_result_str";
const std::string KEY_S3_TITLES_RESULT_VEC = "s3_titles_result_vec";
const std::string KEY_S3_TITLES_RESULT_STR = "s3_titles_result_str";
const std::string KEY_S3_QANCHORS_RESULT_VEC = "s3_qanchors_result_vec";
const std::string KEY_S3_QANCHORS_RESULT_STR = "s3_qanchors_result_str";
const std::string KEY_SC_POS_MODEL_RESULT_STR = "sc_pos_model_result_str";
const std::string KEY_SC_POS_MODEL_VALID_SC_SET = "sc_pos_model_valid_sc_set";
const std::string KEY_SC_POS_MODEL_POS_1_SC_SET = "sc_pos_model_pos_1_sc_set";
const std::string KEY_SC_POS_MODEL_POS_23_SC_SET = "sc_pos_model_pos_23_sc_set";
const std::string KEY_SC_POS_MODEL_POS_2345_SC_SET = "sc_pos_model_pos_2345_sc_set";
const std::string KEY_SC_POS_MODEL_POS_45_SC_SET = "sc_pos_model_pos_45_sc_set";
const std::string KEY_SC_POS_MODEL_INFO_MAP = "sc_pos_model_info_map";
const std::string KEY_SC_POS_MODEL_INFO_LOG = "sc_pos_model_info_log";
const std::string KEY_SC_POS_MODEL_ADJUST_LOG = "sc_pos_model_adjust_log";
const std::string KEY_ETAO_INTENTION_MODIFIED = "ETAO_INTENTION_MODIFIED";
const std::string KEY_ETAO_FILTER_BY_SC_POS_RANK = "ETAO_FILTER_BY_SC_POS_RANK";

const std::string KEY_TIMELINESS_YEAR_TITLE_COUNT = "TIMELINESS_YEAR_TITLE_COUNT";
const std::string KEY_TIMELINESS_TRIGGER_RES = "TIMELINESS_TRIGGER_RES";
const std::string KEY_TIMELINESS_ADJUSTER_MODEL_SCORE = "TIMELINESS_ADJUSTER_MODEL_SCORE";

const std::string KEY_DEDUP_METHOD_TOP = "dedup_method_top";
const std::string KEY_DEDUP_HIT_INFO_VEC = "dedup_hit_info_vec";
const std::string KEY_DEDUP_HIT_INFO_VEC_NEW = "dedup_hit_info_vec_new";
const std::string KEY_QTC_CHECK_IS_NOVEL = "qtc_check_is_novel";

const std::string KEY_QUERYLOG_RECALL_POS = "querylog_recall_pos";
const std::string CONTEXT_CACHE_SWITCHER_KEY = "cache_switcher";
const std::string CONTEXT_GEN_TIMELINESS_RESPONSE_KEY = "gen_timeliness_response";
const std::string QUERY_REWRITE_CACHE_VALUE_KEY = "query_rewrite_cache_value";
const std::string AGGMODEL_ADJUST_KEY = "aggmodel_adjust";
const std::string CONTEXT_QP_QUERY_KEY = "qp_query";
const std::string KEY_ROUTER_SC_REWARD_CONFIG = "router_sc_reward_config";
const std::string KEY_ROUTER_SC_WHITE_LIST = "router_sc_white_list";
const std::string KEY_ROUTER_SC_REWARD_MAKEUP_INFO = "router_sc_reward_makeup_info";
const std::string KEY_ROUTER_SC_MAKEUP_INFO = "router_sc_makeup_info";
const std::string KEY_CONTEXT_FEED_INTENTION = "context_feed_intention";
const std::string KEY_OFFICIAL_URL_SET = "officialUrlSet";
const std::string KEY_QLOG_INFO = "qlog_info";
const std::string KEY_QLOG_INFO_VEC = "qlog_info_vec";
const std::string KEY_CLICK_INFO = "click_info";
const std::string KEY_CLICK_INFO_VEC = "click_info_vec";
const std::string KEY_VPT_CLK_VEC = "vpt_clk_vec";
const std::string KEY_VPT_LP_VEC  = "vpt_lp_vec";
const std::string KEY_VPT_CLK_SORTED_VEC = "vpt_clk_sorted_vec";
const std::string KEY_VPT_CLK_TIME_SORTED_VEC = "vpt_clk_time_sorted_vec";
const std::string KEY_PRE_QLOG_INFO_VEC = "pre_qlog_info_vec";
const std::string KEY_QUERY_LOG_BIG_STATE = "query_log_big_state";
const std::string KEY_PRE_QUERY_LOG_BIG_STATE = "pre_query_log_big_state";
const std::string KEY_QUERY_LOG_NOT_PK = "query_log_not_pk";
const std::string KEY_REQUEST_QU_PROCESSOR_START_TIME = "request_qu_processor_start_time";
const std::string KEY_TIMELINESS_S = "timeliness_s";
const std::string KEY_ALL_STAGE_INFO = "all_stage_info";
const std::string KEY_TRACE_INFO = "trace_info";
const std::string KEY_SEMANTIC_RESPONSE = "semantic_response";
const std::string KEY_SEMANTIC_QRSCLAUSEMAP = "semantic_qrsclausemap";
const std::string KEY_FAISS_NEED_PK_NUM = "faiss_need_pk_num";
const std::string KEY_FAISS_SUCC_PK_NUM = "faiss_succ_pk_num";
const std::string KEY_FAISS_TRIGGER_MODEL_SCORE = "faiss_trigger_model_score";
const std::string KEY_SEMANTIC_HIT_VEC = "semantic_hit_vec";
const std::string KEY_JXWD_TRIGGERED = "jxwd_triggered";
const std::string KEY_WENDA_DEBUG_INFO = "wenda_debug_info";
const std::string KEY_WENDA_REMOVE_HIT = "wenda_remove_hit";
const std::string KEY_HQQRS_BAIKE_RESULT = "hqqrs_baike_result";
const std::string KEY_WEB_SUMMARY_RESULT = "web_summary_result";
const std::string KEY_QU_RESULT_JSON = "qu_result_json";
const std::string KEY_ACNN_CACHE_VALUE = "acnn_cache_value";
const std::string KEY_ACNN_V2_CACHE_VALUE = "acnn_v2_cache_value";
const std::string KEY_ACNN_V1_HTTPRESP_VALUE = "acnn_v1_httpresp_value";
const std::string KEY_ACNN_V2_HTTPRESP_VALUE = "acnn_v2_httpresp_value";
const std::string KEY_L3_BUCKET_STR_VALUE = "l3_bucket_str";
const std::string KEY_L3_LTR_BIZ_VALUE = "l3_ltr_biz_value";
const std::string QCLASSIFY_FEATURE_NODE = "qclassify_feature_node";
const std::string QCLASSIFY_CACHE_NODE = "qclassify_cache_node";
const std::string KEY_QCLASSIFY_CACHE_VALUE = "qlassify_cache_value";
const std::string KEY_QCLASSIFY_IS_CACHE_VALUE = "qlassify_is_cache_value";
const std::string KEY_QCLASSIFY_HTTPRESP_VALUE = "qclassify_httpresp_value";
const std::string MLTRNN_FEATURE_NODE = "mltrnn_feature_node";
const std::string MLTRNN_CACHE_NODE = "mltrnn_cache_node";
const std::string KEY_MLTRNN_CACHE_VALUE = "mltrnn_cache_value";
const std::string KEY_MLTRNN_IS_CACHE_VALUE = "mltrnn_is_cache_value";
const std::string KEY_MLTRNN_HTTPRESP_VALUE = "mltrnn_httpresp_value";
const std::string KEY_CONTENT_QUERY_VALUE = "content_query_value";
const std::string KEY_MLTRNN_REQUEST_VALUE = "mltrnn_request_value";
const std::string KEY_IS_MLTR_VALUE = "is_mltrnn_value";
const std::string KEY_QU_DOC_INDUSTRY_VALUE = "qu_doc_industry_value";
const std::string KEY_QU_NUM_VALUE = "qu_num_value";
const std::string KEY_QSESSION_CACHE_VALUE = "qlassify_cache_value";
const std::string KEY_QSESSION_HTTPRESP_VALUE = "qsession_httpresp_value";
const std::string KEY_DYINTENTION_HTTPRESP_VALUE = "dyintention_httpresp_value";
const std::string MAIN_TERM_CACHE_NODE = "main_term_cache_node";
const std::string KEY_MAIN_TERM_HTTPRESP_VALUE = "main_term_httpresp_value";
const std::string KEY_ENTITY_HTTPRESP_VALUE = "entity_httpresp_value";
const std::string KEY_MAIN_TERM_TITLE_HTTPRESP_VALUE = "main_term_title_httpresp_value";
const std::string KEY_DYINTENTION_HITS_HTTPRESP_VALUE = "dyintention_hits_httpresp_value";
const std::string KEY_HAS_DYINTENTION_RERANK_VALUE = "dyintention_rerank_has_value";
const std::string KEY_QUERY_QU_INDUSTRY = "query_qu_industry";
const std::string KEY_CP_CACHE_VALUE = "cp_v1_cache_value";
const std::string KEY_CP_RESPONSE_VALUE = "cp_v1_response_value";
const std::string QTC_TAIR = "qc_score_cache";
const int QTCNN_CACHE_EXP_SECS = 1 * 24 * 3600;
const int L2NN_CACHE_EXP_SECS = 5 * 24 * 3600;
const std::string KEY_QTCNN_CACHE_VALUE = "qtcnn_cache_value";
const std::string KEY_QTCNNV2_CACHE_VALUE = "qtcnn_v2_cache_value";
const std::string KEY_QTCNN_V1_HTTPRESP_VALUE = "qtcnn_v1_httpresp_value";
const std::string KEY_QTCNN_V2_HTTPRESP_VALUE = "qtcnn_v2_httpresp_value";
const std::string KEY_L2NN_SP_QUERY_HTTPRESP_VALUE = "l2nn_sp_query_httpresp_value";
const std::string KEY_L2NN_QUERY_HTTPRESP_VALUE = "l2nn_query_httpresp_value";
const std::string KEY_L2NN_CACHE_VALUE = "l2nn_cache_value";
const std::string KEY_L2NN_SPELL_CHECK_CACHE_VALUE = "l2nn_spell_check_cache_value";
const std::string KEY_L2_COIL_QUERY_HTTPRESP_VALUE = "l2_coil_semantic_query_httpresp_value";
const std::string KEY_L2_COIL_CACHE_VALUE = "l2_coil_semantic_cache_value";
const std::string KEY_RERANK_UNIT_VEC = "rerank_unit_vec";
const std::string KEY_QUERY_INDUSTRY_INTENTION = "query_industry_intention";
const std::string KEY_BANGBANGDA_SC = "bangbang_sc";
const std::string KEY_BANGBANGDA_TRIGGER = "bangbang_trigger";
const std::string KEY_EXP_EFFECT_INFO = "exp_effect_info";
const std::string KEY_QUARK_BAIKE_TRIGGERED = "quark_baike_triggered";
const std::string KEY_QUARK_BAIKE_RESULT = "quark_baike_result";
const std::string KEY_QUARK_BAIKE_INTENT = "quark_baike_intent";
const std::string KEY_QUARK_BAIKE_EVADE = "quark_baike_evade";

//csite
const std::string KEY_CSITE_DOMAIN = "csite_domain";

// casdnn
const std::string KEY_L5_CASDNN_URL_MAP = "L5_casdnn_url_map";
const std::string KEY_L5_CASDNN_SCORE_RESULT = "L5_casdnn_score_result";
const std::string KEY_L5_CASDNN_SORT_HIT_VEC = "L5_casdnn_sort_hit_vec";
const std::string KEY_L5_CASDNN_SORT_HIT_POS_VEC = "L5_casdnn_sort_hit_pos_vec";

// simdoc
const std::string KEY_SIM_DOC_PAIR = "sim_doc_pair";
const std::string KEY_SIM_DOC_HIT_VEC = "sim_doc_hit_vec";
const std::string KEY_SIMDOC_REPLACE_RESULT = "simdoc_replace_key";
//const std::string KEY_IS_SIMDOC_REPLACE = "is_simdoc_replace";
const std::string KEY_IS_SIMDOC_CLOSE_REPLACE = "is_simdoc_close_replace";
const std::string KEY_SIM_DOC_SCORE_PAIR = "sim_doc_score_map";
//const std::string KEY_IS_SIMDOC_2ND = "is_simdoc_2nd";
const std::string KEY_IS_SIMDOC_OFFLINE = "is_simdoc_offline";
const std::string KEY_SIMDOC_REPLACE_STEP = "simdoc_replace_step";
// simdoc for wukong
const std::string KEY_WK_SIM_DOC_PAIR = "wk_sim_doc_pair";
const std::string KEY_WK_SIM_DOC_HIT_VEC = "wk_sim_doc_hit_vec";
const std::string KEY_IS_WK_SIMDOC_REPLACE = "is_wk_simdoc_replace";
const std::string KEY_WK_SIM_DOC_SCORE_PAIR = "wk_sim_doc_score_map";

// site mapping 
const std::string KEY_IS_SITE_MAPPING_REPLACE = "is_site_mapping_replace";
const std::string KEY_SITE_MAPPING_DOC_PAIR = "site_mapping_doc_pair";
const std::string KEY_SITE_MAPING_DOC_SCORE_PAIR = "site_mapping_doc_score_map";
const std::string KEY_SIM_DOC_SET = "site_mapping_doc_set";
const std::string KEY_SITE_MAPPING_HIT_VEC = "site_mapping_hit_vec"; // 存放top500 官网子链结果

const std::string KEY_SIM_DOC_RECALL_KEY = "site_mapping_doc_recallnum_key";
const std::string KEY_SIM_DOC_RECALL_SET_KEY = "site_mapping_doc_recall_set_key";
const std::string KEY_SIM_DOC_ORIG_SET_KEY = "site_mapping_doc_orig_set_key";
const std::string KEY_SIM_DOC_MAPPING_MAP = "site_mapping_doc_mapping_map";

// query rewrite
const std::string KEY_QUERY_REWRITE_QUERY = "query_rewrite_query";
const std::string KEY_CONTEXT_QUERY_REWRITE_RESULT = "query_rewrite_result_key";
const std::string KEY_CONTEXT_QUERY_REWRITE_FILT_RESULT = "query_rewrite_filt_result_key";
const std::string KEY_CONTEXT_QUERY_REWRITE_BERT_RESULT = "query_rewrite_bert_result_key";
const std::string KEY_CHANGE_QUERY_RAW = "change_query_raw_key";
const std::string QLOG_REWRITE_FLAG = "qlog_rewrite_flag";

const std::string CONTEXT_CLK_URLS_MERGE_KEY = "clk_urls_merge_key";
const std::string CONTEXT_CLK_NORMAL_URLS_MERGE_KEY = "clk_normal_urls_merge_key";
const std::string KEY_CLK_QRS_SCORE_MAP = "clk_qrs_score_map";

const std::string KEY_CONTEXT_FAILED_NODE = "context_failed_node";

const std::string KEY_BACKUP_HIT_VEC = "back_up_hit_vec";
const std::string KEY_AUTHORITY_UNIT_VEC = "authority_unit_vec";
const std::string KEY_CATE_DOMAIN_AUTHORITY_SCORE = "cate_domain_authority_score";
const std::string KEY_OFFICIAL_HIT_V3_VEC = "official_hit_v3_vec";  // 存放官网队列召回的hit, 包含orig + anchor等hit
const std::string KEY_OFFICIAL_RECORD_VEC = "official_record_vec";  // 存放官网url record 
const std::string KEY_OFFICIAL_HIT_VEC = "official_hit_vec";
const std::string KEY_OFFICIAL_URL_VEC = "official_url_vec"; // 潜在挖掘的官网URL
const std::string KEY_OFFICIAL_TOP500_HIT_VEC = "official_top500_hit_vec"; // 存放top500 首页类的官网结果
const std::string KEY_OFFICIAL_SUBLINK_HIT_VEC = "official_sublink_hit_vec"; // 存放top500 官网子链结果
const std::string KEY_OFFICIAL_SUBLINK_HAS_LABEL = "official_sublink_has_label"; // 是否已经请求了sublink tair
const std::string KEY_OFFICIAL_SUBLINK_URL_SET = "official_sublink_url_set"; // 官网子链sublink结果url set
const std::string KEY_OFFICIAL_OFFICIAL_URL_HAS_LABEL = "official_url_has_label"; // 是否已经存储了official url set
const std::string KEY_OFFICIAL_OFFICIAL_URL_VEC = "official_sublink_url_vec"; // 官网子链结果official url set
const std::string KEY_URL_QUERY_VEC = "url_query_vec"; // query == url

const std::string KEY_OFFICIAL_OWFLAG = "official_ow_flag"; // 官标标志
const std::string KEY_OFFICIAL_SITELINK_FLAG = "official_sitelink_flag"; // sitelink


const std::string KEY_FEED_BACK_UP_HIT_VEC = "feed_back_up_hit_vec";

// serializable
const std::string KEY_MAIN_CACHE_RESULT = "main_cache_result";
const std::string KEY_RT_CACHE_INFO = "rt_cache_info";

const std::string SUMM_CACHE_READ_STATUS = "summ_cache_read_status";
const std::string SUMM_CACHE_DIFF_STATUS = "summ_cache_diff_status";

const std::string KEY_INDEX_MAIN_CACHE_INFO = "index_main_cache_info";
const std::string KEY_INDEX_RT_CACHE_INFO = "index_rt_cache_info";
const std::string KEY_FEATURE_MAIN_CACHE_INFO = "feature_main_cache_info";

// index
const std::string KEY_BIG_INDEX_SEARCH_STATUS = "big_index_search_status"; // BigIndex访问标志
const std::string KEY_SECOND_SEARCH_STATUS = "second_search_status";       // 索引二查访问标志
const std::string KEY_SUBCONTEXT_SPELL_CHECK_LEVEL = "subcontext_spell_check_level";   
const std::string KEY_QRS_FAILED = "qrs_failed";
const std::string KEY_QRS_ERROR = "qrs_error";
const std::string KEY_SUMMARY_FAILED = "summary_failed";
const std::string KEY_SUMMARY_ERROR = "summary_error";
const std::string KEY_AT_SECOND = "at_second";                            // NeedATSecond状态标志
const std::string KEY_WENDA_COUNT = "wenda_count";                        // SubContext QA索引的TotalHitCount
const std::string KEY_RT_HIT_COUNT = "rt_hit_count";                      // SubContext Rt索引的TotalHitCount
const std::string KEY_RT_INTENTION_SCORE = "rt_intention_score";          // SubContext RtIntentionScore
const std::string KEY_TIMELINESS_REGRESSION_FEATURE = "timeliness_regression_feature";  // SubContext TimelinessRegressionFeature
const std::string KEY_TIMELINESS_REGRESSION_VALUE = "timeliness_regression_value";      // SubContext TimelinessRegressionValue
const std::string KEY_REQUEST_READ_CACHE_FLAG = "request_read_cache_flag";              // read_cache flag

//qp
const std::string KEY_APP_REWRITE = "app_rewrite";
const std::string KEY_QP_PATTERN_RW = "pattern_rw";
const std::string KEY_ARBITER_PATTERN_RW = "pattern_rw";
const std::string KEY_QP_RAW_RESULT_STR = "qp_raw_result_str";
const std::string KEY_QU_RESULT_STR = "qu_result_str";
// subcontext
//const std::string SPELLCHECK_SUBCONTEXT_NAME = "spell_check";
const std::string KEY_SUBCONTEXT_TYPE = "subcontext_type";
const std::string KEY_ORIGIN_TYPE = "origin_type";
const std::string KEY_RECALL_TYPE = "recall_type";
const std::string KEY_EMPTY_TYPE  = "empty_type";
const std::string KEY_SUBCONTEXT_QUERY = "subcontext_query";
const std::string KEY_REWRITE_QUERY = "rewrite_query";
const std::string KEY_SUBCONTEXT_SPELL_CHECK = "spellcheck_level";
const std::string KEY_SUBCONTEXT_SPELL_CHECK_EXT_INFO  = "SpellCheckExtInfo";
const std::string KEY_SUBCONTEXT_ADULT_LEVEL = "adult_level";
const std::string KEY_SP_ENTITY_MATCH_RESULT = "entity_match_result";
const std::string KEY_QUERYREWRITE_SUBCONTEXT_NAME = "query_rewrite";
const std::string KEY_SUBCONTEXT_SCRESULT_COLLECTOR = "subcontext_scresult_collector";
const std::string KEY_SUBCONTEXT_INTENTION_RESULT = "intention_result";
const std::string KEY_SUBCONTEXT_INTENTION_RESULT_NEW = "intention_result_new";
const std::string KEY_SUBCONTEXT_INTENTION_EXTEND_STATUS = "subcontext_intention_extend";
const std::string KEY_QUERY_REWRITE_CACHE_STATE = "query_rewrite_cache_state";
const std::string KEY_QUERY_REWRITE_CACHE_VALUE = "query_rewrite_cache_value";
const std::string KEY_QLOG_QUERY_REWRITE_CACHE_STATE = "qlog_query_rewrite_cache_state";
const std::string KEY_QLOG_QUERY_REWRITE_CACHE_VALUE = "qlog_query_rewrite_cache_value";
const std::string KEY_ADDITION_CLICKINFO = "addition_click_info";
const std::string KEY_SUBCONTEXT_NO_SUG = "no_sug";
const std::string KEY_SUBCONTEXT_MULTI_INTENTION = "multi_intention";
const std::string KEY_SUBCONTEXT_QUERY_CLASSIFICATION = "query_classification";
const std::string KEY_AUTHORITY_TYPE  = "authority_type";
const std::string KEY_SUBCONTEXT_MAX_CLICK_SCORE = "max_click_score";
const std::string KEY_CONTEXT_PROTECT_SC = "protect_sc";
const std::string KEY_CLICK_SERVER_DATA_RESPONSE = "click_server_data_response";
const std::string KEY_VPTCLK_SERVER_DATA_RESPONSE = "vptclk_server_data_response";
const std::string KEY_AUTHORITY_RECALL_REASON  = "authority_recall_reason";
const std::string KEY_SUBCONTEXT_FAILED_NODE = "subcontext_failed_node";
const std::string KEY_CONTEXT_MEDICAL_TRIGGER_FEATURE = "medical_trigger_feature";
const std::string KEY_SUBCONTEXT_CLICK_NAVI_LEVEL = "click_navi_level";
const std::string KEY_SUBCONTEXT_TOP5_AVG_TEXT_SCORE = "top5_avg_text_score";
const std::string KEY_SUBCONTEXT_FEATURE_MAP_NAME = "subcontext_feature_map";
const std::string KEY_NEWS_START_POS = "NEWS_START_POS";
const std::string KEY_REPLACE_BY_SPELLCHECK = "replace_by_spellcheck";

// 多队列
const std::string KEY_QUEUE_SCORE_VEC_PREFIX = "queue_score_vec_";
const std::string KEY_QUEUE_SCORE_GAP_PREFIX = "queue_score_gap_";

// intention
const std::string KEY_INTENTION_RESULT = "intention_result";
const std::string KEY_CONTENT_INTENTION_RESULT = "content_intention_result";  // AggContext content_intention_result key, 对应原来CONTEXT_CONTENT_INTENTION_RESULT_KEY
const std::string KEY_MEDICAL_INTENTION_RESULT = "medical_intention_result";  // AggContext medical_intention_result key, 对应原来CONTEXT_MEDICAL_INTENTION_RESULT_KEYU
const std::string RESULT_KEY_QUERY_AOL = "query_aol_result";                  // AggContext query_aol_result key, 对应原来CONTEXT_CONTENT_QUERY_AOL_RESULT_KEY
const std::string KEY_JITTER_SOURCE_ID = "jitter_sourceid";

//shield result
const std::string RESULT_KEY_QUERY_SHIELD = "query_shield_result";
const std::string RESULT_KEY_SPELLCHECK_QUERY_SHIELD = "spellcheck_query_shield_result";

// pv click
const std::string KEY_PV_CLICK_INFO = "click_pv_info_vec"; 
const std::string KEY_PV_CLICK_INFO_ORIG = "click_pv_info_vec_orig"; 
const std::string CLICK_DOC_VEC = "click_doc_vec"; 
const std::string CLICK_SC_VEC = "click_sc_vec";
const std::string CLICK_DESCSC_VEC = "click_descsc_vec";
const std::string CLICK_DESC_NOVEL_KEY = "click_desc_novel"; 
const std::string KEY_CLICK_RECALL_POS = "click_recall_pos";
const std::string KEY_FIRST_ARBITER_SC_SET = "first_arbiter_sc_set";
const std::string KEY_ALL_ARBITER_SC_SET = "all_arbiter_sc_set";
const std::string CLICK_SIM_REQ_KEY = "click_doc_sim_key";
const std::string CLICK_SIM_DOC_KEY = "click_doc_sim_set"; 
const std::string KEY_CLICK_TOP_URL_SET = "click_top_url_set";
const std::string KEY_HOST_URL_SET = "host_url_set";
const std::string KEY_HOST_URL_REQUERY = "host_url_requery";

// regroup v2
const std::string KEY_REGROUP_SUBNAME = "regroup_subname_new";

// queryLog
const std::string KEY_QUERY_LOG_INFO_VEC = "query_log_info_vec";    
const std::string KEY_BD_QUERY_LOG_INFO_V2_VEC = "bd_query_log_info_v2_vec"; // Toutiao QueryLogInfoVec   // Baidu QueryLogInfoVec
const std::string KEY_TT_QUERY_LOG_INFO_VEC = "tt_query_log_info_vec"; // Toutiao QueryLogInfoVec
const std::string KEY_TT_QUERY_LOG_INFO_V2_VEC = "tt_query_log_info_v2_vec"; // Toutiao QueryLogInfoVec
const std::string KEY_PRE_TT_QUERY_LOG_INFO_VEC = "pre_tt_query_log_info_vec"; // Toutiao QueryLogInfoVec
const std::string KEY_QLOG_REWRITE_INFO_VEC = "qlog_rewrite_info_vec"; 
const std::string KEY_QUERY_LOG_HIT_VEC = "query_log_hit_vec";
const std::string KEY_QUERY_LOG_HIT_URL_VEC = "query_log_hit_url_vec";
const std::string KEY_QUERY_LOG_HIT_HASH_ID_VEC = "query_log_hit_hash_id_vec";
const std::string QLOG_INFO = "qlog_info";
const std::string QUERYLOG_ADJUST_KEY = "querylog_adjust";
const std::string QUERY_LOG_CACHE_VALUE_KEY = "query_log_cache_value";
const std::string QUERY_LOG_CACHE_STATE_KEY = "query_log_cache_state";
const std::string KEY_TT_QUERY_LOG_SC_INFO_VEC = "tt_query_log_sc_info_vec";
const std::string KEY_BD_QUERY_LOG_SC_INFO_VEC = "bd_query_log_sc_info_vec";
const std::string KEY_PRE_TT_QUERY_LOG_SC_INFO_VEC = "pre_tt_query_log_sc_info_vec";
const std::string KEY_PRE_GQF_BD_QUERY_LOG_SC_INFO_VEC = "pre_gqf_bd_query_log_sc_info_vec";
const std::string KEY_PRE_GQF_TT_QUERY_LOG_SC_INFO_VEC = "pre_gqf_tt_query_log_sc_info_vec";
const std::string KEY_PRE_GQF_QUERY_LOG_DOC_FLAG_INFO = "pre_gqf_query_log_doc_flag_info";

const std::string KEY_QLOG_PREDICT_VEC = "qlog_hit_predict_vec";
const std::string KEY_QLOG_TOP_URL_SET = "qlog_top_url_set";
const std::string KEY_QLOG_MONITOR_MAP = "qlog_monitor_map";

// for querylog timeliness
const std::string KEY_PRE_QUERY_LOG_TIMELINESS_SIGNAL = "pre_query_log_timeliness_signal";
const std::string KEY_PRE_BD_QUERY_LOG_TIMELINESS_SIGNAL = "pre_query_bd_log_timeliness_signal";
const std::string KEY_TT_QUERY_LOG_SC_INFO_NEW_VEC = "tt_query_log_sc_info_new_vec";
const std::string KEY_PRE_TT_QUERY_LOG_TIMELINESS_SIGNAL = "pre_tt_query_log_timeliness_signal";
const std::string KEY_PRE_TT_QUERY_LOG_HIT_POS = "pre_tt_query_log_hit_pos";
const std::string KEY_PRE_BD_QUERY_LOG_HIT_POS = "pre_bd_query_log_hit_pos";
const std::string KEY_PRE_TT_QUERY_LOG_HIT_TIME = "pre_tt_query_log_hit_time";
const std::string KEY_PRE_BD_QUERY_LOG_HIT_TIME = "pre_bd_query_log_hit_time";
const std::string KEY_PRE_TT_QUERY_LOG_VALUE = "pre_tt_query_log_value";
const std::string KEY_PRE_BD_QUERY_LOG_VALUE = "pre_bd_query_log_value";
const std::string KEY_PRE_TAIR_BD_QUERY_LOG_VALUE = "pre_tair_bd_query_log_value";
const std::string KEY_PRE_QUERY_LOG_INFO_VEC = "pre_query_log_info_vec";

// site
const std::string KEY_OFFICIAL_NEED_LABEL = "official_need_label";
const std::string KEY_OFFICIAL_CERTAINTY = "official_certainty";
const std::string KEY_OFFICIAL_RECALL_ORIGINAL = "official_recall_original";
const std::string KEY_OFFICIAL_URL_DETAIL = "official_url_detail";


const std::string KEY_SITE_ALEXA_RANK_KEY = "site_alexa_rank_map";

// interleaving
const std::string KEY_INTERLEAVING_STATUS = "interleaving_status";
const std::string KEY_L4_SKIP_QUERY = "l4_skip_query";
const std::string KEY_L4_SKIP_VEDIO = "l4_skip_vedio";


// result_key
const std::string RESULT_KEY_SC_AOL = "sc_aol_result";  // AggContext ScAolResult key, 对应于原来的CONTEXT_SC_AOL_RESULT_KEY
const std::string RESULT_KEY_ORION = "orion_result";    // ORION_RESULT_KEY
const std::string RESULT_KEY_TAG_REC = "tag_rec_result";    // TAG_REC_RESULT_KEY,泛需求搜索tag推荐
const std::string RESULT_KEY_UGC_VIDEO_RECOMMEND = "ugc_video_recommend_result";  // AggContext UGCVideoRecommendResult key, 对应于原来的CONTEXT_UGC_VIDEO_RECOMMEND_RESULT_KEY
const std::string RESULT_KEY_UADS = "uads_result";   // AggContext UADS result key
const std::string KEY_ARBITER_RETRIGGER_SC_SET = "arbiter_retrigger_sc_set";
const std::string RESULT_KEY_ARBITER_RETRIGGER = "arbiter_retrigger_result";    // AggContext ArbiterRetriggerResult key, 对应于原来的CONTEXT_ARBITER_RETRIGGER_RESULT_KEY
const std::string RESULT_KEY_SUMMARY_MODIFY = "summary_modify_result";          // AggSubContext SummaryModify Result key

const std::string KEY_CONFIG_SERVER_INFO = "config_server_info";   // ConfigServer结果字符串

const std::string KEY_AGG_MED_SC_RESULT = "agg_med_sc_result";   // agg med sc结果
const std::string KEY_AGG_FORE_SC_RESULT = "agg_fore_sc_result";   // agg sc一查结果
const std::string KEY_AGG_REAR_SC_RESULT = "agg_rear_sc_result";   // agg sc二查结果

// trace
const std::string REQUEST_TRACE_SC_APP_KEY = "trace_scapp";

// result_format
const std::string REQUEST_NEED_FIELDS_KEY = "need_fields";
// s3trace
const std::string REQUEST_S3TRACE_KEY = "s3trace";
// spl 
const std::string REQUEST_ISUC_KEY = "isuc";

// commit manner
const std::string REQUEST_BY_KEY = "by";


// sc
const std::string KEY_NEWS_RELETED_POS = "NEWS_RELETED_POS";

// reranker
const std::string KEY_QRS_STATISTICS = "qrs_statistics";

// feed
const std::string HQC_DEFAULT_URL = "hqc_default_url";
const std::string DNN_AT_ID_ATTR2 = "DNN_AT_ID_ATTR2";
const std::string RESULT_KEY_FEED_MERGE = "feed_merge_result_key";  // AggContext FeedMergeResult key, 对应于原来的CONTEXT_FEED_MERGE_RESULT_KEY
const std::string REPLACE_URL = "replace_url";
const std::string KEY_FEED_DEDUP_TITLE = "feed_dedup_title";

// adult_boost
const std::string KEY_ADULT_BOOST = "adult_boost";

// ======================= DataName ========================================
const std::string DATA_ACL_DATA = "acl_data";
const std::string DATA_XISHUASHUA_DATA = "xishuashua_data";
const std::string DATA_FEED_TAG_DATA = "feed_tag_data";
const std::string DATA_RTINDEX_MERGE_MODEL = "rtindex_merge_model";
const std::string DATA_INDEX_INTERLEAVING_DATA = "index_interleaving_data";
const std::string DATA_NEWS_TYPE_CLASSIFICATION_MODEL = "news_type_classification_model";
const std::string DATA_RTINDEX_MERGE_MODEL_NEWS = "rtindex_merge_model_news";
const std::string DATA_FEED_MAKEUP_DICT = "feed_makeup_dict";
const std::string DATA_FEED_MAKEUP_STOP_DICT = "feed_makeup_stop_dict";
const std::string DATA_ONLINE_DEDUP_CONFIG = "multi_online_dedup_config";
const std::string DATA_SC_DATA_SCHEMA = "sc_data_schema_config";
const std::string DATA_SC_CONFIG = "sc_config";
const std::string DATA_ADJUSTER_CONFIG = "adjuster_config";
const std::string DATA_TIMELINESS_CONFIG = "timeliness_config";
const std::string DATA_INTENTION_TEMPLATE_CONFIG = "intention_template_config";
const std::string DATA_SITE_LINK = "site_link";
const std::string DATA_SITE_LINK2 = "site_link2";
const std::string DATA_SITE_LINK3 = "site_link3";
const std::string DATA_STOPWORD = "stopword";
const std::string DATA_TIMELINESS_SITE = "timeliness_site";
const std::string DATA_HOST_INTENTION = "host_intention";
const std::string DATA_DETAIL_INTENTION = "detail_intention";
const std::string DATA_NOVEL_PATTERN_DATA = "novel_pattern_data";
const std::string DATA_SC_DEDUP = "sc_dedup";
const std::string DATA_URL_DEDUP = "url_dedup";
const std::string DATA_URL_DEMOTE = "url_demote";
const std::string DATA_CITY_EXTERNAL_MERGE = "city_external_merge";
const std::string DATA_SPREAD_MAKEUP = "spread_makeup";
const std::string DATA_MULTI_SITE = "multi_site";
const std::string DATA_HOST_WEIGHT = "host_weight";
const std::string DATA_NEWS_SITE = "news_site";
const std::string DATA_ADULT_SITE = "adult_site";
const std::string DATA_BBS_SITE = "bbs_site";
const std::string DATA_MOBILE_FRIENDLY_SITE = "mobile_friendly_site";
const std::string DATA_LR_MODEL = "lr_model";
const std::string DATA_ENTITY_RECOMMEND = "entity_recommend";
const std::string DATA_HOST_ICON = "host_icon";
const std::string DATA_ENTITY_RECOMMEND_BLACK = "entity_recommend_black";
const std::string DATA_ENTITY_RECOMMEND_BLACK_QUERY = "entity_recommend_black_query";
const std::string DATA_ENTITY_QUALITY = "entity_quality";
const std::string DATA_SITE_AUTHORITY = "site_authority";
const std::string DATA_DOMAIN_AUTHORITY = "domain_authority";
const std::string DATA_DOMAIN_AUTHORITY_ID_ALIGN = "domain_authority_id_align";
const std::string DATA_DOMAIN_AUTHORITY_NEW = "domain_authority_new";
const std::string DATA_DOMAIN_AUTHORITY_ID_ALIGN_NEW = "domain_authority_id_align_new";
const std::string DATA_DOMAIN_AUTHORITY_ID_ALIGN_2 = "domain_authority_id_align_2";
const std::string DATA_DOMAIN_QUALITY = "domain_quality";
const std::string DATA_NOVEL_NONCOOP_URL = "novel_noncoop_url";
const std::string DATA_NOVEL_ADJUST_DATA = "novel_adjust_data";
const std::string DATA_TIMELINESS_TRIGGER_MODEL = "timeliness_trigger_model";
const std::string DATA_TIMELINESS_TRIGGER_MODEL_NEW = "timeliness_trigger_model_new";
const std::string DATA_TIMELINESS_RERANK_MODEL = "timeliness_rerank_model";
const std::string DATA_TIMELINESS_RERANK_MODEL_NEW = "timeliness_rerank_model_new";
const std::string DATA_AUTH_TRIGGER_MODEL = "auth_trigger_model";
const std::string DATA_AUTH_RERANK_MODEL = "auth_rerank_model";
const std::string DATA_MEDICAL_RERANK_MODEL = "medical_rerank_model";
const std::string DATA_MEDICAL_AUTH_MODEL = "medical_auth_model";
const std::string DATA_AGG_QRCLICK_MODEL = "agg_qrclick_model";
const std::string DATA_QUERYLOG_MODEL = "querylog_model";
const std::string DATA_QUERYLOG_MODEL_V2 = "querylog_model_v2";
const std::string DATA_AT_MODEL = "at_model";
const std::string DATA_TT_QUERYLOG_MODEL_V1 = "tt_querylog_model_v1";
const std::string DATA_TT_QUERYLOG_MODEL_V2 = "tt_querylog_model_v2";
const std::string DATA_NOVEL_FILTER_HOST = "novel_filter_host";
const std::string DATA_NOVEL_SHIELD = "novel_shield_data";
const std::string DATA_QC_BERT_TOKEN = "qc_bert_token";
const std::string DATA_QC_TERM_WEIGHT = "qc_term_weight";
const std::string DATA_QC_STOP_WORD = "qc_stop_word";
const std::string DATA_POLITICAL_NEWS_SITE = "political_news_site";
const std::string DATA_HIGH_QUALITY_HOST = "high_quality_host";
const std::string DATA_URL_PATTERN = "url_pattern";
const std::string DATA_GENERAL_TIMELINESS_REGEX = "general_timeliness_regex";
const std::string DATA_GENERAL_TIMELINESS_TERM = "general_timeliness_term";
const std::string DATA_SC_TYPE_MAPPING_DATA = "sc_type_mapping_data";
const std::string DATA_SPEED_DATA = "speed_data";
const std::string DATA_SITE_AGGREGATOR_PATTERN = "site_aggregator_pattern";
const std::string DATA_TYPE_SC_MAPPING_DATA = "type_sc_mapping_data";
const std::string DATA_SC_LIST_DATA = "sc_list_data";
const std::string DATA_RELEATE_SC_DATA = "releate_sc_data";
const std::string DATA_TYPE_CUTOFF_THRESHOLD_DATA = "type_cutoff_threshold_data";
const std::string DATA_SC_CUTOFF_THRESHOLD_DATA = "sc_cutoff_threshold_data";
const std::string DATA_SC_NAME_APP_MAP_DATA = "sc_name_app_map_data";
const std::string DATA_FEED_WORDID_100W = "feed_wordid_100w";
const std::string DATA_FEED_WORDID_100W_REVERSE = "feed_wordid_100w_reverse";
const std::string DATA_FEED_WORDID_STOPWORD = "feed_wordid_stopword";
const std::string DATA_SC_TO_QCATE_DICT = "sc_to_qcate_dict";
const std::string DATA_UC_CATE_TO_QCATE_DICT = "uc_cate_to_qcate_dict";
const std::string DATA_SC_WENDA_EVADE_DATA = "sc_wenda_evade_data";
const std::string DATA_SC_QTC_DATA = "sc_qtc_data";
const std::string DATA_SC_QTC_V2_DATA = "sc_qtc_data_v2";
const std::string DATA_SC_QTC_MAKEUP_DATA = "sc_qtc_makeup_data";
const std::string DATA_SC_APP_DATA = "sc_app_data";
const std::string DATA_NEWS_HOST_QUALITY = "news_host_quality_data";
const std::string DATA_OFFICIAL_SITE_ONLINE = "official_site_online";
const std::string DATA_OFFICIAL_SITE_ONLINE_V2 = "official_site_online_v2";
const std::string DATA_OFFICIAL_TOP_SC_FILTER = "official_top_sc_filter";
const std::string DATA_OFFICIAL_DOCUMENT_ONLINE = "official_document_online";
const std::string DATA_SC_RANK_MODEL_AFFECT_SC = "sc_rank_model_affect_sc";
const std::string DATA_TERM_INDEX_COUNT = "term_index_count_data";
const std::string DATA_NU_QUERY= "nu_query";
const std::string DATA_MAKEUP_FOLLOWUP = "makeup_followup";
const std::string DATA_MAKEUP_FOLLOWUP_FIX = "makeup_followup_fix";
const std::string DATA_SC_EXCLUSIVE = "sc_exclusive";
const std::string DATA_BERT_DICT_TERM = "bert_dict_hashmap_term";
const std::string DATA_BERT_DICT_PUNC = "bert_dict_hashmap_punc";
const std::string DATA_BERT_DICT_CONTROL = "bert_dict_hashmap_control";
const std::string DATA_SEMANTIC_QUERY_TERM = "semantic_query_term";
const std::string DATA_PUNC = "bert_segment_unicode_punc_data";
const std::string DATA_CTRL = "bert_segment_unicode_ctrl_data";
const std::string DATA_BERT_WORD_ID_MAPPING = "bert_word_id_mapping_data";
const std::string DATA_FAISS_QU_LABEL = "faiss_qu_label_data";
const std::string DATA_TIMELINESS_FRESH_MAKEUP = "timeliness_fresh_makeup_data";
const std::string DATA_TOOL_SC_NAME = "tool_sc_name_data";
const std::string DATA_SUMMARY_MAKEUP = "summary_makeup_data";
const std::string DATA_SUMMARY_RANK_POS = "summary_rank_pos_data";
const std::string DATA_HOST_DEMOTE = "host_demote_data";
const std::string DATA_INDUSTRY_BLACK_HOST = "industry_black_host_data";
const std::string DATA_HOST_COST = "host_cost_data";
const std::string DATA_CATE_DOMAIN_AUTHORITY = "cate_domain_authority_data";
const std::string DATA_CATE_DOMAIN_AUTHORITY_V2 = "cate_domain_authority_data_v2";
const std::string DATA_CATE_DOMAIN_AUTHORITY_V3 = "cate_domain_authority_data_v3";
const std::string DATA_CATE_DOMAIN_AUTHORITY_ADD_CLICK_HOST = "cate_domain_authority_data_add_click_host";
const std::string DATA_CATE_DOMAIN_AUTHORITY_REVIEW_V1 = "cate_domain_authority_data_review_v1";
const std::string DATA_CATE_HOST_SCORE_REVIEW_V1 = "cate_host_authority_score_review_v1";
const std::string DATA_SPM_FEATURE_DISCRETIZE = "spm_feature_discretize_data";
const std::string DATA_SPM_HOST_ID_FEATURE = "spm_host_id_feature_data";
const std::string DATA_SPM_RECALL_SC_ID_FEATURE = "spm_recall_sc_id_feature_data";
const std::string DATA_SPM_RECALL_SC_ID_FEATURE_V2 = "spm_recall_sc_id_feature_v2_data";
const std::string DATA_SPM_RECALL_SC_ID_FEATURE_V4 = "spm_recall_sc_id_feature_v4_data";
const std::string DATA_SPM_RECALL_ID_SC_FEATURE_V4 = "spm_recall_id_sc_feature_v4_data";
const std::string DATA_SPM_PREDICT_ID_INFO = "spm_predict_id_info_data";
const std::string DATA_SPM_PREDICT_ID_INFO_NEW = "spm_predict_id_info_new_data";
const std::string DATA_SPM_PREDICT_ID_INFO_V2 = "spm_predict_id_info_v2_data";
const std::string DATA_SPM_PREDICT_ID_INFO_V2_RC2 = "spm_predict_id_info_v2_rc2_data";
const std::string DATA_SPM_PREDICT_ID_INFO_V4 = "spm_predict_id_info_v4_data";
const std::string DATA_SC_ADJUST_INFO = "sc_adjust_info_data";
const std::string DATA_SC_ADJUST_INFO_EXP = "sc_adjust_info_data_exp";
const std::string DATA_EDU_OPT = "edu_opt_data";
const std::string DATA_EDU_QUERY_OFFI_MARK = "edu_query_offi_mark_data";
const std::string DATA_EDU_QUERY_HIT_POS = "edu_query_hit_pos_data";
const std::string DATA_OLD_SC_OFFLINE = "old_sc_offline_data";
const std::string DATA_INTENTION_THRESHOLD = "intention_threshold_data";
const std::string DATA_EDU_DIVERSION_TRIGGER = "edu_diversion_trigger_data";
const std::string DATA_FILTER_QUERY_DATA = "filter_query_data";
const std::string DATA_ZHIDA_PAGE_HOST_DATA = "zhida_page_host_data";
const std::string DATA_FORBIDDENT_HOST_DATA = "forbiddent_host_data";
const std::string DATA_SITE_RESPONSE_LEVEL_DATA = "site_response_level";
const std::string DATA_TT_QLOG_SC_SM_DATA = "tt_qlog_sc_sm_data";
const std::string DATA_APP_WHITE_LIST_DICT = "app_white_list_data";
const std::string DATA_QUERY_PRIOR_INTENTION_DICT = "query_prior_intention_data";
const std::string DATA_QUERY_PRIOR_INTENTION_LV3_DICT = "query_prior_intention_data_lv3";
const std::string DATA_QUERY_POSTER_INTENTION_DICT = "query_poster_intention_data";
const std::string DATA_QUERY_POSTER_INTENTION_DOC_DICT = "query_poster_intention_doc_data";
const std::string DATA_HOST_ID_DICT = "host_id_gx_data";
const std::string DATA_PAGE_FILTER_LABEL_DATA = "page_filter_label_data";
const std::string DATA_PAGE_FILTER_WHITE_DATA = "page_filter_white_data";
const std::string DATA_GAME_SITE_AUTHORITY_DATA = "game_site_promotion_data";
const std::string DATA_JXWD_INTENT_EXTEND_QS = "jxwd_intent_extend_qs";
const std::string DATA_SHIELD_CITYTYPE = "shield_citytype";
const std::string DATA_PAGE_SHIELD_STRATEGY_CONFIG = "page_shield_strategy_config";
const std::string DATA_DEGRADE_PARAM_MANAGER = "degrade_param_manager";
const std::string DATA_PAGE_SHIELD_LABEL_MAPPING = "page_shield_label_mapping";

const std::string KEY_QUERY_PRIOR_INTENTION_SC_SCORE_MAP = "query_prior_intention_sc_score_map";
const std::string KEY_QUERY_PRIOR_INTENTION_SC_THRESHOLD_MAP = "query_prior_intention_sc_threshold_map";
const std::string KEY_QUERY_POSTER_INTENTION_SC_THRESHOLD_MAP = "query_poster_intention_sc_threshold_map";
const std::string KEY_QUERY_POSTER_INTENTION_SC_SCORE_MAP = "query_poster_intention_sc_score_map";
const std::string KEY_QUERY_POSTER_INTENTION_LV2_SCORE_VEC = "query_poster_intention_lv2_score_vec";
const std::string KEY_QUERY_PRIOR_INTENTION_LV2_SCORE_VEC = "query_prior_intention_lv2_score_vec";
const std::string KEY_QUERY_POSTER_INTENTION_LV2_SCORE_MAP = "query_poster_intention_lv2_score_map";
const std::string KEY_QUERY_POSTER_INTENTION_LV2_THRESHOLD_MAP = "query_poster_intention_lv2_threshold_map";
const std::string KEY_QUERY_POSTER_INTENTION_SC_LV2_MAP = "query_poster_intention_sc_lv2_map";
const std::string KEY_QUERY_PRIOR_INTENTION_LV2_SCORE_MAP = "query_prior_intention_lv2_score_map";
const std::string KEY_QUERY_PRIOR_INTENTION_LV3_SCORE_MAP = "query_prior_intention_lv3_score_map";
const std::string KEY_QUERY_PRIOR_INTENTION_LV2_THRESHOLD_MAP = "query_prior_intention_lv2_threshold_map";
const std::string KEY_QUERY_PRIOR_INTENTION_LV3_THRESHOLD_MAP = "query_prior_intention_lv3_threshold_map";
const int QUERY_PRIOR_INTENTION_LV3_NUM = 128;
const std::string KEY_GENERATION_RECALL_MAP = "query_generation_recalled_url_map";
// hit
const std::string QRS_DEBUG_SUBCONTEXT_CLICK_SCORE = "debug_sub_qrsclick_score";
const std::string QRS_DEBUG_SUBCONTEXT_CLICK_SCORE_TYPE = "debug_sub_qrsclick_score_type";
const std::string SC_DEBUG_SUBCONTEXT_CLICK_SCORE = "debug_sub_scclick_score";
const std::string SC_DEBUG_SUBCONTEXT_CLICK_SCORE_TYPE = "debug_sub_scclick_score_type";

// qpHit
const std::string QPHIT_RW_KEY = "rw";
const std::string QPHIT_ADULT_LEVEL_KEY = "adult_level";
const std::string QPHIT_NO_SUG_KEY = "no_sug";
const std::string QPHIT_SPELL_CHECK = "spellcheck";
const std::string QPHIT_SPELL_CHECK_DEBUG = "spellCheckDebug";
const std::string QPHIT_MULTI_INTENTION = "multi_intention";
const std::string QPHIT_QUERY_CLASSIFICATION = "qc_result";

// qrsclause
const std::string QRS_KV_ADULT_DEMOTE_KEY = "adult_demote";
const std::string QRS_REQUEST_MULTI_EXP_INFO_KEY = "QRS_REQUEST_MULTI_EXP_INFOS";
const std::string QRS_REQUEST_MULTI_EXP_SCORE_KEY = "QRS_REQUEST_MULTI_EXP_SCORES";
const std::string QRS_CLAUSE_NO_RESTORE = "qrs_clause_no_restore";
const std::string KEY_MODIFY_EXP_CLOSE_SECOND_SEARCH = "MODIFY_EXP_CLOSE_SECOND_SEARCH";

// sc
const std::string GENERANL_INFO_UC_BANNER_KEY = "ucbanner";
const std::string UC_BANNER = "ucbanner";
const std::string MAIN_CONTENT_KEY    = "main_content";
const std::string KEY_LINK_VIEW_DATA = "linkViewData";
const std::string KEY_SPREAD = "spread";
const std::string KEY_CLICK_SCORE = "click_score";
const std::string MAKEUP_POS_KEY      = "MakeUpPos";
const std::string KEY_ETAO_DISPLAY = "etao_display_log";
const std::string KEY_GQF_CROSS_SIGANL_MAP = "gqf_cross_signal_map";
// request param
const std::string SPREAD_GROUP_NAME_KEY = "spread";

// 流量来源
const std::string REQUEST_BELONG_PARAM = "belong";
const std::string QUARK_BELONG_CODE = "quark";

//uc
const std::string RAW_BELONG_CODE = "raw_belong_code";
const std::string RAW_BELONG_CHANGED = "raw_belong_changed";
const std::string UC_TREAT_AS_KK = "uc_treat_as_kk";

// news
const std::string NEWS_START_POS_KEY = "NEWS_START_POS";

// qa
const std::string KEY_WENDA_INTENT = "wenda_intent";
// ==================sc_helper====================================================
const std::string KEY_SUGGEST_POS = "suggest_pos";
const std::string KEY_STRENGTH = "STRENGTH";

// For Debug
const std::string KEY_FINAL_RERANK_PROCESSOR_END_TIME = "final_rerank_processor_end_time";
const std::string KEY_QRS_HIT_VEC_MD5 = "qrs_hit_vec_md5";
const std::string KEY_TOP_QRS_HIT_VEC_MD5 = "top_qrs_hit_vec_md5";

// Hit VAR
const std::string KEY_DISPLAY_COUNT = "display_count";

const std::string KEY_MULTI_TAB = "multi_tab";
const std::string KEY_SUBCONTEXT_DISPLAY_INFO = "subcontext_display_info";

// medical
const std::string MED_LABEL = "MED_LABEL";
const std::string MED_INTENTION = "MED_INTENTION";
const std::string MED_HISTORY_INFO = "MED_HISTORY_INFO";
const std::string MED_INTENT_INFO = "MED_INTENT_INFO";
const std::string MED_NER_INFO = "MED_NER_INFO";
const std::string MED_DECISION_INFO = "MED_DECISION_INFO";
const std::string MED_QUESTION_LLM_INFO = "MED_QUESTION_LLM_INFO";
const std::string MED_LABEL_V2 = "MED_LABEL_V2";
const std::string MED_SCORE = "MED_SCORE";
const std::string KEY_MEDICAL_DOMAIN_MATCH_COUNT = "MEDICAL_DOMAIN_MATCH_COUNT";
const std::string MED_TEXT_RECOMMEND_SHOW = "med_text_recommend_show";
const std::string MED_RECOMMEND_POS = "med_recommend_pos";

// summary modify
const std::string KEY_SUBCONTEXT_SUMMARY_MODIFY_HIT_VEC = "summary_modify_hit_vec";                       // SubContext请求SummaryModify的Hit列表
const std::string KEY_SUBCONTEXT_SUMMARY_MODIFY_REQUEST_HIT_COUNT = "summary_modify_request_hit_count";   // SubContext请求SummaryModify的Hit数量

// hit content key
const std::string KEY_REPLACE_URL = "replace_url";
const std::string KEY_RAW_URL = "RawUrl";
const std::string KEY_UC_DISPLAY_URL = "UC_DISPLAY_URL";
const std::string KEY_NORMALIZED_URL = "NormalizedUrl";
const std::string KEY_ARTICLE_TITLE = "ARTICLE_TITLE";
const std::string KEY_MAIN_BODY = "V_MAIN_BODY";

const std::string KEY_AUTHOR_NAME = "AuthorName";
const std::string KEY_CLICK_SIGNAL_ITEM_FEA = "ClickSignalItemFea";
const std::string KEY_COMMENT_COUNT = "CommentCount";
const std::string KEY_HOST_NAME = "HOST_NAME";
const std::string KEY_PAGE_ENTITY = "PageEntity";
const std::string KEY_SS_TITLE = "SS_TITLE";
const std::string KEY_UC_PV = "UC_PV";
const std::string KEY_UC_SOURCE_MEDIA = "UcSourceMedia";
const std::string KEY_VIEW_COUNT = "ViewCount";
const std::string KEY_WE_MEDIA_TAG = "WeMediaTag";
const std::string KEY_DNN_AT_ID_ATTR2 = "DNN_AT_ID_ATTR2";
const std::string IS_NU_QUERY = "isnuquery";

// 扩展debuger
const std::string KEY_QA_DEBUGER = "qa_debuger";

// recommend
const std::string KEY_SEMANTIC_RECOMMEND = "semantic_recommend";

// AE: Answer Extraction
const std::string KEY_AE_SUMMARY_HITS = "ae_summary_hits";

const std::string KEY_SEMANTIC_QRS_HIT_VEC = "semantic_qrs_hit_vec";

// BAIKE
const std::string KEY_BAIKE_CLICK_PROTECT = "baike_click_protect";

// query filter
const std::string KEY_IS_QUERY_FILTER = "is_query_filter";
const std::string KEY_QUERY_FILTER_VALUE = "query_filter_value";

// official
const std::string SOURCE_CATEGORY_SIGNAL = "source_category_signal";

// content
const std::string KEY_QI_QUERY_EMBEDDING = "qi_query_embedding";
const std::string KEY_IMAGE_INDEX_SERVER_DATA_RESPONSE = "image_index_server_data_response";

// query_parser_service 
const std::string RESULT_KEY_QUERY_PARSER_SERVICE = "query_parser_service_result";
const std::string RESULT_KEY_QUERY_UNDERSTAND_SERVICE = "query_understand_service_result";
const std::string DATA_PERSON_MAKEUP_DATA = "person_makeup_data";

// quark_qu
const std::string RESULT_KEY_QUARK_QU = "quark_qu_result";
const std::string QUARK_QU_DOMAIN_INFO_STR = "quark_qu_domain_info_str";
const std::string QUARK_QU_EXTRA_COMPLEX_INFO_STR = "quark_qu_extra_complex_info_str";

const std::string RESULT_KEY_QUARK_QU_POSTER = "quark_qu_poster_result";
const std::string KEY_QUARK_QU_POSTER_RESULT_STR =  "quark_qu_poster_result_str";
const std::string QUARK_QU_POSTER_DOMAIN_INFO_STR = "quark_qu_domain_info_str";

const std::vector<std::string> QUARK_QU_INTENT_NAME = {"animation", "society", "health", "naming", "dream", "express", "un-born", \
                                                       "car", "game", "film", "other", "estate", "music", "law", "novel", "un-internet", \
                                                       "food", "un-entertainment", "sports", "tv", "unknow", "history", "un-travel", \
                                                       "weather", "lottery", "constellation", "un-business", "politics", "love", "fortune", \
                                                       "show", "finance", "edu", "policy", "plant", "un-military", "animal", "3c", "holiday", \
                                                       "un-art", "soft"};
// 权威性重点行业
const std::vector<std::string> QUARK_QU_AUTHORITY_INTENT_NAME = {"game", "edu", "finance", "sports", "law", "policy", "3c"};
// 弱权威性行业：目前缺少娱乐、成人
const std::vector<std::string> QUARK_QU_UNAUTHORITY_INTENT_NAME = {"novel", "animation", "film", "tv", "show", "music", "health"};
// doc侧行业信号
// const std::vector<std::string> QUARK_QU_DOC_INTENT_NAME = {"其他", "教育", "培训考证", "求职就业", "知识", "体育", "游戏", "小说", "动漫", "电影", "电视剧", "综艺/节目", "音乐", "娱乐", "医疗健康", "成人/色情", "3c", "商业贸易(含购物)", "旅游", "出行", "财经", "汽车", "房产家居", "动物", "植物", "饮食菜谱", "星座生肖", "解梦", "软件", "互联网服务", "生产制造", "政策法律法规", "社会政治", "历史", "军事", "民俗文化", "邮编", "天气", "物流", "日历", "违章", "彩票", "计算器", "赌博", "其他", "新闻资讯", "情感", "占卜", "美术", "姓氏/取名", "购物"};
const std::vector<std::string> QUARK_QU_DOC_INTENT_NAME = {"other", "edu", "培训考证-无此类别", "求职就业-无此类别", "知识-无此类别", "sports", "game", "novel", "animation", "film", "tv", "show", "music", "ent", "health", "adult", "3c", "business", "travel", "transportation", "finance", "car", "estate", "animal", "plant", "food", "constellation", "dream", "soft", "internet", "生产制造-无此类别", "law", "politics", "history", "military", "folklore", "邮编-无此类别", "weather", "express", "holiday", "违章-无此类别", "lottery", "计算器-无此类别", "赌博-无此类别", "其他-无此类别", "新闻资讯-无此类别", "love", "fortune", "fineart", "naming", "shopping"};

//行业信号
const std::string IS_GAME_DOMAIN = "is_game_domain";
const std::string IS_SHOP_DOMAIN = "is_shop_domain";
const std::string IS_EDU_DOMAIN = "is_edu_domain";
// page shield
const std::string KEY_PAGE_FILTER_LOG = "page_filter";
const std::string KEY_PAGE_FILTER_TAG = "page_tag";
const std::string KEY_PAGE_FILTER_NUM_INFO_TAG = "page_stats";
const std::string KEY_PAGE_SHIELD_STRATEGY = "page_shield_strategy";
const std::string KEY_PAGE_SHIELD_STRATEGY_LOG = "shield_strategy";

const std::string WEIBO_CONTENTS = "weibo_contents";
const std::string IS_KUYING = "iskuying";
const std::string DATA_KUYING_QUERY = "kuying_query";
const std::string DATA_FIXPOS = "fixpos_data";
// timeliness
const std::string TIMELINESS_FRESH_ISVALID = "timeliness_fresh_isvalid"; // 1/0，是否时效性可用
const std::string TIMELINESS_FRESH_SCORE = "timeliness_fresh_score";  // 时效性分数

// query_generation
const std::string KEY_QUERY_GENERATION_RESULT = "query_generation_response";
const std::string KEY_QUERY_GENERATION_REVERSE_RESULT = "query_generation_response_reverse";
const std::string KEY_QUERY_GENERATIONS = "query_generations";
const std::string KEY_QUERY_GENERATION_QPHITS = "query_generation_qphits";
//city_external
const std::string KEY_RAW_QUERY = "raw_query";
const std::string KEY_CITY_QP_RESULT = "qp_result";
const std::string KEY_EXTERNAL_SUBCONTEXT_TYPE = "EXTERNAL_SUBCONTEXT_TYPE";

const std::set<std::string> RECRUIT_KEY = {"找工作", "招聘", "待遇", "薪资", "薪酬", "工资", "月薪", "年薪", "求职"};
const std::set<std::string> RECRUIT_URLS = {".jobui.com",".kanzhun.com",".zhipin.com",".hunt007.com",".liepin.com","wap-hunt007-com.sm-tc.cn",".51job.com",".job5156.com"};

//for damoyuan 来源自 agg.sm.cn, test.m.sm.cn 的流量
//关闭二查
//关闭at随机性
//关闭arbiter
//关闭医疗
const std::string REQUEST_SL_PARAM = "sl";
const std::string REQUEST_HOST_PARAM = "host";
const std::string REQUEST_HOST_AGGSMCN = "agg.sm.cn";
const std::string REQUEST_HOST_TESTSMCN = "test.m.sm.cn";
const std::string REQUEST_OSR_KEY = "osr";
const std::string DAMO_OSR_KEY = "damoyuan";
//module
const std::string MODULE_QUERY_REWRITE = "query_rewrite";
const std::string MODULE_QUERY_SECURITY = "query_security";
const std::string MODULE_QUERY_ANALYSIS = "query_analysis";
const std::string MODULE_CITY_EXTERNAL = "city_external";
const std::string MODULE_QCLASSIFY = "qclassify";
const std::string MODULE_AUTHORITY_EXTEND = "authority_extend";
const std::string MODULE_CLICK = "click";
const std::string MODULE_SPELLCHECK = "spellcheck";
const std::string MODULE_INTENTION = "intention";
const std::string MODULE_MEDICAL = "medical";
const std::string MODULE_RECOMMEND= "recommend";
const std::string MODULE_MEDICAL_QP= "medical_qp";
const std::string MODULE_TIMELINESS = "timeliness";
const std::string MODULE_QUERYLOG = "querylog";
const std::string MODULE_CSITE = "csite";
const std::string MODULE_DEMOTE = "demote";
const std::string MODULE_AUTHORITY = "authority";
const std::string MODULE_IDENTITY = "identity";
const std::string MODULE_SIMDOC = "simdoc";
const std::string MODULE_SC = "sc";
const std::string MODULE_QUARK_QU = "quark_qu";

//strategy param gig
const std::string METRIC_STRATEGY_PARAM_GIG_BIZ = "StrategyParamGig/biz/";
const std::string METRIC_STRATEGY_PARAM_GIG_STRATEGY = "strategy/";
const std::string METRIC_STRATEGY_PARAM_GIG_DEGRADE_QPS = "degradeQps";
const std::string METRIC_STRATEGY_PARAM_GIG_TOTAL_QPS = "totalQps";
const std::string METRIC_STRATEGY_PARAM_GIG_DEGRADE_RATIO = "degradeRatio";

//service result
const std::string KEY_SIGNAL_SERVICE_RESULT = "signal_service_result";
//multi app
const std::string KEY_SEARCH_APP_ENV_NAME = "SEARCH_APP_NAME";
const std::string SIGNAL_APP_NAME = "a4_agg_signal";
const std::string AGGWEB_APP_NAME = "a4_agg_web";
const std::string KEY_IS_SIGNAL_ISO_APP = "is_signal_iso_app";

enum AggSmCnSwitch {
    DISABLE_SECOND_REQUEST = 1,
    DISABLE_AT_REQUEST = 2,
    DISABLE_BIG_REQUEST = 4,
    DISABLE_ARBITER = 8,
    DISABLE_MEDICAL = 16
};

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB_COMMON_CONSTANTS_H
