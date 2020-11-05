//
// Created by Andrea Petreti on 05/11/2020.
//

#include "BooleanNetwork.h"
#include <cmath>
#include <utility/Utility.h>

BooleanNetwork::BooleanNetwork(int n, int k, float bias, int inputNode, int outputNode, bool selfLoops) :
    totalNodes(n),
    inputForNode(k),
    bias(bias),
    inputNode(inputNode),
    outputNode(outputNode),
    rnd(123) { // TODO: pass seed to constructor

    auto functionLength = pow(2, k);
    booleanFunctions = Matrix<bool>(totalNodes, functionLength);
    connectionMatrix = Matrix<int>();

    initialize();
}

void BooleanNetwork::initialize() {
    booleanFunctions = booleanFunctions.applyFunction([this](int row, int col, bool value) {
        return this->rnd.next() <= this->bias;
    });
    connectionMatrix = createRandomConnectionMatrix(totalNodes, inputForNode, false);
    states = fillVector(false, totalNodes);

    /* first N nodes are used for input */
    inputNodes = fillVectorByFunction<int>(inputNode, [](int index) { return index; });
    outputNodes = fillVectorByFunction<int>(outputNode, [this](int index) { return this->totalNodes - index - 1; });

    std::cout << booleanFunctions << std::endl;
    std::cout << connectionMatrix << std::endl;
    cout << states << endl;
    cout << "Input nodes " << inputNodes << endl;
    cout << "Output nodes " << outputNodes << endl;
}

void BooleanNetwork::forceInputValues(std::vector<bool> inputs) {
    if(inputs.size() > inputNode) { cerr << "Forcing too inputs than declared " << endl; return; }
    for(int i = 0; i < inputs.size(); i++) {
        this->states[inputNodes[i]] = inputs[i];
    }
}

Matrix<int> BooleanNetwork::createRandomConnectionMatrix(int totalNodes, int inputsForNode, bool selfLoop) {
    std::vector<int> collection(totalNodes);
    std::iota(collection.begin(), collection.end(), 0);

    Matrix<int> connectionMatrix(totalNodes, inputForNode);

    for(int i = 0; i < connectionMatrix.getRows(); i++) {
        auto connections = extractNodeInputIndexes(i, collection, inputsForNode, selfLoop);
        for(int j = 0; j < connections.size(); j++) connectionMatrix.put(i, j, connections[j]);
    }
    return connectionMatrix;
}

std::vector<int> BooleanNetwork::extractNodeInputIndexes(int nodeToLink, std::vector<int> nodeIndexes, int inputsForNode, bool selfLoop) {
    // remove from considered node the inputNode if the self loop are not admitted
    if(!selfLoop) nodeIndexes.erase(std::remove(nodeIndexes.begin(), nodeIndexes.end(), nodeToLink), nodeIndexes.end());
    return strongrandom::extractFromCollection(rnd, nodeIndexes, inputsForNode);
}

void BooleanNetwork::update() {
    auto oldStates = vector<bool>(states); // this allow to update synchronously
    for(int i = 0; i < totalNodes; i++) {
        states[i] = calculateNodeUpdate(i, oldStates);
    }
}

bool BooleanNetwork::calculateNodeUpdate(int nodeIndex, const vector<bool>& oldStates) {
    bool inputValues[connectionMatrix.getColumns()];
    for(int i = 0; i < connectionMatrix.getColumns(); i++) {
        inputValues[i] = oldStates[connectionMatrix(nodeIndex, i)];
    }

    int sum = 0;
    for(int i = 0, state = inputValues[i]; i < connectionMatrix.getColumns(); i++, state = inputValues[i]) {
        sum += boolToInt(state) * ((int) pow(2, i));
    }

    return booleanFunctions.get(nodeIndex, sum);
}

std::vector<bool> BooleanNetwork::getOutputValues() {
    std::vector<bool> output(outputNodes.size());
    for(int i = 0; i < output.size(); i++) {
        output[i] = states[outputNodes[i]];
    }
    return output;
}

void BooleanNetwork::changeBooleanFunction(const Matrix<bool>& newBooleanFunctions) {
    this->booleanFunctions = Matrix<bool>(newBooleanFunctions);
    cout << "Boolean function changed " << this->booleanFunctions << endl;
}

