#include <agg_web2/util/bleu.h>

#include <unordered_map>
#include <A4/util/string_util.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, Bleu);

using namespace std;

Bleu::Bleu(size_t n, bool utf8) : mN(n), mUtf8(utf8), mMatchNgram(4, 0), mCandiNgram(4, 0) {}

void Bleu::AddInst(const std::string& cand, const std::string& ref, size_t max_len)
{
    if(mUtf8) {
        mBpR = std::min(StringUtil::GetCountOfCharUTF8(ref), static_cast<unsigned int>(max_len));
        mBpC = StringUtil::GetCountOfCharUTF8(cand);
    } else {
        mBpR = std::min(ref.size(), max_len);
        mBpC = cand.size();
    }
    for(size_t i = 0; i < mN; ++i) {
        CountNgram(cand, ref, i);
    }
}

void Bleu::CountNgram(const std::string& cand, const std::string& ref, size_t n)
{
    // （1）获取候选答案、参考答案的n-gram字符list
    std::vector<std::string> cand_ngram = GetNgram(cand, n);
    std::vector<std::string> ref_ngram = GetNgram(ref, n);
    GetMatchSize(cand_ngram, ref_ngram, n);
}

void Bleu::GetMatchSize(const std::vector<std::string>& cand_ngram , const std::vector<std::string>& ref_ngram, size_t n)
{
    if(n >= mMatchNgram.size() || n >= mCandiNgram.size()) {
        return;
    }
    std::unordered_map<std::string, size_t> ref_cnt;
    std::unordered_map<std::string, size_t> cand_cnt;
    for(const std::string& ngram : ref_ngram) {
        ++ref_cnt[ngram];
    }
    for(const std::string& ngram : cand_ngram) {
        ++cand_cnt[ngram];
    }
    //（2）两者命中次数match_size
    size_t match_size = 0;
    for(auto iter = cand_cnt.begin(); iter != cand_cnt.end(); ++iter){
        auto it = ref_cnt.find(iter->first);
        if(it != ref_cnt.end()) {
            match_size += fmin(iter->second, it->second);
        }
    }
    mMatchNgram[n] = match_size;
    mCandiNgram[n] = cand_ngram.size();
}

std::vector<std::string> Bleu::GetNgram(const std::string& str, size_t n)
{
    std::vector<std::string> rst;
    if(mUtf8) {
        std::vector <std::string> utf8_vec;
        size_t idx = 0;
        while (idx < str.size()) {
            std::string utf8_str = StringUtil::GetNextCharUTF8(str, idx);
            if(utf8_str.empty()) break;
            utf8_vec.push_back(utf8_str);
            idx += utf8_str.size();
        }
        if((int(utf8_vec.size()) - int(n)) > 0) {
            rst.reserve(utf8_vec.size() - n);
            for(size_t left = 0; left < utf8_vec.size() - n; ++left)
            {
                std::string cur;
                for(size_t i = 0; i < n + 1; ++i) { // n is zero_based
                    cur += utf8_vec[left + i];
                }
                rst.push_back(cur);
            }
        }
    } else {
        if(str.size() > n)
        {
            rst.reserve(str.size() - n);
            for(size_t left = 0; left < str.size() - n; ++left)
            {
                rst.push_back(str.substr(left, n + 1)); // n is zero_based
            }
        }
    }
    return rst;
}
double Bleu::GetGram(size_t n) const
{
    if(n >= mMatchNgram.size() || n >= mCandiNgram.size()) return 0;
    if(mCandiNgram[n] == 0) return 0;
    return static_cast<double>(mMatchNgram[n]) / static_cast<double>(mCandiNgram[n]);
}
double Bleu::GetBp() const
{
    if (mBpR < mBpC) return 1.0;
    return static_cast<float>(mBpC) / mBpR;
}
double Bleu::GetScore(bool bp) const
{
    //n-gram bleu4分数
    double bleu_score = 1.0;
    for(size_t i = 0; i < mN; ++i){
        if(mCandiNgram[i] > 0) {
            bleu_score *= (static_cast<double>(mMatchNgram[i]) / mCandiNgram[i]);
        }
    }
    bleu_score = pow(bleu_score, 1.0/mN);
    if (!bp) return bleu_score;
    /*
    double bp = 1.0;
    if(mBpR >= mBpC) {
        bp = exp(1 - mBpR/float(mBpC));
    }*/
    return GetBp() * bleu_score;
}
AGG_END_NAMESPACE(agg_web2);
