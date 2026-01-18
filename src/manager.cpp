#include "brew/brew.hpp"
#include "external/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace brew {
    void Manager::init() {
        fs::path p(get_dir());
        if (!fs::exists(p)) {
            fs::create_directories(p);
        }
    }
    void Manager::toggle_favorite(const std::string& name) {
    nlohmann::json config;
    std::ifstream f(get_config_path());
    if (f.is_open()) {
        try { f >> config; } catch(...) {}
    }

    if (!config.contains("favorites") || !config["favorites"].is_array()) {
        config["favorites"] = nlohmann::json::array();
    }

    auto& favs = config["favorites"];
    bool found = false;

    for (auto it = favs.begin(); it != favs.end(); ++it) {
        if (*it == name) {
            favs.erase(it);
            found = true;
            std::cout << "⭐ Snippet '" << name << "' removed from favorites." << std::endl;
            break;
        }
    }

    if (!found) {
        favs.push_back(name);
        std::cout << "⭐ Snippet '" << name << "' added to favorites!" << std::endl;
    }

    std::ofstream out(get_config_path());
    out << config.dump(4);
}

bool Manager::is_favorite(const std::string& name) {
    std::ifstream f(get_config_path());
    if (!f.is_open()) return false;
    nlohmann::json config;
    try {
        f >> config;
        if (config.contains("favorites") && config["favorites"].is_array()) {
            for (auto& fav : config["favorites"]) {
                if (fav == name) return true;
            }
        }
    } catch (...) {}
    return false;
}

    std::string Manager::get_dir() {
        char* appdata = getenv("APPDATA");
        if (appdata) {
            return std::string(appdata) + "\\brew\\";
        }
        return ".brew\\";
    }

    std::string Manager::get_index_path() {
        return get_dir() + "index.json";
    }

    std::string Manager::get_config_path() {
        return get_dir() + "config.json";
    }

    void Manager::register_usage(const std::string& name) {
        std::ifstream f(get_index_path());
        if (!f.is_open()) return;

        nlohmann::json j;
        try {
            f >> j;
            if (j.contains(name)) {
                int count = j[name].value("usage_count", 0);
                j[name]["usage_count"] = count + 1;

                std::ofstream out(get_index_path());
                out << j.dump(4);
            }
        } catch (...) {}
    }

std::string Manager::resolve_alias(const std::string& alias_or_name) {
    std::ifstream f(get_config_path());
    if (!f.is_open()) return alias_or_name;

    nlohmann::json config;
    try {
        f >> config;
        if (config.contains("aliases") && config["aliases"].contains(alias_or_name)) {
            return config["aliases"][alias_or_name].get<std::string>();
        }
    } catch (...) {}

    return alias_or_name;
}

    void Manager::set_alias(const std::string& name, const std::string& alias) {
        nlohmann::json config;
        std::ifstream f(get_config_path());
        if (f.is_open()) {
            try { f >> config; } catch(...) {}
        }

        config["aliases"][alias] = name;
        std::ofstream out(get_config_path());
        out << config.dump(4);
        std::cout << "✔ Alias '" << alias << "' set for '" << name << "'" << std::endl;
    }
}