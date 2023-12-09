#include <iostream>
#include <map>
#include <sstream>
#include <vector>

enum class JSONType {
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOLEAN,
    NULL_VALUE
};

class JSONValue {
   public:
    JSONType type;
    void print_json();

    virtual ~JSONValue() {}
};

class JSONObject : public JSONValue {
   public:
    std::map<std::string, JSONValue*> values;

    ~JSONObject() override {
        for (auto& pair : values) {
            delete pair.second;
        }
    }
};

class JSONArray : public JSONValue {
   public:
    std::vector<JSONValue*> elements;

    ~JSONArray() override {
        for (auto& element : elements) {
            delete element;
        }
    }
};

class JSONString : public JSONValue {
   public:
    std::string value;
};

class JSONNumber : public JSONValue {
   public:
    double value;
};

class JSONBoolean : public JSONValue {
   public:
    bool value;
};

class JSONNull : public JSONValue {};

class JSONParser {
   public:
    JSONValue* parse(const std::string& json);

   private:
    JSONValue* parseValue(std::istringstream& stream);

    JSONObject* parseObject(std::istringstream& stream);

    JSONArray* parseArray(std::istringstream& stream);

    JSONString* parseString(std::istringstream& stream);

    JSONNumber* parseNumber(std::istringstream& stream);

    JSONBoolean* parseBoolean(std::istringstream& stream);

    JSONNull* parseNull(std::istringstream& stream);
};

JSONValue* JSONParser::parse(const std::string& json) {
    std::istringstream stream(json);
    JSONValue* parsedValue = parseValue(stream);

    std::cout << "JSON: " << parsedValue << std::endl;
    return parsedValue;
}

JSONValue* JSONParser::parseValue(std::istringstream& stream) {
    char nextChar;
    stream >> std::ws;  // Skip whitespace

    // Peek the next character to determine the value type
    if (stream.peek() == '{') {
        return parseObject(stream);
    } else if (stream.peek() == '[') {
        return parseArray(stream);
    } else if (stream.peek() == '"') {
        return parseString(stream);
    } else if (std::isdigit(stream.peek()) || stream.peek() == '-') {
        return parseNumber(stream);
    } else if (stream.peek() == 't' || stream.peek() == 'f') {
        return parseBoolean(stream);
    } else if (stream.peek() == 'n') {
        return parseNull(stream);
    }

    return nullptr;  // Placeholder
}

JSONObject* JSONParser::parseObject(std::istringstream& stream) {
    JSONObject* obj = new JSONObject();
    stream.ignore();  // Consume '{'

    while (stream.peek() != '}') {
        // Parse key
        JSONString* key = parseString(stream);
        if (!key) {
            delete obj;
            return nullptr;  // Invalid key
        }

        // Consume ':'
        stream >> std::ws;  // Skip whitespace
        if (stream.get() != ':') {
            delete obj;
            delete key;
            return nullptr;  // Expected ':'
        }

        // Parse value
        JSONValue* value = parseValue(stream);
        if (!value) {
            delete obj;
            delete key;
            return nullptr;  // Invalid value
        }

        // Add key-value pair to the object
        obj->values[key->value] = value;

        // Consume ',' if present
        stream >> std::ws;  // Skip whitespace
        if (stream.peek() == ',') {
            stream.ignore();  // Consume ','
        } else if (stream.peek() != '}') {
            delete obj;
            delete key;
            delete value;
            return nullptr;  // Expected ',' or '}'
        }
    }

    stream.ignore();  // Consume '}'
    return obj;
}

JSONArray* JSONParser::parseArray(std::istringstream& stream) {
    JSONArray* arr = new JSONArray();
    stream.ignore();  // Consume '['

    while (stream.peek() != ']') {
        // Parse element
        JSONValue* element = parseValue(stream);
        if (!element) {
            delete arr;
            return nullptr;  // Invalid element
        }

        // Add element to the array
        arr->elements.push_back(element);

        // Consume ',' if present
        stream >> std::ws;  // Skip whitespace
        if (stream.peek() == ',') {
            stream.ignore();  // Consume ','
        } else if (stream.peek() != ']') {
            delete arr;
            delete element;
            return nullptr;  // Expected ',' or ']'
        }
    }

    stream.ignore();  // Consume ']'
    return arr;
}

