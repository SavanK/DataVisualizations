#include "ColumnValue.h"

ColumnValue::ColumnValue()
{
	val_int = -1;
	val_float = -1.0f;
	val_string = "";
}

ColumnValue::~ColumnValue()
{
}

void ColumnValue::get_value(int * val)
{
	*val = val_int;
}

void ColumnValue::get_value(float * val)
{
	*val = val_float;
}

void ColumnValue::get_value(string * val)
{
	*val = val_string;
}

void ColumnValue::set_value(int val)
{
	val_int = val;
}

void ColumnValue::set_value(float val)
{
	val_float = val;
}

void ColumnValue::set_value(string val)
{
	val_string = val;
}