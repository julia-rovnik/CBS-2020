#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <list>
#include <map>
#include <set>

using namespace std;

string breakingTies;
const double sqrtTwo = 1.41421356237;

typedef vector<set<pair<int, int>>> VertexConstrStruct;
typedef vector<set<pair<pair<int, int>, pair<int, int>>>> EdgeConstrStruct;
typedef vector<pair<int, int>> Path;

struct Map {
    int height;
    int width;
    vector<vector<int>> grid;

    Map()
        : height(0)
        , width(0)
        , grid(vector<vector<int>>())
    { }

    void getMapOptions(ifstream& mapFile) {
        string word;
        mapFile >> word >> word;
        mapFile >> word >> height;
        mapFile >> word >> width;
    }

    void getMapGrid(ifstream& mapFile) {
        string word;
        mapFile >> word;

        grid.resize(height, vector<int>(width, 0));
        for (int line = 0; line < height; ++line) {
            string curLine;
            mapFile >> curLine;
            for (int place = 0; place < width; ++place) {
                char letter = curLine[place];

                if (letter == '.' || letter == 'G') {
                    grid[line][place] = 0;
                } else {
                    grid[line][place] = 1;
                }
            }
        }
    }

    bool cellOnGrid(int i, int j) {
        return i >= 0 && i < height && j >= 0 && j < width;
    }

    bool cellIsTraversable(int i, int j) {
        return cellOnGrid(i, j) && grid[i][j] == 0;
    }
};

struct Options {
    string metricType, searchType, breakingTies;
    bool allowSqueeze, allowDiagonal, cutCorners;
    int hweight;

    Options()
        : metricType("manhattan")
        , searchType("astar")
        , hweight(1)
        , breakingTies("g-max")
        , allowSqueeze(false)
        , allowDiagonal(false)
        , cutCorners(false)
    { }
};

struct Agent {
    int start_i, start_j, fin_i, fin_j, agentId;

    Agent(int agentId)
        : start_i(-1)
        , start_j(-1)
        , fin_i(-1)
        , fin_j(-1)
        , agentId(agentId)
    { }

    void getAgent(ifstream& agentFile) {
        string word;
        agentFile >> word >> word >> word >> word;
        agentFile >> start_j >> start_i >> fin_j >> fin_i >> word;
    }
};

struct Agents {
    int size;
    vector<Agent> allAgents;

    Agents()
        : size(0)
        , allAgents(vector<Agent>())
    { }

    void getAgents(ifstream& agentFile, int inputSize) {
        size = inputSize;
        string word;
        agentFile >> word >> word;

        for (int id = 0; id < size; ++id) {
            Agent agent(id);
            agent.getAgent(agentFile);
            allAgents.push_back(agent);
        }
    }
};

struct SearchNode {
    int i, j, t;
    double f, g, h;
    SearchNode *parent;

    SearchNode(int i, int j)
        : i(i)
        , j(j)
        , t(0)
        , f(0)
        , g(0)
        , h(0)
        , parent(nullptr)
    { }

    bool operator< (const SearchNode &other) const {
        if (i == other.i) {
            return j < other.j;
        }
        return i < other.i;
    }

    bool operator== (const SearchNode &other) const {
        return i == other.i && j == other.j && t == other.t;
    }

    bool operator!= (const SearchNode &other) const {
        return !(*this == other);
    }
};

bool CompareAStar(const SearchNode& one, const SearchNode& two) {
    if (one.f == two.f) {
        if (one.g == two.g) {
            return one < two;
        } else {
            if (breakingTies == "g-min") {
                return one.g < two.g;
            }
            return one.g > two.g;
        }
    }
    return one.f < two.f;
}

bool CompareDijkstra (const pair<double, SearchNode>& one, const pair<double, SearchNode>& two) {
    return one.first < two.first;
}

struct Search {
    vector<SearchNode> partPath;
    Path fullPath;
    map<pair<int, int>, double> dist;
    Options options;

