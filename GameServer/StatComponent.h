#pragma once

class StatComponent : public BaseComponent
{
public:
	StatComponent();
	~StatComponent();

	virtual void Update(float delta);

	bool IsAlive() const { return _curHp > 0; }
private:
	
	uint16 _curHp;
	uint16 _maxHp;

	uint16 _attackDamage;
	float _attackRange;
};