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
		Player(EntityType type, float updateInterval);
		~Player();

		virtual void Update(float delta) = 0;

		void Move(float delta);
		bool IsDead() const { return _statComponent.IsDead(); }
		
		const Vector3 GetPosConst() const { return _transformComponent.GetPosConst(); }
	protected:
		void BroadcastMoveState();
		void SendPositionUpdate();

	protected :
		StatComponent _statComponent;
		TransformComponent _transformComponent;
		std::unique_ptr<PlayerStateController> _stateController;

		float _posUpdateInterval;
	private:
		Vector3 _lastUpdatePos;

	};
}