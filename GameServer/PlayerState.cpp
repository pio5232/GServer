#include "pch.h"
#include "PlayerStateBase.h"
#include "PlayerState.h"
#include "Player.h"
/// +-------------------+
/// |  PlayerIdleState	|
/// +-------------------+
/// 

C_Content::PlayerIdleState::PlayerIdleState() : PlayerMoveStateBase(MoveState::Idle){}

void C_Content::PlayerIdleState::OnStayState(C_Content::Player* player, float delta)
{
}


/// +-------------------+
/// |  PlayerMoveState	|
/// +-------------------+
/// 
C_Content::PlayerMoveState::PlayerMoveState() : PlayerMoveStateBase(MoveState::Move) {}

void C_Content::PlayerMoveState::OnStayState(C_Content::Player* player, float delta)
{
	player->Move(delta);

}

/// +-----------------------+
/// |	PlayerAttackState	|
/// +-----------------------+
/// 
C_Content::PlayerAttackState::PlayerAttackState() : PlayerActionStateBase(ActionState::Attack) {}

void C_Content::PlayerAttackState::OnEnterState(C_Content::Player* player)
{
}

void C_Content::PlayerAttackState::OnStayState(C_Content::Player* player, float delta)
{
}

void C_Content::PlayerAttackState::OnExitState(C_Content::Player* player)
{
}

/// +-----------------------+
/// |  PlayerAttackedState	|
/// +-----------------------+
/// 
C_Content::PlayerAttackedState::PlayerAttackedState() : PlayerActionStateBase(ActionState::Attacked) {}

void C_Content::PlayerAttackedState::OnEnterState(C_Content::Player* player)
{
}

void C_Content::PlayerAttackedState::OnStayState(C_Content::Player* player, float delta)
{
}

void C_Content::PlayerAttackedState::OnExitState(C_Content::Player* player)
{
}

/// +-------------------+
/// |  PlayerDeadState	|
/// +-------------------+
C_Content::PlayerDeadState::PlayerDeadState() : PlayerActionStateBase(ActionState::Dead) {}

void C_Content::PlayerDeadState::OnEnterState(C_Content::Player* player)
{
}

void C_Content::PlayerDeadState::OnStayState(C_Content::Player* player, float delta)
{
}

void C_Content::PlayerDeadState::OnExitState(C_Content::Player* player)
{
}

/// +-------------------+
/// |  NoneActionState	|
/// +-------------------+}

void C_Content::NoneActionState::OnEnterState(C_Content::Player* player) {}

void C_Content::NoneActionState::OnStayState(C_Content::Player* player, float delta) {}

void C_Content::NoneActionState::OnExitState(C_Content::Player* player) {}

C_Content::NoneActionState::NoneActionState() : PlayerActionStateBase(ActionState::None) {}

