#include "TableReader.h"

TableReader::TableReader()
{
}

TableReader::~TableReader()
{
}

int TableReader::get_column_type(string col_val_str)
{
	int type;
	int digit_count = 0;
	int dot_count = 0;
	for (int i = 0; i < col_val_str.length(); i++) {
		if (isdigit(col_val_str.at(i))) {
			digit_count++;
		}
		else if (col_val_str.at(i) == '.') {
			dot_count++;
		}
	}

	if (digit_count == col_val_str.length()) {
		type = Column::TYPE_INT;
	}
	else if (digit_count + 1 == col_val_str.length() && dot_count == 1) {
		type = Column::TYPE_FLOAT;
	}
	else {
		type = Column::TYPE_STRING;
	}

	return type;
}

void TableReader::construct_columns(string file_name)
{
	//Parse column name and its type from specified file
	string line;
	ifstream data_file(file_name);
	if (data_file.is_open())
	{
		int line_count = 0;
		while (getline(data_file, line) && line_count < 2)
		{
			if (line_count == 0)
			{
				// first line
				char *column_str;
				char *delimiter = "\t";
				char *line_chars = strdup(line.c_str());
				column_str = strtok(line_chars, delimiter);
				while (column_str != NULL)
				{
					Column *column = new Column();
					column->set_name(column_str);
					columns.push_back(column);
					column_str = strtok(NULL, delimiter);
				}
				column_values.resize(columns.size());
			}
			else if(line_count == 1) {
				// second line
				char *column_value_str;
				char *delimiter = "\t";
				char *line_chars = strdup(line.c_str());
				column_value_str = strtok(line_chars, delimiter);
				int column_index = 0;
				while (column_value_str != NULL)
				{
					columns[column_index]->set_type(get_column_type(column_value_str));
					column_value_str = strtok(NULL, delimiter);
					column_index++;
				}
			}
			line_count++;
		}
		data_file.close();
	}
}

void TableReader::construct_column_values(string file_name)
{
	//Parse column values from specified file
	string line;
	ifstream data_file(file_name);
	if (data_file.is_open())
	{
		int is_first_line = 1;
		while (getline(data_file, line))
		{
			if (is_first_line)
			{
				is_first_line = 0;
			}
			else {
				size_t found = line.find_first_of('\t', 0);
				int start = 0;
				int column_index = 0;
				while (found != string::npos) {
					string token;
					if (found - start == 0) {
						token = "0";
					}
					else {
						token = line.substr(start, found - start);
					}
					column_values[column_index].push_back(create_column_value(token, column_index));
					start = found + 1;
					found = line.find_first_of('\t', start);
					column_index++;
				}
				// add last column
				string token = line.substr(start, found);
				column_values[column_index].push_back(create_column_value(token, column_index));
			}
		}
		data_file.close();
	}

	for (int i = 0; i < columns.size(); i++) {
		vector<ColumnValue*> col_vals = column_values[i];
		if (columns[i]->get_type() == Column::TYPE_INT) {
			int max_i, min_i;
			col_vals[0]->get_value(&max_i);
			col_vals[0]->get_value(&min_i);
			vector<int> col_vals_i;
			for (int j = 1; j < col_vals.size(); j++) {
				int val;
				col_vals[j]->get_value(&val);
				if (val > max_i) {
					max_i = val;
				}
				else if (val < min_i) {
					min_i = val;
				}
				col_vals_i.push_back(val);
			}
			columns[i]->set_max_i(max_i);
			columns[i]->set_min_i(min_i);

			sort(col_vals_i.begin(), col_vals_i.end());
			float median, upper_q, lower_q, upper_w, lower_w;
			
			median = find_median(col_vals_i, 0, col_vals_i.size() - 1);
			upper_q = find_median(col_vals_i, 0, col_vals_i.size() / 2);
			lower_q = find_median(col_vals_i, col_vals_i.size() / 2 + 1, col_vals_i.size() - 1);
			upper_w = find_median(col_vals_i, 0, col_vals_i.size() / 4);
			lower_w = find_median(col_vals_i, col_vals_i.size() * 3 / 4 + 1, col_vals_i.size() - 1);

			columns[i]->set_box_chart_values(median, lower_q, upper_q, lower_w, upper_w);
		}
		else if (columns[i]->get_type() == Column::TYPE_FLOAT) {
			float max_f, min_f;
			col_vals[0]->get_value(&max_f);
			col_vals[0]->get_value(&min_f);
			vector<int> col_vals_f;
			for (int j = 1; j < col_vals.size(); j++) {
				float val;
				col_vals[j]->get_value(&val);
				if (val > max_f) {
					max_f = val;
				}
				else if (val < min_f) {
					min_f = val;
				}
				col_vals_f.push_back(val);
			}
			columns[i]->set_max_f(max_f);
			columns[i]->set_min_f(min_f);

			sort(col_vals_f.begin(), col_vals_f.end());
			float median, upper_q, lower_q, upper_w, lower_w;

			median = find_median(col_vals_f, 0, col_vals_f.size() - 1);
			upper_q = find_median(col_vals_f, 0, col_vals_f.size() / 2);
			lower_q = find_median(col_vals_f, col_vals_f.size() / 2 + 1, col_vals_f.size() - 1);
			upper_w = find_median(col_vals_f, 0, col_vals_f.size() / 4);
			lower_w = find_median(col_vals_f, col_vals_f.size() * 3 / 4 + 1, col_vals_f.size() - 1);

			columns[i]->set_box_chart_values(median, lower_q, upper_q, lower_w, upper_w);
		}
	}
}

