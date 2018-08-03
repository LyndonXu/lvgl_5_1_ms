/******************(C) copyright 天津市XXXXX有限公司 *************************
* All Rights Reserved
* 文件名：gui.c
* 摘要: 界面控制相关程序
* 版本：0.0.1
* 作者：许龙杰
* 日期：2018年04月06日
*******************************************************************************/

#include "gui.h"
#include "screen_protect.h"

#if USE_LVGL

#ifndef _WIN32
#define printf(x, ...)
#endif

#define SW_WIDTH		84
#define SW_HTIGHT		55

#define SLIDER_WIDTH	60

#define KNOB_WIDTH		3
#define KNOB_HEIGHT		2

enum 
{
	_OBJ_TYPE_SLIDER = 0x80,
	_OBJ_TYPE_BTN,
	_OBJ_TYPE_DDLIST,
};

extern lv_font_t lv_font_chs_24;

char *GetFakeUTF8ForCHS(const char *pStr, int32_t s32StrLen);
#define CHS_TO_UTF8(pStr)			GetFakeUTF8ForCHS(pStr, -1)


static struct {
	lv_style_t bg;
	lv_style_t indic;
	lv_style_t knob;
}s_stStyleSlideDisable;


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

const char *c_pCtrlModeSpecial[_Audio_Ctrl_Mode_Reserved] =
{
	"直连",		//"Normal",
	"差分右",		//"L Mute",
	"差分左",		//"R Mute",
	"静音",		//"Mute",
	"左→右",		//"R Use L",
	"右→左",		//"L Use R",
	"混合",		//"Mux",
};/**/


const float c_f32Volume[256] = 
{
	-99.0f, -24.1f, -21.0f, -19.3f, -18.0f, -17.0f, -16.2f, -15.5f, 
	-14.9f, -14.4f, -13.9f, -13.5f, -13.1f, -12.7f, -12.4f, -12.1f, 
	-11.8f, -11.5f, -11.2f, -11.0f, -10.7f, -10.5f, -10.3f, -10.1f, 
	-9.9f, -9.7f, -9.5f, -9.3f, -9.1f, -8.9f, -8.8f, -8.6f,
	-8.5f, -8.3f, -8.1f, -8.0f, -7.9f, -7.7f, -7.6f, -7.5f,
	-7.3f, -7.2f, -7.1f, -6.9f, -6.8f, -6.7f, -6.6f, -6.5f,
	-6.4f, -6.3f, -6.1f, -6.0f, -5.9f, -5.8f, -5.7f, -5.6f,
	-5.5f, -5.4f, -5.3f, -5.2f, -5.1f, -5.0f, -5.0f, -4.9f,
	-4.8f, -4.7f, -4.6f, -4.5f, -4.4f, -4.3f, -4.2f, -4.2f,
	-4.1f, -4.0f, -3.9f, -3.8f, -3.7f, -3.7f, -3.6f, -3.5f,
	-3.4f, -3.3f, -3.3f, -3.2f, -3.1f, -3.0f, -3.0f, -2.9f,
	-2.8f, -2.7f, -2.7f, -2.6f, -2.5f, -2.4f, -2.4f, -2.3f,
	-2.2f, -2.1f, -2.1f, -2.0f, -1.9f, -1.9f, -1.8f, -1.7f,
	-1.6f, -1.6f, -1.5f, -1.4f, -1.4f, -1.3f, -1.2f, -1.2f,
	-1.1f, -1.0f, -1.0f, -0.9f, -0.8f, -0.7f, -0.7f, -0.6f,
	-0.5f, -0.5f, -0.4f, -0.3f, -0.3f, -0.2f, -0.1f, -0.1f,
	0.0f, 0.1f, 0.1f, 0.2f, 0.3f, 0.3f, 0.4f, 0.5f,
	0.5f, 0.6f, 0.7f, 0.7f, 0.8f, 0.9f, 1.0f, 1.0f,
	1.1f, 1.2f, 1.2f, 1.3f, 1.4f, 1.4f, 1.5f, 1.6f,
	1.6f, 1.7f, 1.8f, 1.9f, 1.9f, 2.0f, 2.1f, 2.1f,
	2.2f, 2.3f, 2.4f, 2.4f, 2.5f, 2.6f, 2.7f, 2.7f,
	2.8f, 2.9f, 3.0f, 3.0f, 3.1f, 3.2f, 3.3f, 3.3f,
	3.4f, 3.5f, 3.6f, 3.7f, 3.7f, 3.8f, 3.9f, 4.0f,
	4.1f, 4.2f, 4.2f, 4.3f, 4.4f, 4.5f, 4.6f, 4.7f,
	4.8f, 4.9f, 5.0f, 5.0f, 5.1f, 5.2f, 5.3f, 5.4f,
	5.5f, 5.6f, 5.7f, 5.8f, 5.9f, 6.0f, 6.1f, 6.3f,
	6.4f, 6.5f, 6.6f, 6.7f, 6.8f, 6.9f, 7.1f, 7.2f,
	7.3f, 7.5f, 7.6f, 7.7f, 7.9f, 8.0f, 8.1f, 8.3f,
	8.5f, 8.6f, 8.8f, 8.9f, 9.1f, 9.3f, 9.5f, 9.7f,
	9.9f, 10.1f, 10.3f, 10.5f, 10.7f, 11.0f, 11.2f, 11.5f, 
	11.8f, 12.1f, 12.4f, 12.7f, 13.1f, 13.5f, 13.9f, 14.4f, 
	14.9f, 15.5f, 16.2f, 17.0f, 18.0f, 19.3f, 21.0f, 24.1f,
};


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


StVolumeCtrlGroup stVolumePCCtrlPlay = { 0 };
StVolumeCtrlGroup stVolumePCCtrlRecord = { 0 };

const StVolumeCtrlGroup *c_pValumeCtrlArr[TOTAL_VOLUME_CHANNEL + TOTAL_PC_CTRL_MODE_CTRL] =
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


	&stVolumePCCtrlPlay,
	&stVolumePCCtrlRecord,
};


static StMemoryCtrlGroup s_stMemoryCtrlGroup;
static StPhantomPowerCtrlGroup s_stPhantomPowerCtrlGroup;
static StInputEnableCtrlGroup s_stInputEnableCtrlGroup;
static StOutputEnableCtrlGroup s_stOutputEnableCtrlGroup;




static StMemory s_stTotalCtrlMemroy = { 0 };

static StUniformCheckState s_stTotalUnifromCheckState = 
{
	true, true, true, true, 
	true, true, true, true,
	true, true, true, true,
};


