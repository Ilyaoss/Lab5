#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <getopt.h>
#include <pthread.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
long long result = 1;

struct func_val
{
    long long start;
    long long end;
    long long mod;
    long long pnum;
};

void* thread_factorial(void* a)
{
    struct func_val *t = (struct func_val*)a;
    long long start = t->start;
    long long end = t->end;
    long long mod = t->mod;
    long long pnum =  t->pnum;
    //printf("%lld %lld\n",start, end);
    for(long long i = start; i<=end; i+=pnum)
    {
        pthread_mutex_lock(&mutex1);
        result = (result * (i%mod))%mod;
        pthread_mutex_unlock(&mutex1);
        
    }
};


int main(int argc, char **argv)
{
    long long k = -1;
    long long mod = -1;
    long long pnum = -1;
    while (true)
    {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"k", required_argument, 0, 0},
            {"mod", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "k", options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            switch (option_index)
            {
            case 0:
                k = atoi(optarg);
                if (k <= 0)
                {
                    printf("k must be positive");
                    return 1;
                }
                break;
            case 1:
                mod = atoi(optarg);
                if (mod <= 1)
                {
                    printf("mod must be bigger than 1");
                    return 1;
                }
                break;
            case 2:
                pnum = atoi(optarg);
                if (pnum <= 0)
                {
                    printf("pnum must be more or equal than 1");
                    return 1;
                }
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
            break;
        case 'k':
            k = atoi(optarg);
            if (k <= 0)
            {
                printf("k must be positive");
                return 1;
            }
            break;

        case '?':
            break;

        default:
            printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc)
    {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (k == -1 || mod == -1 || pnum == -1)
    {
        printf("Usage: %s --k \"num\" --mod \"num\" --pnum \"num\" \n",
               argv[0]);
        return 1;
    }
    pthread_t *threads = malloc(pnum*sizeof(pthread_t));
    struct func_val* t = malloc(pnum*sizeof(struct func_val));
    
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    
    for(long long i = 1; i<=pnum; i++)
    {
        t[i-1].start = i;
        t[i-1].end = k;
        t[i-1].mod = mod;
        t[i-1].pnum = pnum;        
        pthread_create( &threads[i-1], NULL, thread_factorial, &t[i-1]);
    }
    for(long long i = 0; i<pnum; i++)
    {
        pthread_join( threads[i], NULL);
    }
    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    long long res_seq = 1;
    struct timeval start_time1;
    gettimeofday(&start_time1, NULL);
    for(long long i = 1; i <= k;i++)
    {
        res_seq = (res_seq*(i%mod))%mod;
    }
    struct timeval finish_time1;
    gettimeofday(&finish_time1, NULL);

    double elapsed_time1 = (finish_time1.tv_sec - start_time1.tv_sec) * 1000.0;
    elapsed_time1 += (finish_time1.tv_usec - start_time1.tv_usec) / 1000.0;

    printf("factorial_seq %lld mod %lld = %lld\n",k, mod, res_seq);
    printf("Elapsed time seq: %fms\n", elapsed_time1);
    
    printf("factorial parallel %lld mod %lld = %lld\n",k, mod, result);
    printf("Elapsed time parallel: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}