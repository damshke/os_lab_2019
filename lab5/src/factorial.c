#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <getopt.h>
#include <math.h>

struct parameters{
  pthread_t thread;
  int begin;
  int end;
  int mod;
};

static int res = 1;
static pthread_mutex_t fac_mtx = PTHREAD_MUTEX_INITIALIZER;


static int multiplication(int a, int b, int mod) {
  int res = 0;
  res = (a % mod) * (b % mod) % mod;
  return res;
}


static void threaded(struct parameters* f) {
  printf("Thread: id = %lu; begin: %d; end: %d\n", f->thread, f->begin, f->end - 1);
  for (int i = f->begin; i < f->end; i++) {
    pthread_mutex_lock(&fac_mtx);
    res = multiplication(res, i, f->mod);
    pthread_mutex_unlock(&fac_mtx);
  }
}

int main(int argc, char* argv[]) {
  int k = -1;
  int pnum = -1;
  int mod = -1;

  int current_optind = optind ? optind : 1;

  while (1) {
    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0} };

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            if (k < 1) {
              printf("k is a positive number\n");
              return -1;
            }
            break;
          case 1:
            pnum = atoi(optarg);
            if (pnum < 1) {
              printf("pnum is a positive number\n");
              return -1;
            }
            break;
          case 2:
            mod = atoi(optarg);
            if (mod < 1) {
              printf("mod is a positive number\n");
              return -1;
            }
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (k == -1 || pnum == -1 || mod == -1) {
    printf("Usage: %s --k \"num\" --mod \"num\" --pnum \"num\" \n", argv[0]);
    return 1;
  }

  if (pnum > k / 2) {
    pnum = k / 2;
    printf("Too much threads.\n");
  }

  float block = (float)k / pnum;
  struct parameters thread_pool[pnum];

  for (int i = 0; i < pnum; i++) {
    int begin = ((int)block * i) + 1;
    int end = (int)(block * (i + 1.f)) + 1;

    thread_pool[i].begin = begin;
    thread_pool[i].end = end;
    thread_pool[i].mod = mod;

    if (pthread_create(&thread_pool[i].thread, NULL, (void *)threaded, (void*)&thread_pool[i]) != 0) {
      printf("Error: cannot create new pthread\n");
      return -1;
    }
  }

  for (int i = 0; i < pnum; i++)
    if (pthread_join(thread_pool[i].thread, 0) != 0) {
      printf("Error: cannot join thread %d\n", i);
      return -1;
    }
    printf("%d! mod %d = %d\n", k, mod, res);
} 
