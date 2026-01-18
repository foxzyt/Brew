#ifdef _WIN32
#include <windows.h>
#endif
#include "brew/brew.hpp"
#include "external/CLI11.hpp"
#include "external/rang.hpp"
#include <iostream>

using namespace brew;

int main(int argc, char** argv) {

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IONBF, 0);
    #endif

    Manager::init();
    CLI::App app{"Brew - Ultimate Snippet Manager"};
    app.require_subcommand(1);

    auto sync = app.add_subcommand("sync", "Sync snippets from repository");
    sync->callback([]() { Core::sync(); });

    std::string l_lang;
    bool l_favs = false;
    auto list = app.add_subcommand("list", "List all snippets");
    list->add_option("--lang", l_lang);
    list->add_flag("-f,--favorites", l_favs, "Show only favorite snippets");
    list->callback([&]() { Core::list(l_lang, l_favs); });

    std::string fav_name;
    auto favorite = app.add_subcommand("favorite", "Toggle snippet as favorite");
    favorite->add_option("name", fav_name)->required();
    favorite->callback([&]() { Manager::toggle_favorite(fav_name); });

    std::string i_name;
    auto info = app.add_subcommand("info", "Show snippet details");
    info->add_option("name", i_name)->required();
    info->callback([&]() { Core::info(i_name); });

    std::string s_term;
    auto search = app.add_subcommand("search", "Search snippets");
    search->add_option("term", s_term)->required();
    search->callback([&]() { Core::search(s_term); });

    std::string g_name, g_out;
    auto get = app.add_subcommand("get", "Copy snippet to clipboard");
    get->add_option("name", g_name)->required();
    get->add_option("-o", g_out);
    get->callback([&]() { Core::get(g_name, g_out); });

    std::string v_name;
    bool v_raw = false;
    auto view_cmd = app.add_subcommand("view", "Show snippet code");
    view_cmd->add_option("name", v_name)->required();
    view_cmd->add_flag("-r,--raw", v_raw, "Show raw content");
    view_cmd->callback([&]() { Core::view(v_name, v_raw); });

    std::string al_name, al_alias;
    auto alias = app.add_subcommand("alias", "Create a shortcut");
    alias->add_option("name", al_name)->required();
    alias->add_option("alias", al_alias)->required();
    alias->callback([&]() { Manager::set_alias(al_name, al_alias); });

    app.add_subcommand("stats", "Show usage statistics")->callback([]() { Core::stats(); });
    app.add_subcommand("purge", "Clear all local data")->callback([]() { Core::purge(); });
    app.add_subcommand("browse", "Open repository in browser")->callback([]() { Core::browse(); });

    std::string sh_path;
    auto share = app.add_subcommand("share", "Prepare a snippet for sharing");
    share->add_option("path", sh_path)->required();
    share->callback([&]() { Core::share(sh_path); });

    CLI11_PARSE(app, argc, argv);
    return 0;
}