#pragma once
#include <unordered_map>
#include "JobQueue.h"

namespace C_Content
{
	class GameWorld;
	class PlayerManager //: public C_Utility::JobQueue
	{
	public:
		static PlayerManager& GetInstance()
		{
			static PlayerManager instance;

			return instance;
		}
		PlayerManager(const PlayerManager&) = delete;
		PlayerManager& operator=(const PlayerManager&) = delete;

		GamePlayerPtr CreatePlayer(GameSessionPtr gameSessionPtr,class C_Content::GameWorld* worldPtr);
		AIPlayerPtr CreateAI(class C_Content::GameWorld* worldPtr);

		ErrorCode DeletePlayer(ULONGLONG userId);
		//ErrorCode DeleteAI(ULONGLONG userId);

		ErrorCode SendToPlayer(C_Network::SharedSendBuffer buffer, ULONGLONG userId);
		ErrorCode SendToAllPlayer(C_Network::SharedSendBuffer buffer);

		uint16 GetPlayerCount() { return _playerCount.load(); }

		void SetDicSize(uint16 requiredUsers, uint16 maxUsers);

		// 모든 플레이어에 대한 MakeOtherPacket 생성
		void MakeUserCharactersPacket(C_Network::SharedSendBuffer& sendBuffer);

		~PlayerManager() { _idToPlayerDic.clear(); }

	private:
		PlayerManager();

	private:
	// uint maxPlayerCnt, class GameServer* owner);
		std::unordered_map<ULONGLONG, GamePlayerPtr> _idToPlayerDic;
		SRWLOCK _playerLock;
		std::atomic<uint16> _playerCount;
	};
}