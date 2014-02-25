#include "mutex.h"
#include <pthread.h>
#include <stdio.h>

#define NTHREAD 8
#define NREPEAT 10000000

struct mutex mutex;
int ctr = 0;

void *tmain(void *arg) {
        int i;
        for (i = 0; i < NREPEAT; i++) {
                mutex_lock(&mutex);
                ctr++;
                mutex_unlock(&mutex);
        }
        return 0;
}

int main() {
        pthread_t thr[NTHREAD];
        int i;
        mutex_init(&mutex);
        for (i = 0; i < NTHREAD; i++) {
                pthread_create(&thr[i], 0, tmain, 0);
        }
        for (i = 0; i < NTHREAD; i++) {
                pthread_join(thr[i], 0);
        }
        printf("%d\n", ctr);
        return 0;
}
