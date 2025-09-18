// eight_queens.cpp
#include <iostream>
#include <vector>
using namespace std;

int N=8;
vector<int> cols;
vector<vector<int>> solutions;

bool safe(int r,int c){
    for(int i=0;i<r;i++){
        if(cols[i]==c) return false;
        if(abs(cols[i]-c)==r-i) return false;
    }
    return true;
}

void solve(int r){
    if(r==N){ solutions.push_back(cols); return; }
    for(int c=0;c<N;c++){
        if(safe(r,c)){ cols[r]=c; solve(r+1); }
    }
}

int main(){
    cols.assign(N, -1);
    solve(0);
    cout<<"Found "<<solutions.size()<<" solutions for "<<N<<"-Queens.\n";
    // print first 3 solutions
    for(size_t k=0;k<solutions.size() && k<3;k++){
        cout<<"Solution "<<k+1<<":\n";
        for(int r=0;r<N;r++){
            for(int c=0;c<N;c++) cout<<(solutions[k][r]==c?'Q':'.');
            cout<<"\n";
        }
        cout<<"\n";
    }
    return 0;
}
