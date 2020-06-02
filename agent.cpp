#include "agent.h"

#include <fstream>

Agent::Agent(int agentId) {
    start_i = -1;
    start_j = -1;
    fin_i = -1;
    fin_j = -1;
    agentId = agentId;
}

void Agent::getAgent(std::ifstream& agentFile) {
    std::string word;
    agentFile >> word >> word >> word >> word;
    agentFile >> start_j >> start_i >> fin_j >> fin_i >> word;
}