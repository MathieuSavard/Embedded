#ifndef BINARY_VALUE_H
#define BINARY_VALUE_H

#include <limits.h>
#include <stdbool.h>

typedef enum { BV_B, BV_kB, BV_MB, BV_GB, BV_TB, BV_PB, BV_EB } BVPrefix;

typedef struct BinVal {
  long double value;
  BVPrefix prefix;
} BinVal;

bool bv_init(BinVal *bv, long double value, BVPrefix prefix);
void bv_print_value(const BinVal *bv);
void bv_print_prefix(const BinVal *bv);
void bv_print_value_prefix(const BinVal *bv);

#endif