#ifndef BINARY_VALUE_H
#define BINARY_VALUE_H

#include <stdbool.h>

#define PREFIX_MAX_CHAR_LENGTH 4

typedef struct Binary_Value {
  long double value;
  char prefixe[PREFIX_MAX_CHAR_LENGTH];
} Binary_Value;

bool bv_format(Binary_Value *binary_value);

#endif