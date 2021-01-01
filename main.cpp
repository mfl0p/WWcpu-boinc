#define MAJORV 1
#define MINORV 0
#define SUFFIXV "4"

#include <iostream>
#include <cinttypes>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>

#include <pthread.h>
#include <thread>

const uint64_t ckpt_range = 600000000ULL;
#define thread_range 10000000

#include "boinc_api.h"
#include "filesys.h"

#include "prime.h"
#include "WW.h"

#define RESULTS_FILENAME "results-WW.txt"
#define STATE_FILENAME_A "stateA-WW.txt"
#define STATE_FILENAME_B "stateB-WW.txt"

using namespace std; 


/* shared data between threads */
uint64_t * hostSpecialPrime;
int32_t * hostResult;
int32_t * hostRem;
int32_t * hostQuot;

/////
uint64_t current_start;
pthread_mutex_t lock1;
/////

/////
int32_t resultcount;
pthread_mutex_t lock2;
/////

/////
//uint64_t totalprimecount;
uint64_t totalchecksum;
pthread_mutex_t lock3;
/////

typedef struct _thread_data_t {
	int id;
	uint64_t end;
} thread_data_t;


const uint64_t p7[7] = { 0x8102040810204081, 0x810204081020408, 0x4081020408102040, 0x408102040810204, 0x2040810204081020, 0x204081020408102, 0x1020408102040810 };

const uint64_t p11[11] = { 0x80100200400801, 0x1002004008010020, 0x40080100200400, 0x801002004008010, 0x20040080100200, 0x400801002004008, 0x8010020040080100, 0x200400801002004, 0x4008010020040080, 0x100200400801002, 0x2004008010020040 };

const uint64_t p13[13] = { 0x10008004002001, 0x400200100080040, 0x8004002001000, 0x200100080040020, 0x8004002001000800, 0x100080040020010, 0x4002001000800400, 0x80040020010008, 0x2001000800400200, 0x40020010008004, 0x1000800400200100, 0x20010008004002, 0x800400200100080 };

const uint64_t p17[17] = { 0x8000400020001, 0x800040002000100, 0x4000200010000, 0x400020001000080, 0x2000100008000, 0x200010000800040, 0x1000080004000, 0x100008000400020, 0x800040002000, 0x80004000200010, 0x8000400020001000, 0x40002000100008, 0x4000200010000800, 0x20001000080004, 0x2000100008000400, 0x10000800040002, 0x1000080004000200 };

const uint64_t p19[19] = { 0x200004000080001, 0x800010000200, 0x100002000040000, 0x400008000100, 0x80001000020000, 0x200004000080, 0x40000800010000, 0x8000100002000040, 0x20000400008000, 0x4000080001000020, 0x10000200004000, 0x2000040000800010, 0x8000100002000, 0x1000020000400008, 0x4000080001000, 0x800010000200004, 0x2000040000800, 0x400008000100002, 0x1000020000400 };

const uint64_t p23[23] = { 0x400000800001, 0x200000400000800, 0x200000400000, 0x100000200000400, 0x100000200000, 0x80000100000200, 0x80000100000, 0x40000080000100, 0x40000080000, 0x20000040000080, 0x20000040000, 0x10000020000040, 0x8000010000020000, 0x8000010000020, 0x4000008000010000, 0x4000008000010, 0x2000004000008000, 0x2000004000008, 0x1000002000004000, 0x1000002000004, 0x800001000002000, 0x800001000002, 0x400000800001000 };

const uint64_t p29[29] = { 0x400000020000001, 0x80000004000, 0x200000010000000, 0x40000002000, 0x100000008000000, 0x20000001000, 0x80000004000000, 0x10000000800, 0x40000002000000, 0x8000000400, 0x20000001000000, 0x4000000200, 0x10000000800000, 0x2000000100, 0x8000000400000, 0x1000000080, 0x4000000200000, 0x800000040, 0x2000000100000, 0x8000000400000020, 0x1000000080000, 0x4000000200000010, 0x800000040000, 0x2000000100000008, 0x400000020000, 0x1000000080000004, 0x200000010000, 0x800000040000002, 0x100000008000 };

const uint64_t p31[31] = { 0x4000000080000001, 0x400000008000, 0x2000000040000000, 0x200000004000, 0x1000000020000000, 0x100000002000, 0x800000010000000, 0x80000001000, 0x400000008000000, 0x40000000800, 0x200000004000000, 0x20000000400, 0x100000002000000, 0x10000000200, 0x80000001000000, 0x8000000100, 0x40000000800000, 0x4000000080, 0x20000000400000, 0x2000000040, 0x10000000200000, 0x1000000020, 0x8000000100000, 0x800000010, 0x4000000080000, 0x400000008, 0x2000000040000, 0x200000004, 0x1000000020000, 0x8000000100000002, 0x800000010000 };

const uint64_t p37[37] = { 0x2000000001, 0x80000000040000, 0x1000000000, 0x40000000020000, 0x800000000, 0x20000000010000, 0x400000000, 0x10000000008000, 0x200000000, 0x8000000004000, 0x100000000, 0x4000000002000, 0x80000000, 0x2000000001000, 0x40000000, 0x1000000000800, 0x20000000, 0x800000000400, 0x10000000, 0x400000000200, 0x8000000, 0x200000000100, 0x8000000004000000, 0x100000000080, 0x4000000002000000, 0x80000000040, 0x2000000001000000, 0x40000000020, 0x1000000000800000, 0x20000000010, 0x800000000400000, 0x10000000008, 0x400000000200000, 0x8000000004, 0x200000000100000, 0x4000000002, 0x100000000080000 };

const uint64_t p41[41] = { 0x20000000001, 0x2000000000100000, 0x10000000000, 0x1000000000080000, 0x8000000000, 0x800000000040000, 0x4000000000, 0x400000000020000, 0x2000000000, 0x200000000010000, 0x1000000000, 0x100000000008000, 0x800000000, 0x80000000004000, 0x400000000, 0x40000000002000, 0x200000000, 0x20000000001000, 0x100000000, 0x10000000000800, 0x80000000, 0x8000000000400, 0x40000000, 0x4000000000200, 0x20000000, 0x2000000000100, 0x10000000, 0x1000000000080, 0x8000000, 0x800000000040, 0x4000000, 0x400000000020, 0x2000000, 0x200000000010, 0x1000000, 0x100000000008, 0x800000, 0x80000000004, 0x8000000000400000, 0x40000000002, 0x4000000000200000 };