    Search(Options options)
        : partPath(vector<SearchNode>())
        , fullPath(Path())
        , dist(map<pair<int, int>, double>())
        , options(options)
    { }

    bool checkVertexConstr(int i, int j, int t, VertexConstrStruct& vertexConstr) {
        if (t < vertexConstr.size()) {
            auto result = vertexConstr[t].find({i, j});
            return result != vertexConstr[t].end();
        }
        return false;
    }

    bool checkEdgeConstr(int i1, int j1, int i2, int j2, int time, EdgeConstrStruct& edgeConstr) {
        if (time < edgeConstr.size()) {
            auto result = edgeConstr[time].find({{i1, j1}, {i2, j2}});
            return result != edgeConstr[time].end();
        }
        return false;
    }

    double computeHFromCellToCell(int i1, int j1, int i2, int j2, Options options) {
        if (options.metricType == "diagonal") {
            return (abs(abs(i2 - i1) - abs(j2 - j1)) + (min(abs(i2 - i1), abs(j2 - j1))) * sqrtTwo) * options.hweight;
        } else if (options.metricType == "manhattan") {
            return abs(i1 - i2) * options.hweight + abs(j1 - j2);
        } else if (options.metricType == "euclidean") {
            return sqrt((i1 - i2) * (i1 - i2) + (j1 - j2) * (j1 - j2)) * options.hweight;
        } else if (options.metricType == "chebyshev") {
            return max(abs(i1 - i2), abs(j1 - j2)) * options.hweight;
        } else {
            return 0;
        }
    }

    void dijkstraPrecalc(Map& map, Options& options, VertexConstrStruct& vertexConstr, EdgeConstrStruct& edgeConstr, Agent& agent) {
        SearchNode startNode(agent.start_i, agent.start_j);
        SearchNode finNode(agent.fin_i, agent.fin_j);

        bool pathFound = false;

        set<pair<double, SearchNode>, bool(*)(const pair<double, SearchNode>&, const pair<double, SearchNode>&)> heap(&CompareDijkstra);

        dist[{finNode.i, finNode.j}] = 0;
        heap.insert({dist[{finNode.i, finNode.j}], finNode});

        while (!pathFound && !heap.empty()) {
            auto best = heap.begin();
            pair<double, SearchNode> curBest = *best;
            heap.erase(curBest);

            double curDist = curBest.first;
            SearchNode curNode = curBest.second;

            if (curNode.i == startNode.i && curNode.j == startNode.j) {
                pathFound = true;
            } else {
                list<SearchNode> successors = findSuccessors(curNode, map, options, vertexConstr, edgeConstr, agent, true);
                for (SearchNode scNode : successors) {
                    double length = computeHFromCellToCell(scNode.i, scNode.j, curNode.i, curNode.j, options);

                    pair<int, int> curPair = {curNode.i, curNode.j};
                    pair<int, int> scPair = {scNode.i, scNode.j};

                    if (dist.find(scPair) != dist.end()) {
                        if (dist[curPair] + length < dist[scPair]) {
                            heap.erase({dist[scPair], scNode});
                            dist[scPair] = dist[curPair] + length;
                            heap.insert({dist[scPair], scNode});
                        }
                    } else {
                        dist[scPair] = dist[curPair] + length;
                        heap.insert({dist[scPair], scNode});
                    }
                }
            }
        }
    }

