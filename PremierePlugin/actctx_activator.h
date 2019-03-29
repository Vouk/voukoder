#pragma once

#include <Windows.h>

class actctx_activator
{
protected:
	ULONG_PTR m_cookie; // Cookie for context deactivation

public:
	// Construct the activator and activates the given activation context
	actctx_activator(_In_ HANDLE hActCtx)
	{
		if (!ActivateActCtx(hActCtx, &m_cookie))
			m_cookie = 0;
	}

	// Deactivates activation context and destructs the activator
	virtual ~actctx_activator()
	{
		if (m_cookie)
			DeactivateActCtx(0, m_cookie);
	}
};