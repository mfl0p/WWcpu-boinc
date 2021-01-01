// WW.h


#define SPECIAL_THRESHOLD 1000

typedef struct {
	uint64_t s0,s1;
}ulong2;


int wieferich(uint64_t thePrime, uint64_t inverted, ulong2 & mone, uint64_t & checksum, int32_t & result, int32_t & rem, int32_t & quot);

int wallsunsun(uint64_t thePrime, uint64_t inverted, ulong2 mone, uint64_t & checksum, int32_t & result, int32_t & rem, int32_t & quot);

