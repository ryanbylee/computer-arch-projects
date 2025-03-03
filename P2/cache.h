#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;

#define L1_CACHE_SETS 16
#define L2_CACHE_SETS 16
#define VICTIM_SIZE 4
#define L2_CACHE_WAYS 8
#define MEM_SIZE 4096
#define BLOCK_SIZE 4 // bytes per block
#define DM 0
#define SA 1


struct cacheBlock
{
	int adr;
	int tag; // you need to compute offset and index to find the tag.
	int lru_position; // for SA only
	int data; // the actual data stored in the cache/memory
	bool valid;
};

struct Stat
{
	int missL1; 
	int missL2; 
	int accL1;
	int accL2;
	int accVic;
	int missVic;
	// add more stat if needed. Don't forget to initialize!
};

class cache {
private:
	cacheBlock L1[L1_CACHE_SETS]; // 1 set per row.
	cacheBlock L2[L2_CACHE_SETS][L2_CACHE_WAYS]; // x ways per row 
	cacheBlock Victim[VICTIM_SIZE]; // 1 set per row

	Stat myStat;
	// add more things here
public:
	cache();
	void printL1();
	void printL2();
	void printVictim();
	Stat getStat();
	void controller(bool MemR, bool MemW, int* data, int adr, int* myMem);
	bool search(string partition, int adr, int* data);
	void update(string partition, int adr, int* data, int type);
	void evict(string partition, int adr, int* data, int type);
};


