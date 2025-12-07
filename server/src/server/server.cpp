#include "server.hpp"

#include <print>
#include <algorithm>
#include <utility>


Server::Server(unsigned short port)
        : ioc_{},
          acceptor_{ioc_, asio::ip::tcp::endpoint{asio::ip::tcp::v4(), port}}
{

}

void Server::accept()
{
    static uint64_t id = 0;
    auto socket = std::make_shared<asio::ip::tcp::socket>(ioc_);

    acceptor_.async_accept(*socket, [s = socket, this](const boost::system::error_code &ec)
    {
        if (ec)
        {
            std::println("Accept error: {}", ec.message());
            return;
        }

        std::println("Accepted new connection: {}",
                     s->remote_endpoint().address().to_string());        
        auto session = std::make_shared<Session>(std::move(*s), std::ref(*this), id);
        
        add_session(id, session);

        session->start();

        ++id; // update for next client
        accept(); // accept next client
    });
}

void Server::add_session(uint64_t id, std::shared_ptr<Session> session)
{
    sessions_[id] = session;
}

void Server::remove_session(uint64_t id)
{
    sessions_.erase(id);
}

void Server::broadcast(const std::string& msg, uint64_t sender_id)
{
    for (auto& [id, session] : sessions_) {
        if (id == sender_id) continue;
        session->send(msg);
    }
}

void Server::run()
{
    std::println("Server started ...");
    accept();
    ioc_.run();
}