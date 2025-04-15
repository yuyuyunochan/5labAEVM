#ifndef MYSIMPLECOMPUTER_H
#define MYSIMPLECOMPUTER_H

#define MEMORY_SIZE 128
#define MAX_VALUE 16384
#define MIN_VALUE -16384

#define OVERFLOW 0
#define DIVISION_ERR_BY_ZERO 1
#define OUT_OF_MEMORY 2
#define IGNORING_TACT_PULSES 3
#define INCORRECT_COMMAND 4

#define CMD_NOP 0x00
#define CMD_CPUINFO 0x01
#define CMD_READ 0x0A
#define CMD_WRITE 0x0B
#define CMD_LOAD 0x14
#define CMD_STORE 0x15
#define CMD_ADD 0x1E
#define CMD_SUB 0x1F
#define CMD_DIVIDE 0x20
#define CMD_MUL 0x21
#define CMD_JUMP 0x28
#define CMD_JNEG 0x29
#define CMD_JZ 0x2A
#define CMD_HALT 0x2D
#define CMD_NOT 0x33
#define CMD_AND 0x34
#define CMD_OR 0x35
#define CMD_XOR 0x36
#define CMD_JNS 0x37
#define CMD_JC 0x38
#define CMD_JNC 0x39
#define CMD_JP 0x3A
#define CMD_NP 0x3B
#define CMD_CHL 0x3C
#define CMD_SHR 0x3D
#define CMD_RCL 0x3E
#define CMD_RCR 0x3F
#define CMD_NEG 0x40
#define CMD_ADDC 0x41
#define CMD_SUBC 0x42
#define CMD_LOGLC 0x43
#define CMD_LOGRC 0x44
#define CMD_RCCL 0x45
#define CMD_RCCR 0x46
#define CMD_MOVA 0x47
#define CMD_MOVR 0x48
#define CMD_MOVCA 0x49
#define CMD_MOVCR 0x4A
#define CMD_ADDC2 0x4B
#define CMD_SUBC2 0x4C

extern int SC_REG_SIZE;

extern int memory[MEMORY_SIZE];

extern int accumulator;
extern int icounter;
extern int flags;

int sc_memoryInit (void);
int sc_memorySet (int address, int value);
int sc_memoryGet (int address, int *value);
int sc_memorySave (const char *filename);
int sc_memoryLoad (const char *filename);

#define FLAG_ZERO (1 << 0)
#define FLAG_SIGN (1 << 1)
#define FLAG_CARRY (1 << 2)

int sc_regInit (void);
int sc_regSet (int register_mask, int value);
int sc_regGet (int register_mask, int *value);
int sc_accumulatorInit (void);
int sc_accumulatorSet (int value);
int sc_accumulatorGet (int *value);
int sc_icounterInit (void);
int sc_icounterSet (int value);
int sc_icounterGet (int *value);

int sc_commandEncode (int sign, int command, int operand, int *value);
int sc_commandDecode (int value, int *sign, int *command, int *operand);
int sc_commandValidate (int command);

#endif
