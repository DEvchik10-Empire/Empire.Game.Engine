#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Platform/Platform.ThirdParty/ThirdParty.Xcb/Xcb.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EnginePlatform
            {
                namespace PlatformApp
                {
                    namespace ConnectionXcb
                    {
                        using namespace EngineKernel::KernelTypes;

                        class Connection final
                        {
                            public:
                                [[nodiscard]] static Connection& Instance() noexcept;

                                Connection(const Connection&) = delete;
                                Connection& operator=(const Connection&) = delete;
                                Connection(Connection&&) = delete;
                                Connection& operator=(Connection&&) = delete;

                                [[nodiscard]] xcb_connection_t* Get() const noexcept;
                                [[nodiscard]] Int32 GetErrorCode() const noexcept;

                            private:
                                Connection() noexcept;
                                ~Connection();

                                void Close() noexcept;

                                xcb_connection_t* NativeConnection = nullptr;
                                Int32 ErrorCode = 0;
                        };
                    }
                }
            }
        }
    }
}
