#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include "FixedLengthMemoryPool.hpp"  

using namespace std;

struct TestObj
{
    int a[8];   // 32×Ö½Ú×óÓÒ
};

const size_t N = 5'000'000;

void TestMalloc()
{
    vector<TestObj*> vec;
    vec.reserve(N);

    auto begin = chrono::high_resolution_clock::now();

    for (size_t i = 0; i < N; ++i)
    {
        TestObj* p = (TestObj*)malloc(sizeof(TestObj));
        new (p) TestObj();
        vec.push_back(p);
    }

    for (size_t i = 0; i < N; ++i)
    {
        vec[i]->~TestObj();
        free(vec[i]);
    }

    auto end = chrono::high_resolution_clock::now();

    cout << "malloc/free time: "
        << chrono::duration<double>(end - begin).count()
        << " s" << endl;
}

void TestMemoryPool()
{
    FixedLengthMemoryPool<TestObj> pool;
    vector<TestObj*> vec;
    vec.reserve(N);

    auto begin = chrono::high_resolution_clock::now();

    for (size_t i = 0; i < N; ++i)
    {
        vec.push_back(pool.New());
    }

    for (size_t i = 0; i < N; ++i)
    {
        pool.Delete(vec[i]);
    }

    auto end = chrono::high_resolution_clock::now();

    cout << "MemoryPool time: "
        << chrono::duration<double>(end - begin).count()
        << " s" << endl;
}

int main()
{
    cout << "Testing with N = " << N << endl;

    TestMalloc();
    TestMemoryPool();

    return 0;
}
