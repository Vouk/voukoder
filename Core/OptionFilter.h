#pragma once

#include "OptionFilterInfo.h"
#include "wxPGOptionProperty.h"

class OptionFilter
{
protected:
	OptionFilterInfo filterInfo;
	wxPropertyGrid *propertyGrid;

	OptionFilter(OptionFilterInfo filterInfo, wxPropertyGrid *propertyGrid) :
		filterInfo(filterInfo),
		propertyGrid(propertyGrid)
	{}

public:
	virtual void Execute() = 0;
};
