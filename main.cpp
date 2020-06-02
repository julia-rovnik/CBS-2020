#include "map.h"
#include "agent.h"
#include "agents.h"
#include "searchNode.h"
#include "search.h"
#include "constr.h"
#include "ctNode.h"
#include "ctSolution.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <tuple>
#include <queue>

int main(int argc,  char **argv) {
    std::ifstream mapFile;
    mapFile.open(argv[1]);
    Map map;
    map.getMapOptions(mapFile);
    map.getMapGrid(mapFile);
    mapFile.close();

    std::ifstream agentFile;
    agentFile.open(argv[2]);
    Agents agents;
    agents.getAgents(agentFile, std::stoi(argv[3]));
    agentFile.close();

    std::vector<std::string> argList(argv + 1, argv + argc);
    
    bool readDijkstra = false;
    if (argList[3] == "true") {
        readDijkstra = true;
    }
    bool readCAT = false;
    if (argList[4] == "true") {
        readCAT = true;
    }
    bool readPC = false;
    if (argList[6] == "true") {
        readPC = true;
    }
    bool readBP = false;
    if (argList[7] == "true") {
        readBP = true;
    }
    bool readECBS = false;
    if (argList[8] == "true") {
        readECBS = true;
    }
    bool printPaths = false;
    if (argList[10] == "true") {
        printPaths = true;
    }

    CTSolution solution(map, agents, readDijkstra, readCAT, argList[5], readPC, readBP, readECBS, std::stof(argList[9]), printPaths);

    // map - argList[0]
    // scen - argList[1]
    // vector of agents
    // true -> use dijkstra precalc - argList[3]
    // true -> use CAT - argList[4]
    // heuristic (normal, number_of_conflicts, number_of_conflicting_agents, number_of_pairs, vertex_cover) - argList[5]
    // true -> prioritize conflicts - argList[6]
    // true -> use bypass - argList[7]
    // true -> use ecbs - argList[8]
    // omega - argList[9]
    // print paths - argList[10]

    std::vector<Path> bestPaths = solution.highLevelSearch();

    return 0;
}