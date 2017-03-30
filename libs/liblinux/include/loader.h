#ifndef __LOADER_LINUX_H
#define __LOADER_LINUX_H

/**
 * Module information structure
 **/
struct module {
    // Misc module info
    char *name;
    char *author;
    char *license;
    char *version;

    int (*init) (void);
    int (*exit) (void);
    void* dlhandle;
};

/**
 * List of all known modules
 **/
struct module_list {
    struct module* mod;
    struct module_list* next;
};

/**
 * Function type that can be applied to a module and fail
 **/
typedef int (*module_handler_t) (struct module*);

/**
 * Metadata fetcher function
 **/
typedef const char* (*metadata_fetcher_t) (void *dlhandle);

/**
 * Appends a module to the list of registered modules
 * @param list the current module list
 * @param elem the module to append
 **/
int append_module(struct module_list *list, struct module* mod);

/**
 * Reads a given metadata from the module refered to by dlhandle
 * @param dest a pointer to a pointer to the destination
 * @param dlhandle dlopen handle to the module shared object
 * @param fetcher the function used to fetch the metadata
 **/
int load_metadata(char** dest, void* dlhandle, metadata_fetcher_t fetcher);

/**
 * Apply a function to all modules currently registered
 * @param list the list of registered modules
 * @param func the function to apply to each module
 **/
int traverse_list(struct module_list *list, int (*func) (struct module* mod));

/**
 * Loads init and exit functions from a given shared object
 * @param obj path to the library on the initrd
 **/
struct module* find_module(const char* obj);

/**
 * Loads all modules currently registered
 * @param modules the list of modules to load
 **/
int init_modules(struct module_list* modules);

#define MODULE_LOAD_FCN    "load_module"
#define MODULE_UNLOAD_FCN  "unload_module"
#define MODULE_NAME_FCN    "module_name"
#define MODULE_LICENSE_FCN "module_license"

#define register_init(x)                        \
    {                                           \
        mod_#x->init = x                        \
    }

// FIXME: for now we assume every call to module_init is done before call
// to module_exit

#define register_exit(x)                        \
    {                                           \
        mod_#x->exit = x;                       \
    }


//Unsupported in linux kernel
#define MODULE_SUPPORTED_DEVICE(name)

// We don't really care about module tags...
#define MODULE_INFO(tag, info)
#define MODULE_SOFTDEP(dep) // TODO: handle this to find out about module deps?

// Compat for module metadata
#define MODULE_NAME(name) char* module_name(void) { return name; }
#define MODULE_VERSION(ver) char* module_version(void) { return ver; }
#define MODULE_LICENSE(lic) char* module_license(void) { return lic; }

// The module loader can fetch pointers to load_module and unload_module from the .so
#define module_init(x)                          \
    struct module _mod_#x;                      \
    int MODULE_LOAD_FCN(struct module *mod)             \
    {                                           \
        register_init(x);                       \
    }

#define module_exit(x)                          \
    int MODULE_UNLOAD_FCN(struct module *mod)       \
    {                                           \
        register_exit(x);                       \
    }

#endif
