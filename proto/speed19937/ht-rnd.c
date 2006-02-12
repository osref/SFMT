/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <stdint.h>
#include <stdio.h>
//#include "types.h"
#include "random-common.h"

#ifndef MEXP
#define MEXP 19937
#endif
#define WORDSIZE 32
#define N ((MEXP-1)/WORDSIZE)
#define NN (N+1) /* gx[N] is an additional state variable */
#define MAXDEGREE (WORDSIZE*NN)

static uint32_t gx[NN];
static unsigned int idx;

#define GMM 294
#define GS2 5
#define GS3 9
#define GROT1 17
#define GROT2 22

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

unsigned int get_onetime_rnds(void) 
{
    return N;
}

void print_param(FILE *fp) {
    fprintf(fp, "gmm = %u\n", GMM);
    fprintf(fp, "gs2 = %u\n", GS2);
    fprintf(fp, "gs3 = %u\n", GS3);
    fprintf(fp, "grot1 = %u\n", GROT1);
    fprintf(fp, "grot2 = %u\n", GROT2);
    fflush(fp);
}

void gen_rand_all(void) {
    uint32_t u;
    unsigned int jump;
    unsigned int i;
  
    idx = 0;
    u = gx[N];
    jump = GMM;
    for (i = 0; jump < N; i++) {
	u ^= (gx[i] >> GROT1) 
	    ^ (gx[i] << GROT2);
	u ^= gx[jump];
	u ^= u << GS2;
	gx[i] ^=  u ^ (u << GS3);
	jump++;
    }
    jump -= N;
    for (; i < N; i++) {
	u ^= (gx[i] >> GROT1) 
	    ^ (gx[i] << GROT2);
	u ^= gx[jump];
	u ^= u << GS2;
	gx[i] ^=  u ^ (u << GS3);
	jump++;
    }
    gx[N] = u;

}

uint32_t gen_rand()
{
    if (idx >= N) {
	gen_rand_all();
    }
    return gx[idx++];
}

void init_gen_rand(uint32_t seed)
{
    int i;
    gx[0] = seed;
    for (i = 1; i < NN; i++) {
	gx[i] = (1812433253UL * (gx[i - 1] ^ (gx[i - 1] >> 30)) + i); 
    }
    idx = N;
}
