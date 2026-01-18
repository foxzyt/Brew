#ifndef BREW_CORE_HPP
#define BREW_CORE_HPP

#include <string>

namespace brew {
    class Fetcher {
    public:
        static std::string fetch_raw(const std::string& path);
    };
}

#endif