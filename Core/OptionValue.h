#pragma once

#include <string>

using namespace std;

enum struct OptionValueType {
	Empty,
	Integer,
	Float,
	Boolean,
	String
};

struct OptionValue
{
	int intValue = 0;
	float floatValue = 0.0f;
	string stringValue;
	bool boolValue = false;
	OptionValueType type = OptionValueType::Empty;

	OptionValue() {};
	OptionValue(string value) : stringValue(value), type(OptionValueType::String) {};
	OptionValue(int value) : intValue(value), type(OptionValueType::Integer) {};
	OptionValue(float value) : floatValue(value), type(OptionValueType::Float) {};
	OptionValue(bool value) : boolValue(value), type(OptionValueType::Boolean) {};

	const string toString()
	{
		switch (type)
		{
		case OptionValueType::Integer:
			return to_string(intValue);
		case OptionValueType::Float:
			return to_string(floatValue);
		case OptionValueType::Boolean:
			return boolValue ? "1" : "0";
		default:
			return stringValue;
		}
	}
};
