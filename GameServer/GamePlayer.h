#pragma once
#include "Player.h"


namespace C_Content
{
	// Session - Network / User - Contents ����.
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
		// �̷� �Լ� ����� �����ϴ� �κ� ��� �и��ϵ��� �Ѵ�.
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