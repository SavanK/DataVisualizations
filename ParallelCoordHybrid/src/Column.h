#pragma once
#include <string>
#include <vector>

using namespace std;

class Column
{
private:
	int type;
	int string_subtype;
	string name;
	int max_value_i, min_value_i;
	float max_value_f, min_value_f;
	int draw_order;

	vector<string> group_names;

public:
	static const int TYPE_INT = 0;
	static const int TYPE_FLOAT = 1;
	static const int TYPE_STRING = 2;
	static const int MIN_TO_MAX = 0;
	static const int MAX_TO_MIN = 1;

	static const int SUBTYPE_STRING_UNIQUE = 0;
	static const int SUBTYPE_STRING_GROUP = 1;

	void set_type(int val_type);
	int get_type();

	int get_string_subtype();
	void set_string_subtype(int string_type);

	void set_name(string name);
	string get_name();

	void add_group_name(string group_name);
	vector<string> get_group_names();

	void set_draw_order(int order);
	int get_draw_order();

	int get_max_i();
	int get_min_i();
	void set_max_i(int i);
	void set_min_i(int i);

	float get_max_f();
	float get_min_f();
	void set_max_f(float f);
	void set_min_f(float f);

	Column();
	~Column();
};

