#include "CPU.h"
#include <stdint.h>

instruction::instruction(bitset<32> fetch)
{
	//cout << fetch << endl;
	instr = fetch;
	//cout << instr << endl;
}

// constructor
CPU::CPU()
{
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}

	for (int i = 0; i < 32; i++){
		regs[i] = 0;
	}

	resetController();
}

// set all control values to 0
void CPU::resetController()
{
	controller["Branch"] = 0;
	controller["MemRead"] = 0;
	controller["MemtoReg"] = 0;
	controller["ALUOp"] = 0;
	controller["MemWrite"] = 0;
	controller["ALUSrc"] = 0;
	controller["RegWrite"] = 0;
	controller["Branch"] = 0;
}

// set appropriate control values for each instr
void CPU::setController(instruction* curr){
	if (curr->opcode == bitset<7>(0b0110011) || curr->opcode == bitset<7>(0b0010011)){
		controller["RegWrite"] = 1;
		controller["ALUOp"] = 2;
	}
	if (curr->opcode == bitset<7>(0b0010011) || curr->opcode == bitset<7>(0b0000011) || curr->opcode == bitset<7>(0b0010011)){
		controller["ALUSrc"] = 1;
	}
	if (curr->opcode == bitset<7>(0b0000011)){
		controller["RegWrite"] = 1;
		controller["MemRead"] = 1;
		controller["MemtoReg"] = 1;
	}
	if (curr->opcode == bitset<7>(0b0100011)){
		controller["MemWrite"] = 1;
		controller["ALUSrc"] = 1;
	}
	if (curr->opcode == bitset<7>(0b1100011)){
		controller["Branch"] = 1;
		controller["ALUOp"] = 1;
		
	}
	if (curr->opcode == bitset<7>(0b1100111)){
		controller["Branch"] = 2;
		controller["ALUSrc"] = 1;
		controller["RegWrite"] = 1;
	}

}
// increment PC based on calculated PC offset
// note: nextPCOffset is calculated in nextPCCalc()

bitset<32> CPU::Fetch(bitset<8> *instmem) {
	PC += nextPCOffset;//increment PC
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction

	cerr<< "\n-------------\nPC is: " << PC << endl;
	return instr;
}


bool CPU::Decode(instruction* curr)
{

	// opcode extraction
	for(int i = 0; i <= 6; i++){
		curr->opcode.set(i, curr->instr[i]);
	}

	// reg. extraction
	for (int i = 7; i <= 24; i++){
		if (i >= 7 && i <= 11){
			curr->rd.set(i - 7, curr->instr[i]);
		}
		if (i >= 15 && i <= 19){
			curr->rs1.set(i - 15, curr->instr[i]);
		}
		if (i >= 20 && i <= 24){
			curr->rs2.set(i - 20, curr->instr[i]);
		}
	}
	// funct3 extraction
	for(int i = 12; i <= 14; i++){
			curr->funct3.set(i-12, curr->instr[i]);
	}
	// funct7 extraction
	curr->funct7.set(0, curr->instr[30]);

	// sw immm extraction
	for(int i = 7; i <= 11; i++){
			curr->swImm1.set(i-7, curr->instr[i]);
	}
	for(int i = 25; i <= 31; i++){
			curr->swImm2.set(i-25, curr->instr[i]);
	}
	// sw rs2 extraction
	for(int i = 20; i <= 24; i++){
			curr->swRs2.set(i-20, curr->instr[i]);
	}

	curr->immediate = ImmGen(curr->opcode, curr);
	cerr << "Immeediate: " << curr->immediate << endl;

	
	
	
	return true;
}

