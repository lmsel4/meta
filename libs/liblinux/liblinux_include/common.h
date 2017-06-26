#ifndef __COMMON_LIBLINUX_H
#define __COMMON_LIBLINUX_H

#include <sel4/sel4.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <vka/object.h>

#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>

extern simple_t simple;

extern vspace_t *vspace;

#define print(level, fmt, ...) fprintf(stderr, level " %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define debug(fmt, ...) print("DEBUG", fmt, ##__VA_ARGS__)
#define warn(fmt, ...) print("WARN", fmt, ##__VA_ARGS__)
#define fatal(fmt, ...) print("FATAL",  fmt, ##__VA_ARGS__)

#endif
