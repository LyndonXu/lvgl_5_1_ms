/******************(C) copyright 天津市XXXXX有限公司 *************************
* All Rights Reserved
* 文件名：gui.c
* 摘要: 界面控制相关程序
* 版本：0.0.1
* 作者：许龙杰
* 日期：2018年04月06日
*******************************************************************************/

#include "gui.h"


#ifndef _WIN32
#define printf(x, ...)
#endif

#define SW_WIDTH	84
#define SW_HTIGHT	42

extern lv_font_t lv_font_chs_24;

char *GetFakeUTF8ForCHS(const char *pStr, int32_t s32StrLen);
#define CHS_TO_UTF8(pStr)			GetFakeUTF8ForCHS(pStr, -1)


const char *c_pCtrlMode[_Audio_Ctrl_Mode_Reserved] =
{
	"直连",		//"Normal",
	"左静音",		//"L Mute",
	"右静音",		//"R Mute",
	"静音",		//"Mute",
	"左→右",		//"R Use L",
	"右→左",		//"L Use R",
	"混合",		//"Mux",
};/**/



StVolumeCtrlGroup stVolumeInput1 = { 0 };
StVolumeCtrlGroup stVolumeInput2 = { 0 };
StVolumeCtrlGroup stVolumeInput3 = { 0 };
StVolumeCtrlGroup stVolumeInput4 = { 0 };
StVolumeCtrlGroup stVolumeInput5 = { 0 };
StVolumeCtrlGroup stVolumeInputMux = { 0 };
StVolumeCtrlGroup stVolumeInputPC = { 0 };


StVolumeCtrlGroup stVolumeOutputHeaderPhone = { 0 };
StVolumeCtrlGroup stVolumeOutputInnerSpeaker = { 0 };
StVolumeCtrlGroup stVolumeOutput = { 0 };

const StVolumeCtrlGroup *c_pValumeCtrlArr[_Channel_Reserved] =
{
	&stVolumeInput1,
	&stVolumeInput2,
	&stVolumeInput3,
	&stVolumeInput4,
	&stVolumeInput5,
	&stVolumeInputMux,
	&stVolumeInputPC,
	&stVolumeOutputHeaderPhone,
	&stVolumeOutputInnerSpeaker,
	&stVolumeOutput,
};


static StMemoryCtrlGroup s_stMemoryCtrlGroup;
static StPhantomPowerCtrlGroup s_stPhantomPowerCtrlGroup;
static StInputEnableCtrlGroup s_stInputEnableCtrlGroup;
static StOutputEnableCtrlGroup s_stOutputEnableCtrlGroup;




static StMemory s_stTotalCtrlMemroy = { 0 };

static StUniformCheckState s_stTotalUnifromCheckState = {false};


const char *c_pTableName[_Tab_Reserved] =
{
	"输入1-2",
	"输入3-5",
	"PC 控制",
	"输出",
	"其他",
	"系统设置",
//	"音量采集",
//	"保留",
};
const uint8_t c_u8CtrlMode4[] =
{
	0, 1, 2, 3
};
const uint8_t c_u8CtrlMode7[] =
{
	0, 1, 2, 3, 4, 5, 6
};
const uint8_t c_u8CtrlMode2[] =
{
	0, 3
};


int32_t GetAudioCtrlMode(uint16_t u16Channel, EmAudioCtrlMode *pMode)
{

	*pMode = s_stTotalCtrlMemroy.emAudioCtrlMode[u16Channel];
	return 0;
}

int32_t SetAudioCtrlMode(uint16_t u16Channel, EmAudioCtrlMode emMode)
{
	if ((u16Channel >= TOTAL_MODE_CTRL) || (emMode >= _Audio_Ctrl_Mode_Reserved))
	{
		return -1;
	}
	s_stTotalCtrlMemroy.emAudioCtrlMode[u16Channel] = emMode;
	return 0;
}
__weak int32_t SendAudioCtrlModeCmd(uint16_t u16Channel, EmAudioCtrlMode emMode)
{
	printf("%s: channel %d, mode %d\n", __FUNCTION__, u16Channel, emMode);
	return 0;
}

void GetAllAudioCtrlMode(EmAudioCtrlMode emAudioCtrlMode[TOTAL_MODE_CTRL])
{
	int s32Size = sizeof(EmAudioCtrlMode) * TOTAL_MODE_CTRL;
	memcpy(emAudioCtrlMode, s_stTotalCtrlMemroy.emAudioCtrlMode, s32Size);
}
void SetAllAudioCtrlMode(EmAudioCtrlMode emAudioCtrlMode[TOTAL_MODE_CTRL])
{
	int i;
	for (i = 0; i < TOTAL_MODE_CTRL; i++)
	{
		SetAudioCtrlMode(i, emAudioCtrlMode[i]);
	}
}



int32_t SetAudioVolume(uint16_t u16Channel, StVolume stVolume)
{
	if ((u16Channel >= TOTAL_VOLUME_CHANNEL))
	{
		return -1;
	}

	memcpy(s_stTotalCtrlMemroy.stVolume + u16Channel, &stVolume, sizeof(StVolume));
	return 0;
}

__weak int32_t SendAudioVolumeCmd(uint16_t u16Channel, StVolume stVolume)
{
	printf("%s: channel %d, volume %d, %d\n", __FUNCTION__, u16Channel, 
		stVolume.u8Channel1, stVolume.u8Channel2);
	return 0;
}


int32_t GetAudioVolume(uint16_t u16Channel, StVolume *pVolume)
{
	if ((u16Channel >= TOTAL_VOLUME_CHANNEL) || (pVolume == NULL))
	{
		return -1;
	}
	memcpy(pVolume, s_stTotalCtrlMemroy.stVolume + u16Channel, sizeof(StVolume));
	return 0;
}

void GetAllAudioVolume(StVolume stVolume[TOTAL_VOLUME_CHANNEL])
{
	int s32Size = sizeof(StVolume) * TOTAL_VOLUME_CHANNEL;
	memcpy(stVolume, s_stTotalCtrlMemroy.stVolume, s32Size);
}
void SetAllAudioVolume(StVolume stVolume[TOTAL_VOLUME_CHANNEL])
{
	int i;
	for (i = 0; i < TOTAL_VOLUME_CHANNEL; i++)
	{
		SetAudioVolume(i, stVolume[i]);
	}
}


int32_t SetUniformCheckState(uint16_t u16Channel, bool boIsCheck)
{
	if ((u16Channel >= TOTAL_VOLUME_CHANNEL))
	{
		return -1;
	}

	s_stTotalUnifromCheckState.boUniformCheckState[u16Channel] = boIsCheck;
	return 0;
}

int32_t GetPhantomPowerState(uint16_t u16Channel, bool *pState)
{
	if ((u16Channel >= PHANTOM_POWER_CTRL) || (pState == NULL))
	{
		return -1;
	}

	*pState = s_stTotalCtrlMemroy.boPhantomPower[u16Channel];

	return 0;
}

