#include <Windows.h>
#include <powrprof.h>
#include <ScrnSave.h>

#include "win/win.h"
#include "resource.h"
#include "common.h"

#pragma comment(lib, "Scrnsave.lib")
#pragma comment(lib, "powrprof.lib")

#pragma warning(disable: 4312)

win win::g_win{};
typedef VOID(WINAPI* PWDCHANGEPASSWORD)(LPCSTR lpcRegkeyname, HWND hwnd, UINT uiReserved1, UINT uiReserved2);
typedef BOOL(WINAPI* VERIFYSCREENSAVEPWD)(HWND hwnd);
ScrMode g_scrmode;
BOOL g_fCheckingPassword = FALSE;
BOOL g_fClosing = FALSE;
BOOL g_fOnWin95 = FALSE;
char g_szClassName[] = "TesseractWindowClass";
HINSTANCE g_hInstPwdDLL;
HINSTANCE g_hInstance;

UINT uShellMessage;
VERIFYSCREENSAVEPWD g_verifyScreenSavePwd;

void doChangePwd(HWND hwnd);
void doConfig(HWND hwnd);
void doSaver(HWND hwndParent);
void hogMachine(BOOL fDisable);
void loadPwdDLL(void);
void unloadPwdDLL(void);
LRESULT CALLBACK RealScreenSaverProc(HWND hwnd, int iMsg, WPARAM wparam, LPARAM lparam);

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_GETMINMAXINFO: {
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y =
			GetSystemMetrics(SM_CYMAXTRACK) +
			GetSystemMetrics(SM_CYCAPTION) +
			GetSystemMetrics(SM_CYMENU) +
			GetSystemMetrics(SM_CYBORDER) * 2;
		return 0;
	}
	case WM_CREATE: {
		win::g_win.init(hWnd);
		return 0;
	}
	case WM_ERASEBKGND: {
		return 0;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		win::g_win.draw(hDC);
		EndPaint(hWnd, &ps);
		InvalidateRect(hWnd, NULL, true);
		return 0;
	}
	case WM_SIZE: {
		win::g_win.resize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_DESTROY: {
		win::g_win.deinit();
		return 0;
	}
	case WM_MOUSEMOVE: {
		if (g_scrmode != ScrMode::smSaver) {
			break;
		}
		int32_t x = LOWORD(lParam);
		int32_t y = HIWORD(lParam);
		RECT rc;
		GetWindowRect(hWnd, &rc);
		if (x * 2 != (rc.right - rc.left) || y * 2 != (rc.bottom - rc.top)) {
			win::g_win.deinit();
		}
		return 0;
	}
	case WM_KEYDOWN: {
		if (g_scrmode != ScrMode::smSaver) {
			break;
		}
		win::g_win.deinit();
		return 0;
	}
	case WM_TIMER: {
		if (g_scrmode != ScrMode::smSaver) {
			break;
		}
		if (wParam == 0x71334) {
			win::g_win.draw(GetDC(hWnd));
			bool closed = false;
			SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &closed, 0);
			if (closed) {
				win::g_win.deinit();
			}
			return 0;
		}
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WINAPI DefScreenSaverProc(HWND hwnd, UINT iMsg, WPARAM wparam, LPARAM lparam) {
	if (iMsg == uShellMessage)
	{
		if (uShellMessage == 0)
			return DefWindowProc(hwnd, iMsg, wparam, lparam);

		PostMessage(hwnd, WM_CLOSE, 0, 0);

		return (g_verifyScreenSavePwd) ? 1 : 0;
	}

	if (g_scrmode != ScrMode::smPreview && !g_fClosing) {
		switch (iMsg)
		{
		case WM_ACTIVATEAPP:
			if (wparam == FALSE)
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_CLOSE:
			if (!g_fOnWin95)
				break;
			if (!g_verifyScreenSavePwd) {
				g_fClosing = TRUE;
				break;
			}
			g_fCheckingPassword = TRUE;
			g_fClosing = g_verifyScreenSavePwd(hwnd);
			g_fCheckingPassword = FALSE;
			if (g_fClosing) {
				break;
			}
			return 0;
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_POWERBROADCAST:
		case WM_RBUTTONDOWN:
		case WM_SYSKEYDOWN:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_MOUSEMOVE:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_POWER:
			if (wparam == PWR_CRITICALRESUME) {
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_SETCURSOR:
			SetCursor(g_fCheckingPassword ? LoadCursor(NULL, IDC_ARROW) : NULL);
			return -1;
		}
	}

	return DefWindowProc(hwnd, iMsg, wparam, lparam);
}

static void doChangePwd(HWND hwnd) {
	if (!g_fOnWin95) {
		return;
	}
	HINSTANCE hInstMPRDLL = LoadLibrary("mpr.dll");
	if (hInstMPRDLL == NULL) {
		return;
	}
	PWDCHANGEPASSWORD pwdChangePassword = (PWDCHANGEPASSWORD)GetProcAddress(hInstMPRDLL, "PwdChangePasswordA");
	if (pwdChangePassword != NULL) {
		(*pwdChangePassword) ("SCRSAVE", hwnd, 0, 0);
	}
	FreeLibrary(hInstMPRDLL);
}

static void doConfig(HWND hwnd) {
	if (RegisterDialogClasses(g_hInstance)) {
		DialogBoxParam(g_hInstance, MAKEINTRESOURCE(DLG_SCRNSAVECONFIGURE), hwnd, (DLGPROC)ScreenSaverConfigureDialog, 0);
	}
}

static void doSaver(HWND pWndParent) {
	HWND hWnd;
	WNDCLASS wc;
	PCSTR pszWindowTitle = "Tesseract";
	UINT uExStyle = 0, uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;

	wc.style = CS_OWNDC | CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)RealScreenSaverProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;

	HWND hOther = FindWindow(g_szClassName, pszWindowTitle);
	if (hOther != NULL && IsWindow((HWND)hOther)) {
		SetForegroundWindow(hOther);
		return;
	}
	
	if (pWndParent != NULL) {
		uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
		uExStyle = 0;
		pszWindowTitle = "Preview";
	} else {
		uStyle = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		uExStyle = WS_EX_TOPMOST;
	}

	if (!RegisterClass(&wc)) {
		return;
	}

	if (g_fOnWin95) {
		loadPwdDLL();
		uShellMessage = RegisterWindowMessage("QueryCancelAutoPlay");
	}
	hogMachine(TRUE);

	
	RECT rc;
	if (pWndParent) {
		GetClientRect(pWndParent, &rc);
	} else {
		HDC hdc = GetDC(HWND_DESKTOP);
		GetClipBox(hdc, &rc);
		ReleaseDC(HWND_DESKTOP, hdc);
	}
	
	hWnd = CreateWindowEx(uExStyle, g_szClassName,
		pszWindowTitle, uStyle,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		pWndParent, NULL, g_hInstance, NULL);

	if (hWnd) {
		SetTimer(hWnd, 0x71334, 1, nullptr);

		if (g_scrmode != ScrMode::smPreview) {
			SetForegroundWindow(hWnd);
		}

		MSG message;
		while (true) {
			if (PeekMessage(&message, hWnd, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
				if (win::g_win.is_closed()) {
					break;
				}
			} else {
				PAINTSTRUCT ps;
				HDC hDC = BeginPaint(pWndParent ? pWndParent : hWnd, &ps);
				win::g_win.draw(hDC);
				EndPaint(pWndParent ? pWndParent : hWnd, &ps);
			}
		}
		KillTimer(hWnd, 0x71334);
	}

	hogMachine(FALSE);

	if (g_fOnWin95) {
		unloadPwdDLL();
	}
}

static void hogMachine(BOOL fDisable) {
	UINT uOldValue;
	if (!g_fOnWin95) {
		return;
	}
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, fDisable, &uOldValue, 0);
}

static void loadPwdDLL(void) {
	HKEY hkey;
	if (RegOpenKey(HKEY_CURRENT_USER, "Control Panel\\Desktop", &hkey)) {
		return;
	}

	DWORD dwVal;
	DWORD dwSize = sizeof(DWORD);
	if (RegQueryValueEx(hkey, "ScreenSaveUsePassword", NULL, NULL, (LPBYTE)&dwVal, &dwSize)) {
		RegCloseKey(hkey);
		return;
	}
	RegCloseKey(hkey);
	if (dwVal != 0) {
		g_hInstPwdDLL = LoadLibrary("password.cpl");
		if (!g_hInstPwdDLL) {
			return;
		}
		g_verifyScreenSavePwd = (VERIFYSCREENSAVEPWD)GetProcAddress(g_hInstPwdDLL, "VerifyScreenSavePwd");
		if (g_verifyScreenSavePwd != NULL) {
			return;
		}
		unloadPwdDLL();
	}
}

static void unloadPwdDLL(void) {
	if (!g_hInstPwdDLL) {
		return;
	}
	FreeLibrary(g_hInstPwdDLL);
	g_hInstPwdDLL = NULL;
	g_verifyScreenSavePwd = NULL;
}

static LRESULT CALLBACK RealScreenSaverProc(HWND hwnd, int iMsg, WPARAM wparam, LPARAM lparam) {
	switch (iMsg)
	{
	case WM_CONTEXTMENU:
	case WM_HELP:
		if (g_scrmode == ScrMode::smPreview) {
			HWND hwndParent = GetParent(hwnd);
			if (hwndParent == NULL || !IsWindow(hwndParent))
				return 1;
			PostMessage(hwndParent, iMsg, (WPARAM)hwndParent, lparam);
			return 1;
		}
		break;
	case WM_CREATE:
		if (g_scrmode != ScrMode::smPreview) {
			SetCursor(NULL);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	}

	return ScreenSaverProc(hwnd, iMsg, wparam, lparam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst) {
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hinstancePrev, LPSTR lpszCmdLine, int iCmdShow)
{
	char* pch;
	HWND hWnd = (HWND)INVALID_HANDLE_VALUE;
	OSVERSIONINFOEX osvi;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	osvi.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
	g_fOnWin95 = VerifyVersionInfo(&osvi, VER_PLATFORMID, VerSetConditionMask(0, VER_PLATFORMID, VER_GREATER_EQUAL));

	
	try {
		g_hInstance = hinstance;
		pch = GetCommandLine();
	
		if (*pch == '\"') {
			pch++;
			while (*pch != '\0' && *pch != '\"') {
				pch++;
			}
		} else {
			while (*pch != '\0' && *pch != ' ') {
				pch++;
			}
		}

		if (*pch != '\0') {
			pch++;
		}

		while (*pch == ' ') {
			pch++;
		}

		if (*pch == '\0') {
			g_scrmode = ScrMode::smConfig;
			hWnd = GetForegroundWindow();
		} else {
			if (*pch == '-' || *pch == '/') {
				pch++;
			}
			if (*pch == 'a' || *pch == 'A') {
				g_scrmode = ScrMode::smPassword;
				pch++;
				while (*pch == ' ' || *pch == ':') {
					pch++;
				}
				hWnd = (HWND)atoi(pch);
			}
			else if (*pch == 'c' || *pch == 'C') {
				g_scrmode = ScrMode::smConfig;
				pch++;
				while (*pch == ' ' || *pch == ':') {
					pch++;
				}
				if (*pch == '\0') {
					hWnd = GetForegroundWindow();
				}
				else {
					hWnd = (HWND)atoi(pch);
				}
			}
			else if (*pch == 'p' || *pch == 'P' || *pch == 'l' || *pch == 'L')
			{
				g_scrmode = ScrMode::smPreview;
				pch++;
				while (*pch == ' ' || *pch == ':') {
					pch++;
				}
				hWnd = (HWND)atoi(pch);
			}
			else if (*pch == 's' || *pch == 'S') {
				g_scrmode = ScrMode::smSaver;
			}
		}
		if (g_scrmode == ScrMode::smConfig) {
			doConfig(hWnd);
		}
		else if (g_scrmode == ScrMode::smPassword) {
			doChangePwd(hWnd);
		}
		else if (g_scrmode == ScrMode::smPreview) {
			doSaver(hWnd);
		}
		else if (g_scrmode == ScrMode::smSaver) {
			doSaver(NULL);
		}
	} catch(...) {
		hogMachine(FALSE);
	}
	return 0;
}
