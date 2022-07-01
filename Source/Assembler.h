#pragma once 

#include <stdint.h>
#include <cstring>
#include <iostream>

struct instruction{
    const char* name;
    //R -> 0
    //I -> 1
    //j -> 2
    //FR -> 3
    //FI -> 4
    uint8_t format;
    uint8_t opcode;
    uint8_t funct;
    uint8_t FMT;
    uint8_t FT;
};

const instruction instructions[] = {
    //name for op fnct   fmt   ft
    {"add", 0, 0, 0x20, 0, 0}, {"addi", 1, 0x08, 0, 0, 0}, {"addiu", 1, 0x09, 0, 0, 0},
    {"addu", 0, 0, 0x21, 0, 0}, {"and", 0, 0, 0x24, 0, 0}, {"andi", 1, 0x0C, 0, 0, 0},
    {"beq", 1, 0x04, 0, 0, 0}, {"bne", 1, 0x05, 0, 0, 0}, {"j", 2, 0x02, 0, 0, 0},
    {"jal", 2, 0x03, 0, 0, 0}, {"jr", 0, 0, 0x08, 0, 0}, {"lbu", 1, 0x24, 0, 0, 0},
    {"lhu", 1, 0x25, 0, 0, 0}, {"ll", 1, 0x30, 0, 0, 0}, {"lui", 1, 0x0F, 0, 0, 0},
    {"lw", 1, 0x23, 0, 0, 0}, {"nor", 0, 0, 0x27, 0, 0}, {"or", 0, 0, 0x25, 0, 0},
    {"ori", 1, 0x0D, 0, 0, 0}, {"slt", 0, 0, 0x2a, 0, 0}, {"slti", 1, 0x0A, 0, 0, 0},
    {"sltiu", 1, 0x0B, 0, 0, 0}, {"sltu", 0, 0, 0x2B, 0, 0}, {"sll", 0, 0, 0x00, 0, 0},
    {"srl", 0, 0, 0x02, 0, 0}, {"sb", 1, 0x28, 0, 0, 0}, {"sc", 1, 0x38, 0, 0, 0},
    {"sh", 1, 0x29, 0, 0, 0}, {"sw", 1, 0x2b, 0, 0, 0}, {"sub", 0, 0, 0x22, 0, 0},
    {"subu", 0, 0, 0x23, 0, 0}, {"bc1t", 4, 0x11, 0, 0x08, 0x01}, {"bc1f", 4, 0x11, 0, 0x08, 0},
    {"div", 0, 0, 0x1A, 0, 0}, {"divu", 0, 0, 0x1B, 0, 0}, {"add.s", 3, 0x11, 0, 0x10, 0},
    {"add.d", 3, 0x11, 0, 0x11, 0}, {"c.eq.s", 3, 0x11, 0x32, 0x10, 0}, {"c.lt.s", 3, 0x11, 0x3C, 0x10, 0},
    {"c.le.s", 3, 0x11, 0x3E, 0x10, 0}, {"c.eq.d", 3, 0x11, 0x32, 0x11, 0}, {"c.lt.d", 3, 0x11, 0x3C, 0x11, 0},
    {"c.le.d", 3, 0x11, 0x3E, 0x11, 0}, {"div.s", 3, 0x11, 0x03, 0x10, 0}, {"div.d", 3, 0x11, 0x03, 0x11, 0},
    {"mul.s", 3, 0x11, 0x02, 0x10, 0}, {"mul.d", 3, 0x11, 0x02, 0x11, 0}, {"mul.d", 3, 0x11, 0x02, 0x11, 0},
    {"sub.s", 3, 0x11, 0x01, 0x10, 0}, {"sub.d", 3, 0x11, 0x01, 0x11, 0}, {"lwc1", 1, 0x31, 0, 0, 0},
    {"ldc1", 1, 0x35, 0, 0, 0}, {"mfhi", 0, 0, 0x10, 0, 0}, {"mflo", 0, 0, 0x12, 0, 0},
    {"mfc0", 0, 0x10, 0, 0, 0}, {"mult", 0, 0, 0x18, 0, 0}, {"multu", 0, 0, 0x19, 0, 0},
    {"sra", 0, 0, 0x03, 0, 0}, {"swc1", 1, 0x39, 0, 0, 0}, {"sdc1", 1, 0x3D, 0, 0, 0}
};

const uint8_t instructionNum = sizeof(instructions) / sizeof(instructions[0]);

struct regVal{
    const char *name;
    uint8_t val;
};

struct label{
    char *name;
    uint32_t location;
    struct label* next;
};

const regVal registers[] = {
    //Regular MIPS register names
    {"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3},
    {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7}, {"$t0", 8},
    {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13},
    {"$t6", 14}, {"$t7", 15}, {"$s0", 16}, {"$s1", 17}, {"$s2", 18},
    {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23},
    {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27}, {"$gp", 28},
    {"$sp", 29}, {"$fp", 30},{"$ra", 31}, 
    
    //Register by numbers
    {"$0", 0}, {"$1", 1},{"$2", 2}, {"$3", 3}, {"$4", 4}, {"$5", 5}, 
    {"$6", 6}, {"$7", 7}, {"$8", 8}, {"$9", 9}, {"$10", 10}, {"$11", 11}, 
    {"$12", 12}, {"$13", 13}, {"$14", 14}, {"$15", 15}, {"$16", 16}, 
    {"$17", 17}, {"$18", 18}, {"$19", 19}, {"$20", 20}, {"$21", 21}, 
    {"$22", 22}, {"$23", 23}, {"$24", 24}, {"$25", 26}, {"$27", 27}, 
    {"$28", 28}, {"$29", 29}, {"$30", 30}, {"$31", 31}, 
    
    //Floating Point Registers
    {"$f0", 0}, {"$f1", 1},{"$f2", 2},{"$f3", 3}, {"$f4", 4}, {"$f5", 5}, 
    {"$f6", 6}, {"$f7", 7}, {"$f8", 8}, {"$f9", 9}, {"$f10", 10}, {"$f11", 11}, 
    {"$f12", 12}, {"$f13", 13}, {"$f14", 14}, {"$f15", 15}, {"$f16", 16}, 
    {"$f17", 17}, {"$f18", 18}, {"$f19", 19}, {"$f20", 20}, {"$f21", 21}, 
    {"$f22", 22}, {"$f23", 23}, {"$f24", 24}, {"$f25", 26}, {"$f27", 27}, 
    {"$f28", 28}, {"$f29", 29}, {"$f30", 30}, {"$f31", 31}
};

const uint8_t regNum = sizeof(registers) / sizeof(registers[0]);

const int lineSize = 30;
const int tokenSize = 12;
const int maxTokens = 4;

//Dissassembler Functions
char** disassemble(uint32_t* instructionList, uint8_t length);
uint8_t instructionLookup(uint8_t opcode, uint8_t funct, uint8_t fmt, uint8_t ft);
uint8_t regLookup(uint8_t num, bool fType); //False for reg true for floating point
char* rTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t rs, uint8_t rt);
char* iTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t rs, uint8_t rt);
char* jTypeDis(uint32_t instruction, uint8_t opcodeIndex);
char* frTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t fmt, uint8_t ft, uint8_t funct);
char* fiTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t fmt, uint8_t ft);