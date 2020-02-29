#ifndef SYSTEM_H
#define SYSTEM_H
#include <iostream>
#include <memory>
#include <unordered_map>
#include <fstream>

#include "parameterstorage.h"
#include "../lib/enhance.hpp"
#include "../lib/finiteElemente/finiteElemente.h"

#include <boost/multi_array.hpp>

#include <thread> 
#include <shared_mutex>


#include <chrono>
#include <ctime>


//(un)comment to (en/dis)able swap tracker
// #define SWAPTRACKER

class System
{
private:
    int acceptorNumber, hoppingSiteNumber, electrodeNumber;
    double * donorPositionsX, * donorPositionsY, * acceptorPositionsX, * acceptorPositionsY, * electrodePositionsX, * electrodePositionsY; // 1D
    double * energies; // 1D
    std::vector<bool> occupation; // 1D
    double * pairEnergies, * rates, * distances, * deltaEnergies; //2D
    
    int lastSwapped1=0,lastSwapped2=0; // swap 1->2 int = index

    double ratesSum=0;
    double constantRatesSumPart=0;
    double locLenA;
    shared_ptr<std::vector<double>> partRatesSumList; //list of accumulated rates for binary search

    std::shared_ptr<ParameterStorage> parameterStorage;
    

    void updateAfterSwap();

    bool readyForRun=false;
    bool storingMode; // if set true performance is optimized by storing known states
    bool ratesInMemory =false; //save if last step was found in stored states. if true, binary search is done to find swap

    #ifdef SWAPTRACKER
        ofstream swapTrackFile;
        int fileNumber=1;
    #endif


public:

    std::unique_ptr<FiniteElemente> finEle; //finEle device


    std::shared_ptr<std::shared_mutex> mutex;


    std::shared_ptr< std::unordered_map<std::vector<bool>,std::shared_ptr<std::vector<double>>>> konwnPartRatesSumList; //map of lists of accumulated rates for binary search, to store known states
    std::shared_ptr< std::unordered_map<std::vector<bool>,double>>  knownRatesSum;

    int * currentCounter; // 1D
    int * outputCurrentCounter;
    bool * storeKnownStates;

    double time=0;

    System(const std::shared_ptr<ParameterStorage> &);
    System(const System & oldSys);



    void createRandomNewDevice();
    void loadDevice();


    void initilizeMatrices();
    void getReadyForRun();

    void findSwap();
    void findSwapBS(); //using binary search

    void updateRatesMPStoring(); //multi  processor, storing mode
    void updateRatesSPStoring(); //single processor, storing mode
    void updateRates();// core calculation of rates
    

    void increaseTime();
    void reset();
    void run(int steps);
    
    void updatePotential(const std::vector<double> & voltages);
    void updatePotential(const mfem::GridFunction &  potential);

    mfem::GridFunction getPotential();
};




#endif // SYSTEM_H
