#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "binary_value.h"

#define PERIOD 1U

int start_display_ressources(void);
void *task(void *arg);

void print_cpu_usage(void);
void print_ram_usage(void);
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
    print_network_usage();
    print_disk_usage();
    print_temperatures();
    print_up_time();
    printf("\n");

    sleep(PERIOD);
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

  if (!first_pass_init)
    printf("CPU: %.2f%%\n",
           100.0 * (1.0 - (double)((idle)-prev_idle) / (total - prev_total)));

  prev_idle = idle;
  prev_total = total;

  first_pass_init = false;

  fflush(stdout);
}

void print_ram_usage() {
  char buffer[128] = {};

  Binary_Value ram_total = {0, "kB"};
  Binary_Value ram_available = {0, "kB"};
  Binary_Value ram_used = {0, "kB"};

  Binary_Value swap_total = {0, "kB"};
  Binary_Value swap_free = {0, "kB"};
  Binary_Value swap_used = {0, "kB"};

  FILE *file = fopen("/proc/meminfo", "r");
  if (file == NULL) {
    printf("Error opening file \"/proc/meminfo\"\n");
    return;
  }

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    if (fgets(buffer, sizeof(buffer), file) == NULL)
      break;

    if (strncmp(buffer, "MemTotal", strlen("MemTotal")) == 0)
      sscanf(buffer, "MemTotal: %Lf kB", &ram_total.value);

    if (strncmp(buffer, "MemAvailable", strlen("MemAvailable")) == 0)
      sscanf(buffer, "MemAvailable: %Lf kB", &ram_available.value);

    if (strncmp(buffer, "SwapTotal", strlen("SwapTotal")) == 0)
      sscanf(buffer, "SwapTotal: %Lf kB", &swap_total.value);

    if (strncmp(buffer, "SwapFree", strlen("SwapFree")) == 0)
      sscanf(buffer, "SwapFree: %Lf kB", &swap_free.value);
  }

  fclose(file);

  ram_used.value = ram_total.value - ram_available.value;

  bv_format(&ram_total);
  bv_format(&ram_used);

  swap_used.value = swap_total.value - swap_free.value;

  bv_format(&swap_total);
  bv_format(&swap_used);

  printf("RAM: %.3Lf %s/%.3Lf %s\n", ram_used.value, ram_used.prefixe,
         ram_total.value, ram_total.prefixe);

  if (swap_used.value)
    printf("Swap: %.3Lf %s/%.3Lf %s\n", swap_used.value, swap_used.prefixe,
           swap_total.value, swap_total.prefixe);

  fflush(stdout);
}

void print_network_usage() {
  static bool first_pass_init = true;
  static unsigned long prev_rx_bytes_total = 0;
  static unsigned long prev_tx_bytes_total = 0;

  char buffer[128] = {};
  char interface[16] = {};

  unsigned long rx_bytes = 0;
  unsigned long tx_bytes = 0;

  unsigned long rx_bytes_total = 0;
  unsigned long tx_bytes_total = 0;

  Binary_Value rx = {0, "B"};
  Binary_Value tx = {0, "B"};

  FILE *file = fopen("/proc/net/dev", "r");
  if (file == NULL) {
    printf("Error opening file \"/proc/net/dev\"\n");
    return;
  }

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    if (fgets(buffer, sizeof(buffer), file) == NULL)
      break;

    if (sscanf(buffer, "%s %lu %*u %*u %*u %*u %*u %*u %*u %lu", interface,
               &rx_bytes, &tx_bytes) == 3)
      if (strcmp(interface, "lo:") != 0) {
        rx_bytes_total += rx_bytes;
        tx_bytes_total += tx_bytes;
      }
  }

  fclose(file);

  if (!first_pass_init) {
    rx.value = rx_bytes_total - prev_rx_bytes_total;
    tx.value = tx_bytes_total - prev_tx_bytes_total;

    bv_format(&rx);
    bv_format(&tx);

    printf("Network: R:%.3Lf %s S:%.3Lf %s\n", rx.value, rx.prefixe, tx.value,
           tx.prefixe);
  }

  prev_rx_bytes_total = rx_bytes_total;
  prev_tx_bytes_total = tx_bytes_total;

  first_pass_init = false;

  fflush(stdout);
}

void print_disk_usage() {}

void print_temperatures() {}

void print_up_time() {}
