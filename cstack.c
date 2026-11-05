#include "cstack.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(VAR) (void)(VAR)

#ifndef CSTACK_MAX_STACKS
#define CSTACK_MAX_STACKS 20
#endif

// ------------ Реализация стека ------------

struct node
{
    struct node* prev;
    unsigned int size;
    char data[];
};
typedef struct node stack_t;

struct stack_entry
{
    int reserved;
    unsigned int length;
    stack_t* stack_top;
};
typedef struct stack_entry stack_entry_t;

stack_entry_t g_table[CSTACK_MAX_STACKS] = {0};

// ------------ Реализация интерфейса стека ------------

hstack_t stack_new(void)
{
    for (unsigned int i = 0; i < CSTACK_MAX_STACKS; i += 1)
    {
        if (g_table[i].reserved == 0)
        {
            g_table[i].reserved = 1;

            return i;
        }
    }

    printf("Не удалось найти свободную память для нового стека.\n");
    return -1;
}

void stack_free(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        return;
    }

    while (g_table[hstack].length != 0)
    {
        stack_t* stack_top = g_table[hstack].stack_top;
        g_table[hstack].stack_top = stack_top->prev;
        free(stack_top);
        g_table[hstack].length -= 1;
    }

    g_table[hstack].stack_top = NULL;
    g_table[hstack].reserved = 0;
}

void stack_free_all(void)
{
    for (unsigned int i = 0; i < CSTACK_MAX_STACKS; i += 1)
    {
        if (g_table[i].reserved == 1)
        {
            stack_free(i);
        }
    }
}

int stack_valid_handler(const hstack_t hstack)
{
    if ((unsigned int)hstack >= CSTACK_MAX_STACKS)
    {
        return 0;
    }
    return g_table[hstack].reserved;
}

unsigned int stack_size(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        printf("Указанный стек не существует.\n");
        return 0;
    }

    return g_table[hstack].length;
}

void
stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    if (!stack_valid_handler(hstack))
    {
        printf("Указанного стека не существует!\n");
        return;
    }
    if (data_in == NULL || size == 0)
    {
        printf("Получен нулевой указатель или размер равен нулю.\n");
        return;
    }

    stack_t* new_top = malloc(sizeof(stack_t) + size);
    if (new_top == NULL)
    {
        printf("Выделить память для new_top не удалось.\n");
        return;
    }

    if (g_table[hstack].stack_top == NULL)
    {
        new_top->prev = NULL;
    }
    else
    {
        new_top->prev = g_table[hstack].stack_top;
    }

    memcpy(new_top->data, data_in, size);
    new_top->size = size;

    g_table[hstack].stack_top = new_top;
    g_table[hstack].length += 1;
}

unsigned int
stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    if (!stack_valid_handler(hstack))
    {
        printf("Указанного стека не существует!\n");
        return 0;
    }
    if (data_out == NULL || size == 0)
    {
        printf("Параметры введеного выходного буфера некорректны.\n");
        return 0;
    }
    if (g_table[hstack].length == 0)
    {
        printf("Указанный стек пуст.\n");
        return 0;
    }

    stack_t* stack_top = g_table[hstack].stack_top;
    unsigned int bytes = stack_top->size;

    if (bytes > size)
    {
        printf("Размер выходного буфера недостаточен.\n");
        return 0;
    }

    memcpy(data_out, stack_top->data, size);
    g_table[hstack].stack_top = stack_top->prev;

    free(stack_top);

    g_table[hstack].length -= 1;
    return bytes;
}

unsigned stack_top_size(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        printf("Указанного стека не существует!\n");
        return 0;
    }
    if (g_table[hstack].stack_top == NULL)
    {
        return 0;
    }

    return g_table[hstack].stack_top->size;
}

hstack_t stack_get_all(void)
{
    hstack_t hstack = stack_new();

    for (unsigned int i = 0; i < CSTACK_MAX_STACKS; i += 1)
    {
        if (i != (unsigned int)hstack && g_table[i].reserved == 1)
        {
            stack_push(hstack, &i, sizeof(hstack_t));
        }
    }

    return hstack;
}
