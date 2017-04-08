#define _CRT_SECURE_NO_DEPRECATE

//The geographic size of cube. 
#define METERS 100 //geographic size in meters
#define DISTX 117 //geographic size in longitude, times 100000.
#define DISTY 90 // geographic size in latitude, times 100000.

//#define METERS 50
//#define DISTX 59
//#define DISTY 45

//#define METERS 24
//#define DISTX 28
//#define DISTY 22

//The temporal size of cube in seconds.
#define T_EPSILON 3600
//#define T_EPSILON 1800
//#define T_EPSILON 600

//The max angle difference of two cubes.
#define A_EPSILON 30
//#define A_EPSILON 45

#define M_PI 3.14159265358979323846 
#define TRY_NUM 794
#define LNG_BOTTOM 11609194
#define LAT_BOTTOM 3968840
#define LNG_TOP 11671474
#define LAT_TOP 4017963
#define TIME_TOP 86400
#define SIZE_X_OF_GRID ((LNG_TOP - LNG_BOTTOM) / DISTX + 2)
#define SIZE_Y_OF_GRID ((LAT_TOP - LAT_BOTTOM) / DISTY + 2)
#define SIZE_Z_OF_GRID (TIME_TOP / T_EPSILON)

