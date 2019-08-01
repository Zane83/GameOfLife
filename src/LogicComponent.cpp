#include "../include/LogicComponent.h"

LogicComponent::LogicComponent(){
    startMPI();
    createMatrices();
    fillInitialMatrix();
    loadGraphicComponent();
}

void LogicComponent::createMatrices(){
    
    //Inizializzo le celle tutte a "morte", per ogni processo
    for(int i = 0; i < DIMV; i++){
        for (int j = 0; j < DIMH; j++)
        {
            A[i][j] = 0;
            B[i][j] = 0;
        }
        
    }
}

void LogicComponent::fillInitialMatrix(){

        //Configurazione iniziale delle celle vive (ghianda)
        A[100][150] = 1;
        A[98][150] = 1;
        A[100][149] = 1;
        A[99][152] = 1;
        A[100][153] = 1;
        A[100][154] = 1;
        A[100][155] = 1;

}

void LogicComponent::checkCells(short F[DIMV][DIMH], short G[DIMV][DIMH]){
    //Ogni processo opera nella sua zona di competenza
    int start = (DIMV / num_cores) * my_rank;
    int end = start + (DIMV / num_cores);
    
    for(short i = start; i < end; i++)
    {
        for(short j = 0; j < DIMH; j++)
        {

            //Conto le celle vive vicine
            int alives = alive_neighbors(i, j, F);
            if(F[i][j] == 1){
                if(alives < 2)
                    G[i][j] = 0;
                else if(alives == 2 || alives == 3)
                    G[i][j] = 1;
                else if(alives > 3)
                    G[i][j] = 0;
            } else {
                if(alives == 3)
                    G[i][j] = 1;
                else
                    G[i][j] = 0;
            }

        }
    }

    //Se non sono con un solo processo...
    if(num_cores > 1){

        //Invio i risultati di ogni processo al processo master (con rank 0)
        if(my_rank != 0){
            for (int i = 0; i < DIMV / num_cores; i++){
                MPI_Send(&G[((DIMV / num_cores) * my_rank) + i], DIMH, MPI_SHORT, 0, 1, MPI_COMM_WORLD);
            }
        }

        //Il rank riceve i risultati riempendo la sua copia della matrice calcolata
        if(my_rank == 0){
            for (int i = 1; i < num_cores; i++){
                for (int j = 0; j < DIMV / num_cores; j++)
                {
                    MPI_Recv(&G[((DIMV / num_cores) * i) + j], DIMH, MPI_SHORT, i, 1, MPI_COMM_WORLD, &status);
                }
            }
        }

        //Invio delle ghost cells
        if(my_rank < num_cores - 1 && my_rank != 0){
            //Se il rank non è l'ultimo o il primo, quindi non ho la zona finale o iniziale
            if(my_rank == 1){

                //Se la zona è la seconda non faccio la send della prima riga perché il processo master ha già tutti i risultati del calcolo
                MPI_Recv(&G[((DIMV / num_cores) * my_rank) - 1], DIMH, MPI_SHORT, my_rank - 1, 2, MPI_COMM_WORLD, &status);
                MPI_Send(&G[((DIMV / num_cores) * my_rank) + (DIMV / num_cores) - 1], DIMH, MPI_SHORT, my_rank + 1, 2, MPI_COMM_WORLD);
                MPI_Recv(&G[((DIMV / num_cores) * my_rank) + (DIMV / num_cores)], DIMH, MPI_SHORT, my_rank + 1, 2, MPI_COMM_WORLD, &status);
            } else {

                //Se la zona non è la seconda (processo con rank maggiore a 1) invio le ghost cell sia sopra che sotto e ricevo da sopra e da sotto
                MPI_Recv(&G[((DIMV / num_cores) * my_rank) - 1], DIMH, MPI_SHORT, my_rank - 1, 2, MPI_COMM_WORLD, &status);
                MPI_Send(&G[((DIMV / num_cores) * my_rank)], DIMH, MPI_SHORT, my_rank - 1, 2, MPI_COMM_WORLD);
                MPI_Send(&G[((DIMV / num_cores) * my_rank) + (DIMV / num_cores) - 1], DIMH, MPI_SHORT, my_rank + 1, 2, MPI_COMM_WORLD);
                MPI_Recv(&G[((DIMV / num_cores) * my_rank) + (DIMV / num_cores)], DIMH, MPI_SHORT, my_rank + 1, 2, MPI_COMM_WORLD, &status);
            }
        } else if(my_rank == num_cores - 1){
            //Se invece mi trovo nell'ultima zona...

            if(my_rank != 1){
                //...e il rank non è 1 (quindi le zone sono più di 2) e quindi devo inviare le ghost cell alla zona superiore
                MPI_Recv(&G[((DIMV / num_cores) * my_rank) - 1], DIMH, MPI_SHORT, my_rank - 1, 2, MPI_COMM_WORLD, &status);
                MPI_Send(&G[((DIMV / num_cores) * my_rank)], DIMH, MPI_SHORT, my_rank - 1, 2, MPI_COMM_WORLD);
            } else {

                //in caso contrario non devo inviare nulla perché il processo master ha già tutti i risultati
                MPI_Recv(&G[((DIMV / num_cores) * my_rank) - 1], DIMH, MPI_SHORT, my_rank - 1, 2, MPI_COMM_WORLD, &status);
            }
        } else {
            //Se invece sono il processo master invio al processo successivo (zona inferiore) e non ricevo nulla perché già ricevuto
            MPI_Send(&G[((DIMV / num_cores) * my_rank) + (DIMV / num_cores) - 1], DIMH, MPI_SHORT, my_rank + 1, 2, MPI_COMM_WORLD);
        }
    }

}

