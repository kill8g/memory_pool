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

int main(int argc, char * argv[]) {
	size_t size = sizeof(TestClass1);
	memory_wheel wheel(size, 1);
	TestClass1* obj1 = NEW(&wheel, TestClass1, 1, 2);
	wheel.recycle(obj1);
	return 0;
}
