#include "includes/Server.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>

Server* g_server = NULL;

void signalHandler(int signal) {
    if (signal == SIGINT && g_server) {
        std::cout << "\nReceived SIGINT, shutting down server gracefully..." << std::endl;
        // clean up
        exit(0);
    }
}

int main(int argc, char** argv) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGPIPE, SIG_IGN);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 6667 password123" << std::endl;
        return 1;
    }
    
    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number. Port must be between 1 and 65535." << std::endl;
        return 1;
    }
    
    std::string password = argv[2];
    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return 1;
    }

    try {
        std::cout << "Starting IRC server on port " << port << "..." << std::endl;
        std::cout << "Server password: " << password << std::endl;
        std::cout << "Press Ctrl+C to stop the server" << std::endl;
        
        Server server(port, password);
        g_server = &server;
        
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
         return 1;
    }
    return 0;
}