    void startSearch(Map& map, Options& options, VertexConstrStruct& vertexConstr, EdgeConstrStruct& edgeConstr, Agent& agent) {
        SearchNode startNode(agent.start_i, agent.start_j);
        SearchNode finNode(agent.fin_i, agent.fin_j);

        if (dist.find({startNode.i, startNode.j}) != dist.end()) {
            startNode.h = dist[{startNode.i, startNode.j}];
        } else {
            startNode.h = computeHFromCellToCell(agent.start_i, agent.start_j, agent.fin_i, agent.fin_j, options);
        }
        startNode.f = startNode.g + startNode.h;

        set<SearchNode, bool(*)(const SearchNode&, const SearchNode&)> open(&CompareAStar);
        set<SearchNode> openCopy;
        set<SearchNode> closed;
        list<SearchNode> parents;

        bool pathFound = false;

        open.insert(startNode);
        openCopy.insert(startNode);

        while (!pathFound && !open.empty()) {
            auto best = open.begin();
            SearchNode curNode = *best;

            if (curNode.i == finNode.i && curNode.j == finNode.j) {
                finNode = curNode;
                pathFound = true;
            } else {
                open.erase(curNode);
                openCopy.erase(curNode);
                closed.insert(curNode);

                parents.push_back(curNode);

                list<SearchNode> successors = findSuccessors(curNode, map, options, vertexConstr, edgeConstr, agent, false);
                for (SearchNode& scNode: successors) {
                    if (closed.find(scNode) == closed.end() && curNode.g + 1 <= scNode.g) {

                        scNode.g = curNode.g + 1;

                        if (dist.find({scNode.i, scNode.j}) != dist.end()) {
                            scNode.h = dist[{scNode.i, scNode.j}];
                        } else {
                            scNode.h = computeHFromCellToCell(scNode.i, scNode.j, agent.fin_i, agent.fin_j, options);
                        }

                        scNode.f = scNode.g + scNode.h;
                        scNode.t = curNode.t + 1;
                        scNode.parent = &parents.back();

                        auto check = openCopy.find(scNode);

                        if (check != openCopy.end() && (*check).f > scNode.f) {
                            open.erase(*check);
                            openCopy.erase(check);

                            open.insert(scNode);
                            openCopy.insert(scNode);
                        }

                        if (check == openCopy.end()) {
                            open.insert(scNode);
                            openCopy.insert(scNode);
                        }
                    }
                }
            }
        }

        if (pathFound) {
            makePartPath(finNode, startNode);
            makeFullPath();
        }
    }

    list<SearchNode> findSuccessors(SearchNode& curNode, Map& map, Options& options, VertexConstrStruct& vertexConstr, EdgeConstrStruct& edgeConstr, Agent& agent, bool dijkstra) {
        list<SearchNode> successors;
        int cur_i = curNode.i;
        int cur_j = curNode.j;

        // обходим все соседние клетки
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (!(i == 0 && j == 0) && map.cellOnGrid(cur_i + i, cur_j + j) && map.cellIsTraversable(cur_i + i, cur_j + j)) {

                    if (i != 0 && j != 0) {
                        if (options.allowDiagonal == false) {
                            continue;
                        }
                        if (options.cutCorners == false) {
                            if ((map.cellOnGrid(cur_i + i, cur_j) && !map.cellIsTraversable(cur_i + i, cur_j)) || (map.cellOnGrid(cur_i, cur_j + j) && !map.cellIsTraversable(cur_i, cur_j + j))) {
                                continue;
                            }
                        }
                        if (options.allowSqueeze == false) {
                            if ((map.cellOnGrid(cur_i + i, cur_j) && !map.cellIsTraversable(cur_i + i, cur_j)) && (map.cellOnGrid(cur_i, cur_j + j) && !map.cellIsTraversable(cur_i, cur_j + j))) {
                                continue;
                            }
                        }
                    }

                    // создаем "соседнее состояние", пересчитываем его g, h и f
                    SearchNode scNode(cur_i + i, cur_j + j);

                    scNode.g = curNode.g + 1;
                    scNode.h = computeHFromCellToCell(scNode.i, scNode.j, agent.fin_i, agent.fin_j, options);
                    scNode.f = scNode.g + scNode.h;
                    scNode.t = curNode.t + 1;

                    if (!dijkstra) {
                        if (!checkVertexConstr(scNode.i, scNode.j, scNode.t, vertexConstr)) {
                            if (!checkEdgeConstr(curNode.i, curNode.j, scNode.i, scNode.j, curNode.t, edgeConstr)) {
                                successors.push_back(scNode);
                            }
                        }
                    } else {
                        successors.push_back(scNode);
                    }
                }
            }
        }

        // создаем "соседнее состояние" wait
        SearchNode scNode(cur_i, cur_j);

        scNode.g = curNode.g + 1;
        scNode.h = curNode.h;
        scNode.f = scNode.g + scNode.h;
        scNode.t = curNode.t + 1;

        if (!dijkstra) {
            if (!checkVertexConstr(scNode.i, scNode.j, scNode.t, vertexConstr)) {
                if (!checkEdgeConstr(curNode.i, curNode.j, scNode.i, scNode.j, curNode.t, edgeConstr)) {
                    successors.push_back(scNode);
                }
            }
        } else {
            successors.push_back(scNode);
        }

        return successors;
    }

    void makePartPath(SearchNode curNode, SearchNode startNode) {
        partPath.clear();
        while (curNode.parent != nullptr) {
            partPath.push_back(curNode);
            curNode = *curNode.parent;
        }

        partPath.push_back(curNode);
        reverse(partPath.begin(), partPath.end());
    }

    void makeFullPath() {
        fullPath.clear();
        int ln = partPath.size();
        for (int node  = 0; node < ln; ++node) {
            if (node + 1 < ln) {
                for (int time = 0; time < partPath[node + 1].t - partPath[node].t; ++time) {
                    fullPath.push_back({partPath[node].i, partPath[node].j});
                }
            } else {
                fullPath.push_back({partPath[node].i, partPath[node].j});
            }
        }
    }
};


