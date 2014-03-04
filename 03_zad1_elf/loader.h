#ifndef LOADER_H
#define LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

int library_load(const char *name);
void *library_getsym(const char *name);

#ifdef __cplusplus
}
#endif

#endif
