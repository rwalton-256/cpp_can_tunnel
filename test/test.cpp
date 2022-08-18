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
#include <cstring>
#include <netdb.h>
#include <assert.h>

int main()
{
    struct hostent* server_name;
    in_addr server_addr;
    server_name = gethostbyname( "localhost" );

    memcpy( &server_addr, server_name->h_addr, sizeof( server_addr ) );

    can_tunnel::ServerTunnel server( "vcan0", 5011 );
    can_tunnel::ClientTunnel client( "vcan1", server_addr, 5011 );

    SocketCAN_Wrapper vcan0( "vcan0" ), vcan1( "vcan1" );

    can_frame f0, f1, f2 =
    {
        .can_id = 0xff,
        .can_dlc = 8,
        .data = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 }
    };

    vcan0.send_frame( f2 );
    vcan1.receive_frame( f0 );
    assert( memcmp( f0.data, f2.data, 8 ) == 0 );
    vcan1.send_frame( f2 );
    vcan0.receive_frame( f1 );
    assert( memcmp( f1.data, f2.data, 8 ) == 0 );

    return 0;
}

