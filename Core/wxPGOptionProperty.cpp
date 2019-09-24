#include "wxPGOptionProperty.h"
#include <wx/numformatter.h>
#include <wx/regex.h>

bool wxPGCheckedRenderer::Render(wxDC& dc, const wxRect& rect, const wxPropertyGrid* propertyGrid, wxPGProperty* property, int column, int item, int flags) const
{
	if (column == 0 && item == -1)
	{
		wxOptionProperty *msp = wxDynamicCast(property, wxOptionProperty);
		if (msp)
		{
			// Make a bitmap of a checkbox
			wxRendererNative& renderer = wxRendererNative::Get();
			wxSize sz = renderer.GetCheckBoxSize(const_cast<wxPropertyGrid*>(propertyGrid));

			wxBitmap bmp(sz.GetWidth(), rect.GetHeight() - 1);
			wxMemoryDC mem(bmp);
			mem.SetBackground(wxBrush(*wxGREEN));
			mem.Clear();
			wxRect r(0, 0, sz.GetWidth(), rect.GetHeight() - 1);

			int cbFlags = msp->IsChecked() ? wxCONTROL_CHECKED : 0;
			renderer.DrawCheckBox(const_cast<wxPropertyGrid*>(propertyGrid), mem, r, cbFlags);

			mem.SelectObject(wxNullBitmap);
			bmp.SetMask(new wxMask(bmp, *wxGREEN));

			// Set the bitmap that was made to be the cell's bitmap
			property->GetCell(0).SetBitmap(bmp);

			// Record the location at which the checkbox was drawn, so that
			// we can tell if mouse clicks occur over it
			int checkMinX =	rect.GetLeft() + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1;
			msp->SetCheckMinX(checkMinX);
			msp->SetCheckMaxX(checkMinX + sz.GetWidth());
		}
	}

	return wxPGDefaultRenderer::Render(dc, rect, propertyGrid, property, column, item, flags);
}

wxOptionProperty::wxOptionProperty(const EncoderOptionInfo &optionInfo):
	wxPGProperty(optionInfo.name, optionInfo.id),
	optionInfo(optionInfo)
{
	labelRenderer = new wxPGCheckedRenderer;

	Enable(optionInfo.control.enabled);
	SetHelpString(optionInfo.description);
	
	EncoderOptionType type = optionInfo.control.type;
	if (type == EncoderOptionType::ComboBox)
	{
		SetValue((long)optionInfo.control.selectedIndex);

		// Add all items
		wxPGChoices items;
		for (int i = 0; i < optionInfo.control.items.size(); i++)
		{
			m_choices.Add(optionInfo.control.items[i].name, i);
		}
	}
	else if (type == EncoderOptionType::Integer)
	{
		SetValue((long)optionInfo.control.value.intValue);
		SetAttribute(wxPG_ATTR_SPINCTRL_STEP, optionInfo.control.singleStep.intValue);
	}
	else if (type == EncoderOptionType::Float)
	{
		SetValue(optionInfo.control.value.floatValue);
		SetAttribute(wxPG_ATTR_SPINCTRL_STEP, optionInfo.control.singleStep.floatValue);
	}
	else if (type == EncoderOptionType::Boolean)
	{
		m_choices.Assign(wxPGGlobalVars->m_boolChoices);
		SetValue(wxPGVariant_Bool(optionInfo.control.value.boolValue));
		m_flags |= wxPG_PROP_USE_DCC;
	}
	else if (type == EncoderOptionType::String)
	{
		SetValue(optionInfo.control.value.stringValue);
	}

	m_checked = false;
}

wxOptionProperty::~wxOptionProperty()
{
	delete(labelRenderer);
}

int wxOptionProperty::GetIndexForValue(int value) const
{
	if (!m_choices.IsOk())
		return -1;

	const int intVal = m_choices.Index(value);
	if (intVal >= 0)
		return intVal;

	return -1;
}

const EncoderOptionInfo wxOptionProperty::GetOptionInfo()
{
	return optionInfo;
}

