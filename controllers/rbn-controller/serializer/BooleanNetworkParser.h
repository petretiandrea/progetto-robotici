//
// Created by Andrea Petreti on 20/11/2020.
//

#ifndef SWARM_GEN_BOOLEANNETWORKPARSER_H
#define SWARM_GEN_BOOLEANNETWORKPARSER_H

#include <controllers/rbn-controller/BooleanNetwork.h>
#include <utility/Matrix.h>
#include <utility/rapidcsv.h>
#include <vector>

/**
 *
 * n;k;input;output;booleanfunctions;connection
 *
 */

class BooleanNetworkParser {

public:
    static BooleanNetworkParser parser;
    void serialize(BooleanNetwork* network, const char* filename) {
        ofstream file;
        file.open(filename);

        file << network->getNumberOfNodes() << ";"
            << network->getInputForNode() << ";"
            << "\"" << serializeVector(network->getInputNodes()) << "\"" << ";"
            << "\"" << serializeVector(network->getOutputNodes()) << "\"" << ";"
            << "\"" << serializeMatrix(network->getBooleanFunctions()) << "\"" << ";"
            << "\"" << serializeMatrix(network->getConnections()) << "\"" << endl;

        file.close();
    }

    BooleanNetwork* deserialize(const char* filename) {

        rapidcsv::Document document(filename, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(';'));

        std::cout << "Reading " << document.GetRowCount() << " " << document.GetColumnCount() << endl;

        auto nodes = document.GetCell<int>(0, 0);
        auto inputForNode = document.GetCell<int>(1, 0);
        auto inputNodes = deserializeVector<int>(document.GetCell<string>(2, 0));
        auto outputNodes = deserializeVector<int>(document.GetCell<string>(3, 0));
        auto booleanFunctions = deserializeMatrix<bool>(document.GetCell<string>(4, 0));
        auto connections = deserializeMatrix<int>(document.GetCell<string>(5, 0));

        return new BooleanNetwork(booleanFunctions, connections, inputNodes.size(), outputNodes.size());
    }

private:
    template <typename T> std::string serializeMatrix(const Matrix<T>& matrix);
    template <typename T> std::string serializeVector(const std::vector<T>& vector);
    template <typename T> std::vector<T> deserializeVector(const string& stringVector);
    template <typename T> Matrix<T> deserializeMatrix(const string& stringMatrix);


    // for string delimiter
    static vector<string> split(string s, const string& delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        string token;
        vector<string> res;

        while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
            token = s.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back (token);
        }

        res.push_back (s.substr (pos_start));
        return res;
    }
};

BooleanNetworkParser BooleanNetworkParser::parser = BooleanNetworkParser();

template <typename T> std::string BooleanNetworkParser::serializeMatrix(const Matrix<T>& matrix) {
    ostringstream ss;
    ss << "[";
    for(int i = 0; i < matrix.getRows(); i++) {
        ss << "[ ";
        for(int j = 0; j < matrix.getColumns(); j++) {
            ss << matrix.get(i, j) << " ";
        }
        ss << "]";
    }
    ss << "]";
    return ss.str();
}

template <typename T> std::string BooleanNetworkParser::serializeVector(const std::vector<T>& vector) {
    ostringstream ss;
    ss << "[ ";
    for(int i = 0; i < vector.size(); i++) {
        ss << vector[i] << " ";
    }
    ss << "]";
    return ss.str();
};

template <typename T> std::vector<T> BooleanNetworkParser::deserializeVector(const string& stringVector) {
    std::vector<T> parsed;
    auto splitVector = split(stringVector, " ");

    for(int i = 1; i < splitVector.size() - 1; i++) {
        stringstream stream(splitVector[i]);
        T value;
        stream >> value;
        parsed.push_back(value);
    }

    return parsed;
}

template <typename T> Matrix<T> BooleanNetworkParser::deserializeMatrix(const string& stringMatrix) {
    std::vector<std::vector<T>> parsed;
    auto splitMatrix = split(stringMatrix, "[");
    // skip first two
    for(int i = 2; i < splitMatrix.size(); i++) {
        auto a = deserializeVector<T>("[" + splitMatrix[i]); // re-add "[" for parse vector
        parsed.push_back(a);
    }
    return Matrix<T>(parsed);
}

#endif //SWARM_GEN_BOOLEANNETWORKPARSER_H
