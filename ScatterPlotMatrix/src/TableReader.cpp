#include "TableReader.h"

TableReader::TableReader()
{
}

TableReader::~TableReader()
{
}

void TableReader::read_data(char *filename)
{
	//Parse column and column values from specified file
	string line;
	ifstream data_file(filename);
	if (data_file.is_open())
	{
		int is_first_line = 1;
		while (getline(data_file, line))
		{
			if (is_first_line) 
			{
				char *column;
				char *delimiter = "\t";
				char *line_chars = strdup(line.c_str());
				column = strtok(line_chars, delimiter);
				while (column != NULL)
				{
					column_names.push_back(column);
					column = strtok(NULL, delimiter);
				}
				column_values.resize(column_names.size());
				is_first_line = 0;
			}
			else {
				char *column_value;
				char *delimiter = "\t";
				char *line_chars = strdup(line.c_str());
				column_value = strtok(line_chars, delimiter);
				int column = 0;
				while (column_value != NULL)
				{
					column_values[column].push_back(stof(column_value));
					column_value = strtok(NULL, delimiter);
					column++;
				}
			}
		}
		data_file.close();
	}

}

int TableReader::get_column_count()
{
	return column_names.size();
}

vector<float> TableReader::get_column(int which_column)
{
	return column_values[which_column];
}

string TableReader::get_column_name(int which_column)
{
	return column_names[which_column];
}
