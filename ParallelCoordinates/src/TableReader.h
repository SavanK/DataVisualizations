#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <set>
#include "ColumnValue.h"
#include "Column.h"
#include <algorithm>

using namespace std;

class TableReader
{
private:
	vector<Column*> columns;
	vector<vector<ColumnValue*>> column_values;

	int get_column_type(string col_val_str);
	void construct_columns(string file_name);
	void construct_column_values(string file_name);
	ColumnValue* create_column_value(string col_val_str, int column_index);
	void check_for_group_columns();
	float find_median(vector<int> arr, int start_index, int end_index);
	float find_median(vector<float> arr, int start_index, int end_index);

public:
	
	TableReader();
	~TableReader();

	void read_data(string filename);
	vector<vector<ColumnValue*>> get_column_values();
	vector<Column*> get_columns();
	void reset();
};