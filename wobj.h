/*
 * Copyright (c) 2019, wuuyi <wuuyi123@gmail.com> All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of LCUI nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
#if defined (_MSC_VER)
#if defined(DEBUG) || defined(_DEBUG)
#error Clofn: not support MSVC on debug mode!
#endif
#pragma warning(disable : 4996)
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

    struct __wobj_mem__ {
        void *ptr;
        struct __wobj_mem__ *next;
    };

    static void *__wobj_malloc__(struct __wobj_mem__ **mem, size_t size)
    {
        struct __wobj_mem__ *c = *mem;
        *mem = malloc(sizeof(struct __wobj_mem__));
        (*mem)->ptr = malloc(size);
        (*mem)->next = c;
        return (*mem)->ptr;
    }

    static void *__wobj_calloc__(struct __wobj_mem__ **mem, size_t count, size_t size)
    {
        struct __wobj_mem__ *c = *mem;
        *mem = malloc(sizeof(struct __wobj_mem__));
        (*mem)->ptr = calloc(count, size);
        (*mem)->next = c;
        return (*mem)->ptr;
    }

    static void *__wobj_realloc__(struct __wobj_mem__ **mem, void *ptr, size_t new_size)
    {
        if (!*mem) return NULL;
        for (struct __wobj_mem__ *c = *mem, *p = NULL; c != NULL;)
        {
            struct __wobj_mem__ *n = c->next;
            if (c->ptr == ptr) {
                if (new_size <= 0) {
                    free(ptr);
                    if (p == NULL) {
                        free(*mem);
                        *mem = n;
                    }
                    else
                        p->next = n; free(c);
                    return NULL;
                }
                else
                    return c->ptr = realloc(ptr, new_size);
            }
            p = c;
            c = n;
        }
        return NULL;
    }

#define __wobj_va_args__(...) ,##__VA_ARGS__

#define wobj(name, public_body, private_body, ...) \
	typedef struct __wobj_##name##__public__ public_body *name; \
	typedef name *lp##name __wobj_va_args__(__VA_ARGS__); \
	struct __wobj_##name { \
		struct public_body; \
		struct private_body; \
	}; \
	struct __wobj_##name##__data__ { \
		struct public_body; \
		struct private_body; \
		struct __wobj_mem__ *__mem__; \
	}; \

#define wobj_np(name, public_body, ...) \
	typedef struct __wobj_##name##__public__ public_body *name; \
	typedef name name __wobj_va_args__(__VA_ARGS__); \
	struct __wobj_##name { \
		struct public_body; \
	}; \
	struct __wobj_##name##__data__ { \
		struct public_body; \
		struct __wobj_mem__ *__mem__; \
	}; \
    
#define wobj_def(name, type, func, args, body) \
    static type __wobj_##name##_##func args { \
        volatile size_t ___cl_size___ = (size_t)_CLOFN_SCIENCE_NUMBER; \
        struct __wobj_##name##__data__ *___wobj___ = (struct __wobj_##name##__data__*)___cl_size___; \
        struct __wobj_##name *_wobj_root_ = (struct __wobj_##name *)___wobj___; \
        body \
    } \
    static size_t __wobj_##name##_##func##__phsize__ = 0; \
    static size_t __wobj_##name##_##func##__phhash__ = 0;

#define wobj_init(name, args_new, body_init, body_free) \
    struct __wobj_##name##__public__ *__wobj_##name##__init__ args_new { \
        struct __wobj_##name##__data__ *___wobj___ = (struct __wobj_##name##__data__*)malloc(sizeof(struct __wobj_##name##__data__)); \
        if (!___wobj___) return NULL; \
        ___wobj___->__mem__ = NULL; \
        struct __wobj_##name *_wobj_root_ = (struct __wobj_##name *)___wobj___; \
        body_init \
        return (struct __wobj_##name##__public__*)_wobj_root_; \
    } \
    void __wobj_##name##__free__(struct __wobj_##name##__public__ * _wobj_root_) { \
        if (!_wobj_root_) return; \
        struct __wobj_##name##__data__ *___wobj___ = (struct __wobj_##name##__data__*)_wobj_root_; \
        body_free \
        for (struct __wobj_mem__ *mem = ___wobj___->__mem__;mem != NULL;) { \
            struct __wobj_mem__ *next = mem->next; \
            free(mem->ptr); \
            free(mem); \
            mem = next; \
        } \
        free(___wobj___); \
    }

#define wobj_set(name, func) \
    _wobj_root_->func = (void*)__wobj_new_clofn__(__wobj_##name##_##func, &(__wobj_##name##_##func##__phsize__), (void*)_wobj_root_); \
    { \
        struct __wobj_mem__ *__new__ = malloc(sizeof(struct __wobj_mem__)); \
        __new__->ptr = _wobj_root_->func; \
        __new__->next = ___wobj___->__mem__; \
        ___wobj___->__mem__ = __new__; \
    }

// internal allocate
#define wobj_malloc(size) __wobj_malloc__(&(___wobj___->__mem__), size)
#define wobj_calloc(count, size) __wobj_calloc__(&(___wobj___->__mem__), count, size)
#define wobj_realloc(ptr, new_size) __wobj_realloc__(&(___wobj___->__mem__), ptr, new_size)
#define wobj_unalloc(ptr) __wobj_realloc__(&(___wobj___->__mem__), ptr, 0)
#define wobj_alloc(size) memset(wobj_malloc(size), '\0', size) // malloc + memset

#define wobj_new(name, var_name, ...) struct __wobj_##name##__public__ *var_name = __wobj_##name##__init__(__VA_ARGS__)
#define wobj_new2(name, var_name) struct __wobj_##name##__public__ *var_name = __wobj_##name##__init__
#define wobj_new3(name, ...) __wobj_##name##__init__(__VA_ARGS__)
#define wobj_new4(name) __wobj_##name##__init__
#define wobj_free(name, var_name) __wobj_##name##__free__(var_name)

#define wobj_sizeof(name) (sizeof(struct __wobj_##name##__public__))

#define wobj_fn(type, func_name, args) type (*func_name) args
#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS)
#define wobj_fn_ex(type, ex, func_name, args) type (ex*func_name) args
#else
#define wobj_fn_ex(type, ex, func_name, args) ex type (*func_name) args
#endif

#ifdef __cplusplus
}
#endif

#endif // !_wobj_h_
