#pragma

#include <string>

class EntityManager;

enum class EntityTag
{
    Default
};

class Entity
{
    friend class EntityManager;

public:
    const EntityTag tag() const;
    bool isAlive() const;
    void destroy();

private:
    Entity(const EntityTag tag, const size_t id);
    
    const EntityTag mTag;
    const size_t mId;
    bool mAlive;
};