const char *c_pTableName[_Tab_Reserved] =
{
	"输入A",
	"输入B",
	"I2S",
	"输出",
	"其他",
	"声卡",
	"外设",
	"设置",
//	"音量采集",
//	"保留",
};

const char *c_pScreenProtectTime[_ScreenProtect_Reserved] =
{
	"15 秒",
	"30 秒",
	"1 分钟",
	"2 分钟",
	"5 分钟",
	"10 分钟",
	"关闭",
};

const char *c_pScreenProtectMode[_ScreenProtect_Mode_Reserved] =
{
	"LOGO",
	"熄屏"
};

const uint8_t c_u8CtrlModeSpecialLeft[] =
{
	0, 2, 3
};
const uint8_t c_u8CtrlModeSpecialRight[] =
{
	0, 1, 3
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


static lv_style_t s_stLogoStyle[_Logo_Color_Reserved][_Logo_State_Reserved] =
{
	0,
};
/* B, G, R */
static lv_color24_t const s_stLogoColor[_Logo_Color_Reserved] =
{
	{ 0 , 0, 0xFF, },
	{ 0 , 0xFF, 0, },
	{ 0xFF, 0 , 0, },
	{ 0xFF, 0xFF , 0xFF, },
	//{ 0x00, 0x00 , 0x00, },
};
static StLogoColorCtrl s_stLogoColorCtrl = { {NULL,}, 0xFF };
static StKeyboardCtrl s_stKeyboardCtrl = { NULL, NULL, true, 0 };

static StScreenProtectCtrl s_stScreenProtectCtrl = { 0 };

int32_t ChannelToReal(uint16_t u16Channel)
{
	if (u16Channel < _Channel_Reserved)
	{
		return u16Channel;
	}

	if (u16Channel >= _Channel_PC_Ctrl && u16Channel < _Channel_PC_Ctrl_Reserved)
	{
		return u16Channel - _Channel_PC_Ctrl + _Channel_Reserved;
	}

	return -1;
}

int32_t GetAudioCtrlMode(uint16_t u16Channel, EmAudioCtrlMode *pMode)
{
	u16Channel = ChannelToReal(u16Channel);

	if (u16Channel >= TOTAL_CHANNEL || pMode == NULL)
	{
		return -1;
	}

	*pMode = s_stTotalCtrlMemroy.emAudioCtrlMode[u16Channel];
	return 0;
}

int32_t SetAudioCtrlMode(uint16_t u16Channel, EmAudioCtrlMode emMode)
{
	u16Channel = ChannelToReal(u16Channel);

	if ((u16Channel >= TOTAL_CHANNEL) || (emMode >= _Audio_Ctrl_Mode_Reserved))
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
	u16Channel = ChannelToReal(u16Channel);

	if ((u16Channel >= TOTAL_CHANNEL))
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
	u16Channel = ChannelToReal(u16Channel);

	if ((u16Channel >= TOTAL_CHANNEL) || (pVolume == NULL))
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
	u16Channel = ChannelToReal(u16Channel);

	if ((u16Channel >= TOTAL_CHANNEL))
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

__weak int32_t SendLogoColorCtrlCmd(lv_color24_t stColor)
{
	printf("%s logo color is: (%02x, %02x, %02x)\n", __FUNCTION__,
		stColor.red, stColor.green, stColor.blue);
	return 0;
}

__weak int32_t SendKeyboardPowerCmd(bool boIsPowerOn)
{
	printf("%s, state: %d\n", __FUNCTION__, boIsPowerOn);
	return 0;
}

__weak int32_t 	SendKeyboardConnectCmd(uint8_t u8CurConnect)
{
	printf("%s, state: %d\n", __FUNCTION__, u8CurConnect);
	return 0;
}

__weak int32_t 	SendScreenProtectTimeCmd(uint8_t u8Index)
{
	printf("%s, state: %d\n", __FUNCTION__, u8Index);
	SrceenProtectSetTime(u8Index);
	return 0;
}
__weak int32_t 	SendScreenProtectModeCmd(uint8_t u8Index)
{
	printf("%s, state: %d\n", __FUNCTION__, u8Index);
	SrceenProtectSetMode(u8Index);
	return 0;
}


int32_t GetUnionVolumeValue(uint16_t u16Channel, bool *pValue)
{
	
	if (u16Channel <= _Channel_NormalOut && 
		u16Channel >= _Channel_AIN_Mux && pValue != NULL)
	{
		*pValue = s_stTotalUnifromCheckState.boUniformCheckState[u16Channel];
		return 0;
	}
	
	return -1;
	
}

int32_t GetLogoColor(lv_color24_t *pValue)
{
	if (pValue != NULL)
	{
		*pValue = s_stLogoColor[s_stLogoColorCtrl.u8CurColorIndex];
		return 0;
	}
	return -1;
}

int32_t GetKeyboardPowerValue(bool *pIsPowerOn)
{
	if (pIsPowerOn != NULL)
	{
		*pIsPowerOn = s_stKeyboardCtrl.boIsPowerOn;
		return 0;
	}
	return -1;
}

int32_t GetKeyboardConnectMode(uint8_t *pCurConnect)
{
	if (pCurConnect != NULL)
	{
		*pCurConnect = s_stKeyboardCtrl.u8CurConnect;
		return 0;
	}
	return -1;
}


int32_t SetUnionVolumeValue(uint16_t u16Channel, bool boValue)
{
	if (u16Channel <= _Channel_NormalOut && 
		u16Channel >= _Channel_AIN_Mux)
	{
		s_stTotalUnifromCheckState.boUniformCheckState[u16Channel] = boValue;
		return 0;
	}

	return -1;
}

int32_t SetLogoColor(lv_color24_t stValue)
{
	int32_t i;
	for (i = 0; i < _Logo_Color_Reserved; i++)
	{
		if (memcmp(&stValue, s_stLogoColor + i, sizeof(lv_color24_t)) == 0)	
		{
			s_stLogoColorCtrl.u8CurColorIndex = i;
			SendLogoColorCtrlCmd(stValue);
			return 0;
		}	
	}

	return -1;
}

int32_t SetKeyboardPowerValue(bool boIsPowerOn)
{
	s_stKeyboardCtrl.boIsPowerOn = boIsPowerOn;
	SendKeyboardPowerCmd(boIsPowerOn);
	return 0;
}

int32_t SetKeyboardConnectMode(uint8_t u8CurConnect)
{
	s_stKeyboardCtrl.u8CurConnect = u8CurConnect;
	SendKeyboardConnectCmd(u8CurConnect);
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
	if (pGroup->boIsFixUniformVoume ||
		((pGroup->pUniformVolume != NULL) && lv_sw_get_state(pGroup->pUniformVolume)))
	{
		if ((obj == pGroup->pLeftVolume) && (pGroup->pRightVolume != NULL))
		{
			lv_slider_set_value(pGroup->pRightVolume, u16NewValue);
		}
		else if (pGroup->pLeftVolume != NULL)
		{
			lv_slider_set_value(pGroup->pLeftVolume, u16NewValue);
		}
	}
	{
		StVolume stVolume;

		if (pGroup->pRightVolume == NULL)
		{
			stVolume.u8Channel1 = stVolume.u8Channel2 = (uint8_t)u16NewValue;
		}
		else
		{
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
		u16NewValue &= 0xFF;
		if (pGroup->pTipsAnim != NULL)
		{
			char c8Str[32];
			switch (pGroup->u8Index)
			{
				case _Channel_AIN_1:
				case _Channel_AIN_2:
				case _Channel_AIN_3:
				case _Channel_AIN_4:
				case _Channel_AIN_5:
				{
					sprintf(c8Str, "%.1fDB", c_f32Volume[u16NewValue]);
					break;
				}
				case _Channel_AIN_Mux:
				{
					float f32Tmp = -103.5f;
					f32Tmp = f32Tmp + (float)u16NewValue * 0.5f;
					sprintf(c8Str, "%.1fDB", f32Tmp);
					break;
				}
				case _Channel_PC:
				{
					float f32Tmp = -127.5f;
					f32Tmp = f32Tmp + (float)u16NewValue * 0.5f;
					sprintf(c8Str, "%.1fDB", f32Tmp);
					break;
				}
				case _Channel_HeaderPhone:
				{
					int8_t s8Tmp = 0;
					uint8_t u8Tmp = u16NewValue >> 1;

					if (u8Tmp < 0x30)
					{
						sprintf(c8Str, "mute");
					}
					else
					{
						s8Tmp = -73 + (u8Tmp - 0x30);
					}
					sprintf(c8Str, "%dDB", s8Tmp);
					break;
				}
				case _Channel_InnerSpeaker:
				{
					sprintf(c8Str, "%.1fDB", c_f32Volume[u16NewValue]);
					break;
				}
				case _Channel_PC_Ctrl_Play:
				case _Channel_PC_Ctrl_Record:
				{
					sprintf(c8Str, "%d", ((u16NewValue * 100 + 128) / 255));
					break;
				}
				default:
				{
					sprintf(c8Str, "%dDB", u16NewValue);
					break;
				}


			}

			if (u16NewValue == 0)
			{
				sprintf(c8Str, "mute");
			}
			lv_label_set_text(pGroup->pTipsLabel, c8Str);

			if (pGroup->pUniformVolume != NULL)
			{
				lv_obj_align(pGroup->pTipsLabel, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_MID, 0, -100);
			}
			else
			{
				lv_obj_align(pGroup->pTipsLabel, pGroup->pCtrlMode, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
			}

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
			if (p2ObjTmp[i] != NULL)
			{
				lv_group_remove_obj(p2ObjTmp[i]);
			}
		}

	}

	return 0;
}

int32_t CreateVolumeCtrlGroupMono(
	lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos,

	StVolumeCtrlGroup *pGroup,
	uint8_t u8Index,
	const uint8_t *pCtrlModeIndex,
	uint8_t u8MaxCtrlMode,
	const char *pTitle,
	const char **p2CtrlModeName)
{
	lv_obj_t *pObjTmp;
	if ((pGroup == NULL) || (pParent == NULL) || 
		(pCtrlModeIndex == NULL) || (p2CtrlModeName == NULL))
	{
		return -1;
	}

	if (u16XPos > LV_HOR_RES)
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
			strcat(c8Str, p2CtrlModeName[pGroup->pCtrlModeIndex[i]]);
		}

		pObjTmp = lv_ddlist_create(pParent, NULL);

		if (pObjTmp == NULL)
		{
			goto err;
		}
		pGroup->pCtrlMode = pObjTmp;

		lv_obj_set_pos(pObjTmp, u16XPos, 290 + 25);
		lv_ddlist_set_options(pObjTmp, CHS_TO_UTF8(c8Str));
		lv_ddlist_set_move_dirction(pObjTmp, true);

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
			100);
	}
#if 1
	{/* left volume object */
		pObjTmp = lv_slider_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}

		lv_obj_set_size(pObjTmp, SLIDER_WIDTH * 2, 256);
		lv_slider_set_range(pObjTmp, 0, 255);
		lv_slider_set_knob_radio(pObjTmp, KNOB_WIDTH * 2, KNOB_HEIGHT);
		lv_slider_set_knob_in(pObjTmp, true);
		lv_slider_set_knob_drag_only(pObjTmp, true);
		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_MID, 0, -20);

		pGroup->pLeftVolume = pObjTmp;

	}


	{
		pGroup->boIsFixUniformVoume = true;
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
		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_TOP_MID, 0, -260 - 25);
	}
