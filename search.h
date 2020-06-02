#ifndef SEARCH_H
#define SEARCH_H

#include "map.h"
#include "agent.h"
#include "agents.h"
#include "searchNode.h"

#include <set>
#include <list>
#include <cmath>
#include <vector>

// полезные структуры данных: структура для хранения вершинных конфликтов
typedef std::vector<std::set<std::pair<int, int>>> VertexConstrStruct;
// структура для хранения векторных конфликтов
typedef std::vector<std::set<std::pair<std::pair<int, int>, std::pair<int, int>>>> EdgeConstrStruct;
// структура для хранения путей (вектор пар i, j)
typedef std::vector<std::pair<int, int>> Path;

bool CompareAStar(const SearchNode& one, const SearchNode& two);
bool CompareDijkstra(const std::pair<double, SearchNode>& one, const std::pair<double, SearchNode>& two);

class Search {
    public:
        std::vector<SearchNode> partPath;
        Path fullPath;
        std::vector<std::vector<double>> dist;

        Search(Map& map);

        bool checkVertexConstr(int i, int j, int t, VertexConstrStruct& vertexConstr);
        bool checkEdgeConstr(int i1, int j1, int i2, int j2, int time, EdgeConstrStruct& edgeConstr);
        double computeHFromCellToCell(Map& map, int i1, int j1, int i2, int j2);
        void dijkstraPrecalc(Map& map, VertexConstrStruct& vertexConstr, EdgeConstrStruct& edgeConstr, Agent& agent);
        void startSearch(Map& map, VertexConstrStruct& vertexConstr, EdgeConstrStruct& edgeConstr, ConfMap& conflictAvoidanceTable, Agent& agent, std::vector<Path>& paths, bool useDijkstra, bool useFocal, double omega);
        std::list<SearchNode> findSuccessors(SearchNode& curNode, Map& map, VertexConstrStruct& vertexConstr, EdgeConstrStruct& edgeConstr, Agent& agent, bool dijkstra);
        void makePartPath(SearchNode curNode, SearchNode startNode);
        void makeFullPath();
};

#endif