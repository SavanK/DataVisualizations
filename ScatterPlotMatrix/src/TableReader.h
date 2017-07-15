#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "string.h"

using namespace std;

class TableReader
{
public:
	vector<string> column_names;
	vector<vector<float>> column_values;
	
	TableReader();
	~TableReader();

	void read_data(char *filename);
	int get_column_count();
	vector<float> get_column(int which_column);
	string get_column_name(int which_column);
};

