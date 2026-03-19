# Arena Allocator in C

A fast and simple **arena memory allocator** implemented in C.  

---

## Installation

1. Include the arena headers in your project:

```c
#include "arena.h"
```

2. Compile your project with the implementation:

```bash
gcc -o my_program main.c arena.c
```

> **Note:** Make sure arena.c and arena.h are in your project directory, or properly included in your compiler's include path.
---

## Usage

### Initialize an Arena
Create a new arena by specifying an initial size and configuration flags.
```c
arena_t *arena = arena_init(INITIAL_SIZE, ARENA_ALLOCATE_AND_ZERO | ARENA_DEBUG);
```

### Allocate Memory
Request memory from the arena. There is no need to free() individual pointers.
```c
my_struct *ptr = arena_alloc(arena, sizeof(my_struct));
```
### Undo / Rollback
Revert the last allocation made in the arena.
```c
arena_undo(arena);
```

### Reset Arena
Clear all allocations within the arena without freeing the underlying buffer, making it ready for reuse.
```c
arena_free(arena);
```

### Destroy Arena
Free the entire arena and all memory associated with it.
```c
arena_destroy(arena);
```
---

## Flags

Use these flags during initialization to customize the allocator's behavior:

| Flag              | Hex Value   | Description                                      |
| :--------------   | :--------   | :----------------------------------------------- |
| `ARENA_ZERO`      | `0x1`       | Automatically zero-initialize all allocations.   |
| `ARENA_DEBUG`     | `0x2`       | Enable debug logging to stdout.                  |
| `ARENA_DYNAMIC`   | `0x4`       | Dynamic growth of the arena                      |
---

## Example
```c
#include "arena.h"

int main() {
    // 1. Initialize arena with 1KB and debug logging enabled
    arena_t *arena = arena_init(1024, ARENA_ZERO | ARENA_DEBUG);

    // 2. Allocate memory for 10 integers
    int *numbers = arena_alloc(arena, 10 * sizeof(int));

    // 3. Rollback the last allocation if needed
    arena_undo(arena); 

    // 4. Cleanup everything at once
    arena_destroy(arena);
    
    return 0;
}
```