//
// Created by Andrea Petreti on 05/11/2020.
//

#include "BooleanNetwork.h"
#include <fplus-fix.h>
// TODO: !!! Important the connection matrix is a full matrix, this implementation cannot handle
// a different input number for node

using namespace fplus;

BooleanNetwork::BooleanNetwork(int n, int k, float bias, int inputNode, int outputNode, int seed, bool selfLoops) {
    auto rnd = newrandom::Random(seed);
    auto functionLength = pow(2, k);
    auto initializer = BooleanNetwork::booleanFunctionsInitializer(&rnd, bias);

    auto boolFunctions = Matrix<bool>(n, functionLength).applyFunction([initializer](int row, int col, bool value) { return initializer(); });
    auto connections = createRandomConnectionMatrix(rnd, n, k, selfLoops);

    this->init(boolFunctions,
               connections,
               inputNode,
               outputNode);
}

BooleanNetwork::BooleanNetwork(Matrix<bool>& booleanFunctions, Matrix<int>& connections, int inputNode, int outputNode) {
    this->init(booleanFunctions, connections, inputNode, outputNode);
}

void BooleanNetwork::init(Matrix<bool>& booleanFunctions, Matrix<int>& connections, int inputNode, int outputNode) {
    this->connectionMatrix = connections;
    this->booleanFunctions = booleanFunctions;

    states = fwd::apply(numbers(0, getNumberOfNodes()), fwd::transform([](int i) { return false; }));
    /* first N nodes are used for input */
    inputNodes = fwd::apply(numbers(0, inputNode), fwd::identity());
    outputNodes = fwd::apply(numbers(0, outputNode), fwd::transform([this](int index) { return this->getNumberOfNodes() - index - 1; }));

    std::cout << booleanFunctions << std::endl;
    std::cout << connectionMatrix << std::endl;
    cout << states << endl;
    cout << "Input nodes " << inputNodes << endl;
    cout << "Output nodes " << outputNodes << endl;
}

void BooleanNetwork::forceInputValues(std::vector<bool> inputs) {
    if(inputs.size() > getInputNodes().size()) { cerr << "Forcing too inputs than declared " << endl; return; }
    for(int i = 0; i < inputs.size(); i++) {
        this->states[inputNodes[i]] = inputs[i];
    }
}

void BooleanNetwork::forceInputValue(int index, bool value) {
    if(index >= getInputNodes().size()) { cerr << "Index of input is out of range " << endl; return; }
    this->states[inputNodes[index]] = value;
}

void BooleanNetwork::update() {
    auto oldStates = vector<bool>(states); // this allow to update synchronously
    for(int i = 0; i < getNumberOfNodes(); i++) {
        states[i] = calculateNodeUpdate(i, oldStates);
    }
}

bool BooleanNetwork::calculateNodeUpdate(int nodeIndex, const vector<bool>& oldStates) {
    bool inputValues[connectionMatrix.getColumns()];
    for(int i = 0; i < connectionMatrix.getColumns(); i++) {
        inputValues[i] = oldStates[connectionMatrix(nodeIndex, i)];
    }

    int sum = 0;
    for(int i = 0; i < connectionMatrix.getColumns(); i++) {
        sum += utility::boolToInt(inputValues[i]) * ((int) pow(2, i));
    }

    return booleanFunctions.get(nodeIndex, sum);
}

std::vector<bool> BooleanNetwork::getOutputValues() {
    std::vector<bool> output(getOutputNodes().size());
    for(int i = 0; i < output.size(); i++) {
        output[i] = states[outputNodes[i]];
    }
    return output;
}

void BooleanNetwork::changeBooleanFunction(const Matrix<bool>& newBooleanFunctions) {
    this->booleanFunctions = Matrix<bool>(newBooleanFunctions);
//    cout << "Boolean function changed " << endl;
//    cout << this->booleanFunctions << endl;
}

Matrix<int> BooleanNetwork::createRandomConnectionMatrix(newrandom::Random& rnd, int totalNodes, int inputsForNode, bool selfLoop) {
    Matrix<int> connectionMatrix(totalNodes, inputsForNode);
    auto nodes = numbers(0, totalNodes);

    for(int i = 0; i < connectionMatrix.getRows(); i++) {
        auto connections = extractNodeInputIndexes(rnd, i, nodes, inputsForNode, selfLoop);
        for(int j = 0; j < connections.size(); j++) connectionMatrix.put(i, j, connections[j]);
    }
    return connectionMatrix;
}

std::vector<int> BooleanNetwork::extractNodeInputIndexes(newrandom::Random& rnd, int nodeToLink, std::vector<int> nodeIndexes, int inputsForNode, bool selfLoop) {
    // remove from considered node the inputNode if the self loop are not admitted
    if(!selfLoop) nodeIndexes.erase(std::remove(nodeIndexes.begin(), nodeIndexes.end(), nodeToLink), nodeIndexes.end());
    return utility::extractFromCollection(rnd, nodeIndexes, inputsForNode);
}


void BooleanNetwork::resetStates() {
    states = fwd::apply(numbers(0, getNumberOfNodes()), fwd::transform([](int i) { return false; }));
}

