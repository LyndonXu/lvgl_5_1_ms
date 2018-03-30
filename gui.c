/*
 * gui.c
 *
 *  Created on: 2018年3月26日
 *      Author: Lyndon
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "lvgl/lvgl.h"

extern lv_font_t lv_font_chs_20;
extern lv_font_t lv_font_chs_24;

char *GetFakeUnicodeForCH(const char *pStr, int32_t s32StrLen);
#define CHS_TO_UN(pStr)			GetFakeUnicodeForCH(pStr, -1)

typedef enum _tagEmAudioCtrlMode
{
	_Audio_Ctrl_Mode_Normal = 0,	/* left to left, right to right */
	_Audio_Ctrl_Mode_ShieldLeft,
	_Audio_Ctrl_Mode_ShieldRight,
	_Audio_Ctrl_Mode_ShieldLeftAndRight,
	_Audio_Ctrl_Mode_LeftToRight,
	_Audio_Ctrl_Mode_RightToLeft,
	_Audio_Ctrl_Mode_Mux,

	_Audio_Ctrl_Mode_Reserved,
}EmAudioCtrlMode;

typedef struct _tagStVolumeCtrlGroup
{
	lv_obj_t *pLeftVolume;
	lv_obj_t *pRightVolume;
	lv_obj_t *pCtrlMode;
	lv_obj_t *pUniformVolume;
	uint16_t u16XPos;

	uint8_t u8Index;
	uint8_t u8MaxCtrlMode;
	const uint8_t *pCtrlModeIndex;
	const char *pTitle;
	bool boIsFixUniformVoume;

}StVolumeCtrlGroup;

const char *c_pCtrlMode[_Audio_Ctrl_Mode_Reserved] =
{
	"Normal",
	"L Mute",
	"R Mute",
	"Mute",
	"R Use L",
	"L Use R",
	"Mux",
};/**/

static lv_theme_t *s_pTheme = NULL;

lv_signal_func_t s_pOrgSliderFun = NULL;
lv_res_t SignalSlider(struct _lv_obj_t * obj,
		lv_signal_t sign, void * param)
{
	if (s_pOrgSliderFun != NULL)
	{
		uint16_t u16OldValue = lv_slider_get_value(obj);
		uint16_t u16NewValue;
		lv_res_t res = s_pOrgSliderFun (obj, sign, param);
		if (res != LV_RES_OK)
		{
			return res;
		}

		if(sign == LV_SIGNAL_PRESSING || sign == LV_SIGNAL_RELEASED
				||sign == LV_SIGNAL_PRESS_LOST)
		{
	        u16NewValue = lv_slider_get_value(obj);
	        if (u16NewValue != u16OldValue)
	        {
				printf("slider signal(%d) value is: %d\n", sign, lv_slider_get_value(obj));
	        	StVolumeCtrlGroup *pGroup = lv_obj_get_free_ptr(obj);
	        	if (pGroup->boIsFixUniformVoume|| lv_cb_is_checked(pGroup->pUniformVolume))
	        	{
	        		if (obj == pGroup->pLeftVolume)
	        		{
	        			lv_slider_set_value(pGroup->pRightVolume, u16NewValue);
	        		}
	        		else
	        		{
	        			lv_slider_set_value(pGroup->pLeftVolume, u16NewValue);
	        		}
	        	}

	        }
		}
	}
	return LV_RES_OK;
}

lv_res_t ActionUniformCB(struct _lv_obj_t * obj)
{
	StVolumeCtrlGroup *pGroup = lv_obj_get_free_ptr(obj);
	(void)pGroup;

	printf("the %dth check box is: %s\n", pGroup->u8Index, 
		lv_cb_is_checked(obj) ? "check" : "uncheck");
	return LV_RES_OK;
}

lv_res_t ActionCtrlModeDDlist(struct _lv_obj_t * obj)
{
	StVolumeCtrlGroup *pGroup = lv_obj_get_free_ptr(obj);
	(void)pGroup;

	printf("the %dth ddlist number is: %s\n", pGroup->u8Index,
		c_pCtrlMode[pGroup->pCtrlModeIndex[lv_ddlist_get_selected(obj)]]);
	return LV_RES_OK;

}



