// kernel/rng.c

#include "types.h"
#include "param.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"
#include "sleeplock.h"
#include "pcg_basic.h"

// avoid false-sharing
typedef struct {
  pcg32_random_t s;
  uint8 pad[64 - (sizeof(pcg32_random_t) % 64)];
} rngcpu_t;

// per-CPU RNG allocation
static rngcpu_t rng_state[NCPU];

// seed initialize
static inline uint64 splitmix64(uint64 x) {
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}

void
rng_init_percpu(uint64 global_seed)
{
  int id = cpuid();
  uint64 seed = splitmix64(global_seed ^ (0x9e3779b97f4a7c15ULL * (id+1)));
  uint64 seq  = ((uint64)id << 1) | 1ULL;
  pcg32_srandom_r(&rng_state[id].s, seed, seq);
}

uint32
rng_u32(void)
{
  return pcg32_random_r(&rng_state[cpuid()].s);
}

uint32
rng_bounded(uint32 bound)
{
  return pcg32_boundedrand_r(&rng_state[cpuid()].s, bound);
}

