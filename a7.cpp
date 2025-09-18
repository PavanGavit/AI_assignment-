#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
using namespace std;

/*
 Forward Chaining for definite (Horn) clauses.
 Input format:
 1) Line: initial facts (space separated tokens). Example: A B C
 2) Line: integer R = number of rules
 3) Next R lines: each rule like "P1 P2 => Q" (antecedents separated by spaces, then "=>" or "->", then single conclusion)
 4) Line: query symbol (single token)
 
 Output: whether query is entailed and the proof order (derived facts).
*/

struct Rule {
    vector<string> antecedents;
    string conclusion;
    int remaining;   // count of antecedents not yet known
    bool fired = false;
};

static inline vector<string> split_ws(const string &s) {
    vector<string> out;
    string token;
    istringstream iss(s);
    while (iss >> token) out.push_back(token);
    return out;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    // Read facts line
    if (!getline(cin, line)) return 0;
    vector<string> facts = split_ws(line);

    // Read number of rules
    if (!getline(cin, line)) return 0;
    int R = stoi(line);

    vector<Rule> rules;
    rules.reserve(R);

    // For each rule, parse antecedents and conclusion
    for (int i = 0; i < R; ++i) {
        if (!getline(cin, line)) {
            cerr << "Unexpected end of input while reading rules.\n";
            return 0;
        }
        // Normalize arrow tokens
        size_t pos = line.find("->");
        if (pos != string::npos) line.replace(pos, 2, "=>");

        // Split by "=>"
        size_t arrow = line.find("=>");
        if (arrow == string::npos) {
            cerr << "Rule format error on line " << (i+1) << ". Use: P1 P2 => Q\n";
            return 0;
        }
        string left = line.substr(0, arrow);
        string right = line.substr(arrow + 2);

        vector<string> antecedents = split_ws(left);
        vector<string> rhs = split_ws(right);
        if (rhs.size() != 1) {
            cerr << "Each rule must have exactly one symbol on the right-hand side.\n";
            return 0;
        }

        Rule r;
        r.antecedents = std::move(antecedents);
        r.conclusion = rhs[0];
        r.remaining = static_cast<int>(r.antecedents.size());
        rules.push_back(std::move(r));
    }

    // Read query
    if (!getline(cin, line)) {
        cerr << "Missing query line\n";
        return 0;
    }
    string query;
    {
        istringstream iss(line);
        iss >> query;
    }

    // Build index: for each symbol, which rules have it in antecedents
    unordered_map<string, vector<int>> index;
    for (int i = 0; i < (int)rules.size(); ++i) {
        for (const auto &ant : rules[i].antecedents) {
            index[ant].push_back(i);
        }
    }

    // Agenda: queue of known facts to process
    queue<string> agenda;
    unordered_set<string> inferred; 
    vector<string> derivedOrder;

    // initialize with given facts
    for (const auto &f : facts) {
        if (inferred.insert(f).second) {
            agenda.push(f);
            derivedOrder.push_back(f);
        }
    }

    bool entailed = false;
    if (inferred.count(query)) entailed = true;

    // Forward chaining loop
    while (!agenda.empty() && !entailed) {
        string p = agenda.front(); 
        agenda.pop();

        // For each rule that has p as an antecedent, decrement remaining
        auto it = index.find(p);
        if (it == index.end()) continue;
        for (int ruleIdx : it->second) {
            Rule &r = rules[ruleIdx];
            if (r.fired) continue; 
            r.remaining--;
            if (r.remaining == 0) {
                // All antecedents satisfied -> infer conclusion if not already known
                if (inferred.insert(r.conclusion).second) {
                    agenda.push(r.conclusion);
                    derivedOrder.push_back(r.conclusion);
                    if (r.conclusion == query) {
                        entailed = true;
                        r.fired = true;
                        break;
                    }
                }
                r.fired = true;
            }
        }
    }

    if (entailed) {
        cout << "Query " << query << " is entailed by the knowledge base.\n";
    } else {
        cout << "Query " << query << " is NOT entailed by the knowledge base.\n";
    }

    cout << "Derived facts order (" << derivedOrder.size() << "):\n";
    for (size_t i = 0; i < derivedOrder.size(); ++i) {
        cout << i+1 << ". " << derivedOrder[i] << "\n";
    }

    return 0;
}
