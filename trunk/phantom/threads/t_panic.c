/**
 *
 * Phantom OS multithreading library.
 *
 * Copyright (C) 2009-2010 Dmitry Zavalishin, dz@dz.ru
 *
 * Panic.
 *
 * Licensed under CPL 1.0, see LICENSE file.
 *
**/

#include <assert.h>
#include <stdarg.h>
#include <hal.h>
#include <phantom_libc.h>

#include "thread_private.h"

#include <phantom_libc.h>


int panic_reenter = 0;


void panic(const char *fmt, ...)
{
    if(panic_reenter)
        _exit(33);

    hal_cli();
    panic_reenter++;

    printf("Panic: ");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
getchar();
    stack_dump();

    dump_thread_stacks();

    exit(33);
}

