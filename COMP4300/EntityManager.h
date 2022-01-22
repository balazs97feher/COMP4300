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
    typedef std::vector<std::shared_ptr<Entity>> EntityVector;
    typedef std::map<EntityTag, EntityVector> EntityMap;

    class EntityManager
    {
    public:
        EntityManager();

        const EntityVector& getEntities();
        const EntityVector& getEntities(const EntityTag tag);

        std::shared_ptr<Entity> addEntity(const EntityTag tag);
        void update();

    private:
        EntityVector mEntities;
        EntityMap mEntityMap;
        size_t mTotalEntities;
    
        EntityVector mToAdd;
    };
} // namespace goldenhand
