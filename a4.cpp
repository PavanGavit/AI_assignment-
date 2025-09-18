#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>

using namespace std;

struct PairHash {
    size_t operator()(const pair<int,int>& p) const noexcept {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

using P = pair<int,int>;

struct PQItem {
    int f, g;
    P pos;
    bool operator>(const PQItem& other) const {
        return f > other.f;
    }
};

vector<P> reconstruct_path(unordered_map<P, P, PairHash>& cameFrom, P current) {
    vector<P> total_path = {current};
    while (cameFrom.find(current) != cameFrom.end()) {
        current = cameFrom[current];
        total_path.push_back(current);
    }
    reverse(total_path.begin(), total_path.end());
    return total_path;
}

int heuristic(P a, P b) {
    return abs(a.first - b.first) + abs(a.second - b.second); // Manhattan distance
}

bool AStar(vector<vector<int>>& grid, P start, P goal) {
    int rows = grid.size(), cols = grid[0].size();

    priority_queue<PQItem, vector<PQItem>, greater<PQItem>> open;
    unordered_map<P, int, PairHash> gScore;
    unordered_map<P, P, PairHash> cameFrom;
    unordered_set<P, PairHash> closed;

    gScore[start] = 0;
    open.push({heuristic(start, goal), 0, start});

    vector<int> dir = {0,1,0,-1,0};

    while (!open.empty()) {
        PQItem current = open.top();
        open.pop();

        if (current.pos == goal) {
            auto path = reconstruct_path(cameFrom, current.pos);
            cout << "Path found:\n";
            for (auto& p : path) {
                cout << "(" << p.first << "," << p.second << ") ";
            }
            cout << endl;
            return true;
        }

        if (closed.count(current.pos)) continue;
        closed.insert(current.pos);

        for (int i = 0; i < 4; i++) {
            int nr = current.pos.first + dir[i];
            int nc = current.pos.second + dir[i+1];
            if (nr < 0 || nc < 0 || nr >= rows || nc >= cols || grid[nr][nc] == 1) continue;

            P neighbor = {nr, nc};
            int tentative_gScore = current.g + 1;

            if (!gScore.count(neighbor) || tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = current.pos;
                gScore[neighbor] = tentative_gScore;
                int f = tentative_gScore + heuristic(neighbor, goal);
                open.push({f, tentative_gScore, neighbor});
            }
        }
    }

    cout << "No path found." << endl;
    return false;
}

int main() {
    vector<vector<int>> grid = {
        {0,0,0,0,0},
        {1,1,0,1,0},
        {0,0,0,0,0},
        {0,1,1,1,0},
        {0,0,0,0,0}
    };

    P start = {0,0};
    P goal = {4,4};

    AStar(grid, start, goal);

    return 0;
}
