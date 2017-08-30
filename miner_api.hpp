#include <stdint.h>
#define MAXSOLS 8
// arbitrary length of header hashed into siphash key
#define HEADERLEN 80

typedef struct solutions{
    uint64_t sols[MAXSOLS][42];
    uint32_t nsols;
} solutions_t;

int verifyhl(uint64_t solution[42], char* header, int nonce);
solutions_t mine(char* header, int nonce);