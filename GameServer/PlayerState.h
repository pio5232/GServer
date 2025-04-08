#pragma once
#include "PlayerStateBase.h"



namespace C_Content
{
	// -------------------------- Move --------------------------

	/// +-------------------+
	/// |  PlayerIdleState	|
	/// +-------------------+
	class PlayerIdleState : public PlayerMoveStateBase
	{
	public:
		static PlayerIdleState& GetInstance()
		{
			static PlayerIdleState instance;

			return instance;
		}
		virtual void OnStayState(C_Content::Player* player, float delta) override;

	private:
		PlayerIdleState();
	};

	/// +-------------------+
	/// |  PlayerMoveState	|
	/// +-------------------+
	class PlayerMoveState : public PlayerMoveStateBase
	{
	public:
		static PlayerMoveState& GetInstance()
		{
			static PlayerMoveState instance;

			return instance;
		}
		virtual void OnStayState(C_Content::Player* player, float delta) override;

	private:
		PlayerMoveState();
	};

	// -------------------------- Action --------------------------

	/// +-----------------------+
	/// |	PlayerAttackState	|
	/// +-----------------------+
	class PlayerAttackState : public PlayerActionStateBase
	{
	public:
		static PlayerAttackState& GetInstance()
		{
			static PlayerAttackState instance;

			return instance;
		}

		virtual void OnEnterState(C_Content::Player* player) override;
		virtual void OnStayState(C_Content::Player* player, float delta) override;
		virtual void OnExitState(C_Content::Player* player) override;

		virtual float GetDuration() const override{ return attackDuration; }
	private:
		PlayerAttackState();
	};

	/// +-----------------------+
	/// |  PlayerAttackedState	|
	/// +-----------------------+
	class PlayerAttackedState : public PlayerActionStateBase
	{
	public:
		static PlayerAttackedState& GetInstance()
		{
			static PlayerAttackedState instance;

			return instance;
		}

		virtual void OnEnterState(C_Content::Player* player) override;
		virtual void OnStayState(C_Content::Player* player, float delta) override;
		virtual void OnExitState(C_Content::Player* player) override;

		virtual float GetDuration() const override { return attackedDuration; }

	private:
		PlayerAttackedState();
	};

	/// +-------------------+
	/// |  PlayerDeadState	|
	/// +-------------------+
	class PlayerDeadState : public PlayerActionStateBase
	{
	public:
		static PlayerDeadState& GetInstance()
		{
			static PlayerDeadState instance;

			return instance;
		}
		virtual void OnEnterState(C_Content::Player* player) override;
		virtual void OnStayState(C_Content::Player* player, float delta) override;
		virtual void OnExitState(C_Content::Player* player) override;

		virtual float GetDuration() const override { return DeadDuration; }

	private:
		PlayerDeadState();
	};

	/// +-------------------+
	/// |  NoneActionState	|
	/// +-------------------+}
	class NoneActionState : public PlayerActionStateBase
	{
	public :
		static NoneActionState& GetInstance()
		{
			static NoneActionState instance;

			return instance;
		}
		virtual void OnEnterState(C_Content::Player* player) override;
		virtual void OnStayState(C_Content::Player* player, float delta) override;
		virtual void OnExitState(C_Content::Player* player) override;
	private:
		NoneActionState();
	};
}