#include "pch.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent(Vector3 pos) : BaseComponent(ComponentType::Transform), _position(pos), _rotation{Vector3::Zero()}, _moveSpeed(defaultWalkSpeed)
{
}
TransformComponent::TransformComponent() : BaseComponent(ComponentType::Transform),_moveSpeed(defaultWalkSpeed), _rotation{Vector3::Zero()}
{
	_position = GenerateRandomPos();
}

TransformComponent::~TransformComponent()
{
}


void TransformComponent::SetRandomDirection()
{
	const float randomRange = 135.0f; // -135 ~ 135

	float offset = static_cast<float>(GetRandDouble(-randomRange, randomRange));

	SetDirection(_rotation.y + offset);

	return;
}

void TransformComponent::Move(float delta)
{
	Vector3 moveVector = _dirNormalized * delta * _moveSpeed;
	if (CanGo(_position.x + moveVector.x, _position.z + moveVector.z) == false)
		return;

	_position += moveVector;

	//printf(" Transform Update -  pos [ %0.3f, %0.3f, %0.3f ]]\n", _position.x, _position.y, _position.z);
	
}

void TransformComponent::SetDirection(float rotY)
{
	const float deg2Rad = 3.141592f / 180.0f;
	
	_rotation.y = NormalizeAngle(rotY);
	// transform.forward.normalized와 같다. 내가 바라보고 있는 방향의 방향 벡터, 현재 로테이션은 RotY
	_dirNormalized = Vector3(sinf(_rotation.y * deg2Rad), 0, cosf(_rotation.y * deg2Rad));
 
}

void TransformComponent::SetPosition(const Vector3& pos)
{
	_position = pos;
}

bool TransformComponent::CanGo(float prediction_x, float prediction_z)
{
	return prediction_x > sectorMinX && prediction_x < sectorMaxX && prediction_z > sectorMinZ && prediction_z < sectorMaxZ;
}