#endif


	lv_slider_set_action(pGroup->pLeftVolume, ActionSliderCB);

	lv_ddlist_set_action(pGroup->pCtrlMode, ActionCtrlModeDDlist);
	lv_obj_set_top(pGroup->pCtrlMode, true);

	{
		uint32_t i;
		lv_obj_t **p2ObjTmp = (lv_obj_t **)pGroup;
		for (i = 0; i < 4; i++)
		{
			if (p2ObjTmp[i] != NULL)
			{
				lv_obj_set_free_ptr(p2ObjTmp[i], pGroup);
				if (pGlobalGroup != NULL)
				{
					lv_group_add_obj(pGlobalGroup, p2ObjTmp[i]);
				}
			}
		}
		lv_obj_set_free_num(pGroup->pLeftVolume, _OBJ_TYPE_SLIDER);
		lv_obj_set_free_num(pGroup->pCtrlMode, _OBJ_TYPE_DDLIST);

	}

	return 0;

err:

	return -1;

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

	    //lv_ddlist_set_fix_height(pObjTmp, LV_DPI);
	    //lv_ddlist_set_hor_fit(pObjTmp, false);


		//lv_obj_set_width(pObjTmp, 300);

		lv_ddlist_set_move_dirction(pObjTmp, true);

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

		lv_obj_set_size(pObjTmp, SLIDER_WIDTH, 256);
		lv_slider_set_range(pObjTmp, 0, 255);
		lv_slider_set_knob_radio(pObjTmp, KNOB_WIDTH, KNOB_HEIGHT);
		lv_slider_set_knob_in(pObjTmp, true);
		lv_slider_set_knob_drag_only(pObjTmp, true);
		//lv_slider_set_progressive_value(pObjTmp, 20);

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
		lv_slider_set_knob_radio(pObjTmp, KNOB_WIDTH, KNOB_HEIGHT);
		lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
		lv_obj_align(pObjTmp, pGroup->pCtrlMode, LV_ALIGN_OUT_BOTTOM_LEFT, -10, 15);
		pGroup->pUniformVolume = pObjTmp;


		pLab = lv_label_create(pParent, NULL);
		lv_label_set_text(pLab, CHS_TO_UTF8("统一音量"));
		lv_obj_align(pLab, pGroup->pCtrlMode, LV_ALIGN_IN_BOTTOM_RIGHT, 10, 53);
		
		if (boIsFixUniformVoume)
		{
			lv_sw_on(pObjTmp);
			lv_obj_set_click(pObjTmp, false);
			lv_sw_set_style(pObjTmp, LV_SLIDER_STYLE_BG, &s_stStyleSlideDisable.bg);
			lv_sw_set_style(pObjTmp, LV_SLIDER_STYLE_INDIC, &s_stStyleSlideDisable.indic);
			lv_sw_set_style(pObjTmp, LV_SW_STYLE_KNOB_OFF, &s_stStyleSlideDisable.knob);
			lv_sw_set_style(pObjTmp, LV_SW_STYLE_KNOB_ON, &s_stStyleSlideDisable.knob);
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

		lv_obj_set_free_num(pGroup->pLeftVolume, _OBJ_TYPE_SLIDER);
		lv_obj_set_free_num(pGroup->pRightVolume, _OBJ_TYPE_SLIDER);
		lv_obj_set_free_num(pGroup->pCtrlMode, _OBJ_TYPE_DDLIST);
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
	if (pGroup->pMBox == NULL)
	{
		return LV_RES_INV;
	}

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
		//if (pGroup->pFactorySet != NULL)
		//{
		//	lv_group_focus_obj(pGroup->pFactorySet);
		//	//lv_obj_set_free_ptr()
		//}
		lv_group_remove_obj(pGroup->pMBox);
		pGroup->pMBox = NULL;
		lv_mbox_start_auto_close(pParent, 100);
	}



	return LV_RES_INV;
}


