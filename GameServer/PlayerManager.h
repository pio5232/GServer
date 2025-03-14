#pragma once
#include <unordered_map>
#include "JobQueue.h"

namespace C_Content
{
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

		GamePlayerPtr CreatePlayer(GameSessionPtr gameSessionPtr);
		AIPlayerPtr CreateAI();

		ErrorCode DeletePlayer(ULONGLONG userId);
		//ErrorCode DeleteAI(ULONGLONG userId);
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