#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <sstream>

using namespace std;

// Variant to hold either int or string
using Value = variant<int, string>;

struct Variable {
    string type;  // "int" or "string"
    Value value;
};

class ScopeManager {
private:
    // Stack of scopes, each scope is an unordered_map of variable name -> Variable
    vector<unordered_map<string, Variable>> scopes;

public:
    ScopeManager() {
        // Start with global scope
        scopes.push_back(unordered_map<string, Variable>());
        scopes.reserve(100);  // Pre-allocate for up to 100 scopes
    }

    void indent() {
        scopes.push_back(unordered_map<string, Variable>());
    }

    void dedent() {
        if (scopes.size() > 1) {
            scopes.pop_back();
        }
    }

    // Declare a variable in the current scope
    bool declare(const string& type, const string& name, const Value& value) {
        // Check if variable already exists in current scope
        if (scopes.back().find(name) != scopes.back().end()) {
            return false;  // Already declared in this scope
        }
        scopes.back()[name] = {type, value};
        return true;
    }

    // Find variable in current or parent scopes
    Variable* findVariable(const string& name) {
        // Search from current scope backwards to global scope
        for (int i = scopes.size() - 1; i >= 0; i--) {
            auto it = scopes[i].find(name);
            if (it != scopes[i].end()) {
                return &it->second;
            }
        }
        return nullptr;  // Not found
    }
};

// Parse string value (remove quotes)
string parseStringValue(const string& s) {
    if (s.length() >= 2 && s[0] == '"' && s[s.length()-1] == '"') {
        return s.substr(1, s.length() - 2);
    }
    return s;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    cin.ignore();  // Ignore newline after n

    ScopeManager manager;

    for (int i = 0; i < n; i++) {
        string line;
        getline(cin, line);

        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "Indent") {
            manager.indent();
        }
        else if (command == "Dedent") {
            manager.dedent();
        }
        else if (command == "Declare") {
            string type, name, valueStr;
            iss >> type >> name;

            if (type == "int") {
                int value;
                if (!(iss >> value)) {
                    cout << "Invalid operation\n";
                    continue;
                }
                if (!manager.declare(type, name, value)) {
                    cout << "Invalid operation\n";
                }
            }
            else if (type == "string") {
                // Read rest of line for string value
                string restOfLine;
                getline(iss, restOfLine);
                // Trim leading space
                size_t start = restOfLine.find_first_not_of(" \t");
                if (start != string::npos) {
                    valueStr = restOfLine.substr(start);
                } else {
                    cout << "Invalid operation\n";
                    continue;
                }

                string value = parseStringValue(valueStr);
                if (!manager.declare(type, name, value)) {
                    cout << "Invalid operation\n";
                }
            }
            else {
                cout << "Invalid operation\n";
            }
        }
        else if (command == "Print") {
            string name;
            iss >> name;

            Variable* var = manager.findVariable(name);
            if (var == nullptr) {
                cout << "Invalid operation\n";
            }
            else {
                cout << name << ":";
                if (var->type == "int") {
                    cout << get<int>(var->value);
                }
                else {
                    cout << get<string>(var->value);
                }
                cout << "\n";
            }
        }
        else if (command == "SelfAdd") {
            string name;
            iss >> name;

            Variable* var = manager.findVariable(name);
            if (var == nullptr) {
                cout << "Invalid operation\n";
                continue;
            }

            if (var->type == "int") {
                int addValue;
                if (!(iss >> addValue)) {
                    cout << "Invalid operation\n";
                    continue;
                }
                var->value = get<int>(var->value) + addValue;
            }
            else if (var->type == "string") {
                string restOfLine;
                getline(iss, restOfLine);
                size_t start = restOfLine.find_first_not_of(" \t");
                if (start == string::npos) {
                    cout << "Invalid operation\n";
                    continue;
                }
                string valueStr = restOfLine.substr(start);
                string addValue = parseStringValue(valueStr);
                var->value = get<string>(var->value) + addValue;
            }
        }
        else if (command == "Add") {
            string result, value1Name, value2Name;
            iss >> result >> value1Name >> value2Name;

            Variable* resultVar = manager.findVariable(result);
            Variable* value1Var = manager.findVariable(value1Name);
            Variable* value2Var = manager.findVariable(value2Name);

            if (resultVar == nullptr || value1Var == nullptr || value2Var == nullptr) {
                cout << "Invalid operation\n";
                continue;
            }

            // Check that all three have the same type
            if (resultVar->type != value1Var->type || resultVar->type != value2Var->type) {
                cout << "Invalid operation\n";
                continue;
            }

            if (resultVar->type == "int") {
                resultVar->value = get<int>(value1Var->value) + get<int>(value2Var->value);
            }
            else {
                resultVar->value = get<string>(value1Var->value) + get<string>(value2Var->value);
            }
        }
    }

    return 0;
}
