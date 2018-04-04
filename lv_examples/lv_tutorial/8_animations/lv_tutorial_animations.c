/**
 * @file lv_tutorial_animation.h
 *
 */

/*
 * YOu can add animations to improve the user experience.
 * Basically you can animate any attribute by writing a
 * 'void func(void *ptr, int32_t value)' function.
 * The animation will call a function like this to update the value of 'ptr' to 'value'.
 * Fortunately near all 'set' functions in LittlevGL have a similar prototype.
 * E.g. lv_obj_set_width(lv_obj_t *obj, lv_coord_t w)
 * You will see below how declare and start an animation.
 *
 * There are built in animation which can be started with the lv_obj_anim() function.
 *
 * The other type of animations are the style animations where you can fade
 * one style into an other. See the example below.
 *
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_animations.h"
#if USE_LV_TUTORIALS && USE_LV_ANIMATION

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
lv_style_t btn3_style;
lv_style_t text_style;
lv_style_t text_style_transp;

void *style = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void flush_style(void)
{
	static uint32_t time = 0;
	if (time == 2000)
	{
		if (style != NULL)
		{
			lv_anim_reflush(style, NULL, -500, 0);
		}
		//time = 0;
	}
	time++;
}

void lv_anim_end_cb(void *des)
{
	printf("%s\n", __FUNCTION__);
	lv_mem_free(des);
	style = NULL;
}


/**
 * Crate some objects an animate them
 */
void lv_tutorial_animations(void)
{
    lv_obj_t *label;

#if 0
    /*Create a button the demonstrate built-in animations*/
    lv_obj_t *btn1;
    btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos(btn1, 10 , 10 );     /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn1, 80 , 50 );

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Float");

    /* Float in the button using a built-in function
     * Delay the animation with 2000 ms and float in 300 ms. NULL means no end callback*/
    lv_obj_animate(btn1, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 2000, NULL);

    /*Create a button to demonstrate user defined animations*/
    lv_obj_t *btn2;
    btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos(btn2, 10 , 80 );     /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn2, 80 , 50 );

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Move");

    /*Create an animation to move the button continuously left to right*/
    lv_anim_t a;
    a.var = btn2;
    a.start = lv_obj_get_x(btn2);
    a.end = a.start + (100 );
    a.fp = (lv_anim_fp_t)lv_obj_set_x;
    a.path = lv_anim_path_linear;
    a.end_cb = NULL;
    a.act_time = -1000;                         /*Negative number to set a delay*/
    a.time = 400;                               /*Animate in 400 ms*/
    a.playback = 1;                             /*Make the animation backward too when it's ready*/
    a.playback_pause = 0;                       /*Wait before playback*/
    a.repeat = 1;                               /*Repeat the animation*/
    a.repeat_pause = 500;                       /*Wait before repeat*/
    lv_anim_create(&a);

    /*Create a button to demonstrate the style animations*/
    lv_obj_t *btn3;
    btn3 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos(btn3, 10 , 150 );     /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn3, 80 , 50 );

    label = lv_label_create(btn3, NULL);
    lv_label_set_text(label, "Style");

    /*Create a unique style for the button*/
    lv_style_copy(&btn3_style, lv_btn_get_style(btn3, LV_BTN_STYLE_REL));
    lv_btn_set_style(btn3, LV_BTN_STATE_REL, &btn3_style);

	{
		/*Animate the new style*/
		lv_style_anim_t sa;
		sa.style_anim = &btn3_style;            /*This style will be animated*/
		sa.style_start = &lv_style_btn_rel;     /*Style in the beginning (can be 'style_anim' as well)*/
		sa.style_end = &lv_style_transp;        /*Style at the and (can be 'style_anim' as well)*/
		sa.act_time = -500;                     /*These parameters are the same as with the normal animation*/
		sa.time = 1000;
		sa.playback = 1;
		sa.playback_pause = 500;
		sa.repeat = 1;
		sa.repeat_pause = 500;
		sa.end_cb = NULL;
		lv_style_anim_create(&sa);
	}
#endif


	label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label, "test text");
	lv_obj_set_pos(label, 10, 230);     /*Set a position. It will be the animation's destination*/

	lv_style_copy(&text_style, lv_label_get_style(label));
	lv_style_copy(&text_style_transp, &text_style);
	text_style_transp.text.opa = LV_OPA_TRANSP;
	//text_style_transp.text.color.blue = 0xFF;
	lv_label_set_style(label, &text_style);

	if(1)
	{
		lv_style_anim_t sa;
		sa.style_anim = &text_style;            /*This style will be animated*/
		sa.style_start = &lv_style_scr;     /*Style in the beginning (can be 'style_anim' as well)*/
		sa.style_end = &text_style_transp;        /*Style at the and (can be 'style_anim' as well)*/
		sa.act_time = -500;                     /*These parameters are the same as with the normal animation*/
		sa.time = 2000;
		sa.playback = 0;
		sa.playback_pause = 500;
		sa.repeat = 0;
		sa.repeat_pause = 500;
		sa.end_cb = lv_anim_end_cb;
		style = lv_style_anim_create(&sa, NULL);
	}

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_TUTORIALS && USE_LV_ANIMATION*/

