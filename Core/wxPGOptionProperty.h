#pragma once

#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "EncoderOptionInfo.h"

class wxPGCheckedRenderer: public wxPGDefaultRenderer
{
public:
	virtual bool Render(wxDC&, const wxRect&, const wxPropertyGrid*, wxPGProperty*, int column, int item, int flags) const wxOVERRIDE;
};

class wxOptionProperty: public wxPGProperty
{
public:
	wxOptionProperty(const EncoderOptionInfo &optionInfo);
	int GetIndexForValue(int value) const;
	~wxOptionProperty();
	const EncoderOptionInfo GetOptionInfo();

	virtual wxPGCellRenderer* GetCellRenderer(int column) const wxOVERRIDE;
	virtual wxString ValueToString(wxVariant& value, int argFlags) const wxOVERRIDE;
	virtual bool StringToValue(wxVariant& variant, const wxString& text, int WXUNUSED(argFlags)) const wxOVERRIDE;
	virtual bool IntToValue(wxVariant& variant, int value, int argFlags) const wxOVERRIDE;
	virtual const wxPGEditor* DoGetEditorClass() const;
	virtual bool ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const wxOVERRIDE;

	bool IsChecked() const;
	void SetChecked(bool checked = TRUE);
	int GetCheckMinX() const;
	int GetCheckMaxX() const;
	void SetCheckMinX(int x);
	void SetCheckMaxX(int x);

protected:
	int GetIndex() const;
	void SetIndex(int index);
	void OnSetValue();
	bool ValueFromString_(wxVariant& value, int* pIndex, const wxString& text, int WXUNUSED(argFlags)) const;
	bool ValueFromInt_(wxVariant& value, int* pIndex, int intVal, int argFlags) const;

private:
	EncoderOptionInfo optionInfo;
	wxPGCellRenderer *labelRenderer;
	wxPGProperty *internalProperty;
	int m_index;
	bool m_checked;
	int m_checkMinX;
	int m_checkMaxX;

	wxDECLARE_ABSTRACT_CLASS(wxOptionProperty);
};