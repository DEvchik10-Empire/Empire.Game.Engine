// Тест FastHashSet, FastHashMap и ScopeExit. Перенесён из UnorderedSetTest, UnorderedMapTest и ScopeExitTest Jolt
// (UnitTests/Core). Те же случаи, включая принудительные коллизии, надгробия и перехеширование на месте.
// Изменения при переносе: методы и поля переименованы под стиль движка (find -> Find, first -> First и так далее),
// JPH_SCOPE_EXIT заменён объектами ScopeExit (макросов в ядре нет), добавлена проверка групповых операций над 16 байтами.
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.HashTables.hpp"

#include "Empire.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.FastHashMap.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.FastHashSet.hpp"
#include "Empire.Kernel/Kernel.Types/Types.ScopeExit.hpp"
#include <cstdio>

namespace KernelTypes = DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelTypes;
namespace KernelCompiler = DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelCompiler;
using DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelContainers::FastHashMap;
using DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelContainers::FastHashSet;
using KernelTypes::Pair;

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

static bool sReversedHash = false;

	static void TestFastHashSet()
	{
		FastHashSet<int> set;
		CHECK(set.BucketCount() == 0);
		set.Reserve(10);
		CHECK(set.BucketCount() == 16);

		// Check system limits
		CHECK(set.MaxBucketCount() == 0x80000000);
		CHECK(set.MaxElementCount() == KernelTypes::UInt64(0x80000000) * 7 / 8);

		// Insert some entries
		CHECK(*set.Insert(1).First == 1);
		CHECK(set.Insert(3).Second);
		CHECK(!set.Insert(3).Second);
		CHECK(set.Size() == 2);
		CHECK(*set.Find(1) == 1);
		CHECK(*set.Find(3) == 3);
		CHECK(set.Find(5) == set.cend());

		// Validate all elements are visited by a visitor
		int count = 0;
		bool visited[10] = { false };
		for (FastHashSet<int>::ConstIterator i = set.begin(); i != set.end(); ++i)
		{
			visited[*i] = true;
			++count;
		}
		CHECK(count == 2);
		CHECK(visited[1]);
		CHECK(visited[3]);
		for (FastHashSet<int>::Iterator i = set.begin(); i != set.end(); ++i)
		{
			visited[*i] = false;
			--count;
		}
		CHECK(count == 0);
		CHECK(!visited[1]);
		CHECK(!visited[3]);

		// Copy the set
		FastHashSet<int> set2;
		set2 = set;
		CHECK(*set2.Find(1) == 1);
		CHECK(*set2.Find(3) == 3);
		CHECK(set2.Find(5) == set2.cend());

		// Swap
		FastHashSet<int> set3;
		set3.Swap(set);
		CHECK(*set3.Find(1) == 1);
		CHECK(*set3.Find(3) == 3);
		CHECK(set3.Find(5) == set3.end());
		CHECK(set.Empty());

		// Move construct
		FastHashSet<int> set4(KernelTypes::Move(set3));
		CHECK(*set4.Find(1) == 1);
		CHECK(*set4.Find(3) == 3);
		CHECK(set4.Find(5) == set4.end());
		CHECK(set3.Empty());

		// Move assign
		FastHashSet<int> set5;
		set5.Insert(999);
		CHECK(*set5.Find(999) == 999);
		set5 = KernelTypes::Move(set4);
		CHECK(set5.Find(999) == set5.end());
		CHECK(*set5.Find(1) == 1);
		CHECK(*set5.Find(3) == 3);
		CHECK(set4.Empty());
	}

	static void TestFastHashSetGrow()
	{
		FastHashSet<int> set;
		for (int i = 0; i < 10000; ++i)
			CHECK(set.Insert(i).Second);

		CHECK(set.Size() == 10000);

		for (int i = 0; i < 10000; ++i)
			CHECK(*set.Find(i) == i);

		CHECK(set.Find(10001) == set.cend());

		for (int i = 0; i < 5000; ++i)
			CHECK(set.Erase(i) == 1);

		CHECK(set.Size() == 5000);

		for (int i = 0; i < 5000; ++i)
			CHECK(set.Find(i) == set.end());

		for (int i = 5000; i < 10000; ++i)
			CHECK(*set.Find(i) == i);

		CHECK(set.Find(10001) == set.cend());

		for (int i = 0; i < 5000; ++i)
			CHECK(set.Insert(i).Second);

		CHECK(!set.Insert(0).Second);

		CHECK(set.Size() == 10000);

		for (int i = 0; i < 10000; ++i)
			CHECK(*set.Find(i) == i);

		CHECK(set.Find(10001) == set.cend());
	}

	static void TestFastHashSetHashCollision()
	{
		// A hash function that's guaranteed to collide
		class MyBadHash
		{
		public:
			KernelTypes::UInt64 operator () (int) const
			{
				return 0;
			}
		};

		FastHashSet<int, MyBadHash> set;
		for (int i = 0; i < 10; ++i)
			CHECK(set.Insert(i).Second);

		CHECK(set.Size() == 10);

		for (int i = 0; i < 10; ++i)
			CHECK(*set.Find(i) == i);

		CHECK(set.Find(11) == set.cend());

		for (int i = 0; i < 5; ++i)
			CHECK(set.Erase(i) == 1);

		CHECK(set.Size() == 5);

		for (int i = 0; i < 5; ++i)
			CHECK(set.Find(i) == set.end());

		for (int i = 5; i < 10; ++i)
			CHECK(*set.Find(i) == i);

		CHECK(set.Find(11) == set.cend());

		for (int i = 0; i < 5; ++i)
			CHECK(set.Insert(i).Second);

		CHECK(!set.Insert(0).Second);

		CHECK(set.Size() == 10);

		for (int i = 0; i < 10; ++i)
			CHECK(*set.Find(i) == i);

		CHECK(set.Find(11) == set.cend());
	}

	static void TestFastHashSetAddRemoveCyles()
	{
		FastHashSet<int> set;
		constexpr int cBucketCount = 64;
		set.Reserve(int(set.MaxLoadFactor() * cBucketCount));
		CHECK(set.BucketCount() == cBucketCount);

		// Repeatedly add and remove elements to see if the set cleans up tombstones
		constexpr int cNumElements = 64 * 6 / 8; // We make sure that the map is max 6/8 full to ensure that we never grow the map but rehash it instead
		int add_counter = 0;
		int remove_counter = 0;
		for (int i = 0; i < 100; ++i)
		{
			for (int j = 0; j < cNumElements; ++j)
			{
				CHECK(set.Find(add_counter) == set.end());
				CHECK(set.Insert(add_counter).Second);
				CHECK(set.Find(add_counter) != set.end());
				++add_counter;
			}

			CHECK(set.Size() == cNumElements);

			for (int j = 0; j < cNumElements; ++j)
			{
				CHECK(set.Find(remove_counter) != set.end());
				CHECK(set.Erase(remove_counter) == 1);
				CHECK(set.Erase(remove_counter) == 0);
				CHECK(set.Find(remove_counter) == set.end());
				++remove_counter;
			}

			CHECK(set.Size() == 0);
			CHECK(set.Empty());
		}

		// Test that adding and removing didn't resize the set
		CHECK(set.BucketCount() == cBucketCount);
	}

	static void TestFastHashSetManyTombStones()
	{
		// A hash function that makes sure that consecutive ints end up in consecutive buckets starting at bucket 63
		class MyBadHash
		{
		public:
			KernelTypes::UInt64 operator () (int inValue) const
			{
				return (inValue + 63) << 7;
			}
		};

		FastHashSet<int, MyBadHash> set;
		constexpr int cBucketCount = 64;
		set.Reserve(int(set.MaxLoadFactor() * cBucketCount));
		CHECK(set.BucketCount() == cBucketCount);

		// Fill 32 buckets
		int add_counter = 0;
		for (int i = 0; i < 32; ++i)
			CHECK(set.Insert(add_counter++).Second);

		// Since we control the hash, we know in which order we'll visit the elements
		// The first element was inserted in bucket 63, so we start at 1
		int expected = 1;
		for (int i : set)
		{
			CHECK(i == expected);
			expected = (expected + 1) & 31;
		}
		expected = 1;
		for (int i : set)
		{
			CHECK(i == expected);
			expected = (expected + 1) & 31;
		}

		// Remove a bucket in the middle with so that the number of occupied slots
		// surrounding the bucket exceed 16 to force creating a tombstone,
		// then add one at the end
		int remove_counter = 16;
		for (int i = 0; i < 100; ++i)
		{
			CHECK(set.Find(remove_counter) != set.end());
			CHECK(set.Erase(remove_counter) == 1);
			CHECK(set.Find(remove_counter) == set.end());

			CHECK(set.Find(add_counter) == set.end());
			CHECK(set.Insert(add_counter).Second);
			CHECK(set.Find(add_counter) != set.end());

			++add_counter;
			++remove_counter;
		}

		// Check that the elements we inserted are still there
		CHECK(set.Size() == 32);
		for (int i = 0; i < 16; ++i)
			CHECK(*set.Find(i) == i);
		for (int i = 0; i < 16; ++i)
			CHECK(*set.Find(add_counter - 1 - i) == add_counter - 1 - i);

		// Test that adding and removing didn't resize the set
		CHECK(set.BucketCount() == cBucketCount);
	}

	

	static void TestFastHashSetRehash()
	{
		// A hash function for which we can switch the hashing algorithm
		class MyBadHash
		{
		public:
			KernelTypes::UInt64 operator () (int inValue) const
			{
				return (sReversedHash? 127 - inValue : inValue) << 7;
			}
		};

		using Set = FastHashSet<int, MyBadHash>;
		Set set;
		constexpr int cBucketCount = 128;
		set.Reserve(int(set.MaxLoadFactor() * cBucketCount));
		CHECK(set.BucketCount() == cBucketCount);

		// Fill buckets
		sReversedHash = false;
		constexpr int cNumElements = 96;
		for (int i = 0; i < cNumElements; ++i)
			CHECK(set.Insert(i).Second);

		// Check that we get the elements in the expected order
		int expected = 0;
		for (int i : set)
			CHECK(i == expected++);

		// Change the hashing algorithm so that a rehash is forced to move elements.
		// The test is designed in such a way that it will both need to move elements to empty slots
		// and to move elements to slots that currently already have another element.
		sReversedHash = true;
		set.Rehash(0);

		// Check that all elements are still there
		for (int i = 0; i < cNumElements; ++i)
			CHECK(*set.Find(i) == i);

		// The hash went from filling buckets 0 .. 95 with values 0 .. 95 to bucket 127 .. 31 with values 0 .. 95
		// However, we don't move elements if they still fall within the same batch, this means that the first 8
		// elements didn't move
		Set::ConstIterator it = set.begin();
		for (int i = 0; i < 8; ++i, ++it)
			CHECK(*it == i);

		// The rest will have been reversed
		for (int i = 95; i > 7; --i, ++it)
			CHECK(*it == i);

		// Test that adding and removing didn't resize the set
		CHECK(set.BucketCount() == cBucketCount);
	}

	static void TestFastHashMap()
	{
		FastHashMap<int, int> map;
		map.Reserve(10);

		// Insert some entries
		CHECK(map.Insert(Pair<int, int>(1, 2)).First->First == 1);
		CHECK(map.Insert(Pair<int, int>(3, 4)).Second);
		CHECK(!map.Insert(Pair<int, int>(3, 5)).Second);
		CHECK(map.Size() == 2);
		CHECK(map.Find(1)->Second == 2);
		CHECK(map.Find(3)->Second == 4);
		CHECK(map.Find(5) == map.end());

		// Use operator []
		map[5] = 6;
		CHECK(map.Size() == 3);
		CHECK(map.Find(5)->Second == 6);
		map[5] = 7;
		CHECK(map.Size() == 3);
		CHECK(map.Find(5)->Second == 7);

		// Validate all elements are visited by a visitor
		int count = 0;
		bool visited[10] = { false };
		for (FastHashMap<int, int>::ConstIterator i = map.begin(); i != map.end(); ++i)
		{
			visited[i->First] = true;
			++count;
		}
		CHECK(count == 3);
		CHECK(visited[1]);
		CHECK(visited[3]);
		CHECK(visited[5]);
		for (FastHashMap<int, int>::Iterator i = map.begin(); i != map.end(); ++i)
		{
			visited[i->First] = false;
			--count;
		}
		CHECK(count == 0);
		CHECK(!visited[1]);
		CHECK(!visited[3]);
		CHECK(!visited[5]);

		// Copy the map
		FastHashMap<int, int> map2;
		map2 = map;
		CHECK(map2.Find(1)->Second == 2);
		CHECK(map2.Find(3)->Second == 4);
		CHECK(map2.Find(5)->Second == 7);
		CHECK(map2.Find(7) == map2.end());

		// Try emplace
		map.TryEmplace(7, 8);
		CHECK(map.Size() == 4);
		CHECK(map.Find(7)->Second == 8);

		// Swap
		FastHashMap<int, int> map3;
		map3.Swap(map);
		CHECK(map3.Find(1)->Second == 2);
		CHECK(map3.Find(3)->Second == 4);
		CHECK(map3.Find(5)->Second == 7);
		CHECK(map3.Find(7)->Second == 8);
		CHECK(map3.Find(9) == map3.end());
		CHECK(map.Empty());

		// Move construct
		FastHashMap<int, int> map4(KernelTypes::Move(map3));
		CHECK(map4.Find(1)->Second == 2);
		CHECK(map4.Find(3)->Second == 4);
		CHECK(map4.Find(5)->Second == 7);
		CHECK(map4.Find(7)->Second == 8);
		CHECK(map4.Find(9) == map4.end());
		CHECK(map3.Empty());
	}

	static void TestFastHashMapGrow()
	{
		FastHashMap<int, int> map;
		for (int i = 0; i < 10000; ++i)
			CHECK(map.TryEmplace(i, ~i).Second);

		CHECK(map.Size() == 10000);

		for (int i = 0; i < 10000; ++i)
			CHECK(map.Find(i)->Second == ~i);

		CHECK(map.Find(10001) == map.end());

		for (int i = 0; i < 5000; ++i)
			CHECK(map.Erase(i) == 1);

		CHECK(map.Size() == 5000);

		for (int i = 0; i < 5000; ++i)
			CHECK(map.Find(i) == map.end());

		for (int i = 5000; i < 10000; ++i)
			CHECK(map.Find(i)->Second == ~i);

		CHECK(map.Find(10001) == map.end());

		for (int i = 0; i < 5000; ++i)
			CHECK(map.TryEmplace(i, i + 1).Second);

		CHECK(!map.TryEmplace(0, 0).Second);

		CHECK(map.Size() == 10000);

		for (int i = 0; i < 5000; ++i)
			CHECK(map.Find(i)->Second == i + 1);

		for (int i = 5000; i < 10000; ++i)
			CHECK(map.Find(i)->Second == ~i);

		CHECK(map.Find(10001) == map.end());
	}