int32_t SetPhantomPowerState(uint16_t u16Channel, bool boIsEnable)
{
	if ((u16Channel >= PHANTOM_POWER_CTRL))
	{
		return -1;
	}
	s_stTotalCtrlMemroy.boPhantomPower[u16Channel] = boIsEnable;
	return 0;
}

__weak int32_t SendPhantomPowerStateCmd(uint16_t u16Channel, bool boIsEnable)
{
	printf("%s: channel %d, mode %d\n", __FUNCTION__, u16Channel, boIsEnable);

	return 0;
}


void GetAllPhantomPowerState(bool boState[PHANTOM_POWER_CTRL])
{
	int i;
	for (i = 0; i < PHANTOM_POWER_CTRL; i++)
	{
		GetPhantomPowerState(i, boState + i);
	}
}

void SetAllPhantomPowerState(bool boState[PHANTOM_POWER_CTRL])
{
	int i;
	for (i = 0; i < PHANTOM_POWER_CTRL; i++)
	{
		SetPhantomPowerState(i, boState[i]);
	}
}

uint8_t GetInputEnableState(void)
{
	return s_stTotalCtrlMemroy.u8AINChannelEnableState;
}

int32_t SetInputEnableState(uint8_t u8NewState)
{
	s_stTotalCtrlMemroy.u8AINChannelEnableState = u8NewState;
	return 0;
}

__weak int32_t SendInputEnableStateCmd(uint8_t u8NewState)
{
	printf("%s, state %02x\n", __FUNCTION__, u8NewState);
	return 0;
}


uint8_t GetOutputEnableState(void)
{
	return s_stTotalCtrlMemroy.u8OutputChannelEnableState;
}

int32_t SetOutputEnableState(uint8_t u8NewState)
{
	s_stTotalCtrlMemroy.u8OutputChannelEnableState = u8NewState;
	return 0;
}

__weak int32_t SendOutputEnableStateCmd(uint8_t u8NewState)
{
	printf("%s, state %02x\n", __FUNCTION__, u8NewState);
	return 0;
}

__weak int32_t SendMemeoryCtrlCmd(uint16_t u16Channel, bool boIsSave)
{
	printf("%s, %s channel %d\n", __FUNCTION__, boIsSave ? "save" : "load", u16Channel);
	return 0;
}
__weak int32_t SendFactoryCtrlCmd(void)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}




static lv_theme_t *s_pTheme = NULL;
static lv_obj_t *s_pTableView = NULL;
static lv_group_t *s_pGroup = NULL;


void AnimationTipsEndCB(void *des)
{

	StVolumeCtrlGroup *pGroup = lv_style_anim_get_free_ptr(des);
	lv_obj_del(pGroup->pTipsLabel);
	lv_mem_free(pGroup->pTipsStyle);
	pGroup->pTipsLabel = NULL;
	pGroup->pTipsStyle = NULL;
	pGroup->pTipsAnim = NULL;

	lv_mem_free(des);
	printf("%s, group index: %d\n", __FUNCTION__, pGroup->u8Index);
}

lv_res_t ActionSliderCB(struct _lv_obj_t * obj)
{
	uint16_t u16NewValue = lv_slider_get_value(obj);

	printf("slider value is: %d\n", u16NewValue);
	StVolumeCtrlGroup *pGroup = lv_obj_get_free_ptr(obj);
	if (pGroup->boIsFixUniformVoume || lv_sw_get_state(pGroup->pUniformVolume))
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
	{
		StVolume stVolume;
		if (obj == pGroup->pLeftVolume)
		{
			stVolume.u8Channel1 = (uint8_t)u16NewValue;
			stVolume.u8Channel2 = (uint8_t)lv_slider_get_value(pGroup->pRightVolume);
		}
		else
		{
			stVolume.u8Channel2 = (uint8_t)u16NewValue;
			stVolume.u8Channel1 = (uint8_t)lv_slider_get_value(pGroup->pLeftVolume);
		}
		SetAudioVolume(pGroup->u8Index, stVolume);
		SendAudioVolumeCmd(pGroup->u8Index, stVolume);
	}
	do
	{
		if (pGroup->pTipsAnim == NULL)
		{
			pGroup->pTipsLabel = lv_label_create(lv_obj_get_parent(obj), NULL);

			if (pGroup->pTipsLabel == NULL)
			{
				break;
			}
			pGroup->pTipsStyle = lv_mem_alloc(sizeof(lv_style_t));
			if (pGroup->pTipsStyle == NULL)
			{
				lv_obj_del(pGroup->pTipsLabel);
				pGroup->pTipsLabel = NULL;
				break;
			}
			lv_style_copy(pGroup->pTipsStyle, lv_label_get_style(pGroup->pTipsLabel));

			lv_label_set_style(pGroup->pTipsLabel, pGroup->pTipsStyle);
			lv_obj_align(pGroup->pTipsLabel, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_MID, 0, -100);

			{
				lv_style_t stStyle;
				lv_style_anim_t stAnim;

				lv_style_copy(&stStyle, pGroup->pTipsStyle);
				stStyle.text.opa = LV_OPA_TRANSP;
				stAnim.style_anim = pGroup->pTipsStyle;            /*This style will be animated*/
				stAnim.style_start = pGroup->pTipsStyle;     /*Style in the beginning (can be 'style_anim' as well)*/
				stAnim.style_end = &stStyle;        /*Style at the and (can be 'style_anim' as well)*/
				stAnim.act_time = -500;                     /*These parameters are the same as with the normal animation*/
				stAnim.time = 2000;
				stAnim.playback = 0;
				stAnim.playback_pause = 0;
				stAnim.repeat = 0;
				stAnim.repeat_pause = 0;
				stAnim.end_cb = AnimationTipsEndCB;

				pGroup->pTipsAnim = lv_style_anim_create(&stAnim, pGroup);
				if (pGroup->pTipsAnim == NULL)
				{
					lv_obj_del(pGroup->pTipsLabel);
					lv_mem_free(pGroup->pTipsStyle);
					pGroup->pTipsLabel = NULL;
					pGroup->pTipsStyle = NULL;
					break;
				}
			}

		}

		if (pGroup->pTipsAnim != NULL)
		{
			char c8Str[32];
			sprintf(c8Str, "%dDB", u16NewValue);
			lv_label_set_text(pGroup->pTipsLabel, c8Str);
			lv_anim_reflush(pGroup->pTipsAnim, NULL, -500, 0);
		}

	} while (0);

	return LV_RES_OK;
}


lv_res_t ActionUniformCB(struct _lv_obj_t * obj)
{
	StVolumeCtrlGroup *pGroup = lv_obj_get_free_ptr(obj);
	(void)pGroup;

#if 1
	printf("the %dth check box is: %s\n", pGroup->u8Index,
		lv_sw_get_state(obj) ? "ON" : "OFF");

	SetUniformCheckState(pGroup->u8Index, lv_sw_get_state(obj));
#else
	printf("the %dth check box is: %s\n", pGroup->u8Index, 
		lv_cb_is_checked(obj) ? "check" : "uncheck");

	SetUniformCheckState(pGroup->u8Index, lv_cb_is_checked(obj));
#endif
	return LV_RES_OK;
}

