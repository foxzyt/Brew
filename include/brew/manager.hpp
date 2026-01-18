#ifndef BREW_MANAGER_HPP
#define BREW_MANAGER_HPP

#include <string>
#include <vector>

namespace brew {
    class Manager {
    public:
        static void setup_env();
        static bool save_index(const std::string& content);
        static std::string get_snippet_path(const std::string& name);
        static std::string get_brew_dir();
    };
}

#endif