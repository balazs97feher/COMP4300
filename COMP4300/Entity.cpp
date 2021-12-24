#include "Entity.h"

using namespace std;

Entity::Entity(const EntityTag tag, const size_t id)
    : mTag(tag), mId(id), mAlive(true)
{
}

const EntityTag Entity::tag() const
{
    return mTag;
}

bool Entity::isAlive() const
{
    return mAlive;
}

void Entity::destroy()
{
    mAlive = false;
}
