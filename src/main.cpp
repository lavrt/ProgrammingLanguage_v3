#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include "cli.hpp"
#include "proc.hpp"

namespace fs = std::filesystem;

int main(int argc, const char** argv) {
    try {
        auto [program, cfg_opt] {app::cli::ParseCli(argc, argv)};
        if (program.mode == app::cli::CliMode::kExit) {
            std::cout << program.exit_action.exit_text;
            return program.exit_action.exit_code;
        }
        const auto& cfg = cfg_opt.value();

        fs::create_directory("tmp");
        fs::create_directory("bin");

        auto frontend = app::proc::RunProcess("./build/frontend/frontend", {cfg.input, cfg.ast});
        if (frontend.exit_code) {
            std::cerr << "Frontend failed:\n" << frontend.stderr_text;
            return frontend.exit_code;
        }

        auto backend = app::proc::RunProcess("./build/backend/backend", {cfg.ast, cfg.output});
        if (backend.exit_code) {
            std::cerr << "Backend failed:\n" << backend.stderr_text;
            return backend.exit_code;
        }

        fs::permissions(cfg.output,
            fs::status(cfg.output).permissions() |
            fs::perms::owner_exec |
            fs::perms::group_exec |
            fs::perms::others_exec,
            fs::perm_options::add
        );

        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
} 
