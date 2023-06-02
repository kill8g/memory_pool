# memory_pool
C++ memory pool

# Notes
- The code is simple and easy to modify, with less than 150 lines in total
- Allocate memory with fixed size
- If in your project, an object has a short lifespan
- If in your project, in most cases, memory is destroyed in the same order as it was created
- Do not use third-party libraries
- In multi-threaded environment, please bind a memory pool to each thread
- In windows environment, the performance is about `1/15` of `malloc` and `free`
- 代码简单易修改, 总共不到150行
- 固定大小分配内存
- 如果在你的项目中, 一个对象的生命周期短暂
- 如果在你的项目中, 在大多数情况下, 内存是按照创建时的顺序来销毁的
- 不使用第三方库
- 多线程下, 请给每个线程绑定一个内存池
- windows环境下, 性能约为`malloc`和`free`的**1/15**

# Usage
```C++
#include "memory_wheel.hpp"
class TestClass1 {
public:
	TestClass1(int v1, int v2) {
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = 0;
		this->b = false;
	}
	TestClass1(int v1, int v2, unsigned long long v3) {
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		this->b = false;
	}

	bool b;
	int v1;
	int v2;
	unsigned long long v3;
};
int main(int argc, char* argv[]) {
	size_t size = sizeof(TestClass1);
	memory_wheel wheel(size, 512);
	TestClass1* obj1 = NEW(&wheel, TestClass1, 1, 2);
	wheel.recycle(obj1);
	TestClass1* obj2 = NEW(&wheel, TestClass1, 50, 200, 999);
	wheel.recycle(obj2);
	return 0;
}

```
