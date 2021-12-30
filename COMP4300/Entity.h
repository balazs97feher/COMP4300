#pragma

#include "Components.h"

#include <memory>
#include <string>

class EntityManager;

enum class EntityTag
{
    Default,
    Player,
    Enemy,
    Bullet,
};

class Entity
{
    friend class EntityManager;

public:
    const EntityTag tag() const;
    bool isAlive() const;
    void destroy();

    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifeSpan> cLifeSpan;

private:
    Entity(const EntityTag tag, const size_t id);
    
    const EntityTag mTag;
    const size_t mId;
    bool mAlive;
};

