#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelSync
                {
                    // Набор мьютексов, выбранных хешированием индекса объекта (перенесён из Jolt 5.7.0, Core/MutexArray.h).
                    // Для выделения собственного хранилища использует Kernel.Memory; потребителю нужен этот модуль при линковке.
                    template <typename MutexType = Mutex>
                    class MutexArray : public KernelTypes::NonCopyable
                    {
                        private:
                            struct alignas(KernelCompiler::CacheLineSize) MutexStorage
                            {
                                MutexType Value;
                            };

                            MutexStorage* Storage = nullptr;
                            KernelTypes::UInt32 Count = 0;

                            static KernelTypes::UInt32 HashObjectIndex(KernelTypes::UInt32 ObjectIndex) noexcept
                            {
                                KernelTypes::UInt64 HashValue = 0xcbf29ce484222325ULL;
                                const KernelTypes::UInt8* Bytes = reinterpret_cast<const KernelTypes::UInt8*>(&ObjectIndex);
                                for (KernelTypes::Size Index = 0; Index < sizeof(ObjectIndex); ++Index)
                                {
                                    HashValue ^= Bytes[Index];
                                    HashValue *= 0x100000001b3ULL;
                                }
                                return static_cast<KernelTypes::UInt32>(HashValue);
                            }

                        public:
                            MutexArray() = default;

                            explicit MutexArray(KernelTypes::UInt32 MutexCount)
                            {
                                Init(MutexCount);
                            }

                            ~MutexArray()
                            {
                                for (KernelTypes::UInt32 Index = Count; Index > 0; --Index)
                                    KernelTypes::DestroyAt(&Storage[Index - 1]);
                                KernelMemory::Deallocate(Storage);
                            }

                            void Init(KernelTypes::UInt32 MutexCount)
                            {
                                KernelAssert::DebugCheck(Storage == nullptr, "MutexArray::Init: массив уже создан");
                                KernelAssert::Check(MutexCount > 0 && KernelTypes::IsPowerOfTwo(MutexCount), "MutexArray::Init: количество должно быть степенью двойки");
                                KernelAssert::Check(MutexCount <= KernelTypes::MaxValue<KernelTypes::Size>() / sizeof(MutexStorage), "MutexArray::Init: слишком большой массив");

                                MutexStorage* NewStorage = static_cast<MutexStorage*>(KernelMemory::Allocate(
                                    static_cast<KernelTypes::Size>(MutexCount) * sizeof(MutexStorage), alignof(MutexStorage)
                                ));
                                for (KernelTypes::UInt32 Index = 0; Index < MutexCount; ++Index)
                                    KernelTypes::ConstructAt(NewStorage + Index);

                                Storage = NewStorage;
                                Count = MutexCount;
                            }

                            [[nodiscard]] KernelTypes::UInt32 GetNumMutexes() const noexcept { return Count; }
                            [[nodiscard]] KernelTypes::UInt32 GetMutexCount() const noexcept { return GetNumMutexes(); }
                            [[nodiscard]] KernelTypes::UInt32 GetMutexIndex(KernelTypes::UInt32 ObjectIndex) const noexcept
                            {
                                KernelAssert::DebugCheck(Count > 0, "MutexArray::GetMutexIndex: массив не инициализирован");
                                return HashObjectIndex(ObjectIndex) & (Count - 1);
                            }
                            MutexType& GetMutexByObjectIndex(KernelTypes::UInt32 ObjectIndex) noexcept { return Storage[GetMutexIndex(ObjectIndex)].Value; }
                            MutexType& GetMutexByIndex(KernelTypes::UInt32 Index) noexcept
                            {
                                KernelAssert::DebugCheck(Index < Count, "MutexArray::GetMutexByIndex: индекс вне диапазона");
                                return Storage[Index].Value;
                            }

                            void LockAll()
                            {
                                for (KernelTypes::UInt32 Index = 0; Index < Count; ++Index)
                                    Storage[Index].Value.Lock();
                            }

                            void UnlockAll() noexcept
                            {
                                for (KernelTypes::UInt32 Index = 0; Index < Count; ++Index)
                                    Storage[Index].Value.Unlock();
                            }
                    };
                }
            }
        }
    }
}
