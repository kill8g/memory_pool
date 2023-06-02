#ifndef _MEMORY_WHEEL_H_
#define _MEMORY_WHEEL_H_

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <stdint.h>

#define PLACEMENT_NEW(ptr, _type, ...) new (ptr) _type { __VA_ARGS__ }
#define NEW(pool, _type, ...) PLACEMENT_NEW((pool)->palloc(), _type, __VA_ARGS__)

using uint64 = unsigned long long;
#define bit_size 64
class bit {// 比使用std::bitset稍快
public:
	bit(size_t len) {
		this->_len = len;
		this->_bitset.resize(len / bit_size, 0);
		if (len % bit_size != 0) {
			this->_bitset.push_back(0);
		}
	}
	bit(const bit&) = delete;
	bit& operator=(const bit&) = delete;
	bool get(size_t offset) {
		assert(offset < this->_len);
		uint64 index = offset / bit_size;
		uint64 mod = offset % bit_size;
		uint64 mask = (uint64)1 << mod;
		uint64 result = this->_bitset[index] & mask;
		return result != 0;
	}
	void set(size_t offset, bool value) {
		assert(offset < this->_len);
		uint64 index = offset / bit_size;
		uint64 mod = offset % bit_size;
		if (!value) {
			uint64 mask = ~(1 << mod);
			this->_bitset[index] = this->_bitset[index] & mask;
		}
		else {
			uint64 mask = (uint64)1 << mod;
			this->_bitset[index] = this->_bitset[index] | mask;
		}
	}
private:
	std::vector<uint64> _bitset;
	size_t _len;
};

#define failed_skip 3

class memory_wheel {
public:
	memory_wheel(size_t class_sizeof, size_t chunk_size) {
		this->_chunk_size = chunk_size;
		this->_class_sizeof = class_sizeof;
		this->_offset = 0;
		this->_failed = 0;
		this->_using = new bit(chunk_size);
		this->_buffer = malloc(chunk_size * class_sizeof);
		this->_start = (uintptr_t)this->_buffer;
		this->_over = this->_start + chunk_size * class_sizeof;
	}

	~memory_wheel() {
		free(this->_buffer);
		delete this->_using;
		this->_offset = 0;
		this->_buffer = nullptr;
		this->_failed = 0;
	}

	memory_wheel(const memory_wheel&) = delete;
	memory_wheel& operator=(const memory_wheel&) = delete;

	void* palloc() {
		// 一整块内存都被使用了, 回到起点
		if (this->_offset >= this->_chunk_size) {
			this->_offset = 0;
		}
		// 这一块内存是否被使用
		if (!this->_using->get(this->_offset)) {
			// 标记内存被占用
			this->_using->set(this->_offset, 1);
			// 分配一块_class_sizeof大小的内存
			uintptr_t addr = (uintptr_t)this->_buffer;
			void* p = (void*)(addr + this->_class_sizeof * this->_offset++);
			memset(p, 0, this->_class_sizeof);
			return p;
		}
		// _offset所在的这一块内存被占用了很久都没有释放
		if (this->_failed++ >= failed_skip) {
			// 跳过他
			++this->_offset;
			// 重置失败次数
			this->_failed = 0;
		}
		void* p = malloc(this->_class_sizeof);
		if (!p)
			return nullptr;
		memset(p, 0, this->_class_sizeof);
		return p;
	}

	void recycle(void* p) {
		if (!p)
			return;
		uintptr_t addr = (uintptr_t)p;
		if (addr >= this->_start && addr <= (this->_over - this->_class_sizeof)) {
			auto diff = addr - this->_start;
			auto mod = diff % this->_class_sizeof;
			assert(mod == 0);
			size_t offset = diff / this->_class_sizeof;
			this->_using->set(offset, 0);
		}
		else {
			free(p);
		}
	}
private:
	size_t _class_sizeof;
	size_t _chunk_size;
	size_t _offset;
	void* _buffer;
	int _failed;
	uintptr_t _start;
	uintptr_t _over;
	bit* _using;
};

#endif // _MEMORY_WHEEL_H_
