#pragma once
#include "Sector.h"

namespace C_Content
{
	class GameWorld
	{
	public:
		GameWorld();
		~GameWorld();

		void Start();
		void Update();

		void TryEnqueueAction(Action&& action, bool mustEnqueue = false);
		void ProcessActions();

		void AddEntity(EntityPtr entityPtr);
		void RemoveEntity(ULONGLONG entityId);

		void Init(uint16 total, uint16 gamePlayerCount); // 1. ai+player, 2. player
		void SetDSCount(uint16 predMaxCnt);

		WorldChatPtr GetWorldChat() { return _worldChat; }

		void SendPacketAroundSector(const Sector& sector, C_Network::SharedSendBuffer sendBuffer);
		void SendPacketAroundSector(int sectorX, int sectorZ, C_Network::SharedSendBuffer sendBuffer);

		const class SectorManager* GetSectorManagerConst() const { return _sectorManager.get(); }
	private:
		std::atomic<bool> _isRunning;
		std::queue<Action> _actionQueue;
		SRWLOCK _actionLock;

		std::thread _logicThread;

		// [Entity_ID, shared_ptr<Entity>]
		std::unordered_map<ULONGLONG, EntityPtr> _entityDic;
		
		// [shared_ptr<Entity>, vectorIndex] - 
		std::unordered_map<EntityPtr, int> _entityToVectorIdxDic;
		std::vector<EntityPtr> _entityArr;

		WorldChatPtr _worldChat;

		std::unique_ptr<class SectorManager> _sectorManager;
	};
}
