#pragma

#include <string>

enum class EntityTag
{
    Default
};

class Entity
{
public:
    Entity(const EntityTag tag, const size_t id);

    const EntityTag tag() const;
    bool isAlive() const;
    void destroy();

private:
    const EntityTag mTag;
    const size_t mId;
    bool mAlive;
};