lv_res_t ActionCtrlModeDDlist(struct _lv_obj_t * obj)
{
	StVolumeCtrlGroup *pGroup = lv_obj_get_free_ptr(obj);
	(void)pGroup;

	printf("the %dth ddlist number is: %s(%d)\n", pGroup->u8Index,
		c_pCtrlMode[pGroup->pCtrlModeIndex[lv_ddlist_get_selected(obj)]],
		lv_ddlist_get_selected(obj));

	SetAudioCtrlMode(pGroup->u8Index,
		(EmAudioCtrlMode)(pGroup->pCtrlModeIndex[lv_ddlist_get_selected(obj)]));
	SendAudioCtrlModeCmd(pGroup->u8Index,
		(EmAudioCtrlMode)(pGroup->pCtrlModeIndex[lv_ddlist_get_selected(obj)]));

	return LV_RES_OK;

}

int32_t ReleaseVolumeCtrlGroup(
	StVolumeCtrlGroup *pGroup
)
{
	if (pGroup == NULL)
	{
		return -1;
	}
	if (pGroup->pTipsAnim != NULL)
	{
		lv_anim_del(pGroup->pTipsAnim, NULL);

		lv_mem_free(pGroup->pTipsAnim);
		pGroup->pTipsAnim = NULL;

		lv_mem_free(pGroup->pTipsStyle);
		pGroup->pTipsStyle = NULL;

	}

	{
		uint32_t i;
		lv_obj_t **p2ObjTmp = (lv_obj_t **)pGroup;
		for (i = 0; i < 4; i++)
		{
			lv_group_remove_obj(p2ObjTmp[i]);
		}

	}

	return 0;
}


int32_t CreateVolumeCtrlGroup(
		lv_obj_t *pParent,
		lv_group_t *pGlobalGroup,
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

	if (u16XPos > LV_HOR_RES)
	{
		return -1;
	}

	if (lv_obj_get_free_ptr(pParent) == NULL)
	{
		pObjTmp = lv_cont_create(pParent, NULL);
		lv_obj_set_style(pObjTmp, &lv_style_transp);
		lv_obj_set_click(pObjTmp, false);
		//lv_cont_set_fit(pObjTmp, false, false);
		//lv_cont_set_layout(pObjTmp, LV_LAYOUT_OFF);
		//lv_obj_set_pos(pObjTmp, 0, 0);
		lv_obj_set_height(pObjTmp, lv_obj_get_height(pParent));
		lv_obj_set_width(pObjTmp, lv_obj_get_width(pParent));
		lv_obj_set_free_ptr(pParent, pObjTmp);

		pParent = pObjTmp;
	}
	else
	{
		pParent = (lv_obj_t *)lv_obj_get_free_ptr(pParent);
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

		lv_ddlist_set_options(pObjTmp, CHS_TO_UTF8(c8Str));

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
#endif

		lv_label_set_long_mode(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
			LV_LABEL_LONG_BREAK);

		lv_obj_set_width(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
				150);
	}
#if 1
	{/* left volume object */
		pObjTmp = lv_slider_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_obj_set_size(pObjTmp, SW_HTIGHT, 256);
		lv_slider_set_range(pObjTmp, 0, 255);

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

	{
		/* check box for uniform volume */
#if 1
		lv_obj_t *pLab = NULL;
		lv_obj_t *pObjTmp = lv_sw_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}
		lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_BOTTOM_LEFT, -10, 20);
		pGroup->pUniformVolume = pObjTmp;


		pLab = lv_label_create(pParent, NULL);
		lv_label_set_text(pLab, CHS_TO_UTF8("统一音量"));
		lv_obj_align(pLab, pGroup->pCtrlMode, LV_ALIGN_IN_BOTTOM_RIGHT, 10, 53);
		
		if (boIsFixUniformVoume)
		{
			lv_sw_on(pObjTmp);
			lv_obj_set_click(pObjTmp, false);
		}

#else
		lv_obj_t *pObjTmp = lv_cb_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_cb_set_text(pObjTmp, CHS_TO_UTF8("统一音量")/*"uniform"*/);

		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
		pGroup->pUniformVolume = pObjTmp;

		if (boIsFixUniformVoume)
		{
			lv_cb_set_checked(pObjTmp, true);
			/*lv_cb_set_inactive(pObjTmp);*/
			lv_obj_set_click(pObjTmp, false);
		}
#endif
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
			lv_label_set_text(pObjTmp, CHS_TO_UTF8(c8Str));
		}
		else
		{
			lv_label_set_text(pObjTmp, CHS_TO_UTF8(pTitle));
		}
		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_MID, 0, -260);
	}
#endif

	lv_obj_set_top(pGroup->pCtrlMode, true);

	lv_slider_set_action(pGroup->pLeftVolume, ActionSliderCB);
	lv_slider_set_action(pGroup->pRightVolume, ActionSliderCB);
#if 1
	lv_sw_set_action(pGroup->pUniformVolume, ActionUniformCB);
#else
	lv_cb_set_action(pGroup->pUniformVolume, ActionUniformCB);
#endif
	lv_ddlist_set_action(pGroup->pCtrlMode, ActionCtrlModeDDlist);

	{
		uint32_t i;
		lv_obj_t **p2ObjTmp = (lv_obj_t **)pGroup;
		for (i = 0; i < 4; i++)
		{
			lv_obj_set_free_ptr(p2ObjTmp[i], pGroup);
			if (pGlobalGroup != NULL)
			{
				lv_group_add_obj(pGlobalGroup, p2ObjTmp[i]);
			}
		}

	}

	return 0;

	err:

	return -1;
}

static char s_c8StrLoad[32] = { 0 };
static char s_c8StrSave[32] = { 0 };
static const char *s_pMemoryMboxBTNs[3] =
{
	NULL,
	NULL,
	"",
};


lv_res_t ActionMemoryMBoxCB(lv_obj_t *btn, const char *txt)
{
	lv_obj_t *pParent = lv_obj_get_parent(btn);
	StMemoryCtrlGroup *pGroup = (StMemoryCtrlGroup *)lv_obj_get_free_ptr(pParent);
	if (strstr(txt, s_c8StrLoad) != NULL)
	{
		printf("%s %d\n", "载入", pGroup->u8TmpMemorySelect);
		SendMemeoryCtrlCmd(pGroup->u8TmpMemorySelect, false);
	}
	else
	{
		printf("%s %d\n", "保存", pGroup->u8TmpMemorySelect);
		SendMemeoryCtrlCmd(pGroup->u8TmpMemorySelect, true);
	}
	if (pGroup->pMBox != NULL)
	{
		lv_group_remove_obj(pGroup->pMBox);
		pGroup->pMBox = NULL;
	}

	lv_mbox_start_auto_close(pParent, 100);


	return LV_RES_INV;
}


