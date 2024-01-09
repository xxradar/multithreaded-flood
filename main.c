#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAX_THREADS 100
#define THREAD_STACK_SIZE (1024 * 1024)

void *thread_function(void *arg) {
  int sockfd;
  struct sockaddr_in servaddr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    pthread_exit(NULL);
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(8080);
  if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) < 0) {
    perror("inet_pton");
    pthread_exit(NULL);
  }

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    pthread_exit(NULL);
  }

  printf("Thread %lu connected to 127.0.0.1:8080\n", (unsigned long)pthread_self());
  close(sockfd); // Explicitly call close()

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <ip_address> <port>\n", argv[0]);
    return 1;
  }

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE);

  pthread_t threads[MAX_THREADS];
  int i;

  for (i = 0; i < MAX_THREADS; i++) {
    if (pthread_create(&threads[i], &attr, thread_function, NULL) != 0) {
      perror("pthread_create");
      return 1;
    }
  }

  for (i = 0; i < MAX_THREADS; i++) {
    if (pthread_join(threads[i], NULL)) {
      perror("pthread_join");
      return 1;
    }
  }

  return 0;
}
