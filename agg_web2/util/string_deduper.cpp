#include <agg_web2/util/string_deduper.h>
#include <set>
#include <algorithm>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, StringDeduper);

using namespace std;

int StringDeduper::SplitToWords(const string &word, vector<string> &result, bool removeSpace, bool mergeAscii)
{
    result.clear();
    if (word.empty())
    {
        return 0;
    }

    int wordSize = word.size();
    for (int i = 0; i < wordSize;)
    {
        if ((word[i] & 240) == 224)
        {
            if (i + 3 <= wordSize)
            {
                result.push_back(string(&word[i], 3));
            }
            i += 3;
        }
        else if ((word[i] & 224) == 192)
        {
            if (i + 2 <= wordSize)
            {
                result.push_back(string(&word[i], 2));
            }
            i += 2;
        }
        else if ((word[i] & 128) == 0)
        {
            if (removeSpace && isspace(word[i]))
            {
                ++i;
                continue;
            }

            int j = i + 1;
            if (mergeAscii && !isspace(word[i]))
            {
                while ((j < wordSize)
                       && ((word[j] & 128) == 0)
                       && !isspace(word[j]))
                {
                    ++j;
                }
            }

            result.push_back(string(&word[i], j - i));
            i = j;
        }
        else
        {
            ++i;
            result.clear();
            break;
        }
    }
    return result.size();
}

vector<string> StringDeduper::getNgrams(const vector<string>& tokens, size_t n)
{
    vector<string> ngrams;
    //generate ngrams
    size_t len = tokens.size();
    for (size_t i = 0; i < len-n+1; ++i) {
        string gram = "";
        for (size_t j = i; (j < n+i) && (j < len); j++) {
            gram += tokens[j];
        }
        ngrams.push_back(gram);
    }
    return ngrams;
}

set<string> StringDeduper::transVec2Set(const vector<string>& tokens)
{
    set<string> res;
    for (size_t i = 0; i < tokens.size(); ++i) {
        res.insert(tokens[i]);
    }
    return res;
}

float StringDeduper::jaccard(const set<string>& seta, const set<string>& setb)
{
    if (seta.size() < 6 || setb.size() < 6)
    {
        return 0.0;
    }

    set<string> aORb, aAndb;
    set_union(seta.begin(), seta.end(),
            setb.begin(), setb.end(),
            inserter(aORb, aORb.begin())
            );
    set_intersection(seta.begin(), seta.end(),
            setb.begin(), setb.end(),
            inserter(aAndb, aAndb.begin())
            );
    size_t min_size = std::min(seta.size(), setb.size());
    size_t max_size = std::max(seta.size(), setb.size());
    //return (aAndb.size()*1.0)/(aORb.size());
    float jac1 = (aAndb.size()*1.0)/(aORb.size()+0.1);
    float jac2 = (aAndb.size()*1.0)/(min_size+0.1);
    //float big_rate = (max_size - min_size)*1.0/(min_size+0.1);
    float jac = jac1;
    if (jac < 0.7) {
        jac = jac2;
    }
    return jac;
}

AGG_END_NAMESPACE(agg_web2);
