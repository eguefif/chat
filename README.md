# chat
Simple chat server and client. Client can talk to another client.

## Description

Two programms: a server and a client.
For now, the server will handle thre different commands:
-> chat (send a message to another client)
-> name (update the name of a client)
-> list (send back to a client the list of all connected clients)

On the CLI, the user is able to specify an IP address and a port. By default, locahost and port 10000 are used.
Upon opening the program, the user will be prompt for a name.
Then the user can ask the list of connected clients with the command /list.
Then he can talk to someone using /NAME content.

## Getting Started

### Dependencies

* g++
* Linux
* make

### Installing

* In the server directory and type make
* In the client directory, type make.

### Executing program

* In the server directory./server
* open another terminal and type ./client IP_ADDRESS PORT in the client directory.

## Authors

Contributors names and contact info

Emmaunel Guefif

## License

This project is licensed under the MIT License - see the LICENSE.md file for details
