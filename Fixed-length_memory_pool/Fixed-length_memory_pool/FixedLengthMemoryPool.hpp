#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <stdlib.h>
using std::cout;
using std::endl;
template<class T>
class FixedLengthMemoryPool
{
public:
	T* New()
	{
		if (_freeList)
		{
			void* tmp = _freeList;
			_freeList = *(void**)_freeList;
			new (tmp)T();
			return (T*)tmp;
		}
		else
		{
			int alloc_size = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
			if (_leftMemory < alloc_size)
			{
				_memory = (char*)malloc(128 * 1024);
				if (_memory == nullptr)
				{
					cout << "malloc fail" << endl;
					exit(0);
				}
				_leftMemory = 128 * 1024;
			}
			_leftMemory -= alloc_size;
			char* res = _memory;
			_memory += alloc_size;
			new (res)T();
			return (T*)res;
		}
	}
	void Delete(void* del_addr)
	{
		((T*)del_addr)->~T();
		//将del_addr前四个字节的空间填充为_freeList前四个字节中存储的内容
		*(void**)del_addr = _freeList;
		_freeList = del_addr;
	}
private:
	char* _memory = nullptr;
	void* _freeList = nullptr;
	int _leftMemory = 0;

};