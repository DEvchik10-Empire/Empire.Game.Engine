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
                    Listener Removed{0, {}};
                    Bool WasRemoved = false;

                    {
                        std::lock_guard Lock(Mutex);

                        for (auto MapIt = Listeners.begin(); MapIt != Listeners.end(); ++MapIt)
                        {
                            auto& EventListeners = MapIt->second;

                            for (auto ListenerIt = EventListeners.begin(); ListenerIt != EventListeners.end(); ++ListenerIt)
                            {
                                if (ListenerIt->Id != Id)
                                    continue;

                                Removed = std::move(*ListenerIt);
                                EventListeners.erase(ListenerIt);

                                if (EventListeners.empty())
                                    Listeners.erase(MapIt);

                                WasRemoved = true;
                                break;
                            }

                            if (WasRemoved)
                                break;
                        }
                    }

                    return WasRemoved;
                }

                void EventBus::Clear()
                {
                    decltype(Listeners) Removed;

                    {
                        std::lock_guard Lock(Mutex);
                        Removed.swap(Listeners);
                    }
                }

                Size EventBus::GetSubscriptionCount() const
                {
                    std::lock_guard Lock(Mutex);

                    Size Count = 0;

                    for (const auto& [EventType, EventListeners] : Listeners)
                    {
                        static_cast<void>(EventType);
                        Count += EventListeners.size();
                    }

                    return Count;
                }
            }
        }
    }
}
