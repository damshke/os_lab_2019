got #include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void do_one_thing(int *);
void do_another_thing(int *);

int common = 0; 

int main() {
  pthread_t thread1, thread2;

  pthread_create(&thread1, NULL, (void*)do_one_thing, (void *)&common);
  pthread_create(&thread2, NULL, (void*)do_another_thing, (void *)&common);
  
  if (pthread_join(thread1, NULL) != 0) {
    perror("Error: cannot join first thread\n");
    exit(1);
    }
  if (pthread_join(thread2, NULL) != 0) {
    perror("Error: cannot join second thread\n");
    exit(1);
    }
  return 0;
}

void do_one_thing(int *pnum_times) {
  unsigned long k;
  int work;
  printf("Executing first function.\n");
  pthread_mutex_lock(&lock1);           // Execution step 1
  sleep(2);
  pthread_mutex_lock(&lock2);           // deadlock
  printf("doing one thing\n");
  work = *pnum_times;
  printf("counter = %d\n", work);
  work++; /* increment, but not write */
  for (k = 0; k < 500000; k++)
     ;                 /* long cycle */
  *pnum_times = work; /* write back */
  pthread_mutex_unlock(&lock2);
  pthread_mutex_unlock(&lock1);
  }

void do_another_thing(int *pnum_times) {
  printf("Executing second function.\n");
  unsigned long k;
  int work;
  pthread_mutex_lock(&lock2);           // Execution step 2
  pthread_mutex_lock(&lock1);
  printf("doing other thing\n");
  work = *pnum_times;
  printf("counter = %d\n", work);
  work++; /* increment, but not write */
  for (k = 0; k < 500000; k++)
     ;                 /* long cycle */
  *pnum_times = work; /* write back */
  pthread_mutex_unlock(&lock1);
  pthread_mutex_unlock(&lock2);
  }
