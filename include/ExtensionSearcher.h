#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

namespace fs = std::filesystem;

struct ExtensionSearcherRef;

struct ExtensionSearcher
{
    friend ExtensionSearcherRef;
    // gets length of initial directory and extension it's going to look for
    ExtensionSearcher(std::size_t dir_name_len, std::string ext);

    std::string format_path(const std::string & path);

    void print_found();

    std::size_t size();

private:
    struct CountedEntry
    {
        CountedEntry() = default;

        CountedEntry(const fs::directory_entry & dir, unsigned cnt = 0)
            : entry(dir)
            , cnt(cnt)
        {
        }

        fs::directory_entry entry;
        unsigned cnt;
    };

    std::map<std::string, CountedEntry> path_of;
    std::vector<std::string> names;
    std::string extension;
    std::size_t dir_name_start = 0;
};

struct ExtensionSearcherRef
{
    static ExtensionSearcherRef search_in_dir(const fs::path & path, std::string extension);


    ExtensionSearcherRef() = default;

    ExtensionSearcherRef(std::size_t dir_name_len, std::string ext);

    void recursive_search(const fs::directory_entry & dir, unsigned depth = 1);

    const ExtensionSearcher::CountedEntry & operator[](const std::string & formatted) const;

    ExtensionSearcher & operator*();

    const ExtensionSearcher & operator*() const;

    ExtensionSearcher * operator->();

    const ExtensionSearcher * operator->() const;

    const std::vector<std::string> & names();

    void clear();

    const std::string & get_extension();

private:
    std::shared_ptr<ExtensionSearcher> m_es_ptr;
};
