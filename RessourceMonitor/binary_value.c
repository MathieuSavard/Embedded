#include "binary_value.h"

#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_PREFIX_NB 7
#define PREFIX_MAX_CHAR_LENGTH 4

const char prefixes[MAX_PREFIX_NB][PREFIX_MAX_CHAR_LENGTH] = {
    {"B"}, {"kB"}, {"MB"}, {"GB"}, {"TB"}, {"PB"}, {"EB"},
};

bool bv_init(BinVal *bv, long double value, BVPrefix prefix) {
  while (value >= 1000) {
    value /= 1024;
    prefix++;
    if (prefix >= MAX_PREFIX_NB)
      return false;
  }

  while (value < 0) {
    if (prefix == 0)
      return false;
    value *= 1024;
    prefix--;
  }

  if (value == 0)
    prefix = BV_B;

  bv->value = value;
  bv->prefix = prefix;

  return true;
}

void bv_print_value(const BinVal *bv) {
  if (bv->value >= 1000 || (bv->value < 1 && bv->value != 0))
    printf("%Lf", LDBL_MAX);
  else if (bv->value >= 100)
    printf("%.1Lf", bv->value);
  else if (bv->value >= 10)
    printf("%.2Lf", bv->value);
  else if (bv->value >= 1)
    printf("%.3Lf", bv->value);
  if (bv->value == 0)
    printf("0");
}

void bv_print_prefix(const BinVal *bv) { printf("%s", prefixes[bv->prefix]); }

void bv_print_value_prefix(const BinVal *bv) {
  bv_print_value(bv);
  printf(" ");
  bv_print_prefix(bv);
}
