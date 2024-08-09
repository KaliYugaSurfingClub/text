#ifndef TEXT_CORE_H
#define TEXT_CORE_H

#include "bits/stdc++.h"

using namespace std;

inline auto to_low = [](const string &str) {
    string res;
    res.reserve(str.size());

    transform(str.begin(), str.end(), back_inserter(res), [](char c) {
        return tolower(c);
    });

    return res;
};

struct Sentence {
    Sentence() = default;

    explicit Sentence(string_view str) : data_(str) {
        if (data_.empty()) {
            return;
        }

        regex exp{R"([^\W_]+(?:['_-][^\W_]+)*)"};
        regex_token_iterator it{str.begin(), str.end(), exp};

        transform(it, {}, inserter(words_, words_.begin()), to_low);
    }

    string data_;
    unordered_set<string> words_;
};

vector<Sentence> split_into_sentences(string_view source) {
    vector<Sentence> sentences;

    regex exp{"[^.?!]+[.?!]"};
    regex_token_iterator it{source.begin(), source.end(), exp};
    regex_token_iterator<string_view::iterator> end_it;

    while (it != end_it) {
        sentences.emplace_back(string{*it++});
    }

    return sentences;
}

string get_text_from_stream(istream &is) {
    string res;

    is >> noskipws;
    copy(istream_iterator<char>{is}, {}, back_inserter(res);
    is.clear();

    return res;
}


map<string, set<string>> get_statistic(string_view source, const unordered_set<string> &forbidden) {
    map<string, set<string>> mapped_statistic;
    vector<Sentence> sentences = split_into_sentences(source);

    for (const auto &[sentence, words]: sentences) {
        for (const auto &word: words) {
            if (!forbidden.contains(word)) {
                mapped_statistic[word].emplace(sentence);
            }
        }
    }

//    vector<pair<string, set<string>>> sorted_statistic(mapped_statistic.size());
//    ranges::copy(mapped_statistic, sorted_statistic.begin());
//    ranges::sort(sorted_statistic, [](const auto &p1, const auto &p2) {
//        return p1.second.size() > p2.second.size();
//    });

    return mapped_statistic;
}

vector<pair<string, set<string>>>
get_statistic(const vector<filesystem::path> &paths, const unordered_set<string> &forbidden) {
    map<string, set<string>> mapped_statistic;

    if (paths.empty()) {
        cout << "enter text\n";
        mapped_statistic = get_statistic(get_text_from_stream(cin), forbidden);
    }

    vector<future<map<string, set<string>>>> futures(paths.size());
    for (const auto &path: paths) {
        ifstream is{path};
        string text = get_text_from_stream(is);

        futures.push_back(async([&]() {
            return get_statistic(text, forbidden);
        }));
    }

    vector<map<string, set<string>>> maps(futures.size());
    for (const auto &future_map: futures) {
        maps.push_back(std::move(future_map));
    }
}


#endif //TEXT_CORE_H
