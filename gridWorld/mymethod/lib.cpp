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

int angleDiff(int a, int b)
{
	int result = abs(a - b);
	if (result > 180)
	{
		result = 360 - result;
	}
	return result;
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
		
		tempPoint.time = (int)floor(datetime+0.5);
		tempPoint.traj = trajNo;
		tempPoint.user = userNo;

		if(tempPoint.time < iPreDateTime)
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
	if(row.size() > 0)
		trajlist.push_back(row);

	return trajlist;
}

int getDist(int lng1, int lat1, int lng2, int lat2, int iDistX, int iDistY, int meters)
{
	int dx = abs(lng2 - lng1) * meters / iDistX;
	int dy = abs(lat2 - lat1) * meters / iDistY;
	return (int)sqrt(pow(dx, 2) + pow(dy, 2));
}

int tagGridNum(GPS aGps)
{
	int x = (aGps.x - LNG_BOTTOM) / DISTX;
	int y = (aGps.y - LAT_BOTTOM) / DISTY;
	int z = aGps.time / T_EPSILON;

	return (z * spaceSize) + (x * SIZE_Y_OF_GRID) + y;
}

gdist * getGridList(vector<GPS> vecGPS)
{
	gdist * vecGdist = new gdist;
	vecGdist->next = nullptr;
	gdist * cond = vecGdist;
	int startX = vecGPS[0].x;
	int startY = vecGPS[0].y;

	int startInd = 0;
	int startGridNum = tagGridNum(vecGPS[0]);
	int newGridNum = 0;
	int sizeVecGPS = vecGPS.size();
	int endVecGPS = sizeVecGPS - 1;

	for (int i = 1; i <= endVecGPS; i++)
	{
		newGridNum = tagGridNum(vecGPS[i]);
		if (newGridNum == startGridNum)
		{
		}
		else
		{
			unsigned int tempDist = getDist(startY, startX, vecGPS[i - 1].y, vecGPS[i - 1].x,DISTX,DISTY,METERS);
			if (tempDist > 0){
				gdist * tempGdist = new gdist;
				tempGdist->trajID = vecGPS[0].traj;
				tempGdist->gridID = startGridNum;
				tempGdist->dist = tempDist;
				tempGdist->next = nullptr;
				cond->next = tempGdist;
				cond = cond->next;
			}
			startInd = i;
			startX = vecGPS[i].x;
			startY = vecGPS[i].y;
			startGridNum = newGridNum;
		}
	}

	if (startInd < endVecGPS)
	{
		unsigned int tempDist = getDist(startY, startX, vecGPS[endVecGPS].y, vecGPS[endVecGPS].x, DISTX, DISTY, METERS);
		if (tempDist > 0){
			gdist * tempGdist = new gdist;
			tempGdist->trajID = vecGPS[0].traj;
			tempGdist->gridID = startGridNum;
			tempGdist->dist = tempDist;
			tempGdist->next = nullptr;
			//vecGdist.push_back(tempGdist);
			cond->next = tempGdist;
		}
	}

	return vecGdist->next;
}

long long unsigned int getGridMethodResult(vector<gdist*>& gridWorldList)
{
	unsigned int * gridWorld = new unsigned int[arrSize];
	gdist * aTraj2;
	gdist * aTraj;
	long long unsigned int result = 0;
	int endOfGridWorldList = gridWorldList.size() - 1;
	for (int i = 0; i <= endOfGridWorldList; i++)
	{
		for (int j = i + 1; j <= endOfGridWorldList; j++)
		{
			fill(gridWorld, gridWorld + arrSize, 0);
			for (aTraj = gridWorldList[i]; aTraj != nullptr; aTraj = aTraj->next)
			{
				gridWorld[aTraj->gridID] += aTraj->dist;
			}
			unordered_set<int> targetIndexSet;
			for (aTraj2 = gridWorldList[j]; aTraj2 != nullptr; aTraj2 = aTraj2->next)
			{
				if (gridWorld[aTraj2->gridID] > 0)
				{
					targetIndexSet.insert(aTraj2->gridID);
					result += aTraj2->dist;
				}
			}
			for (auto& si : targetIndexSet)
			{
				result += gridWorld[si];
			}
			targetIndexSet.clear();
		}

	}
	return result;
}
