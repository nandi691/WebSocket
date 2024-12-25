#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

int main() {
    // Sample JSON string
    std::string jsonString = R"(
        {
            "name": "Nandeesh",
            "age": 34,
            "skills": ["C++", "Python", "Data Science"],
            "experience": {
                "years": 12,
                "field": "System Programming"
            }
        }
    )";

    // Parse JSON string
    json jsonData = json::parse(jsonString);

    // Access JSON data
    std::cout << "Name: " << jsonData["name"] << std::endl;
    std::cout << "Age: " << jsonData["age"] << std::endl;
    std::cout << "First Skill: " << jsonData["skills"][0] << std::endl;
    std::cout << "Experience (Years): " << jsonData["experience"]["years"] << std::endl;

    // Modify JSON data
    jsonData["skills"].push_back("Machine Learning");
    jsonData["experience"]["years"] = 13;

    // Write JSON to a file
    std::ofstream outFile("output.json");
    outFile << jsonData.dump(4); // Pretty print with indent of 4 spaces
    outFile.close();

    std::cout << "Modified JSON saved to 'output.json'!" << std::endl;

    return 0;
}
