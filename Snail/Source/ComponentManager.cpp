#include "ComponentManager.h"

namespace Snail
{

	void ComponentManager::init()
	{
		registerComponent<TransformComponent>();
	}

	void ComponentManager::update()
	{

	}

	void ComponentManager::free()
	{

	}

	void ComponentManager::signalEntityRemoved(EntityId id)
	{
		for (const auto &[compName, compArray] : compArrays)
			compArray->removeDataIfPresent(id);
	}

}