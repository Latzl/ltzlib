#ifndef LTZ_STRING_HPP
#define LTZ_STRING_HPP

#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace ltz {

inline std::string toStr(const char *psz) {
    if (!psz) {
        return "";
    }
    return psz;
}

template <typename T>
inline std::string toStr(const T &t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

/*
    @param trans transform to string function, it should like: std::string trans(It it)
 */
template <typename It, typename UnaryTran>
inline std::string toStr(It first, It last, UnaryTran trans) {
    std::stringstream ss;
    ss << "{ ";
    for (; first != last; first = std::next(first)) {
        ss << trans(first) << ", ";
    }
    std::string s = ss.str();
    if (s.size() >= 2) {
        s[s.size() - 2] = ' ';
        s[s.size() - 1] = '}';
    }
    return s;
}

template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type * = nullptr>
inline std::string toStr(It first, It last) {
    return toStr(first, last, [](It it) { return *it; });
}

namespace str {

/*
    @brief trim leading whitespace
    @param s string to trim
    @return trimmed string
 */
inline std::string &ltrim(std::string &s) {
    auto it = std::find_if(s.begin(), s.end(), [](char ch) { return !std::isspace(ch); });
    s.erase(s.begin(), it);
    return s;
}

/*
    @brief trim trailing whitespace
    @param s string to trim
    @return trimmed string
 */
inline std::string &rtrim(std::string &s) {
    auto it = std::find_if(s.rbegin(), s.rend(), [](char ch) { return !std::isspace(ch); });
    s.erase(it.base(), s.end());
    return s;
}

/*
    @brief trim both side whitespace
    @param s string to trim
    @return trimmed string
 */
inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

inline std::string replace(const std::string &src, const std::string &oldSubstr, const std::string &newSubstr) {
    if (oldSubstr.empty()) {
        return src;
    }
    std::stringstream ss;
    size_t start = 0;
    for (size_t end; (end = src.find(oldSubstr, start)) != std::string::npos; start = end + oldSubstr.size()) {
        ss << src.substr(start, end - start) << newSubstr;
    }
    ss << src.substr(start);
    return ss.str();
}

inline std::string &replace(std::string &src, const std::string &oldSubstr, const std::string &newSubstr) {
    return src = replace(const_cast<const std::string &>(src), oldSubstr, newSubstr);
}

inline std::vector<std::string> split_if(const std::string &src, const std::string &delimiter, std::function<bool(const std::string &)> pred) {
    if (src.empty()) {
        return {};
    }
    if (delimiter.empty()) {
        return {src};
    }
    std::vector<std::string> vRet{};
    size_t last = 0;
    size_t next = 0;
    while ((next = src.find(delimiter, last)) != std::string::npos) {
        vRet.emplace_back(src.substr(last, next - last));
        last = next + delimiter.size();
    }
    if (last < src.size()) {
        std::string to_push = src.substr(last);
        if (pred(to_push)) {
            vRet.push_back(to_push);
        }
    } else if (last == src.size()) {
        std::string to_push = "";
        if (pred(to_push)) {
            vRet.push_back(to_push);
        }
    }
    return vRet;
}

inline std::vector<std::string> split(const std::string &src, const std::string &delimiter) {
    return split_if(src, delimiter, [](const std::string &) { return true; });
}

template <typename InputIt>
inline std::string join(InputIt first, InputIt last, const std::string &delimiter) {
    return join(first, last, delimiter, [](const typename std::iterator_traits<InputIt>::value_type &t) { return t; });
}

/*
    @param trans transform to string function, it should like: std::string trans(It it)
 */
template <typename It, typename UnaryTran>
inline std::string join(It first, It last, const std::string &delimiter, UnaryTran trans) {
    std::string s;
    for (It it = first; it != last; it = std::next(it)) {
        s += trans(*it) + delimiter;
    }
    if (first != last && !s.empty()) {
        s.erase(s.size() - delimiter.size());
    }
    return s;
}
}  // namespace str
}  // namespace ltz


#endif