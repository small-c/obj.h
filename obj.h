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

#define __OBJ_ERR(fmt, ...)    fprintf(stderr, "[obj.h] error: "fmt"\n" __VAARGS(__VA_ARGS__))

#define __OBJ_MAXPHSIZE  1024
#define __OBJ_SCINUMBER  0x58ffffbffdffffafULL

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

static void *__OBJ_CLOFN(void *prototype, void *data) {
	size_t offset = 0;

	for (; offset < __OBJ_MAXPHSIZE; offset++)
		if (*(size_t *)((uintptr_t)prototype + offset) == (size_t)__OBJ_SCINUMBER)
			goto _mk;

	__OBJ_ERR("could't find closure declaration at prototype function (%p)!", prototype);
	return NULL;

_mk:;

#if __OBJ_X64
	size_t ihsize = offset + sizeof(void *) * 2 + 5;
#elif __OBJ_X86
	size_t ihsize = offset + sizeof(void *) * 2 + 1;
#endif

	void *instance = malloc(ihsize);
	if (!__OBJ_ACTIV(instance, ihsize)) {
		__OBJ_ERR("could't change memory type of C.malloc allocated!");
		free(instance);
		return NULL;
	}

	memcpy(instance, prototype, offset);
	uint8_t *current = (uint8_t*)instance + offset;
	*(void **)current = data; current += sizeof(void *);

#if __OBJ_X64
	*(current++) = 0x50;	// push %rax
	*(current++) = 0x48;	// movabs $ptr, %rax
	*(current++) = 0xB8;
	*(uintptr_t *)current = (uintptr_t)prototype + offset + sizeof(uintptr_t) - 1; current += sizeof(uintptr_t);
	*(current++) = 0xFF;	// jmp %rax
	*(current++) = 0xE0;
#elif __OBJ_X86
	*(current++) = 0xE9;	// jmp $ptr
	*(uintptr_t *)current = ((uintptr_t)prototype + offset + sizeof(uintptr_t)) - ((uintptr_t)instance + ihsize);
#endif

	return instance;
}

#if defined(class)
#undef class
#endif

#if defined(public)
#undef public
#endif

#if defined(private)
#undef private
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

#if defined(obj_use) && (obj_use == this && !defined(__cplusplus))
#define __OBJ_ROOT obj_use
#else
#define __OBJ_ROOT self
#endif

struct __OBJ_MEM {
    void *ptr;
    struct __OBJ_MEM *next;
};

static int __OBJ_APPEND(struct __OBJ_MEM **node, void *ptr) {
	if (!ptr) return 0;
	struct __OBJ_MEM *last = malloc(sizeof(struct __OBJ_MEM));
	if (!last) { __OBJ_ERR("could't append memory to node list!"); return 0; }
	last->ptr = ptr;
	last->next = (*node);
	(*node) = last;
	return 1;
}

static void __OBJ_CLEAN(struct __OBJ_MEM *node) {
	for (struct __OBJ_MEM *next, *last = node; last != NULL; ) {
		next = last->next;
		free(last->ptr);
		free(last);
		last = next;
	}
}

#define none
#define public(...)     __VA_ARGS__
#define private(...)    __VA_ARGS__
#define multiple(...)   __VA_ARGS__

// ==========================

#define __OBJ_PUB(n)	struct __OBJ__##n
#define __OBJ_PRV(n)	struct __OBJ_PRV_##n
#define __OBJ_ALL(n)	struct __OBJ_ALL_##n

#define __OBJ_M(n, f)	__OBJ__##n##_##f
#define __OBJ_S(n, f)	__OBJ_S_##n##_##f
#define __OBJ_H(n, f)	__OBJ_H_##n##_##f

#define __OBJ_CON(n)	__OBJ_C_##n
#define __OBJ_DES(n)	__OBJ_D_##n

// ==========================

#define class_def(name) \
    typedef __OBJ_PUB(name) *name

#define class_con(class_name) \
	class_name class_name##_new

#define class_des(class_name) \
	void class_name##_delete(class_name); \
	void __OBJ_DES(class_name)(__OBJ_PRV(class_name)*)

#define class(name, public_members, ...) \
    __OBJ_PUB(name) { \
        struct { char : 0; public_members }; \
    } *name; \
    __OBJ_PRV(name) { \
        struct { char : 0; public_members }; \
        struct { char : 0; __VA_ARGS__ }; \
    }; \
    __OBJ_ALL(name) { \
        struct { char : 0; public_members }; \
        struct { char : 0; __VA_ARGS__ }; \
        struct __OBJ_MEM *__mem; \
    };

#define prepare(class_name) \
	volatile size_t __closn = (size_t)__OBJ_SCINUMBER; \
	__OBJ_ALL(class_name) *__obj = (__OBJ_ALL(class_name)*)__closn; \
	__OBJ_PRV(class_name) *__OBJ_ROOT = (__OBJ_PRV(class_name)*)__obj