bitset<32> CPU::ImmGen(bitset<7> opcode, instruction* curr)
{
	
	// cases for i-type, r-type, sw immediate extraction
	bitset<32> res;
	if (opcode == bitset<7>(0b0010011) || opcode == bitset<7>(0b0000011) || opcode == bitset<7>(0b1100111)){
		bitset<12> immExtract;
		for (int i = 20; i <= 31; i++){
			immExtract.set(i - 20, curr->instr[i]);
		}
		unsigned long bit_extended = immExtract.to_ulong() & 0b00000000000000000000111111111111;

		if (bit_extended & 0b00000000000000000000100000000000) {
        // If number is negative, append
        // leading 1's to the 12-bit sequence
          bit_extended = bit_extended | 0b11111111111111111111000000000000;
		  cerr << "bit extended" << bit_extended << endl;
    	}
		res = bitset<32>(bit_extended);
		
	}
	if (opcode == bitset<7>(0b0100011)){
		

		bitset<12> swImm_combined = bitset<12>(((curr->swImm2.to_ulong() & 0b000001111111) << 5) | curr->swImm1.to_ulong());
		
		
		unsigned long bit_extended = swImm_combined.to_ulong() & 0b00000000000000000000111111111111;

		if (bit_extended & 0b10000000000000000000000000000000) {
        // If number is negative, append
        // leading 1's to the 12-bit sequence
          bit_extended = bit_extended | 0b11111111111111111111000000000000;
    	}
		res = bitset<32>(bit_extended);
	}
	if (opcode == bitset<7>(0b1100011)){
		if (curr->funct3 == bitset<3>(0b100)){
			bitset<5> brImm1 = curr->swImm1;
			bitset<7> brImm2 = curr->swImm2;

			bitset<12> brImm_combined = bitset<12>(((brImm2.to_ulong() & 0b000001111111) << 5) | brImm1.to_ulong());
		
		
			unsigned long bit_extended = brImm_combined.to_ulong() & 0b00000000000000000000111111111111;

			if (bit_extended & 0b10000000000000000000000000000000) {
			// If number is negative, append
			// leading 1's to the 12-bit sequence
				bit_extended = bit_extended | 0b11111111111111111111000000000000;
			}
			res = bitset<32>(bit_extended);

		}
	}
	cerr << "res" << res << endl;
	return res;
}

int CPU::Execute(instruction* curr)
{
	// return the result of ALU
	int ALURes;

	if (controller["ALUSrc"]){
		if (controller["Branch"] == 2){
			// cout << "jalr: rs1 + imm" << endl;;
		}
		cerr << "immediate ulong casted to int32_t: " << (int32_t)(curr->immediate.to_ulong()) << endl;
		ALURes = ALU(regs[(curr->rs1).to_ulong()], (int32_t)(curr->immediate.to_ulong()), controller["ALUOp"], curr);
		
	}
	else{
		ALURes = ALU(regs[(curr->rs1).to_ulong()], regs[(curr->rs2).to_ulong()], controller["ALUOp"], curr);
	}
	return ALURes;
}

int CPU::ALU(int rs1, int rs2, int op, instruction* curr){
	// ALU logic
	if(op == 2){
		// funct3 extraction	
		if (curr->funct3 == bitset<3>(0b111)){
			return rs1 & rs2;
		}
		else if (curr->funct3 == bitset<3>(0b110)){
			return rs1 | rs2;
		}
		else if (curr->funct3 == bitset<3>(0b100)){
			return rs1 ^ rs2;
		}
		else if (curr->funct3 == bitset<3>(0b101)){

			return rs1 >> rs2;
		}
		else if (curr->opcode == bitset<7>(0b0010011)){
			return rs1 + rs2;
		}
		else{
			if (curr->funct7.to_ulong()){
				cerr << "rs1: " << rs1 << " rs2: " << rs2 << endl;
				return rs1 - rs2;
			}else{
				
				return rs1 + rs2;
			}
		}

	}
	else if (op == 0){
		return rs1 + rs2;
	}
	else if (op == 1){
		
		
		return rs1 - rs2;
	}
	return 0;
}


int32_t CPU::MemRW(int addr, instruction* curr)
{
	// Memory read/write control
	if (controller["MemRead"]){
		cerr << "reading at addr: " << addr << " and val is: " <<  (int32_t) (dmemory[addr]) << endl;
		return (int32_t) (dmemory[addr]);
	}
	if (controller["MemWrite"]){
		cerr << "storing at addr: " << addr << " and val is: " <<  (int32_t)(regs[(curr->swRs2).to_ulong()]) << endl;

		dmemory[addr] = (int32_t)(regs[(curr->swRs2).to_ulong()]);
		
		
	}
	return 0;
}
void CPU::WriteBack(instruction* curr, int ALURes, int MemData)
{
	// write back to reg. contents/whether to write depends on control signals
	int dest = (curr->rd).to_ulong();	
	int muxRes;
	if (controller["MemtoReg"]){
		
		muxRes = MemData;
	}
	else if (controller["Branch"] == 2){
		muxRes = PC + 4;
	}
	else{
		muxRes = ALURes;
	}

	if (controller["RegWrite"]){

		regs[dest] = muxRes;
	}

	for(int i = 0; i < 20; i++){
		cerr << "reg " << i << ": " << regs[i] << endl; 
	}
	
	
}

void CPU::nextPCCalc(int ALURes, instruction* curr)
{
	// calculate next PC address based on branch instr.
	int Offset = 4;
	if (controller["Branch"] == 1 && ALURes < 0){

		Offset = (int)((curr->immediate).to_ulong());

	}else if (controller["Branch"] == 2){
		
		
		Offset = (-1 * PC) + (ALURes);
	}
	nextPCOffset = Offset;
}

unsigned long CPU::readPC()
{
	return PC;
}
