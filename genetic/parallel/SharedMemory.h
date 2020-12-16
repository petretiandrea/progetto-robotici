//
// Created by Andrea Petreti on 09/11/2020.
//

#ifndef SWARM_GEN_SHAREDMEMORY_H
#define SWARM_GEN_SHAREDMEMORY_H

#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ga/ga.h>

/** Shared memory manager for data exchange between master and slaves */
class SharedMemory {

public:
    struct GenomeData {
        short* genome;
        int genomeSize;
        double fitness;
        double robotCount;
    };

public:

    SharedMemory(const char* sharedMemoryFile,
                 int genomeSize,
                 int populationSize,
                 int parallelProcess);

    ~SharedMemory();

    GenomeData& GetGenome(int individual);
    void SetGenome(int individual,
                   const GA1DBinaryStringGenome& genome,
                   double score,
                   double robotCount);
    void UpdateEvaluation(int individual, double score, double robotCount);
    void GetEvaluation(int individual, double* score, double* robotCount);

    void SetSlice(int runnerId, int startIndex, int size);
    int* GetSlice(int runnerId);

private:
    int parallelProcess;
    int genomeSize;
    int populationSize;
    int sharedMemFD;
    short* sharedMem;
    const char* sharedMemFile;
    size_t memorySize;
};


#endif //SWARM_GEN_SHAREDMEMORY_H
