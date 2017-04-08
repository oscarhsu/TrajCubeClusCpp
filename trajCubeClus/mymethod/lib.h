#include "constant.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> 
#include <cmath>
#include <math.h>
#include <time.h>
#include <exception>
#include <string>
#include <windows.h>
#include <vector>
#include <set>
#include <sys/types.h>
#include "dirent.h"
#include <errno.h>
//#include <omp.h>
#include "GPS.h"

using namespace std;

vector<vector<GPS>> readGPSfile(const char *, int, int);
int getdir(string dir, vector<string> &files);
int angleDiff(int a, int b);
vector<GPS> cubelize(vector<GPS>, int, int, int);
long long unsigned int pairwise(vector<GPS>, vector<GPS>, int, int, int);
GPS createNewCube(int, int, int, int, int, int, int ,int ,int, int, int, int, int, int, int);
int getDist(int, int, int, int, int, int, int);