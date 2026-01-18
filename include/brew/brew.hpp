#ifndef BREW_HPP
#define BREW_HPP

#include <string>
#include <vector>
#include <filesystem>
#include "../external/json.hpp"

namespace brew {
    class Manager {
    public:
        static void init();
        static std::string get_dir();
        static std::string get_index_path();
        static std::string get_config_path();
        static void register_usage(const std::string& name);
        static void set_alias(const std::string& name, const std::string& alias);
        static std::string resolve_alias(const std::string& alias_or_name);
        static void toggle_favorite(const std::string& name);
        static bool is_favorite(const std::string& name);
    };

    class Core {
    public:
        static void sync();
        static void get(const std::string& name, const std::string& out);
        static void list(const std::string& lang, bool favorites_only);
        static void search(const std::string& term);
        static void view(const std::string& name, bool raw = false);
        static void info(const std::string& name);
        static void stats();
        static void purge();
        static void browse();
        static void share(const std::string& file_path);
    };
}

#endif