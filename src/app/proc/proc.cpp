#include "proc.hpp"

#include <filesystem>
#include <vector>
#include <string>

#include <boost/process/v2.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace app::proc {

namespace bp = boost::process::v2;
namespace asio = boost::asio;
namespace bfs = boost::filesystem;

ProcessResult RunProcess(const bfs::path& exe, const std::vector<std::string>& args) {
    asio::io_context ctx;
    asio::readable_pipe stderr_pipe{ctx};

    bp::process proc{ctx, exe, args, bp::process_stdio{{}, {}, stderr_pipe}};

    std::string stderr_text;
    boost::system::error_code ec;
    asio::read(stderr_pipe, asio::dynamic_buffer(stderr_text), ec);

    proc.wait();

    return {
        .exit_code = proc.exit_code(),
        .stderr_text = std::move(stderr_text)
    };
}

} // namespace app::proc
