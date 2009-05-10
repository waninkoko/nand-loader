#include <stdio.h>
#include <string.h>
#include <ogcsys.h>

#include "config.h"
#include "patches.h"
#include "video.h"
#include "wpad.h"

/* DOL header structure */
typedef struct {
	u32 textoff[7];
	u32 dataoff[11];

	u8 *textmem[7];
	u8 *datamem[11];

	u32 textsize[7];
	u32 datasize[11];

	u8 *bssmem;
	u32 bsssize;

	u32 entry;

	u32 unused[7];
} dolheader;

/* Entry pointer */
typedef void (*dolentry)(void);

/* Constants */
#define DOL_INDEX	1


void __Loader_SetLowMem(void)
{
	/* Setup low memory */
	*(vu32*)0x800000F8 = 0x0E7BE2C0;
	*(vu32*)0x800000FC = 0x2B73A840;

	/* Flush cache */
	DCFlushRange((void *)(0x80000000), 0x3F00);
}

void __Loader_SetVMode(u64 tid)
{
	GXRModeObj *vmode_ptr = NULL;
	u32         vmode_reg;

	u32 progressive, tvmode;

	/* Get video mode configuration */
	progressive = (CONF_GetProgressiveScan() > 0) && VIDEO_HaveComponentCable();
	tvmode      =  CONF_GetVideo();

	/* Select video mode */
	switch(tid & 0xFF) {
	/* PAL */
	case 'P':
		vmode_reg = (CONF_GetEuRGB60() > 0) ? 5 : 1;

		if (tvmode != CONF_VIDEO_PAL) {
			vmode_reg = 5;
			vmode_ptr = &TVNtsc480IntDf;
		}

		break;

	/* NTSC or unknown */
	case 'E':
	case 'J':
		vmode_reg = 0;

		if (tvmode != CONF_VIDEO_NTSC)
			vmode_ptr = &TVEurgb60Hz480IntDf;

		break;

	default:
		vmode_reg = 0;
	}

	/* Override video mode */
	if (loaderCfg.forcePal50) {
		vmode_reg = 1;
		vmode_ptr = &TVPal528IntDf;
	}

	if (loaderCfg.forcePal60) {
		vmode_reg = 5;
		vmode_ptr = &TVEurgb60Hz480IntDf;
	}

	if (loaderCfg.forceNtsc) {
		vmode_reg = 0;
		vmode_ptr = &TVNtsc480IntDf;
	}

	if (loaderCfg.forceHdtv) {
		vmode_reg = 0;
		vmode_ptr = &TVNtsc480Prog;
	}


	/* Set video mode register */
	*(vu32 *)0x800000CC = vmode_reg;

	/* Set video mode */
	if (vmode_ptr) {
		/* If progressive available, use it */
		if (progressive)
			vmode_ptr = &TVNtsc480Prog;

		Video_Configure(vmode_ptr);
		Video_Clear(COLOR_BLACK);
	}
}


void __Loader_PatchDol(u8 *buffer, u32 len)
{
	struct config *cfg = &loaderCfg;

	/* Video mode patching */
	if (cfg->patchVmode)
		Patch_VideoMode(buffer, len);
}

s32 __Loader_ReadDol(dolentry *p_entry)
{
	static dolheader dol ATTRIBUTE_ALIGN(32);

	u32 cnt;
	s32 cfd = -1, ret;

	/* Open DOL file */
	cfd = ES_OpenContent(DOL_INDEX);
	if (cfd < 0)
		return cfd;

	/* Read DOL header */
	ret = ES_ReadContent(cfd, (u8 *)&dol, sizeof(dol));
	if (ret < 0)
		goto out;

	/* Clear BSS */
	memset(dol.bssmem, 0, dol.bsssize);

	/* Read TEXT section */
	for (cnt = 0; cnt < 7; cnt++) {
		u32 offset = dol.textoff[cnt];

		if (offset >= sizeof(dol)) {
			u8 *buffer = (u8 *)dol.textmem[cnt];
			u32 len    = dol.textsize[cnt];

			/* Seek */
			ES_SeekContent(cfd, offset, 0);

			/* Read */
			ret = ES_ReadContent(cfd, buffer, len);
			if (ret < 0)
				goto out;

			/* Patch */
			__Loader_PatchDol(buffer, len);
		}
	}

	/* Read DATA section */
	for (cnt = 0; cnt < 11; cnt++) {
		u32 offset = dol.dataoff[cnt];

		if (offset >= sizeof(dol)) {
			u8 *buffer = (u8 *)dol.datamem[cnt];
			u32 len    = dol.datasize[cnt];

			/* Seek */
			ES_SeekContent(cfd, offset, 0);

			/* Read */
			ret = ES_ReadContent(cfd, buffer, len);
			if (ret < 0)
				goto out;

			/* Patch */
			__Loader_PatchDol(buffer, len);
		}
	}

	/* Set entry point */
	*p_entry = (dolentry)dol.entry;

out:
	/* Close DOL file */
	if (cfd >= 0)
		ES_CloseContent(cfd);

	return ret;
}

void __Loader_Shutdown(void)
{
	/* Deinitialize ISFS */
	ISFS_Deinitialize();

	/* Shutdown IOS subsystems */
	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
}


s32 Loader_Execute(void)
{
	dolentry p_entry = NULL;

	u64 tid;
	s32 ret;

	/* Get title ID */
	ret = ES_GetTitleID(&tid);
	if (ret < 0)
		return ret;

	/* Load DOL */
	ret = __Loader_ReadDol(&p_entry);
	if (ret < 0)
		return ret;

	/* Set low memory */
	__Loader_SetLowMem();

	/* Set video mode */
	__Loader_SetVMode(tid);

	/* Shutdown subsystems */
	__Loader_Shutdown();

	/* Clear screen */
	Video_Clear(COLOR_BLACK);

	/* Jump to the entry point */
	p_entry();

	return 0;
}