lv_res_t ActionMemoryCB(lv_obj_t * obj)
{
	lv_obj_t *pParent = lv_obj_get_parent(obj);
	lv_obj_t *pObjTmp = lv_mbox_create(pParent, NULL);
	StMemoryCtrlGroup *pGroup = (StMemoryCtrlGroup *)lv_obj_get_free_ptr(obj);
	char c8Str[16];

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
	sprintf(c8Str, "存储%d", pGroup->u8TmpMemorySelect + 1);
	lv_mbox_set_text(pObjTmp, CHS_TO_UTF8(c8Str));
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
		lv_label_set_text(pObjTmp, CHS_TO_UTF8( "复位"));

		lv_obj_align(pGroup->pFactorySet, pGroup->pMemoryCtrl, LV_ALIGN_OUT_RIGHT_TOP, 65, 0);

		lv_btn_set_action(pGroup->pFactorySet, LV_BTN_ACTION_CLICK, ActionFactoryCB);
	}

	if (pGlobalGroup != NULL)
	{
		lv_group_add_obj(pGlobalGroup, pGroup->pMemoryCtrl);
		lv_group_add_obj(pGlobalGroup, pGroup->pFactorySet);
	}

	lv_obj_set_free_ptr(pGroup->pMemoryCtrl, pGroup);
	lv_obj_set_free_ptr(pGroup->pFactorySet, pGroup);
	lv_obj_set_free_num(pGroup->pFactorySet, _OBJ_TYPE_BTN);
	lv_obj_set_free_num(pGroup->pMemoryCtrl, _OBJ_TYPE_DDLIST);


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
			lv_slider_set_knob_radio(pObjTmp, KNOB_WIDTH, KNOB_HEIGHT);
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
			lv_obj_set_pos(pObjTmp, u16XPos + j % 2 * 180, u16YPos + 40 + ((j / 2) * 63));
			lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
			lv_slider_set_knob_radio(pObjTmp, KNOB_WIDTH, KNOB_HEIGHT);
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

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("输出使能"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);
	}
	{
		uint16_t /*i, */j;
		const char *const pOutName[ENABLE_OUTPUT_CTRL] =
		{
			"PC", "AUX", "MIX",
		};
		for (j = 0; j < ENABLE_OUTPUT_CTRL; j++)
		{
			lv_obj_t *pLab;
			pObjTmp = lv_sw_create(pParent, NULL);
			lv_obj_set_pos(pObjTmp, u16XPos + j % 2 * 180, u16YPos + 40 + ((j / 2) * 63));
			lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
			lv_slider_set_knob_radio(pObjTmp, KNOB_WIDTH, KNOB_HEIGHT);
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
	CreateVolumeCtrlGroupMono(pTabParent, pGroup, 135, &stVolumeInput1, _Channel_AIN_1,
		c_u8CtrlModeSpecialLeft, sizeof(c_u8CtrlModeSpecialLeft), "输入1", c_pCtrlModeSpecial);

	CreateVolumeCtrlGroupMono(pTabParent, pGroup, 470, &stVolumeInput2, _Channel_AIN_2,
		c_u8CtrlModeSpecialRight, sizeof(c_u8CtrlModeSpecialRight), "输入2", c_pCtrlModeSpecial);

	return 0;
}

int32_t RebulidVolumeCtrlValue(uint16_t u16Index)
{
	const StVolumeCtrlGroup *pGroup = NULL;

	u16Index = ChannelToReal(u16Index);

	if (u16Index >= TOTAL_CHANNEL)
	{
		return -1;
	}

	pGroup = c_pValumeCtrlArr[u16Index];

	if ((pGroup->pLeftVolume != NULL) &&
		(lv_slider_get_value(pGroup->pLeftVolume) != s_stTotalCtrlMemroy.stVolume[u16Index].u8Channel1))
	{
		lv_slider_set_value(pGroup->pLeftVolume, s_stTotalCtrlMemroy.stVolume[u16Index].u8Channel1);
	}
	if ((pGroup->pRightVolume != NULL) && 
		(lv_slider_get_value(pGroup->pRightVolume) != s_stTotalCtrlMemroy.stVolume[u16Index].u8Channel2))
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
	if ((!pGroup->boIsFixUniformVoume) && (pGroup->pUniformVolume != NULL))
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
	CreateVolumeCtrlGroupMono(pTabParent, pGroup, 30, &stVolumeInput3, _Channel_AIN_3,
		c_u8CtrlModeSpecialLeft, sizeof(c_u8CtrlModeSpecialLeft), "输入3", c_pCtrlModeSpecial);

	CreateVolumeCtrlGroupMono(pTabParent, pGroup, 30 + 270, &stVolumeInput4, _Channel_AIN_4,
		c_u8CtrlModeSpecialRight, sizeof(c_u8CtrlModeSpecialRight), "输入4", c_pCtrlModeSpecial);

	CreateVolumeCtrlGroupMono(pTabParent, pGroup, 30 + 270 * 2, &stVolumeInput5, _Channel_AIN_5,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "输入5", c_pCtrlMode);

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

int32_t ReleaseTableI2SCtrl(lv_obj_t *pTabParent)
{
	ReleaseVolumeCtrlGroup(&stVolumeInputMux);
	ReleaseVolumeCtrlGroup(&stVolumeInputPC);
	return 0;
}

int32_t CreateTableI2SCtrl(lv_obj_t *pTabParent, lv_group_t *pGroup)
{
	CreateVolumeCtrlGroup(pTabParent, pGroup, 135, &stVolumeInputMux, _Channel_AIN_Mux,
		c_u8CtrlMode4, sizeof(c_u8CtrlMode4), "MIX", false);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 470, &stVolumeInputPC, _Channel_PC,
		c_u8CtrlMode7, sizeof(c_u8CtrlMode7), "PC", false);

	return 0;
}

int32_t RebulidTableI2SCtrlVaule(void)
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
	CreateVolumeCtrlGroup(pTabParent, pGroup, 135, &stVolumeOutputHeaderPhone, _Channel_HeaderPhone,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "耳机", false);

	CreateVolumeCtrlGroup(pTabParent, pGroup, 470, &stVolumeOutputInnerSpeaker, _Channel_InnerSpeaker,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "输出", true);

	//CreateVolumeCtrlGroup(pTabParent, pGroup, 30 + 270 * 2, &stVolumeOutput, _Channel_NormalOut,
	//	c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "输出", false);
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
	//RebulidVolumeCtrlValue(_Channel_NormalOut);
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

int32_t CreateTablePCVolumeCtrl(lv_obj_t *pTabParent, lv_group_t *pGroup)
{
	CreateVolumeCtrlGroupMono(pTabParent, pGroup, 135, &stVolumePCCtrlPlay, _Channel_PC_Ctrl_Play,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "播放", c_pCtrlModeSpecial);

	CreateVolumeCtrlGroupMono(pTabParent, pGroup, 470, &stVolumePCCtrlRecord, _Channel_PC_Ctrl_Record,
		c_u8CtrlMode2, sizeof(c_u8CtrlMode2), "录制", c_pCtrlModeSpecial);

	return 0;
}

int32_t RebulidTablePCVolumeCtrlVaule(void)
{
	if (s_pTableView == NULL)
	{
		return -1;
	}

	RebulidVolumeCtrlValue(_Channel_PC_Ctrl_Play);
	RebulidVolumeCtrlValue(_Channel_PC_Ctrl_Record);

	return 0;
}
int32_t ReleaseTablePCVolumeCtrl(lv_obj_t *pTabParent)
{
	ReleaseVolumeCtrlGroup(&stVolumePCCtrlPlay);
	ReleaseVolumeCtrlGroup(&stVolumePCCtrlRecord);
	return 0;
}



lv_color_t CreateLogoColor(lv_color24_t color, uint32_t opa)
{
	return LV_COLOR_MAKE((color.red * opa / 255), (color.green * opa / 255),
		(color.blue * opa / 255));
}

void CreateLogoStyle(void)
{
	static bool boIsCreate = false;
	int32_t i;

	if (boIsCreate)
	{
		return;
	}

	for (i = 0; i < _Logo_Color_Reserved; i++)
	{
		lv_style_t *pStyle1 = lv_theme_get_current()->panel;
		lv_style_t *pStyle = &(s_stLogoStyle[i][_Logo_State_TGL_REL]);
		lv_color24_t stColor = s_stLogoColor[i];
		lv_style_copy(pStyle, pStyle1);
		pStyle->body.main_color = CreateLogoColor(stColor, LIGHT_ON);
		pStyle->body.grad_color = CreateLogoColor(stColor, LIGHT_GRAD);
		pStyle->body.border.color = CreateLogoColor(stColor, LIGHT_OFF);
		pStyle->body.shadow.color = CreateLogoColor(stColor, LIGHT_OFF);
		pStyle->body.shadow.width = 10;
		pStyle->body.border.opa = LV_OPA_30;


		pStyle1 = pStyle;
		pStyle = &(s_stLogoStyle[i][_Logo_State_Press]);
		lv_style_copy(pStyle, pStyle1);
		pStyle->body.main_color = CreateLogoColor(stColor, LIGHT_PRESS);

		pStyle1 = pStyle;
		pStyle = &(s_stLogoStyle[i][_Logo_State_REL]);
		lv_style_copy(pStyle, pStyle1);
		pStyle->body.main_color = CreateLogoColor(stColor, LIGHT_OFF);
		pStyle->body.border.color =
		pStyle->body.shadow.color = CreateLogoColor(stColor, LIGHT_SHADOW);
	}
	boIsCreate = true;
}


static lv_res_t ActionLogoColorCtrl(lv_obj_t * btn)
{
	int32_t i;
	StLogoColorCtrl *pCtrl = (StLogoColorCtrl *)lv_obj_get_free_ptr(btn);
	lv_color24_t stColor = {0, 0, 0};

	printf("btn state: %d\n", lv_btn_get_state(btn));
	for (i = 0; i < _Logo_Color_Reserved; i++)
	{
		if (btn != pCtrl->pObjColor[i])
		{
			if (lv_btn_get_state(pCtrl->pObjColor[i]) != LV_BTN_STATE_REL)
			{
				lv_btn_set_state(pCtrl->pObjColor[i], LV_BTN_STATE_REL);
			}
		}
		else
		{
			if (lv_btn_get_state(btn) == LV_BTN_STATE_TGL_REL)
			{
				stColor = s_stLogoColor[i];
				pCtrl->u8CurColorIndex = i;
			}
			else
			{
				pCtrl->u8CurColorIndex = 0xFF;
			}

		}
	}
	SendLogoColorCtrlCmd(stColor);
	return LV_RES_OK;
}



int32_t CreateLogoColorCtrl(lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos, uint16_t u16YPos,
	StLogoColorCtrl *pGroup)
{
	lv_obj_t *pObjTmp;
	int32_t i;
	if ((pGroup == NULL) || (pParent == NULL))
	{
		return -1;
	}

	CreateLogoStyle();

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("LOGO颜色"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);
	}

	for (i = 0; i < _Logo_Color_Reserved; i++)
	{
		pGroup->pObjColor[i] = NULL;
		pObjTmp = lv_btn_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			continue;
		}
		lv_btn_set_action(pObjTmp, LV_BTN_ACTION_CLICK, ActionLogoColorCtrl);
		lv_obj_set_pos(pObjTmp, u16XPos + i % 2 * 180, u16YPos + 40 + ((i / 2) * 63));
		lv_obj_set_size(pObjTmp, SW_HTIGHT * 2, SW_HTIGHT);
		lv_btn_set_toggle(pObjTmp, true);

		lv_btn_set_style(pObjTmp, LV_BTN_STYLE_REL, &s_stLogoStyle[i][_Logo_State_REL]);
		lv_btn_set_style(pObjTmp, LV_BTN_STYLE_PR, &s_stLogoStyle[i][_Logo_State_Press]);
		lv_btn_set_style(pObjTmp, LV_BTN_STYLE_TGL_PR, &s_stLogoStyle[i][_Logo_State_Press]);
		lv_btn_set_style(pObjTmp, LV_BTN_STYLE_TGL_REL, &s_stLogoStyle[i][_Logo_State_TGL_REL]);
		
		lv_obj_set_free_ptr(pObjTmp, pGroup);

		if (pGlobalGroup != NULL)
		{
			lv_group_add_obj(pGlobalGroup, pObjTmp);
		}
		pGroup->pObjColor[i] = pObjTmp;
	}

	return 0;

}


