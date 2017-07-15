#include "Column.h"

void Column::set_type(int val_type)
{
	type = val_type;
	if (type == Column::TYPE_STRING) {
		// set subtype to unique by default
		string_subtype = Column::SUBTYPE_STRING_UNIQUE;
	}
}

int Column::get_type()
{
	return type;
}

int Column::get_string_subtype()
{
	return string_subtype;
}

void Column::set_string_subtype(int string_type)
{
	string_subtype = string_type;
}

void Column::set_name(string name)
{
	this->name = name;
}

string Column::get_name()
{
	return name;
}

void Column::add_group_name(string group_name)
{
	group_names.push_back(group_name);
}

vector<string> Column::get_group_names()
{
	return group_names;
}

void Column::set_draw_order(int order)
{
	draw_order = order;
}

int Column::get_draw_order()
{
	return draw_order;
}

int Column::get_max_i()
{
	return max_value_i;
}

int Column::get_min_i()
{
	return min_value_i;
}

void Column::set_max_i(int i)
{
	max_value_i = i;
}

void Column::set_min_i(int i)
{
	min_value_i = i;
}

float Column::get_max_f()
{
	return max_value_f;
}

float Column::get_min_f()
{
	return min_value_f;
}

void Column::set_max_f(float f)
{
	max_value_f = f;
}

void Column::set_min_f(float f)
{
	min_value_f = f;
}

Column::Column()
{
	type = Column::TYPE_INT;
	string_subtype = Column::SUBTYPE_STRING_UNIQUE;
	draw_order = Column::MIN_TO_MAX;
}

Column::~Column()
{
}
