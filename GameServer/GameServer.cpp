#include "pch.h"
#include <chrono>
#include <random>
#include <WS2tcpip.h>
#include "GameServer.h"
#include "LanClient.h"
#include "PacketHandler.h"
#include "PlayerManager.h"
#include "GamePlayer.h"
#include "LanClientSession.h"
#include "PacketMaker.h"
#include "GameWorld.h"
#include "AIPlayer.h"

C_Network::GameServer::GameServer(const NetAddress& netAddr, uint maxSessionCnt, C_Network::SessionCreator creator) : ServerBase(netAddr, maxSessionCnt, creator), _gameInfo{}, _isRunning(false), _loadCompletedCnt(0)
{
	_gameWorld = std::make_unique<C_Content::GameWorld>();

	std::random_device rd;
	std::mt19937_64 generator(rd()); // 시드 섞음.
	_gameInfo.enterToken = generator();

	C_Network::GameClientPacketHandler::Init();

	printf("----------------------------------------------------- GAME SERVER -------\n");
}

C_Network::GameServer::~GameServer()
{
}

void C_Network::GameServer::Init(uint16 roomNumber, uint16 requiredUsers, uint16 maxUsers)
{
	_sessionMgr->SetMaxCount(maxUsers);
	_gameWorld->SetDSCount(maxUsers);
	C_Content::PlayerManager::GetInstance().SetDicSize(requiredUsers, maxUsers);

	_gameInfo.myRoomNumber = roomNumber;
	_gameInfo.requiredUserCnt = requiredUsers;
	_gameInfo.maxUsers = maxUsers;

	_isRunning.store(false);
}

bool C_Network::GameServer::OnConnectionRequest(const SOCKADDR_IN& clientInfo)
{
	return true;
}



void C_Network::GameServer::OnError(int errCode, WCHAR* cause)
{
}


void C_Network::GameServer::LanClientConnect(const NetAddress& netAddr)
{
	// 포트 번호 동적으로 할당 시 진짜 포트를 찾는다.
	SOCKADDR_IN realAddr;
	int addrLen = sizeof(realAddr);

	if (getsockname(GetListenSock(), (SOCKADDR*)&realAddr, &addrLen) == 0)
	{
		_gameInfo.realPort = ntohs(realAddr.sin_port);
	}
	else
	{
		printf("Find Real Port Failed - %d\n", GetLastError());
	}

	_lanClient = std::make_shared<C_Network::LanClient>(netAddr, [self = shared_from_this()]() { return std::static_pointer_cast<C_Network::Session>(std::make_shared<C_Network::LanClientSession>(self)); });

	_lanClient->Connect();
}

void C_Network::GameServer::LanClientDisconnect()
{
	_lanClient->Disconnect();
}

void C_Network::GameServer::MakeAndSendPlayers()
{
	C_Network::SharedSendBuffer sendBuffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(MakeOtherCharacterPacket) * _gameInfo.maxUsers);

	uint16 aiCount = _gameInfo.maxUsers - _gameInfo.requiredUserCnt;

	C_Network::MakeOtherCharacterPacket makeOtherCharacterPacket;

	for (int i = 0; i < aiCount; i++)
	{
		 AIPlayerPtr aiPlayer = C_Content::PlayerManager::GetInstance().CreateAI();

		 _gameWorld->AddEntity(aiPlayer->shared_from_this());
	
		 makeOtherCharacterPacket.entityId = aiPlayer->GetEntityId();
		 makeOtherCharacterPacket.pos = aiPlayer->GetPosConst();
		 makeOtherCharacterPacket.isAi = true;

		 *sendBuffer << makeOtherCharacterPacket.size << makeOtherCharacterPacket.type << makeOtherCharacterPacket.entityId << makeOtherCharacterPacket.pos << makeOtherCharacterPacket.isAi;

	}
	C_Content::PlayerManager::GetInstance().MakeUserCharactersPacket(sendBuffer);

	C_Content::PlayerManager::GetInstance().SendToAllPlayer(sendBuffer);
}

ErrorCode C_Network::GameServer::TryRun()
{
	if (_isRunning.load() == true)
	{
		printf("GameServer 이미 실행중!!\n");

		return ErrorCode::GAME_SERVER_IS_RUNNING;
	}
	if (_gameInfo.requiredUserCnt.load() == C_Content::PlayerManager::GetInstance().GetPlayerCount())
	{
		bool expected = false;
		bool desired = true;

		if (_isRunning.compare_exchange_strong(expected, desired) == true)
		{
			MakeAndSendPlayers();

			printf("Start Game\n");
		}
	}

	return ErrorCode::NONE;
}

void C_Network::GameServer::CheckLoadingAndStartLogic()
{
	int prevCnt = _loadCompletedCnt.fetch_add(1);
	if (prevCnt == (_gameInfo.requiredUserCnt.load() - 1))
	{
		printf("Check Loading & Start Logic!!! [player+ai count : %d]\n",_gameInfo.maxUsers);
		C_Network::GameStartNotifyPacket gameStartNotifyPacket;
		
		// 이거 추후에 패킷 구조 바뀌게되면 (Header말고 데이터 있으면 따로 <<를 통해서 넣어주는 구조로 다시 만들어야함.) AAA
		SharedSendBuffer sendBuffer = C_Network::PacketMaker::MakePacket(gameStartNotifyPacket);

		C_Content::PlayerManager::GetInstance().SendToAllPlayer(sendBuffer);

		_gameWorld->Start();
		// Start Logic
		// StartPacket. timestamp 포함해서 보내기
	}
}

GamePlayerPtr C_Network::GameServer::CreatePlayer(GameSessionPtr gameSessionPtr)
{
	GamePlayerPtr playerPtr = C_Content::PlayerManager::GetInstance().CreatePlayer(gameSessionPtr);

	EnqueueAction([this, playerPtr]() {_gameWorld->AddEntity(playerPtr); });
		
	return playerPtr;
}

ErrorCode C_Network::GameServer::DeletePlayer(GamePlayerPtr gamePlayerPtr)
{
	ErrorCode ret = C_Content::PlayerManager::GetInstance().DeletePlayer(gamePlayerPtr->GetUserId());
	
	EnqueueAction([this, gamePlayerPtr]() { 

		LeaveGameNotifyPacket leaveNotifyPacket;
		leaveNotifyPacket.entityId = gamePlayerPtr->GetEntityId();

		SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(leaveNotifyPacket));
		*buffer << leaveNotifyPacket.size << leaveNotifyPacket.type << leaveNotifyPacket.entityId;

		C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);

		_gameWorld->RemoveEntity(gamePlayerPtr->GetEntityId()); 
	});
	
	return ret;
}

void C_Network::GameServer::EnqueueAction(Action&& action)
{
	_gameWorld->EnqueueAction(std::move(action));
}
