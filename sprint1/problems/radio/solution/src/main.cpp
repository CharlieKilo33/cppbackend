#include "audio.h"
#include <iostream>
#include <boost/asio.hpp>


using namespace std::literals;
namespace net = boost::asio;
using net::ip::udp;

static const size_t max_buffer_size = 65000;

void StartServer(const uint16_t port){  
    try {
        net::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));
        for (;;) {
            Player player(ma_format_u8, 1);
            std::array<char, max_buffer_size> recv_buf;
            udp::endpoint remote_endpoint;

            auto size = socket.receive_from(net::buffer(recv_buf), remote_endpoint);
            if (size < 0) {
                std::cerr << "Error receiving data" << std::endl;
                continue;
            }


            std::cout << "The record has been received "sv << std::endl;

            player.PlayBuffer(recv_buf.data(), size/player.GetFrameSize(), 1.5s);
            std::cout << "Playing done" << std::endl;
        }

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void StartClient(uint16_t port){
    try {
        net::io_context io_context;
        udp::socket socket(io_context, udp::v4());

        boost::system::error_code ec;
        while (true) {
            Recorder recorder(ma_format_u8, 1);
            std::string str;

            std::cout << "Enter the IP address of the server: " << std::endl;
            std::getline(std::cin, str);

            auto endpoint = udp::endpoint(net::ip::make_address(str, ec), port);

            std::cout << "Press Enter to record message..." << std::endl;
            std::getline(std::cin, str);

            auto rec_result = recorder.Record(max_buffer_size, 1.5s);
            size_t bytes_recorded = rec_result.data.size();
            size_t frame_size = recorder.GetFrameSize();
            size_t total_bytes_recorded = bytes_recorded * frame_size;
            std::string rec_buf(rec_result.data.begin(), rec_result.data.end());
            std::cout << "Recording done" << std::endl;

            socket.send_to(net::buffer(rec_buf), endpoint);
        }

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <client/server> <address>" << std::endl;
        return 1;
    }

    const int port = std::stoi(argv[2]);

    if (std::string(argv[1]) == "client") {
        StartClient(port);
    } else if (std::string(argv[1]) == "server") {
        StartServer(port);
    } else {
        std::cerr << "Invalid argument: " << argv[1] << std::endl;
        return 1;
    }

    return 0;
}