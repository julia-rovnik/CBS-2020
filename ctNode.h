#ifndef CTNODE_H
#define CTNODE_H

#include "map.h"
#include "agent.h"
#include "agents.h"
#include "searchNode.h"
#include "search.h"
#include "constr.h"

#include <vector>

// полезные структуры данных: структура для хранения вершинных конфликтов
typedef std::vector<std::set<std::pair<int, int>>> VertexConstrStruct;
// структура для хранения векторных конфликтов
typedef std::vector<std::set<std::pair<std::pair<int, int>, std::pair<int, int>>>> EdgeConstrStruct;
// структура для хранения путей (вектор пар i, j)
typedef std::vector<std::pair<int, int>> Path;

struct CTNode {
    public:
        std::vector<Path> paths;
        int cost;
        int maxTime;
        VertexConstrStruct vertexConstr;
        EdgeConstrStruct edgeConstr;
        ConfMap conflictAvoidanceTable;
        bool** graph;

        CTNode();

        bool isCover(int V, int k, int E);
        int findMinCover(int n, int m);
        void insertEdge(int u, int v);
        void countCost(std::string heuristic);
        void countCAT();
        std::string findConflictType(Map& map, Agents& agents, Conflict& conflict);
        Conflict findBestConflict(Map& map, Agents& agents, bool prioritizeConflicts);
        bool operator< (CTNode &other);
        bool operator== (CTNode &other);
        bool operator!= (CTNode &other);
};

#endif