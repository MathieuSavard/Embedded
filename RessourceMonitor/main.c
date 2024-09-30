#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "binary_value.h"

int start_display_ressources(void);
void *task(void *arg);

void print_cpu_usage(void);
void print_ram_usage(void);
void print_swap_usage(void);
void print_network_usage(void);
void print_disk_usage(void);
void print_temperatures(void);
void print_up_time(void);

pthread_t thread;

int main() {
  int ret = 0;

  printf("Ressources Monitor Program Start\n");

  ret = start_display_ressources();
  if (ret) {
    return ret;
  }

  printf("Ressources Monitor Program End\n");

  return 0;
}

int start_display_ressources() {
  if (pthread_create(&thread, NULL, task, NULL)) {
    printf("Error creating thread\n");
    return 1;
  }

  if (pthread_join(thread, NULL)) {
    printf("Error joining thread\n");
    return 2;
  }
}

void *task(void *arg) {
  while (true) {
    print_cpu_usage();
    print_ram_usage();
    print_swap_usage();
    print_network_usage();
    print_disk_usage();
    print_temperatures();
    print_up_time();
    printf("\n");

    sleep(1);
  }

  pthread_exit(NULL);
}

void print_cpu_usage() {
  static bool first_pass_init = true;
  static unsigned long long prev_idle = 0;
  static unsigned long long prev_total = 0;

  unsigned long long user = 0;
  unsigned long long nice = 0;
  unsigned long long system = 0;
  unsigned long long idle = 0;
  unsigned long long iowait = 0;
  unsigned long long irq = 0;
  unsigned long long softirq = 0;
  unsigned long long total = 0;

  FILE *file = fopen("/proc/stat", "r");
  if (file == NULL) {
    printf("Error opening file \"/proc/stat\"\n");
    return;
  }

  if (fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu", &user, &nice,
             &system, &idle, &iowait, &irq, &softirq) != 7) {
    printf("Failed to parse CPU statistics\n");
    return;
  }
  fclose(file);

  total = user + nice + system + idle + iowait + irq + softirq;

  if (!first_pass_init) {
    printf("CPU: %.2f%%\n",
           100.0 * (1.0 - (double)((idle + iowait) - prev_idle) /
                              (total - prev_total)));
  }

  prev_idle = idle + iowait;
  prev_total = total;

  first_pass_init = false;

  fflush(stdout);
}

void print_ram_usage() {
  char bufferX[32] = {};
  Binary_Value total = {0, "kB"};
  Binary_Value free = {0, "kB"};
  Binary_Value buffer = {0, "kB"};
  Binary_Value cache = {0, "kB"};
  Binary_Value slab = {0, "kB"};
  Binary_Value used = {0, "kB"};

  FILE *file = fopen("/proc/meminfo", "r");
  if (file == NULL) {
    printf("Error opening file \"/proc/meminfo\"\n");
    return;
  }

  while (true) {
    memset(bufferX, 0, sizeof(bufferX));
    if (fgets(bufferX, sizeof(bufferX), file) == NULL)
      break;

    if (strncmp(bufferX, "MemTotal", strlen("MemTotal")) == 0)
      sscanf(bufferX, "MemTotal: %Lf kB", &total.value);

    if (strncmp(bufferX, "MemFree", strlen("MemFree")) == 0)
      sscanf(bufferX, "MemFree: %Lf kB", &free.value);

    if (strncmp(bufferX, "Buffers", strlen("Buffers")) == 0)
      sscanf(bufferX, "Buffers: %Lf kB", &buffer.value);

    if (strncmp(bufferX, "Slab", strlen("Slab")) == 0)
      sscanf(bufferX, "Slab: %Lf kB", &slab.value);

    if (strncmp(bufferX, "Cached", strlen("Cached")) == 0)
      sscanf(bufferX, "Cached: %Lf kB", &cache.value);
  }

  fclose(file);

  used.value =
      total.value - free.value - buffer.value - cache.value - slab.value;

  bv_format(&total);
  bv_format(&used);

  printf("RAM: %.3Lf %s/%.3Lf %s\n", used.value, used.prefixe, total.value,
         total.prefixe);

  fflush(stdout);
}

void print_swap_usage() {}

void print_network_usage() {}

void print_disk_usage() {}

void print_temperatures() {}

void print_up_time() {}
