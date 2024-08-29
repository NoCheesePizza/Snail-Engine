#include "ComponentManager.h"

namespace Snail
{

	void ComponentManager::init()
	{
		registerComponent<TransformComponent>();
		registerComponent<ShapeComponent>();
	}

	void ComponentManager::update()
	{

	}

	void ComponentManager::free()
	{

	}

	const std::string &ComponentManager::getCompName(CompId compId)
	{
		crashIf(compId >= compNames.size(), "Invalid compId " + toStr(compId));
		return compNames[compId];
	}

	CompId ComponentManager::getCompId(const std::string &compName)
	{
		crashIf(!compIds.count(compName), "Invalid compName " + Util::quote(compName));
		return compIds.at(compName);
	}

	void ComponentManager::signalEntityRemoved(EntityId id)
	{
		for (const auto &[compName, compArray] : compArrays)
			compArray->removeDataIfPresent(id);
	}

}