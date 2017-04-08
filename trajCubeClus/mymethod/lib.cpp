#include "lib.h"

int getdir(string dir, vector<string> &files){

	DIR *dp;//創立資料夾指標
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL){
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}
	while ((dirp = readdir(dp)) != NULL){//如果dirent指標非空

		if (dirp->d_type == DT_REG)
		{
			files.push_back(string(dirp->d_name));//將資料夾和檔案名放入vector
		}
	}
	closedir(dp);//關閉資料夾指標
	return 0;
}

vector<vector<GPS>> readGPSfile(const char * filename, int user, int traj)
{
	vector<vector<GPS>> trajlist;
	vector<GPS> row;
	fstream file;
	char buffer[100];
	double x, y, datetime = 0, preDatetime = -1;
	unsigned int count = 0;
	GPS tempPoint;
	string dt;
	char dtc[50];
	size_t nPos;
	int userNo = user;
	int trajNo = traj;
	int iDateTime = 0;
	int iPreDateTime = -1;
	bool flag = false;

	file.open(filename, ios::in);
	if (!file)
	{
		throw "Can't open file!\n";
	}

	for (int i = 0; i < 6; i++)
		file.getline(buffer, sizeof(buffer));

	do
	{
		file.getline(buffer, sizeof(buffer));
		sscanf(buffer, "%lf,%lf,%*d,%*lf,%s", &y, &x, dtc);

		//ignore the GPS that out of Beijing
		if (x<116.091945 || y <39.688403 || x>116.714733 || y>40.179632)
			continue;
		dt = dtc;
		nPos = dt.find(',');
		dt = dt.substr(0, nPos);
		nPos = dt.find('.');
		if (nPos == dt.npos)
		{
			datetime = 0;
		}
		else
		{
			dt = dt.substr(nPos);
			datetime = atof(dt.c_str());
		}
		datetime += 8.0 / 24.0;
		iDateTime = (int)datetime;


		datetime = datetime - iDateTime;
		datetime = datetime * 24 * 60 * 60;


		tempPoint.x = unsigned int(x * 100000 + 0.5);
		tempPoint.y = unsigned int(y * 100000 + 0.5);

		tempPoint.time = (int)floor(datetime + 0.5);
		tempPoint.traj = trajNo;
		tempPoint.user = userNo;

		if (tempPoint.time < iPreDateTime)
		{
			count = 0;
			trajlist.push_back(row);
			row.clear();
		}
		iPreDateTime = tempPoint.time;

		count++;
		tempPoint.ID = count;
		row.push_back(tempPoint);
	} while (!file.eof());

	file.close();
	if (row.size() > 0)
		trajlist.push_back(row);

	return trajlist;
}

long long unsigned int pairwise(vector<GPS> targetTraj, vector<GPS> allTraj, int iDistX, int iDistY, int iTeps)
{
	int sizeTargetTraj = targetTraj.size();
	int endTargetTraj = sizeTargetTraj - 1;
	int maxTargetCenterTime = targetTraj[endTargetTraj].time;
	int minTargetCenterTime = targetTraj[0].time;
	int iTargetTraj = 0;

	int sizeAllTraj = allTraj.size();
	int endAllTraj = sizeAllTraj - 1;
	int maxAllCenterTime = allTraj[endAllTraj].time;
	int minAllCenterTime = allTraj[0].time;
	int iAllTraj = 0;
	long long unsigned int dist = 0;
	set<int> vecTargetInd;
	set<int> vecAllInd;
	set<int>::iterator si;

	////---------
	////check if the time of allTraj is all above targetTraj
	////----------
	int diffMaxTargetMinAll = minAllCenterTime - maxTargetCenterTime;
	if (diffMaxTargetMinAll > iTeps)
	{
			return 0;
	}

	////--------
	////check if the time of targetTraj is all above allTraj
	////--------
	int diffMinTargetMaxAll = minTargetCenterTime - maxAllCenterTime;
	if (diffMinTargetMaxAll > iTeps)
	{
			return 0;
	}
	//-------------------

	int diffTime = 0;
	iTargetTraj = 0;
	iAllTraj = 0;
	while(true)
	{

		diffTime = allTraj[iAllTraj].time - targetTraj[iTargetTraj].time;

		if (diffTime >= iTeps)
		{
			if (iTargetTraj >= endTargetTraj)
				break;
			else
				iTargetTraj++;
		}
		else if (diffTime <= -iTeps)
		{
			if (iAllTraj >= endAllTraj)
				break;
			else
				iAllTraj++;
		}
		else
		{
			int iTargetTraj2 = iTargetTraj;
			while (iTargetTraj2 <= endTargetTraj && abs(targetTraj[iTargetTraj2].time - allTraj[iAllTraj].time) < iTeps)
			{
				if (abs(targetTraj[iTargetTraj2].x - allTraj[iAllTraj].x) < iDistX
					&& abs(targetTraj[iTargetTraj2].y - allTraj[iAllTraj].y) < iDistY)
				{
					vecAllInd.insert(iAllTraj);
					vecTargetInd.insert(iTargetTraj2);
				}
				iTargetTraj2++;
			}
			if (iAllTraj >= endAllTraj)
				break;
			else
				iAllTraj++;
		}
	}

	for(si = vecTargetInd.begin(); si != vecTargetInd.end(); si++)
	{
		dist += targetTraj[*si].dist;
	}
	for(si = vecAllInd.begin(); si != vecAllInd.end(); si++)
	{
		dist += allTraj[*si].dist;
	}
	return dist;
}