int32_t CreateVolumeCtrlGroup(
		lv_obj_t *pParent,
		lv_obj_t *pGlobalGroup,
		uint16_t u16XPos, 

		StVolumeCtrlGroup *pGroup,
		uint8_t u8Index,
		const uint8_t *pCtrlModeIndex,
		uint8_t u8MaxCtrlMode,
		const char *pTitle,
		bool boIsFixUniformVoume)
{
	lv_obj_t *pObjTmp;
	if ((pGroup == NULL) || (pParent == NULL) || (pCtrlModeIndex == NULL))
	{
		return -1;
	}

	if (u16XPos > 800)
	{
		return -1;
	}

	memset(pGroup, 0, sizeof(StVolumeCtrlGroup));

	if (u8MaxCtrlMode > (uint8_t)_Audio_Ctrl_Mode_Reserved)
	{
		u8MaxCtrlMode = (uint8_t)_Audio_Ctrl_Mode_Reserved;
	}
	pGroup->u16XPos = u16XPos;

	pGroup->u8Index = u8Index;
	pGroup->u8MaxCtrlMode = u8MaxCtrlMode;
	pGroup->pCtrlModeIndex = pCtrlModeIndex;
	pGroup->pTitle = pTitle;

	{/* control mode object */
		char c8Str[96];
		uint32_t i;
		c8Str[0] = 0;
		for (i = 0; i < u8MaxCtrlMode; i++)
		{
			if (i != 0)
			{
				strcat(c8Str, "\n");
			}
			strcat(c8Str, c_pCtrlMode[pGroup->pCtrlModeIndex[i]]);
		}

		pObjTmp = lv_ddlist_create(pParent, NULL);

		if (pObjTmp == NULL)
		{
			goto err;
		}
		pGroup->pCtrlMode = pObjTmp;

		lv_obj_set_pos(pObjTmp, u16XPos, 290);
		//lv_obj_align(pObjTmp, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, u16XPos, 290);

		lv_ddlist_set_options(pObjTmp, c8Str);

	    lv_ddlist_set_fix_height(pObjTmp, LV_DPI);
	    //lv_ddlist_set_hor_fit(pObjTmp, false);


		//lv_obj_set_width(pObjTmp, 300);


#if 1
		lv_label_set_align(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
				LV_LABEL_ALIGN_CENTER);

		/*
		 * on change, some wrong happened
		lv_label_set_body_draw(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
				true);*/

		lv_label_set_long_mode(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
				LV_LABEL_LONG_BREAK);

		lv_obj_set_width(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
				150);
#endif
	}
#if 1
	{/* left volume object */
		pObjTmp = lv_slider_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_obj_set_size(pObjTmp, 40, 255);

		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_LEFT, 0, -20);

		pGroup->pLeftVolume = pObjTmp;

	}

	{/* right volume object */
		pObjTmp = lv_slider_create(pParent, pGroup->pLeftVolume);
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_RIGHT, 0, -20);

		pGroup->pRightVolume = pObjTmp;
	}

	{/* check box for uniform volume */
		lv_obj_t *pObjTmp = lv_cb_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_cb_set_text(pObjTmp, "uniform");

		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
		pGroup->pUniformVolume = pObjTmp;

		if (boIsFixUniformVoume)
		{
			lv_cb_set_checked(pObjTmp, true);
			/*lv_cb_set_inactive(pObjTmp);*/
			lv_obj_set_click(pObjTmp, false);
		}
		pGroup->boIsFixUniformVoume = boIsFixUniformVoume;
	}

	{
		lv_obj_t *pObjTmp = lv_label_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_label_set_align(pObjTmp, LV_LABEL_ALIGN_CENTER);

		if (pTitle == NULL)
		{
			char c8Str[32];
			sprintf(c8Str, "%d", u8Index);
			lv_label_set_text(pObjTmp, CHS_TO_UN(c8Str));
		}
		else
		{
			lv_label_set_text(pObjTmp, CHS_TO_UN(pTitle));
		}
		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_MID, 0, -260);
	}
