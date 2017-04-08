#include "lib.h"

int main()
{
	clock_t t1, t2, t3;
	string dirName1 = "..\\..\\..\\Geolife_Trajectories_1.3\\Data\\";
	string dirName2 = "\\Trajectory\\";
	string dirName = "";
	string dirNo = "000";
	string fullFileName;
	vector<vector<GPS>> trajList;
	vector<vector<GPS>> trajList2;
	stringstream ss2;

	long long unsigned int dist = 0;
	
	try{
		DWORD ftyp = GetFileAttributesA(dirName1.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES){
			throw "DATASET ERROR";
		}

		ofstream myOutFile;
		myOutFile.open("result.txt");
		t1 = clock();
		for (int j = 0; j < 182; j++)
		{
			dirNo = to_string(j);
			dirNo.insert(0, 3 - dirNo.length(), '0');
			dirName = dirName1 + dirNo + dirName2;
			cout << dirName << endl;
			vector<string> files = vector<string>();
			getdir(dirName, files);
			
			for (int i = 0; i < files.size(); i++)
			{
				fullFileName = dirName + files[i];
				trajList = readGPSfile(fullFileName.c_str(),j,i);
				if (trajList.size() <= 0)
					continue;
				for (unsigned int icmcl = 0; icmcl < trajList.size(); icmcl++)
				{
						vector<GPS> temp1;
						temp1 = cubelize(trajList[icmcl], DISTX, DISTY, T_EPSILON);
						if (temp1.size() <= 0)
								continue;
						trajList2.push_back(temp1 );
				}
				trajList.clear();
			}
			files.clear();
		}

		cout << "parewise comparing..." << endl;
		int endOfTrajList = trajList2.size();

		t2 = clock();
		for (int j = 0; j < endOfTrajList; j++)
		{
			//#pragma omp parallel for
			for (int i = j+1; i < endOfTrajList; i++)
			{
				dist += pairwise(trajList2[j], trajList2[i], DISTX, DISTY, T_EPSILON);
			}
		}
		t3 = clock();

		ss2 << "pairwise() accumulative distance: "<< dist <<" meters" << endl
			<< "pairwise() : " << ((double)(t3 - t2)) / CLOCKS_PER_SEC << "  seconds" << endl
			<< "preprocessing time : " << ((double)(t2 - t1)) / CLOCKS_PER_SEC << " seconds" << endl;
		cout << ss2.str();
		myOutFile << ss2.str();
		myOutFile.close();
	}catch (const char* message) {
        cout << message << std::endl;
    }

	system("pause");
	return 0;
}
