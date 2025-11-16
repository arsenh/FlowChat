#include <iostream>
#include <print>
#include <string_view>
#include <thread>
#include <boost/asio.hpp>

using namespace boost;
using namespace boost::asio;

using Tcp = boost::asio::ip::tcp;

unsigned short port = 8081;
asio::io_context ioc{};
Tcp::acceptor acceptor{ioc, Tcp::endpoint(Tcp::v4(), port)};

void handle_client(std::shared_ptr<Tcp::socket> socket,
                   std::shared_ptr<std::vector<char>> buffer,
                   const boost::system::error_code &ec,
                   std::size_t bytes_read)
{
    if (ec)
    {
        std::println("Read error: {}", ec.message());
        return;
    }

    std::string_view msg(buffer->data(), bytes_read);
    std::println("Received {} bytes: {}", bytes_read, msg);
}

void accept_handler(const boost::system::error_code& ec,
                    std::shared_ptr<Tcp::socket> socket)
{
    if (!ec)
    {
        std::println("Accepted {}", socket->remote_endpoint().address().to_string());

        auto buffer = std::make_shared<std::vector<char>>(1024);

        socket->async_read_some(asio::buffer(*buffer),
                                [socket, buffer](auto&&... args)
                                {
                                    handle_client(socket, buffer,
                                                  std::forward<decltype(args)>(args)...);
                                });
    }
    else
    {
        std::println("Accept error: {}", ec.message());
    }

    // Start next accept — **always at end**
    auto new_socket = std::make_shared<Tcp::socket>(ioc);
    acceptor.async_accept(*new_socket,
                          [new_socket](auto&&... args)
                          {
                              accept_handler(std::forward<decltype(args)>(args)..., new_socket);
                          });
}

int main()
{
    std::println("Listening on port {}", port);

    auto socket = std::make_shared<Tcp::socket>(ioc);
    acceptor.async_accept(*socket,
                          [socket](auto&&... args)
                          {
                              accept_handler(std::forward<decltype(args)>(args)..., socket);
                          });

    ioc.run();
}