#include "WindowImplFactory.hpp"

#include "Wayland/Impl.hpp"
#include "legasy/Impl.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace
        {
            // Пробуем создать бэкенд. Любая неудача (нет композитора, нет DISPLAY,
            // исключение из конструктора) = nullptr, чтобы Auto мог перейти
            // к следующему бэкенду, а не падать.
            template <typename T>
            std::unique_ptr<IWindowImpl>
            TryCreate(
                const WindowConfig& config
            )
            {
                try
                {
                    auto impl =
                        std::make_unique<T>(
                            config
                        );

                    if (impl->IsOpen())
                    {
                        return impl;
                    }
                }
                catch (...)
                {
                }

                return nullptr;
            }
        }

        std::unique_ptr<IWindowImpl>
        CreateWindowImpl(
            const WindowConfig& config
        )
        {
            switch (config.backend)
            {
                case WindowBackend::Wayland:
                {
                    return TryCreate<Wayland::Impl>(
                        config
                    );
                }

                case WindowBackend::legasy:
                {
                    return TryCreate<legasy::Impl>(
                        config
                    );
                }

                case WindowBackend::Auto:
                {
                    if (auto impl = TryCreate<Wayland::Impl>(config))
                    {
                        return impl;
                    }

                    return TryCreate<legasy::Impl>(
                        config
                    );
                }
            }

            return nullptr;
        }
    }
}
