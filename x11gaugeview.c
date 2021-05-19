/*
 * Visualize gauge, X11/Xlib version.
 */

#include "gaugeview.h"
#include "gauge.h"
#include "gfxctx.h"
#include "x11.h"
#include "util.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>

struct gaugeview
{
	struct gfxctx *ctx;
	struct gauge *gauge;
	struct gfxwin *win;
	double values_fifo[10];
	int values_first;
	int values_last;
	int nvalues;
};

static void
gaugeview_draw(struct gfxwin *win);

struct gaugeview*
gaugeview_open(struct gauge *gauge, struct gfxctx *ctx)
{
	struct gaugeview *view;

	if ((view = malloc(sizeof(struct gaugeview))) == NULL)
		err(1, "allocate gaugeview");

	view->gauge = gauge;
	view->ctx = ctx;

	view->win = gfxwin_create(ctx, 0, 0, 640, 480, "white", gauge);
	gfxwin_set_draw_callback(view->win, gaugeview_draw);

	view->nvalues = 0;
	view->values_first = 0;
	view->values_last = 0;
	return view;
}

void
gaugeview_clear(struct gaugeview *view)
{
	gfxwin_clear(view->win, 0, 0, gfxwin_width(view->win),
	    gfxwin_height(view->win));
}

void 
gaugeview_draw_value(struct gaugeview *view, double pos, double val)
{	
	int height, width;
	Display *dpy;
	GC gc;
	Window dstwin;
	struct gfxwin *win;
	double cx, cy, x, y;
	double angle_deg;
	static double old_val;

	win = view->win;
	dpy = win->ctx->dpy;
	gc = win->fg;
	dstwin = win->win;
	width = gfxwin_width(win);
	height = gfxwin_height(win);

#if 0
	XClearWindow(dpy, dstwin);
#endif

	cx = width/2.0;
	cy = height/2.0;
	angle_deg = 270.0 - (180.0 * val);
	x = cx + (sin(angle_deg / 180.0 * M_PI) * (width / 2));
	y = cy + (cos(angle_deg / 180.0 * M_PI) * (height / 2));

	/*
	 * Draw gauge.
	 */
	XFillArc(dpy, dstwin, win->hl, 5, 5, width-10, height-10, (0-45) * 64, (180+90) * 64);

	XDrawArc(dpy, dstwin, win->hl, 5, 5, width-10, height-10, 0 * 64, 360 * 64);
	XDrawArc(dpy, dstwin, gc, 5, 5, width-10, height-10, 225 * 64, (-270 * 64) * val);

	{
		XGCValues v;

		v.line_width = width/20.0;
		XChangeGC(dpy, gc, GCLineWidth, &v);

		cy -= ((width/20.0) / 2);


		angle_deg = (270.0+45.0) - ((180.0+90.0) * old_val);
		x = cx + (sin(angle_deg / 180.0 * M_PI) * ((width / 2.0) - width/20.0));
		y = cy + (cos(angle_deg / 180.0 * M_PI) * ((height / 2.0) - height/20.0));
		XDrawLine(dpy, dstwin, win->hl, cx, cy, x, y);

		angle_deg = (270.0+45.0) - ((180.0+90.0) * val);
		x = cx + (sin(angle_deg / 180.0 * M_PI) * ((width / 2.0) - width/20.0));
		y = cy + (cos(angle_deg / 180.0 * M_PI) * ((height / 2.0) - height/20.0));
		XDrawLine(dpy, dstwin, gc, cx, cy, x, y);

		v.line_width = 5;
		XChangeGC(dpy, gc, GCLineWidth, &v);
	}

	XFillArc(dpy, dstwin, win->hl, 5, 5, width-10, height-10, (270-45) * 64, (45+45) * 64);

	XFillArc(dpy, dstwin, gc, cx - (width/5.0/2), cy - (height/5.0/2) + (width/20.0/2), width/5.0, height/5.0, 0 * 64, 360 * 64);

	{
		char buf[80];
		snprintf(buf, sizeof(buf), "%.1f", val);
		XDrawString(dpy, dstwin, win->fg, 30, height-30, buf, strlen(buf));
	}

	XFlush(dpy);

	old_val = val;
}

static void
gaugeview_draw(struct gfxwin *win)
{
	struct gauge *gauge = gfxwin_data(win);

	gauge_refresh_view(gauge);
}
