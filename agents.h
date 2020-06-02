#ifndef AGENTS_H
#define AGENTS_H

#include <fstream>
#include <vector>

class Agents {
    public:
        int size;
        std::vector<Agent> allAgents;

        Agents();
        void getAgents(std::ifstream& agentFile, int inputSize);
};

#endif