#ifndef _CONFIG_H_
#define _CONFIG_H_

/* 'Config' structure */
struct config {
	/* Video modes */
	bool forcePal50;
	bool forcePal60;
	bool forceNtsc;
	bool forceHdtv;

	/* Video mode patching */
	bool patchVmode;
};

/* NANDLoader configuration */
extern struct config loaderCfg;

/* Prototypes */
s32  Config_Read(void);
s32  Config_Write(void);
void Config_Menu(void);

#endif

