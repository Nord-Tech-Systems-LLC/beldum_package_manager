# Create new project

```
beldum create stevensVectorLib_test && cd stevensVectorLib_test
beldum install stevensVectorLib
```

### Paste the following in `main.cpp`:
```cpp
#include <vector>
#include <string>
#include "stevensVectorLib.hpp"

int main()
{

    std::vector<std::string> test_vector;
    test_vector.push_back("Testing");
    stevensVectorLib::contains(test_vector, std::string("Testing"));
    return 0;
}
```

# Build project:
```
beldum run build
beldum run execute
```