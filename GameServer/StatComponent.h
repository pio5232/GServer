#pragma once

class StatComponent : public BaseComponent
{
public:
	StatComponent();
	~StatComponent();

	bool IsDead() const 
	{ 
		return _curHp == 0; 
	}

	void TakeDamage(uint16 damage);

	uint16 GetHp() const { return _curHp; }
	uint16 GetAttackDamage() const { return _attackDamage; }
	float GetAttackRange() const { return _attackRange; }
private:
	
	uint16 _curHp;
	uint16 _maxHp;

	uint16 _attackDamage;
	float _attackRange;
};