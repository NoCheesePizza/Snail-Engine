#pragma once

#include "EntityManager.h"
#include "ComponentArray.h"
#include "Core.h"

#include <unordered_map>

namespace Snail
{

	class ComponentManager : public System
	{
		CompId nextCompId = 0;

		std::unordered_map<std::string, std::unique_ptr<ComponentArrayBase>> compArrays;
		std::unordered_map<std::string, CompId> compIds;

	public:

		void init() override;
		void update() override;
		void free() override;

		template <typename T>
		void registerComponent()
		{
			std::string compName = typeid(T).name();
			crashIf(compArrays.count(compName), "Component " + Util::quote(compName) + 
				" was added more than once");

			compArrays[compName] = std::make_unique<ComponentArray<T>>();
			compIds[compName] = nextCompId++;
		}

		template <typename T>
		CompId getComponentId()
		{
			std::string compName = typeid(T).name();
			crashIf(!compIds.count(compName), "Component " + Util::quote(compName) + " was not registered");

			return compIds.at(compName);
		}

		template <typename T>
		void addComponent(EntityId id, T component = T())
		{
			std::string compName = typeid(T).name();
			crashIf(!compIds.count(compName), "Component " + Util::quote(compName) + " was not registered");
			 
			dynamic_cast<ComponentArray<T> *>(compArrays.at(compName).get())->addData(id, component);

			Signature newSig;
			gs(EntityManager)->setSignature(id, newSig.set(getComponentId<T>()) | 
				gs(EntityManager)->getSignature(id));
		}

		template <typename T>
		void removeComponent(EntityId id)
		{
			std::string compName = typeid(T).name();
			crashIf(!compIds.count(compName), "Component " + Util::quote(compName) + " was not registered");

			dynamic_cast<ComponentArray<T> *>(compArrays.at(compName).get())->removeData(id);
			
			Signature newSig;
			gs(EntityManager)->setSignature(id, newSig.set().reset(getComponentId<T>()) &
				gs(EntityManager)->getSignature(id));
		}

		template <typename T>
		T &getComponent(EntityId id)
		{
			std::string compName = typeid(T).name();
			crashIf(!compIds.count(compName), "Component " + Util::quote(compName) + " was not registered");

			return dynamic_cast<ComponentArray<T> *>(compArrays.at(compName).get())->getData(id);
		}

		void signalEntityRemoved(EntityId id);
	};

}