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
            return mEntities[tag];
        }

        std::shared_ptr<Entity> addEntity(const EntityTag tag)
        {
            auto newEntity = std::shared_ptr<Entity>(new Entity(tag, mTotalEntities++));
            mToAdd.push_back(newEntity);

            return newEntity;
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
    };
} // namespace goldenhand
