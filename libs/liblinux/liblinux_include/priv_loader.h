#ifndef __PRIV_LOADER_H
#define __PRIV_LOADER_H

#include <loader.h>

#include <linux/module.h>

struct module priv_module = {
    .name = "lmsel4",
};

#undef THIS_MODULE

#define THIS_MODULE (&priv_module)

#endif
