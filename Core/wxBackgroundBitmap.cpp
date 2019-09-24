#include "wxBackgroundBitmap.h"

bool wxBackgroundBitmap::ProcessEvent(wxEvent& Event)
{
	bool ret = Inherited::ProcessEvent(Event);

	if (Event.GetEventType() == wxEVT_ERASE_BACKGROUND)
	{
		wxEraseEvent& EraseEvent = dynamic_cast<wxEraseEvent&>(Event);
		wxDC* DC = EraseEvent.GetDC();
		DC->DrawBitmap(Bitmap, 0, 0, false);
		return true;
	}

	return ret;
}
