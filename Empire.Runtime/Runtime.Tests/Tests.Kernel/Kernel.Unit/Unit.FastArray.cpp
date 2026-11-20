// Поведенческий тест FastArray. Перенесён из ArrayTest Jolt (UnitTests/Core/ArrayTest.cpp): те же случаи, включая подсчёт
// конструкторов, копий, перемещений и деструкторов у нетривиального типа. Изменения при переносе:
//  - списки инициализации заменены на FastArray<T>::Of(...) (в Kernel нет std::initializer_list);
//  - методы переименованы под стиль движка (push_back -> PushBack и так далее).
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.FastArray.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"
#include <cstdio>

namespace KernelTypes = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;
using DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers::FastArray;

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

namespace
{
	// A test class that is non-trivially copyable to test if the Array class correctly constructs/destructs/copies and moves elements.
	class NonTriv
	{
	public:
							NonTriv() : mValue(0)										{ ++sNumConstructors; }
		explicit			NonTriv(int inValue) : mValue(inValue)						{ ++sNumConstructors; }
							NonTriv(const NonTriv &inValue) : mValue(inValue.mValue)	{ ++sNumCopyConstructors; }
							NonTriv(NonTriv &&inValue) : mValue(inValue.mValue)			{ inValue.mValue = 0; ++sNumMoveConstructors; }
							~NonTriv()													{ ++sNumDestructors; }

		NonTriv &			operator = (const NonTriv &inRHS)							{ mValue = inRHS.mValue; return *this; }

		bool				operator == (const NonTriv &inRHS) const					{ return mValue == inRHS.mValue; }
		bool				operator != (const NonTriv &inRHS) const					{ return mValue != inRHS.mValue; }

		int					Value() const												{ return mValue; }

		static void			sReset()													{ sNumConstructors = 0; sNumCopyConstructors = 0; sNumMoveConstructors = 0; sNumDestructors = 0; }

		static inline int	sNumConstructors = 0;
		static inline int	sNumCopyConstructors = 0;
		static inline int	sNumMoveConstructors = 0;
		static inline int	sNumDestructors = 0;

		int					mValue;
	};

	void TestConstructLength()
	{
		FastArray<int> arr(55);
		CHECK(arr.Size() == 55);
	}

	void TestConstructLengthNonTriv()
	{
		NonTriv::sReset();
		FastArray<NonTriv> arr(55);
		CHECK(arr.Size() == 55);
		CHECK(NonTriv::sNumConstructors == 55);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
	}

	void TestConstructValue()
	{
		FastArray<int> arr(5, 55);
		CHECK(arr.Size() == 5);
		for (int i = 0; i < 5; ++i)
			CHECK(arr[i] == 55);
	}