const uint64_t p43[43] = { 0x80000000001, 0x200000, 0x40000000000, 0x8000000000100000, 0x20000000000, 0x4000000000080000, 0x10000000000, 0x2000000000040000, 0x8000000000, 0x1000000000020000, 0x4000000000, 0x800000000010000, 0x2000000000, 0x400000000008000, 0x1000000000, 0x200000000004000, 0x800000000, 0x100000000002000, 0x400000000, 0x80000000001000, 0x200000000, 0x40000000000800, 0x100000000, 0x20000000000400, 0x80000000, 0x10000000000200, 0x40000000, 0x8000000000100, 0x20000000, 0x4000000000080, 0x10000000, 0x2000000000040, 0x8000000, 0x1000000000020, 0x4000000, 0x800000000010, 0x2000000, 0x400000000008, 0x1000000, 0x200000000004, 0x800000, 0x100000000002, 0x400000 };

const uint64_t p47[47] = { 0x800000000001, 0x800000, 0x400000000000, 0x400000, 0x200000000000, 0x200000, 0x100000000000, 0x100000, 0x80000000000, 0x80000, 0x40000000000, 0x40000, 0x20000000000, 0x20000, 0x10000000000, 0x8000000000010000, 0x8000000000, 0x4000000000008000, 0x4000000000, 0x2000000000004000, 0x2000000000, 0x1000000000002000, 0x1000000000, 0x800000000001000, 0x800000000, 0x400000000000800, 0x400000000, 0x200000000000400, 0x200000000, 0x100000000000200, 0x100000000, 0x80000000000100, 0x80000000, 0x40000000000080, 0x40000000, 0x20000000000040, 0x20000000, 0x10000000000020, 0x10000000, 0x8000000000010, 0x8000000, 0x4000000000008, 0x4000000, 0x2000000000004, 0x2000000, 0x1000000000002, 0x1000000 };

const uint64_t p53[53] = { 0x20000000000001, 0x4000000, 0x10000000000000, 0x2000000, 0x8000000000000, 0x1000000, 0x4000000000000, 0x800000, 0x2000000000000, 0x400000, 0x1000000000000, 0x200000, 0x800000000000, 0x100000, 0x400000000000, 0x80000, 0x200000000000, 0x40000, 0x100000000000, 0x20000, 0x80000000000, 0x10000, 0x40000000000, 0x8000, 0x20000000000, 0x4000, 0x10000000000, 0x2000, 0x8000000000, 0x1000, 0x4000000000, 0x800, 0x2000000000, 0x8000000000000400, 0x1000000000, 0x4000000000000200, 0x800000000, 0x2000000000000100, 0x400000000, 0x1000000000000080, 0x200000000, 0x800000000000040, 0x100000000, 0x400000000000020, 0x80000000, 0x200000000000010, 0x40000000, 0x100000000000008, 0x20000000, 0x80000000000004, 0x10000000, 0x40000000000002, 0x8000000 };

const uint64_t p59[59] = { 0x800000000000001, 0x20000000, 0x400000000000000, 0x10000000, 0x200000000000000, 0x8000000, 0x100000000000000, 0x4000000, 0x80000000000000, 0x2000000, 0x40000000000000, 0x1000000, 0x20000000000000, 0x800000, 0x10000000000000, 0x400000, 0x8000000000000, 0x200000, 0x4000000000000, 0x100000, 0x2000000000000, 0x80000, 0x1000000000000, 0x40000, 0x800000000000, 0x20000, 0x400000000000, 0x10000, 0x200000000000, 0x8000, 0x100000000000, 0x4000, 0x80000000000, 0x2000, 0x40000000000, 0x1000, 0x20000000000, 0x800, 0x10000000000, 0x400, 0x8000000000, 0x200, 0x4000000000, 0x100, 0x2000000000, 0x80, 0x1000000000, 0x40, 0x800000000, 0x20, 0x400000000, 0x8000000000000010, 0x200000000, 0x4000000000000008, 0x100000000, 0x2000000000000004, 0x80000000, 0x1000000000000002, 0x40000000 };

const uint64_t p61[61] = { 0x2000000000000001, 0x40000000, 0x1000000000000000, 0x20000000, 0x800000000000000, 0x10000000, 0x400000000000000, 0x8000000, 0x200000000000000, 0x4000000, 0x100000000000000, 0x2000000, 0x80000000000000, 0x1000000, 0x40000000000000, 0x800000, 0x20000000000000, 0x400000, 0x10000000000000, 0x200000, 0x8000000000000, 0x100000, 0x4000000000000, 0x80000, 0x2000000000000, 0x40000, 0x1000000000000, 0x20000, 0x800000000000, 0x10000, 0x400000000000, 0x8000, 0x200000000000, 0x4000, 0x100000000000, 0x2000, 0x80000000000, 0x1000, 0x40000000000, 0x800, 0x20000000000, 0x400, 0x10000000000, 0x200, 0x8000000000, 0x100, 0x4000000000, 0x80, 0x2000000000, 0x40, 0x1000000000, 0x20, 0x800000000, 0x10, 0x400000000, 0x8, 0x200000000, 0x8000000000000004, 0x100000000, 0x4000000000000002, 0x80000000 };

const uint64_t p67[67] = { 0x1, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x8000000000000000, 0x20000000, 0x4000000000000000, 0x10000000, 0x2000000000000000, 0x8000000, 0x1000000000000000, 0x4000000, 0x800000000000000, 0x2000000, 0x400000000000000, 0x1000000, 0x200000000000000, 0x800000, 0x100000000000000, 0x400000, 0x80000000000000, 0x200000, 0x40000000000000, 0x100000, 0x20000000000000, 0x80000, 0x10000000000000, 0x40000, 0x8000000000000, 0x20000, 0x4000000000000, 0x10000, 0x2000000000000, 0x8000, 0x1000000000000, 0x4000, 0x800000000000, 0x2000, 0x400000000000, 0x1000, 0x200000000000, 0x800, 0x100000000000, 0x400, 0x80000000000, 0x200, 0x40000000000, 0x100, 0x20000000000, 0x80, 0x10000000000, 0x40, 0x8000000000, 0x20, 0x4000000000, 0x10, 0x2000000000, 0x8, 0x1000000000, 0x4, 0x800000000, 0x2, 0x400000000 };

const uint64_t p71[71] = { 0x1, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x8000000000000000, 0x8000000, 0x4000000000000000, 0x4000000, 0x2000000000000000, 0x2000000, 0x1000000000000000, 0x1000000, 0x800000000000000, 0x800000, 0x400000000000000, 0x400000, 0x200000000000000, 0x200000, 0x100000000000000, 0x100000, 0x80000000000000, 0x80000, 0x40000000000000, 0x40000, 0x20000000000000, 0x20000, 0x10000000000000, 0x10000, 0x8000000000000, 0x8000, 0x4000000000000, 0x4000, 0x2000000000000, 0x2000, 0x1000000000000, 0x1000, 0x800000000000, 0x800, 0x400000000000, 0x400, 0x200000000000, 0x200, 0x100000000000, 0x100, 0x80000000000, 0x80, 0x40000000000, 0x40, 0x20000000000, 0x20, 0x10000000000, 0x10, 0x8000000000, 0x8, 0x4000000000, 0x4, 0x2000000000, 0x2, 0x1000000000 };