lv_res_t ActionMemoryCB(lv_obj_t * obj)
{
	lv_obj_t *pParent = lv_obj_get_parent(obj);
	lv_obj_t *pObjTmp = lv_mbox_create(pParent, NULL);
	StMemoryCtrlGroup *pGroup = (StMemoryCtrlGroup *)lv_obj_get_free_ptr(obj);


	pGroup->u8TmpMemorySelect = (uint8_t)lv_ddlist_get_selected(obj);

	if (s_pMemoryMboxBTNs[0] == NULL)
	{
		strcpy(s_c8StrLoad, CHS_TO_UTF8("载入"));
		strcpy(s_c8StrSave, CHS_TO_UTF8("保存"));
		s_pMemoryMboxBTNs[0] = s_c8StrLoad;
		s_pMemoryMboxBTNs[1] = s_c8StrSave;
	}

	lv_mbox_add_btns(pObjTmp, s_pMemoryMboxBTNs, NULL);
	lv_obj_align(pObjTmp, obj, LV_ALIGN_IN_TOP_LEFT, 0, 0);
	lv_mbox_set_text(pObjTmp, "message");
	lv_mbox_set_action(pObjTmp, ActionMemoryMBoxCB);
	lv_obj_set_free_ptr(pObjTmp, pGroup);
	pGroup->pMBox = pObjTmp;

	if (lv_obj_get_group(obj) != NULL)
	{
		lv_group_add_obj(lv_obj_get_group(obj), pObjTmp);
		lv_group_focus_obj(pObjTmp);
	}

	/*
	printf("the %dth ddlist number is: %s(%d)\n", pGroup->u8Index,
	c_pCtrlMode[pGroup->pCtrlModeIndex[lv_ddlist_get_selected(obj)]],
	lv_ddlist_get_selected(obj));
	*/
	return LV_RES_OK;
}

int32_t ReleaseMemoryCtrl(StMemoryCtrlGroup *pGroup)
{
	if ((pGroup == NULL))
	{
		return -1;
	}

	lv_group_remove_obj(pGroup->pMemoryCtrl);
	lv_group_remove_obj(pGroup->pFactorySet);

	if (pGroup->pMBox != NULL)
	{
		lv_group_remove_obj(pGroup->pMBox);
	}

	return 0;

}

lv_res_t ActionFactoryCB(lv_obj_t *pObj)
{
	printf("%s\n", __FUNCTION__);
	SendFactoryCtrlCmd();
	return LV_RES_OK;
}

