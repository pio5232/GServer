#pragma once
#include "StatComponent.h"
#include "TransformComponent.h"
#include "Entity.h"

namespace C_Content
{
	class PlayerStateController;

	class Player : public Entity
	{
	public:
		Player(EntityType type);
		~Player();

		virtual void Update(float delta) = 0;

		void Move(float delta);
		void Stop();
		bool IsDead() const { return _statComponent.IsDead(); }
		
		const Vector3 GetPosConst() const { return _transformComponent.GetPosConst(); }
	protected:
		void BroadcastMoveState();

	protected :
		StatComponent _statComponent;
		TransformComponent _transformComponent;
		std::unique_ptr<PlayerStateController> _stateController;

	private:

	};
}