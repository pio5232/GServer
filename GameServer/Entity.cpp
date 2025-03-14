#include "pch.h"
#include "Entity.h"
#include <random>

C_Content::Entity::Entity(EntityType type) : _entityType(type)
{
	static std::mt19937_64 gen(1);
	static volatile ULONGLONG generator = gen();

	_entityId = InterlockedIncrement64((LONGLONG*)&generator);

	printf("My ID : %llu\n", _entityId);
}
