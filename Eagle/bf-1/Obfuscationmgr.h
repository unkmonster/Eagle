#pragma once

#include "Frosbite.h"

#include <utility>
#include <cstdint>

typedef DWORD64 QWORD;
typedef QWORD _QWORD;

typedef BYTE _BYTE;
typedef DWORD _DWORD;
typedef WORD _WORD;



template<typename T>
struct hash_node {
	std::pair<_QWORD, T*> mValue;
	hash_node<T>* mpNext;
};

template<typename T>
struct hashtable {
	_QWORD vtable;
	hash_node<T>** mpBucketArray;
	unsigned int mnBucketCount;
	unsigned int mnElementCount;
	//...
};

template<typename T>
struct hashtable_iterator {
	hash_node<T>* mpNode;
	hash_node<T>** mpBucket;
};

hashtable_iterator<_QWORD>* __fastcall hashtable_find(hashtable<_QWORD>* table, hashtable_iterator<_QWORD>* iterator, _QWORD key);

fb::ClientPlayer* GetPlayerById(int id);
fb::ClientPlayer* GetLocalPlayer(void);

class ObfuscationMgr {
	static ObfuscationMgr* GetInstance() {
		return *reinterpret_cast<ObfuscationMgr**>(OFFSET_ObfuscationMgr);
	}
public:
	char pad_0000[0x8];				// 0x0
	hashtable<uint64_t>* hashTable;	// 0x8
	char pad_0016[0x60];			// 0x10
	uint64_t playerListXor;			// 0x70
};