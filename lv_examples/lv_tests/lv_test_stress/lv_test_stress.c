/**
 * @file lv_test_stress.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lv_test_stress.h"

#if USE_LV_TESTS && USE_LV_ANIMATION

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void obj_mem_leak_tester(void *);
static void alloc_free_tester(void *);
static void mem_monitor(void *);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * all_obj_h;
static lv_obj_t * alloc_label;
static lv_obj_t * alloc_ta;
#if defined _WIN32
static const lv_color_t needle_colors[1] = { LV_COLOR_MAKE_WIN32(0xFF, 00, 00) };//{LV_COLOR_RED};
#else
static const lv_color_t needle_colors[1] = {LV_COLOR_RED};
#endif
static const char * mbox_btns[] = {"Ok", "Cancel", ""};
LV_IMG_DECLARE(img_flower_icon);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create and delete a lot of objects and animations.
 */
void lv_test_stress_1(void)
{
    lv_task_create(obj_mem_leak_tester, 200, LV_TASK_PRIO_MID, NULL);
    lv_task_create(mem_monitor, 500, LV_TASK_PRIO_MID, NULL);
    lv_task_create(alloc_free_tester, 100, LV_TASK_PRIO_MID, NULL);

    /* Holder for all object types */
    all_obj_h = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(all_obj_h, LV_HOR_RES / 2, LV_VER_RES);
    lv_obj_set_style(all_obj_h, &lv_style_pretty);

    alloc_ta = lv_ta_create(lv_scr_act(), NULL);
    lv_obj_align(alloc_ta, all_obj_h, LV_ALIGN_OUT_RIGHT_TOP, 10 , 10 );
    lv_obj_set_height(alloc_ta, LV_VER_RES / 4);

    alloc_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(alloc_label, alloc_ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0 , 20 );


    /*Add style animation to the ta*/
    static lv_style_t ta_style;
    lv_style_copy(&ta_style, &lv_style_pretty);
    lv_ta_set_style(alloc_ta, LV_TA_STYLE_BG, &ta_style);

    lv_style_anim_t sa;
    sa.style_anim = &ta_style;
    sa.style_start = &lv_style_pretty;
    sa.style_end = &lv_style_pretty_color;
    sa.act_time = 500;
    sa.time = 500;
    sa.playback = 1;
    sa.playback_pause = 500;
    sa.repeat = 1;
    sa.repeat_pause = 500;
    sa.end_cb = NULL;
    lv_style_anim_create(&sa);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void mem_monitor(void * param)
{
#if LV_EX_PRINTF
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, big free: %6d\n", (int)mon.total_size - mon.free_size,
    		                                                     mon.used_pct,
																 mon.frag_pct,
																 (int)mon.free_biggest_size);
#endif
}

