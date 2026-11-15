#include "Empire.Kernel/Kernel.Events/Events.EventBus.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
        {
            namespace KernelEvents
            {
                Bool EventBus::Unsubscribe(SubscriptionId Id)
                {
                    Callback Removed;
                    Bool WasRemoved = false;

                    {
                        KernelSync::LockGuard Lock(StateMutex);

                        TypeKey EmptiedKey = 0;
                        Bool HasEmptiedKey = false;

                        Listeners.ForEach(
                            [&](const TypeKey& Key, KernelContainers::Array<Listener>& EventListeners)
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
                    KernelContainers::HashMap<TypeKey, KernelContainers::Array<Listener>> Removed;

                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        Removed = Move(Listeners);
                    }
                }

                Size EventBus::GetSubscriptionCount() const
                {
                    KernelSync::LockGuard Lock(StateMutex);

                    Size Total = 0;

                    Listeners.ForEach(
                        [&](const TypeKey&, const KernelContainers::Array<Listener>& EventListeners)
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
