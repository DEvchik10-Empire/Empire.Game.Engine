#include "Empire.Kernel/Kernel.Events/Events.EventBus.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Events
            {
                Bool EventBus::Unsubscribe(SubscriptionId Id)
                {
                    Callback Removed;
                    Bool WasRemoved = false;

                    {
                        Sync::LockGuard Lock(StateMutex);

                        TypeKey EmptiedKey = 0;
                        Bool HasEmptiedKey = false;

                        Listeners.ForEach(
                            [&](const TypeKey& Key, Containers::Array<Listener>& EventListeners)
                            {
                                for (Size Index = 0; Index < EventListeners.GetCount(); Index++)
                                {
                                    if (EventListeners[Index].Id != Id)
                                        continue;

                                    Removed = Move(EventListeners[Index].Handler);
                                    EventListeners.RemoveAt(Index);
                                    WasRemoved = true;

                                    if (EventListeners.IsEmpty())
                                    {
                                        EmptiedKey = Key;
                                        HasEmptiedKey = true;
                                    }

                                    return false;
                                }

                                return true;
                            }
                        );

                        if (HasEmptiedKey)
                            Listeners.Remove(EmptiedKey);
                    }

                    return WasRemoved;
                }

                void EventBus::Clear()
                {
                    Containers::HashMap<TypeKey, Containers::Array<Listener>> Removed;

                    {
                        Sync::LockGuard Lock(StateMutex);
                        Removed = Move(Listeners);
                    }
                }

                Size EventBus::GetSubscriptionCount() const
                {
                    Sync::LockGuard Lock(StateMutex);

                    Size Total = 0;

                    Listeners.ForEach(
                        [&](const TypeKey&, const Containers::Array<Listener>& EventListeners)
                        {
                            Total += EventListeners.GetCount();
                            return true;
                        }
                    );

                    return Total;
                }
            }
        }
    }
}
