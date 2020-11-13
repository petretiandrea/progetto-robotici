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
     * @param booleanFunctionsInitializer
     * @param inputNode
     * @param outputNode
     * @param rnd
     * @param selfLoops
     * @return
     */
    static BooleanNetwork* CreateFromParams(int n,
                                            int k,
                                            float bias,
                                            int inputNode,
                                            int outputNode,
                                            newrandom::Random& rnd,
                                            bool selfLoops = false);

    /**
     *
     * @param connections
     * @param booleanFunctions
     * @param inputNode
     * @param outputNode
     */
    BooleanNetwork(Matrix<int>& connections, Matrix<bool>& booleanFunctions, int inputNode, int outputNode);

    void forceInputValues(std::vector<bool> inputs);
    void forceInputValue(int index, bool value);
    void update();
    vector<bool> getOutputValues();

    int getFunctionLength() const;
    int getOutputNodes() const;
    int getInputNodes() const;

    void changeBooleanFunction(const Matrix<bool>& booleanFunctions);
    void resetStates();

private:
    static Matrix<int> createRandomConnectionMatrix(newrandom::Random& rnd, int totalNodes, int inputsForNode, bool selfLoop);
    static vector<int> extractNodeInputIndexes(newrandom::Random& rnd, int nodeToLink, vector<int> nodeIndexes, int inputsForNode, bool selfLoop);

private:
    int totalNodes;
    Matrix<bool> booleanFunctions;
    Matrix<int> connectionMatrix;
    vector<int> inputNodes;
    vector<int> outputNodes;
    vector<bool> states;

    bool calculateNodeUpdate(int nodeIndex, const vector<bool>& oldStates);
};


#endif //SWARM_GEN_BOOLEANNETWORK_H