void LogicComponent::startMPI(){

    //Inizizlizzo MPI...
    MPI_Init(NULL, NULL);

    //Salvo il rank del processo attuale
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //Salvo il numero di processi
    MPI_Comm_size(MPI_COMM_WORLD, &num_cores);

    /* DECOMMENTA SE VUOI PRENDERE I TEMPI */

    /*MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();*/
}

int LogicComponent::alive_neighbors(int i, int j, short G[DIMV][DIMH]){

    //Conta il numero di celle vive (con valore 1) cercando di non superare i limiti della matrice
    int count = 0;

    for(int _i = i - 1; _i <= i + 1; _i++){
        for(int _j = j - 1; _j <= j + 1; _j++){
            if(_i >= 0 && _i <= DIMV - 1 && _j >= 0 && _j <= DIMH - 1 && !(i == _i && j == _j)){
                if(G[_i][_j] == 1)
                    count++;
            }
        }
    }

    return count;
}

void LogicComponent::updateCells(){

    //Il loop che gestisce la transizione delle celle da uno stato all'altro
    for(int g = 0; !exit; g++)
    {
        if(g % 2 == 0)
        {
            checkCells(A, B);
            //Disegno a schermo
            if(my_rank == 0)
                gc->updateScene(B, DIMV, DIMH);

        } else {
        
            checkCells(B, A);
            //Disegno a schermo
            if(my_rank == 0)
                gc->updateScene(A, DIMV, DIMH);
        }

        //A fine iterazione controllo se il gestore eventi di Allegro (controllato soltanto dal rank 0) mi ha mandato un segnale di pressione del tasto "ESC"
        if(my_rank == 0){
            if(gc->isRunning())
            {   
                //Se così non è il rank 0 invia a tutti un segnale per far capire che devono continuare a processare
                exit = false;
                for (int i = 1; i < num_cores; i++)
                {
                    MPI_Send(&exit, 1, MPI_C_BOOL, i, 3, MPI_COMM_WORLD);
                }
            } else {

                //Oppure di fermarsi in caso contrario
                exit = true;
                for (int i = 1; i < num_cores; i++)
                {
                    MPI_Send(&exit, 1, MPI_C_BOOL, i, 3, MPI_COMM_WORLD);
                }
            }
        } else {

            //Gli altri processi ricevono il segnale
            MPI_Recv(&exit, 1, MPI_C_BOOL, 0, 3, MPI_COMM_WORLD, &status);
        }
    }


    /* DECOMMENTE SE VUOI PRENDERE I TEMPI */
    /*MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if(my_rank == 0){
        cout << end_time - start_time << endl;
    }*/

    //Chiudo MPI
    MPI_Finalize();
}

void LogicComponent::loadGraphicComponent(){

    //Carico la componente grafica sul processo di rank 0
    if(my_rank == 0)
        this->gc = new GraphicComponent();
}
