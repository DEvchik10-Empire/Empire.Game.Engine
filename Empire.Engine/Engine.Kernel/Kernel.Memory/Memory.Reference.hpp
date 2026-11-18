#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelMemory
                {
                    template <typename T>
                    class Ref;

                    template <typename T>
                    class RefConst;

                    // Интрузивный счётчик ссылок (перенесён из Jolt, Core/Reference.h). Отличается от SharedPtr тем, что счётчик лежит
                    // внутри самого объекта: на объект можно сослаться из сырого указателя, не теряя счёт, и не нужен отдельный блок.
                    //
                    // Наследуйся от RefTarget<Класс> и держи объект через Ref<Класс> или RefConst<Класс>. Объект создаётся с нулём
                    // ссылок и удаляется (через delete), когда последний Ref отпущен.
                    // Два объекта не должны ссылаться друг на друга по кругу: счётчики никогда не дойдут до нуля.
                    template <typename T>
                    class RefTarget
                    {
                        public:
                            RefTarget() = default;

                            // Счётчик при копировании не копируется
                            RefTarget(const RefTarget&)
                            {
                            }

                            ~RefTarget()
                            {
                                // На объект уже никто не должен ссылаться
                                if constexpr (KernelCompiler::IsDebugBuild)
                                {
                                    KernelTypes::UInt32 Value = RefCount.Load(KernelSync::MemoryOrder::Relaxed);
                                    KernelAssert::DebugCheck(Value == 0 || Value == Embedded, "RefTarget: уничтожение объекта, на который ещё ссылаются");
                                }
                            }

                            RefTarget& operator=(const RefTarget&)
                            {
                                return *this;
                            }

                            // Пометить объект встроенным: его можно хранить как поле или на стеке. Release такой объект не удаляет,
                            // его уничтожает тот, кто создал, и в этот момент проверяется, что ссылок не осталось.
                            void SetEmbedded() const
                            {
                                [[maybe_unused]] KernelTypes::UInt32 Old = RefCount.FetchAdd(Embedded, KernelSync::MemoryOrder::Relaxed);
                                KernelAssert::DebugCheck(Old < Embedded, "RefTarget::SetEmbedded: объект уже помечен");
                            }

                            // Текущее число ссылок
                            KernelTypes::UInt32 GetRefCount() const
                            {
                                return RefCount.Load(KernelSync::MemoryOrder::Relaxed);
                            }

                            // Добавить ссылку. Достаточно ослабленного порядка памяти.
                            void AddRef() const
                            {
                                RefCount.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                            }

                            // Добавить сразу Count ссылок
                            void AddRef(KernelTypes::UInt32 Count) const
                            {
                                RefCount.FetchAdd(Count, KernelSync::MemoryOrder::Relaxed);
                            }

                            // Отпустить ссылку, на последней объект удаляется
                            void Release() const
                            {
                                KernelTypes::UInt32 OldValue;

                                if constexpr (!KernelCompiler::IsThreadSanitizer)
                                {
                                    // Release-порядок при уменьшении, чтобы поток, удаляющий объект, видел всё, что сделали другие потоки до своего Release...
                                    OldValue = RefCount.FetchSub(1, KernelSync::MemoryOrder::Release);
                                    if (OldValue == 1)
                                    {
                                        // ...а Acquire-барьер перед удалением это гарантирует
                                        KernelCompiler::AtomicThreadFence(KernelSync::MemoryOrder::Acquire);
                                        delete static_cast<const T*>(this);
                                    }
                                }
                                else
                                {
                                    // ThreadSanitizer не понимает отдельный барьер, поэтому ставим Acquire-Release на каждую операцию
                                    OldValue = RefCount.FetchSub(1, KernelSync::MemoryOrder::AcquireRelease);
                                    if (OldValue == 1)
                                    {
                                        delete static_cast<const T*>(this);
                                    }
                                }

                                KernelAssert::DebugCheck(OldValue != 0 && OldValue != Embedded, "RefTarget::Release: слишком много вызовов Release");
                            }

                        protected:
                            // Большое число, которое прибавляется к счётчику у встроенных объектов
                            static constexpr KernelTypes::UInt32 Embedded = 0x0ebedded;

                            mutable KernelSync::Atomic<KernelTypes::UInt32> RefCount{0};
                    };

                    // Чисто виртуальный вариант RefTarget
                    class RefTargetVirtual
                    {
                        public:
                            virtual ~RefTargetVirtual() = default;

                            virtual void AddRef() = 0;
                            virtual void Release() = 0;
                    };

                    // Умный указатель на объект со счётчиком: при присваивании увеличивает счётчик, при уничтожении или
                    // замене уменьшает, на нуле объект удаляется.
                    template <typename T>
                    class Ref
                    {
                        public:
                            Ref() = default;

                            Ref(T* Pointer)
                                : Ptr(Pointer)
                            {
                                AddRef();
                            }

                            Ref(const Ref& Other)
                                : Ptr(Other.Ptr)
                            {
                                AddRef();
                            }

                            Ref(Ref&& Other) noexcept
                                : Ptr(Other.Ptr)
                            {
                                Other.Ptr = nullptr;
                            }

                            ~Ref()
                            {
                                Release();
                            }

                            Ref& operator=(T* Pointer)
                            {
                                if (Ptr != Pointer)
                                {
                                    Release();
                                    Ptr = Pointer;
                                    AddRef();
                                }

                                return *this;
                            }

                            Ref& operator=(const Ref& Other)
                            {
                                if (Ptr != Other.Ptr)
                                {
                                    Release();
                                    Ptr = Other.Ptr;
                                    AddRef();
                                }

                                return *this;
                            }

                            Ref& operator=(Ref&& Other) noexcept
                            {
                                if (Ptr != Other.Ptr)
                                {
                                    Release();
                                    Ptr = Other.Ptr;
                                    Other.Ptr = nullptr;
                                }

                                return *this;
                            }

                            operator T*() const
                            {
                                return Ptr;
                            }

                            T* operator->() const
                            {
                                return Ptr;
                            }

                            T& operator*() const
                            {
                                return *Ptr;
                            }

                            KernelTypes::Bool operator==(const T* Other) const
                            {
                                return Ptr == Other;
                            }

                            KernelTypes::Bool operator==(const Ref& Other) const
                            {
                                return Ptr == Other.Ptr;
                            }

                            KernelTypes::Bool operator!=(const T* Other) const
                            {
                                return Ptr != Other;
                            }

                            KernelTypes::Bool operator!=(const Ref& Other) const
                            {
                                return Ptr != Other.Ptr;
                            }

                            T* GetPtr() const
                            {
                                return Ptr;
                            }

                        private:
                            template <typename Other>
                            friend class RefConst;

                            // Чтобы отпустить объект, присвой nullptr, а эти функции напрямую не вызывай
                            void AddRef()
                            {
                                if (Ptr != nullptr)
                                {
                                    Ptr->AddRef();
                                }
                            }

                            void Release()
                            {
                                if (Ptr != nullptr)
                                {
                                    Ptr->Release();
                                }
                            }

                            T* Ptr = nullptr;
                    };

                    // То же самое для указателя на константный объект
                    template <typename T>
                    class RefConst
                    {
                        public:
                            RefConst() = default;

                            RefConst(const T* Pointer)
                                : Ptr(Pointer)
                            {
                                AddRef();
                            }

                            RefConst(const RefConst& Other)
                                : Ptr(Other.Ptr)
                            {
                                AddRef();
                            }

                            RefConst(RefConst&& Other) noexcept
                                : Ptr(Other.Ptr)
                            {
                                Other.Ptr = nullptr;
                            }

                            RefConst(const Ref<T>& Other)
                                : Ptr(Other.Ptr)
                            {
                                AddRef();
                            }

                            RefConst(Ref<T>&& Other) noexcept
                                : Ptr(Other.Ptr)
                            {
                                Other.Ptr = nullptr;
                            }

                            ~RefConst()
                            {
                                Release();
                            }

                            RefConst& operator=(const T* Pointer)
                            {
                                if (Ptr != Pointer)
                                {
                                    Release();
                                    Ptr = Pointer;
                                    AddRef();
                                }

                                return *this;
                            }

                            RefConst& operator=(const RefConst& Other)
                            {
                                if (Ptr != Other.Ptr)
                                {
                                    Release();
                                    Ptr = Other.Ptr;
                                    AddRef();
                                }

                                return *this;
                            }

                            RefConst& operator=(RefConst&& Other) noexcept
                            {
                                if (Ptr != Other.Ptr)
                                {
                                    Release();
                                    Ptr = Other.Ptr;
                                    Other.Ptr = nullptr;
                                }

                                return *this;
                            }

                            RefConst& operator=(const Ref<T>& Other)
                            {
                                if (Ptr != Other.Ptr)
                                {
                                    Release();
                                    Ptr = Other.Ptr;
                                    AddRef();
                                }

                                return *this;
                            }

                            RefConst& operator=(Ref<T>&& Other) noexcept
                            {
                                if (Ptr != Other.Ptr)
                                {
                                    Release();
                                    Ptr = Other.Ptr;
                                    Other.Ptr = nullptr;
                                }

                                return *this;
                            }

                            operator const T*() const
                            {
                                return Ptr;
                            }

                            const T* operator->() const
                            {
                                return Ptr;
                            }

                            const T& operator*() const
                            {
                                return *Ptr;
                            }

                            KernelTypes::Bool operator==(const T* Other) const
                            {
                                return Ptr == Other;
                            }

                            KernelTypes::Bool operator==(const RefConst& Other) const
                            {
                                return Ptr == Other.Ptr;
                            }

                            KernelTypes::Bool operator==(const Ref<T>& Other) const
                            {
                                return Ptr == Other.Ptr;
                            }

                            KernelTypes::Bool operator!=(const T* Other) const
                            {
                                return Ptr != Other;
                            }

                            KernelTypes::Bool operator!=(const RefConst& Other) const
                            {
                                return Ptr != Other.Ptr;
                            }

                            KernelTypes::Bool operator!=(const Ref<T>& Other) const
                            {
                                return Ptr != Other.Ptr;
                            }

                            const T* GetPtr() const
                            {
                                return Ptr;
                            }

                        private:
                            void AddRef()
                            {
                                if (Ptr != nullptr)
                                {
                                    Ptr->AddRef();
                                }
                            }

                            void Release()
                            {
                                if (Ptr != nullptr)
                                {
                                    Ptr->Release();
                                }
                            }

                            const T* Ptr = nullptr;
                    };
                }
            }
        }
    }
}
