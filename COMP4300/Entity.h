#pragma

#include <memory>
#include <string>

namespace goldenhand
{
    template<typename EntityTag, typename ComponentTuple>
    class Entity
    {
        template<typename EntityTag, typename ComponentTuple> friend class EntityManager;

    public:
        const EntityTag tag() const
        {
            return mTag;
        }

        bool isAlive() const
        {
            return mAlive;
        }

        uint16_t id() const
        {
            return mId;
        }

        void destroy()
        {
            mAlive = false;
        }

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
        Entity(const EntityTag tag, const uint16_t id) : mTag(tag), mId(id), mAlive(true) {}

        const EntityTag mTag;
        const uint16_t mId;
        bool mAlive;
        ComponentTuple mComponents;
    };
} // namespace goldenhand