#endif

	lv_obj_set_top(pGroup->pCtrlMode, true);

	if (s_pOrgSliderFun == NULL)
	{
		s_pOrgSliderFun = lv_obj_get_signal_func(pGroup->pLeftVolume);
	}
	lv_obj_set_signal_func(pGroup->pLeftVolume, SignalSlider);
	lv_obj_set_signal_func(pGroup->pRightVolume, SignalSlider);

	lv_cb_set_action(pGroup->pUniformVolume, ActionUniformCB);

	lv_ddlist_set_action(pGroup->pCtrlMode, ActionCtrlModeDDlist);

	{
		uint32_t i;
		lv_obj_t **p2ObjTmp = (lv_obj_t **)pGroup;
		for (i = 0; i < 4; i++)
		{
			lv_obj_set_free_ptr(p2ObjTmp[i], pGroup);
		}

	}

	return 0;

	err:

	return -1;
}

StVolumeCtrlGroup stVolumeInput0 = { 0 };
StVolumeCtrlGroup stVolumeInput1 = { 0 };
StVolumeCtrlGroup stVolumeInput2 = { 0 };
StVolumeCtrlGroup stVolumeInput3 = { 0 };
StVolumeCtrlGroup stVolumeInput4 = { 0 };
StVolumeCtrlGroup stVolumeInput5 = { 0 };
StVolumeCtrlGroup stVolumeInput6 = { 0 };


StVolumeCtrlGroup stVolumeOutputHeaderPhone = { 0 };
StVolumeCtrlGroup stVolumeOutput1 = { 0 };
StVolumeCtrlGroup stVolumeOutput2 = { 0 };

const char *pStr = "我";

const uint8_t c_u8CtrlMode4[] =
{
	0, 1, 2, 3
};
const uint8_t c_u8CtrlMode6[] =
{
	0, 1, 2, 3, 4, 5
};
const uint8_t c_u8CtrlMode2[] =
{
	0, 3
};


int32_t CreateTableView(void)
{
	s_pTheme = lv_theme_alien_init(200, &lv_font_chs_24);

	if (s_pTheme == NULL)
	{
		return -1;
	}

	lv_theme_set_current(s_pTheme);

	//lv_font_add(&lv_font_chs_20, LV_FONT_DEFAULT);

/*
    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_scr_load(scr);
    lv_cont_set_style(scr, s_pTheme->bg);
*/

    lv_obj_set_style(lv_scr_act(), s_pTheme->bg);

	lv_obj_t *tv = lv_tabview_create(lv_scr_act(), NULL);

	lv_obj_t *pTab[8] = { NULL };
	const char *pName[8] =
	{
		"输入1-2",
		"输入3-5",
		"PC 控制",
		"输出",
		"其他",
		"系统设置",
		"系统1置",
		"系统2置",
	};
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		pTab[i] = lv_tabview_add_tab(tv, CHS_TO_UN(pName[i]));

	}
	for (i = 0; i < 8; i++)
	{
		lv_page_set_scrl_fit(pTab[i], false, false);
		lv_page_set_scrl_height(pTab[i], lv_obj_get_height(pTab[i]) - 16);
		lv_page_set_sb_mode(pTab[i], LV_SB_MODE_OFF);
	}


	CreateVolumeCtrlGroup(pTab[0], NULL, 135, &stVolumeInput0, 0,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入1", true);

	CreateVolumeCtrlGroup(pTab[0], NULL, 480, &stVolumeInput1, 1,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入2", true);


	CreateVolumeCtrlGroup(pTab[1], NULL, 20, &stVolumeInput2, 2, 
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入3", true);

	CreateVolumeCtrlGroup(pTab[1], NULL, 20 + 150 + 138, &stVolumeInput3, 3,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入4", true);

	CreateVolumeCtrlGroup(pTab[1], NULL, 20 + (150 + 138) * 2, &stVolumeInput4, 4,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入5", true);



	CreateVolumeCtrlGroup(pTab[2], NULL, 135, &stVolumeInput5, 5,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "总输入", true);

	CreateVolumeCtrlGroup(pTab[2], NULL, 480, &stVolumeInput6, 6,
		c_u8CtrlMode6, sizeof(c_u8CtrlMode6), "PC输入", true);


	CreateVolumeCtrlGroup(pTab[3], NULL, 20, &stVolumeOutputHeaderPhone, 7,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "耳机", false);

	CreateVolumeCtrlGroup(pTab[3], NULL, 20 + 150 + 138, &stVolumeOutput1, 8,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "扬声器", false);

	CreateVolumeCtrlGroup(pTab[3], NULL, 20 + (150 + 138) * 2, &stVolumeOutput2, 9,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "输出", false);

	return 0;
}