struct Constraint {
    // vertex constraint: agent нельзя находиться в v1 во время time
    // edge constraint: agent нельзя находиться в v1 и пытаться перейти в v2 во время time
    string type;
    int agent;
    int time;
    pair<int, int> v1;
    pair<int, int> v2;

    Constraint(string type, int agent, int time, pair<int, int> v1, pair<int, int> v2)
        : type(type)
        , agent(agent)
        , time(time)
        , v1(v1)
        , v2(v2)
    { }
};

struct Conflict {
    // vertex conflict: agent[0] и agent[1] пытались быть в v1 во время time
    // edge conflict: agent[0] пытался перейти из v1 в v2 во время time, agent2 пытался перейти из v2 в v1
    // none conflict
    string type;
    pair<int, int> agents;
    int time;
    pair<int, int> v1;
    pair<int, int> v2;

    Conflict(string type, pair<int, int> agents, int time, pair<int, int> v1, pair<int, int> v2)
        : type(type)
        , agents(agents)
        , time(time)
        , v1(v1)
        , v2(v2)
    { }

    Conflict(string type)
        : type(type)
        , agents(pair<int, int>())
        , time(0)
        , v1(pair<int, int>())
        , v2(pair<int, int>())
    { }
};

struct CTNode {
    vector<Path> paths;
    int cost;
    int maxTime;
    VertexConstrStruct vertexConstr;
    EdgeConstrStruct edgeConstr;

    CTNode()
        : paths(vector<Path>())
        , cost(0)
        , maxTime(10000)
        , vertexConstr(VertexConstrStruct(maxTime))
        , edgeConstr(EdgeConstrStruct(maxTime))
    { }

    Conflict validate() {
        int ln = paths.size();
        for (int i = 0; i < ln; ++i) { // первый агент
            for (int j = i + 1; j < ln; ++j) { // второй агент
                int steps = min(paths[i].size(), paths[j].size());
                for (int step = 0; step < steps; ++step) {
                    if (paths[i][step] == paths[j][step]) {
                        return Conflict("vertex", {i, j}, step, paths[i][step], pair<int, int>());
                    }
                    if (step + 1 < steps) {
                        if (paths[i][step] == paths[j][step + 1] && paths[i][step + 1] == paths[j][step]) {
                            return Conflict("edge", {i, j}, step, paths[i][step], paths[i][step + 1]);
                        }
                    }
                }
            }
        }
        return Conflict("none");
    }

