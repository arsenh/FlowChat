#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <memory>

namespace asio = boost::asio;

class Server
{
public:
    explicit Server(unsigned short port);

    void accept_connection();

    void handle_accept(std::shared_ptr<asio::ip::tcp::socket> sock);


    std::string handle_read(std::shared_ptr<asio::ip::tcp::socket> sock,
                             std::shared_ptr<asio::streambuf> buffer,
                             const boost::system::error_code &ec,
                             std::size_t bytes);

    void handle_client(std::shared_ptr<asio::ip::tcp::socket> sock);

    void run();


private:
    asio::io_context ioc;
    asio::ip::tcp::acceptor acceptor_;
};

#endif // SERVER_HPP