const uint64_t p73[73] = { 0x1, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x8000000000000000, 0x4000000, 0x4000000000000000, 0x2000000, 0x2000000000000000, 0x1000000, 0x1000000000000000, 0x800000, 0x800000000000000, 0x400000, 0x400000000000000, 0x200000, 0x200000000000000, 0x100000, 0x100000000000000, 0x80000, 0x80000000000000, 0x40000, 0x40000000000000, 0x20000, 0x20000000000000, 0x10000, 0x10000000000000, 0x8000, 0x8000000000000, 0x4000, 0x4000000000000, 0x2000, 0x2000000000000, 0x1000, 0x1000000000000, 0x800, 0x800000000000, 0x400, 0x400000000000, 0x200, 0x200000000000, 0x100, 0x100000000000, 0x80, 0x80000000000, 0x40, 0x40000000000, 0x20, 0x20000000000, 0x10, 0x10000000000, 0x8, 0x8000000000, 0x4, 0x4000000000, 0x2, 0x2000000000 };

const uint64_t p79[79] = { 0x1, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x8000000000000000, 0x800000, 0x4000000000000000, 0x400000, 0x2000000000000000, 0x200000, 0x1000000000000000, 0x100000, 0x800000000000000, 0x80000, 0x400000000000000, 0x40000, 0x200000000000000, 0x20000, 0x100000000000000, 0x10000, 0x80000000000000, 0x8000, 0x40000000000000, 0x4000, 0x20000000000000, 0x2000, 0x10000000000000, 0x1000, 0x8000000000000, 0x800, 0x4000000000000, 0x400, 0x2000000000000, 0x200, 0x1000000000000, 0x100, 0x800000000000, 0x80, 0x400000000000, 0x40, 0x200000000000, 0x20, 0x100000000000, 0x10, 0x80000000000, 0x8, 0x40000000000, 0x4, 0x20000000000, 0x2, 0x10000000000 };

const uint64_t p83[83] = { 0x1, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x8000000000000000, 0x200000, 0x4000000000000000, 0x100000, 0x2000000000000000, 0x80000, 0x1000000000000000, 0x40000, 0x800000000000000, 0x20000, 0x400000000000000, 0x10000, 0x200000000000000, 0x8000, 0x100000000000000, 0x4000, 0x80000000000000, 0x2000, 0x40000000000000, 0x1000, 0x20000000000000, 0x800, 0x10000000000000, 0x400, 0x8000000000000, 0x200, 0x4000000000000, 0x100, 0x2000000000000, 0x80, 0x1000000000000, 0x40, 0x800000000000, 0x20, 0x400000000000, 0x10, 0x200000000000, 0x8, 0x100000000000, 0x4, 0x80000000000, 0x2, 0x40000000000 };

const uint64_t p89[89] = { 0x1, 0x100000000000, 0x0, 0x80000000000, 0x0, 0x40000000000, 0x0, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x0, 0x200000, 0x0, 0x100000, 0x0, 0x80000, 0x8000000000000000, 0x40000, 0x4000000000000000, 0x20000, 0x2000000000000000, 0x10000, 0x1000000000000000, 0x8000, 0x800000000000000, 0x4000, 0x400000000000000, 0x2000, 0x200000000000000, 0x1000, 0x100000000000000, 0x800, 0x80000000000000, 0x400, 0x40000000000000, 0x200, 0x20000000000000, 0x100, 0x10000000000000, 0x80, 0x8000000000000, 0x40, 0x4000000000000, 0x20, 0x2000000000000, 0x10, 0x1000000000000, 0x8, 0x800000000000, 0x4, 0x400000000000, 0x2, 0x200000000000 };

const uint64_t p97[97] = { 0x1, 0x1000000000000, 0x0, 0x800000000000, 0x0, 0x400000000000, 0x0, 0x200000000000, 0x0, 0x100000000000, 0x0, 0x80000000000, 0x0, 0x40000000000, 0x0, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x0, 0x200000, 0x0, 0x100000, 0x0, 0x80000, 0x0, 0x40000, 0x0, 0x20000, 0x0, 0x10000, 0x0, 0x8000, 0x8000000000000000, 0x4000, 0x4000000000000000, 0x2000, 0x2000000000000000, 0x1000, 0x1000000000000000, 0x800, 0x800000000000000, 0x400, 0x400000000000000, 0x200, 0x200000000000000, 0x100, 0x100000000000000, 0x80, 0x80000000000000, 0x40, 0x40000000000000, 0x20, 0x20000000000000, 0x10, 0x10000000000000, 0x8, 0x8000000000000, 0x4, 0x4000000000000, 0x2, 0x2000000000000 };

const uint64_t p101[101] = { 0x1, 0x4000000000000, 0x0, 0x2000000000000, 0x0, 0x1000000000000, 0x0, 0x800000000000, 0x0, 0x400000000000, 0x0, 0x200000000000, 0x0, 0x100000000000, 0x0, 0x80000000000, 0x0, 0x40000000000, 0x0, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x0, 0x200000, 0x0, 0x100000, 0x0, 0x80000, 0x0, 0x40000, 0x0, 0x20000, 0x0, 0x10000, 0x0, 0x8000, 0x0, 0x4000, 0x0, 0x2000, 0x8000000000000000, 0x1000, 0x4000000000000000, 0x800, 0x2000000000000000, 0x400, 0x1000000000000000, 0x200, 0x800000000000000, 0x100, 0x400000000000000, 0x80, 0x200000000000000, 0x40, 0x100000000000000, 0x20, 0x80000000000000, 0x10, 0x40000000000000, 0x8, 0x20000000000000, 0x4, 0x10000000000000, 0x2, 0x8000000000000 };

const uint64_t p103[103] = { 0x1, 0x8000000000000, 0x0, 0x4000000000000, 0x0, 0x2000000000000, 0x0, 0x1000000000000, 0x0, 0x800000000000, 0x0, 0x400000000000, 0x0, 0x200000000000, 0x0, 0x100000000000, 0x0, 0x80000000000, 0x0, 0x40000000000, 0x0, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x0, 0x200000, 0x0, 0x100000, 0x0, 0x80000, 0x0, 0x40000, 0x0, 0x20000, 0x0, 0x10000, 0x0, 0x8000, 0x0, 0x4000, 0x0, 0x2000, 0x0, 0x1000, 0x8000000000000000, 0x800, 0x4000000000000000, 0x400, 0x2000000000000000, 0x200, 0x1000000000000000, 0x100, 0x800000000000000, 0x80, 0x400000000000000, 0x40, 0x200000000000000, 0x20, 0x100000000000000, 0x10, 0x80000000000000, 0x8, 0x40000000000000, 0x4, 0x20000000000000, 0x2, 0x10000000000000 };

