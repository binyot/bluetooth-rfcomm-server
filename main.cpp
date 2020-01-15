#include <iostream>
#include <string>
#include <algorithm>

#include "SocketServer.h"

int main() {
    SocketServer server;
    for (bool running = true; running; ) {
        server.listen([&running](std::ostream &os, std::istream &is){
            for (std::string line; std::getline(is, line); ) {
                std::cerr << "Received \"" << line << "\"" << std::endl;
                os << "You sent me \"" << line << "\"" << std::endl;
                os.flush();
                std::transform(begin(line), end(line), begin(line),
                               [](unsigned char c){return std::tolower(c);});
                if (line == "die") {
                    running = false;
                    break;
                }
            }
        });
    }
    return 0;
}
