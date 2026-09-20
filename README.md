# kudzu

Tiny dynamic array header, mostly meant for me.

Supports C99. Only tested on amd64. Only depends on libc.

## Usage

```
#define KUDZU_IMPLEMENTATION
#include "kudzu.h"
```

See examples.c for the rest.

## Trivia

Q: Why such an edgy name like "kudzu"? WTH does that even mean? \
A: I came across the name in an [article](https://vickiboykis.com/2026/09/01/bad-code-is-kudzu/) and thought it was cool. It fits on two levels: dynamic arrays grow like kudzu does, and the author argues that bad code is like kudzu -- this is pretty bad code, so it is pretty much like kudzu.