    void countCost() {
        int ln = paths.size();
        for (int i = 0; i < ln; ++i) {
            for (int j = i + 1; j < ln; ++j) {
                int steps = min(paths[i].size(), paths[j].size());
                for (int step = 0; step < steps; ++step) {
                    if (paths[i][step] == paths[j][step]) {
                        ++cost;
                    }
                }
            }
        }
    }

    bool operator< (CTNode &other) {
        return cost < other.cost;
    }

    bool operator== (CTNode &other) {
        return cost == other.cost;
    }

    bool operator!= (CTNode &other) {
        return !(*this == other);
    }
};

struct CompareCBS {
    bool operator()(CTNode& one, CTNode& two) {
        return one.cost < two.cost;
    }
};

struct CTSolution {
    priority_queue<CTNode,vector<CTNode>, CompareCBS> heap;
    Map map;
    Options options;
    Agents agents;

    CTSolution(Map map, Options options, Agents agents)
        : heap(priority_queue<CTNode, vector<CTNode>, CompareCBS>())
        , map(map)
        , options(options)
        , agents(agents)
    { }

    Path lowLevelSearch(CTNode node, int i) {
        Search search(options);
        search.dijkstraPrecalc(map, options, node.vertexConstr, node.edgeConstr, agents.allAgents[i]);
        search.startSearch(map, options, node.vertexConstr, node.edgeConstr, agents.allAgents[i]);
        return search.fullPath;
    }

    vector<Path> highLevelSearch() {
        CTNode root;
        for (int i = 0; i < agents.size; ++i) {
            root.paths.push_back(lowLevelSearch(root, i));
        }
        root.countCost();

        heap.push(root);

        while (!heap.empty()) {
            CTNode best;
            best = heap.top();
            heap.pop();

            Conflict conflict = best.validate();
            if (conflict.type == "none") {
                return best.paths;
            }

            for (int i = 0; i < 2; ++i) {
                int agent;
                if (i == 0) {
                    agent = (conflict.agents).first;
                } else {
                    agent = (conflict.agents).second;
                }

                Constraint constraint(conflict.type, agent, conflict.time, conflict.v1, conflict.v2);

                CTNode node;
                node.vertexConstr = best.vertexConstr;
                node.edgeConstr = best.edgeConstr;

                if (conflict.type == "vertex") {
                    node.vertexConstr[conflict.time].insert(conflict.v1);
                } else if (conflict.type == "edge") {
                    pair<pair<int, int>, pair<int, int>> confTuple;
                    if (i == 0) {
                        confTuple = {{conflict.v1.first, conflict.v1.second}, {conflict.v2.first, conflict.v2.second}};
                    } else {
                        confTuple = {{conflict.v2.first, conflict.v2.second}, {conflict.v1.first, conflict.v1.second}};
                    }
                    node.edgeConstr[conflict.time].insert(confTuple);
                }

                node.paths = best.paths;
                node.paths[agent] = lowLevelSearch(node, agent);
                node.countCost();

                heap.push(node);
            }
        }
        return root.paths;
    }
};        

int main(int argc,  char **argv) {
    ifstream mapFile;
    mapFile.open(argv[1]);
    Map map;
    map.getMapOptions(mapFile);
    map.getMapGrid(mapFile);
    mapFile.close();

    Options options;
    breakingTies = options.breakingTies;

    ifstream agentFile;
    agentFile.open(argv[2]);
    Agents agents;
    agents.getAgents(agentFile, stoi(argv[3]));
    agentFile.close();

    CTSolution solution(map, options, agents);
    vector<Path> bestPaths = solution.highLevelSearch();

    /*for (int i = 0; i < bestPaths.size(); ++i) {
        for (int j = 0; j < bestPaths[i].size(); ++j) {
            cout << bestPaths[i][j].first << " " << bestPaths[i][j].second << "\n";
        }
        cout << "\n";
    }*/

    return 0;
}