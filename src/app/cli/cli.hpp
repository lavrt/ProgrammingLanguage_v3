#pragma once

#include "cli.hpp"

#include <optional>
#include <string>

namespace app::cli {

enum class CliMode {
    kRun,
    kExit,
};

struct ExitAction {
    int exit_code;
    std::string exit_text;
};

struct CliResult {
    CliMode mode = CliMode::kRun;
    ExitAction exit_action;
};

struct ProgramConfig {
    std::string input;
    std::string output;
    std::string ast;
};

std::pair<CliResult, std::optional<ProgramConfig>> ParseCli(int argc, const char** argv);

} // namespace app::cli
