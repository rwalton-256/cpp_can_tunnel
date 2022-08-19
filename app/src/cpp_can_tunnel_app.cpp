/**
 * @file cpp_can_tunnel.cpp
 * @author Ryan Walton
 * @brief 
 * @version 0.1
 * @date 2022-08-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <can_tunnel/tunnel.hpp>

#include <iostream>
#include <condition_variable>
#include <csignal>
#include <cstring>
#include <netdb.h>

std::condition_variable cv;

int main( int argc, char** argv )
{
    if( ( argc < 3 ) || ( argc > 4 ) ||
        ( strcmp( argv[1], "--help" ) == 0 ) ||
        ( strcmp( argv[1], "-h" ) == 0 ) )
    {
        std::cout << "Usage: " << argv[0] << " can_iface port [ip_addr]" << std::endl;
        std::cout << "ex (server): " << argv[0] << " can0 5000" << std::endl;
        std::cout << "ex (client): " << argv[0] << " vcan0 5000 192.168.100.1" << std::endl;
        return 0;
    }

    signal( SIGINT, []( int signal ){ cv.notify_one(); } );

    std::mutex m;
    std::unique_lock<std::mutex> ul( m );

    if( argc == 3 )
    {
        // Server
        can_tunnel::ServerTunnel server( argv[1], atoi( argv[2] ) );

        cv.wait( ul );
    }
    else if( argc == 4 )
    {
        // Client
        struct hostent* server_name;
        in_addr server_addr;
        server_name = gethostbyname( argv[3] );

        memcpy( &server_addr, server_name->h_addr, sizeof( server_addr ) );

        can_tunnel::ClientTunnel client( argv[1], server_addr, atoi( argv[2] ) );

        cv.wait( ul );
    }

    return 0;
}

