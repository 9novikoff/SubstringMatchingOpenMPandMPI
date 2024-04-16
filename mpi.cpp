#include <stdio.h>
#include <mpi.h>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <iostream>

using namespace std;

#define MASTER 0

vector<int> substringMatchingMPI(const string& str, const string& pattern) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int strSize = str.size();
    int patternSize = pattern.size();
    vector<int> localRes;

    int chunkSize = strSize / size;
    int startIndex = rank * chunkSize;
    int endIndex = (rank == size - 1) ? strSize : (rank + 1) * chunkSize;

    endIndex = min(endIndex + patternSize - 1, strSize);

    for (int i = startIndex; i <= endIndex - patternSize; i++) {
        bool isMatch = true;
        for (int j = 0; j < patternSize; j++) {
            if (str[i + j] != pattern[j]) {
                isMatch = false;
                break;
            }
        }
        if (isMatch)
            localRes.push_back(i);
    }

    int localResSize = localRes.size();
    vector<int> recvCounts(size);
    vector<int> displs(size);
    MPI_Gather(&localResSize, 1, MPI_INT, recvCounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; ++i) {
            displs[i] = displs[i - 1] + recvCounts[i - 1];
        }
    }
    vector<int> res(strSize);
    MPI_Gatherv(localRes.data(), localResSize, MPI_INT, res.data(), recvCounts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        vector<int> finalRes;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < recvCounts[i]; ++j) {
                finalRes.push_back(res[displs[i] + j]);
            }
        }
        return finalRes;
    }
    else {
        return vector<int>();
    }
}

int main(int argc, char* argv[]) {
    int world_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


    ifstream file("C:/Users/Novikov/source/repos/SubstringMatching/input.txt");
    string str;
    string text;
    while (getline(file, str))
    {
        text += str;
        text.push_back('\n');
    }

    string pattern = "ROMEO";

    double start_time = MPI_Wtime();
    vector<int> result = substringMatchingMPI(text, pattern);
    double end_time = MPI_Wtime();

    if (world_rank == 0) {
        std::cout << "Time taken: " << end_time - start_time << " seconds" << std::endl;
        std::cout << result.size();
    }


    MPI_Finalize();
    return 0;
}