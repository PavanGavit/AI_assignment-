// csp_map_coloring.cpp
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
using namespace std;

/*
Simple CSP solver with backtracking + forward checking.
Example solves Australia map coloring with 3 colors.
*/

using Var = string;
using Domain = vector<string>;
using Assignment = unordered_map<Var, string>;

vector<pair<Var, Var>> neighbors = {
    {"WA","NT"},{"WA","SA"},{"NT","SA"},{"NT","Q"},{"SA","Q"},{"SA","NSW"},
    {"SA","V"},{"Q","NSW"},{"NSW","V"}
};

vector<Var> variables = {"WA","NT","SA","Q","NSW","V","T"};

unordered_map<Var, Domain> init_domains(){
    Domain colors = {"red","green","blue"};
    unordered_map<Var, Domain> d;
    for(auto &v: variables) d[v]=colors;
    return d;
}

bool consistent(const Var &v, const string &val, const Assignment &assign){
    for(auto &p: neighbors){
        if(p.first==v){
            auto it=assign.find(p.second);
            if(it!=assign.end() && it->second==val) return false;
        } else if(p.second==v){
            auto it=assign.find(p.first);
            if(it!=assign.end() && it->second==val) return false;
        }
    }
    return true;
}

bool forward_check(const Var &var, const string &val, unordered_map<Var, Domain> &domains, const Assignment &assign){
    // reduce domains for neighbors; return false if some domain becomes empty
    for(auto &p: neighbors){
        Var nb="";
        if(p.first==var) nb=p.second;
        else if(p.second==var) nb=p.first;
        if(nb=="") continue;
        if(assign.count(nb)) continue;
        auto &dom = domains[nb];
        size_t before = dom.size();
        dom.erase(remove(dom.begin(), dom.end(), val), dom.end());
        if(dom.empty()) return false;
    }
    return true;
}

bool backtrack(Assignment &assign, unordered_map<Var, Domain> &domains){
    if(assign.size()==variables.size()) return true;
    // MRV heuristic
    Var sel="";
    size_t best = SIZE_MAX;
    for(auto &v: variables) if(!assign.count(v)){
        if(domains[v].size() < best){ best = domains[v].size(); sel=v; }
    }
    if(sel=="") return false;
    Domain domain_copy = domains[sel];
    for(auto &val: domain_copy){
        if(!consistent(sel,val,assign)) continue;
        // save domains snapshot
        auto saved = domains;
        assign[sel]=val;
        bool ok = forward_check(sel,val,domains,assign);
        if(ok && backtrack(assign,domains)) return true;
        // restore
        domains = saved;
        assign.erase(sel);
    }
    return false;
}

int main(){
    auto domains = init_domains();
    Assignment assign;
    bool solved = backtrack(assign, domains);
    if(solved){
        cout<<"Solution:\n";
        for(auto &v: variables) cout<<v<<": "<<assign[v]<<"\n";
    } else cout<<"No solution found\n";
    return 0;
}
