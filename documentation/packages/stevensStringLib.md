# Create new project

```
beldum create stevensStringLib_test && cd stevensStringLib_test
beldum install stevensStringLib
```

### Paste the following in `main.cpp`:
```cpp
#include "stevensStringLib.h"

int main()
{
    stevensStringLib::containsOnly("111011112222", "12");
    return 0;
}
```

# Build project:
```
beldum run build
beldum run execute
```