#include "ExtensionSearcher.h"

#include <utility>

std::string ExtensionSearcher::format_path(const std::string & path)
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

void ExtensionSearcher::print_found()
{
    std::size_t idx = 0;
    for (const auto & name : names) {
        std::cout << idx++ << ". " << name << '\n';
    }
}

std::size_t ExtensionSearcher::size()
{
    return names.size();
}

// ExtensionSearcherRef

void ExtensionSearcherRef::recursive_search(const fs::directory_entry & dir, unsigned int depth)
{
    if (dir.is_symlink()) {
        return;
    }
    if (!dir.exists() || !dir.is_directory()) {
        return;
    }
    if (((dir.status().permissions() & fs::perms::others_read) == fs::perms::none)) {
        return;
    }

    fs::directory_iterator dir_iter(dir);
    std::string formatted_path = m_es_ptr->format_path(dir.path().string());
    for (const auto & sub_element : dir_iter) {
        if (sub_element.is_symlink()) {
            continue;
        }
        if (sub_element.is_regular_file()) {
            if (has_extension(sub_element.path().extension())) {
                if (const auto & [it, emplaced] = m_es_ptr->path_of.try_emplace(formatted_path, dir, 1); emplaced) {
                    m_es_ptr->names.emplace_back(it->first);
                } else {
                    ++it->second.cnt;
                }
            }
        }
        else if (sub_element.is_directory() && sub_element.exists() &&
                 ((sub_element.status().permissions() & fs::perms::others_read) != fs::perms::none)) {
            recursive_search(sub_element, depth + 1);
        }
    }
}

ExtensionSearcher & ExtensionSearcherRef::operator*()
{
    return *m_es_ptr;
}

const ExtensionSearcher & ExtensionSearcherRef::operator*() const
{
    return *m_es_ptr;
}

ExtensionSearcher * ExtensionSearcherRef::operator->()
{
    return m_es_ptr.get();
}

const ExtensionSearcher * ExtensionSearcherRef::operator->() const
{
    return m_es_ptr.get();
}

void ExtensionSearcherRef::clear()
{
    m_es_ptr->path_of.clear();
    m_es_ptr->names.clear();
}

const ExtensionSearcher::CountedEntry & ExtensionSearcherRef::operator[](const std::string & formatted) const
{
    auto iter = m_es_ptr->path_of.find(formatted);
    if (iter == m_es_ptr->path_of.end()) {
        return ExtensionSearcher::CountedEntry{fs::directory_entry(), 0};
    }
    return iter->second;
}


const ExtensionSearcher::CountedEntry & ExtensionSearcherRef::operator[](std::size_t idx) const
{
    if (idx < m_es_ptr->size()) {
        return (*this)[names()[idx]];
    }
    return ExtensionSearcher::CountedEntry{fs::directory_entry(), 0};
}

const std::vector<std::string> & ExtensionSearcherRef::names() const
{
    return m_es_ptr->names;
}