static void TestScopeExitOrder()
{
    int value = 0;
    {
        // Последний созданный вызывается первым
        KernelTypes::ScopeExit First([&value] { CHECK(value == 1); value = 2; });
        KernelTypes::ScopeExit Second([&value] { CHECK(value == 0); value = 1; });
        CHECK(value == 0);
    }
    CHECK(value == 2);
}

static void TestScopeExitRelease()
{
    int value = 0;
    {
        KernelTypes::ScopeExit scope_exit([&value] { value++; });
        CHECK(value == 0);
        scope_exit.Release();
    }
    CHECK(value == 0);
}

static void TestScopeExitInvoke()
{
    int value = 0;
    {
        KernelTypes::ScopeExit scope_exit([&value] { value++; });
        CHECK(value == 0);
        scope_exit.Invoke();
        CHECK(value == 1);
        // Второй раз при выходе вызываться не должна
    }
    CHECK(value == 1);
}

static void TestMask16()
{
    // Проверка групповых операций над 16 байтами, которыми пользуются хеш-таблицы
    KernelTypes::UInt8 Bytes[32];
    for (int i = 0; i < 32; ++i)
        Bytes[i] = static_cast<KernelTypes::UInt8>((i * 37) & 0xff);
    for (int Offset = 0; Offset < 16; ++Offset)
    {
        KernelTypes::UInt32 ExpectedHigh = 0;
        for (int i = 0; i < 16; ++i)
            if (Bytes[Offset + i] & 0x80)
                ExpectedHigh |= 1u << i;
        CHECK(KernelCompiler::HighBitMask16(Bytes + Offset) == ExpectedHigh);
        for (int Value = 0; Value < 256; ++Value)
        {
            KernelTypes::UInt32 Expected = 0;
            for (int i = 0; i < 16; ++i)
                if (Bytes[Offset + i] == Value)
                    Expected |= 1u << i;
            CHECK(KernelCompiler::EqualMask16(Bytes + Offset, static_cast<KernelTypes::UInt8>(Value)) == Expected);
        }
    }
    CHECK(KernelCompiler::CountTrailingZeros(0) == 32);
    CHECK(KernelCompiler::CountTrailingZeros(8) == 3);
    CHECK(KernelCompiler::CountLeadingZeros(0) == 32);
    CHECK(KernelCompiler::CountLeadingZeros(1) == 31);
    CHECK(KernelTypes::NextPowerOfTwo(17) == 32);
    CHECK(KernelTypes::NextPowerOfTwo(16) == 16);
}

int RunHashTablesUnitTests()
{
    Failures = 0;
    Checks = 0;
    TestFastHashSet();
    TestFastHashSetGrow();
    TestFastHashSetHashCollision();
    TestFastHashSetAddRemoveCyles();
    TestFastHashSetManyTombStones();
    TestFastHashSetRehash();
    TestFastHashMap();
    TestFastHashMapGrow();
    TestScopeExitOrder();
    TestScopeExitRelease();
    TestScopeExitInvoke();
    TestMask16();
    std::printf("HashTables: %d checks, %d failures\n", Checks, Failures);
    return Failures == 0 ? 0 : 1;
}
