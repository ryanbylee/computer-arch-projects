#include "cache.h"
#include <iostream>
cache::cache()
{
	for (int i=0; i<L1_CACHE_SETS; i++)
		L1[i].valid = false; 

	for (int i=0; i<L2_CACHE_SETS; i++)
		for (int j=0; j<L2_CACHE_WAYS; j++)
			L2[i][j].valid = false; 

	for (int k = 0; k < VICTIM_SIZE; k++)
		Victim[k].valid = false;

	this->myStat.missL1 =0;
	this->myStat.missL2 =0;
	this->myStat.accL1 =0;
	this->myStat.accL2 =0;
	this->myStat.accVic =0;
	this->myStat.missVic =0;
	
}

Stat cache::getStat()
{
	return this->myStat;
}
bool cache::search(string partition, int adr, int* data)
{
	
	int offset = adr & 0b000000000011;
	int index =  (adr & 0b000000111100) >> 2;	
	int tag =	 (adr & 0b111111000000) >> 6;
	int victim_tag = (adr & 0b111111111100) >> 2;

	if (partition == "L1"){

		if (L1[index].valid && L1[index].tag == tag){
			return true;
		}
		return false;
	}
	else if (partition == "Victim"){
		for (int i = 0; i < VICTIM_SIZE; i++){
			if (Victim[i].valid && Victim[i].tag == victim_tag){
				return true;
			}
		}
		return false;
	}
	else if (partition == "L2"){
		for (int i = 0; i < L2_CACHE_WAYS; i++){
			if (L2[index][i].valid && L2[index][i].tag == tag){
				return true;
			}
		}
		return false;
		
	}
	else{
		cerr << "Error: invalid partition" << endl;
		exit(1);
	}
}

void cache::update(string partition, int adr, int* data, int type)
{
	
	int offset = adr & 0b000000000011;
	int index =  (adr & 0b000000111100) >> 2;	
	int tag =	 (adr & 0b111111000000) >> 6;
	int victim_tag = (adr & 0b111111111100) >> 2;

	if (partition == "L1"){
		if (L1[index].valid){
			evict("L1", L1[index].adr, &L1[index].data, type);
		}
		L1[index].adr = adr;
		L1[index].valid = true;
		L1[index].tag = tag;
		L1[index].data = *data;
	}
	if (partition == "Victim"){
		for (int i = 0; i < VICTIM_SIZE; i++){
			if (!Victim[i].valid){
				Victim[i].adr = adr;
				Victim[i].valid = true;
				Victim[i].tag = victim_tag;
				Victim[i].data = *data;

				// update lru positions
				Victim[i].lru_position = 0;
				for (int j = 0; j < VICTIM_SIZE; j++){
					if (j != i && Victim[j].valid){
						Victim[j].lru_position += 1;
					}
				}
				return;
			}
			else if (Victim[i].lru_position == VICTIM_SIZE - 1){
				evict("Victim", Victim[i].adr, data, type);
				Victim[i].adr = adr;
				Victim[i].valid = true;
				Victim[i].tag = victim_tag;
				Victim[i].data = *data;

				// update lru positions
				Victim[i].lru_position = 0;
				for (int j = 0; j < VICTIM_SIZE; j++){
					if (j != i && Victim[j].valid){
						Victim[j].lru_position += 1;
					}
				}
				return;
			}
		}
	}
	if (partition == "L2"){
		for (int i = 0; i < L2_CACHE_WAYS; i++){
			if (!L2[index][i].valid){
				L2[index][i].adr = adr;
				L2[index][i].valid = true;
				L2[index][i].tag = tag;
				L2[index][i].data = *data;
				// update lru positions
				L2[index][i].lru_position = 0;
				for (int j = 0; j < L2_CACHE_WAYS; j++){
					if (j != i && L2[index][j].valid){
						L2[index][j].lru_position += 1;
					}
				}
				return;
			}
			else if (L2[index][i].lru_position == L2_CACHE_WAYS - 1){
				L2[index][i].adr = adr;
				L2[index][i].valid = true;
				L2[index][i].tag = tag;
				L2[index][i].data = *data;

				L2[index][i].lru_position = 0;
				for (int j = 0; j < L2_CACHE_WAYS; j++){
					if (j != i && L2[index][j].valid){
						L2[index][j].lru_position += 1;
					}
				}
				return;
			}
		}
	}
	// printL1();
}

void cache::evict(string partition, int adr, int* data, int type)
{
	int offset = adr & 0b000000000011;
	int index =  (adr & 0b000000111100) >> 2;	
	int tag =	 (adr & 0b111111000000) >> 6;

	// if (type == DM){
	// 	update(L2[index], adr, data, SA);
	// }
	if (partition == "L1"){
		update("Victim", adr, data, DM);
	}
	if (partition == "Victim"){
		update("L2", adr, data, DM);
	}

}
void cache::controller(bool MemR, bool MemW, int* data, int adr, int* myMem)
{
	if(MemR){
		if (search("L1", adr, &myMem[adr])){
			
			this->myStat.accL1 += 1;
		}
		else{
			this->myStat.missL1 += 1;
			if (search("Victim", adr, &myMem[adr])){
				this->myStat.accVic += 1;
				update("L1", adr, &myMem[adr], DM);
			}
			else{
				this->myStat.missVic += 1;
				if (search("L2", adr, &myMem[adr])){
					this->myStat.accL2 += 1;
					update("L1", adr, &myMem[adr], DM);
				}
				else{
					this->myStat.missL2 += 1;
					update("L1", adr, &myMem[adr], DM);
				}
			}
		}
		
	}

	if (MemW){
		if (search("L1", adr, &myMem[adr])){
			update("L1", adr, &myMem[adr], DM);
			myMem[adr] = *data;
		}else if (search("Victim", adr, &myMem[adr])){
			update("Victim", adr, &myMem[adr], DM);
			myMem[adr] = *data;
			
		}else{
			myMem[adr] = *data;
		}
	}
	// printL1();
	// printVictim();


}


// for debugging purposes
void cache::printL1()
{
	cout << "L1" << endl;
	for (int i=0; i<L1_CACHE_SETS; i++)
	{
		cout << L1[i].data << " | ";
	}
	cout << endl;
}

void cache::printVictim()
{
	cout << "Victim" << endl;
	for (int i=0; i<VICTIM_SIZE; i++)
	{
		cout << Victim[i].data << " pos: " << Victim[i].lru_position << " | ";
	}
	cout << endl;
}