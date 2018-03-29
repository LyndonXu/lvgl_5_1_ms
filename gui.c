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
	uint8_t u8MaxCtrlMode;
	uint8_t u8Index;
	bool boIsFixUniformVoume;

}StVolumeCtrlGroup;

const char *c_pCtrlMode[_Audio_Ctrl_Mode_Reserved] =
{
	"Normal",
	"Mute",
	"L Mute",
	"R Mute",
	"R Use L",
	"L Use R",
	"Mux",
};/**/

static lv_theme_t *s_pTheme = NULL;

lv_signal_func_t s_pOrgSliderFun = NULL;
lv_res_t lv_signal_func_test(struct _lv_obj_t * obj,
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

int32_t CreateVolumeCtrlGroup(StVolumeCtrlGroup *pGroup, uint8_t u8Index,
		lv_obj_t *pParent,
		lv_obj_t *pGlobalGroup,
		uint16_t u16XPos, uint8_t u8MaxCtrlMode,
		bool boIsFixUniformVoume)
{
	lv_obj_t *pObjTmp;
	if ((pGroup == NULL) || (pParent == NULL))
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

	pGroup->u8MaxCtrlMode = u8MaxCtrlMode;
	pGroup->u16XPos = u16XPos;


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
			strcat(c8Str, c_pCtrlMode[i]);
		}

		//pObjTmp = lv_btn_create(pParent, NULL);
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


	lv_obj_set_top(pGroup->pCtrlMode, true);

	if (s_pOrgSliderFun == NULL)
	{
		s_pOrgSliderFun = lv_obj_get_signal_func(pGroup->pLeftVolume);
	}
	lv_obj_set_signal_func(pGroup->pLeftVolume, lv_signal_func_test);
	lv_obj_set_signal_func(pGroup->pRightVolume, lv_signal_func_test);

	{
		uint32_t i;
		lv_obj_t **p2ObjTmp = (lv_obj_t **)pGroup;
		for (i = 0; i < 4; i++)
		{
			lv_obj_set_free_ptr(p2ObjTmp[i], pGroup);
		}

	}

	pGroup->u8Index = u8Index;

	{
		lv_obj_t *pObjTmp = lv_label_create(pParent, NULL);
		char c8Str[32];
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_label_set_align(pObjTmp, LV_LABEL_ALIGN_CENTER);

#if 1
		sprintf(c8Str, "输入%d", u8Index);

		lv_label_set_text(pObjTmp, CHS_TO_UN(c8Str));
#else
		sprintf(c8Str, "Input%d", u8Index);
		lv_label_set_text(pObjTmp, c8Str);
#endif
		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_MID, 0, -260);
	}
#endif
	return 0;

	err:

	return -1;
}

StVolumeCtrlGroup stVolumeInput1 = {0};
StVolumeCtrlGroup stVolumeInput2 = {0};
StVolumeCtrlGroup stVolumeInput3 = {0};

const char *pStr = "我";

int32_t CreateTableView(void)
{
	printf("%02hhx, %02hhx\n", (uint8_t)pStr[0], (uint8_t)pStr[1]);
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

	//lv_obj_set_size(tv, LV_HOR_RES, LV_VER_RES);

	lv_obj_t *pTab1 = NULL;
	lv_obj_t *pTab2 = NULL;
	lv_obj_t *pTab3 = NULL;

	pTab1 = lv_tabview_add_tab(tv, CHS_TO_UN("输入1-3"));
/**/

	pTab2 = lv_tabview_add_tab(tv, CHS_TO_UN("输入4-6"));
	pTab3 = lv_tabview_add_tab(tv, CHS_TO_UN("输出"));

	//lv_tabview_set_tab_act(tv, 1, false);
    lv_page_set_scrl_fit(pTab1, false, false);
    lv_page_set_scrl_height(pTab1, lv_obj_get_height(pTab1) - 16);
    lv_page_set_sb_mode(pTab1, LV_SB_MODE_OFF);

	//lv_page_set_scrl_layout(pTab1, LV_LAYOUT_GRID);


	CreateVolumeCtrlGroup(&stVolumeInput1, 1, pTab1, NULL, 20,
			_Audio_Ctrl_Mode_Reserved, true);


	CreateVolumeCtrlGroup(&stVolumeInput2, 2, pTab1, NULL, 20 + 150 + 138,
			_Audio_Ctrl_Mode_Reserved, true);

	CreateVolumeCtrlGroup(&stVolumeInput3, 3, pTab1, NULL, 20 + (150 + 138) * 2,
			_Audio_Ctrl_Mode_Reserved, true);

	return 0;
}
