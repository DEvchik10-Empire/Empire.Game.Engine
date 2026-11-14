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
        namespace EmpireKernel
        {
            namespace KernelEvents
            {
                class EventBus
                {
                    public:
                        using SubscriptionId = KernelTypes::UInt64;

                    private:
                        using Callback = KernelContainers::Function<void(const void*)>;

                        struct Listener
                        {
                            SubscriptionId Id;
                            Callback Handler;

                            Listener(SubscriptionId ListenerId, Callback ListenerHandler)
                                : Id(ListenerId), Handler(KernelTypes::Move(ListenerHandler))
                            {
                            }
                        };

                        mutable KernelSync::Mutex StateMutex;
                        KernelContainers::HashMap<KernelTypes::TypeKey, KernelContainers::Array<Listener>> Listeners;
                        SubscriptionId NextId = 1;

                    public:
                        EventBus() = default;
                        EventBus(const EventBus&) = delete;
                        EventBus& operator=(const EventBus&) = delete;

                        template <typename EventType, typename Handler>
                        SubscriptionId Subscribe(Handler&& Function)
                        {
                            Callback NewCallback(
                                [Target = KernelTypes::Forward<Handler>(Function)](const void* EventData) mutable
                                {
                                    Target(*static_cast<const EventType*>(EventData));
                                }
                            );

                            KernelSync::LockGuard Lock(StateMutex);

                            if (NextId == 0)
                                throw KernelAssert::OverflowError("EventBus subscription id space is exhausted");

                            const SubscriptionId Id = NextId++;
                            const KernelTypes::TypeKey Key = KernelTypes::GetTypeKey<EventType>();

                            KernelContainers::Array<Listener>* EventListeners = Listeners.Find(Key);

                            if (EventListeners == nullptr)
                            {
                                Listeners.Insert(Key, KernelContainers::Array<Listener>());
                                EventListeners = Listeners.Find(Key);
                            }

                            EventListeners->Add(Listener(Id, KernelTypes::Move(NewCallback)));

                            return Id;
                        }

                        template <typename EventType>
                        void Publish(const EventType& Event) const
                        {
                            KernelContainers::Array<Callback> Callbacks;

                            {
                                KernelSync::LockGuard Lock(StateMutex);
                                const KernelContainers::Array<Listener>* EventListeners = Listeners.Find(KernelTypes::GetTypeKey<EventType>());

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
                        KernelTypes::Size GetListenerCount() const
                        {
                            KernelSync::LockGuard Lock(StateMutex);
                            const KernelContainers::Array<Listener>* EventListeners = Listeners.Find(KernelTypes::GetTypeKey<EventType>());
                            return EventListeners == nullptr ? 0 : EventListeners->GetCount();
                        }

                        KernelTypes::Bool Unsubscribe(SubscriptionId Id);
                        void Clear();
                        KernelTypes::Size GetSubscriptionCount() const;
                };
            }
        }
    }
}
