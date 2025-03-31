#pragma once

#include "PlayerStateBase.h"

namespace C_Content
{
	class Player;

	/// +-----------------------+
	///	| PlayerStateController |
	/// +-----------------------+
	class PlayerStateController
	{
	public:
		PlayerStateController(Player* player);

		void ChangeState(PlayerMoveStateBase* to);
		void ChangeState(PlayerActionStateBase* to);
		
		void Update(float delta);
		PlayerMoveStateBase::MoveState GetMoveType();
		PlayerActionStateBase::ActionState GetActionType();
	private:
		// 동적할당하지않는 포인터.
		C_Content::Player* _player;

		C_Content::PlayerActionStateBase* _actionStateBasePtr;
		C_Content::PlayerMoveStateBase* _moveStateBasePtr;

		float _actionElapsedTime;
	};
}

