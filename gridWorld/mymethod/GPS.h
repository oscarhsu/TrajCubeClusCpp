#ifndef GPS_H
#define GPS_H

struct GPS{
	unsigned int ID;
	unsigned int user;
	unsigned int traj;
	int time;
	int x;
	int y;
	int dist;
	int angle;
};

#endif