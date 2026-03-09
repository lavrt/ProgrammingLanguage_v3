#pragma once

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

namespace app::proc {

struct ProcessResult {
    int exit_code = -1;
    std::string stderr_text;
};

ProcessResult RunProcess(const boost::filesystem::path& exe, const std::vector<std::string>& args);

} // app::namespace proc
