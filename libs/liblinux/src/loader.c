/**
 * A pretty hacky way to support dynamic linux module loading under seL4
 **/

#undef NULL /* Avoid name clashing with kernel */
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

extern void * kmalloc(size_t, unsigned);

// Libnux headers
#include <loader.h>

// FIXME: avoid name clash with Linux
struct FILE;

#define FILE struct FILE

extern FILE* stderr;

int fprintf(FILE*, const char*, ...);

struct module_list all_modules = {
    .mod = NULL,
    .next = NULL
};

int append_module(struct module_list* list, struct seL4_Module* elem)
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

    return 0;
}

int load_metadata(char** dest, void *dlhandle, metadata_fetcher_t fetcher)
{
    const char* ptr = fetcher(dlhandle);
    size_t len;

    if (!ptr)
    {
        *dest = NULL;
        return -ENOENT;
    }

    len = strlen(ptr) + 1;
    *dest = calloc(len, sizeof(char));
    strncpy(*dest, ptr, len);

    return 0;
}

int traverse_list(struct module_list *list, int (*func) (struct seL4_Module* mod))
{
    assert(list);

    int res;
    struct module_list *cur;

    for (cur = list; cur != NULL; cur = cur->next)
    {
        res = func(cur->mod);

        if (res < 0)
        {
            return res;
        }

    }
    return 0;
}

int modulectl_set_param(const char* name, void *value, void *mhandle)
{
    unsigned int len = strlen("PARAM_SETTER_PREFIX") + strlen(name);
    char *setter = kmalloc(len + 1, 0);

    strncpy(setter, "PARAM_SETTER_PREFIX", len);
    strncat(setter, name, len);

    module_param_setter set_func =  dlsym(mhandle, setter);

    if (!set_func)
    {
        printk("Param %s not found\n", name);
        return -ENOENT;
    }

    set_func(value);
    return 0;
}

/**
 * Loads a module
 **/
int load_module(struct seL4_Module *mod) {
    assert(mod->init);
    return mod->init();
}

/**
 * Unloads a module
 **/
int unload_module(struct seL4_Module *mod)
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
int find_sym_and_apply(void *dlhandle, const char *sym, struct seL4_Module* mod)
{
    module_handler_t handler = dlsym(dlhandle, sym);

    if (!handler)
    {
        fprintf(stderr, "Unable to find symbol %s: %s\n", sym, dlerror());
        return -1;
    }

    return handler(mod);
}

struct seL4_Module* module_by_name(const char* obj)
{
    struct seL4_Module* out = malloc(sizeof(struct seL4_Module));
    void *dlhandle = NULL;
    int res;

    dlhandle = dlopen(obj, RTLD_NOW);

    if (!dlhandle)
    {
        fprintf(stderr, "Error loading %s: %s\n", obj, dlerror());
        errno = ENOENT;
        return NULL;
    }

    out->dlhandle = dlhandle;

    res = find_sym_and_apply(dlhandle, "MODULE_LOAD_FCN", out);

    if (res < 0)
    {
        fprintf(stderr, "Module %s failed to load: %s\n", obj, strerror(errno));
        free(out);
        return NULL;
    }

    res = find_sym_and_apply(dlhandle, "MODULE_UNLOAD_FCN", out);

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
