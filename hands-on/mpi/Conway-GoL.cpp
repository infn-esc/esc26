/******************************************************************************
 *
 * MPI Conway's Game of Life
 *
 * Simplified Version::
 * - 1D per row decomposition
 * - ghost cells
 * - MPI_Sendrecv
 * - MPI_Gather
 * - MPI_Bcast
 *
 * How to compile:
 *   mpicxx -O2 -std=c++17 life_sendrecv.cpp -o life_sendrecv
 *
 * How to execute:
 *   mpirun -np 4 ./life_sendrecv 40 80 100
 *
 * Arguments:
 *   rows columns generations
 *
 *
 * Each process stores:
 *
 *      +-------------------+
 *      | ghost row (north) |
 *      +-------------------+
 *      |                   |
 *      | local rows        |
 *      |                   |
 *      +-------------------+
 *      | ghost row (south) |
 *      +-------------------+
 *
 * Ghost rows contain copies of boundary data coming from neighbouring
 * MPI processes and are updated at every iteration.
 ******************************************************************************/



/******************************************************************************
 * Conway's Game of Life rules:
 *
 * 1. Any live cell with fewer than two live neighbors dies
 *    (underpopulation).
 *
 * 2. Any live cell with two or three live neighbors survives
 *    to the next generation.
 *
 * 3. Any live cell with more than three live neighbors dies
 *    (overpopulation).
 *
 * 4. Any dead cell with exactly three live neighbors becomes
 *    a live cell (reproduction).
 *
 * All cells are updated simultaneously to produce the next
 * generation from the current generation.
 ******************************************************************************/

/******************************************************************************
 *
 * PARALLEL GAME OF LIFE WITH MPI
 *
 * ---------------------------------------------------------------------------
 * DOMAIN DECOMPOSITION
 * ---------------------------------------------------------------------------
 *
 * Most scientific simulations operate on data structures that are too large
 * to fit efficiently on a single processor. A common parallelization strategy
 * is called domain decomposition.
 *
 * The idea is simple: instead of assigning the entire computational domain
 * (the grid) to a single process, the domain is divided into smaller
 * subdomains, and each MPI process becomes responsible for one portion
 * of the data.
 *
 * Example:
 *
 * Global grid
 *
 *   +---------------------------------------------------+
 *   |                                                   |
 *   |                   Entire problem                  |
 *   |                                                   |
 *   +---------------------------------------------------+
 *
 * With four MPI processes:
 *
 *   Rank 0        Rank 1        Rank 2        Rank 3
 *
 *   +----+        +----+        +----+        +----+
 *   |    |        |    |        |    |        |    |
 *   |    |        |    |        |    |        |    |
 *   +----+        +----+        +----+        +----+
 *
 * In this example, the decomposition is one-dimensional:
 * the rows are distributed among MPI processes.
 *
 * ---------------------------------------------------------------------------
 * STENCIL COMPUTATIONS
 * ---------------------------------------------------------------------------
 *
 * The Game of Life is an example of a stencil computation.
 *
 * A stencil computation updates each grid point using information from a
 * fixed set of neighboring grid points.
 *
 * For Conway's Game of Life, each cell depends on its eight neighbors:
 *
 *   +---+---+---+
 *   | x | x | x |
 *   +---+---+---+
 *   | x | C | x |
 *   +---+---+---+
 *   | x | x | x |
 *   +---+---+---+
 *
 * where C is the current cell and x are the neighboring cells.
 *
 * Many scientific applications use stencil operations, including:
 *
 *   - Heat diffusion
 *   - Computational fluid dynamics (CFD)
 *   - Weather and climate models
 *   - Finite difference solvers
 *   - Image processing
 *
 * ---------------------------------------------------------------------------
 * GHOST CELLS (HALO CELLS)
 * ---------------------------------------------------------------------------
 *
 * A process can only directly access its local portion of the grid.
 *
 * However, cells near the boundary of a subdomain require information
 * owned by neighboring MPI processes.
 *
 * To solve this problem, additional rows (or columns) called ghost cells
 * or halo cells are added.
 *
 * Example:
 *
 *     ghost row (received from north neighbor)
 *                    |
 *                    v
 *
 *      +-----------------------------+
 *      |  ghost north row            |
 *      +-----------------------------+
 *      |  local row 1               |
 *      |  local row 2               |
 *      |  ...                       |
 *      |  local row N               |
 *      +-----------------------------+
 *      |  ghost south row            |
 *      +-----------------------------+
 *
 *                    ^
 *                    |
 *     ghost row (received from south neighbor)
 *
 * These ghost rows store copies of boundary data coming from neighboring
 * processes and allow local computations to proceed as if the entire grid
 * were available.
 *
 * ---------------------------------------------------------------------------
 * WHY COMMUNICATION IS REQUIRED EVERY ITERATION
 * ---------------------------------------------------------------------------
 *
 * The Game of Life is an iterative simulation.
 *
 * After every generation, cell states change.
 *
 * Consequently, the boundary rows owned by each process also change.
 *
 * Ghost cells therefore become out of date after every iteration and must
 * be refreshed before the next update step.
 *
 * Typical iteration:
 *
 *   1. Exchange halo rows
 *   2. Compute next generation
 *   3. Swap old and new grids
 *   4. Repeat
 *
 * This communication pattern is very common in stencil-based HPC
 * applications.
 *
 * ---------------------------------------------------------------------------
 * MPI_SENDRECV
 * ---------------------------------------------------------------------------
 *
 * This example uses MPI_Sendrecv() to exchange halo rows.
 *
 * MPI_Sendrecv() combines a send and a receive into a single operation.
 *
 * Example:
 *
 *   Rank i sends data to Rank i+1
 *   while simultaneously receiving data from Rank i-1
 *
 * Advantages:
 *
 *   - Simple to understand
 *   - Avoids many deadlock situations
 *   - Convenient for nearest-neighbor communication
 *
 * The communication is blocking: the call returns only after the exchange
 * has completed.
 *
 * ---------------------------------------------------------------------------
 * MPI_ISEND / MPI_IRECV
 * ---------------------------------------------------------------------------
 *
 * An alternative approach uses nonblocking communication:
 *
 *   MPI_Isend(...)
 *   MPI_Irecv(...)
 *
 * followed later by:
 *
 *   MPI_Wait(...)
 *
 * or:
 *
 *   MPI_Waitall(...)
 *
 * Advantages:
 *
 *   - Communication can be initiated early
 *   - Computation may proceed while data is in transit
 *   - Better scalability on large systems
 *
 * MPI_Sendrecv() is often easier to understand,
 * while MPI_Isend()/MPI_Irecv() are more commonly used in production
 * HPC applications.
 *
 * ---------------------------------------------------------------------------
 * COMMUNICATION / COMPUTATION OVERLAP
 * ---------------------------------------------------------------------------
 *
 * Communication/computation overlap is a technique used to improve
 * parallel efficiency.
 *
 * Instead of waiting idle for communication to finish, a process
 * performs useful work while messages are being transferred.
 *
 * Example:
 *
 *   MPI_Irecv(...)
 *   MPI_Isend(...)
 *
 *   Compute interior cells
 *   (cells that do not depend on incoming halo data)
 *
 *   MPI_Waitall(...)
 *
 *   Compute boundary cells
 *   (cells that require updated halo rows)
 *
 * Timeline:
 *
 *   Time ------------------------------------------------------------>
 *
 *   Communication:
 *   [==========================]
 *
 *   Computation:
 *        [==================]
 *
 *   The two activities execute concurrently instead of sequentially.
 *
 * This technique becomes increasingly important on modern HPC systems
 * where communication costs may limit scalability.
 *
 ******************************************************************************/