lv_res_t ActionKeyboardPowerCB(lv_obj_t *pObj)
{
	StKeyboardCtrl *pGroup = lv_obj_get_free_ptr(pObj);
	if (lv_sw_get_state(pObj))
	{
		pGroup->boIsPowerOn = true;
	}
	else
	{
		pGroup->boIsPowerOn = false;
	}

	SendKeyboardPowerCmd(pGroup->boIsPowerOn);
	printf("set the keyboard power %s\n", lv_sw_get_state(pObj) ? "ON" : "OFF");

	return LV_RES_OK;
}

lv_res_t ActionKeyboardConnectCB(lv_obj_t *pObj)
{
	StKeyboardCtrl *pGroup = lv_obj_get_free_ptr(pObj);

	pGroup->u8CurConnect = (uint8_t)lv_ddlist_get_selected(pObj);

	SendKeyboardConnectCmd(pGroup->u8CurConnect);
	printf("set the keyboard connect %s\n", pGroup->u8CurConnect ? "UART" : "HID");

	return LV_RES_OK;
}


int32_t CreateKeyBoardCtrl(lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos, uint16_t u16YPos,
	StKeyboardCtrl *pGroup)
{
	lv_obj_t *pObjTmp;

	if ((pGroup == NULL) || (pParent == NULL))
	{
		return -1;
	}

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("键盘控制"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);
	}

	{
		lv_obj_t *pLab = NULL;
		lv_obj_t *pObjTmp = lv_sw_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}
		lv_slider_set_knob_radio(pObjTmp, KNOB_WIDTH, KNOB_HEIGHT);
		lv_obj_set_size(pObjTmp, SW_WIDTH, SW_HTIGHT);
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos + 40);
		lv_sw_on(pObjTmp);
		pGroup->pPowerCtrl = pObjTmp;

		pLab = lv_label_create(pParent, NULL);
		lv_label_set_text(pLab, CHS_TO_UTF8("电源"));
		lv_obj_align(pLab, pGroup->pPowerCtrl, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

		if (pGlobalGroup != NULL)
		{
			lv_group_add_obj(pGlobalGroup, pObjTmp);
		}

	}

	{
		pObjTmp = lv_ddlist_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}
		pGroup->pConnectCtrl = pObjTmp;

		lv_obj_set_pos(pObjTmp, u16XPos + 180, u16YPos + 40);

		lv_ddlist_set_options(pObjTmp, CHS_TO_UTF8("HID\nUART"));

		lv_ddlist_set_move_dirction(pObjTmp, true);

		lv_label_set_align(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
			LV_LABEL_ALIGN_CENTER);

		lv_label_set_long_mode(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
			LV_LABEL_LONG_BREAK);

		lv_obj_set_width(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label, 80);

		if (pGlobalGroup != NULL)
		{
			lv_group_add_obj(pGlobalGroup, pObjTmp);
		}
	}

	lv_obj_set_free_ptr(pGroup->pConnectCtrl, pGroup);
	lv_obj_set_free_ptr(pGroup->pPowerCtrl, pGroup);

	lv_sw_set_action(pGroup->pPowerCtrl, ActionKeyboardPowerCB);
	lv_ddlist_set_action(pGroup->pConnectCtrl, ActionKeyboardConnectCB);
	lv_obj_set_free_num(pGroup->pConnectCtrl, _OBJ_TYPE_DDLIST);

	return 0;
