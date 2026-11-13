#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.Function.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

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
                        using Callback = Containers::Function<void(const void*)>;

                        struct Listener
                        {
                            SubscriptionId Id;
                            Callback Handler;

                            Listener(SubscriptionId ListenerId, Callback ListenerHandler)
                                : Id(ListenerId), Handler(Types::Move(ListenerHandler))
                            {
                            }
                        };

                        mutable Sync::Mutex StateMutex;
                        Containers::HashMap<Types::TypeKey, Containers::Array<Listener>> Listeners;
                        SubscriptionId NextId = 1;

                    public:
                        EventBus() = default;
                        EventBus(const EventBus&) = delete;
                        EventBus& operator=(const EventBus&) = delete;

                        template <typename EventType, typename Handler>
                        SubscriptionId Subscribe(Handler&& Function)
                        {
                            Callback NewCallback(
                                [Target = Types::Forward<Handler>(Function)](const void* EventData) mutable
                                {
                                    Target(*static_cast<const EventType*>(EventData));
                                }
                            );

                            Sync::LockGuard Lock(StateMutex);

                            if (NextId == 0)
                                throw Assert::OverflowError("EventBus subscription id space is exhausted");

                            const SubscriptionId Id = NextId++;
                            const Types::TypeKey Key = Types::GetTypeKey<EventType>();

                            Containers::Array<Listener>* EventListeners = Listeners.Find(Key);

                            if (EventListeners == nullptr)
                            {
                                Listeners.Insert(Key, Containers::Array<Listener>());
                                EventListeners = Listeners.Find(Key);
                            }

                            EventListeners->Add(Listener(Id, Types::Move(NewCallback)));

                            return Id;
                        }

                        template <typename EventType>
                        void Publish(const EventType& Event) const
                        {
                            Containers::Array<Callback> Callbacks;

                            {
                                Sync::LockGuard Lock(StateMutex);
                                const Containers::Array<Listener>* EventListeners = Listeners.Find(Types::GetTypeKey<EventType>());

                                if (EventListeners == nullptr)
                                    return;

                                Callbacks.Reserve(EventListeners->GetCount());

                                for (const Listener& Item : *EventListeners)
                                    Callbacks.Add(Item.Handler);
                            }

                            for (const Callback& Item : Callbacks)
                                Item(&Event);
                        }

                        template <typename EventType>
                        Types::Size GetListenerCount() const
                        {
                            Sync::LockGuard Lock(StateMutex);
                            const Containers::Array<Listener>* EventListeners = Listeners.Find(Types::GetTypeKey<EventType>());
                            return EventListeners == nullptr ? 0 : EventListeners->GetCount();
                        }

                        Types::Bool Unsubscribe(SubscriptionId Id);
                        void Clear();
                        Types::Size GetSubscriptionCount() const;
                };
            }
        }
    }
}
