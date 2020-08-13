#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace fs = std::filesystem;

struct ExtensionSearcherRef;

struct ExtensionSearcher
{
    friend ExtensionSearcherRef;
    // gets length of initial directory and extension it's going to look for
    template <class... Strings>
    explicit ExtensionSearcher(std::size_t dir_name_len, Strings &&... strings)
        : path_of()
        , names()
        , extensions{std::forward<Strings>(strings)...}
        , dir_name_start(dir_name_len + 1) // we want it to be after the subsequent slash
    {
    }

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
    std::set<std::string> extensions;
    std::size_t dir_name_start = 0;
};

struct ExtensionSearcherRef
{
    template <class...Strings>
    static ExtensionSearcherRef search_in_dir(const fs::path & path, Strings && ... extensions)
    {
        ExtensionSearcherRef searcher(path.string().length(), std::forward<Strings>(extensions)...);

        fs::directory_iterator dir_iter(path);
        std::for_each(fs::begin(dir_iter), fs::end(dir_iter), [&searcher](const fs::directory_entry & dir) { searcher.recursive_search(dir); });
        return searcher;
    }

    ExtensionSearcherRef() = default;

    template <class... Strings>
    explicit ExtensionSearcherRef(std::size_t dir_name_len, Strings &&... exts)
        : m_es_ptr(new ExtensionSearcher(dir_name_len, std::forward<Strings>(exts)...))
    {
    }

    void recursive_search(const fs::directory_entry & dir, unsigned depth = 1);

    const ExtensionSearcher::CountedEntry & operator[](const std::string & formatted) const;

    const ExtensionSearcher::CountedEntry & operator[](std::size_t idx) const;

    ExtensionSearcher & operator*();

    const ExtensionSearcher & operator*() const;

    ExtensionSearcher * operator->();

    const ExtensionSearcher * operator->() const;

    const std::vector<std::string> & names() const;

    template <class String>
    bool has_extension(String && ext) const
    {
        return m_es_ptr->extensions.find(ext) != m_es_ptr->extensions.end();
    }

    void clear();

private:
    std::shared_ptr<ExtensionSearcher> m_es_ptr;
};
