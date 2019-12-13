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

#ifndef _WOBJ_H_
#define _WOBJ_H_
#pragma once

#ifdef __cplusplus
extern "C" {
#define __bool__ bool
#else
#define __bool__ _Bool
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// start clofn [thanks to https://github.com/yulon/clofn]
// ==============================================

#define CLOFN_PHSIZE_MAX 1024
#define _CLOFN_SCIENCE_NUMBER 0x58ffffbffdffffafULL

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/mman.h>
#include <sys/user.h>
#define __wobj_ativ_mem(ptr, size) \
    (mprotect((void *)(((size_t)ptr >> PAGE_SHIFT) << PAGE_SHIFT), size, PROT_READ | PROT_EXEC | PROT_WRITE) == 0)
#elif defined(_WIN32)
#if defined (_MSC_VER)
#if defined(DEBUG) || defined(_DEBUG)
#error Clofn: not support MSVC on debug mode!
#endif
#pragma warning(disable : 4996)
#endif
#include <windows.h>
    static inline __bool__ __wobj_ativ_mem(void *ptr, size_t size) {
        DWORD old_protect;
        return VirtualProtect(ptr, size, PAGE_EXECUTE_READWRITE, &old_protect);
    }
#else
#error Clofn: not support this OS!
#endif

    static void *__wobj_new_clofn(void *prototype, size_t *phsize, void *data) {

        size_t offset = *phsize;
        if (!offset) {
            for (; offset < CLOFN_PHSIZE_MAX; offset++) {
                if (*(size_t *)((uintptr_t)prototype + offset) == (size_t)_CLOFN_SCIENCE_NUMBER) {
                    if (!*phsize) {
                        *phsize = offset;
                    }

                    goto mk;
                }
            }

            printf("Clofn: could't find closure declaration at prototype function (%p)!\n", prototype);
            return NULL;
        }
    mk:;

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__) || defined(_WIN64)
        size_t ihsize = offset + sizeof(void *) * 2 + 5;
#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(__i386) || defined(__i686__) || defined(__i686) || defined(_WIN32)
        size_t ihsize = offset + sizeof(void *) * 2 + 1;
#else
#error Clofn: not support this arch!
#endif

        void *instance = malloc(ihsize);
        if (!__wobj_ativ_mem(instance, ihsize)) {
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

        return instance;
    }

// end clofn
// ==============================================

// start wobj
// ==============================================

#if defined(public)
#undef public
#endif

#if defined(private)
#undef private
#endif

#if defined(wobj)
#undef wobj
#endif

#if defined(func)
#undef func
#endif

#if defined(multi)
#undef multi
#endif

#if defined(new)
#undef new
#endif

#ifndef __VAARGS
#define __VAARGS(...) ,##__VA_ARGS__
#endif

#ifndef _wobj_root_
#if defined(wobj_use_other)
#define _wobj_root_ wobj_use_other
#elif (defined (wobj_this) || defined(this) || defined(use_this)) && !defined(__cplusplus)
#define _wobj_root_ this // exclude C++
#else
#define _wobj_root_ self // default
#endif
#endif

// ==========================

#define WOBJ_MALLOC   0x0001
#define WOBJ_CALLOC   0x0002
#define WOBJ_REALLOC  0x0004

    struct _wobj_mem {
        void *ptr;
        struct _wobj_mem *next;
    };

    static void *__wobj_alloc(struct _wobj_mem **mem, uint8_t type, size_t count, size_t size, void *ptr)
    {
        if (*mem == NULL) return NULL;
        switch (type) {
            case WOBJ_MALLOC: {
                struct _wobj_mem *c = *mem;
                *mem = malloc(sizeof(struct _wobj_mem));
                (*mem)->ptr = malloc(size);
                (*mem)->next = c;
                return (*mem)->ptr;
            }
            case WOBJ_CALLOC: {
                struct _wobj_mem *c = *mem;
                *mem = malloc(sizeof(struct _wobj_mem));
                (*mem)->ptr = calloc(count, size);
                (*mem)->next = c;
                return (*mem)->ptr;
            }
            case WOBJ_REALLOC: {
                for (struct _wobj_mem *c = *mem, *p = NULL; c != NULL;)
                {
                    struct _wobj_mem *n = c->next;
                    if (c->ptr == ptr) {
                        if (size <= 0) {
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
                            return c->ptr = realloc(ptr, size);
                    }
                    p = c;
                    c = n;
                }
                return NULL;
            }
        }
        return NULL;
    }

// ==========================

#define public(...)  __VA_ARGS__
#define private(...) __VA_ARGS__
#define multi(...)   __VA_ARGS__

#define func(func_name, args) (* func_name) args
#define func_ex(func_name, ex) (ex * func_name)

// ==========================

#define __wobj_G(n) struct __wobj__##n
#define __wobj_P(n) struct __wobj_P_##n
#define __wobj_F(n) struct __wobj_F_##n

#define __wobj_m(n, f) __wobj__##n##_##f
#define __wobj_s(n, f) __wobj_s_##n##_##f
#define __wobj_h(n, f) __wobj_h_##n##_##f

#define __wobj_n(n) __wobj_n_##n
#define __wobj_f(n) __wobj_f_##n

// ==========================

#define wobj_decl(name) \
    typedef __wobj_G(name) *name

#define wobj(name, public, ...) \
    typedef __wobj_G(name) { \
        struct { char : 0; public }; \
    } *name; \
    __wobj_P(name) { \
        struct { char : 0; public }; \
        struct { char : 0; __VA_ARGS__ }; \
    }; \
    __wobj_F(name) { \
        struct { char : 0; public }; \
        struct { char : 0; __VA_ARGS__ }; \
        struct _wobj_mem *__mem; \
    }; \
    __wobj_G(name) *__wobj_n(name)(); \
    void __wobj_f(name)(void **__in__);

#define wobj_def(name, func_name, args, body) \
     __wobj_m(name, func_name) args { \
        volatile size_t __closize = (size_t)_CLOFN_SCIENCE_NUMBER; \
        __wobj_F(name) *__wobj = (__wobj_F(name)*)__closize; \
        __wobj_P(name) *_wobj_root_ = (__wobj_P(name)*)__wobj; \
        body \
    } \
    static size_t __wobj_s(name, func_name) = 0; \
    static size_t __wobj_h(name, func_name) = 0;


#define __WOBJ_EXPAND(x) x

#define __WOBJ_SET_2(name, _1) \
    wobj_set(name, _1)
#define __WOBJ_SET_3(name, _1, _2) \
    wobj_set(name, _1); wobj_set(name, _2)
#define __WOBJ_SET_4(name, _1, _2, _3) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3)
#define __WOBJ_SET_5(name, _1, _2, _3, _4) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3); wobj_set(name, _4)
#define __WOBJ_SET_6(name, _1, _2, _3, _4, _5) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3); wobj_set(name, _4); wobj_set(name, _5)
#define __WOBJ_SET_7(name, _1, _2, _3, _4, _5, _6) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3); wobj_set(name, _4); wobj_set(name, _5); wobj_set(name, _6)
#define __WOBJ_SET_8(name, _1, _2, _3, _4, _5, _6, _7) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3); wobj_set(name, _4); wobj_set(name, _5); wobj_set(name, _6); wobj_set(name, _7)
#define __WOBJ_SET_9(name, _1, _2, _3, _4, _5, _6, _7, _8) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3); wobj_set(name, _4); wobj_set(name, _5); wobj_set(name, _6), wobj_set(name, _7); wobj_set(name, _8)
#define __WOBJ_SET_10(name, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3); wobj_set(name, _4); wobj_set(name, _5); wobj_set(name, _6); wobj_set(name, _7); wobj_set(name, _8); wobj_set(name, _9)
#define __WOBJ_SET_11(name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    wobj_set(name, _1); wobj_set(name, _2); wobj_set(name, _3); wobj_set(name, _4); wobj_set(name, _5); wobj_set(name, _6); wobj_set(name, _7); wobj_set(name, _8); wobj_set(name, _9); wobj_set(name, _10)

#define __WOBJ_FC(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, ...) _12
#define __WOBJ_FR(argsWithParentheses) __WOBJ_FC argsWithParentheses
#define __WOBJ_CF_ARGC(...) \
    __WOBJ_FR((__VA_ARGS__, \
        __WOBJ_SET_11, __WOBJ_SET_10, __WOBJ_SET_9, __WOBJ_SET_8, \
        __WOBJ_SET_7, __WOBJ_SET_6, __WOBJ_SET_5, __WOBJ_SET_4, \
        __WOBJ_SET_3, __WOBJ_SET_2, , ))

#define __WOBJ_MC(...) __WOBJ_CF_ARGC(__VA_ARGS__ ())

#define wobj_set(name, func_name) \
    _wobj_root_->func_name = (void*)__wobj_new_clofn(__wobj_m(name, func_name), &(__wobj_s(name, func_name)), (void*)_wobj_root_); \
    { \
        struct _wobj_mem *__new = malloc(sizeof(struct _wobj_mem)); \
        __new->ptr = _wobj_root_->func_name; \
        __new->next = __wobj->__mem; \
        __wobj->__mem = __new; \
    }
#define wobj_sets(name, ...) __WOBJ_EXPAND(__WOBJ_MC(name, __VA_ARGS__)(name, __VA_ARGS__))

#define wobj_init(name, args_new, body_init, body_free) \
    __wobj_G(name) *__wobj_n(name) args_new { \
        __wobj_F(name) *__wobj = (__wobj_F(name)*)malloc(sizeof(__wobj_F(name))); \
        if (!__wobj) return NULL; \
        __wobj->__mem = malloc(sizeof(struct _wobj_mem)); \
        __wobj->__mem->ptr = __wobj; \
        __wobj->__mem->next = NULL; \
        __wobj_P(name) *_wobj_root_ = (__wobj_P(name)*)__wobj; \
        body_init \
        return (__wobj_G(name)*)_wobj_root_; \
    } \
    void __wobj_f(name)(void **__in__) { \
        if (!__in__ || !(*__in__)) return; \
        __wobj_P(name) * _wobj_root_ = *__in__; \
        __wobj_F(name) *__wobj = (__wobj_F(name)*)_wobj_root_; \
        body_free \
        for (struct _wobj_mem *mem = __wobj->__mem; mem != NULL; ) { \
            struct _wobj_mem *next = mem->next; \
            free(mem->ptr); \
            free(mem); \
            mem = next; \
        } \
        *__in__ = self = NULL; \
    }

// internal allocate
#define __wobj_imem (__wobj->__mem)
#define wobj_malloc(size)           __wobj_alloc(&__wobj_imem, WOBJ_MALLOC,  0,     size,     NULL)
#define wobj_calloc(count, size)    __wobj_alloc(&__wobj_imem, WOBJ_CALLOC,  count, size,     NULL)
#define wobj_realloc(ptr, new_size) __wobj_alloc(&__wobj_imem, WOBJ_REALLOC, 0,     new_size, ptr)
#define wobj_unalloc(ptr)           __wobj_alloc(&__wobj_imem, WOBJ_REALLOC, 0,     0,        ptr)
#define wobj_alloc(size)     memset(__wobj_alloc(&__wobj_imem, WOBJ_MALLOC,  0,     size,     NULL), '\0', size)

// external allocate
#define __wobj_omem(name, var) (((__wobj_F(name)*)var)->__mem)
#define wobj_malloco(name, var, size)           __wobj_alloc(&__wobj_omem(name, var), WOBJ_MALLOC,  0,     size,     NULL)
#define wobj_calloco(name, var, count, size)    __wobj_alloc(&__wobj_omem(name, var), WOBJ_CALLOC,  count, size,     NULL)
#define wobj_realloco(name, var, ptr, new_size) __wobj_alloc(&__wobj_omem(name, var), WOBJ_REALLOC, 0,     new_size, ptr)
#define wobj_unalloco(name, var, ptr)           __wobj_alloc(&__wobj_omem(name, var), WOBJ_REALLOC, 0,     0,        ptr)
#define wobj_alloco(name, var, size)     memset(__wobj_alloc(&__wobj_omem(name, var), WOBJ_MALLOC,  0,     size,     NULL), '\0', size)

#define wobj_new(name, var_name, ...) __wobj_G(name) *var_name = __wobj_n(name)(__VA_ARGS__)
#define wobj_new2(name, var_name)     __wobj_G(name) *var_name = __wobj_n(name)
#define wobj_new3(name, ...)                                     __wobj_n(name)(__VA_ARGS__)
#define wobj_new4(name)                                          __wobj_n(name)

#define new(name, ...) __wobj_n(name)(__VA_ARGS__)

#define wobj_free(name, var_name)     __wobj_f(name)((void**)&(var_name))

#define wobj_sizeof(name)  (sizeof(__wobj_G(name)))
#define wobj_sizeoff(name) (sizeof(__wobj_P(name)))
#define wobj_sizeofv(var)  (sizeof(*var))

// public from
#define wobj_public(public_name) __wobj_G(public_name) public_name

#define wobj_setp(name, public_name, func_name) \
    _wobj_root_->public_name.func_name = (void*)__wobj_new_clofn(__wobj_m(name, func_name), &(__wobj_s(name, func_name)), (void*)_wobj_root_); \
    { \
        struct _wobj_mem *__new = malloc(sizeof(struct _wobj_mem)); \
        __new->ptr = _wobj_root_->public_name.func_name; \
        __new->next = __wobj->__mem; \
        __wobj->__mem = __new; \
    }

// end wobj
// ==============================================

#ifdef __cplusplus
}
#endif
#endif // !_WOBJ_H_
