#pragma once

using namespace C_Network;
class TransformComponent : public BaseComponent
{
public:
	TransformComponent(Vector3 pos);
	TransformComponent();

	~TransformComponent();

	virtual void Update(float delta);

	bool IsMoving() const { return _isMoving; }
	uint16 GetMoveDir() const { return _moveDir; }

	const Vector3& GetPosConst() const { return _position; }
	void AutoChangeDir();
	void SetDir(uint16 dir);

	void SetPos(const Vector3& pos);
private:
	bool CanGo(float prediction_x, float prediction_z);

private:
	Vector3 _rotation;
	Vector3 _position;
	float _moveSpeed;

	Vector3 _dirNormalized;
	MoveDir _moveDir;
	bool _isMoving;
};

