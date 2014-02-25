#ifndef MUTEX_H
#define MUTEX_H

struct mutex {
        int val;
};

void mutex_init(struct mutex *mutex);
void mutex_lock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);

#endif