wxString wxOptionProperty::ValueToString(wxVariant& value, int argFlags) const
{
	EncoderOptionType type = optionInfo.control.type;

	if (type == EncoderOptionType::ComboBox)
	{
		int index = m_choices.Index(value.GetLong());
		if (index >= 0)
		{
			if (argFlags & wxPG_FULL_VALUE)
			{
				return optionInfo.control.items[index].value;
			}
			return m_choices.GetLabel(index);
		}
	}
	else if (type == EncoderOptionType::Integer)
	{
		return wxNumberFormatter::ToString(value.GetInteger());
	}
	else if (type == EncoderOptionType::Float)
	{
		return wxNumberFormatter::ToString(value.GetDouble(), 1);
	}
	else if (type == EncoderOptionType::Boolean)
	{
		bool boolValue = value.GetBool();
		if (argFlags & wxPG_FULL_VALUE)
		{
			return boolValue ? "1" : "0";
		}
		return wxPGGlobalVars->m_boolChoices[boolValue ? 1 : 0].GetText();
	}
	else if (type == EncoderOptionType::String)
	{
		return value.GetString();
	}

	return wxEmptyString;
}

bool wxOptionProperty::StringToValue(wxVariant& variant, const wxString& text, int argFlags) const
{
	if (optionInfo.control.type == EncoderOptionType::ComboBox)
	{
		return ValueFromString_(variant, NULL, text, argFlags);
	}
	else if (optionInfo.control.type == EncoderOptionType::Integer)
	{
		long value;
		if (text.ToLong(&value))
		{
			variant = value;
			return true;
		}
	}
	else if (optionInfo.control.type == EncoderOptionType::Float)
	{
		double value;
		if (text.ToDouble(&value))
		{
			variant = value;
			return true;
		}
	}
	else if (optionInfo.control.type == EncoderOptionType::Boolean)
	{
		variant = wxPGVariant_Bool(text == "1");
		return true;
	}
	else if (optionInfo.control.type == EncoderOptionType::String)
	{
		if (variant != text)
		{
			variant = text;
			return true;
		}
	}

	return false;
}

bool wxOptionProperty::IntToValue(wxVariant& variant, int value, int argFlags) const
{
	if (optionInfo.control.type == EncoderOptionType::ComboBox)
	{
		return ValueFromInt_(variant, NULL, value, argFlags);
	}
	else if (optionInfo.control.type == EncoderOptionType::Integer)
	{
		if (!variant.IsType(wxPG_VARIANT_TYPE_LONG) || variant != (long)value)
		{
			variant = (long)value;
			return true;
		}
	}
	else if (optionInfo.control.type == EncoderOptionType::Boolean)
	{
		bool boolValue = value ? true : false;
		if (variant != boolValue)
		{
			variant = wxPGVariant_Bool(boolValue);
			return true;
		}
	}

	return false;
}

void wxOptionProperty::OnSetValue()
{
	if (optionInfo.control.type == EncoderOptionType::ComboBox)
	{
		const wxString valType(m_value.GetType());

		int index = -1;
		if (valType == wxPG_VARIANT_TYPE_LONG)
		{
			ValueFromInt_(m_value, &index, m_value.GetLong(), wxPG_FULL_VALUE);
		}
		else if (valType == wxPG_VARIANT_TYPE_STRING)
		{
			ValueFromString_(m_value, &index, m_value.GetString(), 0);
		}
		else
		{
			wxFAIL_MSG(wxS("Unexpected value type"));
			return;
		}

		m_index = index;
	}
	else if (optionInfo.control.type == EncoderOptionType::String)
	{
		if (!m_value.IsNull() && m_value.GetString() == wxS("<composed>"))
			SetFlag(wxPG_PROP_COMPOSED_VALUE);

		if (HasFlag(wxPG_PROP_COMPOSED_VALUE))
		{
			wxString s;
			DoGenerateComposedValue(s);
			m_value = s;
		}
	}
}

bool wxOptionProperty::ValueFromString_(wxVariant& value, int* pIndex, const wxString& text, int WXUNUSED(argFlags)) const
{
	int useIndex = -1;
	long useValue = 0;

	for (unsigned int i = 0; i < m_choices.GetCount(); i++)
	{
		const wxString& entryLabel = m_choices.GetLabel(i);
		if (text.CmpNoCase(entryLabel) == 0)
		{
			useIndex = (int)i;
			useValue = m_choices.GetValue(i);
			break;
		}
	}

	if (pIndex)
	{
		*pIndex = useIndex;
	}

	if (useIndex != GetIndex())
	{
		if (useIndex != -1)
		{
			value = (long)useValue;
			return true;
		}

		value.MakeNull();
	}

	return false;
}

