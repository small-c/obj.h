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
 *
 * Special thanks to clofn (https://github.com/yulon/clofn)
 *
 */

#ifndef _OBJ_H_
#define _OBJ_H_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifndef __VAARGS
#define __VAARGS(...) ,##__VA_ARGS__
#endif

#ifndef __EXPAND
#define __EXPAND(x) x
#endif

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/mman.h>
#include <sys/user.h>
#define __OBJ_ACTIV(ptr, size) \
    (mprotect((void *)(((size_t)ptr >> PAGE_SHIFT) << PAGE_SHIFT), size, PROT_READ | PROT_EXEC | PROT_WRITE) == 0)
#elif defined(_WIN32)
#if defined (_MSC_VER)
#pragma warning(disable : 4996)
#pragma comment(lib, "kernel32.lib")
#if defined(DEBUG) || defined(_DEBUG)
#error obj.h does not support MSVC on debug mode!
#endif
#endif
#if !defined(_WINDOWS_) || !defined(_INC_WINDOWS) || !defined(_MEMORYAPI_H_)
__declspec(dllimport) int __stdcall VirtualProtect(void *lpAddress, size_t dwSize, unsigned long flNewProtect, unsigned long *lpflOldProtect);
#endif
#ifndef PAGE_EXECUTE_READWRITE
#define PAGE_EXECUTE_READWRITE  0x40
#endif
static inline int __OBJ_ACTIV(void *ptr, size_t size) {
    unsigned long old_protect;
	return VirtualProtect(ptr, size, PAGE_EXECUTE_READWRITE, &old_protect) != 0;
}
#else
#error This OS is not supported!
#endif

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__) || defined(_M_X64) || defined(_WIN64)
#define __OBJ_X64	1
#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(__i386) || defined(__i686__) || defined(__i686) || defined(_M_IX86) || defined(_WIN32)
#define __OBJ_X86	1
#else
#error This architecture is not supported!
#endif

#define __OBJ_ERR(fmt, ...)    fprintf(stderr, "[obj.h] error: "fmt"\n" __VAARGS(__VA_ARGS__))

#define __OBJ_MAXPHSIZE 1024
#if __OBJ_X64
#define __OBJ_CLOFNNUM	0x58ffffbffdffffafULL
#elif __OBJ_X86
#define __OBJ_CLOFNNUM  0x58ffffbfU
#endif

static void *__OBJ_clofn(void *prototype, size_t *phsize, void *data) {
	uint8_t *code;
	size_t offset, ihsize;

	offset = *phsize;
	if (offset == 0) {
		for (; offset < __OBJ_MAXPHSIZE; offset++) {
			if (*(size_t *)((uintptr_t)prototype + offset) == (size_t)__OBJ_CLOFNNUM) {
				if (*phsize == 0) *phsize = offset;
				goto _mk;
			}
		}
	}

	__OBJ_ERR("could't find closure declaration at prototype function (%p)!", prototype);
	return NULL;

_mk:;
#if __OBJ_X64
	ihsize = offset + sizeof(void *) * 2 + 5;
#elif __OBJ_X86
	ihsize = offset + sizeof(void *) * 2 + 1;
#endif

	code = malloc(ihsize);
	if (!__OBJ_ACTIV(code, ihsize)) {
		__OBJ_ERR("could't change memory type of C.malloc allocated!");
		free(code);
		return NULL;
	}

#pragma pack(push, 1)
#if __OBJ_X64
	static struct {
		uintptr_t	data;
		uint8_t		push_rax;
		uint8_t		mov_rax[2];
		uintptr_t   addr;
		uint8_t     jmp_rax[2];
	} asmc = { .push_rax = 0x50, .mov_rax = { 0x48, 0xB8 }, .jmp_rax = { 0xFF, 0xE0 } };
	asmc.addr = (uintptr_t)prototype + offset + sizeof(uintptr_t) - 1;
#elif __OBJ_X86
	static struct {
		uintptr_t	data;
		uint8_t		jmp;
		uintptr_t   addr;
	} asmc = { .jmp = 0xE9 };
	asmc.addr = ((uintptr_t)prototype + offset + sizeof(uintptr_t)) - ((uintptr_t)code + ihsize);
#endif
#pragma pack(pop)

	asmc.data = (uintptr_t)data;
	memcpy(code, prototype, offset);
	memcpy(code + offset, &asmc, sizeof(asmc));

	return (void*)code;
}

#if defined(class)
#undef class
#endif

#if defined(classdef)
#undef classdef
#endif

#if defined(public)
#undef public
#endif

#if defined(private)
#undef private
#endif

#if defined(new)
#undef new
#endif

#if defined(none)
#undef none
#endif

