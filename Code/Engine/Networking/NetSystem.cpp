#include "Engine/Networking/NetSystem.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


NetSystem::~NetSystem()
{
}

NetSystem::NetSystem()
{
}

NetSystem::NetSystem(NetSystemConfig const& config)
	: m_config(config)
{
}

void NetSystem::Startup()
{
	NetworkMode mode = GetNetworkModeFromString(m_config.m_modeStr);

	Strings hostIPAndPort;
	SplitStringOnDelimiter(hostIPAndPort, m_config.m_hostAddressStr, ':');

	WSADATA data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	// result will be 0 if WSAStartup was successful, error code otherwise
	if (result != 0)
	{
		ERROR_AND_DIE("Could not startup windows socket!");
	}

	if (mode == NetworkMode::SERVER)
	{
		m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		unsigned long blockingMode = 1;
		result = ioctlsocket(m_listenSocket, FIONBIO, &blockingMode);
		if (result != 0)
		{
			ERROR_AND_DIE("Server could not set blocking mode for server listen socket!");
		}

		m_hostAddress = INADDR_ANY;
		m_hostPort = (unsigned short)(atoi(hostIPAndPort[1].c_str()));

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = htonl(m_hostAddress);
		addr.sin_port = htons(m_hostPort);
		result = bind(m_listenSocket, (sockaddr*)&addr, (int)sizeof(addr));
		if (result != 0)
		{
			ERROR_AND_DIE("Server could not bind socket!");
		}

		result = listen(m_listenSocket, SOMAXCONN);
		if (result != 0)
		{
			ERROR_AND_DIE("Server could not start listening on socket!");
		}
	}
	else if (mode == NetworkMode::CLIENT)
	{
		m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		unsigned long blockingMode = 1;
		ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);

		if (hostIPAndPort.size() != 2)
		{
			ERROR_AND_DIE("Ill-formed host address: NetSystem failed to start!");
		}

		IN_ADDR addr;
		result = inet_pton(AF_INET, hostIPAndPort[0].c_str(), &addr);
		m_hostAddress = ntohl(addr.S_un.S_addr);
		m_hostPort = (unsigned short)(atoi(hostIPAndPort[1].c_str()));
	}


	m_sendBuffer = new char[m_config.m_sendBufferSize];
	std::memset(m_sendBuffer, 0, m_config.m_sendBufferSize);
	m_recvBuffer = new char[m_config.m_recvBufferSize];
	std::memset(m_recvBuffer, 0, m_config.m_recvBufferSize);
}

void NetSystem::BeginFrame()
{
	NetworkMode mode = GetNetworkModeFromString(m_config.m_modeStr);

	if (mode == NetworkMode::SERVER)
	{
		if (m_connectionState == ConnectionState::NOT_CONNECTED || m_connectionState == ConnectionState::ATTEMPTING)
		{
			uintptr_t socket = accept(m_listenSocket, NULL, NULL);
			m_connectionState = ConnectionState::ATTEMPTING;

			if (socket != SOCKET_INVALID)
			{
				m_connectionState = ConnectionState::CONNECTED;
				unsigned long blockingMode = 1;
				int result = ioctlsocket(m_listenSocket, FIONBIO, &blockingMode);
				if (result != 0)
				{
					ERROR_AND_DIE("Could not set blocking mode on client socket!");
				}
				m_clientSocket = socket;
			}
		}
	}
	else if (mode == NetworkMode::CLIENT)
	{
		bool shouldAttemptConnection = false;
		
		if (m_connectionState == ConnectionState::ATTEMPTING)
		{
			fd_set exceptSockets;
			fd_set writeSockets;

			FD_ZERO(&exceptSockets);
			FD_ZERO(&writeSockets);

			FD_SET(m_clientSocket, &writeSockets);
			FD_SET(m_clientSocket, &exceptSockets);

			timeval waitTime = {};
			int result = select(0, NULL, &writeSockets, &exceptSockets, &waitTime);

			if (result == SOCKET_ERROR)
			{
				shouldAttemptConnection = true;
			}
			else if (result > 0)
			{
				if (FD_ISSET(m_clientSocket, &exceptSockets))
				{
					shouldAttemptConnection = true;
				}
				else if (FD_ISSET(m_clientSocket, &writeSockets))
				{
					m_connectionState = ConnectionState::CONNECTED;
				}
			}
		}

		if (m_connectionState == ConnectionState::NOT_CONNECTED || shouldAttemptConnection)
		{
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_addr = htonl(m_hostAddress);
			addr.sin_port = htons(m_hostPort);
			connect(m_clientSocket, (sockaddr*)(&addr), (int)sizeof(addr));
			m_connectionState = ConnectionState::ATTEMPTING;
		}
	}

	if (m_connectionState == ConnectionState::CONNECTED)
	{
		SendAndReceiveData();
	}
}

