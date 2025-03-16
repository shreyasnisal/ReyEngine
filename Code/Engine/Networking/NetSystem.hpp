#pragma once

#include <string>
#include <vector>


constexpr uintptr_t SOCKET_INVALID = ~0ull;


enum class ConnectionState
{
	NOT_CONNECTED = -1,
	ATTEMPTING,
	CONNECTED
};

enum class NetworkMode
{
	NONE = -1,
	CLIENT,
	SERVER
};

struct NetSystemConfig
{
public:
	std::string m_modeStr;
	std::string m_hostAddressStr;
	int m_sendBufferSize = 2048;
	int m_recvBufferSize = 2048;
};

class NetSystem
{
public:
	~NetSystem();
	NetSystem();
	explicit NetSystem(NetSystemConfig const& config);

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void QueueMessageForSend(std::string message);
	std::string GetNextReceivedMessage();

	NetworkMode GetNetworkMode() const;

public:
	NetSystemConfig m_config;

	uintptr_t m_clientSocket = SOCKET_INVALID;
	uintptr_t m_listenSocket = SOCKET_INVALID;
	unsigned long m_hostAddress = 0;
	unsigned short m_hostPort = 0;

	char* m_sendBuffer = nullptr;
	char* m_recvBuffer = nullptr;

	std::vector<std::string> m_sendQueue;
	std::vector<std::string> m_recvQueue;
	std::string m_partialReceivedMessage;

	ConnectionState m_connectionState = ConnectionState::NOT_CONNECTED;


public:
	static NetworkMode GetNetworkModeFromString(std::string networkModeStr);

protected:
	void SendAndReceiveData();
	void InitializeClientSocket();
};
