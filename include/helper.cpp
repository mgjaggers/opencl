#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "helper.h"
using namespace std;
namespace helper{
	void get_data(char * filename, vector<uint32_t> * data){
		cout << "Opening: " << filename << endl;
		// Opening file
		ifstream infile(filename);
		while(infile){
			string s;
			if(!getline(infile,s)) break;
			istringstream ss(s);
			
			while(ss){
				string s;
				if(!getline(ss,s,',')) break;
                                cout << s << endl;
				data->push_back(stoi(s));
			}
		}
		infile.close();
	}
}
