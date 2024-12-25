#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include <string>

typedef websocketpp::client<websocketpp::config::asio> client;

client echo_client;

void on_open(client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connection established!" << std::endl;

    std::string message = "Hello World from client!";
    c->send(hdl, message, websocketpp::frame::opcode::text);
}

void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
}

int main() {
    try {
        // Set logging settings
        echo_client.set_access_channels(websocketpp::log::alevel::all);
        echo_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        echo_client.init_asio();

        // Register handlers
        echo_client.set_open_handler(bind(&on_open, &echo_client, std::placeholders::_1));
        echo_client.set_message_handler(bind(&on_message, &echo_client, std::placeholders::_1, std::placeholders::_2));

        // Create a connection to the server
        websocketpp::uri uri("ws://localhost:9002");
        websocketpp::connection_hdl hdl = echo_client.get_connection(uri.str(), websocketpp::client<websocketpp::config::asio>::handler_ptr());
        echo_client.connect(hdl);

        // Run the client
        echo_client.run();
    } catch (const websocketpp::exception& e) {
        std::cerr << "WebSocket client error: " << e.what() << std::endl;
    }
}
