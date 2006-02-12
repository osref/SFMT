/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static uint32_t sfmt[N][4];
static unsigned int index;

static unsigned int POS1 = 1;
static unsigned int SL1 = 11;
static unsigned int SL2 = 7;
static unsigned int SR1 = 17;

static void gen_rand_all(void);

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6) {
    POS1 = p1 % (N-3) + 1;
    SL1 = p2 % (32 - 1) + 1;
    SL2 = p3 % (32 - 1) + 1;
    SR1 = p4 % (32 - 1) + 1;
    memset(sfmt, 0, sizeof(sfmt));
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SR1] = [%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1);
    fflush(fp);
}

static void gen_rand_all(void) {
    int i, j;

    for (i = 0; i < N; i++) {
	for (j = 0; j < 4; j++) {
	    sfmt[i][j] = sfmt[i][j] ^ (sfmt[i][(j & 2) | ((j & 1) ^ 1)] << SL1)
		^ sfmt[(i + POS1) % N][j] 
		^ (sfmt[(i + POS1) % N][(j + 1) % 4] >> SR1)
		^ sfmt[(i + N - 2) % N][j] 
		^ (sfmt[(i + N - 2) % N][(j + 3) % 4] << SL2);
	}
    }
}

/*
  inside for loop means 
	sfmt[i][0] = sfmt[i][0] ^ (sfmt[i][1] << PSL1)
	    ^ sfmt[(i + PM) % N][0] ^ (sfmt[(i + PM) % N][1] >> PSR1)
	    ^ sfmt[(i + N - 2) % N][0] ^ (sfmt[(i + N -2) % N][3] << PSL2);
	sfmt[i][1] = sfmt[i][1] ^ (sfmt[i][0] << PSL1)
	    ^ sfmt[(i + PM) % N][1] ^ (sfmt[(i + PM) % N][2] >> PSR1)
	    ^ sfmt[(i + N - 2) % N][1] ^ (sfmt[(i + N -2) % N][0] << PSL2);
	sfmt[i][2] = sfmt[i][2] ^ (sfmt[i][3] << PSL1)
	    ^ sfmt[(i + PM) % N][3] ^ (sfmt[(i + PM) % N][3] >> PSR1)
	    ^ sfmt[(i + N - 2) % N][2] ^ (sfmt[(i + N -2) % N][1] << PSL2);
	sfmt[i][3] = sfmt[i][3] ^ (sfmt[i][2] << PSL1)
	    ^ sfmt[(i + PM) % N][3] ^ (sfmt[(i + PM) % N][0] >> PSR1)
	    ^ sfmt[(i + N - 2) % N][3] ^ (sfmt[(i + N -2) % N][2] << PSL2);
*/

uint32_t gen_rand(void)
{
    uint32_t r;

    if (index >= N * 4) {
	gen_rand_all();
	index = 0;
    }
    r = sfmt[index / 4][index % 4];
    index++;
    return r;
}

void init_gen_rand(uint32_t seed)
{
    int i;

    sfmt[0][0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmt[i/4][i%4] = 1812433253UL 
	    * (sfmt[(i - 1) / 4][(i - 1) % 4]
	       ^ (sfmt[(i - 1) / 4][(i - 1) % 4] >> 30)) 
	    + i;
    }
    index = N * 4;
}
