#pragma once

class StatComponent : public BaseComponent
{
public:
	StatComponent();
	~StatComponent();

	virtual void Update(float delta);

	bool IsDead() const { return _curHp == 0; }
private:
	
	short _curHp;
	short _maxHp;

	short _attackDamage;
	float _attackRange;
};