#include "agent.h"
#include "agents.h"

#include <fstream>
#include <vector>

Agents::Agents() {
    allAgents.clear();
    size = 0;
}

void Agents::getAgents(std::ifstream& agentFile, int inputSize) {
    size = inputSize;
    std::string word;
    agentFile >> word >> word;

    for (int id = 0; id < size; ++id) {
        Agent agent(id);
        agent.getAgent(agentFile);
        allAgents.push_back(agent);
    }
}