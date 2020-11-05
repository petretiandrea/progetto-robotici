#include <iostream>
#include <controllers/rbn-controller/BooleanNetwork.h>
#include <utility/BetterRandom.h>
#include <utility/Utility.h>

int main() {

    BooleanNetwork network(10, 3, 0.1f, 3, 2);
    network.forceInputValues(std::vector<bool>(3, true));



    std::string a;

    do {
        network.update();
        std::cin >> a;
    } while (a != "c");

    return 0;
}
