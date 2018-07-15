//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include "example/common/root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace Connection
{
namespace Boost
{

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class ISession
{
public:
    virtual void run(std::string host, std::string port, std::string target, int version) = 0;

    virtual ~ISession() = default;
};

namespace
{

// Report a failure
void fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

}

// Performs an HTTP GET and prints the response
class Session : public ISession
{
public:
    // Resolver and stream require an io_service
    explicit Session(boost::asio::io_service& ios, ssl::context& ctx)
        : resolver_(ios)
        , stream_(ios, ctx)
    {
    }

    // Start the asynchronous operation
    void run(std::string host, std::string port, std::string target, int version)
    {
        // Set up an HTTP GET request message
        req_.version(version);
        req_.method(http::verb::get);
        req_.target(target);
        req_.set(http::field::host, host);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Look up the domain name
        resolver_.async_resolve({host, port},
            std::bind(
                &Session::on_resolve,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void on_resolve(boost::system::error_code ec, tcp::resolver::iterator result)
    {
        if (ec)
            return fail(ec, "resolve");

        // Make the connection on the IP address we get from a lookup
        boost::asio::async_connect(
            stream_.next_layer(),
            result,
            std::bind(
                &Session::on_connect,
                this,
                std::placeholders::_1));
    }

    void on_connect(boost::system::error_code ec)
    {
        if (ec)
            return fail(ec, "connect");

        // Perform the SSL handshake
        stream_.async_handshake(
            ssl::stream_base::client,
            std::bind(
                &Session::on_handshake,
                this,
                std::placeholders::_1));
    }

    void on_handshake(boost::system::error_code ec)
    {
        if (ec)
            return fail(ec, "handshake");

        // Send the HTTP request to the remote host
        http::async_write(stream_, req_,
            std::bind(
                &Session::on_write,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");
        
        // Receive the HTTP response
        http::async_read(stream_, buffer_, res_,
            std::bind(
                &Session::on_read,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        // Write the message to standard out
        std::cout << res_ << std::endl;

        // Gracefully close the stream
        stream_.async_shutdown(
            std::bind(
                &Session::on_shutdown,
                this,
                std::placeholders::_1));
    }

    void on_shutdown(boost::system::error_code ec)
    {
        if (ec == boost::asio::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec.assign(0, ec.category());
        }
        if (ec)
            return fail(ec, "shutdown");

        // If we get here then the connection is closed gracefully
    }

private:
    tcp::resolver resolver_;
    ssl::stream<tcp::socket> stream_;
    boost::beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::empty_body> req_;
    http::response<http::string_body> res_;
};

} // namespace Boost
} // namespace Connection
