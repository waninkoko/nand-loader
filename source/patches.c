#include <stdio.h>
#include <string.h>
#include <ogcsys.h>

#include "config.h"

GXRModeObj *PAL2NTSC[] = {
	&TVMpal480IntDf,	&TVNtsc480IntDf,
	&TVPal264Ds,		&TVNtsc240Ds,
	&TVPal264DsAa,		&TVNtsc240DsAa,
	&TVPal264Int,		&TVNtsc240Int,
	&TVPal264IntAa,		&TVNtsc240IntAa,
	&TVPal524IntAa,		&TVNtsc480IntAa,
	&TVPal528Int,		&TVNtsc480IntAa,
	&TVPal528IntDf,		&TVNtsc480IntDf,
	&TVPal574IntDfScale,	&TVNtsc480IntDf,
	&TVEurgb60Hz240Ds,	&TVNtsc240Ds,
	&TVEurgb60Hz240DsAa,	&TVNtsc240DsAa,
	&TVEurgb60Hz240Int,	&TVNtsc240Int,
	&TVEurgb60Hz240IntAa,	&TVNtsc240IntAa,
	&TVEurgb60Hz480Int,	&TVNtsc480IntAa,
	&TVEurgb60Hz480IntDf,	&TVNtsc480IntDf,
	&TVEurgb60Hz480IntAa,	&TVNtsc480IntAa,
	&TVEurgb60Hz480Prog,	&TVNtsc480Prog,
	&TVEurgb60Hz480ProgSoft,&TVNtsc480Prog,
	&TVEurgb60Hz480ProgAa,  &TVNtsc480Prog,
	NULL,			NULL
};

GXRModeObj *NTSC2PAL[] = {
	&TVNtsc240Ds,		&TVPal264Ds,		
	&TVNtsc240DsAa,		&TVPal264DsAa,
	&TVNtsc240Int,		&TVPal264Int,
	&TVNtsc240IntAa,	&TVPal264IntAa,
	&TVNtsc480IntDf,	&TVPal528IntDf,
	&TVNtsc480IntAa,	&TVPal524IntAa,
	&TVNtsc480Prog,		&TVPal528IntDf,
	NULL,			NULL
};

GXRModeObj *NTSC2PAL60[]={
	&TVNtsc240Ds,		&TVEurgb60Hz240Ds,		
	&TVNtsc240DsAa,		&TVEurgb60Hz240DsAa,
	&TVNtsc240Int,		&TVEurgb60Hz240Int,
	&TVNtsc240IntAa,	&TVEurgb60Hz240IntAa,
	&TVNtsc480IntDf,	&TVEurgb60Hz480IntDf,
	&TVNtsc480IntAa,	&TVEurgb60Hz480IntAa,
	&TVNtsc480Prog,		&TVEurgb60Hz480Prog,
	NULL,			NULL
};


void Patch_VideoMode(u8 *buffer, u32 len)
{
	GXRModeObj **table = NULL;

	u32 cnt, idx;

	/* Select conversion table */
	if (loaderCfg.forcePal50)
		table = NTSC2PAL;
	if (loaderCfg.forcePal60)
		table = NTSC2PAL60;
	if (loaderCfg.forceNtsc)
		table = PAL2NTSC;

	/* No table selected */
	if (!table)
		return;

	for (cnt = 0; cnt < len; cnt += 4) {
		u8 *ptr = buffer + cnt;

		/* Search for video modes */
		for (idx = 0; table[idx]; idx += 2) {
			/* Patch video mode */
			if (!memcmp(ptr, table[idx], sizeof(GXRModeObj)))
				memcpy(ptr, table[idx+1], sizeof(GXRModeObj));
		}
	}
}

// void __Patch_VideoMode(u8 *buffer, u32 len)
// {
// 	const u8 pattern[] = { 0x54, 0x00, 0xFF, 0xFE };
// 
// 	u64 tid;
// 	u32 cnt;
// 
// 	/* Get title ID */
// 	ES_GetTitleID(&tid);
// 
// 	for (cnt = 0; cnt < len; cnt += 4) {
// 		u32 *ptr = (u32 *)(buffer + cnt);
// 
// 		/* Pattern found */
// 		if (!memcmp(ptr, pattern, sizeof(pattern))) {
// 			u8 region = tid & 0xFF;
// 
// 			/* USA region */
// 			if (region == 'E')
// 				*ptr = 0x38000001;
// 			/* JAP region */
// 			if (region == 'J')
// 				*ptr = 0x38000000;
// 
// 			break;
// 		}
// 	}
// }
// 
// 
// void Patch_VideoMode(u8 *buffer, u32 len)
// {
// 	const u8 pattern[] = { 0x41, 0x82, 0x00, 0x0C, 0x41, 0x80, 0x00, 0x1C, 0x48, 0x00, 0x00, 0x18 };
// 
// 	u32 cnt;
// 
// 	for (cnt = 0; cnt < len; cnt += 4) {
// 		u8 *ptr = buffer + cnt;
// 
// 		/* Pattern found */
// 		if (!memcmp(ptr, pattern, sizeof(pattern)))
// 			__Patch_VideoMode(ptr, len - cnt);
// 	}
// }
