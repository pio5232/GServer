#pragma once
#include "Player.h"

namespace C_Content
{
	// Session - Network / User - Contents ¿µ¿ª.
	class GamePlayer : public Player
	{
	public:
		GamePlayer(GameSessionPtr gameSessionPtr); // User.
		~GamePlayer() { printf("~~~GamePlayer Destructor\n"); }

		virtual void Update(float delta);

		ULONGLONG GetUserId() const { return _userId; }
		std::weak_ptr<class GameSession> GetOwnerSessionWptr() { return _ownerSession; }
		
		// process packet data
		void ProcessMoveStartPacket(const MoveStartRequestPacket& clientPacket);
		void ProcessMoveStopPacket(const MoveStopRequestPacket& clientPacket);
	private:
		void CheckSync(const Vector3& clientPos);
		ULONGLONG _userId;
		std::weak_ptr<class GameSession> _ownerSession;
	};
}