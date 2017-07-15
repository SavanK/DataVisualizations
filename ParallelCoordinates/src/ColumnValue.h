#pragma once
#include <string>

using namespace std;

class ColumnValue
{
private:
	int val_int;
	float val_float;
	string val_string;

public:
	void get_value(int *val);
	void get_value(float *val);
	void get_value(string *val);
	void set_value(int val);
	void set_value(float val);
	void set_value(string val);

	ColumnValue();
	~ColumnValue();
};
