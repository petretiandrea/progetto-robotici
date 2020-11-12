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

/** Shared memory manager for data exchange between master and slaves */
class SharedMemory {

public:

    SharedMemory(const char* sharedMemoryFile,
                 int genomeSize,
                 int populationSize,
                 int slices);

    ~SharedMemory();

    GA1DBinaryStringGenome* GetGenome(int individual);

    void SetGenome(int individual,
                   const GA1DBinaryStringGenome& genome);

    float GetScore(int individual);

    void SetScore(int individual,
                  float score);

    void SetSlice(int runnerId, int startIndex, int size);
    int* GetSlice(int runnerId);

private:
    int slices;
    int genomeSize;
    int populationSize;
    int sharedMemFD;
    short* sharedMem;
    const char* sharedMemFile;
    size_t memorySize;
};


SharedMemory::SharedMemory(const char *sharedMemoryFile, int genomeSize, int populationSize, int slices) :
        sharedMemFile(sharedMemoryFile),
        genomeSize(genomeSize),
        populationSize(populationSize),
        slices(slices)
{
    sharedMemFD = ::shm_open(sharedMemoryFile,
                             O_RDWR | O_CREAT,
                             S_IRUSR | S_IWUSR);

    if(sharedMemFD < 0) {
        ::perror(sharedMemoryFile);
        exit(1);
    }

    /**
     * Genomes[] ...
     * Scores[] ...
     * Slices[] ...
     */
    memorySize = populationSize * (genomeSize * sizeof(short)) + // genomes
            populationSize * (genomeSize * sizeof(float)) + // scores
            slices * (2 * sizeof(int)); // slices

    ::ftruncate(sharedMemFD, memorySize);

    sharedMem = reinterpret_cast<short*>(::mmap(
            NULL,
            memorySize,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            sharedMemFD,
            0));

    if(sharedMem == MAP_FAILED) {
        ::perror("Shared memory error");
        exit(1);
    }

}


SharedMemory::~SharedMemory() {
    munmap(sharedMem, memorySize);
    close(sharedMemFD);
    shm_unlink(sharedMemFile);
}

GA1DBinaryStringGenome* SharedMemory::GetGenome(int individual) {
    auto* genome = new GA1DBinaryStringGenome(genomeSize);
    short* ptr = sharedMem + (individual * genomeSize);
    for(int i = 0; i < genome->size(); i++) {
        genome->gene(i, ptr[i]);
    }
    return genome;
}

void SharedMemory::SetGenome(int individual, const GA1DBinaryStringGenome& genome) {
    short* ptr = sharedMem + (individual * genomeSize);
    for(int i = 0; i < genome.size(); i++) {
        ptr[i] = genome.gene(i);
    }
}

float SharedMemory::GetScore(int individual) {
    float* ptr = (float*) (sharedMem + (populationSize * genomeSize));
    return ptr[individual];
}

void SharedMemory::SetScore(int individual, float score) {
    float* ptr = (float*) (sharedMem + (populationSize * genomeSize));
    ptr[individual] = score;
}

void SharedMemory::SetSlice(int runnerId, int startIndex, int size) {
    float* offsetGenomes = (float*) (sharedMem + (populationSize * genomeSize));
    int* ptr = (int*) (offsetGenomes + populationSize);
    int baseAddr = runnerId * slices;
    ptr[baseAddr] = startIndex;
    ptr[baseAddr + 1] = size;
}

int* SharedMemory::GetSlice(int runnerId) {
    float* offsetGenomes = (float*) (sharedMem + (populationSize * genomeSize));
    int* ptr = (int*) (offsetGenomes + populationSize);
    int baseAddr = runnerId * slices;

    int* out = new int[2];
    out[0] = ptr[baseAddr];
    out[1] = ptr[baseAddr + 1];
    return out;
}


#endif //SWARM_GEN_SHAREDMEMORY_H
