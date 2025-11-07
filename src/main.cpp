#include "core/version.hpp"
#include "../include/httplib.h"
#include <csignal>
#include <iostream>
#include "net/routes/append.hpp"

httplib::Server server;

void signalHandler(int signal)
{
    std::cout << "\nShutting down... " << signal << std::endl;
    server.stop();
}

int main()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "Hyperlog v" << hyperlog::version::MAJOR << "."
              << hyperlog::version::MINOR << "."
              << hyperlog::version::PATCH << std::endl;

    server.Post("/append", [](const httplib::Request &req, httplib::Response &res)
                { routes::HandleSingularAppend(req, res); });

    std::cout << "Server listening on http://0.0.0.0:6121" << std::endl;
    server.listen("0.0.0.0", 6121);

    return 0;
}