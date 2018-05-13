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

/**********************
 *  STATIC VARIABLES
 **********************/
typedef struct _tagStPentagon
{
	GUI_POINT stPoint[5];
	lv_color24_t stColor;
}StPentagon;

typedef struct _tagStTactangle
{
	GUI_POINT stPoint[4];
	lv_color24_t stColor;
}StTactangle;

typedef struct _tagStCycle
{
	uint16_t x;
	uint16_t y;
	uint16_t r;
	lv_color24_t stColor;
}StCycle;

static StPentagon s_stPentagon[3] =
{
	{
		{ { 52, 395, },{ 750, 180, },{ 1075, 368 },{ 976, 866 },{ 21, 866 } },
		{75, 29, 21},
	},
	{ 
		{ { 78, 406, },{ 750, 200, },{ 1046, 372 },{ 954, 843 },{ 48, 843 } },
		{ 51, 51, 51 }
	},
	{
		{ { 110, 420, },{ 722, 233, },{ 1003, 396, },{ 915, 817, },{ 84, 817 } },
		{ 75, 29, 21 }
	},
};

static StTactangle s_stTactangle[3] =
{
	{
		{ { 234, 817, },{ 255, 600, },{ 316, 600, },{ 297, 817, }, },
		{ 51, 51, 51 }
	},
	{
		{ { 472, 610, },{ 534, 280, },{ 595, 260, },{ 535, 614, }, },
		{ 51, 51, 51 }
	},
	{
		{ { 712, 613, },{ 776, 619, },{ 793, 524, },{ 727, 517 }, },
		{33, 101, 230},
	},
};

static StCycle s_stCycle[4] =
{
	{
		290, 535, 75,
		{ 51, 51, 51 }
	},
	{
		500, 679, 75,
		{ 51, 51, 51 }
	},
	{
		768, 468, 71,
		{ 33, 101, 230 },
	},
	{
		739, 680, 71,
		{ 33, 101, 230 },
	},
};

void LOGOCoordsInit(void)
{
	uint32_t i;
	for (i = 0; i < 3; i++)
	{
		uint32_t j;
		for (j = 0; j < 5; j++)
		{
			s_stPentagon[i].stPoint[j].x = (s_stPentagon[i].stPoint[j].x * LV_VER_RES + (1080 / 2))/ 1080;
			s_stPentagon[i].stPoint[j].y = (s_stPentagon[i].stPoint[j].y * LV_VER_RES + (1080 / 2)) / 1080;
			
			s_stPentagon[i].stPoint[j].x += ((LV_HOR_RES - LV_VER_RES) / 2);
		}
	}

	for (i = 0; i < 3; i++)
	{
		uint32_t j;
		for (j = 0; j < 4; j++)
		{
			s_stTactangle[i].stPoint[j].x = (s_stTactangle[i].stPoint[j].x * LV_VER_RES + (1080 / 2)) / 1080;
			s_stTactangle[i].stPoint[j].y = (s_stTactangle[i].stPoint[j].y * LV_VER_RES + (1080 / 2)) / 1080;

			s_stTactangle[i].stPoint[j].x += ((LV_HOR_RES - LV_VER_RES) / 2);
		}
	}

	for (i = 0; i < 4; i++)
	{
		s_stCycle[i].x = (s_stCycle[i].x * LV_VER_RES + (1080 / 2)) / 1080;
		s_stCycle[i].y = (s_stCycle[i].y * LV_VER_RES + (1080 / 2)) / 1080;
		s_stCycle[i].r = (s_stCycle[i].r * LV_VER_RES + (1080 / 2)) / 1080;

		s_stCycle[i].x += ((LV_HOR_RES - LV_VER_RES) / 2);

	}
}



/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void LOGODraw(void)
{

	uint32_t i;
	GUI_SetColor(GUI_MAKE_ARGB(0, 51, 51, 51));
	GUI_FillRect(0, 0, LV_HOR_RES - 1, LV_VER_RES - 1) ;

	for (i = 0; i < 3; i++)
	{
		GUI_SetColor(GUI_MAKE_ARGB(0, s_stPentagon[i].stColor.red, s_stPentagon[i].stColor.green, s_stPentagon[i].stColor.blue));
		GUI_FillPolygon(s_stPentagon[i].stPoint, 5, 0, 0);
	}

	for (i = 0; i < 3; i++)
	{
		GUI_SetColor(GUI_MAKE_ARGB(0, s_stTactangle[i].stColor.red, s_stTactangle[i].stColor.green, s_stTactangle[i].stColor.blue));
		GUI_FillPolygon(s_stTactangle[i].stPoint, 4, 0, 0);
	}

	for (i = 0; i < 4; i++)
	{
		GUI_SetColor(GUI_MAKE_ARGB(0, s_stCycle[i].stColor.red, s_stCycle[i].stColor.green, s_stCycle[i].stColor.blue));
		GUI_FillCircle(s_stCycle[i].x, s_stCycle[i].y, s_stCycle[i].r);
	}

}
#undef main
int main(int argc, char** argv)
{
	/*Initialize LittlevGL*/
	lv_init();

	/*Initialize the HAL for LittlevGL*/
	hal_init();

	{
		const GUI_POINT stArrow[] = {
			{ 0, -5 },
			{ -40, -35 },
			{ -10, -25 },
			{ -10, -85 },
			{ 10, -85 },
			{ 10, -25 },
			{ 40, -35 }
		};
		GUI_Init();

		//GUI_FillPolygon(stArrow, 7,LV_HOR_RES / 2, LV_VER_RES / 2);

		//GUI_FillCircle(100, 100, 50);
		//GUI_DrawEllipse(100, 100, 50, 80);

		LOGOCoordsInit();
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
#ifdef TEST_GUI
	{
		CreateTableView();
	}
#endif
	if (0)
	{
		lv_theme_t *s_pTheme = lv_theme_alien_init(200, NULL);

		if (s_pTheme == NULL)
		{
			return -1;
		}

		lv_theme_set_current(s_pTheme);

		//lv_test_bar_1();
		//lv_test_cont_1();
		//lv_tutorial_animations();
		lv_test_mbox_1();
	}

	//lv_test_ddlist_1();

	//lv_test_group_1();
	//lv_test_img_1();
	while (1)
	{
		/* Periodically call the lv_task handler.
		 * It could be done in a timer interrupt or an OS task too.*/
		lv_task_handler();
#if (defined TEST_GUI) && 0
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
					StVolume stVolume = { rand(), rand() };
					SetAudioVolume(6, stVolume);
					ReflushActiveTable(0, 6);
					u8Cnt = 0;
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