#if defined(multiple)
#undef multiple
#endif

#if defined(extends)
#undef extends
#endif

#if defined(constructor)
#undef constructor
#endif

#if defined(destructor)
#undef destructor
#endif

#if defined(obj_use) && (obj_use == this && !defined(__cplusplus))
#define __OBJ_ROOT obj_use
#else
#define __OBJ_ROOT self
#endif

struct __OBJ_base {
	void (* alloc)(size_t);
	void (* release)();
	void (* reserved[2]);
};

struct __OBJ_mem {
    void *ptr;
    struct __OBJ_mem *next;
};

static void *__OBJ_append(struct __OBJ_mem **node, void *ptr) {
	if (!ptr) return NULL;
	struct __OBJ_mem *last = malloc(sizeof(struct __OBJ_mem));
	if (last) {
		last->ptr = ptr, last->next = *node;
		*node = last;
		return ptr;
	}
	__OBJ_ERR("could't append memory %p to node list!", ptr);
	return NULL;
}

static void __OBJ_clean(struct __OBJ_mem *node) {
	for (struct __OBJ_mem *next, *last = node; last != NULL; ) {
		next = last->next;
		free(last->ptr);
		free(last);
		last = next;
	}
}

static size_t __OBJ_alloc_s = 0;
static void *__OBJ_alloc(size_t size) {
	volatile size_t closn = __OBJ_CLOFNNUM;
	struct __OBJ_base *base = (struct __OBJ_base *)closn;

	void *ptr = malloc(size);
	return __OBJ_append((struct __OBJ_mem **)&base->reserved[0], ptr);
}

static size_t __OBJ_release_s = 0;
static void __OBJ_release() {
	volatile size_t closn = __OBJ_CLOFNNUM;
	struct __OBJ_base *base = (struct __OBJ_base *)closn;

	struct __OBJ_mem *mem = base->reserved[0];
	void (* destructor)() = base->reserved[1];

	free(base->release);
	base->release = NULL;

	if (destructor) destructor();
	free(base->alloc);
	__OBJ_clean(mem);
}

#define __OBJ_PUB(n)	struct __OBJ__##n
#define __OBJ_PRV(n)	struct __OBJ_PRV_##n
#define __OBJ_INF(n)    struct __OBJ_INF_##n

#define __OBJ_M(n, f)	__OBJ__##n##_##f
#define __OBJ_S(n, f)	__OBJ_S_##n##_##f
#define __OBJ_H(n, f)	__OBJ_H_##n##_##f

#define __OBJ_CON(n)	__OBJ_C_##n
#define __OBJ_DES(n)	__OBJ_D_##n

#define none
#define public(...)     __VA_ARGS__
#define private(...)    __VA_ARGS__
#define multiple(...)   __VA_ARGS__
#define extends(parent_name) \
	__OBJ_PUB(parent_name) parent_name

#define classdef(name) \
    typedef __OBJ_PUB(name) *name

#define class(name, public_members, ...) \
    __OBJ_PUB(name) { \
		struct __OBJ_base base; \
        struct { char : 0; public_members }; \
    } *name; \
    __OBJ_PRV(name) { \
		struct __OBJ_base base; \
        struct { char : 0; public_members }; \
        struct { char : 0; __VA_ARGS__ }; \
    }

#define constructor(class_name) \
	class_name class_name##_new

#define destructor(class_name) \
	void __OBJ_DES(class_name)(__OBJ_PRV(class_name) *__OBJ_ROOT)

#define method(class_name, return_type, name) \
	static size_t __OBJ_S(class_name, name); \
	static return_type __OBJ_M(class_name, name)

#define new(class_name) \
	class_name##_new

#define obj_prepare(class_name) \
	volatile size_t __closn = (size_t)__OBJ_CLOFNNUM; \
	__OBJ_PRV(class_name) *__OBJ_ROOT = (__OBJ_PRV(class_name)*)__closn

