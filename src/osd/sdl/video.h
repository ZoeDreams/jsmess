//============================================================
//
//  video.h - SDL implementation of MAME video routines
//
//  Copyright (c) 1996-2006, Nicola Salmoria and the MAME Team.
//  Visit http://mamedev.org for licensing and usage restrictions.
//
//  SDLMAME by Olivier Galibert and R. Belmont
//
//============================================================

#ifndef __SDL_VIDEO__
#define __SDL_VIDEO__

#include "osdcomm.h"
#include "osd_cpu.h"
#include "mamecore.h"

//============================================================
//  CONSTANTS
//============================================================

#define MAX_VIDEO_WINDOWS   	(4)

#define VIDEO_MODE_SOFT		0
#define VIDEO_MODE_OPENGL	1

#define SDL_MULTIMON	(0)

#define VIDEO_YUV_MODE_NONE	0
#define VIDEO_YUV_MODE_YV12	1
#define VIDEO_YUV_MODE_YV12X2	2
#define VIDEO_YUV_MODE_YUY2	3
#define VIDEO_YUV_MODE_YUY2X2	4
#define VIDEO_YUV_MODE_MAX	VIDEO_YUV_MODE_YUY2X2
#define VIDEO_YUV_MODE_MIN	VIDEO_YUV_MODE_NONE


// texture formats
// This used to be an enum, but these are now defines so we can use them as
// preprocessor conditionals
#define SDL_TEXFORMAT_ARGB32			 0	// non-16-bit textures or specials
#define SDL_TEXFORMAT_RGB32			 1
#define SDL_TEXFORMAT_RGB32_PALETTED		 2
#define SDL_TEXFORMAT_YUY16			 3
#define SDL_TEXFORMAT_YUY16_PALETTED		 4
#define SDL_TEXFORMAT_PALETTE16			 5
#define SDL_TEXFORMAT_RGB15			 6
#define SDL_TEXFORMAT_RGB15_PALETTED		 7
#define SDL_TEXFORMAT_PALETTE16			 5
#define SDL_TEXFORMAT_RGB15			 6
#define SDL_TEXFORMAT_RGB15_PALETTED		 7
#define SDL_TEXFORMAT_PALETTE16A		 8
// special texture formats for 16bpp texture destination support, do not use
// to address the tex properties / tex functions arrays!
#define SDL_TEXFORMAT_PALETTE16_ARGB1555	 16
#define SDL_TEXFORMAT_RGB15_ARGB1555		 17
#define SDL_TEXFORMAT_RGB15_PALETTED_ARGB1555	 18


//============================================================
//  TYPE DEFINITIONS
//============================================================

typedef struct _sdl_monitor_info sdl_monitor_info;
struct _sdl_monitor_info
{
	sdl_monitor_info  *	next;					// pointer to next monitor in list
	UINT32			handle;					// handle to the monitor
	int			monitor_width;
	int			monitor_height;
	char			monitor_device[64];
	float			aspect;					// computed/configured aspect ratio of the physical device
	int			reqwidth;				// requested width for this monitor
	int			reqheight;				// requested height for this monitor
	int			center_width;				// width of first physical screen for centering
	int			center_height;				// height of first physical screen for centering
};


typedef struct _sdl_window_config sdl_window_config;
struct _sdl_window_config
{
	float				aspect;						// decoded aspect ratio
	int					width;						// decoded width
	int					height;						// decoded height
	int					depth;						// decoded depth
	int					refresh;					// decoded refresh

	int					totalColors;		 // total colors from machine
};


typedef struct _sdl_video_config sdl_video_config;
struct _sdl_video_config
{
	// performance options
	int					novideo;				// don't draw, for pure CPU benchmarking

	// global configuration
	int					windowed;	 			// start windowed?
	int					prescale;				// prescale factor (not currently supported)
	int					keepaspect;	 			// keep aspect ratio?
	int					numscreens;	 			// number of screens
	int					layerconfig;				// default configuration of layers
	int					centerh;
	int					centerv;


	// per-window configuration
	sdl_window_config	window[MAX_VIDEO_WINDOWS];		// configuration data per-window

	// hardware options
	int					mode;			// output mode
	int					waitvsync;		// spin until vsync
	int					switchres;		// switch resolutions

	int					fullstretch;

	// OpenGL options
	int					filter;			// enable filtering, disabled if glsl_filter>0
	int					prescale_effect;
	int					prefer16bpp_tex;
	int					glsl;
	int					glsl_filter;		// glsl filtering, >0 disables filter
	char *                                  glsl_shader_custom;     // custom glsl shader set
	int					glsl_vid_attributes;	// glsl brightness, contrast and gamma for RGB bitmaps
	int					pbo;
	int					vbo;
	int					allowtexturerect;	// allow GL_ARB_texture_rectangle, default: no
	int					forcepow2texture;	// force power of two textures, default: no

	int					perftest;		// print out real video fps

	// vector options
	int					isvector;		// 1 if vector, 0 if raster
	float				beamwidth;		// beam width

	// X11 options
	int					restrictonemonitor;	// in fullscreen, confine to Xinerama monitor 0

	// YUV options	
	int					yuv_mode;
};



//============================================================
//  GLOBAL VARIABLES
//============================================================


extern sdl_video_config video_config;


//============================================================
//  PROTOTYPES
//============================================================

int sdlvideo_init(running_machine *machine);

void sdlvideo_monitor_refresh(sdl_monitor_info *monitor);
float sdlvideo_monitor_get_aspect(sdl_monitor_info *monitor);
sdl_monitor_info *sdlvideo_monitor_from_handle(UINT32 monitor);

#endif