const uint64_t p107[107] = { 0x1, 0x20000000000000, 0x0, 0x10000000000000, 0x0, 0x8000000000000, 0x0, 0x4000000000000, 0x0, 0x2000000000000, 0x0, 0x1000000000000, 0x0, 0x800000000000, 0x0, 0x400000000000, 0x0, 0x200000000000, 0x0, 0x100000000000, 0x0, 0x80000000000, 0x0, 0x40000000000, 0x0, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x0, 0x200000, 0x0, 0x100000, 0x0, 0x80000, 0x0, 0x40000, 0x0, 0x20000, 0x0, 0x10000, 0x0, 0x8000, 0x0, 0x4000, 0x0, 0x2000, 0x0, 0x1000, 0x0, 0x800, 0x0, 0x400, 0x8000000000000000, 0x200, 0x4000000000000000, 0x100, 0x2000000000000000, 0x80, 0x1000000000000000, 0x40, 0x800000000000000, 0x20, 0x400000000000000, 0x10, 0x200000000000000, 0x8, 0x100000000000000, 0x4, 0x80000000000000, 0x2, 0x40000000000000 };

const uint64_t p109[109] = { 0x1, 0x40000000000000, 0x0, 0x20000000000000, 0x0, 0x10000000000000, 0x0, 0x8000000000000, 0x0, 0x4000000000000, 0x0, 0x2000000000000, 0x0, 0x1000000000000, 0x0, 0x800000000000, 0x0, 0x400000000000, 0x0, 0x200000000000, 0x0, 0x100000000000, 0x0, 0x80000000000, 0x0, 0x40000000000, 0x0, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x0, 0x200000, 0x0, 0x100000, 0x0, 0x80000, 0x0, 0x40000, 0x0, 0x20000, 0x0, 0x10000, 0x0, 0x8000, 0x0, 0x4000, 0x0, 0x2000, 0x0, 0x1000, 0x0, 0x800, 0x0, 0x400, 0x0, 0x200, 0x8000000000000000, 0x100, 0x4000000000000000, 0x80, 0x2000000000000000, 0x40, 0x1000000000000000, 0x20, 0x800000000000000, 0x10, 0x400000000000000, 0x8, 0x200000000000000, 0x4, 0x100000000000000, 0x2, 0x80000000000000 };

const uint64_t p113[113] = { 0x1, 0x100000000000000, 0x0, 0x80000000000000, 0x0, 0x40000000000000, 0x0, 0x20000000000000, 0x0, 0x10000000000000, 0x0, 0x8000000000000, 0x0, 0x4000000000000, 0x0, 0x2000000000000, 0x0, 0x1000000000000, 0x0, 0x800000000000, 0x0, 0x400000000000, 0x0, 0x200000000000, 0x0, 0x100000000000, 0x0, 0x80000000000, 0x0, 0x40000000000, 0x0, 0x20000000000, 0x0, 0x10000000000, 0x0, 0x8000000000, 0x0, 0x4000000000, 0x0, 0x2000000000, 0x0, 0x1000000000, 0x0, 0x800000000, 0x0, 0x400000000, 0x0, 0x200000000, 0x0, 0x100000000, 0x0, 0x80000000, 0x0, 0x40000000, 0x0, 0x20000000, 0x0, 0x10000000, 0x0, 0x8000000, 0x0, 0x4000000, 0x0, 0x2000000, 0x0, 0x1000000, 0x0, 0x800000, 0x0, 0x400000, 0x0, 0x200000, 0x0, 0x100000, 0x0, 0x80000, 0x0, 0x40000, 0x0, 0x20000, 0x0, 0x10000, 0x0, 0x8000, 0x0, 0x4000, 0x0, 0x2000, 0x0, 0x1000, 0x0, 0x800, 0x0, 0x400, 0x0, 0x200, 0x0, 0x100, 0x0, 0x80, 0x8000000000000000, 0x40, 0x4000000000000000, 0x20, 0x2000000000000000, 0x10, 0x1000000000000000, 0x8, 0x800000000000000, 0x4, 0x400000000000000, 0x2, 0x200000000000000 };

// 5 * wheel mod 30
// this way we don't have to check for index wrap around
const int wheel5[40] = {2, 1, 2, 1, 2, 3, 1, 3, 2, 1, 2, 1, 2, 3, 1, 3, 2, 1, 2, 1, 2, 3, 1, 3, 2, 1, 2, 1, 2, 3, 1, 3, 2, 1, 2, 1, 2, 3, 1, 3};


// find mod 30 wheel index based on starting N
// this is used by threads to iterate over the number line
void findWheelOffset(uint64_t & start, int32_t & index){

	int32_t wheel[8] = {4, 2, 4, 2, 4, 6, 2, 6};
	int32_t idx = -1;

	// find starting number using mod 6 wheel
	// N=(k*6)-1, N=(k*6)+1 ...
	// where k, k+1, k+2 ...
	uint64_t k = start / 6;
	int32_t i = 1;
	uint64_t N = (k * 6)-1;


	while( N < start || N % 5 == 0 ){
		if(i){
			i = 0;
			N += 2;
		}
		else{
			i = 1;
			N += 4;
		}
	}

	start = N;

	// find mod 30 wheel index by iterating with a mod 6 wheel until finding N divisible by 5
	// forward to find index
	while(idx < 0){

		if(i){
			N += 2;
			i = 0;
			if(N % 5 == 0){
				N -= 2;
				idx = 5;
			}

		}
		else{
			N += 4;
			i = 1;
			if(N % 5 == 0){
				N -= 4;
				idx = 7;
			}
		}
	}

	// reverse to find starting index
	while(N != start){
		--idx;
		if(idx < 0)idx=7;
		N -= wheel[idx];
	}


	index = idx;

}


