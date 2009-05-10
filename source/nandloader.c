#include <stdio.h>
#include <unistd.h>
#include <ogcsys.h>

#include "config.h"
#include "gui.h"
#include "loader.h"
#include "sys.h"
#include "video.h"
#include "wpad.h"

/* Constans */
#define RESTART_SECONDS		4


/* Override IOS_LoadStartupIOS */
s32 __IOS_LoadStartupIOS() { return 0; }


void Menu(void)
{
	/* Initialize Wiimote */
	Wpad_Init();

	/* Draw background */
	Gui_DrawBackground();

	/* Initialize console */
	Gui_InitConsole();

	/* Configuration menu */
	Config_Menu();

	/* Destroy console */
	Con_Destroy();

	/* Disconnect Wiimote */
	Wpad_Disconnect();
}

int main(int argc, char **argv)
{
	s32 ret;

	/* Initialize subsystems */
	Sys_Init();

	/* Set video mode */
	Video_SetMode();

	/* Initialize ISFS */
	ISFS_Initialize();
 
	/* Read config */
	Config_Read();

	/* Config menu */
	ret = SYS_ResetButtonDown();
	if (ret)
		Menu();

	/* Draw loading image */
	Gui_DrawLoading();

	/* Execute application */
	Loader_Execute();

	/* Draw error image */
	Gui_DrawError();

	/* Sleep */
	sleep(RESTART_SECONDS);

	/* Load System Menu */
	Sys_LoadMenu();

	return 0;
}
