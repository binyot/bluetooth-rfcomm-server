# bluetooth-rfcomm-server
bluez rfcomm server

## Usage

When SocketServer socket is created, a SDP servce record is automatically registered.

```c++
SocketServer server;
```

To listen and handle connections, use the ```listen``` method.
You can pass any callable with a ```void(std::ostream &, std::istream &)``` type

```c++
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
```

When passed callable returns, the connection is closed and ```listen``` can be called again.