#include <mpi.h>

#include <vector>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

/******************************************************************************
 * Class representing a 2D grid stored in a linear array.
 *
 * Instead of allocating a true 2D array, we store all elements in a single
 * linear memory region. This approach is commonly used in HPC applications
 * because it improves memory locality and simplifies MPI communications.
 ******************************************************************************/
class Grid
{
public:
    int rows;
    int cols;

    std::vector<int> data;

    Grid(int r, int c)
        : rows(r), cols(c), data(r * c, 0)
    {}

    int& operator()(int i, int j)
    {
        return data[i * cols + j];
    }

    const int& operator()(int i, int j) const
    {
        return data[i * cols + j];
    }
};

/******************************************************************************
 * Randomly initialize the grid.
 ******************************************************************************/
void initialize(Grid& g)
{
    srand(42);

    for (int i = 0; i < g.rows; i++)
        for (int j = 0; j < g.cols; j++)
            g(i, j) = rand() % 2;
}

/******************************************************************************
 * Count the number of live neighbors.
 ******************************************************************************/
int countNeighbors(const Grid& g, int i, int j)
{
    int count = 0;

    for (int di = -1; di <= 1; di++)
    {
        for (int dj = -1; dj <= 1; dj++)
        {
            if (di == 0 && dj == 0)
                continue;

            count += g(i + di, j + dj);
        }
    }

    return count;
}

/******************************************************************************
 * Apply the Game of Life rules.
 ******************************************************************************/
void update(Grid& current, Grid& next, int startRow, int endRow)
{
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = 1; j < current.cols - 1; j++)
        {
            int neighbors = countNeighbors(current, i, j);

            if (current(i, j))
            {
                next(i, j) =
                    (neighbors == 2 || neighbors == 3);
            }
            else
            {
                next(i, j) =
                    (neighbors == 3);
            }
        }
    }
}

/******************************************************************************
 * Print the global grid.
 ******************************************************************************/
