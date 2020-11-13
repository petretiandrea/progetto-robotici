//
// Created by Andrea Petreti on 05/11/2020.
//

#include "BooleanNetwork.h"
#include <fplus-fix.h>
// TODO: !!! Important the connection matrix is a full matrix, this implementation cannot handle
// a different input number for node

using namespace fplus;

BooleanNetwork* BooleanNetwork::CreateFromParams(int n,
                                                 int k,
                                                 float bias,
                                                 int inputNode,
                                                 int outputNode,
                                                 newrandom::Random& rnd,
                                                 bool selfLoops) {
    auto functionLength = pow(2, k);
    auto initializer = BooleanNetwork::booleanFunctionsInitializer(&rnd, bias);
    auto booleanFunctions = Matrix<bool>(n, functionLength).applyFunction([initializer](int row, int col, bool value) {
        return initializer();
    });

    auto connections = createRandomConnectionMatrix(rnd, n, k, selfLoops);
    return new BooleanNetwork(connections, booleanFunctions, inputNode, outputNode);
}

BooleanNetwork::BooleanNetwork(Matrix<int>& connections, Matrix<bool>& booleanFunctions, int inputNode, int outputNode) :
    connectionMatrix(connections),
    booleanFunctions(booleanFunctions),
    totalNodes(connections.getRows()) {

    states = fwd::apply(numbers(0, totalNodes), fwd::transform([](int i) { return false; }));
    /* first N nodes are used for input */
    inputNodes = fwd::apply(numbers(0, inputNode), fwd::identity());
    outputNodes = fwd::apply(numbers(0, outputNode), fwd::transform([this](int index) { return this->totalNodes - index - 1; }));

    std::cout << booleanFunctions << std::endl;
    std::cout << connectionMatrix << std::endl;
    cout << states << endl;
    cout << "Input nodes " << inputNodes << endl;
    cout << "Output nodes " << outputNodes << endl;
}

void BooleanNetwork::forceInputValues(std::vector<bool> inputs) {
    if(inputs.size() > getInputNodes()) { cerr << "Forcing too inputs than declared " << endl; return; }
    for(int i = 0; i < inputs.size(); i++) {
        this->states[inputNodes[i]] = inputs[i];
    }
}

void BooleanNetwork::forceInputValue(int index, bool value) {
    if(index >= getInputNodes()) { cerr << "Index of input is out of range " << endl; return; }
    this->states[inputNodes[index]] = value;
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
    for(int i = 0; i < connectionMatrix.getColumns(); i++) {
        sum += utility::boolToInt(inputValues[i]) * ((int) pow(2, i));
    }

    return booleanFunctions.get(nodeIndex, sum);
}

std::vector<bool> BooleanNetwork::getOutputValues() {
    std::vector<bool> output(getOutputNodes());
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

int BooleanNetwork::getInputNodes() const {
    return inputNodes.size();
}

int BooleanNetwork::getOutputNodes() const {
    return outputNodes.size();
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

int BooleanNetwork::getFunctionLength() const {
    return booleanFunctions.getColumns();
}

void BooleanNetwork::resetStates() {
    states = fwd::apply(numbers(0, totalNodes), fwd::transform([](int i) { return false; }));
}

