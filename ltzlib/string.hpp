#ifndef LTZ_STRING_HPP
#define LTZ_STRING_HPP

#include <string>
#include <vector>
#include <algorithm>

namespace ltz {

namespace str {
/*
    @brief trim leading whitespace
    @param s string to trim
    @return trimmed string
 */
inline std::string& ltrim(std::string& s) {
    auto it = std::find_if(s.begin(), s.end(), [](char ch) { return !std::isspace(ch); });
    s.erase(s.begin(), it);
    return s;
}

/*
    @brief trim trailing whitespace
    @param s string to trim
    @return trimmed string
 */
inline std::string& rtrim(std::string& s) {
    auto it = std::find_if(s.rbegin(), s.rend(), [](char ch) { return !std::isspace(ch); });
    s.erase(it.base(), s.end());
    return s;
}

/*
    @brief trim both side whitespace
    @param s string to trim
    @return trimmed string
 */
inline std::string& trim(std::string& s) {
    return ltrim(rtrim(s));
}


}  // namespace str
}  // namespace ltz


#endif