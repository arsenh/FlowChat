#include "server.hpp"

#include <print>
#include <istream>
#include <algorithm>
#include <utility>


Server::Server(unsigned short port)
        : ioc{},
          acceptor_{ioc, asio::ip::tcp::endpoint{asio::ip::tcp::v4(), port}}
{

}

void Server::accept_connection()
{
    auto sock = std::make_shared<asio::ip::tcp::socket>(ioc);

    acceptor_.async_accept(*sock, [sock, this](const boost::system::error_code &ec)
    {
        if (ec)
        {
            std::println("Accept error: {}", ec.message());
            return;
        }

        std::println("Accepted new connection: {}",
                     sock->remote_endpoint().address().to_string());

        handle_accept(sock);
        accept_connection(); // accept next client
    });
}

void Server::handle_accept(std::shared_ptr<asio::ip::tcp::socket> sock)
{
    handle_client(std::move(sock));
}

std::string Server::handle_read(std::shared_ptr<asio::ip::tcp::socket> sock,
                         std::shared_ptr<asio::streambuf> buffer,
                         const boost::system::error_code &ec, std::size_t bytes)
{
    if (ec)
    {
        std::println("Failed to read client data: {}", ec.message());
        // handle this error as client DISCONNECTED
        return "";
    }

    std::istream is(buffer.get());
    std::string client_data;
    std::getline(is, client_data);
    buffer->consume(bytes);

    std::println("Received Data From Client: {}", client_data);

    std::ranges::transform(client_data, client_data.begin(), ::toupper);
    return client_data;
}

void Server::handle_client(std::shared_ptr<asio::ip::tcp::socket> sock)
{
    auto buffer = std::make_shared<asio::streambuf>();
    async_read_until(*sock, *buffer, '\n',
    [this, sock, buffer](const boost::system::error_code &ec, std::size_t bytes)
    {
        auto client_data = handle_read(sock, buffer, ec, bytes);

        async_write(*sock, asio::buffer(client_data + "\n"),
        [this, sock, buffer](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (error)
            {
                std::println("Write error: {}", error.message());
                return;
            }
            std::println("Server transferred {} bytes", bytes_transferred);
            handle_client(sock);
        });
    });
}

void Server::run()
{
    std::println("Server started ...");
    accept_connection();
    ioc.run();
}