err:
	return -1;
}


int32_t RebulidLogoColorCtrlValue(StLogoColorCtrl *pGroup)
{
	if (pGroup->u8CurColorIndex < _Logo_Color_Reserved)
	{
		lv_btn_set_state(pGroup->pObjColor[pGroup->u8CurColorIndex], LV_BTN_STATE_TGL_REL);
	}
	return 0;
}

int32_t RebulidKeyBoardCtrlValue(StKeyboardCtrl *pGroup)
{
	lv_ddlist_set_selected(pGroup->pConnectCtrl, pGroup->u8CurConnect);
	if (pGroup->boIsPowerOn)
	{
		lv_sw_on(pGroup->pPowerCtrl);
	}
	else
	{
		lv_sw_off(pGroup->pPowerCtrl);
	}

	return 0;
}

int32_t CreateTablePeripheralCtrl(lv_obj_t *pParent, lv_group_t *pGroup)
{
	CreateLogoColorCtrl(pParent, pGroup, 20, 20, &s_stLogoColorCtrl);
	CreateKeyBoardCtrl(pParent, pGroup, 20, 200, &s_stKeyboardCtrl);
	return 0;
}

int32_t RebulidTablePeripheralVaule(void)
{
	RebulidLogoColorCtrlValue(&s_stLogoColorCtrl);
	RebulidKeyBoardCtrlValue(&s_stKeyboardCtrl);
	return 0;
}