#define __OBJ_IMPL(class_name, method_name) \
    do { \
		void *__pf = __OBJ_clofn(__OBJ_M(class_name, method_name), &__OBJ_S(class_name, method_name), (void*)__OBJ_ROOT); \
		if (__pf) { \
			__OBJ_ROOT->method_name = __OBJ_append((struct __OBJ_mem **)&__OBJ_ROOT->base.reserved[0], __pf); \
		} \
		else { \
			__OBJ_ERR("could't implement the method '%s'!", #method_name); \
			goto __err__; \
		} \
    } while (0)

#define __OBJ_S2(n, _1) \
    __OBJ_IMPL(n, _1)
#define __OBJ_S3(n, _1, _2) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2)
#define __OBJ_S4(n, _1, _2, _3) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3)
#define __OBJ_S5(n, _1, _2, _3, _4) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4)
#define __OBJ_S6(n, _1, _2, _3, _4, _5) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5)
#define __OBJ_S7(n, _1, _2, _3, _4, _5, _6) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6)
#define __OBJ_S8(n, _1, _2, _3, _4, _5, _6, _7) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6); __OBJ_IMPL(n, _7)
#define __OBJ_S9(n, _1, _2, _3, _4, _5, _6, _7, _8) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6), __OBJ_IMPL(n, _7); __OBJ_IMPL(n, _8)
#define __OBJ_S10(n, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6); __OBJ_IMPL(n, _7); __OBJ_IMPL(n, _8); __OBJ_IMPL(n, _9)
#define __OBJ_S11(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6); __OBJ_IMPL(n, _7); __OBJ_IMPL(n, _8); __OBJ_IMPL(n, _9); __OBJ_IMPL(n, _10)
#define __OBJ_S12(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6); __OBJ_IMPL(n, _7); __OBJ_IMPL(n, _8); __OBJ_IMPL(n, _9); __OBJ_IMPL(n, _10); \
	__OBJ_IMPL(n, _11)
#define __OBJ_S13(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6); __OBJ_IMPL(n, _7); __OBJ_IMPL(n, _8); __OBJ_IMPL(n, _9); __OBJ_IMPL(n, _10); \
	__OBJ_IMPL(n, _11); __OBJ_IMPL(n, _12)
#define __OBJ_S14(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    __OBJ_IMPL(n, _1); __OBJ_IMPL(n, _2); __OBJ_IMPL(n, _3); __OBJ_IMPL(n, _4); __OBJ_IMPL(n, _5); \
	__OBJ_IMPL(n, _6); __OBJ_IMPL(n, _7); __OBJ_IMPL(n, _8); __OBJ_IMPL(n, _9); __OBJ_IMPL(n, _10); \
	__OBJ_IMPL(n, _11); __OBJ_IMPL(n, _12); __OBJ_IMPL(n, _13)

#define __OBJ_FC(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define __OBJ_FR(args) __OBJ_FC args
#define __OBJ_CFA(...) __OBJ_FR((__VA_ARGS__, \
		__OBJ_S14, __OBJ_S13, __OBJ_S12, \
        __OBJ_S11, __OBJ_S10, __OBJ_S9, __OBJ_S8, \
        __OBJ_S7, __OBJ_S6, __OBJ_S5, __OBJ_S4, \
        __OBJ_S3, __OBJ_S2, , ))
#define __OBJ_MC(...) __OBJ_CFA(__VA_ARGS__ ())

#define obj_implement(class_name, ...)  __EXPAND(__OBJ_MC(class_name, __VA_ARGS__)(class_name, __VA_ARGS__))

#define obj_setup(class_name) \
	__OBJ_PRV(class_name) *__OBJ_ROOT = (__OBJ_PRV(class_name)*)malloc(sizeof(__OBJ_PRV(class_name))); \
	do { \
		if (!__OBJ_ROOT) { \
			__OBJ_ERR("could't create new class '%s' instance!", #class_name); \
			return NULL; \
		} \
		__OBJ_ROOT->base.reserved[0] = NULL; \
		__OBJ_ROOT->base.reserved[1] = NULL; \
		__OBJ_ROOT->base.alloc = __OBJ_clofn((void*)__OBJ_alloc, &__OBJ_alloc_s, (void*)__OBJ_ROOT); \
		__OBJ_ROOT->base.release = __OBJ_clofn((void*)__OBJ_release, &__OBJ_release_s, (void*)__OBJ_ROOT); \
		if (!__OBJ_append((struct __OBJ_mem **)&__OBJ_ROOT->base.reserved[0], __OBJ_ROOT)) goto __err__; \
	} while (0)

#define obj_error() \
	goto __err__

#define obj_done(class_name) \
	(class_name)__OBJ_ROOT; \
	__err__:; \
	__OBJ_clean((struct __OBJ_mem *)__OBJ_ROOT->base.reserved[0]); \
	return NULL

#define obj_onfree(class_name) \
	__OBJ_ROOT->base.reserved[1] = (void*)__OBJ_DES(class_name)

#define obj_override(class_name, parent_name, method_name) \
	do { \
		void *ptr = __OBJ_clofn(__OBJ_M(class_name, method_name), (void*)__OBJ_ROOT); \
		if (ptr) { __obj->parent_name.method_name = __OBJ_append(&__obj->__mem, ptr); } \
		else { __OBJ_ERR("could't implement the method '%s.%s'!", #parent_name, #method_name); goto __err__; } \
	} while (0)

#ifdef __cplusplus
}
#endif
#endif
