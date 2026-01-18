#include "brew/brew.hpp"
#include "external/httplib.h"
#include "external/json.hpp"
#include "external/rang.hpp"
#include "external/clip.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <map>

using json = nlohmann::json;
namespace fs = std::filesystem;


static std::string extract_markdown(const std::string& content) {
    std::string result = "";
    std::stringstream ss(content);
    std::string line;
    bool in_code = false;

    while (std::getline(ss, line)) {
        if (line.substr(0, 3) == "```") {
            in_code = !in_code;
            if (!in_code) result += "\n";
            continue;
        }
        if (in_code) {
            result += line + "\n";
        }
    }
    return result.empty() ? content : result;
}

namespace brew {


void Core::sync() {
    httplib::Client cli("https://raw.githubusercontent.com");
    cli.set_follow_location(true);

    #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        cli.enable_server_certificate_verification(false);
    #endif

    auto res = cli.Get("/foxzyt/brew-snippets/main/index.json");

    if (res && res->status == 200) {
        try {
            json j = json::parse(res->body);
            int count = j.size();

            std::ofstream f(Manager::get_index_path());
            f << res->body;

            std::cout << rang::fg::green << "✔ Index synced successfully!" << rang::style::reset << std::endl;
            std::cout << rang::fg::gray << "(" << count << " snippets updated)" << rang::style::reset << std::endl;
        } catch (...) {
            std::cerr << "✖ Error parsing synced index." << std::endl;
        }
    } else {
        std::cerr << rang::fg::red << "✖ Connection Failed!" << rang::style::reset << std::endl;
    }
}

void Core::list(const std::string& lang, bool favorites_only) {
    std::ifstream f(Manager::get_index_path());
    if (!f.is_open()) {
        std::cout << rang::fg::yellow << "⚠ Index not found. Run 'brew sync' first." << rang::style::reset << std::endl;
        return;
    }

    try {
        json j = json::parse(f);
        std::map<std::string, std::vector<std::string>> grouped;

        for (auto& [key, val] : j.items()) {
            std::string s_lang = val.value("language", "other");
            bool favorite = Manager::is_favorite(key);

            if (favorites_only && !favorite) continue;

            if (lang.empty() || s_lang == lang) {
                grouped[s_lang].push_back(key);
            }
        }

        std::cout << rang::style::bold << "\n--- Brew Snippets Inventory "
                  << (favorites_only ? "(Favorites)" : "") << " ---" << rang::style::reset << std::endl;

        for (auto const& [language, snippets] : grouped) {
            std::cout << "\n" << rang::fg::magenta << "📦 " << language << ":" << rang::style::reset << std::endl;
            for (const auto& name : snippets) {
                bool fav = Manager::is_favorite(name);
                std::cout << "  • " << (fav ? "⭐ " : "") << rang::fg::cyan << name << rang::style::reset << std::endl;
            }
        }
        std::cout << std::endl;
    } catch (...) {
        std::cerr << "✖ Error reading index." << std::endl;
    }
}



    void Core::search(const std::string& term) {
        std::ifstream f(Manager::get_index_path());
        if (!f.is_open()) return;

        json j = json::parse(f);
        std::string query = term;
        std::transform(query.begin(), query.end(), query.begin(), ::tolower);

        std::cout << "Searching for '" << query << "':" << std::endl;
        bool found = false;

        for (auto& [key, val] : j.items()) {
            std::string name = key;
            std::string desc = val.value("description", "");
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);

            if (name.find(query) != std::string::npos || desc.find(query) != std::string::npos) {
                std::cout << rang::fg::green << "-> " << rang::style::bold << key << rang::style::reset
                          << " (" << val.value("language", "txt") << ") - " << val.value("description", "") << std::endl;
                found = true;
            }
        }
        if (!found) std::cout << "No snippets match your search." << std::endl;
    }

void Core::get(const std::string& name, const std::string& out) {
    std::string real_name = Manager::resolve_alias(name);
    std::ifstream f(Manager::get_index_path());
    if (!f.is_open()) return;
    json j = json::parse(f);

    if (!j.contains(real_name)) {
        std::cout << rang::fg::red << "✖ Snippet '" << real_name << "' not found." << rang::style::reset << std::endl;
        return;
    }

    std::string snippet_path = j[real_name]["path"].get<std::string>();

    httplib::Client cli("https://raw.githubusercontent.com");
    cli.set_follow_location(true);

    #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        cli.enable_server_certificate_verification(false);
    #endif

    std::cout << "Downloading " << real_name << "..." << std::endl;

    std::string full_path = "/foxzyt/brew-snippets/main/" + snippet_path;
    auto res = cli.Get(full_path.c_str());

    if (res && res->status == 200) {
        std::string final_content = res->body;

        if (snippet_path.size() >= 3 && snippet_path.substr(snippet_path.size() - 3) == ".md") {
            final_content = extract_markdown(res->body);
        }

        if (!out.empty()) {
            std::ofstream of(out);
            of << final_content;
            std::cout << rang::fg::green << "✔ Saved to " << out << rang::style::reset << std::endl;
        } else {
            if (clip::set_text(final_content)) {
                std::cout << rang::fg::green << "✔ Code copied to clipboard!" << rang::style::reset << std::endl;
            }
        }
        Manager::register_usage(real_name);
    } else {
        std::cerr << rang::fg::red << "✖ Download failed! " << (res ? "HTTP " + std::to_string(res->status) : "Check your Internet/SSL/OpenSSL DLLs.") << rang::style::reset << std::endl;
    }
}

