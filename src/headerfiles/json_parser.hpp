// #include <iostream>
// #include <iomanip>
// #include <vector>
// #include <map>

// // Forward declaration
// void prettyPrintJson(const std::map<std::string, json>& jsonObj, int indentationLevel);

// void prettyPrintJson(const json& jsonObj, int indentationLevel = 0) {
//     if (jsonObj.is_object()) {
//         prettyPrintJson(jsonObj.get<std::map<std::string, json>>(), indentationLevel);
//     } else if (jsonObj.is_array()) {
//         const auto& arr = jsonObj.get<std::vector<json>>();
//         for (const auto& element : arr) {
//             prettyPrintJson(element, indentationLevel + 1);
//         }
//     } else {
//         std::cout << std::setw(indentationLevel * 4) << jsonObj.dump() << std::endl;
//     }
// }

// void prettyPrintJson(const std::map<std::string, json>& jsonObj, int indentationLevel) {
//     std::cout << "{" << std::endl;
//     for (const auto& pair : jsonObj) {
//         std::cout << std::setw((indentationLevel + 1) * 4) << "\"" << pair.first << "\": ";
//         prettyPrintJson(pair.second, indentationLevel + 1);
//     }
//     std::cout << std::setw(indentationLevel * 4) << "}" << std::endl;
// }