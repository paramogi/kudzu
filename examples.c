/*
 * example.c - usage examples for kudzu.h
 *
 * Build:
 * cc -std=c99 -Wall -Wextra -o examples examples.c
 */

#define KUDZU_IMPLEMENTATION
#include "kudzu.h"

#include <stdio.h>
#include <string.h>

// ints
static void example_int(void)
{
    // 0 capacity is fine, the first darray_append allocates
    darray a = darray_init(sizeof(int), 0);
    size_t i;

    for (i = 0; i < 5; i++) {
        int v = (int)(i * i);
        // darray_append copies the value, so v can go out of scope after this
        if (!darray_append(&a, &v)) {
            fprintf(stderr, "out of memory\n");
            darray_free(&a);
            return;
        }
    }

    printf("ints:");
    for (i = 0; i < a.len; i++)
        // clunky asf, i know.
        // however, this was the only way i found of doing it
        printf(" %d", *(int *)darray_at(&a, i));
    printf(" (len=%zu, cap=%zu)\n", a.len, a.cap);

    darray_free(&a);
}

// doubles
static void example_double(void)
{
    // preallocating avoids the regrow
    darray d = darray_init(sizeof(double), 4);
    double sum = 0.0;
    size_t i;

    darray_append(&d, &(double){ 3.14 });
    darray_append(&d, &(double){ 2.71 });
    darray_append(&d, &(double){ 1.41 });

    for (i = 0; i < d.len; i++)
        // again, clunky asf... sorry to whoever is reading this
        sum += *(double *)darray_at(&d, i);

    printf("doubles: sum=%.2f (len=%zu, cap=%zu)\n", sum, d.len, d.cap);

    darray_free(&d);
}

// structs
typedef struct {
    char    name[16];
    int     hp;
} monster;

static void example_struct(void)
{
    darray m = darray_init(sizeof(monster), 2);
    monster *p;
    size_t i;

    darray_append(&m, &(monster){ "vine", 30 });
    darray_append(&m, &(monster){ "creeper", 55 });
    darray_append(&m, &(monster){ "kudzu", 100 });

    // darray_append returns a pointer to the stored copy, so we can edit it
    p = darray_append(&m, &(monster){ "sprout", 0 });
    if (p)
        p->hp = 5;

    printf("monsters:\n");
    for (i = 0; i < m.len; i++) {
        monster *mp = darray_at(&m, i);
        printf(" %s hp=%d\n", mp->name, mp->hp);
    }

    darray_free(&m);
}

// strings
static void example_string_buffer(void)
{
    darray s = darray_init(sizeof(char), 0);
    const char *msg = "bad code is kudzu";
    size_t i;

    for (i = 0; msg[i]; i++)
        darray_append(&s, &msg[i]);
    darray_append(&s, &(char){ '\0' });

    // items is contiguous, so it can be handed to any char* function
    printf("string: %s (len=%zu including NUL, cap=%zu)\n", (char *)s.items, s.len, s.cap);

    darray_free(&s);
}

// multiple strings
static void example_string_list(void)
{
    darray list = darray_init(sizeof(char *), 0);
    const char *third = "goodbye";
    size_t i;

    // the element IS a pointer, so we pass the address of the pointer
    darray_append(&list, &(const char *){ "hello" });
    darray_append(&list, &(const char *){ "world" });
    darray_append(&list, &third);

    printf("multiple strings:");
    for (i = 0; i < list.len; i++) {
        // darray_at gives a char**, hence the extra deref
        printf(" %s", *(const char **)darray_at(&list, i));
    }
    printf("\n");

    // only need to free the items list, since every other string is on the stack
    darray_free(&list);
}

// malloc'd strings
static char *dup_str(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);

    if (p)
        memcpy(p, s, n);
    return p;
}

static void example_owned_strings(void)
{
    darray list = darray_init(sizeof(char *), 0);
    const char *words[] = { "prune", "the", "vines" };
    size_t i;

    for (i = 0; i < sizeof words / sizeof *words; i++) {
        char *copy = dup_str(words[i]);
        if (!copy || !darray_append(&list, &copy)) {
            free(copy);
            break;
        }
    }

    printf("malloc'd:");
    for (i = 0; i < list.len; i++)
        printf(" %s", *(char **)darray_at(&list, i));
    printf("\n");

    // darray_free never touches what the elements point to
    // so we need to free each one individually
    for (i = 0; i < list.len; i++)
        free(*(char **)darray_at(&list, i));
    darray_free(&list);
}

// bounds and pointer invalidation
static void example_gotchas(void)
{
    darray a = darray_init(sizeof(int), 1);
    int *first;
    size_t i;

    darray_append(&a, &(int){ 1 });

    // out of range gives NULL
    printf("at(99) = %p\n", (void *)darray_at(&a, 99));

    // a pointer into the array stays valid only until the next regrow
    first = darray_at(&a, 0);
    for (i = 0; i < 64; i++)
        darray_append(&a, &(int){ 0 });
    // 'first' is dangling now, since realloc may have moved the block.
    // we need to re-fetch after any append that can grow
    first = darray_at(&a, 0);
    printf("first = %d (len=%zu, cap=%zu)\n", *first, a.len, a.cap);

    // darray_free leaves the struct reusable
    darray_free(&a);
    darray_append(&a, &(int){ 7 });
    printf("reused after free: %d\n", *(int *)darray_at(&a, 0));
    darray_free(&a);
}

int main(void)
{
    example_int();
    example_double();
    example_struct();
    example_string_buffer();
    example_string_list();
    example_owned_strings();
    example_gotchas();
    return 0;
}