int32_t CreateMemoryCtrl(
	lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos, uint16_t u16YPos,

	StMemoryCtrlGroup *pGroup)
{
	lv_obj_t *pObjTmp;
	if ((pGroup == NULL) || (pParent == NULL))
	{
		return -1;
	}

	if ((u16XPos >= LV_HOR_RES) || (u16YPos >= LV_VER_RES))
	{
		return -1;
	}

	if (lv_obj_get_free_ptr(pParent) == NULL)
	{
		pObjTmp = lv_cont_create(pParent, NULL);
		lv_obj_set_style(pObjTmp, &lv_style_transp);
		lv_obj_set_click(pObjTmp, false);
		lv_obj_set_height(pObjTmp, lv_obj_get_height(pParent));
		lv_obj_set_width(pObjTmp, lv_obj_get_width(pParent));
		lv_obj_set_free_ptr(pParent, pObjTmp);

		pParent = pObjTmp;
	}
	else
	{
		pParent = (lv_obj_t *)lv_obj_get_free_ptr(pParent);
	}

	memset(pGroup, 0, sizeof(StMemoryCtrlGroup));

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("记忆存储"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);

		pGroup->pLabel = pObjTmp;
	}

	{
		int32_t i;
		char c8Options[128];
		c8Options[0] = 0;
		for (i = 0; i < 8; i++)
		{
			char c8Str[32];
			sprintf(c8Str, "存储 %d", i + 1);
			if (i != 0)
			{
				strcat(c8Options, "\n");
			}
			strcat(c8Options, c8Str);
		}
		pObjTmp = lv_ddlist_create(pParent, NULL);

		lv_ddlist_set_options(pObjTmp, CHS_TO_UTF8(c8Options));

		lv_ddlist_set_action(pObjTmp, ActionMemoryCB);

		lv_obj_align(pObjTmp, pGroup->pLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
		lv_obj_set_top(pObjTmp, true);
		
		pGroup->pMemoryCtrl = pObjTmp;
	}

	{
		pObjTmp = lv_btn_create(pParent, NULL);
		lv_btn_set_fit(pObjTmp, true, false);
		//lv_btn_set_toggle(pObjTmp, true);
		lv_obj_set_height(pObjTmp, lv_obj_get_height(pGroup->pMemoryCtrl));
		pGroup->pFactorySet = pObjTmp;

		pObjTmp = lv_label_create(pGroup->pFactorySet, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8( "工厂设置"));

		lv_obj_align(pGroup->pFactorySet, pGroup->pMemoryCtrl, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

		lv_btn_set_action(pGroup->pFactorySet, LV_BTN_ACTION_CLICK, ActionFactoryCB);
	}

	if (pGlobalGroup != NULL)
	{
		lv_group_add_obj(pGlobalGroup, pGroup->pMemoryCtrl);
		lv_group_add_obj(pGlobalGroup, pGroup->pFactorySet);
	}

	lv_obj_set_free_ptr(pGroup->pMemoryCtrl, pGroup);
	lv_obj_set_free_ptr(pGroup->pFactorySet, pGroup);


	return 0;
}

lv_res_t ActionPhantomPowerCB(lv_obj_t *pObj)
{
	StPhantomPowerCtrlGroup *pGroup = lv_obj_get_free_ptr(pObj);
	uint16_t i;
	for (i = 0; i < PHANTOM_POWER_CTRL; i++)
	{
		if (pObj == pGroup->pCBArr[i])
		{
			printf("set the %dth phantom power %s\n", i, lv_sw_get_state(pObj) ? "ON" : "OFF");
			SetPhantomPowerState(i, lv_sw_get_state(pObj));
			SendPhantomPowerStateCmd(i, lv_sw_get_state(pObj));
			break;
		}
	}
	return LV_RES_OK;
}

int32_t CreatePhantomPowerCtrl(
	lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos, uint16_t u16YPos,

	StPhantomPowerCtrlGroup *pGroup)
{
	lv_obj_t *pObjTmp;
	if ((pGroup == NULL) || (pParent == NULL))
	{
		return -1;
	}

	if ((u16XPos >= LV_HOR_RES) || (u16YPos >= LV_VER_RES))
	{
		return -1;
	}

	if (lv_obj_get_free_ptr(pParent) == NULL)
	{
		pObjTmp = lv_cont_create(pParent, NULL);
		lv_obj_set_style(pObjTmp, &lv_style_transp);
		lv_obj_set_click(pObjTmp, false);
		lv_obj_set_height(pObjTmp, lv_obj_get_height(pParent));
		lv_obj_set_width(pObjTmp, lv_obj_get_width(pParent));
		lv_obj_set_free_ptr(pParent, pObjTmp);

		pParent = pObjTmp;
	}
	else
	{
		pParent = (lv_obj_t *)lv_obj_get_free_ptr(pParent);
	}

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("幻象电源"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);
	}
	{
		uint16_t /*i, */j;
		for (j = 0; j < PHANTOM_POWER_CTRL; j++)
		{
			char c8Str[32];
			lv_obj_t *pLab;
			sprintf(c8Str, "第%d组", j + 1);
#if 1
			pObjTmp = lv_sw_create(pParent, NULL);
			lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
			lv_obj_set_pos(pObjTmp, u16XPos + j * 180, u16YPos + 50);
			pGroup->pCBArr[j] = pObjTmp;
			if (pGlobalGroup != NULL)
			{
				lv_group_add_obj(pGlobalGroup, pObjTmp);
			}
			lv_sw_set_action(pObjTmp, ActionPhantomPowerCB);
			lv_obj_set_free_ptr(pObjTmp, pGroup);

			pLab = lv_label_create(pParent, NULL);
			lv_label_set_text(pLab, CHS_TO_UTF8(c8Str));
			lv_obj_align(pLab, pObjTmp, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

#else
			pObjTmp = lv_cb_create(pParent, NULL);
			lv_cb_set_text(pObjTmp, CHS_TO_UTF8(c8Str));
			lv_obj_set_pos(pObjTmp, u16XPos + j * 160, u16YPos + 40);
			pGroup->pCBArr[j] = pObjTmp;
			if (pGlobalGroup != NULL)
			{
				lv_group_add_obj(pGlobalGroup, pObjTmp);
			}
			lv_obj_set_free_ptr(pObjTmp, pGroup);
			lv_cb_set_action(pObjTmp, ActionPhantomPowerCB);
#endif
		}
	}

	return 0;

}

int32_t RebulidPhantomPowerCtrlVaule(StPhantomPowerCtrlGroup *pGroup)
{
	uint16_t i;
	if (pGroup == NULL)
	{
		return -1;
	}

	for (i = 0; i < PHANTOM_POWER_CTRL; i++)
	{
		bool boSetValue = false;
		GetPhantomPowerState(i, &boSetValue);

		if (lv_sw_get_state(pGroup->pCBArr[i]) != boSetValue)
		{
			if (boSetValue)
			{
				lv_sw_on(pGroup->pCBArr[i]);
			}
			else
			{
				lv_sw_off(pGroup->pCBArr[i]);
			}
		}
	}

	return 0;
}



lv_res_t ActionInputEnableCB(lv_obj_t *pObj)
{
	StInputEnableCtrlGroup *pGroup = lv_obj_get_free_ptr(pObj);
	uint16_t i;
	for (i = 0; i < ENABLE_INPUT_CTRL; i++)
	{
		if (pGroup->pCBArr[i] == pObj)
		{
			uint8_t u8State = GetInputEnableState();
			if (lv_sw_get_state(pObj))
			{
				u8State |= (1 << i);
			}
			else
			{
				u8State &= (~(1 << i));
			}
			SetInputEnableState(u8State);
			SendInputEnableStateCmd(u8State);
			printf("set the %dth input channel %s\n", i, lv_sw_get_state(pObj) ? "ON" : "OFF");
			
			break;
		}
	}
	return LV_RES_OK;
}

int32_t CreateInputEnableCtrl(
	lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos, uint16_t u16YPos,

	StInputEnableCtrlGroup *pGroup)
{
	lv_obj_t *pObjTmp;
	if ((pGroup == NULL) || (pParent == NULL))
	{
		return -1;
	}

	if ((u16XPos >= LV_HOR_RES) || (u16YPos >= LV_VER_RES))
	{
		return -1;
	}

	if (lv_obj_get_free_ptr(pParent) == NULL)
	{
		pObjTmp = lv_cont_create(pParent, NULL);
		lv_obj_set_style(pObjTmp, &lv_style_transp);
		lv_obj_set_click(pObjTmp, false);
		lv_obj_set_height(pObjTmp, lv_obj_get_height(pParent));
		lv_obj_set_width(pObjTmp, lv_obj_get_width(pParent));
		lv_obj_set_free_ptr(pParent, pObjTmp);

		pParent = pObjTmp;
	}
	else
	{
		pParent = (lv_obj_t *)lv_obj_get_free_ptr(pParent);
	}

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("输入使能"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);
	}
	{
		uint16_t /*i, */j;
		for (j = 0; j < ENABLE_INPUT_CTRL; j++)
		{
			lv_obj_t *pLab;
			char c8Str[32];
			sprintf(c8Str, "第%d组", j + 1);
			pObjTmp = lv_sw_create(pParent, NULL);
			lv_obj_set_pos(pObjTmp, u16XPos + j % 2 * 180, u16YPos + 40 + ((j / 2) * 65));
			lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
			pGroup->pCBArr[j] = pObjTmp;
			if (pGlobalGroup != NULL)
			{
				lv_group_add_obj(pGlobalGroup, pObjTmp);
			}
			lv_obj_set_free_ptr(pObjTmp, pGroup);
			lv_sw_set_action(pObjTmp, ActionInputEnableCB);


			pLab = lv_label_create(pParent, NULL);
			lv_label_set_text(pLab, CHS_TO_UTF8(c8Str));
			lv_obj_align(pLab, pObjTmp, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
		}
	}

	return 0;

}

int32_t RebulidInputEnableCtrlVaule(StInputEnableCtrlGroup *pGroup)
{
	uint16_t i;
	uint8_t u8State = GetInputEnableState();

	if (pGroup == NULL)
	{
		return -1;
	}

	for (i = 0; i < ENABLE_INPUT_CTRL; i++)
	{
		bool boSetValue = (u8State & (1 << i)) != 0;
		if (lv_sw_get_state(pGroup->pCBArr[i]) != boSetValue)
		{
			if (boSetValue)
			{
				lv_sw_on(pGroup->pCBArr[i]);
			}
			else
			{
				lv_sw_off(pGroup->pCBArr[i]);
			}
		}
	}

	return 0;
}

lv_res_t ActionOutputEnableCB(lv_obj_t *pObj)
{
	StOutputEnableCtrlGroup *pGroup = lv_obj_get_free_ptr(pObj);
	uint16_t i;
	for (i = 0; i < ENABLE_OUTPUT_CTRL; i++)
	{
		if (pGroup->pCBArr[i] == pObj)
		{
			uint8_t u8State = GetOutputEnableState();
			if (lv_sw_get_state(pObj))
			{
				u8State |= (1 << i);
			}
			else
			{
				u8State &= (~(1 << i));
			}
			SetOutputEnableState(u8State);
			SendOutputEnableStateCmd(u8State);

			printf("set the %dth output channel %s\n", i, lv_sw_get_state(pObj) ? "ON" : "OFF");
			break;
		}
	}
	return LV_RES_OK;
}


int32_t CreateOutputEnableCtrl(
	lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos, uint16_t u16YPos,

	StOutputEnableCtrlGroup *pGroup)
{
	lv_obj_t *pObjTmp;
	if ((pGroup == NULL) || (pParent == NULL))
	{
		return -1;
	}

	if ((u16XPos >= LV_HOR_RES) || (u16YPos >= LV_VER_RES))
	{
		return -1;
	}

	if (lv_obj_get_free_ptr(pParent) == NULL)
	{
		pObjTmp = lv_cont_create(pParent, NULL);
		lv_obj_set_style(pObjTmp, &lv_style_transp);
		lv_obj_set_click(pObjTmp, false);
		lv_obj_set_height(pObjTmp, lv_obj_get_height(pParent));
		lv_obj_set_width(pObjTmp, lv_obj_get_width(pParent));
		lv_obj_set_free_ptr(pParent, pObjTmp);

		pParent = pObjTmp;
	}
	else
	{
		pParent = (lv_obj_t *)lv_obj_get_free_ptr(pParent);
	}

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("输出使能"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);
	}
	{
		uint16_t /*i, */j;
		const char *const pOutName[ENABLE_OUTPUT_CTRL] =
		{
			"PC", "AUX", "MUX",
		};
		for (j = 0; j < ENABLE_OUTPUT_CTRL; j++)
		{
			lv_obj_t *pLab;
			pObjTmp = lv_sw_create(pParent, NULL);
			lv_obj_set_pos(pObjTmp, u16XPos + j % 2 * 180, u16YPos + 40 + ((j / 2) * 65));
			lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
			pGroup->pCBArr[j] = pObjTmp;
			if (pGlobalGroup != NULL)
			{
				lv_group_add_obj(pGlobalGroup, pObjTmp);
			}
			lv_obj_set_free_ptr(pObjTmp, pGroup);
			lv_sw_set_action(pObjTmp, ActionOutputEnableCB);

			pLab = lv_label_create(pParent, NULL);
			lv_label_set_text(pLab, CHS_TO_UTF8(pOutName[j]));
			lv_obj_align(pLab, pObjTmp, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

		}
	}

	return 0;

}

int32_t RebulidOutputEnableCtrlVaule(StOutputEnableCtrlGroup *pGroup)
{
	uint16_t i;
	uint8_t u8State = GetOutputEnableState();

	if (pGroup == NULL)
	{
		return -1;
	}

	for (i = 0; i < ENABLE_OUTPUT_CTRL; i++)
	{
		bool boSetValue = (u8State & (1 << i)) != 0;
		if (lv_sw_get_state(pGroup->pCBArr[i]) != boSetValue)
		{
			if (boSetValue)
			{
				lv_sw_on(pGroup->pCBArr[i]);
			}
			else
			{
				lv_sw_off(pGroup->pCBArr[i]);
			}
		}
	}

	return 0;
}

typedef int32_t (*PFUN_CreateTable)(lv_obj_t *pTabPage, lv_group_t *pGroup);
typedef int32_t (*PFUN_ReleaseTable)(lv_obj_t *pTabPage);
typedef int32_t(*PFUN_RebulidTableValue)(void);

int32_t ReleaseTableInput1To2(lv_obj_t *pTabParent)
{
	ReleaseVolumeCtrlGroup(&stVolumeInput1);
	ReleaseVolumeCtrlGroup(&stVolumeInput2);

	return 0;
}
int32_t CreateTableInput1To2(lv_obj_t *pTabParent, lv_group_t *pGroup)
{
	CreateVolumeCtrlGroup(pTabParent, pGroup, 135, &stVolumeInput1, _Channel_AIN_1,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入1", true);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 470, &stVolumeInput2, _Channel_AIN_2,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入2", true);

	return 0;
}

int32_t RebulidVolumeCtrlValue(uint16_t u16Index)
{
	const StVolumeCtrlGroup *pGroup = NULL;
	if (u16Index >= _Channel_Reserved)
	{
		return -1;
	}

	pGroup = c_pValumeCtrlArr[u16Index];

	if (lv_slider_get_value(pGroup->pLeftVolume) != s_stTotalCtrlMemroy.stVolume[u16Index].u8Channel1)
	{
		lv_slider_set_value(pGroup->pLeftVolume, s_stTotalCtrlMemroy.stVolume[u16Index].u8Channel1);
	}
	if (lv_slider_get_value(pGroup->pRightVolume) != s_stTotalCtrlMemroy.stVolume[u16Index].u8Channel2)
	{
		lv_slider_set_value(pGroup->pRightVolume, s_stTotalCtrlMemroy.stVolume[u16Index].u8Channel2);
	}
	{
		uint16_t i, u16Selected = 0;
		for (i = 0; i < pGroup->u8MaxCtrlMode; i++)
		{
			if (pGroup->pCtrlModeIndex[i] == (uint8_t)s_stTotalCtrlMemroy.emAudioCtrlMode[u16Index])
			{
				u16Selected = i;
				break;
			}
		}
		if (lv_ddlist_get_selected(pGroup->pCtrlMode) != u16Selected)
		{
			lv_ddlist_set_selected(pGroup->pCtrlMode, u16Selected);
		}
	}
	if (!pGroup->boIsFixUniformVoume)
	{
#if 1
		if (lv_sw_get_state(pGroup->pUniformVolume) != s_stTotalUnifromCheckState.boUniformCheckState[u16Index])
		{
			if (s_stTotalUnifromCheckState.boUniformCheckState[u16Index])
			{
				lv_sw_on(pGroup->pUniformVolume);
			}
			else
			{
				lv_sw_off(pGroup->pUniformVolume);
			}
		}

#else
		if (lv_cb_is_checked(pGroup->pUniformVolume) != s_stTotalUnifromCheckState.boUniformCheckState[u16Index])
		{
			lv_cb_set_checked(pGroup->pUniformVolume,
				s_stTotalUnifromCheckState.boUniformCheckState[u16Index]);
		}
#endif
	}
	return 0;
}


int32_t RebulidTableInput1To2Vaule(void)
{
	if (s_pTableView == NULL)
	{
		return -1;
	}

	//if (lv_tabview_get_tab_act(s_pTableView) != _Tab_Input_1_2)
	//{
	//	return -1;
	//}
	
	RebulidVolumeCtrlValue(_Channel_AIN_1);
	RebulidVolumeCtrlValue(_Channel_AIN_2);
	
	return 0;
}


int32_t ReleaseTableInput3To5(lv_obj_t *pTabParent)
{
	ReleaseVolumeCtrlGroup(&stVolumeInput3);
	ReleaseVolumeCtrlGroup(&stVolumeInput4);
	ReleaseVolumeCtrlGroup(&stVolumeInput5);

	return 0;
}

int32_t CreateTableInput3To5(lv_obj_t *pTabParent, lv_group_t *pGroup)
{
	CreateVolumeCtrlGroup(pTabParent, pGroup, 30, &stVolumeInput3, _Channel_AIN_3,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入3", true);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 30 + 270, &stVolumeInput4, _Channel_AIN_4,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入4", true);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 30 + 270 * 2, &stVolumeInput5, _Channel_AIN_5,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "输入5", true);

	return 0;
}

