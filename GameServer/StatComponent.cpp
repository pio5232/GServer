#include "pch.h"
#include "StatComponent.h"

StatComponent::StatComponent() : BaseComponent(ComponentType::Stat),
_curHp(defaultMaxHp), _maxHp(defaultMaxHp), _attackRange(defaultAttackRange), _attackDamage(defaultAttackDamage)
{

}

StatComponent::~StatComponent()
{
}

void StatComponent::Update(float delta)
{
}
