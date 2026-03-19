#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>
#include <stdbool.h>

struct arena
{
    unsigned char *memory; // byte buffer
    size_t capacity;
    size_t offset;

    size_t previous_offset; // If undo
    size_t previous_size;

    uint8_t flags;
};

typedef struct arena arena_t;

#define DEFAULT_GROWTH_FACTOR 2
#define ARENA_MAX_SIZE (1024 * 1024 * 1024) // 1 GB

/* Flags (Powers of two) */
#define ARENA_ZERO 0x1
#define ARENA_DEBUG 0x2
#define ARENA_DYNAMIC 0x4

/* Checks */
bool valid_size(size_t size);
bool valid_arena(arena_t *arena);

/* NOTE: uint8_t flags max 8 flags */
arena_t *arena_init(size_t arena_size, uint8_t flags);

bool arena_has_space(arena_t *arena, size_t requested_size);

bool arena_grow(arena_t *arena, size_t growth_factor);
unsigned char *arena_current_ptr(arena_t *arena);
unsigned char *arena_alloc(arena_t *arena, size_t memory_size);

/* Undoes only the most recent allocation */
void arena_undo(arena_t *arena);

bool arena_free(arena_t *arena);

void arena_destroy(arena_t *arena);

/* Helpers */
static inline int arena_has_debug_flag(const arena_t *arena)
{
    return (arena->flags & ARENA_DEBUG) != 0;
}

static inline int arena_has_zero_flag(const arena_t *arena)
{
    return (arena->flags & ARENA_ZERO) != 0;
}

static inline int arena_has_dynamic_flag(const arena_t *arena)
{
    return (arena->flags & ARENA_DYNAMIC) != 0;
}

#endif // ARENA_H