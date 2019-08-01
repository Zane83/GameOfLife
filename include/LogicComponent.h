#ifndef LOGIC_COMPONENT_H
#define LOGIC_COMPONENT_H
#define DIMV 1000
#define DIMH 1000
#include "mpi.h"
#include "GraphicComponent.h"
#include <iostream>
using namespace std;

class LogicComponent{

    private:
        short A[DIMV][DIMH];
        short B[DIMV][DIMH];
        int my_rank;
        int num_cores;
        MPI_Status status;
        GraphicComponent *gc;
        bool exit = false;
        double start_time, end_time;
        int x = 0;

        int alive_neighbors(int i, int j, short G[DIMV][DIMH]);

    public:
        LogicComponent();
        void createMatrices();
        void fillInitialMatrix();
        void checkCells(short F[DIMV][DIMH], short G[DIMV][DIMH]);
        void startMPI();
        void updateCells();
        void loadGraphicComponent();
};

#endif