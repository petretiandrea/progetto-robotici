//
// Created by Andrea Petreti on 23/11/2020.
//

#include "SharedMemory.h"

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