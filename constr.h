#ifndef CONSTR_H
#define CONSTR_H

struct Constraint {
    // vertex constraint: agent нельзя находиться в v1 во время time
    // edge constraint: agent нельзя находиться в v1 и пытаться перейти в v2 во время time
    std::string type;
    int agent;
    int time;
    std::pair<int, int> v1;
    std::pair<int, int> v2;

    Constraint(std::string type, int agent, int time, std::pair<int, int> v1, std::pair<int, int> v2)
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
    std::string type;
    std::pair<int, int> agents;
    int time;
    std::pair<int, int> v1;
    std::pair<int, int> v2;

    Conflict(std::string type, std::pair<int, int> agents, int time, std::pair<int, int> v1, std::pair<int, int> v2)
        : type(type)
        , agents(agents)
        , time(time)
        , v1(v1)
        , v2(v2)
    { }

    Conflict(std::string type)
        : type(type)
        , agents(std::pair<int, int>())
        , time(0)
        , v1(std::pair<int, int>())
        , v2(std::pair<int, int>())
    { }
};

#endif