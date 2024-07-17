#ifndef TEXT_APP_H
#define TEXT_APP_H

#include "bits/stdc++.h"
#include "core.h"

using namespace std;

class AppRunner {
public:
    AppRunner(int argc, char *argv[]) {
        strategies_ = {
            {"source", [this](string_view str) mutable -> void { source_path_ = filesystem::absolute(str); }},
            {"forbidden", [this](string_view str) mutable -> void { forbidden_path_ = filesystem::absolute(str); }},
            {"result", [this](string_view str) mutable -> void { result_path_ = filesystem::absolute(str); }},
            {"min_count", [this](string_view str) mutable -> void { min_count_ = stoi(string{str}); }},
        };

        for_each(argv + 1, argv + argc, [this](const char *ptr) {
            string_view arg{ptr};
            size_t end_of_name = arg.find_first_of('=');

            if (end_of_name == string_view::npos || arg.at(0) != '-') {
                throw invalid_argument{"incorrect args"};
            };

            string_view arg_name = arg.substr(1, end_of_name - 1);
            string_view arg_value = arg.substr(end_of_name + 1, arg.size());

            strategies_.at(arg_name)(arg_value);
        });
    }

    void run() {
        string source;
        unordered_set<string> forbidden;

        if (source_path_) {
            ifstream source_is{*source_path_};
            source_is >> noskipws;
            copy(istream_iterator<char>{source_is}, {}, back_inserter(source));
        } else {
            cout << "enter text\n";
            cin >> noskipws;
            copy(istream_iterator<char>{cin}, {}, back_inserter(source));
            cin.clear();
        }

        auto b = get_statistic(source, get_forbidden());

        ofstream os(*result_path_);

        for (const auto &[word, sentences]: b) {
            if (sentences.size() > 100) {
                os << word << ' ' << sentences.size() << '\n';
                for (const auto &sent: sentences) {
                    os << sent << '\n';
                }
                os << '\n';
            }
        }
    }

private:
    unordered_set<string> get_forbidden() {
        unordered_set<string> forbidden;
        auto it = inserter(forbidden, forbidden.begin());

        if (forbidden_path_) {
            ifstream forbidden_is{*forbidden_path_};
            copy(istream_iterator<string>{forbidden_is}, {}, it);
        } else {
            cout << "enter forbidden words\n";
            copy(istream_iterator<string>{cin}, {}, it);
        }

        transform(forbidden.begin(), forbidden.end(), inserter(forbidden, forbidden.begin()), [](const string &s) {
            return to_low(s);
        });

        return forbidden;
    }

    map<string_view, function<void(string_view)>> strategies_;

    optional<filesystem::path> source_path_;
    optional<filesystem::path> forbidden_path_;
    optional<filesystem::path> result_path_;
    size_t min_count_ = 1;
};

#endif //TEXT_APP_H
