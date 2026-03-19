#include <arena.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void debug(const char *msg, const char *type)
{
    char buffer[500];
    snprintf(buffer, sizeof(buffer), "[%s] %s\n", type, msg);
    fputs(buffer, stdout);
    fflush(stdout);
}

bool valid_size(size_t size)
{
    return size != 0 && size <= ARENA_MAX_SIZE;
}

bool valid_arena(arena_t *arena)
{
    if (!arena)
        return false;

    if (!(arena->memory))
        return false;

    return true;
}

arena_t *arena_init(size_t arena_size, uint8_t flags)
{
    if (!valid_size(arena_size))
    {
        if (flags & ARENA_DEBUG)
            debug("Failed to init the arena. The argument 'arena_size' is zero or above the maximum size.", "ERROR");
        return NULL;
    }

    arena_t *arena = (arena_t *)malloc(sizeof *arena);
    if (!arena)
    {
        debug("Failed to initialize a new arena.", "MALLOC");
        return NULL;
    }

    arena->capacity = arena_size;
    arena->previous_offset = 0;
    arena->offset = 0;
    arena->flags = flags;

    unsigned char *mem = (unsigned char *)malloc(arena_size);
    if (!mem)
    {
        debug("Failed to initialize a new arena memory.", "MALLOC");
        free(arena);
        return NULL;
    }

    if (arena_has_debug_flag(arena))
        debug("Arena has been initialized with a debug flag.", "INFO");

    if (arena_has_zero_flag(arena))
    {
        if (arena_has_debug_flag(arena))
            debug("Arena has been initialized and zeroed all its memory", "INFO");
        memset(mem, 0, arena->capacity);
    }

    arena->memory = mem;

    return arena;
}

bool arena_has_space(arena_t *arena, size_t requested_size)
{
    return arena->capacity > arena->offset + requested_size;
}

bool arena_grow(arena_t *arena, size_t growth_factor)
{
    if (!valid_arena(arena))
    {
        debug("Failed to grow the arena. The argument 'arena' is NULL.", "ERROR");
        return false;
    }

    if (!valid_size(growth_factor))
    {
        if (arena_has_debug_flag(arena))
            debug("Failed to grow the arena. The argument 'growth_factor' is zero or above the maximum size.", "ERROR");

        return false;
    }

    arena->capacity *= growth_factor;

    if (arena->capacity >= ARENA_MAX_SIZE)
        arena->capacity = ARENA_MAX_SIZE;

    unsigned char *new_mem = (unsigned char *)realloc((void *)arena->memory, arena->capacity);
    if (!new_mem)
    {
        if (arena_has_debug_flag(arena))
            debug("Failed to grow the arena. Realloc failed.", "REALLOC");

        return false;
    }

    arena->memory = new_mem;

    if (arena_has_debug_flag(arena))
        debug("The arena has been grown", "SUCCESS");

    return true;
}

unsigned char *arena_current_ptr(arena_t *arena)
{
    if (!valid_arena(arena))
    {
        debug("Failed to check current pointer to memory. The argument 'arena' is NULL.", "ERROR");
        return NULL;
    }

    if (arena->offset > arena->capacity)
    {
        debug("Couldn't check current pointer to memory. Offset exceeds capacity", "ERROR");
        return NULL;
    }

    unsigned char *mem = arena->memory + arena->offset;
    return mem;
}

unsigned char *arena_alloc(arena_t *arena, size_t memory_size)
{
    if (!valid_arena(arena))
    {
        debug("Failed to allocate. The argument 'arena' is NULL.", "ERROR");
        return NULL;
    }

    if (!valid_size(memory_size))
    {
        if (arena_has_debug_flag(arena))
            debug("Failed to allocate. The argument 'memory_size' is zero or above the maximum size.", "ERROR");

        return NULL;
    }

    if (!arena_has_space(arena, memory_size))
    {
        if (!arena_has_dynamic_flag(arena))
        {
            if (arena_has_debug_flag(arena))
                debug("The arena capacity is too small to allocate and doesn't have flag for dynamic expansion.", "WARN");

            return NULL;
        }

        if (arena_has_debug_flag(arena))
            debug("The arena capacity is too small to allocate, expanding the arena.", "INFO");

        if (!arena_grow(arena, DEFAULT_GROWTH_FACTOR))
            return NULL;

        // Checks if it actually expanded and if it still fit it means it reached the max size
        if (!arena_has_space(arena, memory_size))
        {
            debug("The arena capacity is too small to allocate and also reached the maximum size.", "INFO");
            return NULL;
        }
    }

    arena->previous_offset = arena->offset;
    arena->previous_size = memory_size;
    unsigned char *mem = arena_current_ptr(arena);
    arena->offset += memory_size;

    return mem;
}

void arena_undo(arena_t *arena)
{
    if (!valid_arena(arena))
    {
        debug("Cannot undo. The argument 'arena' is NULL.", "ERROR");
        return;
    }

    if (arena_has_zero_flag(arena))
    {
        if (arena_has_debug_flag(arena))
            debug("Arena has been undone and its memory zeroed", "INFO");
        memset(arena->memory + arena->previous_offset, 0, arena->previous_size);
    }

    arena->offset = arena->previous_offset;
}

bool arena_free(arena_t *arena)
{
    if (!valid_arena(arena))
    {
        debug("Cannot free arena. The argument 'arena' is NULL.", "ERROR");
        return false;
    }

    free(arena->memory);
    arena->memory = NULL;
    arena->capacity = 0;
    arena->offset = 0;
    return true;
}

void arena_destroy(arena_t *arena)
{
    if (!arena_free(arena))
        return;

    free(arena);
    arena = NULL;
}
