#include "lib.h"

int main()
{
	clock_t t1, t2, t3, t4;

	string dirName1 = "../../../Geolife_Trajectories_1.3/Data/";
	string dirName2 = "/Trajectory/";
	string dirName = "";
	string dirNo = "000";
	string fullFileName;
	vector<vector<GPS>> trajList;
	long long unsigned int dist = 0;
	vector<gdist*> gridWorldList2;
	gdist * tempVec;
	cout << "Grid Size = " << arrSize << " blocks" << endl;
	cout << "Geolife dataset Loading..." << endl;
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
				int trajListSize = trajList.size();
				if (trajListSize <= 0)
					continue;
				for (unsigned int icmcl = 0; icmcl < trajListSize; icmcl++)
				{
					tempVec = getGridList(trajList[icmcl]);

					if (tempVec != nullptr){
						gridWorldList2.push_back(tempVec);
					}
				}
				trajList.clear();
			}
		}
		t2 = clock();
		cout << "gridWordList is built" << endl;
		

		//--------------
		cout << "Grid clustering start..." << endl;
		t3 = clock();
		dist = getGridMethodResult(gridWorldList2);
		t4 = clock();
		cout << "Grid clustering finish" << endl;
		

		stringstream ss2;
		ss2 << "getGridMethodResult() accumulative distance: "<< dist <<" meters" << endl
		 << "getGridMethodResult() : " << ((double)(t4 - t3)) / CLOCKS_PER_SEC << "  seconds" << endl
		 << "preprocessing time : " << ((double)(t2 - t1)) / CLOCKS_PER_SEC << " seconds" << endl;
		cout << ss2.str();
		myOutFile << ss2.str();

		myOutFile.close();
		cout << "end of program" << endl;
	}catch (const char* message) {
        cout << message << std::endl;
    }

	system("pause");
	return 0;
}
