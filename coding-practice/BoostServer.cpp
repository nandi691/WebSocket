#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>
#include <string>

typedef websocketpp::server<websocketpp::config::asio> server;

server echo_server;

void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
    echo_server.send(hdl, msg->get_payload(), msg->get_opcode());
}

int main() {
    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        echo_server.init_asio();

        // Register handler for incoming messages
        echo_server.set_message_handler(&on_message);

        // Listen on port 9002
        echo_server.listen(9002);
        echo_server.start_accept();

        // Start the server
        std::cout << "WebSocket server started on port 9002..." << std::endl;
        echo_server.run();
    } catch (const websocketpp::exception& e) {
        std::cerr << "WebSocket server error: " << e.what() << std::endl;
    }
}
