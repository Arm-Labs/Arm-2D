/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*============================ INCLUDES ======================================*/
#include "./scene_player_demo.h"
#include "platform.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapter_0.h"
#include "arm_extra_controls.h"

#include <stdlib.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunused-function"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileCMSISLogo;
extern const arm_2d_tile_t c_tileCMSISLogoMask;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static void __on_scene_depose(arm_2d_scene_t *ptScene)
{
    free(ptScene);
}

/*----------------------------------------------------------------------------*
 * Scene 0                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene0_frame_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
    /* switch to next scene after 3s */
    if (arm_2d_helper_is_time_out(3000)) {
        arm_2d_user_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene0_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    
    arm_2d_align_centre(ptTile->tRegion, c_tileCMSISLogo.tRegion.tSize) {
        arm_2d_tile_copy_with_src_mask( &c_tileCMSISLogo,
                                        &c_tileCMSISLogoMask,
                                        ptTile,
                                        &__centre_region,
                                        ARM_2D_CP_MODE_COPY);
    }

    arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    arm_lcd_puts("Scene 0");

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static void __app_scene0_init(void)
{

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* a region for the busy wheel */
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),
        
        /* top left corner for text display */
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = 0,
                .iY = 0,
            },
            .tSize = {
                .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                .iHeight = 8,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()
    
    s_tDirtyRegions[0].tRegion.tLocation = (arm_2d_location_t){
        .iX = ((__DISP0_CFG_SCEEN_WIDTH__ - c_tileCMSISLogo.tRegion.tSize.iWidth) >> 1),
        .iY = ((__DISP0_CFG_SCEEN_HEIGHT__ - c_tileCMSISLogo.tRegion.tSize.iHeight) >> 1),
    };
    s_tDirtyRegions[0].tRegion.tSize = c_tileCMSISLogo.tRegion.tSize;
    
    
    arm_2d_scene_t *ptScene = (arm_2d_scene_t *)malloc(sizeof(arm_2d_scene_t));
    assert(NULL != ptScene);
    
    *ptScene = (arm_2d_scene_t){
        .fnBackground   = &__pfb_draw_scene0_background_handler,
        .fnScene        = &__pfb_draw_scene0_handler,
        .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        .fnOnBGStart    = NULL,
        .fnOnBGComplete = NULL,
        .fnOnFrameStart = NULL,
        .fnOnFrameCPL   = &__on_scene0_frame_complete,
        .fnDepose       = &__on_scene_depose,
    };
    arm_2d_user_scene_player_append_scenes( &DISP0_ADAPTER, ptScene, 1);
}
 
/*----------------------------------------------------------------------------*
 * Scene 1                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene1_frame_complete(arm_2d_scene_t *ptScene)
{
    ARM_2D_UNUSED(ptScene);
    
//    /* switch to next scene after 3s */
//    if (arm_2d_helper_is_time_out(3000)) {
//        arm_2d_user_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
//    }
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene1_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);

    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}


static
IMPL_PFB_ON_DRAW(__pfb_draw_scene1_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    
    arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    
    progress_bar_drill_show(ptTile, 0, bIsNewFrame);
    
    arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
    arm_lcd_text_location(0,0);
    arm_lcd_puts("Scene 1");
    
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static void __app_scene1_init(void)
{
#define PROGRESSBAR_WIDTH       (__DISP0_CFG_SCEEN_WIDTH__ * 3 >> 3)

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static const)

        /* a region for the busy wheel */
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = ((__DISP0_CFG_SCEEN_WIDTH__ - PROGRESSBAR_WIDTH) >> 1),
                .iY = ((__DISP0_CFG_SCEEN_HEIGHT__ - 32) >> 1),
            },
            .tSize = {
                .iWidth = PROGRESSBAR_WIDTH,
                .iHeight = 32,
            },
        ),
        
        /* top left corner for text display */
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = 0,
                .iY = 0,
            },
            .tSize = {
                .iWidth = __DISP0_CFG_SCEEN_WIDTH__,
                .iHeight = 8,
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()
    
    arm_2d_scene_t *ptScene = (arm_2d_scene_t *)malloc(sizeof(arm_2d_scene_t));
    assert(NULL != ptScene);
    
    *ptScene = (arm_2d_scene_t){
        .fnBackground   = &__pfb_draw_scene1_background_handler,
        .fnScene        = &__pfb_draw_scene1_handler,
        .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
        .fnOnBGStart    = NULL,
        .fnOnBGComplete = NULL,
        .fnOnFrameStart = NULL,
        .fnOnFrameCPL   = &__on_scene1_frame_complete,
        .fnDepose       = &__on_scene_depose,
    };
    arm_2d_user_scene_player_append_scenes( &DISP0_ADAPTER, ptScene, 1);
}



void scene_player_demo_init(void)
{
    __app_scene0_init();
    __app_scene1_init();
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

