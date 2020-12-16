//
// Created by andrea on 15/12/20.
//

#ifndef SWARM_GEN_PRINTABLE_H
#define SWARM_GEN_PRINTABLE_H

class Printable {
public:
    virtual std::ostream& print(std::ostream& os) = 0;
};

#endif //SWARM_GEN_PRINTABLE_H
