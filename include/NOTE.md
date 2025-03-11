# IMPORTANT NOTES

Some notes when going through the headers of the Anvilock source tree. The project mostly uses header only code that are included in `src/main.c`

These will especially be useful if you are looking to contribute:

1. **ALWAYS USE ANVIL_SAFE_\* METHODS INSTEAD OF TRADITIONAL STDLIB MEMORY CALLS**:

It is enforced within the anvilock source tree to use custom `ANVIL_SAFE_*` memory function calls than the traditional `stdlib` calls.

```c 
int x = 100, y = 50;
/* DO NOT USE THIS */
char* image = malloc((x*y)image); // WRONG!

/* USE THIS */
char *image;
ANVIL_SAFE_ALLOC(image, char, x*y); // CORRECT!

/* DONT USE THIS */ 
free(ptr); // WRONG!

/* USE THIS */ 
ANVIL_SAFE_FREE(ptr); // CORRECT!
```

2. **WRITE MORE COMMENTS**:

Write meaningful and essential comments that explain what you are trying to achieve. 

Try to follow this format:

```c 
/*
 * @<Comment Type>:
 * 
 * <Comments>
 * ....
 * ....
 * 
 */
```

`<Comment Type>` can be **NOTE** / **IMPORTANT** / **WARNING** (similar to markdown blobs)

Do not write meaningless OR unnecessary comments, they should to the point and should explain the function / code snippet well enough. 

Doxygen like comments are also **NOT** appreciated, will sort this out in the future for more docs support.

3. **FUNCTION STYLE**:

Each function should carry out **ONE** task *only*. Currently the repository is under heavy revamp and restructuring, so you will find a lot of functions that are not optimal and that do not follow this rule. We aim for this to be the code standard in the future and are working towards it.

Therefore, it is highly advised to follow this note as it really makes it easier for maintainers to properly understand the logical flow of your code.

4. **FORMAT CODE**:

Before attempting to open a PR, do format your code by:

```bash 
make format
```

This further helps in making the codebase more streamlined and more readable.
