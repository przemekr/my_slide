LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/agg-2.5/include 
LOCAL_CPPFLAGS += -DMOBILE -DDEBUG
LOCAL_CXXFLAGS += -fexceptions

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	agg-2.5/src/agg_arc.cpp \
	agg-2.5/src/agg_arrowhead.cpp \
	agg-2.5/src/agg_bezier_arc.cpp \
	agg-2.5/src/agg_bspline.cpp \
	agg-2.5/src/agg_curves.cpp \
	agg-2.5/src/agg_embedded_raster_fonts.cpp \
	agg-2.5/src/agg_gsv_text.cpp \
	agg-2.5/src/agg_image_filters.cpp \
	agg-2.5/src/agg_line_aa_basics.cpp \
	agg-2.5/src/agg_line_profile_aa.cpp \
	agg-2.5/src/agg_rounded_rect.cpp \
	agg-2.5/src/agg_sqrt_tables.cpp \
	agg-2.5/src/agg_trans_affine.cpp \
	agg-2.5/src/agg_trans_double_path.cpp \
	agg-2.5/src/agg_trans_single_path.cpp \
	agg-2.5/src/agg_trans_warp_magnifier.cpp \
	agg-2.5/src/agg_vcgen_bspline.cpp \
	agg-2.5/src/agg_vcgen_contour.cpp \
	agg-2.5/src/agg_vcgen_dash.cpp \
	agg-2.5/src/agg_vcgen_markers_term.cpp \
	agg-2.5/src/agg_vcgen_smooth_poly1.cpp \
	agg-2.5/src/agg_vcgen_stroke.cpp \
	agg-2.5/src/agg_vpgen_clip_polygon.cpp \
	agg-2.5/src/agg_vpgen_clip_polyline.cpp \
	agg-2.5/src/agg_vpgen_segmentator.cpp \
	agg-2.5/src/ctrl/agg_bezier_ctrl.cpp \
	agg-2.5/src/ctrl/agg_cbox_ctrl.cpp \
	agg-2.5/src/ctrl/agg_gamma_ctrl.cpp \
	agg-2.5/src/ctrl/agg_gamma_spline.cpp \
	agg-2.5/src/ctrl/agg_polygon_ctrl.cpp \
	agg-2.5/src/ctrl/agg_rbox_ctrl.cpp \
	agg-2.5/src/ctrl/agg_scale_ctrl.cpp \
	agg-2.5/src/ctrl/agg_slider_ctrl.cpp \
	agg-2.5/src/ctrl/agg_spline_ctrl.cpp \
	agg-2.5/src/platform/sdl2/agg_platform_support.cpp \
	agg_app.cc agg_button_ctrl.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)
