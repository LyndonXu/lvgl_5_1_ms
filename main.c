/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#if !defined _WIN32
#include <unistd.h>
#else
#include <stdio.h>
#include <string.h>
#include <windows.h>
#endif
#include <string.h>
#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "lv_examples/lv_apps/benchmark/benchmark.h"
#include "lv_examples/lv_tests/lv_test_theme/lv_test_theme.h"
#include "lv_examples/lv_tests/lv_test_group/lv_test_group.h"
#include "lv_examples/lv_tests/lv_test.h"
#include "lv_examples/lv_tutorial/8_animations/lv_tutorial_animations.h"

#include "gui.h"
#include "C2D.h"
#include "logo.h"
#include "screen_protect.h"

 /*********************
 *      DEFINES
 *********************/
#define TEST_GUI

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static int tick_thread(void *data);


#undef main
int main(int argc, char** argv)
{
	/*Initialize LittlevGL*/
	lv_init();

	/*Initialize the HAL for LittlevGL*/
	hal_init();
	GUI_Init();
	LOGOCoordsInit();

	if (0)
	{
		LOGODraw();

		Sleep(3 * 1000);
	}

	/*Load a demo*/
	//demo_create();
	/*Or try the benchmark too to see the speed of your MCU*/
	//benchmark_create();
	/*Or try a theme (Enable the theme in lv_conf.h with USE_LV_THEME_...  1 )*/
	//lv_theme_t * th = lv_theme_night_init(210, NULL); /*Hue: 210; Font: NULL (default)*/
	//lv_theme_t * th = lv_theme_alien_init(210, NULL); /*Hue: 210; Font: NULL (default)*/
	//lv_test_theme_1(th);
	//lv_test_btn_1();
#if ((defined TEST_GUI) && 1)
	{
		ReflushLanguageInit();
		CreateTableInit();
		CreateTableView(~0);
	}
#endif
	if (0)
	{
		lv_theme_t *s_pTheme = lv_theme_zen_init(120, NULL);

		if (s_pTheme == NULL)
		{
			return -1;
		}

		lv_theme_set_current(s_pTheme);

		lv_obj_set_style(lv_scr_act(), s_pTheme->bg);

		{
			lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
			lv_btn_set_state(btn1, LV_BTN_STATE_INA);
		}

		//lv_test_bar_1();
		//lv_test_cont_1();
		//lv_tutorial_animations();
		//lv_test_mbox_1();
		//lv_test_btn_1();
	}

	//lv_test_ddlist_1();

	//lv_test_group_1();
	//lv_test_img_1();
	while (1)
	{
		/* Periodically call the lv_task handler.
		 * It could be done in a timer interrupt or an OS task too.*/
		if (!SrceenProtectIsStart())
		{
			lv_task_handler();
		}
		ReflushLanguage();
#if (defined TEST_GUI) && 1
		{
			static int cnt = 0;
			if (cnt == 25)
			{
				cnt = 0;
				{
					static lv_mem_monitor_t stMemMonitor_bak;
					static lv_obj_t * label = NULL;
					lv_mem_monitor_t stMemMonitor;
					char c8Buf[64];


					lv_mem_monitor(&stMemMonitor);
					sprintf(c8Buf, "Total: %d, free: %d%%, using: %d",
						stMemMonitor.total_size, stMemMonitor.free_size * 100 / stMemMonitor.total_size,
						stMemMonitor.total_size - stMemMonitor.free_size);
					if (label != NULL)
					{
						lv_obj_del(label);
					}
					label = lv_label_create(lv_scr_act(), NULL);
					if (label != NULL)
					{
						lv_label_set_text(label, c8Buf);
						lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -50);
					}
				}
			
			}
			if (cnt == 6)
			{
				static uint8_t u8Cnt = 0;
				u8Cnt++;

				if (u8Cnt == 40)
				{
					{
						StVolume stVolume = { rand(), rand() };
						SetAudioVolume(_Channel_AIN_Mux, stVolume);
						{
							StVolumeCtrlEnable stEnable;
							stEnable.boIsVolumeCtrlDisable = (rand() & 0x01);
							stEnable.boIsCtrlModeDisable = (rand() & 0x01);
							stEnable.boIsUniformVoumeDisable = (rand() & 0x01);
							SetVolumeCtrlState(_Channel_AIN_Mux, &stEnable);
						}
						ReflushActiveTable(_Fun_AudioVolume, _Channel_AIN_Mux);
						u8Cnt = 0;
					}
					{
						const char *pList[2] = 
						{
							"List1_1\nList1_2\nList1_3",
							"List2_1\nList2_2\nList2_3"
						};

						SetAudioDeviceList(_Channel_PC_Ctrl_Play/* + (rand() & 0x01)*/,
							pList[(rand() & 0x01)], -1);
						SetAudioDeviceListIndex(_Channel_PC_Ctrl_Play + (rand() & 0x01),
							(rand() % 0x03));
					}
				}


			}
			if (cnt == 12)
			{
				void BarValueTest(void);
				BarValueTest();
			}
			cnt++;
		}
#endif
#if !defined _WIN32
		usleep(1000); /*Just to let the system breath*/
#else
		Sleep(1);
#endif
#if 0
		{
			void flush_style(void);
			flush_style();
		}
#endif

		SrceenProtectFlush();
	}

	return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the Littlev graphics library
 */
static void hal_init(void)
{
	/* Add a display
	 * Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
	monitor_init();
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv); /*Basic initialization*/
	disp_drv.disp_flush = monitor_flush;
	disp_drv.disp_fill = monitor_fill;
	disp_drv.disp_map = monitor_map;
	lv_disp_drv_register(&disp_drv);

	/* Add the mouse (or touchpad) as input device
	 * Use the 'mouse' driver which reads the PC's mouse*/
	mouse_init();
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv); /*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read = mouse_read; /*This function will be called periodically (by the library) to get the mouse position and state*/
	lv_indev_drv_register(&indev_drv);

	lv_indev_drv_init(&indev_drv); /*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_KEYPAD;
	indev_drv.read = keyboard_read; /*This function will be called periodically (by the library) to get the mouse position and state*/
	lv_indev_drv_register(&indev_drv);
	/* Tick init.
	 * You have to call 'lv_tick_handler()' in every milliseconds
	 * Create an SDL thread to do this*/
	SDL_CreateThread(tick_thread, "tick", NULL);
}

/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data)
{
	while (1)
	{
		lv_tick_inc(1);
		SDL_Delay(1); /*Sleep for 1 millisecond*/
	}

	return 0;
}