void *thr_func(void *arg) {

	thread_data_t *data = (thread_data_t *)arg;
	uint64_t checksum = 0;
//	uint64_t primecount = 0;
	int err;
	int32_t wheelidx;
	int32_t idx;
	int32_t Result;
	int32_t Rem;
	int32_t Quot;

	err = pthread_mutex_lock(&lock1);
	if (err){
		fprintf(stderr, "ERROR: pthread_mutex_lock, code: %d\n", err);
		exit(EXIT_FAILURE);
	}
	uint64_t start = current_start;
	uint64_t stop = start + thread_range;
	if(stop > data->end || stop < start){  // ck overflow
		stop = data->end;
	}
	current_start = stop;
	err = pthread_mutex_unlock(&lock1);
	if (err){
		fprintf(stderr, "ERROR: pthread_mutex_unlock, code: %d\n", err);
		exit(EXIT_FAILURE);
	}

	while( start < stop ){

		uint64_t start2 = start;

		findWheelOffset(start2, wheelidx);

		while( start2 < stop ){

			// 4 turns of the wheel
			int32_t widx = wheelidx;
			uint64_t P = start2;
			uint64_t end = P + 120;

			if(end > stop || end < start2){  // ck overflow
				end = stop;
			}

			// sieve small primes to 113, this seems optimal
			uint64_t bitsieve = ~(p7[P%7] | p11[P%11] | p13[P%13] | p17[P%17] | p19[P%19] | p23[P%23] | p29[P%29] | p31[P%31]
					| p37[P%37] | p41[P%41] | p43[P%43] | p47[P%47] | p53[P%53] | p59[P%59] | p61[P%61] | p67[P%67]
					| p71[P%71] | p73[P%73] | p79[P%79] | p83[P%83] | p89[P%89] | p97[P%97] | p101[P%101]
					| p103[P%103] | p107[P%107] | p109[P%109] | p113[P%113]);

			while(P < end){
				if(bitsieve & 1){

					uint64_t q;
					ulong2 one;

					if(strong_prp_base2( P, q )){
						//++primecount;
						if(wieferich(P, q, one, checksum, Result, Rem, Quot)){
							err = pthread_mutex_lock(&lock2);
							if (err){
								fprintf(stderr, "ERROR: pthread_mutex_lock, code: %d\n", err);
								exit(EXIT_FAILURE);
							}
							idx = resultcount;
							++resultcount;
							err = pthread_mutex_unlock(&lock2);
							if (err){
								fprintf(stderr, "ERROR: pthread_mutex_unlock, code: %d\n", err);
								exit(EXIT_FAILURE);
							}

							hostSpecialPrime[idx] = P;
							hostResult[idx] = Result;
							hostRem[idx] = Rem;
							hostQuot[idx] = Quot;
						}
						if(wallsunsun(P, q, one, checksum, Result, Rem, Quot)){
							err = pthread_mutex_lock(&lock2);
							if (err){
								fprintf(stderr, "ERROR: pthread_mutex_lock, code: %d\n", err);
								exit(EXIT_FAILURE);
							}
							idx = resultcount;
							++resultcount;
							err = pthread_mutex_unlock(&lock2);
							if (err){
								fprintf(stderr, "ERROR: pthread_mutex_unlock, code: %d\n", err);
								exit(EXIT_FAILURE);
							}

							hostSpecialPrime[idx] = P;
							hostResult[idx] = Result;
							hostRem[idx] = Rem;
							hostQuot[idx] = Quot;
						}
					}
				}

				int inc = wheel5[widx++];

				bitsieve >>= inc;
				P += inc*2;

				if( P < start2 ){  // ck overflow
					P = end;
				}
			}

			start2 += 120;

			if(start2 < start){  // ck overflow
				start2 = stop; 
			}


		}

		err = pthread_mutex_lock(&lock1);
		if (err){
			fprintf(stderr, "ERROR: pthread_mutex_lock, code: %d\n", err);
			exit(EXIT_FAILURE);
		}
		start = current_start;
		stop = start + thread_range;
		if(stop > data->end || stop < start){  // ck overflow
			stop = data->end;
		}
		current_start = stop;
		err = pthread_mutex_unlock(&lock1);
		if (err){
			fprintf(stderr, "ERROR: pthread_mutex_unlock, code: %d\n", err);
			exit(EXIT_FAILURE);
		}
	}

	err = pthread_mutex_lock(&lock3);
	if (err){
		fprintf(stderr, "ERROR: pthread_mutex_lock, code: %d\n", err);
		exit(EXIT_FAILURE);
	}
	totalchecksum += checksum;
	//totalprimecount += primecount;
	err = pthread_mutex_unlock(&lock3);
	if (err){
		fprintf(stderr, "ERROR: pthread_mutex_unlock, code: %d\n", err);
		exit(EXIT_FAILURE);
	}

	pthread_exit(NULL);

	return NULL;
}
 

FILE *my_fopen(const char * filename, const char * mode)
{
	char resolved_name[512];

	boinc_resolve_filename(filename,resolved_name,sizeof(resolved_name));
	return boinc_fopen(resolved_name,mode);

}


