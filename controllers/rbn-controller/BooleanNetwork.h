//
// Created by Andrea Petreti on 05/11/2020.
//

#ifndef SWARM_GEN_BOOLEANNETWORK_H
#define SWARM_GEN_BOOLEANNETWORK_H

#include <vector>
#include <utility/Matrix.h>
#include <utility/BetterRandom.h>

using namespace std;

class BooleanNetwork {

public:
    /**
     *
     * @param n Total number of node
     * @param k Number of input for each node
     * @param bias
     * @param inputNode Number of input node
     * @param outputNode Number of output node
     * @param selfLoops Allow self loops
     */
    BooleanNetwork(int n, int k, float bias, int inputNode, int outputNode, bool selfLoops = false);

    void forceInputValues(std::vector<bool> inputs);
    void update();
    vector<bool> getOutputValues();

    void changeBooleanFunction(const Matrix<bool>& booleanFunctions);

private:
    void initialize();
    Matrix<int> createRandomConnectionMatrix(int totalNodes, int inputsForNode, bool selfLoop);
    vector<int> extractNodeInputIndexes(int nodeToLink, vector<int> nodeIndexes, int inputsForNode, bool selfLoop);

private:
    int totalNodes;
    int inputForNode;
    float bias;
    int inputNode;
    int outputNode;
    Matrix<bool> booleanFunctions;
    Matrix<int> connectionMatrix;
    vector<int> inputNodes;
    vector<int> outputNodes;
    vector<bool> states;
    strongrandom::Random rnd;

    bool calculateNodeUpdate(int nodeIndex, const vector<bool>& oldStates);
};

#endif //SWARM_GEN_BOOLEANNETWORK_H
