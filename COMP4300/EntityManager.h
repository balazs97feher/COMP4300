#pragma once

#include "Entity.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace goldenhand
{
    // todo: use concepts
    template<typename EntityTag, typename ComponentTuple>
    class EntityManager
    {
    public:
        typedef Entity<EntityTag, ComponentTuple> Entity;
        typedef std::vector<std::shared_ptr<Entity>> EntityVector;
        typedef std::map<EntityTag, EntityVector> EntityMap;

        EntityManager() : mTotalEntities{ 0 } {}

        const EntityVector& getEntities()
        {
            return mEntities;
        }
        const EntityVector& getEntities(const EntityTag tag)
        {
            return mEntityMap[tag];
        }

        std::shared_ptr<Entity> addEntity(const EntityTag tag)
        {
            auto newEntity = std::shared_ptr<Entity>(new Entity(tag, mTotalEntities++));
            mToAdd.push_back(newEntity);

            return newEntity;
        }

        std::shared_ptr<Entity> cloneEntity(const std::shared_ptr<Entity>& original)
        {
            auto clone = addEntity(original->tag());
            cloneComponents(original->mComponents, clone->mComponents);
            return clone;
        }

        void update()
        {
            erase_if(mEntities, [](const auto& e) {return !e->isAlive(); });

            for (auto& [tag, group] : mEntityMap)
            {
                erase_if(group, [](const auto& e) {return !e->isAlive(); });
            }

            for (auto& e : mToAdd)
            {
                mEntities.push_back(e);
                mEntityMap[e->tag()].push_back(e);
            }

            mToAdd.clear();
        }

    private:
        EntityVector mEntities;
        EntityMap mEntityMap;
        size_t mTotalEntities;
    
        EntityVector mToAdd;

        template<size_t I = 0, typename ...Components>
        void cloneComponents(std::tuple<Components...>& original, std::tuple<Components...>& clone)
        {
            if constexpr (I == sizeof...(Components))
            {
                return;
            }
            else
            {
                std::get<I>(clone) = std::get<I>(original);
                cloneComponents<I + 1>(original, clone);
            }
        }
    };
} // namespace goldenhand
