#include "pch.h"
#include "Player.h"
#include "PlayerStateController.h"
#include "PlayerState.h"
/// +-----------------------+
///	| PlayerStateController |
/// +-----------------------+
/// 
C_Content::PlayerStateController::PlayerStateController(C_Content::Player* player) : _player(player), _actionStateBasePtr(&NoneActionState::GetInstance()), 
_moveStateBasePtr(nullptr),_actionElapsedTime(0)
{
}

void C_Content::PlayerStateController::ChangeState(PlayerMoveStateBase* to)
{
	if (to == nullptr || to == _moveStateBasePtr)
		return;

	_moveStateBasePtr = to;
}

void C_Content::PlayerStateController::ChangeState(PlayerActionStateBase* to)
{
	if (to == nullptr || to == _actionStateBasePtr)
		return;

	_actionElapsedTime = 0;

	_actionStateBasePtr->OnExitState(_player);
	_actionStateBasePtr = to;
	_actionStateBasePtr->OnEnterState(_player);
}

void C_Content::PlayerStateController::Update(float delta)
{
	if (_moveStateBasePtr != nullptr)
		_moveStateBasePtr->OnStayState(_player, delta);

	if (_actionStateBasePtr != nullptr)
	{		
		if (_actionStateBasePtr->GetType() == C_Content::PlayerActionStateBase::ActionState::None)
			return;

		_actionStateBasePtr->OnStayState(_player, delta);		
	

		_actionElapsedTime += delta;

		if (_actionElapsedTime >= _actionStateBasePtr->GetDuration())
			ChangeState(&C_Content::NoneActionState::GetInstance());
	}

}

C_Content::PlayerMoveStateBase::MoveState C_Content::PlayerStateController::GetMoveType()
{
	if(_moveStateBasePtr == nullptr)
		return C_Content::PlayerMoveStateBase::MoveState::None;

	return _moveStateBasePtr->GetType();
}

C_Content::PlayerActionStateBase::ActionState C_Content::PlayerStateController::GetActionType()
{
	if (_actionStateBasePtr == nullptr)
		return PlayerActionStateBase::ActionState::None;

	return _actionStateBasePtr->GetType();
} 
