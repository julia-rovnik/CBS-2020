#include "map.h"
#include "agent.h"
#include "agents.h"
#include "searchNode.h"
#include "search.h"
#include "constr.h"
#include "ctNode.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <tuple>
#include <queue>

struct CompareCBS {
    bool operator()(CTNode& one, CTNode& two) {
        return one.cost < two.cost;
    }
};

struct CTSolution {
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

    CTSolution(Map& map, Agents& agents, bool useDijkstraPrecalc, bool useCAT, std::string heuristic, bool prioritizeConflicts, bool useBypass, bool useFocal, double omega)
        : heap(std::priority_queue<CTNode, std::vector<CTNode>, CompareCBS>())
        , map(map)
        , agents(agents)
        , useDijkstraPrecalc(useDijkstraPrecalc)
        , useCAT(useCAT)
        , heuristic(heuristic)
        , prioritizeConflicts(prioritizeConflicts)
        , useBypass(useBypass)
        , useFocal(useFocal)
        , omega(omega)
    { }

    Path lowLevelSearch(CTNode node, int i) {
        Search search(map);

        if (useDijkstraPrecalc) {
            search.dijkstraPrecalc(map, node.vertexConstr, node.edgeConstr, agents.allAgents[i]);
        }

        search.startSearch(map, node.vertexConstr, node.edgeConstr, node.conflictAvoidanceTable, agents.allAgents[i], node.paths, useDijkstraPrecalc, useFocal, omega);
        return search.fullPath;
    }

    std::vector<Path> highLevelSearch() {
        // первый раз находим пути для каждого агента, не обращаем внимания на возможные конфликты
        CTNode root;
        for (int i = 0; i < agents.size; ++i) {
            root.paths.push_back(lowLevelSearch(root, i));
        }
        root.countCost(heuristic);

        if (useCAT) {
            root.countCAT();
        }

        heap.push(root);

        while (!heap.empty()) {
            CTNode best;
            best = heap.top();
            heap.pop();

            Conflict conflict = best.findBestConflict(map, agents, prioritizeConflicts);
            if (conflict.type == "none") {
                return best.paths;
            }

            // в конфликте участвует два агента, i = 0 означает первого агента, i = 1 второго агента
            for (int i = 0; i < 2; ++i) {
                int agent;
                if (i == 0) {
                    agent = (conflict.agents).first;
                } else {
                    agent = (conflict.agents).second;
                }

                Constraint constraint(conflict.type, agent, conflict.time, conflict.v1, conflict.v2);

                // заполняем поля вершины с новой constraint
                CTNode node;
                node.vertexConstr = best.vertexConstr;
                node.edgeConstr = best.edgeConstr;

                if (conflict.type == "vertex") {
                    node.vertexConstr[conflict.time].insert(conflict.v1);
                } else if (conflict.type == "edge") {
                    std::pair<std::pair<int, int>, std::pair<int, int>> confTuple;
                    if (i == 0) {
                        confTuple = {{conflict.v1.first, conflict.v1.second}, {conflict.v2.first, conflict.v2.second}};
                    } else {
                        confTuple = {{conflict.v2.first, conflict.v2.second}, {conflict.v1.first, conflict.v1.second}};
                    }
                    node.edgeConstr[conflict.time].insert(confTuple);
                }

                node.paths = best.paths;

                if (useCAT) {
                    node.countCAT();
                }

                node.paths[agent] = lowLevelSearch(node, agent);
                node.countCost(heuristic);

                heap.push(node);
            }
        }
        return root.paths;
    }
};        

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

    CTSolution solution(map, agents, readDijkstra, readCAT, argList[5], readPC, readBP, readECBS, std::stof(argList[9]));

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

    std::vector<Path> bestPaths = solution.highLevelSearch();

    return 0;
}