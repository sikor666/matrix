#pragma once

#include "example/common/root_certificates.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/http.hpp>

#include <chrono>

namespace Time
{

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

template
<
    typename Session,
    typename IOContext,
    typename SSLContext,
    typename Clock = std::chrono::high_resolution_clock,
    typename Duration = std::chrono::nanoseconds
>
class Travel
{
public:
    using TimePoint = typename Clock::time_point;

    Travel() : sslContext{ ssl::context::sslv23_client },
               session{ ioContext, sslContext },
               start{ Clock::now() }
    {
        // This holds the root certificate used for verification
        load_root_certificates(sslContext);
    }

    auto distance()
    {
        return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
    }

    void launch()
    {
        // Launch the asynchronous operation
        session.run("www.boost.org", "443", "/LICENSE_1_0.txt", 11);
    }

    void run()
    {
        // Run the I/O service. The call will return when
        // the get operation is complete.
        ioContext.run();
    }

    Travel(Travel const&) = delete;
    Travel& operator=(Travel const&) = delete;
    Travel(Travel&&) = delete;
    Travel& operator=(Travel&&) = delete;

private:
    // The IO context is required for all I/O
    IOContext ioContext;

    // The SSL context is required, and holds certificates
    SSLContext sslContext;

    Session session;
    TimePoint start;
};

} // namespace Time