static void obj_mem_leak_tester(void * param)
{
    static int16_t state = 0;
    lv_obj_t *obj;
    static lv_obj_t *page;

    lv_anim_t a;
    a.path = lv_anim_path_linear;
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = 500;
    a.playback_pause = 100;
    a.repeat_pause = 100;


    switch(state) {
        case 0:
            obj = lv_obj_create(all_obj_h, NULL);
            lv_obj_set_pos(obj, 10 , 5 );
            a.playback = 1;
            a.repeat = 1;
            a.fp = (lv_anim_fp_t)lv_obj_set_x;
            a.var = obj;
            a.start = 10 ;
            a.end = 100 ;
            lv_anim_create(&a);
            break;
        case 1:
            obj = lv_btn_create(all_obj_h, NULL);
            lv_obj_set_pos(obj, 60 , 5 );
            a.playback = 0;
            a.repeat = 1;
            a.fp = (lv_anim_fp_t)lv_obj_set_x;
            a.var = obj;
            a.start = 150 ;
            a.end = 200 ;
            lv_anim_create(&a);
            obj = lv_label_create(obj, NULL);
            lv_label_set_text(obj, "Button");
            break;
        case 2:     /*Page tests container too*/
            page = lv_page_create(all_obj_h, NULL);
            lv_obj_set_pos(page, 10 , 60 );
            lv_obj_set_size(page, lv_obj_get_width(all_obj_h) - (20 ), 3 * LV_VER_RES / 4);
            lv_page_set_scrl_layout(page, LV_LAYOUT_PRETTY);
            break;
        case 3:
            obj = lv_label_create(page, NULL);
            lv_label_set_text(obj, "Label");
            break;
        case 4:
            obj = lv_img_create(page, NULL);
            lv_img_set_src(obj, &img_flower_icon);
            break;
        case 5:
            obj = lv_cb_create(page, NULL);
            lv_cb_set_text(obj, "Check box");
            break;
        case 7:             /*Switch tests bar and slider memory leak too*/
            obj = lv_sw_create(page, NULL);
            lv_sw_on(obj);
            break;
        case 8:     /*Kb tests butm too*/
            obj = lv_kb_create(all_obj_h, NULL);
            lv_obj_set_size(obj, LV_HOR_RES / 3, LV_VER_RES / 5);
            lv_obj_set_pos(obj, 30, 90);
            lv_obj_animate(obj, LV_ANIM_FLOAT_BOTTOM | LV_ANIM_IN, 200, 0, NULL);
            break;
        case 9: /*Roller test ddlist too*/
            obj = lv_roller_create(page, NULL);
            lv_roller_set_options(obj, "One\nTwo\nThree");
            lv_roller_set_anim_time(obj, 300);
            lv_roller_set_selected(obj, 2, true);
            break;
        case 10: /*Gauge test lmeter too*/
            obj = lv_gauge_create(page, NULL);
            lv_gauge_set_needle_count(obj, 1, needle_colors);
            lv_gauge_set_value(obj, 1, 30);
            break;
        case 15: /*Wait a little to see the previous results*/
            obj = lv_list_create(all_obj_h, NULL);
            lv_obj_set_pos(obj, 40 , 50 );
            lv_list_add(obj, SYMBOL_OK, "List 1", NULL);
            lv_list_add(obj, SYMBOL_OK, "List 2", NULL);
            lv_list_add(obj, SYMBOL_OK, "List 3", NULL);
            lv_list_add(obj, SYMBOL_OK, "List 4", NULL);
            lv_list_add(obj, SYMBOL_OK, "List 5", NULL);
            lv_list_add(obj, SYMBOL_OK, "List 6", NULL);
            lv_obj_animate(obj, LV_ANIM_GROW_V | LV_ANIM_IN, 5000, 0, NULL);
            break;
        case 16:
            obj = lv_win_create(all_obj_h, NULL);
            lv_win_add_btn(obj, SYMBOL_CLOSE, NULL);
            lv_win_add_btn(obj, SYMBOL_OK, NULL);
            lv_win_set_style(obj, LV_WIN_STYLE_BG, &lv_style_pretty);
            lv_obj_set_size(obj, LV_HOR_RES / 3, LV_VER_RES / 3);
            lv_obj_set_pos(obj, 20 , 100 );
            break;
        case 17:
            obj = lv_tabview_create(all_obj_h, NULL);
            lv_tabview_add_tab(obj, "tab1");
            lv_tabview_add_tab(obj, "tab2");
            lv_tabview_add_tab(obj, "tab3");
            lv_tabview_set_style(obj, LV_TABVIEW_STYLE_BG, &lv_style_pretty);
            lv_obj_set_size(obj, LV_HOR_RES / 3, LV_VER_RES / 3);
            lv_obj_set_pos(obj, 50 , 140 );
            break;
        case 18:
            obj = lv_mbox_create(all_obj_h, NULL);
            lv_obj_set_width(obj, LV_HOR_RES / 4);
            lv_mbox_set_text(obj, "message");
            lv_mbox_add_btns(obj, mbox_btns, NULL); /*Set 3 times to test btnm add memory leasks*/
            lv_mbox_add_btns(obj, mbox_btns, NULL);
            lv_mbox_add_btns(obj, mbox_btns, NULL);
            lv_mbox_set_anim_time(obj, 300);
            lv_mbox_start_auto_close(obj, 500);
            break;

        /*Delete object from the page*/
        case 20:
            obj = lv_obj_get_child(lv_page_get_scrl(page), NULL);
            if(obj) lv_obj_del(obj);
            else state = 24;
            break;
        case 21:
            obj = lv_obj_get_child_back(lv_page_get_scrl(page), NULL);       /*Delete from the end too to be more random*/
            if(obj){
                lv_obj_del(obj);
                state -= 2;     /*Go back to delete state*/
            }
            else state = 24;
            break;
        /*Remove object from 'all_obj_h'*/
        case 25:
            obj = lv_obj_get_child(all_obj_h, NULL);
            if(obj) lv_obj_del(obj);
            else state = 29;
            break;
        case 26:
            obj = lv_obj_get_child_back(all_obj_h, NULL);       /*Delete from the end too to be more random*/
            if(obj){
                lv_obj_del(obj);
                state -= 2;     /*Go back to delete state*/
            }
            else state = 29;
            break;

        case 30:
            state = -1;
            break;
        default:
            break;
    }

    state ++;
}


