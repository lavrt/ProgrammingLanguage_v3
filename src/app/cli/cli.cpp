#include "cli.hpp"

#include <filesystem>
#include <sstream>
#include <stdexcept>

#include <boost/program_options.hpp>

namespace app::cli {

namespace po = boost::program_options;

std::pair<CliResult, std::optional<ProgramConfig>> ParseCli(int argc, const char** argv) {
    po::options_description desc("Options");
    desc.add_options()
        (
            "help,h",
            "show this help and exit"
        )
        (
            "input,i",
            po::value<std::string>()->required(),
            "path to input file"
        )
        (
            "ast,a",
            po::value<std::string>()->default_value("./tmp/tree.txt"),
            "path to ast serialization file"
        )
        (
            "output,o",
            po::value<std::string>()->default_value("./bin/a.elf"),
            "path to output file"
        );

    std::ostringstream help_text;
    help_text << desc;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    
    if (vm.count("help")) {
        return {
            CliResult {
                .mode = CliMode::kExit,
                .exit_action {
                    .exit_code = 0,
                    .exit_text = help_text.str()
                }
            },
            std::nullopt
        };
    }

    po::notify(vm);

    return {
        CliResult{},
        ProgramConfig{
            .input = vm["input"].as<std::string>(),
            .output = vm["output"].as<std::string>(),
            .ast = vm["ast"].as<std::string>()
        }
    };
}

} // namespace app::cli