ColumnValue* TableReader::create_column_value(string col_val_str, int column_index)
{
	ColumnValue *col_val = new ColumnValue();

	Column *column = columns[column_index];

	switch (column->get_type()) {
	case Column::TYPE_INT:
		if (col_val_str.empty()) {
			col_val->set_value(0);
		}
		else {
			col_val->set_value(stoi(col_val_str));
		}
		break;

	case Column::TYPE_FLOAT:
		if (col_val_str.empty()) {
			col_val->set_value(0.0f);
		}
		else {
			col_val->set_value(stof(col_val_str));
		}
		break;

	case Column::TYPE_STRING:
		if (col_val_str.empty()) {
			col_val->set_value("");
		}
		else {
			col_val->set_value(col_val_str);
		}
		break;
	}

	return col_val;
}

void TableReader::check_for_group_columns()
{
	for (int i = 0; i < columns.size(); i++) {
		if (columns[i]->get_type() == Column::TYPE_STRING) {
			int sub_type = Column::SUBTYPE_STRING_UNIQUE;

			vector<ColumnValue*> col_i_values = column_values[i];
			set<string> group_values;
			for (int j = 0; j < col_i_values.size(); j++) {
				string *val = new string();
				col_i_values[j]->get_value(val);
				group_values.insert(*val);
			}

			if (group_values.size() < col_i_values.size()) {
				// duplicates exist, then grouped string column
				sub_type = Column::SUBTYPE_STRING_GROUP;
				columns[i]->set_string_subtype(sub_type);
				for (set<string>::iterator it = group_values.begin(); it != group_values.end(); ++it) {
					columns[i]->add_group_name(*it);
				}
			}
		}

		if (columns[i]->get_type() == Column::TYPE_STRING && 
			columns[i]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
			// found one string group column
			// even if there is another string grouped column later, it will treated as a unique string column
			// for simplicity reasons
			break;
		}
	}
}

float TableReader::find_median(vector<int> arr, int start_index, int end_index)
{
	int count = end_index - start_index + 1;
	float median;
	if (count % 2 == 1) {
		median = arr[start_index + ((count - 1) / 2)];
	}
	else {
		median = (arr[start_index + count / 2] + arr[start_index + count / 2 + 1]) / 2.0f;
	}
	return median;
}

float TableReader::find_median(vector<float> arr, int start_index, int end_index)
{
	int count = end_index - start_index + 1;
	float median;
	if (count % 2 == 1) {
		median = arr[start_index + ((count - 1) / 2)];
	}
	else {
		median = (arr[start_index + count / 2] + arr[start_index + count / 2 + 1]) / 2.0f;
	}
	return median;
}

void TableReader::read_data(string filename)
{
	construct_columns(filename);
	construct_column_values(filename);
	check_for_group_columns();
}

vector<vector<ColumnValue*>> TableReader::get_column_values()
{
	return column_values;
}

vector<Column*> TableReader::get_columns()
{
	return columns;
}

void TableReader::reset()
{
	for (int i = 0; i < columns.size(); i++) {
		column_values[i].clear();
	}
	columns.clear();
	column_values.clear();
}
