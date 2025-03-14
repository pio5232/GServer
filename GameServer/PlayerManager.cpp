#include "pch.h"
#include "GamePlayer.h"
#include "PlayerManager.h"
#include "GameServer.h"
#include "PacketHandler.h"
#include "GameSession.h"
#include "AIPlayer.h"

GamePlayerPtr C_Content::PlayerManager::CreatePlayer(GameSessionPtr gameSessionPtr)
{
	SRWLockGuard lockGuard(&_playerLock);
		
	GamePlayerPtr gamePlayerPtr = std::make_shared<GamePlayer>(gameSessionPtr);

	_idToPlayerDic.insert({ gameSessionPtr->GetUserId(), gamePlayerPtr });
	
	_playerCount.fetch_add(1);
	
	printf("CreatePlayer - GameSession User ID = [ %d ]\n ", gameSessionPtr->GetUserId());
	return gamePlayerPtr;
}

AIPlayerPtr C_Content::PlayerManager::CreateAI()
{
	static ULONGLONG idGenerator = 0;

	AIPlayerPtr aiPlayer = std::make_shared<C_Content::AIPlayer>();

	//SRWLockGuard lockGuard(&_aiLock);

	//_idToAiDic.insert({ aiId, aiPlayer });

	return aiPlayer;
}

ErrorCode C_Content::PlayerManager::DeletePlayer(ULONGLONG userId)
{
	SRWLockGuard lockGuard(&_playerLock);
	
	if (_idToPlayerDic.find(userId) == _idToPlayerDic.end())
		return ErrorCode::NOT_FOUND;

	_idToPlayerDic.erase(userId);
	
	_playerCount.fetch_sub(1);

	return ErrorCode::NONE;
}

//ErrorCode C_Content::PlayerManager::DeleteAI(ULONGLONG userId)
//{
//	SRWLockGuard lockGuard(&_aiLock);
//
//	if (_idToAiDic.find(userId) == _idToAiDic.end())
//		return ErrorCode::NOT_FOUND;
//
//	_idToAiDic.erase(userId);
//
//	_playerCount.fetch_sub(1);
//
//	return ErrorCode::NONE;
//}

ErrorCode C_Content::PlayerManager::SendToAllPlayer(SharedSendBuffer buffer)
{
	std::vector<std::weak_ptr<GameSession>> gameSessionWptrVec;
	{
		SRWLockGuard lockGuard(&_playerLock);
		
		gameSessionWptrVec.reserve(_playerCount);

		for (auto& [userId, gamePlayerPtr] : _idToPlayerDic)
		{
			gameSessionWptrVec.push_back(gamePlayerPtr->GetOwnerSessionWptr());
		}
	}

	int count = 0;
	for (std::weak_ptr<GameSession>& gameSessionWptr : gameSessionWptrVec)
	{
		if (gameSessionWptr.expired())
			continue;

		GameSessionPtr gameSessionPtr = gameSessionWptr.lock();

		gameSessionPtr->Send(buffer);
		
		count++;
	}
	
	//printf("SendToAll Player count - %d\n", count);
	return ErrorCode::NONE;
}

void C_Content::PlayerManager::SetDicSize(uint16 requiredUsers, uint16 maxUsers)
{
	// 이거 서버의 maxCnt가 0이라서 받지 않음.
	_idToPlayerDic.reserve(requiredUsers);
}

void C_Content::PlayerManager::MakeUserCharactersPacket(C_Network::SharedSendBuffer& sendBuffer)
{
	std::vector<GamePlayerPtr> gamePlayersVec;
	{
		SRWLockGuard lockGuard(&_playerLock);

		for (auto& [userId, gamePlayerPtr] : _idToPlayerDic)
		{
			gamePlayersVec.push_back(gamePlayerPtr);
		}
	}

	C_Network::MakeOtherCharacterPacket makeOtherPacket;
	for (GamePlayerPtr& gamePlayer : gamePlayersVec)
	{
		makeOtherPacket.entityId = gamePlayer->GetEntityId();
		makeOtherPacket.pos = gamePlayer->GetPosConst();

		*sendBuffer << makeOtherPacket.size << makeOtherPacket.type << makeOtherPacket.entityId << makeOtherPacket.pos << makeOtherPacket.isAi;
	}

	return;
}

C_Content::PlayerManager::PlayerManager() : _playerCount(0)
{
	InitializeSRWLock(&_playerLock); 
}
