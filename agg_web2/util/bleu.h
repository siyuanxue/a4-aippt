#ifndef A4_AGG_WEB_UTIL_BLEU
#define A4_AGG_WEB_UTIL_BLEU

#include <string>
#include <vector>
#include <A4/common/log.h>
#include <A4/common/common.h>
#include <agg2/common/agg_log.h>

AGG_BEGIN_NAMESPACE(agg_web2);

/*
 * 深度模型指标评估算法 Bleu
 * https://www.aclweb.org/anthology/P02-1040.pdf
 */
class Bleu
{
public:
    Bleu(size_t n = 4, bool utf8 = true);
    ~Bleu() = default;
    void AddInst(const std::string& cand, const std::string& ref, size_t max_len = 35);
    double GetScore(bool bp = false) const;
    double GetGram(size_t n) const;
    double GetBp() const;
private:
    void CountNgram(const std::string& cand, const std::string& ref, size_t n);
    std::vector<std::string> GetNgram(const std::string& str, size_t n);
    void GetMatchSize(const std::vector<std::string>& cand_ngram , const std::vector<std::string>& ref_ngram, size_t n);
    size_t mN;
    bool mUtf8;
    std::vector<size_t> mMatchNgram;
    std::vector<size_t> mCandiNgram;
    size_t mBpR; // 参考答案长度
    size_t mBpC; // 候选答案长度
private:   
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);
#endif
