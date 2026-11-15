#include "Interface.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            void InterfaceBinder::Bind(
                void* data,
                Types::u32 interfaceId
            )
            {
                std::unique_ptr<Interface> interface =
                    CreateInterface(data);

                interface->binder = this;
                interface->interfaceId = interfaceId;

                interfaces.push_back(
                    std::move(interface)
                );
            }


            Types::b8 InterfaceBinder::Unbind(
                Types::u32 interfaceId
            )
            {
                for (
                    std::vector<
                        std::unique_ptr<Interface>
                    >::iterator iterator =
                        interfaces.begin();

                    iterator != interfaces.end();

                    ++iterator
                )
                {
                    Interface* interface =
                        iterator->get();

                    if (
                        interface->interfaceId ==
                        interfaceId
                    )
                    {
                        interfaces.erase(iterator);
                        return true;
                    }
                }

                return false;
            }


            Interface* InterfaceBinder::GetInterface(
                Types::u32 interfaceId
            ) const
            {
                for (
                    const std::unique_ptr<Interface>& interface :
                    interfaces
                )
                {
                    if (
                        interface->interfaceId ==
                        interfaceId
                    )
                    {
                        return interface.get();
                    }
                }

                return nullptr;
            }


            Interface* InterfaceBinder::GetFirstInterface() const
            {
                if (interfaces.empty())
                {
                    return nullptr;
                }

                return interfaces.front().get();
            }
        }
    }
}