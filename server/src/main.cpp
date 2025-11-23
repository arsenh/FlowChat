#include <iostream>
#include <print>
#include <string_view>
#include <thread>
#include <boost/asio.hpp>
#include <memory>

namespace asio = boost::asio;

class Server
{
public:
    Server(unsigned short port)
        : ioc{},
          acceptor_{ioc, asio::ip::tcp::endpoint{asio::ip::tcp::v4(), port}}
    {
    }

    void accept_connection()
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

    void handle_accept(std::shared_ptr<asio::ip::tcp::socket> sock)
    {
        auto buffer = std::make_shared<asio::streambuf>();
        read(sock, buffer);
    }

    void read(std::shared_ptr<asio::ip::tcp::socket> sock,
              std::shared_ptr<asio::streambuf> buffer)
    {
        async_read_until(*sock, *buffer, '\n',
                         [this, sock, buffer](const boost::system::error_code &ec, std::size_t bytes)
                         {
                             if (ec)
                             {
                                 std::println("Failed to read client data: {}", ec.message());
                                 return;
                             }

                             std::istream is(buffer.get());
                             std::string client_data;
                             std::getline(is, client_data);
                             buffer->consume(bytes);

                             std::println("Received Data From Client: {}", client_data);

                             std::transform(client_data.begin(), client_data.end(),
                                            client_data.begin(), ::toupper);

                             async_write(*sock, asio::buffer(client_data + "\n"),
                                         [this, sock, buffer](const boost::system::error_code &error,
                                                              std::size_t bytes_transferred)
                                         {
                                             if (error)
                                             {
                                                 std::println("Write error: {}", error.message());
                                                 return;
                                             }

                                             std::println("Server transferred {} bytes", bytes_transferred);
                                             read(sock, buffer);
                                         });
                         });
    }

    void run()
    {
        std::println("Server started ...");
        accept_connection();
        ioc.run();
    }

private:
    asio::io_context ioc;
    asio::ip::tcp::acceptor acceptor_;
};

int main()
{
    Server s{8081};
    s.run();
    return 0;
}
