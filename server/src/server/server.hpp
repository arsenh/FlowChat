#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>

#include "session/session.hpp"

namespace asio = boost::asio;


class Server
{
public:
    explicit Server(unsigned short port);
    void accept();
    void add_session(uint64_t id, std::shared_ptr<Session> s);
    void remove_session(uint64_t id);

    void broadcast(const std::string& msg, uint64_t sender_id);
    void run();

private:
    asio::io_context ioc_;
    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<uint64_t, std::shared_ptr<Session>> sessions_;
};

#endif // SERVER_HPP