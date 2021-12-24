#include "EntityManager.h"

#include <tuple>

using namespace std;

EntityManager::EntityManager()
    : mTotalEntities(0)
{
}

const EntityVector& EntityManager::getEntities()
{
    return mEntities;
}

const EntityVector& EntityManager::getEntities(const EntityTag tag)
{
    return mEntityMap[tag];
}

shared_ptr<Entity> EntityManager::addEntity(const EntityTag tag)
{
    auto newEntity = shared_ptr<Entity>(new Entity(tag, mTotalEntities++));
    mToAdd.push_back(newEntity);

    return newEntity;
}

void EntityManager::update()
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
