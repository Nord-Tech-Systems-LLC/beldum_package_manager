# Create new project

```
beldum create stevensMapLib_test && cd stevensMapLib_test
beldum install stevensMapLib
```

### Paste the following in `main.cpp`:

```cpp
#include "stevensMapLib.hpp"

#include <map>
#include <string>
#include <iostream>

int main()
{
    // Define a map of string keys and double values
    std::map<std::string, double> prices = {
        {"apple", 1.0},
        {"banana", 0.5},
        {"cherry", 2.0}
    };

    // Factor to multiply each price by
    long double factor = 1.1; // For example, a 10% price increase

    // Call the function to multiply each value in the map
    prices = stevensMapLib::multiplyWithValues(prices, factor);

    // Output the updated map
    for (const auto& [key, value] : prices) {
        std::cout << key << ": " << value << std::endl;
    }

    return 0;
}
```

# Build project:
```
beldum run build
beldum run execute
```