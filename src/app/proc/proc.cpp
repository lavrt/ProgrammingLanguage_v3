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
    asio::readable_pipe err_pipe{ctx};

    bp::process proc{
        ctx,
        exe,
        args,
        bp::process_stdio{
            {},
            {},
            err_pipe
        }
    };

    std::string stderr_text;
    boost::system::error_code ec;

    asio::read(err_pipe, asio::dynamic_buffer(stderr_text), ec);

    proc.wait();

    return {
        proc.exit_code(),
        std::move(stderr_text)
    };
}

} // namespace app::proc
