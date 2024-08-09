#ifndef TEXT_APP_H
#define TEXT_APP_H

#include "bits/stdc++.h"
#include "core.h"

using namespace std;

class Options {
public:
    Options(int argc, char *argv[]) {
        const static unordered_map<string_view, function<void(string_view)>> strategies_{
            {"source_file", [this](string_view str) { set_source_file(str); }},
            {"source_dir", [this](string_view str) { set_source_dir(str); }},
            {"forbidden", [this](string_view str) { forbidden_path_ = filesystem::absolute(str); }},
            {"result", [this](string_view str)  { result_path_ = filesystem::absolute(str); }},
            {"min_count", [this](string_view str)  { min_count_ = stoi(string{str}); }},
        };

        for_each(argv + 1, argv + argc, [&](const char *ptr) {
            string_view arg{ptr};
            size_t end_of_name = arg.find_first_of('=');

            if (end_of_name == string_view::npos || arg.at(0) != '-') {
                throw invalid_argument{"incorrect args"};
            };

            string_view arg_name = arg.substr(1, end_of_name - 1);
            string_view arg_value = arg.substr(end_of_name + 1, arg.size());

            if (!strategies_.contains(arg_name)) {
                auto error = string{arg_name} + " is incorrect arg";
                throw invalid_argument{error};
            }

            strategies_.at(arg_name)(arg_value);
        });
    }

    tuple<vector<filesystem::path>, optional<filesystem::path>, optional<filesystem::path>, size_t>
    decompose() {
        return {source_paths_, forbidden_path_, result_path_, min_count_};
    }

    void set_source_file(const filesystem::path &path) {
        check_path(path);

        if (path.extension() != ".txt") {
            throw invalid_argument{path.string() + "must be a .txt"};
        }

        source_paths_.push_back(path);
    }

    void set_source_dir(const filesystem::path &path) {
        check_path(path);

        if (!is_directory(path)) {
            throw invalid_argument{path.string() + "must be a dir"};
        }

        copy(
            filesystem::recursive_directory_iterator{path},
            {},
            back_inserter(source_paths_)
        );
    }

private:
    static void check_path(const filesystem::path &path) {
        if (!filesystem::exists(path)) {
            throw invalid_argument{path.string() + "doesnt exist"};
        }
    }

    vector<filesystem::path> source_paths_;
    optional<filesystem::path> forbidden_path_;
    optional<filesystem::path> result_path_;
    size_t min_count_ = 1;
};

unordered_set<string> get_forbidden(const optional<filesystem::path> &path) {
    unordered_set<string> forbidden;
    auto it = inserter(forbidden, forbidden.begin());

    if (path) {
        ifstream forbidden_is{*path};
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


void run(int argc, char *argv[]) {
    const auto &[
        source_paths,
        forbidden_paths,
        result_path,
        min_count
    ] = Options{argc, argv}.decompose();

    string source;
    unordered_set<string> forbidden;

    if (!source_paths.empty()) {
        ifstream source_is{source_paths[0]};
        source_is >> noskipws;
        copy(istream_iterator<char>{source_is}, {}, back_inserter(source));
        source_is.clear();
    } else {
        cout << "enter text\n";
        cin >> noskipws;
        copy(istream_iterator<char>{cin}, {}, back_inserter(source));
        cin.clear();
    }

    auto b = get_statistic(source, get_forbidden(forbidden_paths));

    ofstream os(*result_path);

    for (const auto &[word, sentences]: b) {
        if (sentences.size() > 1) {
            os << word << ' ' << sentences.size() << '\n';
            for (const auto &sent: sentences) {
                os << sent << '\n';
            }
            os << '\n';
        }
    }
}




#endif //TEXT_APP_H
