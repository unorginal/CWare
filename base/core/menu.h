// used: std::function
#include <functional>

// used: winapi, directx, imgui, fmt includes
#include "../common.h"

class CTab
{
public:
	const char* szName = nullptr;
	std::function<void()> pRenderFunction = nullptr;
};

/*
 * WINDOWS
 */
namespace W
{
	// Get
	/* main cheat window */
	void MainWindow(IDirect3DDevice9* pDevice);

	// Values
	// @note: add smth like Vars.iMainExtra to show extra functions in menu e.g. hitchance precision and other (if disabled, will be used default optimal values)
	/* open state of main window */
	inline bool bMainOpened = false;
	/* current selected tab in main window */
	inline int iMainTab = 0;
}

/*
 * TABS
 */
namespace T
{
	// Get
	/* render tabs selection */
	void RageBot();
	void LegitBot();
	void Welcome();
	void Visuals();
	void Miscellaneous();
	void AntiAim();
	void Lua();
	void MenuSettings();
	void SkinChanger();

	// Values
	/* current selected sub-tab in visuals tab */
	inline int			iEspTab = 0;
	/* user-defined configuration filename in miscellaneous tab */
	inline std::string	szConfigFile = { };
	/* current selected configuration in miscellaneous tab */
	inline int			iSelectedConfig = 0;
	/* current selected color setting in miscellaneous tab */
	inline int			iSelectedColor = 0;
}