int32_t RebulidTableInput3To5Vaule(void)
{
	if (s_pTableView == NULL)
	{
		return -1;
	}

	RebulidVolumeCtrlValue(_Channel_AIN_3);
	RebulidVolumeCtrlValue(_Channel_AIN_4);
	RebulidVolumeCtrlValue(_Channel_AIN_5);
	return 0;
}

int32_t ReleaseTableInputPCCtrl(lv_obj_t *pTabParent)
{
	ReleaseVolumeCtrlGroup(&stVolumeInputMux);
	ReleaseVolumeCtrlGroup(&stVolumeInputPC);
	return 0;
}

int32_t CreateTableInputPCCtrl(lv_obj_t *pTabParent, lv_group_t *pGroup)
{
	CreateVolumeCtrlGroup(pTabParent, pGroup, 135, &stVolumeInputMux, _Channel_AIN_Mux,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "总输入", false);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 470, &stVolumeInputPC, _Channel_PC,
		c_u8CtrlMode7, sizeof(c_u8CtrlMode7), "PC输入", false);

	return 0;
}

int32_t RebulidTableInputPCCtrlVaule(void)
{
	if (s_pTableView == NULL)
	{
		return -1;
	}

	RebulidVolumeCtrlValue(_Channel_AIN_Mux);
	RebulidVolumeCtrlValue(_Channel_PC);
	return 0;
}