JSONString* JSONParser::parseString(std::istringstream& stream) {
    JSONString* str = new JSONString();
    stream.ignore();  // Consume '"'

    // Read characters until closing '"'
    while (stream.peek() != '"' && stream.peek() != EOF) {
        char nextChar = stream.get();
        if (nextChar == '\\') {
            // Handle escape characters if needed
            // (For simplicity, this example doesn't handle all escape characters)
            char escapedChar = stream.get();
            switch (escapedChar) {
                case 'n':
                    str->value += '\n';
                    break;
                case 't':
                    str->value += '\t';
                    break;
                // Add other escape character handling as needed
                default:
                    str->value += escapedChar;
            }
        } else {
            str->value += nextChar;
        }
    }

    if (stream.peek() == '"') {
        stream.ignore();  // Consume closing '"'
        return str;
    } else {
        delete str;
        return nullptr;  // Expected closing '"'
    }
}

JSONNumber* JSONParser::parseNumber(std::istringstream& stream) {
    JSONNumber* num = new JSONNumber();
    stream >> num->value;
    return num;
}

JSONBoolean* JSONParser::parseBoolean(std::istringstream& stream) {
    JSONBoolean* boolean = new JSONBoolean();
    if (stream.peek() == 't') {
        boolean->value = true;
        stream.ignore(4);  // Consume 'true'
    } else if (stream.peek() == 'f') {
        boolean->value = false;
        stream.ignore(5);  // Consume 'false'
    } else {
        delete boolean;
        return nullptr;  // Expected 'true' or 'false'
    }

    return boolean;
}

JSONNull* JSONParser::parseNull(std::istringstream& stream) {
    JSONNull* null = new JSONNull();
    stream.ignore(4);  // Consume 'null'
    return null;
}

// Helper function to print JSON values
void printJSONValue(const JSONValue* value, int indent = 0) {
    if (!value) {
        std::cout << "null";
        return;
    }

    switch (value->type) {
        case JSONType::OBJECT: {
            const JSONObject* obj = dynamic_cast<const JSONObject*>(value);
            std::cout << "{\n";
            for (const auto& pair : obj->values) {
                std::cout << std::string(indent + 2, ' ') << "\"" << pair.first << "\": ";
                printJSONValue(pair.second, indent + 2);
                std::cout << ",\n";
            }
            std::cout << std::string(indent, ' ') << "}";
            break;
        }
        case JSONType::ARRAY: {
            const JSONArray* arr = dynamic_cast<const JSONArray*>(value);
            std::cout << "[\n";
            for (const auto& element : arr->elements) {
                std::cout << std::string(indent + 2, ' ');
                printJSONValue(element, indent + 2);
                std::cout << ",\n";
            }
            std::cout << std::string(indent, ' ') << "]";
            break;
        }
        case JSONType::STRING: {
            const JSONString* str = dynamic_cast<const JSONString*>(value);
            std::cout << "\"" << str->value << "\"";
            break;
        }
        case JSONType::NUMBER: {
            const JSONNumber* num = dynamic_cast<const JSONNumber*>(value);
            std::cout << num->value;
            break;
        }
        case JSONType::BOOLEAN: {
            const JSONBoolean* boolean = dynamic_cast<const JSONBoolean*>(value);
            std::cout << (boolean->value ? "true" : "false");
            break;
        }
        case JSONType::NULL_VALUE: {
            std::cout << "null";
            break;
        }
    }
}

void JSONValue::print_json() {
    std::cout << "testing" << std::endl;
}

// Implement the remaining parsing functions for object, array, string, number, boolean, and null

// int main() {
//     std::string json = R"(
//         {
//             "name": "John",
//             "age": 30,
//             "city": "New York",
//             "isStudent": false,
//             "grades": [90, 85, 92],
//             "address": {
//                 "street": "123 Main St",
//                 "zip": "10001"
//             },
//             "isNull": null
//         }
//     )";

//     JSONParser parser;
//     JSONValue* root = parser.parse(json);

//     // Process the parsed JSON data here

//     // Clean up memory
//     delete root;

//     return 0;
// }
