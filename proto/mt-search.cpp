/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include <iostream>

extern "C" {
#include "mt-square.h"
#include "mt19937ar.h"
}
//#include "debug.h"

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

NTL_CLIENT

int non_reducible(GF2X& fpoly, int degree);
void search(unsigned int n);
void berlekampMassey(GF2X& minpoly, unsigned int maxdegree, 
		     unsigned int bitpos);

static unsigned long long all_count = 0;
static unsigned long long pass1_count = 0;
static unsigned long long pass2_count = 0;
static unsigned long long pass3_count = 0;
static unsigned long long pass4_count = 0;
int limit;

int non_reducible(GF2X& fpoly, int degree) {
  GF2X t2m = GF2X(2, 1);
  GF2X t1 = GF2X(1, 1);
  GF2X t;
  GF2X alpha;
  int m;
  int count;

  //DPRINT("degree = %u\n", degree);
  //DPRINTPOLY("fpoly =", fpoly);
  if (deg(fpoly) < degree) {
    return 0;
  }
  count = 1;
  t = t1;
  t += t2m;
  
  for (m = 1; deg(fpoly) > degree; m++) {
    //DPRINTPOLY("t =", t);
    for(;;) {
      GCD(alpha, fpoly, t);
      //DPRINTPOLY("alpha =", alpha);
      if (IsOne(alpha)) {
	break;
      }
      fpoly /= alpha;
      //DPRINTPOLY("f =", fpoly);
       if (deg(fpoly) < degree) {
	return 0;
      }
    }
    if ((deg(fpoly) > degree) && (deg(fpoly) <= degree + m)) {
      //DPRINT("maybe fpoly is larger m = %d, DEG = %u\n", m, 
      //     (unsigned int)deg(fpoly));
      return 0;
    }
    t2m *= t2m;
    count++;
    t2m %= fpoly;
    add(t, t2m, t1);
  }
  if (deg(fpoly) != degree) {
    return 0;
  }
  pass1_count++;
  for (; m <= limit; m++) {
    //DPRINTPOLY("t =", t);
    for(;;) {
      GCD(alpha, fpoly, t);
      //DPRINTPOLY("alpha =", alpha);
      if (IsOne(alpha)) {
	break;
      }
      fpoly /= alpha;
      //DPRINTPOLY("f =", fpoly);
       if (deg(fpoly) < degree) {
	return 0;
      }
    }
    t2m *= t2m;
    count++;
    t2m %= fpoly;
    add(t, t2m, t1);
  }
  pass2_count++;
  for (;m < degree; m++) {
    t2m *= t2m;
    t2m %= fpoly;
    count++;
  }
  add(t, t1, t2m);
  if (deg(t) == -1) {
    //DPRINT("check ok fdeg = %u\n", (unsigned int)DEG(fpoly));
    pass3_count++;
    return 1;
  } else {
    //DPRINT("check ng fdeg = %u\n", (unsigned int)DEG(fpoly));
    return 0;
  }
}

void generating_polynomial(vec_GF2& vec, unsigned int bitpos, 
			   unsigned int maxdegree)
{
  unsigned int i;
  unsigned int mask = 1UL << bitpos;

  //DPRINTHT("in gene:", rand);
  i = 0;
  while ((gen_rand() & mask) == 0) {
    i++;
    if(i > 2 * maxdegree){
      printf("generating_polynomial:too much zeros\n");
      vec[0] = 1;
      return;
    }
  }
  //DPRINTHT("middle gene:", rand);
  vec[0] = 1;

  for (i=1; i<= 2 * maxdegree-1; i++) {
    if ((gen_rand() & mask) == mask){
      vec[i] = 1;
    }
  }
  //DPRINTHT("end gene:", rand);
}

void berlekampMassey(GF2X& minpoly, unsigned int maxdegree,
		     unsigned int bitpos) {
  vec_GF2 genvec = vec_GF2(INIT_SIZE, 2 * maxdegree);
  GF2X zero;

  generating_polynomial(genvec, bitpos, maxdegree);
  if (genvec.length() == 0) {
    minpoly = zero;
    return;
  }
  MinPolySeq(minpoly, genvec, maxdegree);
}

void printBinary(ostream& os, GF2X& poly)
{
  int i;
  if (deg(poly) < 0) {
    os << "0deg=-1\n";
    return;
  }
  for(i = 0; i <= deg(poly); i++) {
    if(rep(coeff(poly, i)) == 1) {
      os << '1';
    } else {
      os << '0';
    }
    /* printf("%1d", (unsigned int)(poly[j] >> (i % WORDLL)) & 0x1ULL);*/
    if ((i % 32) == 31) {
      os << '\n';
    }
  }
  os << "deg=" << deg(poly) << endl;
}

void search(unsigned int n) {
  int j;
  unsigned int succ = 0;
  int bmOk;
  GF2X minpoly;
  unsigned int gmm;
  unsigned int grot1;
  unsigned int maxdegree;
  unsigned int mexp;
  
  maxdegree = get_rnd_maxdegree();
  mexp = get_rnd_mexp();
  for (;;) {
    gmm = genrand_int32() % ((mexp)/32 + 1);
    grot1 = genrand_int32();
    setup_param(gmm, grot1);
    init_gen_rand(genrand_int32()+3);
    bmOk = 1;
    //    for (j = 0; j < 32; j++) {
    for (j = 0; j < 1; j++) {
      berlekampMassey(minpoly, maxdegree, j);
      if (deg(minpoly) == -1) {
	bmOk = 0;
	break;
      }
      all_count++;
      if (!non_reducible(minpoly, mexp)) {
	bmOk = 0;
	break;
      }
    }
    if (bmOk) {
      pass4_count++;
      printf("----------\n");
      printf("succ = %u\n", ++succ);
      printf("deg = %ld\n", deg(minpoly));
      printf("gmm = %d\n", gmm);
      printf("grot1 = %u\n", grot1);
      printBinary(cout, minpoly);
      fflush(stdout);
      if (succ >= n) {
	break;
      }
    }
    if (all_count % 10000 == 0) {
      printf("count = %llu\n", all_count);
      printf("pass1 = %llu\n", pass1_count);
      printf("pass2 = %llu\n", pass2_count);
      printf("pass3 = %llu\n", pass3_count);
      printf("pass4 = %llu\n", pass4_count);
      fflush(stdout);
    }
  }
  printf("count = %llu\n", all_count);
  printf("pass1 = %llu\n", pass1_count);
  printf("pass2 = %llu\n", pass2_count);
  printf("pass3 = %llu\n", pass3_count);
  printf("pass4 = %llu\n", pass4_count);
  fflush(stdout);
}

int main(int argc, char* argv[]){
  int n;
  unsigned long seed;

  setup_param(1, 21);

  if (argc != 3) {
    limit = 32;
    n = 1;
  } else {
    limit = atoi(argv[1]);
    n = atoi(argv[2]);
  }

  printf("MEXP = %d\n", get_rnd_mexp());
  seed = (long)time(NULL);
  printf("seed = %lu\n", seed);
  init_genrand(seed);
  printf("search limit degree = %d\n", limit);
  printf("now search %d times\n", n);
  fflush(stdout);
  search(n);

  return 0;
}