void printGlobal(const std::vector<int>& global,
                 int rows,
                 int cols,
                 int generation)
{
    std::cout << "\nGeneration " << generation << "\n\n";

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            std::cout << (global[i * cols + j] ? '#' : '.');
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int globalRows = 40;
    int globalCols = 80;
    int generations = 100;

    /**********************************************************************
     * Rank 0 reads the command-line arguments.
     **********************************************************************/
    if (rank == 0)
    {
        if (argc >= 4)
        {
            globalRows  = atoi(argv[1]);
            globalCols  = atoi(argv[2]);
            generations = atoi(argv[3]);
        }
    }

    /**********************************************************************
     * All ranks must know the simulation parameters.
     **********************************************************************/
    MPI_Bcast(&globalRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&globalCols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&generations, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (globalRows % size != 0)
    {
        if (rank == 0)
            std::cerr << "Rows must be divisible by number of processes\n";

        MPI_Finalize();
        return 1;
    }

    int localRows = globalRows / size;

    /**********************************************************************
     * Local grid:
     *
     * 0                 -> north ghost row
     * 1..localRows      -> real data
     * localRows+1       -> south ghost row
     **********************************************************************/
    Grid current(localRows + 2, globalCols + 2);
    Grid next(localRows + 2, globalCols + 2);

    if (rank == 0)
    {
        Grid global(globalRows, globalCols);

        initialize(global);

        /**************************************************************
         * Initial data distribution.
         **************************************************************/
        for (int p = 0; p < size; p++)
        {
            if (p == 0)
            {
                for (int i = 0; i < localRows; i++)
                    for (int j = 0; j < globalCols; j++)
                        current(i + 1, j + 1) =
                            global(i, j);
            }
            else
            {
                std::vector<int> tmp(localRows * globalCols);

                for (int i = 0; i < localRows; i++)
                    for (int j = 0; j < globalCols; j++)
                        tmp[i * globalCols + j]
                            = global(p * localRows + i, j);

                MPI_Send(tmp.data(),
                         tmp.size(),
                         MPI_INT,
                         p,
                         0,
                         MPI_COMM_WORLD);
            }
        }
    }
    else
    {
        std::vector<int> tmp(localRows * globalCols);

        MPI_Recv(tmp.data(),
                 tmp.size(),
                 MPI_INT,
                 0,
                 0,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        for (int i = 0; i < localRows; i++)
            for (int j = 0; j < globalCols; j++)
                current(i + 1, j + 1) =
                    tmp[i * globalCols + j];
    }

    int north = rank - 1;
    int south = rank + 1;

    if (north < 0) north = MPI_PROC_NULL;
    if (south >= size) south = MPI_PROC_NULL;
/**************************************************************************
* MPI_PROC_NULL is a special rank value defined by the MPI standard that represents a non-existent process.
*
* Its main purpose is to simplify communication at the boundaries of process topologies. Instead of writing
* special-case code for the first or last process, you can communicate with MPI_PROC_NULL and MPI will handle it gracefully
*****************************************************************************/

    for (int gen = 0; gen < generations; gen++)
    {
        /******************************************************************
         * Exchange upper and lower halo rows.
         * Exchange halo rows with neighbouring ranks.
         *
         * The first exchange sends our first real row to the north neighbour and
         * receives the south neighbour's boundary row into our south ghost row.
         *
         * MPI_Sendrecv is often the simplest and safest way to implement stencil
          * communications because it avoids many deadlock situations.
         ******************************************************************/

        MPI_Sendrecv(
            &current(1, 1),
            globalCols,
            MPI_INT,
            north,
            0,

            &current(localRows + 1, 1),
            globalCols,
            MPI_INT,
            south,
            0,

            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        MPI_Sendrecv(
            &current(localRows, 1),
            globalCols,
            MPI_INT,
            south,
            1,

            &current(0, 1),
            globalCols,
            MPI_INT,
            north,
            1,

            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        update(current, next, 1, localRows);

        std::swap(current.data, next.data);

        std::vector<int> sendbuf(localRows * globalCols);

        for (int i = 0; i < localRows; i++)
            for (int j = 0; j < globalCols; j++)
                sendbuf[i * globalCols + j]
                    = current(i + 1, j + 1);

        std::vector<int> recvbuf;

        if (rank == 0)
            recvbuf.resize(globalRows * globalCols);
/******************************************************************************
 * Collect all local subdomains on rank 0.
 *
 * MPI_Gather is a collective communication operation:
 * every process sends a buffer to the root process.
 *
 * Here we use it only for visualization purposes.
 ******************************************************************************/

        MPI_Gather(sendbuf.data(),
                   localRows * globalCols,
                   MPI_INT,
                   recvbuf.data(),
                   localRows * globalCols,
                   MPI_INT,
                   0,
                   MPI_COMM_WORLD);

        if (rank == 0 && gen % 10 == 0)
        {
            printGlobal(recvbuf,
                        globalRows,
                        globalCols,
                        gen);
        }
    }

    MPI_Finalize();

    return 0;
}
