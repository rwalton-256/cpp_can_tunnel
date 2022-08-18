/**
 * @file tunnel.hpp
 * @author Ryan Walton
 * @brief 
 * @version 0.1
 * @date 2022-08-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <socketcan_wrapper/socketcan_wrapper.hpp>
#include <tcp_server_client_wrapper/server.hpp>
#include <tcp_server_client_wrapper/client.hpp>

namespace can_tunnel
{

class Tunnel
{
protected:
    enum State
    {
        State_Uninit,
        State_Init,
        State_Shutdown
    };
    std::mutex _mMutex;
    std::atomic<State> _mState;
    std::condition_variable _mStateCv;
    std::thread _mCanRxThread;
    std::thread _mTcpRxThread;
    SocketCAN_Wrapper* _mCanWrapperPtr;
    tcp_wrapper::Endpoint* _mTcpEpPtr;
    void _mCanRxFunc();
    void _mTcpRxFunc();
public:
    Tunnel();
};

class ServerTunnel : protected Tunnel
{
protected:
    SocketCAN_Wrapper _mCanWrapper;
    tcp_wrapper::Server _mTcpServer;
public:
    ServerTunnel( const std::string& _aCanIface, in_port_t _aPort );
    ~ServerTunnel();
};

class ClientTunnel : protected Tunnel
{
protected:
    SocketCAN_Wrapper _mCanWrapper;
    tcp_wrapper::Client _mTcpClient;
public:
    ClientTunnel( const std::string& _aCanIface, in_addr _aAddr, in_port_t _aPort );
    ~ClientTunnel();
};

}//can_tunnel