lv_res_t ActionScreenProtectTimeCB(lv_obj_t *pObj)
{
	StScreenProtectCtrl *pGroup = lv_obj_get_free_ptr(pObj);

	pGroup->u8CurTimeIndex = (uint8_t)lv_ddlist_get_selected(pObj);

	SendScreenProtectTimeCmd(pGroup->u8CurTimeIndex);

	printf("set the screen protect time %s\n", c_pScreenProtectTime[pGroup->u8CurTimeIndex]);

	return LV_RES_OK;
}


lv_res_t ActionScreenProtectModeCB(lv_obj_t *pObj)
{
	StScreenProtectCtrl *pGroup = lv_obj_get_free_ptr(pObj);

	pGroup->u8CurModeIndex = (uint8_t)lv_ddlist_get_selected(pObj);

	SendScreenProtectModeCmd(pGroup->u8CurModeIndex);

	printf("set the screen protect mode %s\n", c_pScreenProtectMode[pGroup->u8CurModeIndex]);

	return LV_RES_OK;
}


int32_t CreateScreenProtectCtrl(lv_obj_t *pParent,
	lv_group_t *pGlobalGroup,
	uint16_t u16XPos, uint16_t u16YPos,
	StScreenProtectCtrl *pGroup)
{
	lv_obj_t *pObjTmp;

	if ((pGroup == NULL) || (pParent == NULL))
	{
		return -1;
	}

	{
		pObjTmp = lv_label_create(pParent, NULL);
		lv_label_set_text(pObjTmp, CHS_TO_UTF8("屏幕保护"));
		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos);
	}

	{
		lv_obj_t *pLab = NULL;
		lv_obj_t *pObjTmp = NULL;
		char c8Str[96];
		uint32_t i;
		c8Str[0] = 0;
		for (i = 0; i < _ScreenProtect_Reserved; i++)
		{
			if (i != 0)
			{
				strcat(c8Str, "\n");
			}
			strcat(c8Str, c_pScreenProtectTime[i]);
		}

		
		pObjTmp = lv_ddlist_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}
		pGroup->pTimeCtrl = pObjTmp;

		lv_obj_set_pos(pObjTmp, u16XPos, u16YPos + 40);

		lv_ddlist_set_options(pObjTmp, CHS_TO_UTF8(c8Str));

		lv_label_set_align(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
			LV_LABEL_ALIGN_CENTER);

		lv_label_set_long_mode(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
			LV_LABEL_LONG_BREAK);

		lv_obj_set_width(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label, 120);

		if (pGlobalGroup != NULL)
		{
			lv_group_add_obj(pGlobalGroup, pObjTmp);
		}

	}

	{
		char c8Str[96];
		uint32_t i;
		c8Str[0] = 0;
		for (i = 0; i < _ScreenProtect_Mode_Reserved; i++)
		{
			if (i != 0)
			{
				strcat(c8Str, "\n");
			}
			strcat(c8Str, c_pScreenProtectMode[i]);
		}

		pObjTmp = lv_ddlist_create(pParent, NULL);
		if (pObjTmp == NULL)
		{
			goto err;
		}
		pGroup->pModeCtrl = pObjTmp;

		lv_obj_set_pos(pObjTmp, u16XPos + 180, u16YPos + 40);


		lv_ddlist_set_options(pObjTmp, CHS_TO_UTF8(c8Str));

		lv_label_set_align(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
			LV_LABEL_ALIGN_CENTER);

		lv_label_set_long_mode(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label,
			LV_LABEL_LONG_BREAK);

		lv_obj_set_width(((lv_ddlist_ext_t *)lv_obj_get_ext_attr(pObjTmp))->label, 80);

		if (pGlobalGroup != NULL)
		{
			lv_group_add_obj(pGlobalGroup, pObjTmp);
		}
	}

	lv_obj_set_free_ptr(pGroup->pTimeCtrl, pGroup);
	lv_obj_set_free_ptr(pGroup->pModeCtrl, pGroup);

	lv_obj_set_free_num(pGroup->pTimeCtrl, _OBJ_TYPE_DDLIST);
	lv_obj_set_free_num(pGroup->pModeCtrl, _OBJ_TYPE_DDLIST);

	lv_ddlist_set_action(pGroup->pTimeCtrl, ActionScreenProtectTimeCB);
	lv_ddlist_set_action(pGroup->pModeCtrl, ActionScreenProtectModeCB);

	return 0;
err:
	return -1;
}


int32_t RebuildScreenProtectCtrlValue(StScreenProtectCtrl *pGroup)
{
	lv_ddlist_set_selected(pGroup->pTimeCtrl, pGroup->u8CurTimeIndex);
	lv_ddlist_set_selected(pGroup->pModeCtrl, pGroup->u8CurModeIndex);

	return 0;
}

int32_t CreateTableSystemSetCtrl(lv_obj_t *pParent, lv_group_t *pGroup)
{
	CreateScreenProtectCtrl(pParent, pGroup, 20, 20, &s_stScreenProtectCtrl);
	return 0;
}

int32_t RebulidTableSystemSetVaule(void)
{
	RebuildScreenProtectCtrlValue(&s_stScreenProtectCtrl);
	return 0;
}



const PFUN_ReleaseTable c_pFUN_ReleaseTable[_Tab_Reserved] = 
{
	ReleaseTableInput1To2,
	ReleaseTableInput3To5,
	ReleaseTableI2SCtrl,
	ReleaseTableOutputCtrl,
	ReleaseTableOtherCtrl,
	ReleaseTablePCVolumeCtrl,
	NULL,
};

const PFUN_CreateTable c_pFUN_CreateTable[_Tab_Reserved] =
{
	CreateTableInput1To2,
	CreateTableInput3To5,
	CreateTableI2SCtrl,
	CreateTableOutputCtrl,
	CreateTableOtherCtrl,
	CreateTablePCVolumeCtrl,
	CreateTablePeripheralCtrl,
	CreateTableSystemSetCtrl,
};

