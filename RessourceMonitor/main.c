#include <pthread.h>
#include <statgrab.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

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
  if (SG_ERROR_NONE != sg_init(1)) {
    printf("Failed to initialize StatGrab\n");
    pthread_exit(NULL);
  }

  while (true) {
    print_cpu_usage();
    // print_ram_usage();
    // print_swap_usage();
    // print_network_usage();
    // print_disk_usage();
    // print_temperatures();
    // print_up_time();
    printf("\n");

    sleep(4);

    // getchar();
  }

  pthread_exit(NULL);
}

void print_cpu_usage() {
  double cpu_usage = 0;


  size_t entries = 0;

  sg_cpu_percents *cpu_percents =
      sg_get_cpu_percents_of(sg_new_diff_cpu_percent, &entries);

  cpu_usage = 100.0f - cpu_percents->idle;
  printf("CPU Usage: %.2f%%\n", cpu_usage);

  // sg_free_stats_buf(cpu_percents);

  fflush(stdout);
}

void print_ram_usage() {
  double ram_usage = 0;
  double ram_total = 0;

  size_t entries = 0;

  sg_mem_stats *mem_stats = sg_get_mem_stats(&entries);

  ram_usage = mem_stats->used / 1000000;
  ram_total = mem_stats->total / 1000000;

  ram_usage /= 1000;
  ram_total /= 1000;
  printf("RAM Usage: %.3f/%.3f GB\n", ram_usage, ram_total);

  // sg_free_stats_buf(mem_stats);

  fflush(stdout);
}

void print_swap_usage() {
  double swap_usage = 0;
  double swap_total = 0;

  size_t entries = 0;

  sg_mem_stats *mem_stats = sg_get_mem_stats(&entries);

  printf("Swap Usage: %.3f/%.3f GB\n", swap_usage, swap_total);

  // sg_free_stats_buf(mem_stats);

  fflush(stdout);
}

void print_network_usage() {
  double network_download = 0;
  double network_upload = 0;

  size_t entries = 0;

  sg_network_io_stats *network_stats = sg_get_network_io_stats_diff(&entries);

  for (uint32_t i = 0; i < entries; i++) {
    network_download = network_stats[i].rx / 1024;
    network_upload = network_stats[i].tx / 1024;
    network_download /= 1;
    network_upload /= 1;
    printf("Network\tDownload: %.3f kbps\tUpload: %.3f kbps\n",
           network_download, network_upload);
  }

  // sg_free_stats_buf(network_stats);

  fflush(stdout);
}

void print_disk_usage() {
  double fs_used = 0;
  double fs_size = 0;
  double fs_free = 0;
  double fs_available = 0;

  size_t entries = 0;

  sg_fs_stats *fs_stats = sg_get_fs_stats_diff(&entries);

  fs_used = fs_stats->used / 1000;
  fs_size = fs_stats->size / 1000;
  fs_free = fs_stats->free / 1000;
  fs_available = fs_stats->avail / 1000;

  fs_used /= 1;
  fs_size /= 1;
  fs_free /= 1;
  fs_available /= 1;

  printf("Disk\tUsed: %lld\tSize: %lld\tFree: %lld\tAvailable: %lld\n",
         fs_stats->used, fs_stats->size, fs_stats->free, fs_stats->avail);

  // sg_free_stats_buf(mem_stats);

  fflush(stdout);
}

void print_temperatures() {}

void print_up_time() {}
