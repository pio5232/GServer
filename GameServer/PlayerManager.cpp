#include "pch.h"
#include "GamePlayer.h"
#include "PlayerManager.h"
#include "GameServer.h"
#include "PacketHandler.h"
#include "GameSession.h"
#include "AIPlayer.h"
#include "GameWorld.h"
using namespace C_Network;
GamePlayerPtr C_Content::PlayerManager::CreatePlayer(GameSessionPtr gameSessionPtr, GameWorld* worldPtr)
{
	SRWLockGuard lockGuard(&_playerLock);
		
	GamePlayerPtr gamePlayerPtr = std::make_shared<GamePlayer>(gameSessionPtr, worldPtr);

	_idToPlayerDic.insert({ gameSessionPtr->GetUserId(), gamePlayerPtr });
	
	_playerCount.fetch_add(1);
	
	printf("CreatePlayer - GameSession User ID = [ %llu ]\n ", gameSessionPtr->GetUserId());
	return gamePlayerPtr;
}

AIPlayerPtr C_Content::PlayerManager::CreateAI(GameWorld* worldPtr)
{
	static ULONGLONG idGenerator = 0;

	AIPlayerPtr aiPlayer = std::make_shared<C_Content::AIPlayer>(worldPtr);

	//SRWLockGuard lockGuard(&_aiLock);

	//_idToAiDic.insert({ aiId, aiPlayer });

	return aiPlayer;
}

ErrorCode C_Content::PlayerManager::DeletePlayer(ULONGLONG userId)
{
	{
		SRWLockGuard lockGuard(&_playerLock);

		if (_idToPlayerDic.find(userId) == _idToPlayerDic.end())
			return ErrorCode::NOT_FOUND;

		_idToPlayerDic.erase(userId);
	}
	
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
//	_gamePlayerCount.fetch_sub(1);
//
//	return ErrorCode::NONE;
//}

ErrorCode C_Content::PlayerManager::SendToPlayer(C_Network::SharedSendBuffer buffer, ULONGLONG userId)
{
	std::unordered_map<ULONGLONG,GamePlayerPtr>::iterator findIt;
	
	{
		SRWLockGuard lockGuard(&_playerLock);
		findIt = _idToPlayerDic.find(userId);

		if(findIt == _idToPlayerDic.end())
			return ErrorCode::NOT_FOUND;
	}

	std::weak_ptr<C_Network::GameSession> gameSessionWptr = findIt->second->GetOwnerSessionWptr();

	if (gameSessionWptr.expired())
		return ErrorCode::ACCESS_DELETE_MEMBER;

	GameSessionPtr gameSessionPtr = gameSessionWptr.lock();

	gameSessionPtr->Send(buffer);

	return ErrorCode::NONE;
}

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
		makeOtherPacket.pos = gamePlayer->GetPosition();

		*sendBuffer << makeOtherPacket.size << makeOtherPacket.type << makeOtherPacket.entityId << makeOtherPacket.pos;
	}

	return;
}

C_Content::PlayerManager::PlayerManager() : _playerCount(0)
{
	InitializeSRWLock(&_playerLock); 
}
