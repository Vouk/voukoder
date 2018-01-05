#pragma once

#include <string>
#include <vector>

using namespace std;

typedef struct ElementGroup
{
	string name;
	string label;
	string parent;
} ElementGroup;

typedef struct ParamGroup
{
	string name;
	string delimiter;
	string separator;
	string noValueReplacement;
	vector<string> parameters;
} ParamGroup;

// Required members to create an ui element
struct ParamInfo
{
	string name;
	string label;
	string type;
	vector<string> flags;
	bool isSubValue = false;

	union
	{
		int intValue;
		float floatValue = NAN;
	} default;

	union
	{
		int intValue;
		float floatValue = NAN;
	} min;

	union
	{
		int intValue;
		float floatValue = NAN;
	} max;
};

// All members to parse a subvalue from json
struct ParameterSubValue : ParamInfo
{
	string pixelFormat;
	bool useDefaultValue = false;
	map<string, string> parameters;
};

// 
struct ParameterValueInfo
{
	int id;
	string name;
	string pixelFormat;
	vector<ParameterSubValue> subValues;
	map<string, string> parameters;
};

// All members to parse a param from json
struct ParameterInfo : ParamInfo
{
	string group;
	bool useDefaultValue = false;
	vector<ParameterValueInfo> values;
	map<string, string> parameters;
};

template <typename T> 
T FilterTypeVectorById(vector<T> vec, int id)
{
	vector<T>::iterator it = find_if(vec.begin(), vec.end(), [id](T item)
	{
		return item.id == id;
	});

	T result = *it;
	if (it == vec.end())
	{
		result.id = -1;
	}

	return result;
}