#define method(class_name, name) \
	__OBJ_M(class_name, name)

#define obj_impl(class_name, method) \
    { \
		void *ptr = __OBJ_CLOFN(__OBJ_M(class_name, method), (void*)__OBJ_ROOT); \
		if (ptr) { __obj->method = ptr; __OBJ_APPEND(&__obj->__mem, ptr); } \
		else { __OBJ_ERR("could't implement the method '%s'!", #method); goto __err__; } \
    }

#define __OBJ_S2(n, _1) \
    obj_impl(n, _1)
#define __OBJ_S3(n, _1, _2) \
    obj_impl(n, _1); obj_impl(n, _2)
#define __OBJ_S4(n, _1, _2, _3) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3)
#define __OBJ_S5(n, _1, _2, _3, _4) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4)
#define __OBJ_S6(n, _1, _2, _3, _4, _5) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5)
#define __OBJ_S7(n, _1, _2, _3, _4, _5, _6) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6)
#define __OBJ_S8(n, _1, _2, _3, _4, _5, _6, _7) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6); obj_impl(n, _7)
#define __OBJ_S9(n, _1, _2, _3, _4, _5, _6, _7, _8) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6), obj_impl(n, _7); obj_impl(n, _8)
#define __OBJ_S10(n, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6); obj_impl(n, _7); obj_impl(n, _8); obj_impl(n, _9)
#define __OBJ_S11(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6); obj_impl(n, _7); obj_impl(n, _8); obj_impl(n, _9); obj_impl(n, _10)
#define __OBJ_S12(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6); obj_impl(n, _7); obj_impl(n, _8); obj_impl(n, _9); obj_impl(n, _10); obj_impl(n, _11)
#define __OBJ_S13(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6); obj_impl(n, _7); obj_impl(n, _8); obj_impl(n, _9); obj_impl(n, _10); obj_impl(n, _11); obj_impl(n, _12)
#define __OBJ_S14(n, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    obj_impl(n, _1); obj_impl(n, _2); obj_impl(n, _3); obj_impl(n, _4); obj_impl(n, _5); obj_impl(n, _6); obj_impl(n, _7); obj_impl(n, _8); obj_impl(n, _9); obj_impl(n, _10); obj_impl(n, _11); obj_impl(n, _12); obj_impl(n, _13)

#define __OBJ_FC(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define __OBJ_FR(args) __OBJ_FC args
#define __OBJ_CFA(...) \
    __OBJ_FR((__VA_ARGS__, \
		__OBJ_S14, __OBJ_S13, __OBJ_S12, \
        __OBJ_S11, __OBJ_S10, __OBJ_S9, __OBJ_S8, \
        __OBJ_S7, __OBJ_S6, __OBJ_S5, __OBJ_S4, \
        __OBJ_S3, __OBJ_S2, , ))

#define __OBJ_MC(...) __OBJ_CFA(__VA_ARGS__ ())

#define obj_impls(class_name, ...)  __EXPAND(__OBJ_MC(class_name, __VA_ARGS__)(class_name, __VA_ARGS__))

#define obj_init(class_name) \
	__OBJ_ALL(class_name) *__obj = (__OBJ_ALL(class_name)*)malloc(sizeof(__OBJ_ALL(class_name))); \
	if (!__obj) { __OBJ_ERR("could't create a new object instance!"); return NULL; } \
	__obj->__mem = NULL; \
	if (!__OBJ_APPEND(&__obj->__mem, __obj)) return NULL; \
	__OBJ_PRV(class_name) *__OBJ_ROOT = (__OBJ_PRV(class_name)*)(__obj);

#define obj_err() \
	goto __err__

#define obj_done(class_name) \
	(class_name)__obj; \
	__err__: __OBJ_CLEAN(__obj->__mem); return NULL

#define constructor(class_name) \
	class_name class_name##_new

#define destructor(class_name) \
	void class_name##_delete(class_name obj) { \
		if (!obj) return; \
		__OBJ_DES(class_name)((__OBJ_PRV(class_name)*)obj); \
		__OBJ_ALL(class_name) *self = (__OBJ_ALL(class_name)*)obj; \
		__OBJ_CLEAN(self->__mem); \
	} \
	void __OBJ_DES(class_name)(__OBJ_PRV(class_name) *__OBJ_ROOT)

#define new(class_name)					class_name##_new
#define delete(class_name, var_name)    class_name##_delete(var_name)
#define extends(parent_name)			__OBJ_PUB(parent_name) parent_name

#define override(class_name, parent_name, method_name) \
	{ \
		void *ptr = __OBJ_CLOFN(__OBJ_M(class_name, method_name), (void*)__OBJ_ROOT); \
		if (ptr) { __obj->parent_name.method_name = ptr; __OBJ_APPEND(&__obj->__mem, ptr); } \
		else { __OBJ_ERR("could't implement the method '%s.%s'!", #parent_name, #method_name); goto __err__; } \
	}

#ifdef __cplusplus
}
#endif
#endif