void write_state(uint64_t start, uint64_t stop, uint64_t current, uint64_t cksm, bool & write_state_a_next)
{
	FILE *out;

        if (write_state_a_next){
		if ((out = my_fopen(STATE_FILENAME_A,"w")) == NULL)
			fprintf(stderr,"Cannot open %s !!!\n",STATE_FILENAME_A);
	}
	else{
                if ((out = my_fopen(STATE_FILENAME_B,"w")) == NULL)
                        fprintf(stderr,"Cannot open %s !!!\n",STATE_FILENAME_B);
        }
	if (fprintf(out,"%" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n",start,stop,current,cksm) < 0){
		if (write_state_a_next)
			fprintf(stderr,"Cannot write to %s !!! Continuing...\n",STATE_FILENAME_A);
		else
			fprintf(stderr,"Cannot write to %s !!! Continuing...\n",STATE_FILENAME_B);

		// Attempt to close, even though we failed to write
		fclose(out);
	}
	else{
		// If state file is closed OK, write to the other state file
		// next time round
		if (fclose(out) == 0) 
			write_state_a_next = !write_state_a_next; 
	}
}

/* Return 1 only if a valid checkpoint can be read.
   Attempts to read from both state files,
   uses the most recent one available.
 */
int read_state(uint64_t start, uint64_t stop, uint64_t & current, uint64_t & cksm, bool & write_state_a_next)
{
	FILE *in;
	bool good_state_a = true;
	bool good_state_b = true;
	uint64_t tmp1, tmp2;
	uint64_t current_a, cksm_a, current_b, cksm_b;

        // Attempt to read state file A
	if ((in = my_fopen(STATE_FILENAME_A,"r")) == NULL){
		good_state_a = false;
        }
	else if (fscanf(in,"%" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n",&tmp1,&tmp2,&current_a,&cksm_a) != 4){
		fprintf(stderr,"Cannot parse %s !!!\n",STATE_FILENAME_A);
		good_state_a = false;
	}
	else{
		fclose(in);
		/* Check that start stop match */
		if (tmp1 != start || tmp2 != stop){
			good_state_a = false;
		}
	}

        // Attempt to read state file B
        if ((in = my_fopen(STATE_FILENAME_B,"r")) == NULL){
                good_state_b = false;
        }
        else if (fscanf(in,"%" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n",&tmp1,&tmp2,&current_b,&cksm_b) != 4){
                fprintf(stderr,"Cannot parse %s !!!\n",STATE_FILENAME_B);
                good_state_b = false;
        }
        else{
                fclose(in);
		/* Check that start stop match */
		if (tmp1 != start || tmp2 != stop){
                        good_state_b = false;
                }
        }

        // If both state files are OK, check which is the most recent
	if (good_state_a && good_state_b)
	{
		if (current_a > current_b)
			good_state_b = false;
		else
			good_state_a = false;
	}

        // Use data from the most recent state file
	if (good_state_a && !good_state_b)
	{
		current = current_a;
		cksm = cksm_a;
		write_state_a_next = false;
		return 1;
	}
        if (good_state_b && !good_state_a)
        {
                current = current_b;
                cksm = cksm_b;
		write_state_a_next = true;
		return 1;
        }

	// If we got here, neither state file was good
	return 0;
}


void checkpoint(uint64_t start, uint64_t stop, uint64_t current, uint64_t cksm, bool & write_state_a_next)
{

	write_state(start, stop, current, cksm, write_state_a_next);

	if(boinc_is_standalone()){
		printf("Checkpoint, current: %" PRIu64 "\n", current);
	}

	boinc_checkpoint_completed();

}


// N is a strong pseudoprime to base 2
int isPRP(uint64_t N)
{
	if (N%2==0)
		return 0;

	// check if the first few primes divide N
	// Must match CPU program to maintain checksum
	if(N % 3)
	if(N % 5)
	if(N % 7)
	if(N % 11)
	if(N % 13)
	if(N % 17)
	if(N % 19)
	if(N % 23)
	if(N % 29)
	if(N % 31)
	if(N % 37)
	if(N % 41)
	if(N % 43)
	if(N % 47)
	if(N % 53)
	if(N % 59)
	if(N % 61)
	if(N % 67)
	if(N % 71)
	if(N % 73)
	if(N % 79)
	if(N % 83)
	if(N % 89)
	if(N % 97)
	if(N % 101)
	if(N % 103)
	if(N % 107)
	if(N % 109)
	if(N % 113){

		uint64_t q;

		if( strong_prp_base2( N, q ) )
			return 1;
	}

	return 0;
}


// N is prime.
// Miller–Rabin primality test
//    if n <  3,825,123,056,546,413,051, it is enough to test a = 2, 3, 5, 7, 11, 13, 17, 19, and 23.
//                  3825123056546413051
//    if n < 18,446,744,073,709,551,616, it is enough to test a = 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, and 37.
//                 18446744073709551616
// minimum is 29
int isPrime(uint64_t N)
{
	const int base[12] = {2,3,5,7,11,13,17,19,23,29,31,37};

	if (N % 2==0)
		return 0;

	uint64_t q, one, r2, nmo;

	montInit(N, &q, &one, &nmo, &r2);

	if ( N < 3825123056546413051ULL ){
		for (int i = 0; i < 9; ++i)
			if (!strong_prp(base[i], N, q, one, nmo, r2))
				return 0;
	}
	else if ( N <= UINT64_MAX ){
		for (int i = 0; i < 12; ++i)
			if (!strong_prp(base[i], N, q, one, nmo, r2))
				return 0;
	}

	return 1;
}


void report_solution( char * results ){

	FILE * resfile = my_fopen(RESULTS_FILENAME,"a");

	if( resfile == NULL ){
		fprintf(stderr,"Cannot open %s !!!\n",RESULTS_FILENAME);
		exit(EXIT_FAILURE);
	}

	if( fprintf( resfile, "%s", results ) < 0 ){
		fprintf(stderr,"Cannot write to %s !!!\n",RESULTS_FILENAME);
		exit(EXIT_FAILURE);
	}

	fflush(resfile);

#if defined (_WIN32)
	_commit(_fileno(resfile));
#else
	fsync(fileno(resfile));
#endif

	fclose(resfile);

}
  

int main(int argc, char *argv[])
{ 
	uint64_t bottom;
	uint64_t top;
	uint64_t current;
	uint32_t num_threads = 1;
	int err;
	uint32_t k;

	int32_t numresults = 15000;
	bool write_state_a_next;

	time_t boinc_last, boinc_curr;
	time_t wu_end, wu_start;

	hostSpecialPrime = (uint64_t *)malloc(numresults*sizeof(uint64_t));
	if( hostSpecialPrime == NULL ){
		fprintf(stderr,"malloc error\n");
		exit(EXIT_FAILURE);
	}
	hostResult = (int32_t *)malloc(numresults*sizeof(int32_t));
	if( hostResult == NULL ){
		fprintf(stderr,"malloc error\n");
		exit(EXIT_FAILURE);
	}
	hostRem = (int32_t *)malloc(numresults*sizeof(int32_t));
	if( hostRem == NULL ){
		fprintf(stderr,"malloc error\n");
		exit(EXIT_FAILURE);
	}
	hostQuot = (int32_t *)malloc(numresults*sizeof(int32_t)); 
	if( hostQuot == NULL ){
		fprintf(stderr,"malloc error\n");
		exit(EXIT_FAILURE);
	}


        // Initialize BOINC
        BOINC_OPTIONS options;
        boinc_options_defaults(options);
	options.multi_thread = true; 
        boinc_init_options(&options);

	fprintf(stderr, "\nWWcpu version %d.%d%s by Bryan Little and Yves Gallot\n",MAJORV,MINORV,SUFFIXV);
	fprintf(stderr, "Compiled " __DATE__ " with GCC " __VERSION__ "\n");
	fprintf(stderr, "A Wieferich and WallSunSun prime number search program\n");
	fprintf(stderr, "Based on wwww by Mark Rodenkirch\n");
	if(boinc_is_standalone()){
		printf("WWcpu version %d.%d%s by Bryan Little and Yves Gallot\n",MAJORV,MINORV,SUFFIXV);
		printf("Compiled " __DATE__ " with GCC " __VERSION__ "\n");
		printf("A Wieferich and WallSunSun prime number search program\n");
		printf("Based on wwww by Mark Rodenkirch\n");	
	}

        // Print out cmd line for diagnostics
        fprintf(stderr, "Command line: ");
        for (int i = 0; i < argc; i++)
        	fprintf(stderr, "%s ", argv[i]);
        fprintf(stderr, "\n");


	/* Get search parameters from command line */
	if(argc < 3){
		printf("Usage: %s START END -t # or --nthreads #\nWhere 127 <= START < END < 2^64\nAnd -t # or --nthreads # is optional number of threads\n",argv[0]);
		printf("Example format: %s 10000000 20000000\n",argv[0]);
		printf("            Or: %s 1e7 2e7\n",argv[0]);
		exit(EXIT_FAILURE);
	}


	// get START value
	char * a1 = strstr(argv[1], (char*)"e");
	if(a1 != NULL){
		// exp notation
		uint64_t num = 0;
		int32_t exp = 0;

		num = strtoull(argv[1], &a1, 0);
		exp = strtoul(a1+1, NULL, 0);

		if(num == 0 || exp == 0){
			printf("ERROR: unable to parse START\n");
			fprintf(stderr, "ERROR: unable to parse START\n");
			exit(EXIT_FAILURE);
		}

		for(int i=0; i < exp; ++i){
			if(num > UINT64_MAX/10){
				printf("ERROR: START is out of range\n");
				fprintf(stderr, "ERROR: START is out of range\n");
				exit(EXIT_FAILURE);
			}
			else{
				num *= 10;
			}
		}
		
		bottom = num;

	}
	else{
		// std notation
		sscanf(argv[1],"%" PRIu64 "",&bottom);
	}

	
	// get END value
	char * a2 = strstr(argv[2], (char*)"e");
	if(a2 != NULL){
		// exp notation
		uint64_t num = 0;
		int32_t exp = 0;

		num = strtoull(argv[2], &a2, 0);
		exp = strtoul(a2+1, NULL, 0);

		if(num == 0 || exp == 0){
			printf("ERROR: unable to parse END\n");
			fprintf(stderr, "ERROR: unable to parse END\n");
			exit(EXIT_FAILURE);
		}

		for(int i=0; i < exp; ++i){
			if(num > UINT64_MAX/10){
				printf("ERROR: END is out of range\n");
				fprintf(stderr, "ERROR: END is out of range\n");
				exit(EXIT_FAILURE);
			}
			else{
				num *= 10;
			}
		}
		
		top = num;		
	}
	else{
		// std notation
		sscanf(argv[2],"%" PRIu64 "",&top);
	}	


	// check search range
	if(bottom < 127){
		printf("START below minimum. Starting search at 127\n");
		fprintf(stderr, "START below minimum. Starting search at 127\n");
		bottom = 127;
	}
	if(top <= bottom){
		printf("ERROR: END must be > START\n");
		fprintf(stderr, "ERROR: END must be > START\n");
		exit(EXIT_FAILURE);
	}


	fprintf(stderr, "Starting search at: %" PRIu64 "\nStopping search at: %" PRIu64 "\n", bottom,top);
	if(boinc_is_standalone()){
		printf("Starting search at: %" PRIu64 "\nStopping search at: %" PRIu64 "\n", bottom,top);
	}

	if(argc == 5){
		if( strcmp(argv[3], "-t") == 0 || strcmp(argv[3], "--nthreads") == 0 ){

			uint32_t NT;
			sscanf(argv[4],"%u",&NT);

			if(NT < 1){
				printf("ERROR: number of threads must be at least 1.\n");
				fprintf(stderr, "ERROR: number of threads must be at least 1.\n");
				exit(EXIT_FAILURE);
			}
			else if(NT > 64){
				NT=64;
				if(boinc_is_standalone()){
					printf("maximum value for number of threads is 64.\n");
				}
				fprintf(stderr, "maximum value for number of threads is 64.\n");
			}

			uint32_t maxthreads = 0;
			maxthreads = std::thread::hardware_concurrency();

			if(maxthreads){
				if(NT > maxthreads){
					if(boinc_is_standalone()){
						printf("Detected %u logical processors.  Using %u threads.\n", maxthreads, maxthreads);
					}
					fprintf(stderr, "Detected %u logical processors.  Using %u threads.\n", maxthreads, maxthreads);
					NT = maxthreads;
				}
				else{
					if(boinc_is_standalone()){
						printf("Detected %u logical processors.  Using %u threads.\n", maxthreads, NT);
					}
					fprintf(stderr, "Detected %u logical processors.  Using %u threads.\n", maxthreads, NT);
				}
			}
			else{
				if(boinc_is_standalone()){
					printf("Unable to detect logical processor count.  Using %u threads.\n", NT);
				}
				fprintf(stderr, "Unable to detect logical processor count.  Using %u threads.\n", NT);
			}
					
			num_threads = NT;
		}
	}
	else{
		if(boinc_is_standalone()){
			printf("Running single threaded.\n");
		}
		fprintf(stderr, "Running single threaded.\n");
	}


	/* Resume from checkpoint if there is one */
	if (read_state(bottom,top,current,totalchecksum,write_state_a_next)){
		if(boinc_is_standalone()){
			printf("Resuming search from checkpoint. Current: %" PRIu64 "\n",current);
		}
		fprintf(stderr,"Resuming search from checkpoint. Current: %" PRIu64 "\n",current);
	}
	else{
		if(boinc_is_standalone()){
			printf("Beginning a new search with parameters from the command line\n");
		}
		current = bottom;
                write_state_a_next = true;
		totalchecksum = 0;

		// clear result file
		FILE * temp_file = my_fopen(RESULTS_FILENAME,"w");
		if (temp_file == NULL){
			fprintf(stderr,"Cannot open %s !!!\n",RESULTS_FILENAME);
			exit(EXIT_FAILURE);
		}
		fclose(temp_file);
	}
	//trying to resume a finished workunit
	if(current == top){
		if(boinc_is_standalone()){
			printf("Workunit complete.\n");
		}
		boinc_finish(EXIT_SUCCESS);
		return 0;
	}

	time (&wu_start);
	time (&boinc_last);

	// initialize pthread mutex
	err = pthread_mutex_init(&lock1, NULL);
	if (err){
		fprintf(stderr, "ERROR: pthread_mutex_init, code: %d\n", err);
		exit(EXIT_FAILURE);
	}
	err = pthread_mutex_init(&lock2, NULL);
	if (err){
		fprintf(stderr, "ERROR: pthread_mutex_init, code: %d\n", err);
		exit(EXIT_FAILURE);
	}
	err = pthread_mutex_init(&lock3, NULL);
	if (err){
		fprintf(stderr, "ERROR: pthread_mutex_init, code: %d\n", err);
		exit(EXIT_FAILURE);
	}


	uint64_t calc_range = num_threads * ckpt_range;
	uint64_t stop;
	//totalprimecount = 0;

	uint64_t start = current;
	if(start % 2 == 0){
		++start;
	}

	for ( ; start < top; start = stop ){

		stop = start + calc_range;

		if(stop > top || stop < start){  // ck overflow
			stop = top;
		}

		// clear results
		resultcount = 0;

		pthread_t thr[num_threads];

		// create a thread_data_t argument array
		thread_data_t thr_data[num_threads];

		// initialize shared data
		current_start = start;

		// create threads
		for (k = 0; k < num_threads; ++k) {
			thr_data[k].id = k;
			thr_data[k].end = stop;
			err = pthread_create(&thr[k], NULL, thr_func, &thr_data[k]);
			if (err){
				fprintf(stderr, "ERROR: pthread_create, code: %d\n", err);
				exit(EXIT_FAILURE);
			}
		}

		// update BOINC fraction done every 2 sec and sleep the main thread
		err = pthread_mutex_lock(&lock1);
		if (err){
			fprintf(stderr, "ERROR: pthread_mutex_lock, code: %d\n", err);
			exit(EXIT_FAILURE);
		}
		uint64_t now = current_start;
		err = pthread_mutex_unlock(&lock1);
		if (err){
			fprintf(stderr, "ERROR: pthread_mutex_unlock, code: %d\n", err);
			exit(EXIT_FAILURE);
		}
		while(now < stop){
			struct timespec sleep_time;
			sleep_time.tv_sec = 1;
			sleep_time.tv_nsec = 0;
			nanosleep(&sleep_time,NULL);

			time (&boinc_curr);
			if( ((int)boinc_curr - (int)boinc_last) > 1 ){
	    			double fd = (double)(now-bottom)/(double)(top-bottom);
				boinc_fraction_done(fd);
				if(boinc_is_standalone()){
					printf("Tests done: %.4f%%\n",fd*100.0);
				}
				boinc_last = boinc_curr;
			}

			err = pthread_mutex_lock(&lock1);
			if (err){
				fprintf(stderr, "ERROR: pthread_mutex_lock, code: %d\n", err);
				exit(EXIT_FAILURE);
			}
			now = current_start;
			err = pthread_mutex_unlock(&lock1);
			if (err){
				fprintf(stderr, "ERROR: pthread_mutex_unlock, code: %d\n", err);
				exit(EXIT_FAILURE);
			}
		}

		// block until all threads complete
		for (k = 0; k < num_threads; ++k) {
			err = pthread_join(thr[k], NULL);
			if (err){
				fprintf(stderr, "ERROR: pthread_join, code: %d\n", err);
				exit(EXIT_FAILURE);
			}
		}

		if(resultcount > numresults){
			fprintf(stderr,"Error: number of results overflowed array.\n");
			exit(EXIT_FAILURE);
		}

		boinc_begin_critical_section();

		if( resultcount > 0 ){

			// sort results by prime size if needed
			int32_t i,j;

			if(resultcount > 1){
				for (i = 0; i < resultcount-1; i++){    
					for (j = 0; j < resultcount-i-1; j++){
						if (hostSpecialPrime[j] > hostSpecialPrime[j+1]){
							swap(hostSpecialPrime[j], hostSpecialPrime[j+1]);
							swap(hostRem[j], hostRem[j+1]);
							swap(hostQuot[j], hostQuot[j+1]);
							swap(hostResult[j], hostResult[j+1]);
						}
					}
				}
			}


			uint32_t PRPcnt = 0;
			char buffer[256];
			char * resbuff = (char *)malloc( resultcount * sizeof(char) * 256 );
			if( resbuff == NULL ){
				fprintf(stderr,"malloc error\n");
				exit(EXIT_FAILURE);
			}
			resbuff[0] = '\0';

			// write results
			for (i = 0; i < resultcount; ++i){

				if( isPrime(hostSpecialPrime[i]) ){
					// Wieferich Prime
					if(hostResult[i] == 1093){
						if (hostRem[i] == 1 && hostQuot[i] == 0){
							if( sprintf( buffer, "%" PRIu64 " is a Wieferich prime\n", hostSpecialPrime[i] ) < 0 ){
								fprintf(stderr,"error in sprintf()\n");
								exit(EXIT_FAILURE);
							}
							strcat( resbuff, buffer );
						}
						else{
							if( sprintf( buffer, "%" PRIu64 " is a Wieferich special instance (%+d %+d p)\n", hostSpecialPrime[i], hostRem[i], hostQuot[i]) < 0 ){
								fprintf(stderr,"error in sprintf()\n");
								exit(EXIT_FAILURE);
							}
							strcat( resbuff, buffer );
						}
					}
					// WallSunSun Primes
					else if(hostResult[i] == 1){
						if ( sprintf( buffer, "%" PRIu64 " is a WallSunSun prime\n", hostSpecialPrime[i]) < 0 ){
							fprintf(stderr,"error in sprintf()\n");
							exit(EXIT_FAILURE);
						}	
						strcat( resbuff, buffer );			
					}
					else if(hostResult[i] == 2){
						if ( sprintf( buffer, "%" PRIu64 " is a WallSunSun special instance (+0 %+d p)\n", hostSpecialPrime[i], hostQuot[i]) < 0 ){
							fprintf(stderr,"error in sprintf()\n");
							exit(EXIT_FAILURE);
						}
						strcat( resbuff, buffer );				
					}
					else{
						fprintf(stderr,"Error: hostResult returned %d.\n", hostResult[i]);
						exit(EXIT_FAILURE);				
					}
				}
				else if( isPRP(hostSpecialPrime[i]) ){
					// fast search does a base 2 PRP test only.
					// CPU calculations are OK, but we can discard this result
					++PRPcnt;
				}
				else{
					// calculated an invalid result
					fprintf(stderr, "Computation error, found a composite: %" PRIu64 "\n", hostSpecialPrime[i]);
					exit(EXIT_FAILURE);
				}

			}

			report_solution( resbuff );

			free( resbuff );

		}

		// done with workunit
		if( stop == top ){

			// print checksum
			char buffer[256];
			if( sprintf( buffer, "%016" PRIX64 "\n",totalchecksum ) < 0 ){
				fprintf(stderr,"error in sprintf()\n");
				exit(EXIT_FAILURE);
			}
			report_solution( buffer );

			double d = 1.0;
			boinc_fraction_done(d);

		}

		checkpoint(bottom,top,stop,totalchecksum,write_state_a_next);

		boinc_end_critical_section();

	}

//	printf("total primes: %" PRIu64 "\n",totalprimecount);

	fprintf(stderr,"Workunit complete.\n");

	if(boinc_is_standalone()){
		time (&wu_end);
		printf("Workunit time: %" PRIu64 " sec\n", (uint64_t)wu_end - (uint64_t)wu_start );
		printf("Checksum: %016" PRIX64 "\n", totalchecksum);
	}


	free(hostSpecialPrime);
	free(hostResult);
	free(hostRem);
	free(hostQuot); 

	boinc_finish(EXIT_SUCCESS);

	return 0; 
} 