void NetSystem::EndFrame()
{
}

void NetSystem::Shutdown()
{
	closesocket(m_clientSocket);
	closesocket(m_listenSocket);

	WSACleanup();
}

void NetSystem::QueueMessageForSend(std::string message)
{
	if (message.size() > m_config.m_sendBufferSize)
	{
		g_console->AddLine(DevConsole::WARNING, "Message too long to send!");
		return;
	}

	//std::memcpy(m_sendBuffer, message.c_str(), message.size());
	m_sendQueue.push_back(message);
}

std::string NetSystem::GetNextReceivedMessage()
{
	if (m_recvQueue.empty())
	{
		return "";
	}
	
	std::string message = m_recvQueue[0];
	m_recvQueue.erase(m_recvQueue.begin());
	return message;
}

NetworkMode NetSystem::GetNetworkMode() const
{
	return GetNetworkModeFromString(m_config.m_modeStr);
}

NetworkMode NetSystem::GetNetworkModeFromString(std::string networkModeStr)
{
	if (!strcmp(networkModeStr.c_str(), "Client"))
	{
		return NetworkMode::CLIENT;
	}
	else if (!strcmp(networkModeStr.c_str(), "Server"))
	{
		return NetworkMode::SERVER;
	}

	return NetworkMode::NONE;
}

void NetSystem::SendAndReceiveData()
{
	NetworkMode mode = GetNetworkModeFromString(m_config.m_modeStr);

	// Send Data
	//---------------------------------------------------------------------------------------
	for (int messageIndex = 0; messageIndex < (int)m_sendQueue.size(); messageIndex++)
	{
		std::memcpy(m_sendBuffer, m_sendQueue[messageIndex].c_str(), m_sendQueue[messageIndex].length());
		if (strlen(m_sendBuffer) != 0)
		{
			int result = send(m_clientSocket, m_sendBuffer, (int)strlen(m_sendBuffer) + 1, 0);
			if (result <= 0)
			{
				int errCode = WSAGetLastError();
				if (errCode == 0 || errCode == WSAECONNABORTED || errCode == WSAECONNRESET)
				{
					m_connectionState = ConnectionState::NOT_CONNECTED;
				}
				else if (errCode != WSAEWOULDBLOCK)
				{
					ERROR_AND_DIE("Could not send over network!");
				}
			}

			if (result == strlen(m_sendBuffer) + 1)
			{
				memset(m_sendBuffer, 0, m_config.m_sendBufferSize);
			}
		}
	}
	m_sendQueue.clear();
	
	// Receive Data
	//---------------------------------------------------------------------------------------
	int result = recv(m_clientSocket, m_recvBuffer, sizeof(m_recvBuffer), 0);
	if (result <= 0)
	{
		int errCode = WSAGetLastError();
		if (errCode == 0 || errCode == WSAECONNABORTED || errCode == WSAECONNRESET)
		{
			m_connectionState = ConnectionState::NOT_CONNECTED;
		}
		else if (errCode != WSAEWOULDBLOCK)
		{
			ERROR_AND_DIE("Could not receive over the socket!");
		}
	}

	for (int recvCharIndex = 0; recvCharIndex < result; recvCharIndex++)
	{
		if (m_recvBuffer[recvCharIndex] == '\0')
		{
			m_recvQueue.push_back(m_partialReceivedMessage);
			m_partialReceivedMessage = "";
			continue;
		}
		m_partialReceivedMessage += m_recvBuffer[recvCharIndex];
	}

	if (m_connectionState == ConnectionState::NOT_CONNECTED && mode == NetworkMode::CLIENT)
	{
		FireEvent("NetworkDisconnected");
		g_console->AddLine(DevConsole::WARNING, "Connection lost. Attempting to reconnect...");
		closesocket(m_clientSocket);
		//WSACleanup();
		//m_clientSocket = SOCKET_INVALID;
		InitializeClientSocket();
	}
	else if (m_connectionState == ConnectionState::NOT_CONNECTED && mode == NetworkMode::SERVER)
	{
		//m_clientSocket = SOCKET_INVALID;
	}
}

void NetSystem::InitializeClientSocket()
{
	Strings hostIPAndPort;
	SplitStringOnDelimiter(hostIPAndPort, m_config.m_hostAddressStr, ':');

	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	unsigned long blockingMode = 1;
	ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);

	if (hostIPAndPort.size() != 2)
	{
		ERROR_AND_DIE("Ill-formed host address: NetSystem failed to start!");
	}

	IN_ADDR addr;
	inet_pton(AF_INET, hostIPAndPort[0].c_str(), &addr);
	m_hostAddress = ntohl(addr.S_un.S_addr);
	m_hostPort = (unsigned short)(atoi(hostIPAndPort[1].c_str()));
}
