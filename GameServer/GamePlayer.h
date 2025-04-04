#pragma once
#include "Player.h"


namespace C_Content
{
	// Session - Network / User - Contents 영역.
	class GameWorld;
	class GamePlayer : public Player
	{
	public:
		GamePlayer(GameSessionPtr gameSessionPtr, GameWorld* worldPtr); // User.
		~GamePlayer() { _aliveGamePlayerCount.fetch_sub(1); }

		virtual void Update(float delta);

		ULONGLONG GetUserId() const { return _userId; }
		std::weak_ptr<class C_Network::GameSession> GetOwnerSessionWptr() { return _ownerSession; }
		
		// process packet data
		// 이런 함수 만들고 전송하는 부분 모두 분리하도록 한다.
		void ProcessMoveStartPacket(const C_Network::MoveStartRequestPacket& clientPacket);
		void ProcessMoveStopPacket(const C_Network::MoveStopRequestPacket& clientPacket);
		void ProcessAttackPacket();

		static int GetAliveGamePlayerCount() { return _aliveGamePlayerCount.load(); }
	private:
		static std::atomic<int> _aliveGamePlayerCount;

		void SyncPos(const Vector3& clientPos);
		ULONGLONG _userId;
		std::weak_ptr<class C_Network::GameSession> _ownerSession;
	};
}