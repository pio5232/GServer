#pragma once
#include <iostream>
namespace C_Content
{
	class Entity : public std::enable_shared_from_this<Entity>
	{
	public:
		enum class EntityType : byte
		{
			GamePlayer = 0,
			AIPlayer,
		};
	public:
		Entity(EntityType type);
		~Entity() {}

		virtual void Update(float delta) = 0;

		ULONGLONG GetEntityId() const { return _entityId; }
		EntityType GetType() const { return _entityType; }
	private:
		ULONGLONG _entityId;
		const EntityType _entityType;
	};
}