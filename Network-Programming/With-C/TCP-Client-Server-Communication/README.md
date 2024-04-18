# TCP Client-Server Communication

This repository contains C code for implementing a simple TCP Client-Server communication system. The system consists of two programs: a server program (tcpServer.c) and a client program (tcpClient.c). The server program listens for incoming connections from clients and echoes back any messages received from clients to all connected clients. The client program connects to the server and allows users to send messages to the server.

## Features

~ Server listens for connections on a specified port.

~ Server echoes messages received from clients to all connected clients.

~ Client connects to the server and sends messages to the server.

~ Cross-platform compatibility: Supports both Windows and Unix-based systems.

## Repository Structure

~ tcpServer.c: Contains the code for the server program.

~ tcpClient.c: Contains the code for the client program.

~ libs.h: Header file containing macros, function prototypes, and includes for socket programming.

~ README.md: Markdown file containing information about the project, instructions for building and running the programs, and any other relevant details.

## Usage

To run this program on Unix, you need to type the following commands in the terminal:

#### gcc tcpServer.c -o server
#### ./server

#### gcc tcpClient.c -o client
#### ./client

And to work in Windows, you need to type the following commands in the terminal:

#### gcc -o server.exe server.c -lws2_32
#### server.exe

#### gcc -o client.exe tcpClient.c -lws2_32
#### client.exe

## Dependencies

~ For Windows: Requires Winsock2 library.

~ For Unix-based systems: No additional dependencies.

## Medium Article
Check out the accompanying Medium article for a detailed explanation of the TCP client-server communication project. The article covers the design principles, implementation details, and usage instructions for both the server and client programs. Additionally, it provides insights into the underlying concepts of socket programming and how they are applied in this project.

Link to Medium Article [link](https://github.com/HaykuhiMk/Picsart-Academy-Projects/tree/bd7f8ecce8a016e230abfcbbfc4eb52344537a85/Network-Programming/With-C/TCP-Client-Server-Communication)

Feel free to read the article for a deeper understanding of the project and leave any feedback or questions in the comments section. Your feedback is valuable and helps improve the quality of the project and the accompanying article.
