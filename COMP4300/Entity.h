#pragma

#include "Components.h"

#include <memory>
#include <string>
#include <tuple>

typedef std::tuple<CTransform, CShape, CBoundingBox, CScore, CLifeSpan, CAnimation> ComponentTuple;

namespace goldenhand
{
    class EntityManager;
    
    enum class EntityTag
    {
        Default,
        Player,
        Enemy,
        SmallEnemy,
        Bullet,
    };

    class Entity
    {
        friend class EntityManager;

    public:
        const EntityTag tag() const;
        bool isAlive() const;
        void destroy();

        template<typename T>
        T& getComponent()
        {
            return std::get<T>(mComponents);
        }

        template<typename T>
        const T& getComponent() const
        {
            return std::get<T>(mComponents);
        }

        template<typename T>
        bool hasComponent() const
        {
            return getComponent<T>().has;
        }

        template<typename T, typename... Targs>
        T& addComponent(Targs&&... args)
        {
            auto& component = getComponent<T>();
            component = T(std::forward<Targs>(args)...);
            component.has = true;
            return component;
        }

    private:
        Entity(const EntityTag tag, const size_t id);

        const EntityTag mTag;
        const size_t mId;
        bool mAlive;
        ComponentTuple mComponents;
    };
} // namespace goldenhand
