#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

#include <functional>
#include <mutex>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Events
            {
                class EventBus
                {
                    public:
                        using SubscriptionId = Types::UInt64;

                    private:
                        struct Listener
                        {
                            SubscriptionId Id;
                            std::function<void(const void*)> Callback;
                        };

                        mutable std::mutex Mutex;
                        std::unordered_map<std::type_index, std::vector<Listener>> Listeners;
                        SubscriptionId NextId = 1;

                    public:
                        EventBus() = default;
                        EventBus(const EventBus&) = delete;
                        EventBus& operator=(const EventBus&) = delete;

                        template <typename EventType, typename Callback>
                        SubscriptionId Subscribe(Callback&& Function)
                        {
                            std::function<void(const EventType&)> Handler(std::forward<Callback>(Function));

                            std::lock_guard Lock(Mutex);

                            if (NextId == 0)
                                throw std::overflow_error("EventBus subscription id space is exhausted");

                            const SubscriptionId Id = NextId++;
                            auto& EventListeners = Listeners[std::type_index(typeid(EventType))];

                            EventListeners.push_back(Listener{
                                Id,
                                [Handler = std::move(Handler)](const void* EventData)
                                {
                                    Handler(*static_cast<const EventType*>(EventData));
                                }
                            });

                            return Id;
                        }

                        template <typename EventType>
                        void Publish(const EventType& Event) const
                        {
                            std::vector<std::function<void(const void*)>> Callbacks;

                            {
                                std::lock_guard Lock(Mutex);
                                const auto It = Listeners.find(std::type_index(typeid(EventType)));

                                if (It == Listeners.end())
                                    return;

                                Callbacks.reserve(It->second.size());

                                for (const Listener& Item : It->second)
                                    Callbacks.push_back(Item.Callback);
                            }

                            for (const auto& Callback : Callbacks)
                                Callback(&Event);
                        }

                        template <typename EventType>
                        Types::Size GetListenerCount() const
                        {
                            std::lock_guard Lock(Mutex);
                            const auto It = Listeners.find(std::type_index(typeid(EventType)));
                            return It == Listeners.end() ? 0 : It->second.size();
                        }

                        Types::Bool Unsubscribe(SubscriptionId Id);
                        void Clear();
                        Types::Size GetSubscriptionCount() const;
                };
            }
        }
    }
}
