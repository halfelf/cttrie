# Compile-time TRIE based string matcher (C++11)

Upgraded from [original library](https://github.com/smilingthax/cttrie) written by Tobias Hoffmann.

Using C++17 std::string_view and std::index_sequence.


Usage:

```
#include "cttrie.h"

...

  TRIE(str)
    ... error case
  CASE("abc")
    ... str matches abc ...
  CASE("ad")
    ... etc
  ENDTRIE;   // <- trailing semicolon!
```

* compile with optimization enabled!
* cases may return a value (see e.g. test_cttrie.cpp)
  -> return types must match! (deduced from error case)
* using [cmake](https://cmake.org/) and [conan](https://conan.io/) to build/run tests.
