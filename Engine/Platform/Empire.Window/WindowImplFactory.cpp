#include "WindowImplFactory.hpp"

#include "Wayland/Impl.hpp"
#include "legasy/Impl.hpp"

namespace Engine
{
    namespace Platform
    {
        std::unique_ptr<IWindowImpl>
        CreateWindowImpl(
            const WindowConfig& config
        )
        {
            switch (config.backend)
            {
                case WindowBackend::Wayland:
                {
                    auto impl =
                        std::make_unique<Wayland::Impl>(
                            config
                        );

                    if (!impl->IsOpen())
                    {
                        return nullptr;
                    }

                    return impl;
                }

                case WindowBackend::legasy:
                {
                    auto impl =
                        std::make_unique<legasy::Impl>(
                            config
                        );

                    if (!impl->IsOpen())
                    {
                        return nullptr;
                    }

                    return impl;
                }

                case WindowBackend::Auto:
                {
                    {
                        auto impl =
                            std::make_unique<Wayland::Impl>(
                                config
                            );

                        if (impl->IsOpen())
                        {
                            return impl;
                        }
                    }

                    {
                        auto impl =
                            std::make_unique<legasy::Impl>(
                                config
                            );

                        if (impl->IsOpen())
                        {
                            return impl;
                        }
                    }

                    return nullptr;
                }
            }

            return nullptr;
        }
    }
}