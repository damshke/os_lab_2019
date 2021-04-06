#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"
// lab 4

void killer(int sig) // функция для убийства дочерних процессов
{
    kill(0, SIGKILL);
    printf("timeout\n\n");
}
 
int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1;
  bool with_files = false;
 
  while (true) {
    int current_optind = optind ? optind : 1;
 
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
 
    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);
 
    if (c == -1) break;
 
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed is a positive number\n");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("array_size is a positive number\n");
                 return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg); // количество потоков
            if (pnum <= 0) { 
                printf("pnum is a positive number\n");
                return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4: // для lab 4
            timeout = atoi(optarg);
            if (timeout <= 0) { 
                printf("timeout is a positive number\n");
                return 1;
            }
            printf("Timeout = %d\n", timeout);
            break;
 
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
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
 
  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }
 
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
 
  int i;
  for (i = 0; i < array_size; i++)   // на всякий случай вывод массива
  {
    printf("%d ", array[i]);
  }
  printf("\n");
 
  int part_size = array_size / pnum; // делим обработку массива между потоками
 
  FILE* file; // для файла
  int** pipe_array; //
 
 
  if (with_files) { // если работает с файлом
      file = fopen("file.txt", "w");
  }
  else { // если работаем с pipe
      pipe_array = (int**)malloc(sizeof(int*) * pnum); 
    }
 
  int active_child_processes = 0; // число активных дочерних процессов
 
 
  struct timeval start_time; // время начала
  gettimeofday(&start_time, NULL);
 
  // подключаем многопоточность
 
  for (int i = 0; i < pnum; i++) {
 
      if (!with_files) {
       
          pipe_array[i] = (int*)malloc(sizeof(int) * 2); // создание ключа
       
          if (pipe(pipe_array[i]) < 0) {
              printf("Error with pipe");
              return 1;
          }
      }
      
      pid_t child_pid = fork();
      
      if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        struct MinMax sub_min_max;
        // parallel somehow
 
        if (i != pnum - 1) { // локальные структуры, каждая из котрых имеет свои min и max
            sub_min_max = GetMinMax(array, i * part_size, (i + 1) * part_size);
        }
        else {
            sub_min_max = GetMinMax(array, i * part_size, array_size);
        }
 
        if (with_files) { // работа с файлом
          // use files here
        
          fwrite(&sub_min_max.min, sizeof(int), 1 , file); // запись в файл минимума
          fwrite(&sub_min_max.max, sizeof(int), 1 , file); // запись в файл максимума
         
        } 
        else {
          // use pipe here
          close(pipe_array[i][0]); // закрываем один конец
          write(pipe_array[i][1], &sub_min_max.min, sizeof(int)); // засовываем min
          write(pipe_array[i][1],&sub_min_max.max, sizeof(int)); // засовываем max
          close(pipe_array[i][1]); // закрываем совсем
        }
        return 0;
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }
 
  if (with_files) {
      fclose(file);
      file = fopen("file.txt", "r");
  }

  // lab_4
  printf("Timeout now: %d\n", timeout);   // выводит время перед началом таймаута

  if (timeout > 0)
  {
    signal(SIGALRM, killer);
    alarm(timeout);
    printf("Timeout is %d \n", timeout);
    sleep(3);                        
  }
 
  while (active_child_processes > 0) {
    // your code here
    wait(NULL);
    active_child_processes -= 1;
  }
 
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
 
  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
 
    if (with_files) {
      // read from files
      fread(&min, sizeof(int), 1, file);
      fread(&max, sizeof(int), 1, file);
    } else {
      // read from pipes
      read(pipe_array[i][0], &min, sizeof(int));
      read(pipe_array[i][0], &max, sizeof(int));
 
      close(pipe_array[i][0]);

      free(pipe_array[i]);
      
    }
 
    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }
 
  if (with_files) { 
      fclose(file);
      remove("file.txt");
  }
  else {
      free(pipe_array);
  }
  
  // структура завершения подсчета времени
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);
 
  // считаем, сколько времени пршло 
 
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  
  // освобождаем массив
  free(array);
  
 
  // выводим min, max, время
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}