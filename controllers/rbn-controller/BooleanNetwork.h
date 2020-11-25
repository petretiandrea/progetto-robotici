//
// Created by Andrea Petreti on 05/11/2020.
//

#ifndef SWARM_GEN_BOOLEANNETWORK_H
#define SWARM_GEN_BOOLEANNETWORK_H

#include <cmath>
#include <vector>
#include <utility/Matrix.h>
#include <utility/Utility.h>
#include <utility/NewRandom.h>

using namespace std;

class BooleanNetwork {

public:
    static std::function<bool()> booleanFunctionsInitializer(newrandom::Random* random, float bias) {
        return [random, bias]() -> bool { return random->next() <= bias; };
    }

    /**
     *
     * @param n
     * @param k
     * @param bias
     * @param inputNode
     * @param outputNode
     * @param seed
     * @param selfLoops
     */
    BooleanNetwork(int n, int k, float bias, int inputNode, int outputNode, int seed, bool selfLoops = false);

    /**
     *
     * @param connections
     * @param booleanFunctions
     * @param inputNode
     * @param outputNode
     */
    BooleanNetwork(Matrix<bool>& booleanFunctions, Matrix<int>& connections, int inputNode, int outputNode);

    /** Update the input nodes of network */
    void forceInputValues(std::vector<bool> inputs);

    /** Update the input node to specified value */
    void forceInputValue(int index, bool value);

    /** It compute one step of network. */
    void update();

    /** Get the values of output nodes. */
    vector<bool> getOutputValues();

    /** Set the boolean functions to specified value */
    void changeBooleanFunction(const Matrix<bool>& booleanFunctions);

    /** Restore the states of nodes to initial state (0) */
    void resetStates();

public:
    /**
     * Get the boolean functions of network as a matrix N x 2^K
     */
    inline const Matrix<bool>& getBooleanFunctions() { return booleanFunctions; }

    /**
     * Get the connections matrix of network as a matrix N x K
     */
    inline const Matrix<int>& getConnections() { return connectionMatrix; }

    /**
     * Get the nodes marked as input of network
     */
    inline const vector<int>& getInputNodes() { return inputNodes; }

    /**
     * Get the nodes marked as output of network
     */
    inline const vector<int>& getOutputNodes() { return outputNodes; }

    /**
     * Total number of nodes of network
     */
    inline int getNumberOfNodes() const { return connectionMatrix.getRows(); }

    /**
     * Get number of input for node (K)
     */
    inline int getInputForNode() const { return connectionMatrix.getColumns(); }

private:
    static Matrix<int> createRandomConnectionMatrix(newrandom::Random& rnd, int totalNodes, int inputsForNode, bool selfLoop);
    static vector<int> extractNodeInputIndexes(newrandom::Random& rnd, int nodeToLink, vector<int> nodeIndexes, int inputsForNode, bool selfLoop);

public:
    vector<bool> states;

private:
    Matrix<bool> booleanFunctions;
    Matrix<int> connectionMatrix;
    vector<int> inputNodes;
    vector<int> outputNodes;


    /** Initialize the network */
    void init(Matrix<bool>& booleanFunctions, Matrix<int>& connections, int inputNode, int outputNode);
    bool calculateNodeUpdate(int nodeIndex, const vector<bool>& oldStates);
};


#endif //SWARM_GEN_BOOLEANNETWORK_H
