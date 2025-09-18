// family_kb.cpp
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
using namespace std;

/*
Simple family KB parser.
Input lines:
parent A B
parent B C
...
after facts, type "queries" then lines like:
is_parent A B
is_grandparent A C
is_sibling X Y
is_ancestor A D
Type "exit" to stop.
*/

unordered_map<string, vector<string>> children_of;
unordered_map<string, vector<string>> parents_of;

bool is_parent(const string &a,const string &b){
    for(auto &c: children_of[a]) if(c==b) return true;
    return false;
}
bool is_grandparent(const string &a,const string &b){
    for(auto &c: children_of[a]) if(is_parent(c,b)) return true;
    return false;
}
bool is_sibling(const string &a,const string &b){
    if(a==b) return false;
    for(auto &p: parents_of[a]) for(auto &q: parents_of[b]) if(p==q) return true;
    return false;
}
bool is_ancestor(const string &a,const string &b, unordered_set<string> &visited){
    if(is_parent(a,b)) return true;
    for(auto &c: children_of[a]){
        if(visited.count(c)) continue;
        visited.insert(c);
        if(is_ancestor(c,b,visited)) return true;
    }
    return false;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout<<"Enter facts (one per line), e.g. 'parent alice bob'. Type 'queries' to switch:\n";
    string line;
    while(getline(cin,line)){
        if(line.empty()) continue;
        if(line=="queries") break;
        if(line=="exit") return 0;
        istringstream iss(line);
        string pred,a,b; iss>>pred>>a>>b;
        if(pred=="parent"){
            children_of[a].push_back(b);
            parents_of[b].push_back(a);
        } else {
            cout<<"Unknown fact format. Use 'parent A B'\n";
        }
    }
    cout<<"Now enter queries. Examples: is_parent A B, is_grandparent A C, is_sibling A B, is_ancestor A D. Type 'exit' to stop.\n";
    while(getline(cin,line)){
        if(line.empty()) continue;
        if(line=="exit") break;
        istringstream iss(line);
        string q,a,b; iss>>q>>a>>b;
        if(q=="is_parent"){
            cout<<(is_parent(a,b)?"YES":"NO")<<"\n";
        } else if(q=="is_grandparent"){
            cout<<(is_grandparent(a,b)?"YES":"NO")<<"\n";
        } else if(q=="is_sibling"){
            cout<<(is_sibling(a,b)?"YES":"NO")<<"\n";
        } else if(q=="is_ancestor"){
            unordered_set<string> visited; cout<<(is_ancestor(a,b,visited)?"YES":"NO")<<"\n";
        } else {
            cout<<"Unknown query\n";
        }
    }
    return 0;
}
