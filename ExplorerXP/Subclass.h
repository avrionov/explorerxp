/* Copyright 2002-2021 Nikolay Avrionov. All Rights Reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#ifndef _SUBCLASSW_H
#define _SUBCLASSW_H


#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
	#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))
#endif

class CSubclassWnd;

class ISubclassCallback
{
public:
	virtual void PostNcDestroy(HWND hWnd) = 0;
};

class CSubclassWnd
{
	friend class CSubclassWndMap;
	friend class CSubclasser;

public:
	CSubclassWnd();
	virtual ~CSubclassWnd();

	virtual BOOL HookWindow(HWND hRealWnd, CSubclasser* pSubclasser = NULL);
	virtual BOOL IsValid() const { return IsValidHook(); }

	inline HWND GetHwnd() const { return m_hWndHooked; }
	inline CWnd* GetCWnd() const { return CWnd::FromHandle(m_hWndHooked); }

	static void SetCallback(ISubclassCallback* pCallback) { s_pCallback = pCallback; }

protected:
	HWND			m_hWndHooked;		// the window hooked
	WNDPROC			m_pOldWndProc;		// ..and original window proc
	CSubclassWnd*	m_pNext;			// next in chain of hooks for this window
	CSubclasser*	m_pSubclasser;
	BOOL			m_bPreMFCSubclass;

	static ISubclassCallback* s_pCallback;

protected:
	// this is called only when m_hWndHooked is detached as a result
	// of receiving WM_NCDESTROY else HookWindow(NULL) was called
	virtual void PreDetachWindow() { }
	virtual void PostDetachWindow() { }

	// Subclass a window. Hook(NULL) to unhook (automatic on WM_NCDESTROY)
	virtual BOOL IsHooked() const { return m_hWndHooked != NULL; }
	virtual BOOL IsValidHook() const { return ::IsWindow(m_hWndHooked); }

	inline DWORD GetExStyle() const { return ::GetWindowLong(m_hWndHooked, GWL_EXSTYLE); }
	inline DWORD GetStyle() const { return ::GetWindowLong(m_hWndHooked, GWL_STYLE); }
	inline HWND GetParent() const { return ::GetParent(m_hWndHooked); }
	inline void GetClientRect(LPRECT pRect) const { ::GetClientRect(m_hWndHooked, pRect); }
	inline void GetWindowRect(LPRECT pRect) const { ::GetWindowRect(m_hWndHooked, pRect); }
	inline void Invalidate(BOOL bErase = TRUE) const { ::InvalidateRect(m_hWndHooked, NULL, bErase); }
	inline BOOL IsWindowEnabled() const { return ::IsWindowEnabled(m_hWndHooked); }
	inline BOOL IsWindowVisible() const { return ::IsWindowVisible(m_hWndHooked); }

	void ClientToWindow(LPRECT pRect) const;
	void ScreenToClient(LPRECT pRect) const;
	void ClientToScreen(LPRECT pRect) const;
	void ScreenToWindow(LPRECT pRect) const;
	void ClientToWindow(LPPOINT pPoint) const;
	void ScreenToClient(LPPOINT pPoint) const;
	void ClientToScreen(LPPOINT pPoint) const;
	void ScreenToWindow(LPPOINT pPoint) const;

	void SetRedraw(BOOL bRedraw = TRUE) const { ::SendMessage(m_hWndHooked, WM_SETREDRAW, bRedraw, 0); }
	virtual void Redraw() const { Invalidate(); }

	//virtual BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;
	virtual LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;

	static LRESULT CALLBACK HookWndProc(HWND, UINT, WPARAM, LPARAM);

	static CMapPtrToPtr& GetValidMap(); // map containing every CSubclassWnd
	static BOOL IsValid(const CSubclassWnd* pScWnd);

	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT Default(); // call this at the end of handler fns if you are happy with the defaults

};

class CSubclasser
{
	friend class CSubclassWnd;
	friend class CSubclassWndMap;

protected:
	CSubclasser() {}

	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		ASSERT(hRealWnd == m_subclass.GetHwnd()); 
		return m_subclass.WindowProc(hRealWnd, msg, wp, lp); 
	}

	virtual CSubclasser* GetTopSubclasser() { return this; }

	// this is called only when m_subclass.m_hWndHooked is detached as a result
	// of receiving WM_NCDESTROY else m_subclass.HookWindow(NULL) was called
	virtual void ScPreDetachWindow() { }
	virtual void ScPostDetachWindow() { }

	// Subclass a window. Hook(NULL) to unhook (automatic on WM_NCDESTROY)
	inline operator HWND() const { return m_subclass.GetHwnd(); }
	inline BOOL ScHookWindow(HWND hWnd) { return m_subclass.HookWindow(hWnd, GetTopSubclasser()); }
	inline BOOL ScIsHooked() { return m_subclass.IsHooked(); }
	inline BOOL ScIsValidHook() { return m_subclass.IsValidHook(); }
	inline CWnd* ScGetCWnd() { return m_subclass.GetCWnd(); }
	inline HWND ScGetHwnd() { return m_subclass.GetHwnd(); }

	//inline BOOL ScPostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	//	{ return m_subclass.PostMessage(message, wParam, lParam); }

	LRESULT ScSendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
		{ return m_subclass.SendMessage(message, wParam, lParam); }

protected:
	CSubclassWnd m_subclass;

protected:
	LRESULT ScDefault(HWND hRealWnd) 
	{ 
		ASSERT(hRealWnd == m_subclass.GetHwnd()); 
		return m_subclass.Default(); 
	} // in time we will have mutiple subclassed wnds
};

#endif // _SUBCLASSW_H

