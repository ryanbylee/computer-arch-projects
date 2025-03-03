#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
using namespace std;


class instruction {
public:
	bitset<32> instr;//instruction
	instruction(bitset<32> fetch); // constructor
	bitset<7> opcode;
	bitset<5> rs1;
	bitset<5> rs2;
	bitset<5> rd;
	bitset<32> immediate;
	bitset<32> branchImm;
	bitset<3> funct3;
	bitset<1> funct7;
	bitset<5> swImm1;
	bitset<7> swImm2;
	bitset<5> swRs2;

};

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc
	map<string, int> controller;
	int nextPCOffset;

public:
	CPU();
	int regs[32];
	unsigned long readPC();
	bitset<32> Fetch(bitset<8> *instmem);
	bool Decode(instruction* instr);
	bitset<32> ImmGen(bitset<7> opcode, instruction* curr);
	void resetController();
	int Execute(instruction* instr);
	int ALU(int rs1, int rs2, int op, instruction* curr);
	void setController(instruction* curr);

	int32_t MemRW(int addr, instruction* curr);
	void WriteBack(instruction* curr, int ALURes, int MemData);
	void nextPCCalc(int ALURes, instruction* curr);
};

// add other functions and objects here