GPS createNewCube(int cubeIndex, int minX, int maxX, int minY,int maxY, int minTime, int maxTime,int userNo,int trajNo,
				  int endPointX , int startPointX, int endPointY , int startPointY, int iDistX, int iDistY)
{
			GPS tempCube;
			tempCube.ID = cubeIndex;
			tempCube.x = (minX + maxX) / 2;
			tempCube.y = (minY + maxY) / 2;
			tempCube.time = (minTime + maxTime) / 2;
			tempCube.user = userNo;
			tempCube.traj = trajNo;

			int dx = endPointX - startPointX;
			int dy = endPointY - startPointY;
			tempCube.angle = int(atan2(dy, dx) * 180 / M_PI + 0.5);
			tempCube.dist = getDist(startPointX, startPointY, endPointX, endPointY, iDistX, iDistY, METERS);

			return tempCube;
}

vector<GPS> cubelize(vector<GPS> traj, int iDistX, int iDistY, int iTeps)
{
	//if across day, divide into two traj
	vector<GPS> cubelist;
	int userNo = traj[0].user;
	int trajNo = traj[0].traj;
	int cubeIndex = 0;
	int startPoint = 0;
	int endPoint = 0;
	int minX = traj[0].x;
	int minY = traj[0].y;
	int maxX = traj[0].x;
	int maxY = traj[0].y;
	int newMinX = minX;
	int newMaxX = maxX;
	int newMinY = minY;
	int newMaxY = maxY;
	unsigned int dist = 0;
	bool flag = false;
	for (int i = 0; i < traj.size(); i++)
	{
		flag = true;

		if (flag)
			if (traj[i].x > maxX){
				if ((traj[i].x - minX) < iDistX)
					newMaxX = traj[i].x;
				else
					flag = false;
			}
			else if (traj[i].x < minX){
				if ((maxX - traj[i].x) < iDistX)
					newMinX = traj[i].x;
				else
					flag = false;
			}

			if (flag)
				if (traj[i].y > maxY){
					if ((traj[i].y - minY) < iDistY)
						newMaxY = traj[i].y;
					else
						flag = false;
				}
				else if (traj[i].y < minY){
					if ((maxY - traj[i].y) < iDistY)
						newMinY = traj[i].y;
					else
						flag = false;
				}

				if (flag)
					if ((traj[i].time - traj[startPoint].time) >= iTeps)
					{
						flag = false;
					}

				if (flag)
				{
					minX = newMinX;
					maxX = newMaxX;
					minY = newMinY;
					maxY = newMaxY;
				}
				else
				{

					cubeIndex++;
					cubelist.push_back(
						createNewCube(
						cubeIndex, minX, maxX, minY, maxY, traj[startPoint].time, traj[endPoint].time, userNo, trajNo,
						traj[endPoint].x, traj[startPoint].x, traj[endPoint].y, traj[startPoint].y, iDistX, iDistY));

					minX = traj[i].x;
					maxX = traj[i].x;
					minY = traj[i].y;
					maxY = traj[i].y;
					newMinX = traj[i].x;
					newMaxX = traj[i].x;
					newMinY = traj[i].y;
					newMaxY = traj[i].y;

					startPoint = i;
				}
				endPoint = i;

	}
	cubeIndex++;
	cubelist.push_back(createNewCube(cubeIndex, minX, maxX, minY, maxY, traj[startPoint].time, traj[endPoint].time, userNo, trajNo,
		traj[endPoint].x, traj[startPoint].x, traj[endPoint].y, traj[startPoint].y, iDistX, iDistY));

	return cubelist;
}

int angleDiff(int a, int b)
{
	int result = abs(a - b);
	if (result > 180)
	{
		result = 360 - result;
	}
	return result;
}

int getDist(int lng1, int lat1, int lng2, int lat2, int iDistX, int iDistY, int meters)
{
	int dx = abs(lng2 - lng1) * meters / iDistX;
	int dy = abs(lat2 - lat1) * meters / iDistY;
	return (int)sqrt(pow(dx, 2) + pow(dy, 2));
}
