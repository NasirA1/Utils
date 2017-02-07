#pragma once
#include <afxwin.h>
#include <sstream>

#define MSGBOX(msg,nType)			{ std::wostringstream os; os << ##msg; ::AfxMessageBox(os.str().c_str(), nType); }
#define MSGBOX_ERR(msg)				MSGBOX(msg, MB_OK | MB_ICONERROR)
#define MSGBOX_INFO(msg)			MSGBOX(msg, MB_OK | MB_ICONINFORMATION)
#define MSGBOX_WARN(msg)			MSGBOX(msg, MB_OK | MB_ICONWARNING)
#define MSGBOX_EXCEPTION(e)		{ TCHAR buff[1024] = { 0 }; e->GetErrorMessage(buff, 1023); MSGBOX_ERR(buff); }