	void TestConstructValueNonTriv()
	{
		NonTriv v(55);
		NonTriv::sReset();
		FastArray<NonTriv> arr(5, v);
		CHECK(arr.Size() == 5);
		for (int i = 0; i < 5; ++i)
			CHECK(arr[i].Value() == 55);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 5);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
	}

	void TestConstructIterator()
	{
		int values[] = { 1, 2, 3 };

		FastArray<int> arr(values, values + 3);
		CHECK(arr.Size() == 3);
		CHECK(arr[0] == 1);
		CHECK(arr[1] == 2);
		CHECK(arr[2] == 3);
	}

	void TestConstructIteratorNonTriv()
	{
		NonTriv values[] = { NonTriv(1), NonTriv(2), NonTriv(3) };

		NonTriv::sReset();
		FastArray<NonTriv> arr(values, values + 3);
		CHECK(arr.Size() == 3);
		CHECK(arr[0].Value() == 1);
		CHECK(arr[1].Value() == 2);
		CHECK(arr[2].Value() == 3);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 3);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
	}

	void TestConstructInitializerList()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		CHECK(arr.Size() == 3);
		CHECK(arr[0] == 1);
		CHECK(arr[1] == 2);
		CHECK(arr[2] == 3);
	}

	void TestConstructInitializerListNonTriv()
	{
		NonTriv::sReset();
		FastArray<NonTriv> arr = FastArray<NonTriv>::Of(NonTriv(1), NonTriv(2), NonTriv(3));
		CHECK(arr.Size() == 3);
		CHECK(arr[0].Value() == 1);
		CHECK(arr[1].Value() == 2);
		CHECK(arr[2].Value() == 3);
		CHECK(NonTriv::sNumConstructors == 3); // For the initializer list
		CHECK(NonTriv::sNumCopyConstructors == 0); // Of() перемещает временные значения, а не копирует (в Jolt initializer_list копировался)
		CHECK(NonTriv::sNumMoveConstructors == 3);
		CHECK(NonTriv::sNumDestructors == 3); // For the initializer list
	}

	void TestConstructFromArray()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		FastArray<int> arr2(arr);
		CHECK(arr2.Size() == 3);
		CHECK(arr2[0] == 1);
		CHECK(arr2[1] == 2);
		CHECK(arr2[2] == 3);
	}

	void TestConstructFromArrayNonTriv()
	{
		FastArray<NonTriv> arr = FastArray<NonTriv>::Of(NonTriv(1), NonTriv(2), NonTriv(3));
		NonTriv::sReset();
		FastArray<NonTriv> arr2(arr);
		CHECK(arr2.Size() == 3);
		CHECK(arr2[0].Value() == 1);
		CHECK(arr2[1].Value() == 2);
		CHECK(arr2[2].Value() == 3);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 3);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
	}

	void TestMoveFromArray()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		FastArray<int> arr2(KernelTypes::Move(arr));
		CHECK(arr2.Size() == 3);
		CHECK(arr2[0] == 1);
		CHECK(arr2[1] == 2);
		CHECK(arr2[2] == 3);
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 0);
	}

	void TestMoveFromArrayNonTriv()
	{
		FastArray<NonTriv> arr = FastArray<NonTriv>::Of(NonTriv(1), NonTriv(2), NonTriv(3));
		NonTriv::sReset();
		FastArray<NonTriv> arr2(KernelTypes::Move(arr)); // This just updates the mElements pointer so should not call any constructors/destructors etc.
		CHECK(arr2.Size() == 3);
		CHECK(arr2[0].Value() == 1);
		CHECK(arr2[1].Value() == 2);
		CHECK(arr2[2].Value() == 3);
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 0);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
	}

	void TestClear()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		CHECK(arr.Size() == 3);
		arr.Clear();
		CHECK(arr.Size() == 0);
	}

	void TestClearNonTriv()
	{
		NonTriv::sReset();
		FastArray<NonTriv> arr = FastArray<NonTriv>::Of(NonTriv(1), NonTriv(2), NonTriv(3));
		CHECK(arr.Size() == 3);
		CHECK(NonTriv::sNumConstructors == 3); // For initializer list
		CHECK(NonTriv::sNumCopyConstructors == 0); // Of() перемещает временные значения, а не копирует (в Jolt initializer_list копировался)
		CHECK(NonTriv::sNumMoveConstructors == 3);
		CHECK(NonTriv::sNumDestructors == 3); // For initializer list
		NonTriv::sReset();
		arr.Clear();
		CHECK(arr.Size() == 0);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 3);
	}

	void TestPushBack()
	{
		FastArray<int> arr;
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 0);

		arr.PushBack(1);
		CHECK(arr.Size() == 1);
		CHECK(arr[0] == 1);

		arr.PushBack(2);
		CHECK(arr.Size() == 2);
		CHECK(arr[0] == 1);
		CHECK(arr[1] == 2);

		arr.PopBack();
		CHECK(arr.Size() == 1);

		arr.PopBack();
		CHECK(arr.Size() == 0);
		CHECK(arr.Empty());
	}

	void TestPushBackNonTriv()
	{
		NonTriv v1(1);
		NonTriv v2(2);

		NonTriv::sReset();
		FastArray<NonTriv> arr;
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 0);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);

		NonTriv::sReset();
		arr.PushBack(v1);
		CHECK(arr.Size() == 1);
		CHECK(arr[0].Value() == 1);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 1);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);

		NonTriv::sReset();
		arr.PushBack(v2);
		CHECK(arr.Size() == 2);
		CHECK(arr[0].Value() == 1);
		CHECK(arr[1].Value() == 2);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 1);
		CHECK(NonTriv::sNumMoveConstructors == 1); // Array resizes from 1 to 2
		CHECK(NonTriv::sNumDestructors == 1); // Array resizes from 1 to 2

		NonTriv::sReset();
		arr.PopBack();
		CHECK(arr.Size() == 1);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 1);

		NonTriv::sReset();
		arr.PopBack();
		CHECK(arr.Size() == 0);
		CHECK(arr.Empty());
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 1);
	}

	void TestPushBackMove()
	{
		FastArray<FastArray<int>> arr;
		FastArray<int> arr2 = FastArray<int>::Of(1, 2, 3);
		arr.PushBack(KernelTypes::Move(arr2));
		CHECK(arr2.Size() == 0);
		CHECK(arr[0] == FastArray<int>::Of(1, 2, 3));
	}

	void TestEmplaceBack()
	{
		struct Test
		{
			Test(int inA, int inB) : mA(inA), mB(inB) { }

			int mA;
			int mB;
		};

		FastArray<Test> arr;
		arr.EmplaceBack(1, 2);
		CHECK(arr.Size() == 1);
		CHECK(arr[0].mA == 1);
		CHECK(arr[0].mB == 2);
	}

	void TestReserve()
	{
		FastArray<int> arr;
		CHECK(arr.GetCapacity() == 0);

		arr.Reserve(123);
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 123);

		arr.Reserve(456);
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 456);
	}

	void TestReserveNonTriv()
	{
		NonTriv::sReset();

		FastArray<NonTriv> arr;
		CHECK(arr.GetCapacity() == 0);

		arr.Reserve(123);
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 123);

		arr.Reserve(456);
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 456);

		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
	}

	void TestResize()
	{
		FastArray<int> arr;
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 0);
		CHECK(arr.Data() == nullptr);

		arr.Resize(0);
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 0);
		CHECK(arr.Data() == nullptr);

		arr.Resize(123);
		CHECK(arr.Size() == 123);
		CHECK(arr.GetCapacity() == 123);
		for (int i = 0; i < 123; ++i)
			arr[i] = i;

		arr.Resize(456);
		CHECK(arr.Size() == 456);
		CHECK(arr.GetCapacity() == 456);
		for (int i = 0; i < 123; ++i)
			CHECK(arr[i] == i);

		arr.Resize(10);
		CHECK(arr.Size() == 10);
		CHECK(arr.GetCapacity() >= 10);
	}

	void TestResizeNonTriv()
	{
		NonTriv::sReset();
		FastArray<NonTriv> arr;
		CHECK(arr.Size() == 0);
		CHECK(arr.GetCapacity() == 0);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);

		NonTriv::sReset();
		arr.Resize(123);
		CHECK(arr.Size() == 123);
		CHECK(arr.GetCapacity() == 123);
		CHECK(NonTriv::sNumConstructors == 123);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
		for (int i = 0; i < 123; ++i)
			arr[i] = NonTriv(i);

		NonTriv::sReset();
		arr.Resize(456);
		CHECK(arr.Size() == 456);
		CHECK(arr.GetCapacity() == 456);
		for (int i = 0; i < 123; ++i)
			CHECK(arr[i].Value() == i);
		CHECK(NonTriv::sNumConstructors == 456 - 123);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 123);
		CHECK(NonTriv::sNumDestructors == 123); // Switched to a new block, all old elements are destroyed after being moved

		NonTriv::sReset();
		arr.Resize(10);
		CHECK(arr.Size() == 10);
		CHECK(arr.GetCapacity() >= 10);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 456 - 10);
	}

	void TestResizeWithValue()
	{
		FastArray<int> arr;
		arr.Resize(10, 55);
		CHECK(arr.Size() == 10);
		CHECK(arr.GetCapacity() == 10);
		for (int i = 0; i < 10; ++i)
			CHECK(arr[i] == 55);
	}

	void TestResizeWithValueNonTriv()
	{
		NonTriv v(55);
		FastArray<NonTriv> arr;
		NonTriv::sReset();
		arr.Resize(10, v);
		CHECK(arr.Size() == 10);
		CHECK(arr.GetCapacity() == 10);
		for (int i = 0; i < 10; ++i)
			CHECK(arr[i].Value() == 55);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 10);
		CHECK(NonTriv::sNumMoveConstructors == 0);
		CHECK(NonTriv::sNumDestructors == 0);
	}

	void TestShrinkToFit()
	{
		FastArray<int> arr;
		for (int i = 0; i < 5; ++i)
			arr.PushBack(i);
		CHECK(arr.GetCapacity() > 5);
		CHECK(arr.Size() == 5);

		arr.ShrinkToFit();
		CHECK(arr.GetCapacity() == 5);
		CHECK(arr.Size() == 5);
		for (int i = 0; i < 5; ++i)
			CHECK(arr[i] == i);

		arr.Clear();
		CHECK(arr.GetCapacity() == 5);
		CHECK(arr.Size() == 0);

		arr.ShrinkToFit();
		CHECK(arr.GetCapacity() == 0);
		CHECK(arr.Data() == nullptr);
	}

	void TestShrinkToFitNonTriv()
	{
		FastArray<NonTriv> arr;
		for (int i = 0; i < 5; ++i)
			arr.PushBack(NonTriv(i));
		CHECK(arr.GetCapacity() > 5);
		CHECK(arr.Size() == 5);

		NonTriv::sReset();
		arr.ShrinkToFit();
		CHECK(arr.GetCapacity() == 5);
		CHECK(arr.Size() == 5);
		for (int i = 0; i < 5; ++i)
			CHECK(arr[i].Value() == i);
		CHECK(NonTriv::sNumConstructors == 0);
		CHECK(NonTriv::sNumCopyConstructors == 0);
		CHECK(NonTriv::sNumMoveConstructors == 5);
		CHECK(NonTriv::sNumDestructors == 5); // Switched to a new block, all old elements are destroyed after being moved
	}

	void TestAssignIterator()
	{
		int values[] = { 1, 2, 3 };

		FastArray<int> arr = FastArray<int>::Of(4, 5, 6);
		arr.Assign(values, values + 3);
		CHECK(arr.Size() == 3);
		CHECK(arr[0] == 1);
		CHECK(arr[1] == 2);
		CHECK(arr[2] == 3);
	}

	void TestAssignInitializerList()
	{
		FastArray<int> arr = FastArray<int>::Of(4, 5, 6);
		arr = FastArray<int>::Of(1, 2, 3);
		CHECK(arr.Size() == 3);
		CHECK(arr[0] == 1);
		CHECK(arr[1] == 2);
		CHECK(arr[2] == 3);
	}

	void TestSwap()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		FastArray<int> arr2 = FastArray<int>::Of(4, 5, 6);
		arr.Swap(arr2);
		CHECK(arr == FastArray<int>::Of(4, 5, 6));
		CHECK(arr2 == FastArray<int>::Of(1, 2, 3));
	}

	void TestInsertBegin()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Insert(arr.begin(), 4);
		CHECK(arr == FastArray<int>::Of(4, 1, 2, 3));
	}

	void TestInsertMid()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Insert(arr.begin() + 1, 4);
		CHECK(arr == FastArray<int>::Of(1, 4, 2, 3));
	}

	void TestInsertEnd()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Insert(arr.begin() + 3, 4);
		CHECK(arr == FastArray<int>::Of(1, 2, 3, 4));
	}

	void TestInsertMultipleBegin()
	{
		FastArray<int> values_to_insert = FastArray<int>::Of(4, 5, 6, 7);
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Insert(arr.begin(), values_to_insert.begin(), values_to_insert.end());
		CHECK(arr == FastArray<int>::Of(4, 5, 6, 7, 1, 2, 3));
	}

	void TestInsertMultipleMid()
	{
		FastArray<int> values_to_insert = FastArray<int>::Of(4, 5, 6, 7);
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Insert(arr.begin() + 1, values_to_insert.begin(), values_to_insert.end());
		CHECK(arr == FastArray<int>::Of(1, 4, 5, 6, 7, 2, 3));
	}

	void TestInsertMultipleEnd()
	{
		FastArray<int> values_to_insert = FastArray<int>::Of(4, 5, 6, 7);
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Insert(arr.begin() + 3, values_to_insert.begin(), values_to_insert.end());
		CHECK(arr == FastArray<int>::Of(1, 2, 3, 4, 5, 6, 7));
	}

	void TestFrontBack()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		CHECK(arr.Front() == 1);
		CHECK(arr.Back() == 3);
	}

	void TestAssign()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		FastArray<int> arr2 = FastArray<int>::Of(4, 5, 6);
		arr = arr2;
		CHECK(arr == FastArray<int>::Of(4, 5, 6));
		FastArray<int> &arr3 = arr; // Avoid compiler warning
		arr = arr3;
		CHECK(arr == FastArray<int>::Of(4, 5, 6));
		arr = FastArray<int>::Of(7, 8, 9);
		CHECK(arr == FastArray<int>::Of(7, 8, 9));
	}

	void TestAssignMove()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		FastArray<int> arr2 = FastArray<int>::Of(4, 5, 6);
		arr = KernelTypes::Move(arr2);
		CHECK(arr == FastArray<int>::Of(4, 5, 6));
		CHECK(arr2.Empty());
	}

	void TestEraseBegin()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Erase(arr.begin());
		CHECK(arr == FastArray<int>::Of(2, 3));
	}

	void TestEraseMid()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Erase(arr.begin() + 1);
		CHECK(arr == FastArray<int>::Of(1, 3));
	}

	void TestEraseEnd()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		arr.Erase(arr.begin() + 2);
		CHECK(arr == FastArray<int>::Of(1, 2));
	}

	void TestEraseMultipleBegin()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3, 4, 5);
		arr.Erase(arr.begin(), arr.begin() + 2);
		CHECK(arr == FastArray<int>::Of(3, 4, 5));
	}

	void TestEraseMultipleMid()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3, 4, 5);
		arr.Erase(arr.begin() + 2, arr.begin() + 4);
		CHECK(arr == FastArray<int>::Of(1, 2, 5));
	}

	void TestEraseMultipleEnd()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3, 4, 5);
		arr.Erase(arr.begin() + 3, arr.begin() + 5);
		CHECK(arr == FastArray<int>::Of(1, 2, 3));
	}

	void TestEquals()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3);
		FastArray<int> arr2 = FastArray<int>::Of(4, 5, 6);
		CHECK(arr == arr);
		CHECK(!(arr == arr2));
		CHECK(!(arr != arr));
		CHECK(arr != arr2);
	}

	void TestEmptyIterators()
	{
		FastArray<int> arr;
		CHECK(arr.begin() == arr.end());
		CHECK(arr.rbegin() == arr.rend());
		CHECK((arr.rbegin() + 0) == arr.rend());
		(void)arr.GetHash();
		arr.Assign(arr.begin(), arr.end());
		arr.Insert(arr.begin(), arr.begin(), arr.end());
		CHECK(arr.Erase(arr.begin(), arr.end()) == arr.end());
		const FastArray<int>& const_arr = arr;
		CHECK(const_arr.begin() == const_arr.end());
		CHECK(const_arr.rbegin() == const_arr.rend());

		FastArray<NonTriv> non_trivial;
		NonTriv value(7);
		non_trivial.Resize(0);
		non_trivial.Resize(0, value);
		CHECK(non_trivial.Empty());

		int values[] = {6, 7};
		FastArray<int> empty_insert;
		empty_insert.Insert(empty_insert.begin(), values, values + 2);
		CHECK(empty_insert.Size() == 2 && empty_insert[0] == 6 && empty_insert[1] == 7);

		FastArray<int> tail_insert = FastArray<int>::Of(5);
		tail_insert.Insert(tail_insert.end(), values, values + 2);
		CHECK(tail_insert.Size() == 3 && tail_insert[0] == 5 && tail_insert[1] == 6 && tail_insert[2] == 7);
	}

	void TestReverseIterator()
	{
		FastArray<int> arr = FastArray<int>::Of(1, 2, 3, 4, 5, 6);
		FastArray<int>::ReverseIterator r = arr.rbegin();
		CHECK(*r == 6);

		int v = *(++r);
		CHECK(v == 5);
		CHECK(*r == 5);

		v = *(--r);
		CHECK(v == 6);
		CHECK(*r == 6);

		v = *(r++);
		CHECK(v == 6);
		CHECK(*r == 5);

		v = *(r--);
		CHECK(v == 5);
		CHECK(*r == 6);

		v = *(r += 2);
		CHECK(v == 4);
		CHECK(*r == 4);

		v = *(r -= 2);
		CHECK(v == 6);
		CHECK(*r == 6);

		CHECK(r == arr.rbegin());
		CHECK(r != arr.rend());

		r += 6;
		CHECK(r == arr.rend());

		CHECK(*(arr.rbegin() + 3) == 3);
		CHECK(*(arr.rend() - 3) == 3);
		CHECK(arr.rbegin()[2] == 4 && arr.rend() - arr.rbegin() == 6);
	}

	void TestConstReverseIterator()
	{
		const FastArray<int> arr = FastArray<int>::Of(1, 2, 3, 4, 5, 6);
		FastArray<int>::ConstReverseIterator r = arr.rbegin();
		CHECK(*r == 6);

		int v = *(++r);
		CHECK(v == 5);
		CHECK(*r == 5);

		v = *(--r);
		CHECK(v == 6);
		CHECK(*r == 6);

		v = *(r++);
		CHECK(v == 6);
		CHECK(*r == 5);

		v = *(r--);
		CHECK(v == 5);
		CHECK(*r == 6);

		v = *(r += 2);
		CHECK(v == 4);
		CHECK(*r == 4);

		v = *(r -= 2);
		CHECK(v == 6);
		CHECK(*r == 6);

		CHECK(r == arr.rbegin());
		CHECK(r == arr.crbegin());
		CHECK(r != arr.rend());
		CHECK(r != arr.crend());

		r += 6;
		CHECK(r == arr.rend());
		CHECK(r == arr.crend());

		CHECK(*(arr.rbegin() + 3) == 3);
		CHECK(*(arr.rend() - 3) == 3);
		CHECK(arr.rbegin()[2] == 4 && arr.rend() - arr.rbegin() == 6);
	}
}

