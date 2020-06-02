#include "ctNode.h"

#include <vector>

CTNode::CTNode() {
    paths = std::vector<std::vector<std::pair<int, int>>>();
    cost = 0;
    maxTime = 10000;
    vertexConstr = VertexConstrStruct(maxTime);
    edgeConstr = EdgeConstrStruct(maxTime);
    conflictAvoidanceTable = ConfMap();
}

bool CTNode::isCover(int V, int k, int E) { 
    // для вершинного покрытия
    int set = (1 << k) - 1; 
    int limit = (1 << V); 
    bool vis[200][200];
    while (set < limit) {
        memset(vis, 0, sizeof vis); 
        int cnt = 0;
        for (int j = 1, v = 1; j < limit; j = j << 1, v++) { 
            if (set & j) {
                for (int k = 1; k <= V; k++) { 
                    if (graph[v][k] && !vis[v][k]) { 
                        vis[v][k] = 1; 
                        vis[k][v] = 1; 
                        cnt++; 
                    } 
                } 
            } 
        }
        if (cnt == E) {
            return true; 
        }
        int c = set & -set; 
        int r = set + c; 
        set = (((r^set) >> 2) / c) | r; 
    } 
    return false; 
} 
      
int CTNode::findMinCover(int n, int m) {
    // для вершинного покрытия
    int left = 1, right = n; 
    while (right > left) { 
        int mid = (left + right) >> 1; 
        if (isCover(n, mid, m) == false) {
            left = mid + 1; 
        } else {
            right = mid; 
        }
    }
    return left; 
} 

void CTNode::insertEdge(int u, int v) { 
    // для вершинного покрытия
    graph[u][v] = 1; 
    graph[v][u] = 1;
}

void CTNode::countCost(std::string heuristic) {
    // находим цену решения
    int ln = paths.size();
    int numOfConflicts = 0;
    int agentInConflict = 0;
    int pairsInConflict = 0;
    int minVertexCover = 0;
    int sumLenghts = 0;

    if (heuristic == "normal") {
        for (int i = 0; i < ln; ++i) {
            sumLenghts += paths[i].size();
        }
        cost = sumLenghts;
        return;
    }

    if (heuristic == "vertex_cover") {
        graph = new bool *[200];
        for (int i = 0; i < 200; ++i) {
            graph[i] = new bool[200];
        } 
    }

    for (int i = 0; i < ln; ++i) {
        int wasConflict_i = 0;
        for (int j = i + 1; j < ln; ++j) {
            int wasConflict_j = 0;
            int steps = std::min(paths[i].size(), paths[j].size());
            for (int step = 0; step < steps; ++step) {
                if (paths[i][step] == paths[j][step]) {
                    ++numOfConflicts;
                    wasConflict_i = 1;
                    wasConflict_j = 1;
                    if (heuristic == "vertex_cover") {
                        insertEdge(i + 1, j + 1);
                    }
                }
            }
            pairsInConflict += wasConflict_j;
        }
        agentInConflict += wasConflict_i;
    }

    if (heuristic == "number_of_conflicts") {
        cost = numOfConflicts;
    } else if (heuristic == "number_of_conflicting_agents") {
        cost = agentInConflict;
    } else if (heuristic == "number_of_pairs") {
        cost = pairsInConflict;
    } else if (heuristic == "vertex_cover") {
        cost = findMinCover(agentInConflict, pairsInConflict);
    }
}

void CTNode::countCAT() {
    // заполняем conflict avoidance table
    int ln = paths.size();
    for (int i = 0; i < ln; ++i) {
        for (int j = i + 1; j < ln; ++j) {
            int steps = std::min(paths[i].size(), paths[j].size());
            for (int step = 0; step < steps; ++step) {
                // изменяем количество агентов, находившихся в состоянии <i, j, t>
                KeyThree state = std::make_tuple(paths[i][step].first, paths[i][step].second, step);
                if (conflictAvoidanceTable.find(state) != conflictAvoidanceTable.end()) {
                    ++conflictAvoidanceTable[state];
                } else {
                    conflictAvoidanceTable[state] = 1;
                }
            }
        }
    }
}

std::string CTNode::findConflictType(Map& map, Agents& agents, Conflict& conflict) {
    return "non-cardinal";
}

Conflict CTNode::findBestConflict(Map& map, Agents& agents, bool prioritizeConflicts) {
    int ln = paths.size();
    int maxCheck = 5;
    int curCheck = 0;
    bool foundSemiCard = false;
    Conflict bestConflict("none");

    for (int i = 0; i < ln; ++i) {
        for (int j = i + 1; j < ln; ++j) {
            int steps = std::min(paths[i].size(), paths[j].size());
            for (int step = 0; step < steps; ++step) {
                Conflict curConflict("none");

                if (paths[i][step] == paths[j][step]) {
                    curConflict = Conflict("vertex", {i, j}, step, paths[i][step], std::pair<int, int>());
                    if (!prioritizeConflicts) {
                        return curConflict;
                    }
                }

                if (step + 1 < steps) {
                    if (paths[i][step] == paths[j][step + 1] && paths[i][step + 1] == paths[j][step]) {
                        curConflict = Conflict("edge", {i, j}, step, paths[i][step], paths[i][step + 1]);
                        if (!prioritizeConflicts) {
                            return curConflict;
                        }
                    }
                }

                // если включена приоретизация конфликтов, надо определить тип конфликта
                if (prioritizeConflicts && curConflict.type != "none") {
                    std::string type = findConflictType(map, agents, curConflict);
                    ++curCheck;
                    if (type == "cardinal") {
                        return curConflict;
                    } else if (type == "semi-cardinal") {
                        bestConflict = curConflict;
                        foundSemiCard = true;
                    } else if (type == "non-cardinal" && !foundSemiCard) {
                        bestConflict = curConflict;
                    }
                    if (curCheck >= maxCheck) {
                        return bestConflict;
                    }
                }
            }
        }
    }

    return bestConflict;
}

bool CTNode::operator< (CTNode &other) {
    return cost < other.cost;
}

bool CTNode::operator== (CTNode &other) {
    return cost == other.cost;
}

bool CTNode::operator!= (CTNode &other) {
    return !(*this == other);
}