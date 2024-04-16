# Chat Application in C

This is a simple chat application implemented in C programming language using sockets and multithreading for communication between clients and a server.

## Features

~ Clients can connect to the server using TCP/IP sockets.

~ Clients can send messages to the server.

~The server broadcasts messages to all connected clients.

~ Multithreading is used to handle multiple clients simultaneously.

~ Each client can receive messages from the server in real-time.

## Prerequisites
To run this application, you need:

~ A C compiler (e.g., gcc)

~ POSIX-compliant operating system (Linux, macOS)

~ Basic understanding of network programming concepts

## Running with Manual Compilation

1.Open a terminal or command prompt.

2.Navigate to the directory where you have downloaded or cloned the Chat repository.

3.Compile the source code files using the following command:
### Run the server:

#### gcc main.c server.c -o server
#### ./server