bool wxOptionProperty::ValueFromInt_(wxVariant& value, int* pIndex, int intVal, int argFlags) const
{
	// If wxPG_FULL_VALUE is *not* in argFlags, then intVal is index from combo box.
	//
	int setAsNextIndex = -2;

	if (argFlags & wxPG_FULL_VALUE)
	{
		setAsNextIndex = GetIndexForValue(intVal);
	}
	else
	{
		if (intVal != GetIndex())
		{
			setAsNextIndex = intVal;
		}
	}

	if (setAsNextIndex != -2)
	{
		if (!(argFlags & wxPG_FULL_VALUE))
			intVal = m_choices.GetValue(intVal);

		value = (long)intVal;

		if (pIndex)
		{
			*pIndex = setAsNextIndex;
		}
		return true;
	}

	if (pIndex)
	{
		*pIndex = intVal;
	}
	return false;
}

bool wxOptionProperty::ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const
{
	if (optionInfo.control.type == EncoderOptionType::Integer)
	{
		int intValue = value.GetInteger();
		if (intValue < optionInfo.control.minimum.intValue)
		{
			value = (long)optionInfo.control.minimum.intValue;
			return false;
		}
		else if (intValue > optionInfo.control.maximum.intValue)
		{
			value = (long)optionInfo.control.maximum.intValue;
			return false;
		}
	}
	else if (optionInfo.control.type == EncoderOptionType::Float)
	{
		double dblValue = value.GetDouble();
		if (dblValue < optionInfo.control.minimum.floatValue)
		{
			value = optionInfo.control.minimum.floatValue;
			return false;
		}
		else if (dblValue > optionInfo.control.maximum.floatValue)
		{
			value = optionInfo.control.maximum.floatValue;
			return false;
		}
	}
	else if (optionInfo.control.type == EncoderOptionType::String)
	{
		if (optionInfo.control.regex.size() > 0)
		{
			wxRegEx regex(optionInfo.control.regex, wxRE_ADVANCED);
			if (regex.IsValid())
			{
				wxString strValue = value.GetString();
				if (!regex.Matches(strValue))
				{
					return false;
				}
			}
		}
	}

	return true;
}

const wxPGEditor* wxOptionProperty::DoGetEditorClass() const
{
	EncoderOptionType type = optionInfo.control.type;
	
	if (type == EncoderOptionType::Integer ||
		type == EncoderOptionType::Float)
	{
		return wxPGEditor_SpinCtrl;
	}
	else if (type == EncoderOptionType::Boolean || 
		type == EncoderOptionType::ComboBox)
	{
		return wxPGEditor_Choice;
	}

	return wxPGEditor_TextCtrl;
}

wxPGCellRenderer* wxOptionProperty::GetCellRenderer(int column) const
{
	// Show a checkbox next to the property if it is not forced
	if (column == 0 && !optionInfo.isForced)
		return labelRenderer;
	else
		return wxPGProperty::GetCellRenderer(column);
}

void wxOptionProperty::SetIndex(int index)
{
	m_index = index;
}

int wxOptionProperty::GetIndex() const
{
	if (m_value.IsNull())
		return -1;

	return m_index;
}

bool wxOptionProperty::IsChecked() const
{
	return m_checked || optionInfo.isForced;
}

void wxOptionProperty::SetChecked(bool checked)
{
	m_checked = checked;
}

int wxOptionProperty::GetCheckMinX() const
{
	return m_checkMinX;
}

int wxOptionProperty::GetCheckMaxX() const
{
	return m_checkMaxX;
}

void wxOptionProperty::SetCheckMinX(int x)
{
	m_checkMinX = x;
}

void wxOptionProperty::SetCheckMaxX(int x)
{
	m_checkMaxX = x;
}

wxIMPLEMENT_ABSTRACT_CLASS(wxOptionProperty, wxStringProperty);