/**
 * Test alloc and free by settings the text of a label and instering text to a text area
 */
static void alloc_free_tester(void * param)
{
    static int16_t state = 0;

    switch(state) {
         case 0:
             lv_label_set_text(alloc_label, "a");
             break;

         case 1:
             lv_ta_set_text(alloc_ta, "Initilal");
             break;

         case 2:
             lv_label_set_text(alloc_label, "long long long\nlong long long");
             break;

         case 3:
             lv_label_set_text(alloc_label, "b");
             break;

         case 6:    /*Wait to be random*/
             lv_ta_set_cursor_pos(alloc_ta, 0);
             lv_ta_add_text(alloc_ta, "aaaaaaaaaaaaaaaaaaaaaaaaaaa!\n");
             break;

         case 7:
             lv_label_set_text(alloc_label, "medium");
             break;

         case 8:
             lv_label_set_text(alloc_label, "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n"
                                             "very very long very very long\n");
             break;

         case 10:       /*Wait to be random*/
             lv_label_set_text(alloc_label, "some text");
             break;

         case 11:
             lv_ta_set_cursor_pos(alloc_ta, 20);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             lv_ta_del_char(alloc_ta);
             break;

         case 12:
             lv_label_set_text(alloc_label, SYMBOL_DIRECTORY);
             break;

         case 16:     /*Wait to be random*/
             lv_label_set_text(alloc_label, "A dummy sentence nothing else");
             break;

         case 17:
             lv_ta_set_cursor_pos(alloc_ta, 5);
             lv_ta_add_char(alloc_ta, 'A');
             lv_ta_add_char(alloc_ta, 'A');
             lv_ta_add_char(alloc_ta, 'A');
             lv_ta_add_char(alloc_ta, 'A');
             lv_ta_add_char(alloc_ta, 'A');
             break;

         case 18:
             lv_label_set_text(alloc_label, "ok");
             break;

         case 20:   /*Wait to be random*/
             lv_label_set_text(alloc_label, SYMBOL_FILE);
             break;
         case 21:
             lv_label_set_text(alloc_label, "c");
             break;
#if LV_TXT_UTF8
         case 22:
             lv_ta_set_cursor_pos(alloc_ta, 20);
             lv_ta_add_text(alloc_ta, "Ű");
             lv_ta_add_text(alloc_ta, "Ű");
             lv_ta_add_text(alloc_ta, "Ű");
             lv_ta_add_text(alloc_ta, "Ű");
             lv_ta_add_text(alloc_ta, "Ű");
             break;

         case 23:
             lv_label_set_text(alloc_label, "ÁÁÁÁÁÉÉÉÉÉÉÉŐŐŐŐŐŐŐŐŐ");
             break;
#endif
         case 25:
             lv_ta_set_text(alloc_ta, "");
             break;

         case 26:
             lv_label_set_text(alloc_label, "");
             break;

         case 28:
             state =-1 ;
             break;
         default:
             break;
    }

    state ++;
}

#endif /*USE_LV_TESTS && USE_LV_ANIMATION*/

