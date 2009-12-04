/*
constitle.cpp

��������� �������
*/
/*
Copyright (c) 1996 Eugene Roshal
Copyright (c) 2000 Far Group
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "headers.hpp"
#pragma hdrstop

#include "constitle.hpp"
#include "lang.hpp"
#include "language.hpp"
#include "interf.hpp"
#include "config.hpp"
#include "ctrlobj.hpp"

static string& GetFarTitleAddons();

ConsoleTitle::ConsoleTitle(const wchar_t *title)
{
	apiGetConsoleTitle(strOldTitle);

	if (title)
		ConsoleTitle::SetFarTitle(title);
}

ConsoleTitle::~ConsoleTitle()
{
	wchar_t *lpwszTitle = strOldTitle.GetBuffer();

	if (*lpwszTitle)
	{
		string titleAddons = GetFarTitleAddons();

		lpwszTitle += StrLength(lpwszTitle);
		lpwszTitle -= StrLength(titleAddons);

		if (!StrCmpI(lpwszTitle, titleAddons))
			*lpwszTitle = 0;
	}

	strOldTitle.ReleaseBuffer();
	ConsoleTitle::SetFarTitle(strOldTitle);
}

void ConsoleTitle::Set(const wchar_t *fmt,...)
{
	wchar_t msg[2048];
	va_list argptr;
	va_start(argptr, fmt);
	vsnwprintf(msg, countof(msg)-1, fmt, argptr);
	va_end(argptr);
	ConsoleTitle::SetFarTitle(msg);
}

void ConsoleTitle::SetFarTitle(const wchar_t *Title)
{
	static string strFarTitle;
	string strOldFarTitle;

	if (Title)
	{
		apiGetConsoleTitle(strOldFarTitle);
		strFarTitle=Title;
		strFarTitle.SetLength(0x100);
		strFarTitle+=GetFarTitleAddons();
		TitleModified=TRUE;

		if (StrCmp(strOldFarTitle, strFarTitle) &&
		        ((CtrlObject->Macro.IsExecuting() && !CtrlObject->Macro.IsDsableOutput()) ||
		         !CtrlObject->Macro.IsExecuting() || CtrlObject->Macro.IsExecutingLastKey()))
		{
			SetConsoleTitle(strFarTitle);
			TitleModified=FALSE;
		}
	}
	else
	{
		/*
			Title=NULL ��� ����, ����� �㦭� ���⠢��� �।.���������
			SetFarTitle(NULL) - �� �� ��� ���!
			��� �맮� ����� �ࠢ� ������ ⮫쪮 ����-������!
		*/
		SetConsoleTitle(strFarTitle);
		TitleModified=FALSE;
		//_SVS(SysLog(L"  (NULL)FarTitle='%s'",FarTitle));
	}
}

static string& GetFarTitleAddons()
{
	// " - Far%Ver%Admin"
	/*
		%Ver   - 2.0
		%Build - 1259
		%Admin - MFarTitleAddonsAdmin
    */
	static string TitleAddons = Opt.TitleAddons;
	string Ver, Build;

	Ver.Format(L" %u.%u",HIBYTE(LOWORD(FAR_VERSION)),LOBYTE(LOWORD(FAR_VERSION)));
	Build.Format(L" %u",HIWORD(FAR_VERSION));

	ReplaceStrings(TitleAddons,L"%Ver",Ver);
	ReplaceStrings(TitleAddons,L"%Build",Build);
	ReplaceStrings(TitleAddons,L"%Admin",(Opt.IsUserAdmin && WinVer.dwMajorVersion >= 6?MSG(MFarTitleAddonsAdmin):L""));

	return TitleAddons;
}
