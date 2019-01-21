#ifndef _wobj_h_
#define _wobj_h_
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define CLOFN_PHSIZE_MAX 1024
#define _CLOFN_SCIENCE_NUMBER 0x58ffffbffdffffafULL

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/mman.h>
#include <sys/user.h>
    static inline bool _clofn_active_memory(void *ptr, size_t size) {
        return mprotect((void *)(((size_t)ptr >> PAGE_SHIFT) << PAGE_SHIFT), size, PROT_READ | PROT_EXEC | PROT_WRITE) == 0;
    }
#elif defined(_WIN32)
#if (defined(DEBUG) || defined(_DEBUG)) && defined(_MSC_VER)
#error Clofn: not support MSVC on debug mode!
#endif
#include <windows.h>
    static inline bool _clofn_active_memory(void *ptr, size_t size) {
        DWORD old_protect;
        return VirtualProtect(ptr, size, PAGE_EXECUTE_READWRITE, &old_protect);
    }
#else
#error Clofn: not support this OS!
#endif

    static void *__wobj_new_clofn__(void *prototype, size_t *phsize, void *data) {
#ifdef CLOFN_PRINT_HEADER
        printf("Clofn: prototype header (%p) { ", prototype);
#endif

        size_t offset = *phsize;
        if (!offset) {
            for (; offset < CLOFN_PHSIZE_MAX; offset++) {
                if (*(size_t *)((uintptr_t)prototype + offset) == (size_t)_CLOFN_SCIENCE_NUMBER) {
                    if (!*phsize) {
                        *phsize = offset;
                    }

#ifdef CLOFN_PRINT_HEADER
                    printf("} @%u+%u\n", offset, sizeof(uintptr_t));
#endif

                    goto mk;
                }
#ifdef CLOFN_PRINT_HEADER
                else printf("%02X ", *(uint8_t *)(prototype + offset));
#endif
            }
#ifdef CLOFN_PRINT_HEADER
            puts("...");
#endif

            printf("Clofn: could't find closure declaration at prototype function (%p)!\n", prototype);
            return NULL;
        }
#ifdef CLOFN_PRINT_HEADER
        else {
            printf("Clofn: prototype header (%p) { ", prototype);
            for (size_t i = 0; i < phsize; i++) {
                printf("%02X ", *(uint8_t *)(prototype + i));
            }
            printf("} @%u+%u\n", offset, sizeof(uintptr_t));
        }
#endif

    mk:;

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__) || defined(_WIN64)
        size_t ihsize = offset + sizeof(void *) * 2 + 5;
#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(__i386) || defined(__i686__) || defined(__i686) || defined(_WIN32)
        size_t ihsize = offset + sizeof(void *) * 2 + 1;
#else
#error Clofn: not support this arch!
#endif

        void *instance = malloc(ihsize);
        if (!_clofn_active_memory(instance, ihsize)) {
            puts("Clofn: could't change memory type of C.malloc allocated!");
            free(instance);
            return NULL;
        }
        memcpy(instance, prototype, offset);
        uintptr_t current = (uintptr_t)instance + offset;
        *(void **)current = data;
        current += sizeof(void *);

#if defined(__x86_64__)  || defined(__x86_64)  || defined(__amd64)  || defined(__amd64__) || defined(_WIN64)
        *(uint8_t *)current = 0x50;
        current++;
        *(uint8_t *)current = 0x48;
        current++;
        *(uint8_t *)current = 0xB8;
        current++;
        *(uintptr_t *)current = (uintptr_t)prototype + offset + sizeof(uintptr_t) - 1; // 0x58 in _CLOFN_SCIENCE_NUMBER
        current += sizeof(uintptr_t);
        *(uint16_t *)current = 0xE0FF;
#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(__i386) || defined(__i686__) || defined(__i686) || defined(_WIN32)
        *(uint8_t *)current = 0xE9;
        current++;
        *(uintptr_t *)current = ((uintptr_t)prototype + offset + sizeof(uintptr_t)) - ((uintptr_t)instance + ihsize);
#endif

#ifdef CLOFN_PRINT_HEADER
        printf("Clofn: instance header (%p) { ", instance);
        for (size_t i = 0; i < ihsize; i++) {
            printf("%02X ", *(uint8_t *)(instance + i));
        }
        printf("}\n");
#endif

        return instance;
    }

#if defined(wobj_use_other)
#define _wobj_root_ wobj_use_other
#elif (defined (wobj_this) || (this) || (use_this)) && !defined (__cplusplus)
#define _wobj_root_ this
#else
#define _wobj_root_ self
#endif