int RunFastArrayUnitTests()
{
    namespace KM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelMemory;
    const auto BaseBlocks = KM::GetLiveBlockCount();

    TestConstructLength();
    TestConstructLengthNonTriv();
    TestConstructValue();
    TestConstructValueNonTriv();
    TestConstructIterator();
    TestConstructIteratorNonTriv();
    TestConstructInitializerList();
    TestConstructInitializerListNonTriv();
    TestConstructFromArray();
    TestConstructFromArrayNonTriv();
    TestMoveFromArray();
    TestMoveFromArrayNonTriv();
    TestClear();
    TestClearNonTriv();
    TestPushBack();
    TestPushBackNonTriv();
    TestPushBackMove();
    TestEmplaceBack();
    TestReserve();
    TestReserveNonTriv();
    TestResize();
    TestResizeNonTriv();
    TestResizeWithValue();
    TestResizeWithValueNonTriv();
    TestShrinkToFit();
    TestShrinkToFitNonTriv();
    TestAssignIterator();
    TestAssignInitializerList();
    TestSwap();
    TestInsertBegin();
    TestInsertMid();
    TestInsertEnd();
    TestInsertMultipleBegin();
    TestInsertMultipleMid();
    TestInsertMultipleEnd();
    TestFrontBack();
    TestAssign();
    TestAssignMove();
    TestEraseBegin();
    TestEraseMid();
    TestEraseEnd();
    TestEraseMultipleBegin();
    TestEraseMultipleMid();
    TestEraseMultipleEnd();
    TestEquals();
    TestEmptyIterators();
    TestReverseIterator();
    TestConstReverseIterator();

    // Ни один тест не должен оставить память в Kernel.Memory
    CHECK(KM::GetLiveBlockCount() == BaseBlocks);

    std::printf("FastArray: %d тестов, %d проверок, %d ошибок\n", 48, Checks, Failures);
    return Failures == 0 ? 0 : 1;
}
