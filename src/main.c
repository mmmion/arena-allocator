#include <stdio.h>
#include <stdlib.h>
#include <arena.h>

int main()
{
    arena_t *arena = arena_init(sizeof(int) * 300, ARENA_ZERO | ARENA_DEBUG);

    if (!arena)
        return -1;

    arena_free(arena);

    int *array = (int *)arena_alloc(arena, sizeof(int) * 500); // Expand the capacity
    if (!array)                                                // Failed to allocate
        return -1;

    for (int i = 0; i < 100; i++)
    {
        array[i] = i;
        printf("%d : %d\n", i, array[i]);
    }

    arena_undo(arena);

    for (int i = 0; i < 100; i++)
    {
        printf("%d : %d\n", i, array[i]); // All 0
    }

    arena_destroy(arena);

    return 0;
}