int32_t ReleaseTableOutputCtrl(lv_obj_t *pTabParent)
{
	ReleaseVolumeCtrlGroup(&stVolumeOutputHeaderPhone);
	ReleaseVolumeCtrlGroup(&stVolumeOutputInnerSpeaker);
	ReleaseVolumeCtrlGroup(&stVolumeOutput);
	return 0;
}

int32_t CreateTableOutputCtrl(lv_obj_t *pTabParent, lv_group_t *pGroup)
{
	CreateVolumeCtrlGroup(pTabParent, pGroup, 30, &stVolumeOutputHeaderPhone, _Channel_HeaderPhone,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "耳机", false);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 30 + 270, &stVolumeOutputInnerSpeaker, _Channel_InnerSpeaker,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "扬声器", false);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 30 + 270 * 2, &stVolumeOutput, _Channel_NormalOut,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "输出", false);
	return 0;
}

int32_t RebulidTableOutputVaule(void)
{
	if (s_pTableView == NULL)
	{
		return -1;
	}

	RebulidVolumeCtrlValue(_Channel_HeaderPhone);
	RebulidVolumeCtrlValue(_Channel_InnerSpeaker);
	RebulidVolumeCtrlValue(_Channel_NormalOut);
	return 0;
}

int32_t ReleaseTableOtherCtrl(lv_obj_t *pTabParent)
{
	ReleaseMemoryCtrl(&s_stMemoryCtrlGroup);
	return 0;
}


int32_t CreateTableOtherCtrl(lv_obj_t *pParent, lv_group_t *pGroup)
{
	CreateMemoryCtrl(pParent, pGroup, 20, 20, &s_stMemoryCtrlGroup);

	CreatePhantomPowerCtrl(pParent, pGroup, 20, 140, &s_stPhantomPowerCtrlGroup);

	CreateInputEnableCtrl(pParent, pGroup, 400 + 20, 20, &s_stInputEnableCtrlGroup);
	
	CreateOutputEnableCtrl(pParent, pGroup, 400 + 20, 250, &s_stOutputEnableCtrlGroup);
	return 0;
}

int32_t RebulidTableOtherVaule(void)
{
	if (s_pTableView == NULL)
	{
		return -1;
	}

	RebulidPhantomPowerCtrlVaule(&s_stPhantomPowerCtrlGroup);
	RebulidInputEnableCtrlVaule(&s_stInputEnableCtrlGroup);
	RebulidOutputEnableCtrlVaule(&s_stOutputEnableCtrlGroup);
	return 0;
}



const PFUN_ReleaseTable c_pFUN_ReleaseTable[_Tab_Reserved] = 
{
	ReleaseTableInput1To2,
	ReleaseTableInput3To5,
	ReleaseTableInputPCCtrl,
	ReleaseTableOutputCtrl,
	ReleaseTableOtherCtrl,
	NULL,
};

const PFUN_CreateTable c_pFUN_CreateTable[_Tab_Reserved] =
{
	CreateTableInput1To2,
	CreateTableInput3To5,
	CreateTableInputPCCtrl,
	CreateTableOutputCtrl,
	CreateTableOtherCtrl,
	NULL,
};

const PFUN_RebulidTableValue c_pFun_RebulidTableValue[_Tab_Reserved] =
{
	RebulidTableInput1To2Vaule,
	RebulidTableInput3To5Vaule,
	RebulidTableInputPCCtrlVaule,
	RebulidTableOutputVaule,
	RebulidTableOtherVaule,
	NULL,
};


int32_t ReleaseTable(lv_obj_t *pTabPage, uint16_t u16TableIndex)
{
	if (u16TableIndex >= _Tab_Reserved)
	{
		return -1;
	}
	if (c_pFUN_ReleaseTable[u16TableIndex] != NULL)
	{
		return c_pFUN_ReleaseTable[u16TableIndex](pTabPage);
	}
	return -1;
}

int32_t CreateTable(lv_obj_t *pTabPage, uint16_t u16TableIndex)
{
	if (u16TableIndex >= _Tab_Reserved)
	{
		return -1;
	}
	if (c_pFUN_CreateTable[u16TableIndex] != NULL)
	{
		if (c_pFUN_CreateTable[u16TableIndex](pTabPage, 
			lv_obj_get_free_ptr(lv_obj_get_parent(lv_obj_get_parent(pTabPage)))) == 0)
		{
			if (c_pFun_RebulidTableValue[u16TableIndex] != NULL)
			{
				c_pFun_RebulidTableValue[u16TableIndex]();
			}
			return 0;
		}
		return -1;
	}
	return -1;
}


static lv_obj_t *pBar[14];

