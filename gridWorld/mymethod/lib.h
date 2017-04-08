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
//#include <list>
#include <unordered_set>
#include <sys/types.h>
#include "dirent.h"
#include <errno.h>
//#include <omp.h>
#include "GPS.h"
#include <memory>

using namespace std;

struct gdist{
	unsigned int trajID;
	unsigned int gridID;
	unsigned int dist;
	gdist * next;
};

vector<vector<GPS>> readGPSfile(const char *, int, int);
int getdir(string dir, vector<string> &files);
int angleDiff(int a, int b);
gdist * getGridList(vector<GPS>);
int tagGridNum(GPS );
int getDist(int, int, int, int, int, int, int);
long long unsigned int getGridMethodResult(vector<gdist*>&);

const int sizeZ = TIME_TOP / T_EPSILON;
const unsigned int spaceSize = SIZE_X_OF_GRID * SIZE_Y_OF_GRID;
const unsigned int arrSize = SIZE_Z_OF_GRID * spaceSize;
 