typedef struct _wobj_node_t {
    void *ptr;
    struct _wobj_node_t *next;
} wobj_node_t;

static void *__wobj_malloc__(wobj_node_t **node, size_t size) {
    wobj_node_t *c = *node;
    *node = malloc(sizeof(wobj_node_t));
    (*node)->ptr = malloc(size);
    (*node)->next = c;
    return (*node)->ptr;
}

static void *__wobj_calloc__(wobj_node_t **node, size_t count, size_t size) {
    wobj_node_t *c = *node;
    *node = malloc(sizeof(wobj_node_t));
    (*node)->ptr = calloc(count, size);
    (*node)->next = c;
    return (*node)->ptr;
}

static void *__wobj_realloc__(wobj_node_t **node, void *ptr, size_t new_size) {
    for (wobj_node_t *c = *node, *p = NULL; c != NULL;) 
    {
        wobj_node_t *n = c->next;
        if (c->ptr == ptr) {
            if (new_size <= 0) {
                free(ptr);
                if (p == NULL) {
                    free(*node);
                    *node = n;
                }
                else {
                    p->next = n; free(c);
                }
                return NULL;
            }
            else {
                return c->ptr = realloc(ptr, new_size);
            }
        }
        p = c;
        c = n;
    }
    return NULL;
}

#define wobj_malloc(name, size) __wobj_malloc__(&__wobj_##name##__node__, size)
#define wobj_calloc(name, count, size) __wobj_calloc__(&__wobj_##name##__node__, count, size)
#define wobj_realloc(name, ptr, new_size) __wobj_realloc__(&__wobj_##name##__node__, ptr, new_size)
#define wobj_unalloc(name, ptr) __wobj_realloc__(&__wobj_##name##__node__, ptr, 0)
#define wobj_alloc(name, size) memset(wobj_malloc(name, size), '\0', size) // malloc + memset

#define wobj(name) \
    static wobj_node_t *__wobj_##name##__node__ = NULL; \
    typedef struct __wobj_##name name; \
    typedef struct __wobj_##name

#define wobj_def(name, type, func, args, body) \
    static type __wobj_##name##_##func args { \
        volatile size_t __wobj_self = (size_t)_CLOFN_SCIENCE_NUMBER; \
        struct __wobj_##name *_wobj_root_ = (struct __wobj_##name *)__wobj_self; \
        body \
    } \
    static size_t __wobj_##name##_##func##__phsize__ = 0; \
    static size_t __wobj_##name##_##func##__phhash__ = 0;

#define wobj_init(name, args_new, body_init, body_free) \
    inline struct __wobj_##name * __wobj_##name##__init__ args_new { \
        struct __wobj_##name *_wobj_root_ = (struct __wobj_##name *)wobj_alloc(name, sizeof(struct __wobj_##name)); \
        if (!_wobj_root_) return NULL; \
        body_init \
        return _wobj_root_; \
    } \
    inline void __wobj_##name##__free__(struct __wobj_##name * _wobj_root_) { \
        if (!_wobj_root_) return; \
        body_free \
        for (wobj_node_t *__node__ = __wobj_##name##__node__;__node__ != NULL;) { \
            wobj_node_t *__next__ = __node__->next; \
            free(__node__->ptr); \
            free(__node__); \
            __node__ = __next__; \
            puts("cleaning..."); \
        } \
    }

#define wobj_set(name, func) \
    _wobj_root_->func = (void*)__wobj_new_clofn__(__wobj_##name##_##func, &(__wobj_##name##_##func##__phsize__), (void*)_wobj_root_); \
    { \
        wobj_node_t *__node__ = __wobj_##name##__node__; \
        __wobj_##name##__node__ = malloc(sizeof(wobj_node_t)); \
        __wobj_##name##__node__->ptr = _wobj_root_->func; \
        __wobj_##name##__node__->next = __node__; \
    }

#define wobj_new(name, var_name, ...) struct __wobj_##name *var_name = __wobj_##name##__init__(__VA_ARGS__)
#define wobj_new2(name, var_name) struct __wobj_##name *var_name = __wobj_##name##__init__
#define wobj_new3(name, ...) __wobj_##name##__init__(__VA_ARGS__)
#define wobj_new4(name) __wobj_##name##__init__
#define wobj_free(name, var_name) __wobj_##name##__free__(var_name)

#ifdef __cplusplus
}
#endif

#endif // !_wobj_h_
