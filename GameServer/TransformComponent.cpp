#include "pch.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent(Vector3 pos) : BaseComponent(ComponentType::Transform), _position(pos), _isMoving(false), _moveDir(MoveDir::DIR_MAX),
_moveSpeed(defaultWalkSpeed)
{
}
TransformComponent::TransformComponent() : BaseComponent(ComponentType::Transform), _moveDir(MoveDir::DIR_MAX),
_moveSpeed(defaultWalkSpeed), _isMoving(false)
{
	_position = GenerateRandomPos();
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::Update(float delta)
{
	if (_isMoving == false)
		return;

	Vector3 moveVector = _dirNormalized * delta * _moveSpeed;
	if (CanGo(_position.x + moveVector.x, _position.z + moveVector.z) == false)
		return;

	_position += moveVector;

	//printf(" Transform Update -  pos [ %0.3f, %0.3f, %0.3f ]]\n", _position.x, _position.y, _position.z);
}

void TransformComponent::AutoChangeDir()
{
	uint16 newDir;
	do
	{
		//newDir = GetRand(0, DIR_MAX - 1);
		newDir = GetRand(0, DIR_MAX + 2);// DIR에 해당하지 않음 -> STOP으로 볼 것임

		if (newDir >= DIR_MAX)
		{
			_isMoving = false;
			return;
		}
	} while (newDir == _moveDir);

	SetDir(newDir);

	return;
}

void TransformComponent::SetDir(uint16 dir)
{
	if (dir >= MoveDir::LEFT && dir < MoveDir::DIR_MAX)
	{
		_moveDir = static_cast<MoveDir>(dir);
		_isMoving = true;
	}
	else
	{
		_moveDir = MoveDir::STOP;
		_dirNormalized = Vector3::Zero();
		_isMoving = false;
		return;
	}

	switch (_moveDir)
	{
	case LEFT:
		_dirNormalized = Vector3::Left();
		break;
	case LEFT_UP:
		_dirNormalized = Vector3(-1.0f, 0, 1.0f).Normalize();
		break;
	case UP:
		_dirNormalized = Vector3::Forward();
		break;
	case RIGHT_UP:
		_dirNormalized = Vector3(1.0f, 0, 1.0f).Normalize();
		break;
	case RIGHT:
		_dirNormalized = Vector3::Right();
		break;
	case RIGHT_DOWN:
		_dirNormalized = Vector3(1.0f, 0, -1.0f).Normalize();
		break;
	case DOWN:
		_dirNormalized = Vector3::Back();
		break;
	case LEFT_DOWN:
		_dirNormalized = Vector3(-1.0f, 0, -1.0f).Normalize();
		break;
	default:
		break;
	}
}

void TransformComponent::SetPos(const Vector3& pos)
{
	_position = pos;
}

bool TransformComponent::CanGo(float prediction_x, float prediction_z)
{
	return prediction_x > sectorMinX && prediction_x < sectorMaxX && prediction_z > sectorMinZ && prediction_z < sectorMaxZ;
}
