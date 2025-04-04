#include "pch.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "PacketDefine.h"
#include "BufferMaker.h"
#include <WS2tcpip.h>
#include "GamePlayer.h"
#include "PlayerManager.h"
#include "GameServer.h"

std::atomic<int> C_Network::GameSession::_aliveGameSessionCount;

C_Network::GameSession::GameSession() : _userId(0),_playerPtr(nullptr), _isLoadCompleted(false)
{
	_aliveGameSessionCount.fetch_add(1);
}

void C_Network::GameSession::Init(ULONGLONG userId)
{
	_userId = userId;
}

void C_Network::GameSession::OnConnected()
{
	WCHAR ipWstr[IP_STRING_LEN];

	// 주소체계, &IN_ADDR

	printf("[OnConnected IP : %s, Port : %u ]\n", InetNtopW(AF_INET, &GetNetAddr().GetSockAddr().sin_addr, ipWstr, sizeof(ipWstr) / sizeof(WCHAR))
		, ntohs(GetNetAddr().GetSockAddr().sin_port));
}

void C_Network::GameSession::OnDisconnected()
{
	std::shared_ptr<C_Network::GameServer> server = std::static_pointer_cast<C_Network::GameServer>(GetServer());
	
	if (_playerPtr != nullptr)
	{
		printf("GameSession On DIsconnect - ");
		server->DeletePlayer(_playerPtr);

		_playerPtr.reset();
	}
}

void C_Network::GameSession::OnRecv(C_Utility::CSerializationBuffer& buffer, uint16 type)
{
	GameSessionPtr gameSessionPtr = std::static_pointer_cast<C_Network::GameSession>(shared_from_this());

	if (ErrorCode::NONE != GameClientPacketHandler::ProcessPacket(gameSessionPtr, type, buffer))
	{
		// LOG
	}
}

