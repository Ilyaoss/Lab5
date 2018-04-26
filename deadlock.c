#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;

void fuction_1();
void fuction_2();

int main(int argc, char **argv) {
  pthread_t thread_1; 
  pthread_t thread_2;

  if (pthread_create(&(thread_1), NULL, (void*)fuction_1, NULL) != 0)
  {
      perror("pthread_create");
      exit(1);
  }

  if (pthread_create(&(thread_1), NULL, (void*)fuction_2, NULL) != 0)
  {
      perror("pthread_create");
      exit(1);
  }

  if (pthread_join(thread_1, NULL) != 0)
  {
      perror("pthread_join");
      exit(1);
  }

  if (pthread_join(thread_2, NULL) != 0)
  {
      perror("pthread_join");
      exit(1);
  }

  return 0;
}

void fuction_1()
{
   pthread_mutex_lock(&mutex_1);
   printf("thread_1 Mutex 1\n");
   sleep(1);
   
   pthread_mutex_lock(&mutex_2);
   printf("thread_1 Mutex 2\n");
   pthread_mutex_unlock(&mutex_2);
   pthread_mutex_unlock(&mutex_1);
}

void fuction_2() 
{
   pthread_mutex_lock(&mutex_2);
   printf("thread_2 Mutex 2\n");
   sleep(1);
   
   pthread_mutex_lock(&mutex_1);
   printf("thread_2 Mutex 1\n");
   pthread_mutex_unlock(&mutex_1);
   pthread_mutex_unlock(&mutex_2);
}