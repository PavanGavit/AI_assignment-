// eight_puzzle_search.cpp
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <string>
using namespace std;

using State = array<int,9>;
string to_key(const State &s){ string k; for(int x:s){ k+=char(x+ '0'); } return k; }
int inv_count(const State &s){
    int inv=0;
    for(int i=0;i<9;i++) if(s[i])
        for(int j=i+1;j<9;j++) if(s[j] && s[i]>s[j]) inv++;
    return inv;
}
bool solvable(const State &start,const State &goal){
    return (inv_count(start)%2)==(inv_count(goal)%2);
}
vector<State> neighbors(const State &s){
    int z=-1; for(int i=0;i<9;i++) if(s[i]==0) { z=i; break;}
    int r=z/3,c=z%3;
    vector<State> out;
    const int dr[4]={-1,1,0,0}, dc[4]={0,0,-1,1};
    for(int k=0;k<4;k++){
        int nr=r+dr[k], nc=c+dc[k];
        if(nr>=0 && nr<3 && nc>=0 && nc<3){
            State t=s;
            swap(t[z], t[nr*3+nc]);
            out.push_back(t);
        }
    }
    return out;
}
void print_state(const State &s){
    for(int i=0;i<9;i++){
        if(s[i]==0) cout<<' ';
        else cout<<s[i];
        if(i%3==2) cout<<"\n"; else cout<<" ";
    }
    cout<<"\n";
}

// BFS
bool bfs_solve(const State &start,const State &goal, vector<State> &path){
    queue<State> q; unordered_map<string,string> parent; // key->parentkey
    string sk=to_key(start), gk=to_key(goal);
    q.push(start); parent[sk]="";
    while(!q.empty()){
        State cur=q.front(); q.pop();
        string ck=to_key(cur);
        if(ck==gk){
            // reconstruct
            path.clear();
            string curk = gk;
            while(curk!=""){
                State s;
                for(int i=0;i<9;i++) s[i]=curk[i]-'0';
                path.push_back(s);
                curk = parent[curk];
            }
            reverse(path.begin(), path.end());
            return true;
        }
        for(auto &n: neighbors(cur)){
            string nk=to_key(n);
            if(!parent.count(nk)){
                parent[nk]=ck;
                q.push(n);
            }
        }
    }
    return false;
}

// IDDFS wrapper for DFS
bool dls(const State &cur,const State &goal,int depth, unordered_set<string> &visited, vector<State> &path){
    string ck=to_key(cur);
    if(ck==to_key(goal)){ path.push_back(cur); return true; }
    if(depth==0) return false;
    visited.insert(ck);
    for(auto &n: neighbors(cur)){
        string nk=to_key(n);
        if(visited.count(nk)) continue;
        if(dls(n,goal,depth-1,visited,path)){
            path.push_back(cur);
            return true;
        }
    }
    visited.erase(ck);
    return false;
}
bool iddfs_solve(const State &start,const State &goal, vector<State> &path,int max_depth=30){
    for(int d=0;d<=max_depth;d++){
        unordered_set<string> visited; path.clear();
        if(dls(start,goal,d,visited,path)){
            reverse(path.begin(), path.end());
            return true;
        }
    }
    return false;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    // Example usage:
    // Input 9 integers for start, optional 9 integers for goal.
    // Example start: 1 2 3 4 0 6 7 5 8
    cout<<"Enter 9 numbers for start state (0 for blank):\n";
    State start, goal;
    for(int i=0;i<9;i++) if(!(cin>>start[i])) return 0;
    cout<<"Enter 9 numbers for goal state (or press enter to use 1..8,0):\n";
    bool haveGoal=true;
    for(int i=0;i<9;i++){
        if(!(cin>>goal[i])) { haveGoal=false; break;}
    }
    if(!haveGoal){
        goal = {1,2,3,4,5,6,7,8,0};
    }
    if(!solvable(start,goal)){
        cout<<"This puzzle is not solvable (parity mismatch).\n"; return 0;
    }

    vector<State> path;
    cout<<"\nRunning BFS (shortest path)...\n";
    if(bfs_solve(start,goal,path)){
        cout<<"Solved in "<<path.size()-1<<" moves.\n";
        for(auto &s: path){ print_state(s); }
    } else cout<<"BFS could not find solution within memory limits.\n";

    cout<<"\nRunning IDDFS (depth-limited DFS)…\n";
    if(iddfs_solve(start,goal,path,30)){
        cout<<"Solved in "<<path.size()-1<<" moves (IDDFS).\n";
        for(auto &s: path){ print_state(s); }
    } else cout<<"IDDFS failed up to depth 30.\n";
    return 0;
}
