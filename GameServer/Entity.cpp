#include "pch.h"
#include "Entity.h"
#include <random>
C_Content::Entity::Entity(EntityType type) : _entityType(type), _transformComponent()
{
	static std::mt19937_64 gen(1);
	static volatile ULONGLONG generator = gen();

	_entityId = InterlockedIncrement64((LONGLONG*)&generator);

	printf("My ID : %llu\n", _entityId);

	Vector3 position = _transformComponent.GetPosConst();
	_curSector.x = position.x / sectorCriteriaSize + 1;
	_curSector.z = position.z / sectorCriteriaSize + 1;

	_prevSector = _curSector;
}

C_Content::Entity::Entity(EntityType type, const Vector3& startPos) : _entityType(type), _transformComponent(startPos)
{
	static std::mt19937_64 gen(1);
	static volatile ULONGLONG generator = gen();

	_entityId = InterlockedIncrement64((LONGLONG*)&generator);

	printf("My ID : %llu\n", _entityId);

	Vector3 position = _transformComponent.GetPosConst();
	_curSector.x = position.x / sectorCriteriaSize + 1;
	_curSector.z = position.z / sectorCriteriaSize + 1;

	_prevSector = _curSector;
}

bool C_Content::Entity::IsSectorUpdated()
{
	_prevSector = _curSector;

	Vector3 position = _transformComponent.GetPosConst();

	_curSector.x = position.x / sectorCriteriaSize + 1;
	_curSector.z = position.z / sectorCriteriaSize + 1;

	return _prevSector != _curSector;
}