void Core::view(const std::string& name, bool raw) {
    std::string real_name = Manager::resolve_alias(name);
    std::ifstream f(Manager::get_index_path());
    if (!f.is_open()) return;
    json j = json::parse(f);

    if (!j.contains(real_name)) {
        std::cout << rang::fg::red << "✖ Snippet '" << real_name << "' not found." << rang::style::reset << std::endl;
        return;
    }

    std::string snippet_path = j[real_name]["path"].get<std::string>();

    httplib::Client cli("https://raw.githubusercontent.com");
    cli.set_follow_location(true);

    #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        cli.enable_server_certificate_verification(false);
    #endif

    std::cout << rang::fg::gray << "Fetching " << real_name << "..." << rang::style::reset << std::endl;
    std::string full_url = "/foxzyt/brew-snippets/main/" + snippet_path;

    auto res = cli.Get(full_url.c_str());

    if (res && res->status == 200) {
        bool is_md = (snippet_path.size() >= 3 && snippet_path.substr(snippet_path.size() - 3) == ".md");

        if (is_md && !raw) {
            std::stringstream ss(res->body);
            std::string line;
            bool in_code = false;
            std::cout << "\n" << rang::style::bold << rang::fg::magenta << "--- PREVIEW: " << real_name << " ---" << rang::style::reset << std::endl;

            while (std::getline(ss, line)) {
                if (line.substr(0, 3) == "```") {
                    in_code = !in_code;
                    std::cout << rang::fg::gray << "   ---------------------------------------" << rang::style::reset << std::endl;
                    continue;
                }
                if (in_code) {
                    std::cout << "   " << rang::fg::yellow << line << rang::style::reset << std::endl;
                } else if (line.substr(0, 2) == "# ") {
                    std::cout << "\n" << rang::style::bold << rang::fg::cyan << line << rang::style::reset << std::endl;
                } else if (line.substr(0, 2) == "> ") {
                    std::cout << rang::fg::green << line << rang::style::reset << std::endl;
                } else {
                    std::cout << line << std::endl;
                }
            }
            std::cout << rang::fg::magenta << "------------------------------------------\n" << rang::style::reset << std::endl;
        } else {
            std::cout << res->body << std::endl;
        }
    } else {
        std::cerr << rang::fg::red << "✖ Connection Error! Status: " << (res ? std::to_string(res->status) : "No Response") << rang::style::reset << std::endl;
    }
}

    void Core::info(const std::string& name) {
        std::string real_name = Manager::resolve_alias(name);
        std::ifstream f(Manager::get_index_path());
        if (!f.is_open()) return;
        json j = json::parse(f);

        if (j.contains(real_name)) {
            auto data = j[real_name];
            std::cout << rang::style::bold << "\nSnippet Info: " << real_name << rang::style::reset << std::endl;
            std::cout << "Language: " << data.value("language", "N/A") << std::endl;
            std::cout << "Path: " << data.value("path", "N/A") << std::endl;
            std::cout << "Description: " << data.value("description", "N/A") << std::endl;
            std::cout << "Times used: " << data.value("usage_count", 0) << std::endl;
        }
    }

    void Core::stats() {
        std::ifstream f(Manager::get_index_path());
        if (!f.is_open()) return;
        json j = json::parse(f);

        std::cout << rang::style::bold << "\n--- Brew Usage Statistics ---" << rang::style::reset << std::endl;
        bool any = false;
        for (auto& [key, val] : j.items()) {
            int count = val.value("usage_count", 0);
            if (count > 0) {
                std::cout << rang::fg::cyan << key << rang::style::reset << ": " << count << " uses" << std::endl;
                any = true;
            }
        }
        if (!any) std::cout << "No statistics recorded yet." << std::endl;
    }

    void Core::purge() {
        fs::remove_all(Manager::get_dir());
        Manager::init();
        std::cout << rang::fg::red << "✔ All local cache and configurations have been purged!" << rang::style::reset << std::endl;
    }

    void Core::browse() {
        std::string url = "https://github.com/foxzyt/brew-snippets";
        #ifdef _WIN32
            std::string command = "start " + url;
        #elif __APPLE__
            std::string command = "open " + url;
        #else
            std::string command = "xdg-open " + url;
        #endif
        std::system(command.c_str());
    }

    void Core::share(const std::string& file_path) {
        if (!fs::exists(file_path)) {
            std::cout << "File not found: " << file_path << std::endl;
            return;
        }

        fs::path p(file_path);
        json entry;
        entry["path"] = p.filename().string();
        entry["description"] = "New snippet shared from Brew";
        entry["language"] = p.extension().string().substr(1);

        std::cout << rang::fg::yellow << "\nAdd this to your index.json on GitHub:\n" << rang::style::reset << std::endl;
        std::cout << "\"" << p.stem().string() << "\": " << entry.dump(4) << "," << std::endl;
    }
}