#pragma once


namespace C_Content
{
	class GameWorld
	{
	public:
		GameWorld();
		~GameWorld();

		void Start();
		void Update();

		void EnqueueAction(Action&& action);
		void ProcessActions();

		void AddEntity(EntityPtr entityPtr);
		void RemoveEntity(ULONGLONG entityId);

		void SetDSCount(uint16 predMaxCnt);

		WorldChatPtr GetWorldChat() { return _worldChat; }

	private:
		HANDLE _startEvent;

		std::queue<Action> _actionQueue;
		SRWLOCK _actionLock;

		std::thread _logicThread;

		// [Entity_ID, shared_ptr<Entity>]
		std::unordered_map<ULONGLONG, EntityPtr> _entityDic;
		
		// [shared_ptr<Entity>, vectorIndex] - 
		std::unordered_map<EntityPtr, int> _entityToVectorIdxDic;
		std::vector<EntityPtr> _entityArr;

		WorldChatPtr _worldChat;


	};
}
