#include <algorithm>
#include <filesystem>
#include <gflags/gflags.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <string_view>

namespace fs = std::filesystem;

DEFINE_string(directory, "", "Directory where experiments images are");

namespace {

struct TiffSearcher
{

    TiffSearcher()
        : to_print(new std::set<fs::path>)
    {
    }

    TiffSearcher(std::size_t dir_name_start)
        : to_print(new std::set<fs::path>)
        , dir_name_start(dir_name_start + 1) // we want it to be after the subsequent slash
    {
    }

    void operator()(const fs::directory_entry & dir)
    {
        if (!dir.is_directory()) {
            return;
        }
        fs::directory_iterator dir_iter(dir);
        std::string dir_path = dir.path().string();
        for (const auto & sub_element : dir_iter) {
            if (sub_element.is_regular_file()) {
                if (sub_element.path().extension() == ".cpp") {
                    to_print->emplace(dir);
                }
            }
            else if (sub_element.is_directory()) {
                fs::directory_iterator sub_dir(sub_element);
                std::for_each(fs::begin(sub_dir), fs::end(sub_dir), *this);
            }
        }
    }

    std::string format_path(const std::string & path)
    {
        std::string res(path.substr(dir_name_start));
        std::size_t filename_begin = res.find_last_of(fs::path::preferred_separator);
        if (filename_begin == std::string::npos) {
            return res;
        }

        std::transform(res.begin(), res.begin() + filename_begin, res.begin(), [this](const char & c){
            return (c == fs::path::preferred_separator ? '_' : c);
        });
        return res;
    }

    void clear()
    {
        to_print->clear();
    }

    void print_found()
    {
        std::transform(to_print->begin(),to_print->end(), std::ostream_iterator<std::string_view>(std::cout, "\n"), [this](const fs::path & path){
            return format_path(path.string());
        });
    }

private:
    std::shared_ptr<std::set<fs::path>> to_print;
    std::size_t dir_name_start = 0;
};

} // namespace

int main(int argc, char ** argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_directory.empty()) {
        std::cerr << "no '-directory' flag found\n";
        return 1;
    }

    fs::path dir_path(FLAGS_directory);
    if (!exists(dir_path)) {
        std::cerr << "path " << dir_path << " doesn't exist\n";
        return 2;
    }

    if (!is_directory(dir_path)) {
        std::cerr << dir_path << " is not a path to directory\n";
        return 3;
    }

    fs::directory_entry initial_dir(dir_path);
    fs::directory_iterator dir_iter(initial_dir);
    TiffSearcher ts(dir_path.string().length());
    std::for_each(fs::begin(dir_iter), fs::end(dir_iter), ts);
    ts.print_found();
}
