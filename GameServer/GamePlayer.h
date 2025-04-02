#pragma once
#include "Player.h"

namespace C_Content
{
	// Session - Network / User - Contents 영역.
	class GamePlayer : public Player
	{
	public:
		GamePlayer(GameSessionPtr gameSessionPtr); // User.
		~GamePlayer() { printf("~~~GamePlayer Destructor\n"); }

		virtual void Update(float delta);

		ULONGLONG GetUserId() const { return _userId; }
		std::weak_ptr<class GameSession> GetOwnerSessionWptr() { return _ownerSession; }
		
		// process packet data
		// 이런 함수 만들고 전송하는 부분 모두 분리하도록 한다.
		void ProcessMoveStartPacket(const MoveStartRequestPacket& clientPacket);
		void ProcessMoveStopPacket(const MoveStopRequestPacket& clientPacket);
		void ProcessAttackPacket();
	private:
		void SyncPos(const Vector3& clientPos);
		ULONGLONG _userId;
		std::weak_ptr<class GameSession> _ownerSession;
	};
}