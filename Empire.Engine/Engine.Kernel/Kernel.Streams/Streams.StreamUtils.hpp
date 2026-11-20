#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.ObjectToIdMap.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Result.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Streams/Streams.StreamIn.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Streams/Streams.StreamOut.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelStreams
                {
                    namespace StreamUtils
                    {
                        inline constexpr KernelTypes::UInt32 NullObjectId = KernelTypes::MaxValue<KernelTypes::UInt32>();

                        // Сохраняет ссылку и сериализует объект только при первой встрече (Jolt 5.7.0, Core/StreamUtils.h).
                        template <typename T>
                        void SaveObjectReference(
                            StreamOut& Output,
                            const T* Object,
                            KernelContainers::ObjectToIDMap<T>* ObjectIds
                        )
                        {
                            if (Object == nullptr || ObjectIds == nullptr)
                            {
                                Output.Write(NullObjectId);
                                return;
                            }

                            const KernelTypes::UInt32* ExistingId = ObjectIds->Find(Object);
                            if (ExistingId != nullptr)
                            {
                                Output.Write(*ExistingId);
                                return;
                            }

                            const KernelTypes::UInt32 Id = static_cast<KernelTypes::UInt32>(ObjectIds->GetCount());
                            ObjectIds->Set(Object, Id);
                            Output.Write(Id);
                            if (!Output.IsFailed())
                                Object->SaveBinaryState(Output);
                        }

                        // Single-object binary state uses an explicit registered type hash before its payload.
                        template <typename T>
                        void SaveObject(StreamOut& Output, const T& Object, KernelTypes::UInt32 TypeHash)
                        {
                            Output.Write(TypeHash);
                            if (!Output.IsFailed())
                                Object.SaveBinaryState(Output);
                        }

                        template <typename T>
                        using RestoreObjectFunction = KernelContainers::Result<KernelMemory::Ref<T>> (*)(StreamIn& Input);

                        // Type-hash dispatch is explicit: the engine supplies a callback instead of relying on a global Factory singleton.
                        template <typename T>
                        using RestoreObjectByTypeFunction = KernelContainers::Result<KernelMemory::Ref<T>> (*)(KernelTypes::UInt32 TypeHash, StreamIn& Input);

                        template <typename T>
                        KernelContainers::Result<KernelMemory::Ref<T>> RestoreObject(
                            StreamIn& Input,
                            RestoreObjectByTypeFunction<T> Restore
                        )
                        {
                            KernelContainers::Result<KernelMemory::Ref<T>> Result;
                            KernelTypes::UInt32 TypeHash = 0;
                            Input.Read(TypeHash);
                            if (Input.IsEOF() || Input.IsFailed())
                            {
                                Result.SetError("Failed to read object type hash");
                                return Result;
                            }

                            if (Restore == nullptr)
                            {
                                Result.SetError("No object restore callback registered");
                                return Result;
                            }

                            Result = Restore(TypeHash, Input);
                            if (Input.IsEOF() || Input.IsFailed())
                            {
                                if (!Result.HasError())
                                    Result.SetError("Failed to restore object state");
                            }
                            return Result;
                        }

                        // Восстанавливает ссылку; создание нового объекта делегируется зарегистрированному typed callback.
                        template <typename T>
                        KernelContainers::Result<KernelMemory::Ref<T>> RestoreObjectReference(
                            StreamIn& Input,
                            KernelContainers::Array<KernelMemory::Ref<T>>& ObjectsById,
                            RestoreObjectFunction<T> Restore
                        )
                        {
                            KernelContainers::Result<KernelMemory::Ref<T>> Result;
                            KernelTypes::UInt32 Id = NullObjectId;
                            Input.Read(Id);
                            if (Input.IsEOF() || Input.IsFailed())
                            {
                                Result.SetError("Failed to read object id");
                                return Result;
                            }

                            if (Id == NullObjectId)
                            {
                                Result.Set(KernelMemory::Ref<T>());
                                return Result;
                            }

                            if (Id < ObjectsById.GetCount())
                            {
                                Result.Set(ObjectsById[Id]);
                                return Result;
                            }

                            if (Id != ObjectsById.GetCount())
                            {
                                Result.SetError("Object id is out of sequence");
                                return Result;
                            }

                            if (Restore == nullptr)
                            {
                                Result.SetError("No object restore callback registered");
                                return Result;
                            }

                            KernelContainers::Result<KernelMemory::Ref<T>> Restored = Restore(Input);
                            if (Restored.HasError() || !Restored.IsValid() || Input.IsEOF() || Input.IsFailed())
                            {
                                if (Restored.HasError())
                                    Result.SetError(Restored.GetError().GetView());
                                else
                                    Result.SetError("Failed to restore object");
                                return Result;
                            }

                            ObjectsById.Add(Restored.Get());
                            Result.Set(KernelTypes::Move(Restored.Get()));
                            return Result;
                        }

                        template <typename T, typename ArrayType>
                        void SaveObjectArray(StreamOut& Output, const ArrayType& Objects, KernelContainers::ObjectToIDMap<T>* ObjectIds)
                        {
                            KernelTypes::UInt32 Count = 0;
                            if constexpr (requires { Objects.GetCount(); })
                                Count = static_cast<KernelTypes::UInt32>(Objects.GetCount());
                            else if constexpr (requires { Objects.Size(); })
                                Count = static_cast<KernelTypes::UInt32>(Objects.Size());
                            else
                                Count = static_cast<KernelTypes::UInt32>(Objects.size());
                            Output.Write(Count);
                            for (const T* Object : Objects)
                                SaveObjectReference(Output, Object, ObjectIds);
                        }

                        template <typename T, typename ArrayType = KernelContainers::Array<KernelMemory::Ref<T>>>
                        KernelContainers::Result<ArrayType> RestoreObjectArray(
                            StreamIn& Input,
                            KernelContainers::Array<KernelMemory::Ref<T>>& ObjectsById,
                            RestoreObjectFunction<T> Restore
                        )
                        {
                            KernelContainers::Result<ArrayType> Result;
                            KernelTypes::UInt32 Count = 0;
                            Input.Read(Count);
                            if (Input.IsEOF() || Input.IsFailed())
                            {
                                Result.SetError("Failed to read object array length");
                                return Result;
                            }

                            ArrayType Objects;
                            Objects.Reserve(Count);
                            for (KernelTypes::UInt32 Index = 0; Index < Count; ++Index)
                            {
                                auto Restored = RestoreObjectReference(Input, ObjectsById, Restore);
                                if (Restored.HasError())
                                {
                                    Result.SetError(Restored.GetError().GetView());
                                    return Result;
                                }
                                if constexpr (requires(ArrayType& Target, KernelMemory::Ref<T>&& Value) { Target.Add(KernelTypes::Move(Value)); })
                                    Objects.Add(KernelTypes::Move(Restored.Get()));
                                else
                                    Objects.PushBack(KernelTypes::Move(Restored.Get()));
                            }

                            Result.Set(KernelTypes::Move(Objects));
                            return Result;
                        }
                    }
                }
            }
        }
    }
}