void ActionTabview(lv_obj_t *pTV, uint16_t u16CurTable)
{
	uint16_t u16OrgTable = lv_tabview_get_tab_act(pTV);
	printf("the old page is: %d, and the new is: %d\n",
		u16OrgTable, u16CurTable);

	if (u16OrgTable != u16CurTable)
	{
		//if (u16OrgTable == 1)
		{
			lv_obj_t *pTable = lv_tabview_get_tab(pTV, u16OrgTable);
			lv_obj_t *pCont = (lv_obj_t *)lv_obj_get_free_ptr(pTable);
			if (pCont != NULL)
			{
				ReleaseTable(pTable, u16OrgTable);

				lv_obj_del(pCont);
			}
			lv_obj_set_free_ptr(pTable, NULL);
		}

		//if (u16CurTable == 1)
		{
			lv_obj_t *pTable = lv_tabview_get_tab(pTV, u16CurTable);
			lv_obj_t *pCont = (lv_obj_t *)lv_obj_get_free_ptr(pTable);
			if (pCont == NULL)
			{
				CreateTable(pTable, u16CurTable);
			}
		}
	}
}

static void GroupStyleMod(lv_style_t * style)
{
#if LV_COLOR_DEPTH != 1
	/*Make the style to be a little bit orange*/
	style->body.border.opa = LV_OPA_COVER;
	style->body.border.color = LV_COLOR_ORANGE;

	/*If not empty or has border then emphasis the border*/
	if (style->body.empty == 0 || style->body.border.width != 0) 
		style->body.border.width = LV_DPI / 40;

	//style->body.main_color = lv_color_mix(style->body.main_color, LV_COLOR_ORANGE, LV_OPA_70);
	//style->body.grad_color = lv_color_mix(style->body.grad_color, LV_COLOR_ORANGE, LV_OPA_70);
	//style->body.shadow.color = lv_color_mix(style->body.shadow.color, LV_COLOR_ORANGE, LV_OPA_60);

	//style->text.color = lv_color_mix(style->text.color, LV_COLOR_ORANGE, LV_OPA_70);
#else
	style->body.border.opa = LV_OPA_COVER;
	style->body.border.color = LV_COLOR_BLACK;
	style->body.border.width = 3;

#endif

}



int32_t ReflushActiveTable(uint32_t u32Fun, uint32_t u32Channel)
{
	uint16_t u16ActiveTableIndex = ~0;
	if (s_pTableView == NULL)
	{
		return -1;
	}

	if (u32Fun >= _Fun_Reserved)
	{
		return -1;
	}

	u16ActiveTableIndex = lv_tabview_get_tab_act(s_pTableView);
	if (u16ActiveTableIndex >= _Tab_Reserved)
	{
		return -1;
	}

	switch (u32Fun)
	{
		case _Fun_AudioVolume: 
		case _Fun_AudioMode:
		{
			if (u32Channel >= _Channel_Reserved)
			{
				return -1;
			}

			if (u32Channel <= _Channel_AIN_2)
			{
				if (u16ActiveTableIndex != _Tab_Input_1_2)
				{
					return 0;
				}
			}
			else if (u32Channel <= _Channel_AIN_5)
			{
				if (u16ActiveTableIndex != _Tab_Input_3_5)
				{
					return 0;
				}
			}
			else if (u32Channel <= _Channel_PC)
			{
				if (u16ActiveTableIndex != _Tab_Input_PC_Ctrl)
				{
					return 0;
				}
			}
			else
			{
				if (u16ActiveTableIndex != _Tab_Output)
				{
					return 0;
				}
			}
			break;
		}
		case _Fun_PhantomPower:
		case _Fun_InputEnable:
		case _Fun_OutputEnable:
		{
			if (u16ActiveTableIndex != _Tab_Other_Ctrl)
			{
				return 0;
			}
			break;
		}
		default:
			break;
	}


	if (c_pFun_RebulidTableValue[u16ActiveTableIndex] != NULL)
	{
		c_pFun_RebulidTableValue[u16ActiveTableIndex]();
	}

	return 0;
}

int32_t CreateTableView(void)
{
	lv_theme_t * lv_theme_zen_init(uint16_t hue, lv_font_t *font);

	//lv_font_add(&lv_font_symbol_20, &lv_font_chs_24);
	s_pTheme = lv_theme_zen_init(120, &lv_font_chs_24);

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

	lv_obj_t *pTableView = lv_tabview_create(lv_scr_act(), NULL);
	lv_tabview_set_sliding(pTableView, false);


#if 0
	{
		extern const lv_img_t img_bubble_pattern;
		lv_obj_t *wp = lv_img_create(lv_scr_act(), NULL);
		lv_img_set_src(wp, &img_bubble_pattern);
		lv_obj_set_width(wp, LV_HOR_RES * 6);
		lv_obj_set_protect(wp, LV_PROTECT_POS);

		lv_obj_set_parent(wp, ((lv_tabview_ext_t *)pTableView->ext_attr)->content);
		lv_obj_set_pos(wp, 0, -5);
	}
#endif


	
	{
		bool keyboard_read(lv_indev_data_t * data);

		s_pGroup = lv_group_create();
		lv_group_set_style_mod_cb(s_pGroup, GroupStyleMod);
#ifdef _WIN32
		lv_indev_drv_t kb_drv;
		kb_drv.type = LV_INDEV_TYPE_KEYPAD;
		kb_drv.read = keyboard_read;
		lv_indev_t *kb_indev = lv_indev_drv_register(&kb_drv);
		lv_indev_set_group(kb_indev, s_pGroup);
#endif
		if (s_pGroup != NULL)
		{
			lv_group_add_obj(s_pGroup, pTableView);
		}

		lv_obj_set_free_ptr(pTableView, s_pGroup);

	}

	lv_obj_t *pTab[_Tab_Reserved] = { NULL };

	uint8_t i;
	for (i = 0; i < _Tab_Reserved; i++)
	{
		pTab[i] = lv_tabview_add_tab(pTableView, CHS_TO_UTF8(c_pTableName[i]));
	}
	for (i = 0; i < _Tab_Reserved; i++)
	{
		lv_page_set_scrl_fit(pTab[i], false, false);
		lv_page_set_scrl_height(pTab[i], lv_obj_get_height(pTab[i]) - 16);
		lv_page_set_sb_mode(pTab[i], LV_SB_MODE_OFF);
	}



	CreateTable(pTab[_Tab_Input_PC_Ctrl], _Tab_Input_PC_Ctrl);

#if 0
	{
		uint32_t i;
		for (i = 0; i < 7; i++)
		{
			uint32_t j;
			for (j = 0; j < 2; j++)
			{
				lv_obj_t * bar2 = lv_bar_create(pTab[6], NULL);
				lv_obj_set_size(bar2, 20, 300);
				lv_obj_set_pos(bar2, 30 + i * 110 + j * 45, 60);
				lv_bar_set_range(bar2, 0, 300);
				lv_bar_set_value(bar2, i * 20 + j * 5);

				pBar[i * 2 + j] = bar2;
			}
		}
	}
#endif

	lv_tabview_set_tab_act(pTableView, _Tab_Input_PC_Ctrl, false);

	lv_tabview_set_tab_load_action(pTableView, ActionTabview);

	s_pTableView = pTableView;

	return 0;
}

void BarValueTest(void)
{
#if 0
	uint32_t i;
	for (i = 0; i < 14; i++)
	{
		lv_bar_set_value(pBar[i], rand()%300);
	}
#endif
}
