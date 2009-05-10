#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>

#include "config.h"
#include "title.h"
#include "video.h"
#include "wpad.h"

/* 'Menu' structure */
struct menu {
	/* Entry name */
	char *name;

	/* Entry value */
	bool *value;
};


/* NANDLoader configuration */
struct config loaderCfg ATTRIBUTE_ALIGN(32) = { 0 };

/* Config entries */
struct menu entryList[] = {
	{ "Force PAL50",	&loaderCfg.forcePal50 },
	{ "Force PAL60",	&loaderCfg.forcePal60 },
	{ "Force NTSC",		&loaderCfg.forceNtsc },
	{ "Force HDTV",		&loaderCfg.forceHdtv },
	{ "Patch Video Mode",	&loaderCfg.patchVmode },
};

/* Macros */
#define NB_ENTRIES	(sizeof(entryList) / sizeof(struct menu))


void __Config_Credits(void)
{
	/* Clear console */
	Con_Clear();

	/* Write credits */
	printf("[+] Custom NAND Loader for Wii\n\n");

	printf("    developed by Waninkoko\n");
	printf("    www.teknoconsolas.es\n\n\n");

	printf("[+] Thanks to all my betatesters:\n");
	printf("    - linkinworm\n");
	printf("    - hectorscasa\n");
	printf("    - SoraK05\n");
	printf("    - And the rest... ;-)\n\n");

	printf("[+] Also kudos to:\n\n");
	printf("    - Team Twiizers\n");
	printf("    - SoftMii Team\n");
	printf("    - crediar\n");
	printf("    - WiiGator and kwiirk\n");
	printf("    - Teknoconsolas.es community\n\n");

	printf("\n");
	printf("[+] Press any button to continue...\n");

	/* Wait for any button */
	Wpad_WaitButtons();
}

s32 __Config_GetFilePath(char *outbuf)
{
	static char nandpath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(32);

	u64 tid;
	s32 ret;

	/* Get title ID */
	ret = ES_GetTitleID(&tid);
	if (ret < 0)
		return ret;

	/* Get data directory */
	ret = ES_GetDataDir(tid, nandpath);
	if (ret < 0)
		return ret;

	/* Generate filepath */
	sprintf(outbuf, "%s/nand.cfg", nandpath);

	return 0;
}


s32 Config_Read(void)
{
	char filepath[ISFS_MAXPATH];
	s32  fd, ret;

	/* Get config filepath */
	ret = __Config_GetFilePath(filepath);
	if (ret < 0)
		return ret;

	/* Open config file */
	fd = IOS_Open(filepath, ISFS_OPEN_READ);
	if (fd < 0)
		return fd;

	/* Read config */
	ret = IOS_Read(fd, &loaderCfg, sizeof(loaderCfg));

	/* Close config file */
	IOS_Close(fd);

	return ret;
}

s32 Config_Write(void)
{
	char filepath[ISFS_MAXPATH];
	s32  fd, ret;

	/* Get config filepath */
	ret = __Config_GetFilePath(filepath);
	if (ret < 0)
		return ret;

	/* Create config file */
	ISFS_CreateFile(filepath, 0, ISFS_OPEN_RW, ISFS_OPEN_READ, ISFS_OPEN_READ);

	/* Open config file */
	fd = IOS_Open(filepath, ISFS_OPEN_WRITE);
	if (fd < 0)
		return fd;

	/* Write config */
	ret = IOS_Write(fd, &loaderCfg, sizeof(loaderCfg));

	/* Close config file */
	IOS_Close(fd);

	return ret;
}

void Config_Menu(void)
{
	u32 var = 0;
	s32 selected = 0;

	for (;;) {
		u32 cnt;

		/* Clear console */
		Con_Clear();

		for (cnt = 0; cnt < NB_ENTRIES; cnt++) {
			struct menu *entry = &entryList[cnt];

			/* Print entry */
			printf("\t%2s %-16s : %s\n", (selected == cnt) ? ">>" : "  ", entry->name, (*entry->value) ? "Yes" : "No");
		}

		printf("\n");

		printf("[+] Press UP/DOWN to move cursor.\n");
		printf("    Press LEFT/RIGHT to toggle option.\n\n");

		printf("    Press A button to save and continue...\n");

		u32 buttons = Wpad_WaitButtons();

		/* UP/DOWN buttons */
		if (buttons & WPAD_BUTTON_UP) {
			if ((--selected) <= -1)
				selected = (NB_ENTRIES - 1);
		}
		if (buttons & WPAD_BUTTON_DOWN) {
			if ((++selected) >= NB_ENTRIES)
				selected = 0;
		}

		/* LEFT/RIGHT buttons */
		if (buttons & (WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT))
			*entryList[selected].value ^= 1;

		/* A button */
		if (buttons & WPAD_BUTTON_A)
			break;


		/* Easter Egg */
		if (buttons & WPAD_BUTTON_1) {
			switch (var) {
			case 0:
				var++;
				break;
			case 3:
				/* Show credits screen */
				__Config_Credits();

				var = 0;
				break;
			default:
				var = 0;
			}
		}
		if (buttons & WPAD_BUTTON_2) {
			switch (var) {
			case 1:
			case 2:
				var++;
				break;
			default:
				var = 0;
			}
		}
	}

	/* Write configuration */
	Config_Write();
}
