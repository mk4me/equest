#include <coreui/KioskUtilities.h>
#include "QtCore/QCommandLineParser"

#ifdef _WINDOWS

#include <windows.h>
#include <winuser.h>

namespace {

	HHOOK g_hKeyboardHook;
	LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
		// By returning a non-zero value from the hook procedure, the
		// message does not get passed to the target window
		KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;
		BOOL bControlKeyDown = 0;

		switch (nCode) {
		case HC_ACTION:
		{
			// Check to see if the CTRL key is pressed
			bControlKeyDown = GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);

			// Disable CTRL+ESC
			if (pkbhs->vkCode == VK_ESCAPE && bControlKeyDown)
				return 1;

			// Disable ALT+TAB
			if (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN)
				return 1;

			// Disable ALT+ESC
			if (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags & LLKHF_ALTDOWN)
				return 1;
			// Disable ALT+F4
			if (pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN)
				return 1;

			if (pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN)
				return 1;

			break;
		}

		default:
			break;
		}
		return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
	}

	void block() {
		if (g_hKeyboardHook) {
			throw std::runtime_error("Keyboard already blocked");
		}
		g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
	}

	void unblock() {
		if (!g_hKeyboardHook) {
			throw std::runtime_error("Keyboard was not blocked");
		}
		UnhookWindowsHookEx(g_hKeyboardHook);
		g_hKeyboardHook = nullptr;
	}
}
#else
#endif
#include "corelib/PluginCommon.h"

void coreUI::KioskUtilities::blockSpecialKeyCombinations()
{
	block();
}

void coreUI::KioskUtilities::unblockSpecialKeyCombinations()
{
	unblock();
}

void coreUI::KioskUtilities::startKioskMode(additionalOperations f)
{
	f();
	blockSpecialKeyCombinations();
}

void coreUI::KioskUtilities::startKioskModeIfParamIsSet(additionalOperations f)
{
	auto args = QCoreApplication::arguments();
	QCommandLineParser parser;
	QCommandLineOption kioskOption("kiosk", "Kiosk mode");
	parser.addOption(kioskOption);
	if (!parser.parse(args)) {
		PLUGIN_LOG_INFO(parser.errorText().toStdString());
	}

	if (parser.isSet(kioskOption)) {
		startKioskMode(f);
	}
}

void coreUI::KioskUtilities::stopKioskMode(additionalOperations f)
{
	f();
	unblockSpecialKeyCombinations();
}

void coreUI::KioskUtilities::safeStopKioskMode(additionalOperations f)
{
	try {
		stopKioskMode(f);
	}
	catch (...) {

	}
}

coreUI::ScopedKiosk::ScopedKiosk(KioskUtilities::additionalOperations startOp, KioskUtilities::additionalOperations stopOp, bool onlyIfParam) :
	stopOperation(stopOp)
{
	if (onlyIfParam) {
		KioskUtilities::startKioskModeIfParamIsSet(startOp);
	} else {
		KioskUtilities::startKioskMode(startOp);
	}
}

coreUI::ScopedKiosk::~ScopedKiosk()
{
	KioskUtilities::safeStopKioskMode(stopOperation);
}
