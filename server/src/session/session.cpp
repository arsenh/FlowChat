#include "session.hpp"

#include <print>
#include <boost/asio.hpp>

namespace asio = boost::asio;

Session::Session(asio::ip::tcp::socket socket, std::reference_wrapper<Server> server, uint64_t id)
    : socket_{std::move(socket)},
    server_{server},
    id_{id}

{

}

void Session::start()
{
    read();
}

void Session::send(const std::string& msg)
{
    
}

void Session::read()
{
    auto self = shared_from_this();
    auto buffer = std::make_shared<asio::streambuf>();
    async_read_until(socket_, *buffer, '\n',
    [self, buffer](const boost::system::error_code &ec, std::size_t bytes)
    {
        auto client_data = Session::handle_client_read(buffer, ec, bytes);

        async_write(self->socket_, asio::buffer(client_data + "\n"),
        [self, buffer](const boost::system::error_code &error, std::size_t bytes_transferred)
        {
            if (error)
            {
                std::println("Write error: {}", error.message());
                return;
            }
            std::println("Server transferred {} bytes", bytes_transferred);
            self->read();
        });
    });
}

std::string Session::handle_client_read(const std::shared_ptr<asio::streambuf>& buffer, const boost::system::error_code &ec, std::size_t bytes)
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
