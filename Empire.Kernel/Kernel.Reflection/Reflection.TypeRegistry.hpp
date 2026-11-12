#pragma once

#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Types/Types.hpp"

#include <mutex>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Reflection
            {
                using TypeId = Types::UInt64;

                class TypeInfo
                {
                    private:
                        TypeId IdValue;
                        Containers::String NameValue;
                        Types::Size SizeValue;
                        Types::Size AlignmentValue;
                        std::type_index NativeTypeValue;

                    public:
                        TypeInfo(
                            TypeId Id,
                            const Types::Char* Name,
                            Types::Size ByteSize,
                            Types::Size Alignment,
                            std::type_index NativeType
                        );

                        TypeId GetId() const noexcept;
                        Containers::StringView GetName() const noexcept;
                        Types::Size GetSize() const noexcept;
                        Types::Size GetAlignment() const noexcept;
                        const std::type_index& GetNativeType() const noexcept;
                };

                class TypeRegistry
                {
                    private:
                        mutable std::mutex Mutex;
                        TypeId NextId = 1;
                        std::unordered_map<TypeId, TypeInfo> RegisteredTypes;
                        std::unordered_map<std::type_index, TypeId> NativeTypes;

                    public:
                        TypeRegistry() = default;
                        TypeRegistry(const TypeRegistry&) = delete;
                        TypeRegistry& operator=(const TypeRegistry&) = delete;

                        template <typename T>
                        TypeId RegisterType(const Types::Char* Name)
                        {
                            return Register(
                                std::type_index(typeid(T)),
                                Name,
                                sizeof(T),
                                alignof(T)
                            );
                        }

                        TypeId Register(
                            std::type_index NativeType,
                            const Types::Char* Name,
                            Types::Size ByteSize,
                            Types::Size Alignment
                        );

                        const TypeInfo* Find(TypeId Id) const;
                        const TypeInfo* FindByName(Containers::StringView Name) const;

                        template <typename T>
                        const TypeInfo* Find() const
                        {
                            std::lock_guard Lock(Mutex);
                            const auto It = NativeTypes.find(std::type_index(typeid(T)));

                            if (It == NativeTypes.end())
                                return nullptr;

                            const auto TypeIt = RegisteredTypes.find(It->second);
                            return TypeIt == RegisteredTypes.end() ? nullptr : &TypeIt->second;
                        }

                        Types::Size GetTypeCount() const;
                };
            }
        }
    }
}
