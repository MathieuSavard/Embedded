#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int start_retrieve_weather(void);
void *task(void *arg);

pthread_t thread;

int main() {
  int ret = 0;

  printf("Weather Retriever Program Start\n");

  ret = start_retrieve_weather();
  if (ret) {
    return ret;
  }

  printf("Weather Retriever Program End\n");

  return 0;
}

int start_retrieve_weather() {
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
    getchar();
    break;
  }

  pthread_exit(NULL);
}