#pragma once

namespace C_Content
{
	class Player;

	class PlayerMoveStateBase
	{
	public:
		enum class MoveState : byte
		{
			None = 0,
			Idle,
			Move,
		};
		PlayerMoveStateBase(MoveState state) : _moveState(state) {}

		virtual void OnStayState(C_Content::Player* player, float delta) = 0;

		MoveState GetType() const { return _moveState; }
	private:
		MoveState _moveState;
	};

	class PlayerActionStateBase
	{
	public:
		enum class ActionState
		{
			None = 0,
			Attack,
			Attacked,
			Dead,
		};
		PlayerActionStateBase(ActionState state) : _actionState(state) {}
		virtual void OnEnterState(C_Content::Player* player) = 0;
		virtual void OnStayState(C_Content::Player* player, float delta) = 0;
		virtual void OnExitState(C_Content::Player* player) = 0;

		virtual float GetDuration() const { return 0; }
		ActionState GetType() const { return _actionState; }
	private:
		ActionState _actionState;
	};
}