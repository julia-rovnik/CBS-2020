#ifndef CTSOLUTION_H
#define CTSOLUTION_H

#include "search.h"
#include "ctNode.h"

#include <queue>
#include <vector>

struct CompareCBS {
    bool operator()(CTNode& one, CTNode& two) {
        return one.cost < two.cost;
    }
};

class CTSolution {
    public:
        std::priority_queue<CTNode, std::vector<CTNode>, CompareCBS> heap;
        Map map;
        Agents agents;
        bool useDijkstraPrecalc;
        bool useCAT;
        std::string heuristic;
        bool prioritizeConflicts;
        bool useBypass;
        bool useFocal;
        double omega;
        bool printPaths;

        CTSolution(Map& map, Agents& agents, bool useDijkstraPrecalc, bool useCAT, std::string heuristic, bool prioritizeConflicts, bool useBypass, bool useFocal, double omega, bool printPaths);
        
        Path lowLevelSearch(CTNode node, int i);
        std::vector<Path> highLevelSearch();
};

#endif