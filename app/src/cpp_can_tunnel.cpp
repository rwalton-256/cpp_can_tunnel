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

namespace can_tunnel
{

Tunnel::Tunnel()
    :
    _mState( State_Uninit ),
    _mCanRxThread( [&](){ _mCanRxFunc(); } ),
    _mTcpRxThread( [&](){ _mTcpRxFunc(); } )
{

}

void Tunnel::_mCanRxFunc()
{
    {
        std::unique_lock<std::mutex> ul( _mMutex );
        _mStateCv.wait( ul, [&](){ return _mState.load() != State_Uninit; } );
    }

    while( _mState.load() != State_Shutdown )
    {
        if( _mTcpEpPtr->wait_for_connection( std::chrono::milliseconds( 100 ) ) )
        {
            can_frame f;
            try
            {
                _mCanWrapperPtr->receive_frame( f );
            }
            catch( const SocketCAN_Wrapper::Timeout& )
            {
                continue;
            }
            try
            {
                _mTcpEpPtr->write( &f, sizeof( f ) );
            }
            catch( const tcp_wrapper::Endpoint::Connection_Reset& )
            {
                continue;
            }
        }
    }
}

void Tunnel::_mTcpRxFunc()
{
    {
        std::unique_lock<std::mutex> ul( _mMutex );
        _mStateCv.wait( ul, [&](){ return _mState.load() != State_Uninit; } );
    }

    while( _mState.load() != State_Shutdown )
    {
        if( _mTcpEpPtr->wait_for_connection( std::chrono::milliseconds( 100 ) ) )
        {
            can_frame f;
            try
            {
                _mTcpEpPtr->read( &f, sizeof( f ) );
            }
            catch( const tcp_wrapper::Endpoint::Timeout& )
            {
                continue;
            }
            catch( const tcp_wrapper::Endpoint::Connection_Reset& )
            {
                continue;
            }
            _mCanWrapperPtr->send_frame( f );
        }
    }
}

ServerTunnel::ServerTunnel( const std::string& _aCanIface, in_port_t _aPort )
    :
    _mCanWrapper( _aCanIface, std::chrono::milliseconds( 100 ) ),
    _mTcpServer( _aPort, std::chrono::milliseconds( 100 ) )
{
    _mCanWrapperPtr = &_mCanWrapper;
    _mTcpEpPtr = &_mTcpServer;

    _mState.store( State_Init );
    _mStateCv.notify_all();
}

ServerTunnel::~ServerTunnel()
{
    _mState.store( State_Shutdown );
    _mStateCv.notify_all();

    _mCanRxThread.join();
    _mTcpRxThread.join();
}

ClientTunnel::ClientTunnel( const std::string& _aCanIface, in_addr _aAddr, in_port_t _aPort )
    :
    _mCanWrapper( _aCanIface, std::chrono::milliseconds( 100 ) ),
    _mTcpClient( _aAddr, _aPort, std::chrono::milliseconds( 100 ) )
{
    _mCanWrapperPtr = &_mCanWrapper;
    _mTcpEpPtr = &_mTcpClient;

    _mState.store( State_Init );
    _mStateCv.notify_all();
}

ClientTunnel::~ClientTunnel()
{
    _mState.store( State_Shutdown );
    _mStateCv.notify_all();

    _mCanRxThread.join();
    _mTcpRxThread.join();
}

};//can_tunnel
