#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
using namespace std;

/*
 Backward Chaining for definite (Horn) clauses.

 Input format:
 1) Line: initial facts (space separated tokens). Example: A B
 2) Line: integer R = number of rules
 3) Next R lines: each rule like "P1 P2 => Q" (antecedents separated by spaces, then "=>" or "->", then single conclusion)
 4) Line: query symbol (single token)

 Output: whether query is entailed by the knowledge base.
*/

struct Rule {
    vector<string> antecedents;
    string conclusion;
};

// Utility: split a string by whitespace
static inline vector<string> split_ws(const string &s) {
    vector<string> out;
    string token;
    istringstream iss(s);
    while (iss >> token) out.push_back(token);
    return out;
}

// Recursive backward chaining
bool backward_chain(const string &query,
                    const unordered_set<string> &facts,
                    const unordered_map<string, vector<Rule>> &rules,
                    unordered_set<string> &visited) {
    // If query is already known fact
    if (facts.count(query)) return true;

    // Avoid infinite loops (circular reasoning)
    if (visited.count(query)) return false;
    visited.insert(query);

    // If there are no rules that conclude this query, fail
    auto it = rules.find(query);
    if (it == rules.end()) return false;

    // Try each rule that concludes query
    for (const Rule &r : it->second) {
        bool allTrue = true;
        for (const string &ant : r.antecedents) {
            if (!backward_chain(ant, facts, rules, visited)) {
                allTrue = false;
                break;
            }
        }
        if (allTrue) return true; // all antecedents satisfied
    }

    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    // Read initial facts
    if (!getline(cin, line)) return 0;
    vector<string> factList = split_ws(line);
    unordered_set<string> facts(factList.begin(), factList.end());

    // Read number of rules
    if (!getline(cin, line)) return 0;
    int R = stoi(line);

    vector<Rule> allRules;
    allRules.reserve(R);
    unordered_map<string, vector<Rule>> rulesByConclusion;

    // Read rules
    for (int i = 0; i < R; i++) {
        if (!getline(cin, line)) return 0;
        size_t pos = line.find("->");
        if (pos != string::npos) line.replace(pos, 2, "=>");
        size_t arrow = line.find("=>");
        if (arrow == string::npos) {
            cerr << "Rule format error at line " << (i+1) << "\n";
            return 0;
        }
        string left = line.substr(0, arrow);
        string right = line.substr(arrow + 2);
        vector<string> ants = split_ws(left);
        vector<string> rhs = split_ws(right);
        if (rhs.size() != 1) {
            cerr << "Rule must have exactly one RHS symbol.\n";
            return 0;
        }
        Rule r;
        r.antecedents = std::move(ants);
        r.conclusion = rhs[0];
        rulesByConclusion[r.conclusion].push_back(r);
        allRules.push_back(r);
    }

    // Read query
    if (!getline(cin, line)) return 0;
    string query;
    {
        istringstream iss(line);
        iss >> query;
    }

    unordered_set<string> visited;
    bool result = backward_chain(query, facts, rulesByConclusion, visited);

    if (result) {
        cout << "Query " << query << " is entailed by the knowledge base.\n";
    } else {
        cout << "Query " << query << " is NOT entailed by the knowledge base.\n";
    }

    return 0;
}
