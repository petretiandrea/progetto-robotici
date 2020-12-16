//
// Created by Andrea Petreti on 23/11/2020.
//

#include "SharedMemory.h"

SharedMemory::SharedMemory(const char *sharedMemoryFile, int genomeSize, int populationSize, int parallelProcess) :
        sharedMemFile(sharedMemoryFile),
        genomeSize(genomeSize),
        populationSize(populationSize),
        parallelProcess(parallelProcess)
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
     // total genome required memory
    auto genomeMemorySize = populationSize * (genomeSize * sizeof(short));
    auto structMemorySize = populationSize * (sizeof(GenomeData));
    auto parallelProcessMemory = parallelProcess * (2 * sizeof(int));

    memorySize = genomeMemorySize + structMemorySize + parallelProcessMemory;

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

SharedMemory::GenomeData& SharedMemory::GetGenome(int individual) {
    auto* structs = (GenomeData*) sharedMem;
    return structs[individual];
}

void SharedMemory::UpdateEvaluation(int individual, double score, double robotCount) {
    auto* structs = (GenomeData*) sharedMem;

    structs[individual].fitness = score;
    structs[individual].robotCount = robotCount;
}

void SharedMemory::GetEvaluation(int individual, double *score, double *robotCount) {
    auto* structs = (GenomeData*) sharedMem;

    *score = structs[individual].fitness;
    *robotCount = structs[individual].robotCount;
}


void printData(SharedMemory::GenomeData& data) {
    std::cout << "Size " << data.genomeSize << " Fit " << data.fitness << " count " << data.robotCount;
    std::cout << " genome: ";
    for(int i = 0; i < data.genomeSize; i++) {
        std::cout << data.genome[i];
    }
    std::cout << std::endl;
}

void SharedMemory::SetGenome(int individual, const GA1DBinaryStringGenome& genome, double score, double robotCount) {
    auto* structs = (GenomeData*) sharedMem;
    auto* genomes = (short*) (structs + populationSize);
    short* genomePtr = genomes + (individual * genomeSize);

    structs[individual].genome = genomePtr;
    structs[individual].fitness = score;
    structs[individual].robotCount = robotCount;
    structs[individual].genomeSize = genomeSize;

    for(int i = 0; i < genome.size(); i++) {
        genomePtr[i] = genome.gene(i);
    }
    //printData(structs[individual]);
}

void SharedMemory::SetSlice(int runnerId, int startIndex, int size) {
    auto* structs = (GenomeData*) sharedMem;
    auto* genomes = (short*) (structs + populationSize);
    auto* sliceSpace = (int*) (genomes + (populationSize * genomeSize));
    int index = runnerId * 2;

    sliceSpace[index] = startIndex;
    sliceSpace[index + 1] = size;
}

int* SharedMemory::GetSlice(int runnerId) {
    auto* structs = (GenomeData*) sharedMem;
    auto* genomes = (short*) (structs + populationSize);
    auto* sliceSpace = (int*) (genomes + (populationSize * genomeSize));
    int index = runnerId * 2;

    int* out = new int[2];
    out[0] = sliceSpace[index];
    out[1] = sliceSpace[index + 1];
    return out;
}
