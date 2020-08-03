#include <algorithm>
#include <filesystem>
#include <gflags/gflags.h>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

DEFINE_string(directory, "", "Directory where experiments images are");

namespace {

struct ExtensionSearcher
{

    explicit ExtensionSearcher(std::string ext = ".tif")
        : path_of(new std::map<std::string, fs::directory_entry>)
        , names(new std::vector<std::string>)
        , extension(std::move(ext))
    {
    }

    explicit ExtensionSearcher(std::size_t dir_name_start, std::string ext = ".tif")
        : path_of(new std::map<std::string, fs::directory_entry>)
        , names(new std::vector<std::string>)
        , extension(std::move(ext))
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
                if (sub_element.path().extension() == extension) {
                    if (const auto & [it, emplaced] = path_of->emplace(format_path(dir_path), dir); emplaced) {
                        names->emplace_back(it->first);
                    }
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

        std::transform(res.begin(), res.begin() + filename_begin, res.begin(), [this](const char & c) {
            return (c == fs::path::preferred_separator ? '_' : c);
        });
        return res;
    }

    void clear()
    {
        path_of->clear();
    }

    void print_found()
    {
        std::size_t idx = 0;
        for (const auto & name : *names) {
            std::cout << idx++ << ". " << name << '\n';
        }
    }

    fs::directory_entry & operator[](std::size_t i)
    {
        return path_of->find((*names)[i])->second;
    }

    std::size_t size() {
        return names->size();
    }

private:
    std::shared_ptr<std::map<std::string, fs::directory_entry>> path_of;
    std::shared_ptr<std::vector<std::string>> names;
    std::string extension;
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
    std::string extension = ".cpp";
    ExtensionSearcher ts(dir_path.string().length(), extension);
    std::for_each(fs::begin(dir_iter), fs::end(dir_iter), ts);
    ts.print_found();

    std::size_t chosen;
    while (std::cin >> chosen) {
        if (chosen < ts.size()) {
            dir_iter = fs::directory_iterator(ts[chosen]);
            for (const auto & sub_element : dir_iter) {
                if (sub_element.is_regular_file() && sub_element.path().extension() == extension) {
                    std::cout << sub_element.path().filename() << '\n';
                }
            }
        }
    }
}
