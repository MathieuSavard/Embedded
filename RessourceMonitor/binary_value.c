#include "binary_value.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_PREFIX_NB 7

const char prefixes[MAX_PREFIX_NB][PREFIX_MAX_CHAR_LENGTH] = {
    {"B"}, {"kB"}, {"MB"}, {"GB"}, {"TB"}, {"PB"}, {"EB"},
};

bool get_index(const Binary_Value *binary_value, uint8_t *index);
bool change_prefix(Binary_Value *binary_value, const uint8_t index);

bool get_index(const Binary_Value *binary_value, uint8_t *index) {
  for (uint32_t i = 0; i < MAX_PREFIX_NB; i++)
    if (strcmp(binary_value->prefixe, prefixes[i]) == 0) {
      *index = i;
      return true;
    }

  printf("Error get_index");
  return false;
}

bool change_prefix(Binary_Value *binary_value, const uint8_t index) {
  if (index >= MAX_PREFIX_NB)
    return false;

  strcpy(binary_value->prefixe, prefixes[index]);

  return true;
}

bool bv_format(Binary_Value *binary_value) {
  uint8_t index = 0;

  if (!get_index(binary_value, &index))
    return false;

  while (binary_value->value >= 1000) {
    binary_value->value /= 1024;
    index++;
    if (index >= MAX_PREFIX_NB)
      return false;
  }

  if (!change_prefix(binary_value, index))
    return false;

  return true;
}
