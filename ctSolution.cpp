#include "map.h"
#include "agent.h"
#include "agents.h"
#include "searchNode.h"
#include "search.h"
#include "constr.h"
#include "ctNode.h"
#include "ctSolution.h"

#include <unordered_map>
#include <vector>
#include <tuple>
#include <iostream>

CTSolution::CTSolution(Map& map_1, Agents& agents_1, bool useDijkstraPrecalc_1, bool useCAT_1, std::string heuristic_1, bool prioritizeConflicts_1, bool useBypass_1, bool useFocal_1, double omega_1, bool printPaths_1) {
    map = map_1;
    agents = agents_1;
    useDijkstraPrecalc = useDijkstraPrecalc_1;
    useCAT = useCAT_1;
    heuristic = heuristic_1;
    prioritizeConflicts = prioritizeConflicts_1;
    useBypass = useBypass_1;
    useFocal = useFocal_1;
    omega = omega_1;
    printPaths = printPaths_1;
}

Path CTSolution::lowLevelSearch(CTNode node, int i) {
    Search search(map);

    if (useDijkstraPrecalc) {
        search.dijkstraPrecalc(map, node.vertexConstr, node.edgeConstr, agents.allAgents[i]);
    }

    search.startSearch(map, node.vertexConstr, node.edgeConstr, node.conflictAvoidanceTable, agents.allAgents[i], node.stateAgentMap, useDijkstraPrecalc, useFocal, omega);
    return search.fullPath;
}

std::vector<Path> CTSolution::highLevelSearch() {
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

            if (printPaths) {
                for (int agent = 0; agent < (best.paths).size(); ++agent) {
                    std::cout << "agent " << agent + 1 << ": [ ";
                    for (auto pr : best.paths[agent]) {
                        std::cout << "[" << pr.first << ", " << pr.second << "] ";
                    }
                    std::cout << "]\n";
                }
            }

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