const PFUN_RebulidTableValue c_pFun_RebulidTableValue[_Tab_Reserved] =
{
	RebulidTableInput1To2Vaule,
	RebulidTableInput3To5Vaule,
	RebulidTableI2SCtrlVaule,
	RebulidTableOutputVaule,
	RebulidTableOtherVaule,
	RebulidTablePCVolumeCtrlVaule,
	RebulidTablePeripheralVaule,
	RebulidTableSystemSetVaule,
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
#if 1
		lv_obj_t *pParent = NULL;
		if (lv_obj_get_free_ptr(pTabPage) == NULL)
		{
			lv_obj_t *pObjTmp = lv_cont_create(pTabPage, NULL);
			lv_obj_set_style(pObjTmp, &lv_style_transp);
			lv_obj_set_click(pObjTmp, false);
			lv_obj_set_height(pObjTmp, lv_obj_get_height(pTabPage));
			lv_obj_set_width(pObjTmp, lv_obj_get_width(pTabPage));
			lv_obj_set_free_ptr(pTabPage, pObjTmp);

			pParent = pObjTmp;
		}
		else
		{
			pParent = (lv_obj_t *)lv_obj_get_free_ptr(pTabPage);
		}
#endif
		if (c_pFUN_CreateTable[u16TableIndex](pParent,
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
				if (u16ActiveTableIndex != _Tab_Input_I2S_Ctrl)
				{
					return 0;
				}
			}
			else if (u32Channel <= _Channel_InnerSpeaker)
			{
				if (u16ActiveTableIndex != _Tab_Output)
				{
					return 0;
				}
			}
			else if (u32Channel <= _Channel_Reserved)
			{
				return -1;
			}
			else if (u32Channel <= _Channel_PC_Ctrl_Record)
			{
				if (u16ActiveTableIndex != _Tab_PC_Volume_Ctrl)
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

void GroupFocusCB(lv_group_t * pGroup)
{
	lv_obj_t *pObj = lv_group_get_focused(pGroup);
	lv_obj_type_t stType = { NULL };
	lv_obj_get_type(pObj, &stType);
	printf("object is: %s\n", stType.type[0]);

}


lv_res_t ActionTabPagePressRelease(struct _lv_obj_t * obj)
{
	lv_obj_t *pChild = lv_obj_get_child(obj, NULL);
	if (pChild == NULL)
	{
		return LV_RES_OK;
	}

	pChild = lv_obj_get_child(pChild, NULL);
	if (pChild == NULL)
	{
		return LV_RES_OK;
	}

	pChild = lv_obj_get_child(pChild, NULL);

	while(pChild != NULL)
	{
#if (defined _WIN32) && 0

		lv_obj_type_t stType = { NULL };
		lv_obj_get_type(pChild, &stType);
		printf("child has: %s\n", stType.type[0]);
#endif
		if (lv_obj_get_free_num(pChild) == _OBJ_TYPE_DDLIST)
		{
			lv_ddlist_close(pChild, true);
		}

		pChild = lv_obj_get_child(obj, pChild);
	};

	
	return LV_RES_OK;
}


int32_t SlideDisableStyleInit(void)
{
	lv_style_copy(&s_stStyleSlideDisable.bg, (lv_theme_get_current()->slider.bg));
	lv_style_copy(&s_stStyleSlideDisable.indic, (lv_theme_get_current()->slider.indic));
	lv_style_copy(&s_stStyleSlideDisable.knob, (lv_theme_get_current()->slider.knob));
	
	s_stStyleSlideDisable.bg.body.border.color = 
	s_stStyleSlideDisable.bg.body.main_color = lv_color_hsv_to_rgb(120, 40, 60);
	
	s_stStyleSlideDisable.indic.body.main_color = 
	s_stStyleSlideDisable.indic.body.grad_color = lv_color_hsv_to_rgb(120, 40, 60);

	s_stStyleSlideDisable.knob.body.main_color =
	s_stStyleSlideDisable.knob.body.grad_color = lv_color_hsv_to_rgb(120, 40, 60);

	return 0;

}


int32_t CreateTableView(void)
{
	lv_theme_t * lv_theme_zen_init(uint16_t hue, lv_font_t *font);

	//lv_font_add(&lv_font_symbol_20, &lv_font_chs_24);
	s_pTheme = lv_theme_zen_init(120, &lv_font_chs_24);
	//s_pTheme = lv_theme_alien_init(120, &lv_font_chs_24);

	if (s_pTheme == NULL)
	{
		return -1;
	}

	lv_theme_set_current(s_pTheme);

	SlideDisableStyleInit();
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
#if ((defined _WIN32) || 1)
		lv_indev_drv_t kb_drv;
		kb_drv.type = LV_INDEV_TYPE_KEYPAD;
		kb_drv.read = keyboard_read;
		lv_indev_t *kb_indev = lv_indev_drv_register(&kb_drv);
		lv_indev_set_group(kb_indev, s_pGroup);
#endif
		if (s_pGroup != NULL)
		{
			lv_group_add_obj(s_pGroup, pTableView);

			lv_group_set_focus_cb(s_pGroup, GroupFocusCB);
		}

		lv_obj_set_free_ptr(pTableView, s_pGroup);

	}

	lv_obj_t *pTab[_Tab_Reserved] = { NULL };

	uint8_t i;
	for (i = 0; i < _Tab_Reserved; i++)
	{
		pTab[i] = lv_tabview_add_tab(pTableView, CHS_TO_UTF8(c_pTableName[i]));
		lv_page_set_rel_action(pTab[i], ActionTabPagePressRelease);
	}
	for (i = 0; i < _Tab_Reserved; i++)
	{
		lv_page_set_scrl_fit(pTab[i], false, false);
		lv_page_set_scrl_height(pTab[i], lv_obj_get_height(pTab[i]) - 16);
		lv_page_set_sb_mode(pTab[i], LV_SB_MODE_OFF);
	}


	s_pTableView = pTableView;
	CreateTable(pTab[_Tab_Input_I2S_Ctrl], _Tab_Input_I2S_Ctrl);

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

	lv_tabview_set_tab_act(pTableView, _Tab_Input_I2S_Ctrl, false);

	lv_tabview_set_tab_load_action(pTableView, ActionTabview);

	//s_pTableView = pTableView;

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

void SetKeySpeek(uint16_t u16Speed)
{
	lv_obj_t *pObj;
	if (s_pGroup == NULL)
	{
		return;
	}
	
	pObj = lv_group_get_focused(s_pGroup);
	
	if (pObj != NULL)
	{
		if (lv_obj_get_free_num(pObj) == _OBJ_TYPE_SLIDER)
		{
			lv_slider_set_progressive_value(pObj, u16Speed);
		}
	}
}


void SetKeyValue(uint32_t u32Key, bool boIsPress)
{
	if (u32Key == LV_GROUP_KEY_ENTER)
	{
		lv_obj_t *pObj;

		if (s_pGroup == NULL)
		{
			return;
		}

		pObj = lv_group_get_focused(s_pGroup);

		if (pObj != NULL)
		{
			if (lv_obj_get_free_num(pObj) == _OBJ_TYPE_BTN)
			{
				if (!lv_obj_get_click(pObj))
				{
					return;
				}
				lv_btn_set_state(pObj, boIsPress ? LV_BTN_STATE_PR : LV_BTN_STATE_REL);
			}
		}

	}

}


#endif
