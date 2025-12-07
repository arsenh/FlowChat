#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>

#include <functional>

namespace asio = boost::asio;

class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(asio::ip::tcp::socket socket, std::reference_wrapper<Server> server, uint64_t id);
    void start();
    void send(const std::string& msg);

private:
    void read();
    static std::string handle_client_read(const std::shared_ptr<asio::streambuf>& buffer, const boost::system::error_code &ec, std::size_t bytes);

private:
    asio::ip::tcp::socket socket_;
    std::reference_wrapper<Server> server_;
    uint64_t id_;
};

#endif // SESSION_HPP