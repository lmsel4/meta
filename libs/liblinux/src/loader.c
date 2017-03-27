/**
 * A pretty hacky way to support dynamic linux module loading under seL4
 **/

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

// Libnux headers
#include <libnux/loader.h>

static int mod_count = 0;
static struct module_list all_modules = {
    .mod = NULL,
    .next = NULL
};

int append_module(struct module_list* list, struct module* elem)
{
    assert(elem);
    assert(list);

    struct module_list* n = malloc(sizeof(struct module_list));

    if (!n) {
        return -ENOMEM;
    }

    list->next = n;
    n->mod = elem;
    n->next = NULL;

    mod_count++;

    return 0;
}

int traverse_list(struct module_list *list, int (*func) (struct module* mod)) {
    assert(list);

    int res;
    struct module_list *current;

    for (current = list; current != NULL; current = current->next)
    {
        res = func(current->mod);

        if (res < 0)
        {
            return res;
        }

    }
    return 0;
}

/**
 * Loads a module
 **/
int load_module(struct module *mod) {
    assert(mod->init);
    return mod->init();
}

/**
 * Unloads a module
 **/
int unload_module(struct module *mod)
{
    assert(mod->exit);
    return mod->exit();
}

int unload_modules(struct module_list *list)
{
    return traverse_list(list, unload_module);
}

/**
 * Finds a symbol inside a lib and applies it to the given mod
 * @param dlhandle handle to the lib
 * @param sym name of the symbol to apply
 * @param mod the module to apply the symbol to
 **/
int find_sym_and_apply(void *dlhandle, const char *sym, struct module* mod)
{
    module_handler_t handler = dlsym(dlhandle, sym);

    if (!handler)
    {
        fprintf(stderr, "Unable to find symbol %s: %s\n", dlerror());
        return -1;
    }

    return handler(mod);
}

struct module* find_module(const char* obj)
{
    struct module* out = malloc(sizeof(struct module));
    void *dlhandle = NULL;
    module_handler_t loader;
    int res;

    dlhandle = dlopen(obj, RTLD_NOW);

    if (!dlhandle)
    {
        fprintf(stderr, "Error loading %s: %s\n", obj, dlerror());
        errno = ENOENT;
        return NULL;
    }

    out->dlhandle = dlhandle;

    res = find_sym_and_apply(dlhandle, MODULE_LOAD_FCN, out);

    if (res < 0)
    {
        fprintf(stderr, "Module %s failed to load: %s\n", obj, strerror(errno));
        free(out);
        return NULL;
    }

    res = find_sym_and_apply(dlhandle, MODULE_UNLOAD_FCN, out);

    if (res < 0)
    {
        fprintf(stderr, "Module %s failed to find unload func: %s\n", obj, strerror(errno));
        free(out);
        return NULL;
    }

    return out;
}

int init_modules(struct module_list* modules)
{
    return traverse_list(modules, load_module);
}
