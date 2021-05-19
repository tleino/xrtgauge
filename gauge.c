/*
 * Graph business logic and data.
 */

#include "gauge.h"
#include "gaugeview.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdint.h>

struct value
{
	time_t time;
	double val;
};

struct gauge
{
	struct value value[4096];
	double maxval;	
	struct gaugeview *view;
	time_t bound;
	size_t nvalue;
	size_t index;
	double zoom_level;
};

static double mkpos(time_t, double);
static void draw_value(struct gauge *, struct value *, double pos);

#define DAY_SECS		(24 * 60 * 60)
#define DEFAULT_ZOOM_LEVEL	0.01

struct gauge*
gauge_create(struct gfxctx *ctx, double maxval)
{
	struct gauge *gauge;

	if ((gauge = calloc(1, sizeof(struct gauge))) == NULL)
		err(1, "allocate gauge");

	gauge->maxval = maxval;
	gauge->nvalue = 0;
	gauge->index = 0;
	gauge->zoom_level = DEFAULT_ZOOM_LEVEL;
	gauge->view = gaugeview_open(gauge, ctx);

	gauge_zoom(gauge, 0);

	return gauge;
}

static double
mkpos(time_t t, double bound)
{
	struct tm *tm;

	tm = localtime(&t);
	t = tm->tm_sec + (tm->tm_min * 60) + (tm->tm_hour * 60 * 60);
	t %= (time_t) bound;

	return t / bound;
}

static void
draw_value(struct gauge *gauge, struct value *v, double pos)
{
	double val;

	val = v->val / gauge->maxval;
	gaugeview_draw_value(gauge->view, pos, val);
}

void
gauge_add_data(struct gauge *gauge, time_t t, double val)
{
	struct value *v;
	size_t i;
	double pos, oldpos;
	double old_maxval, old_val;

	/*
	 * If we have same pos value, let's ignore the entry.
	 */
#if 0
	pos = mkpos(t, gauge->bound);
	if (gauge->index > 0) {
		oldv = &gauge->value[gauge->index - 1];
		oldpos = mkpos(oldv->time, gauge->bound);
		if (oldpos == pos) {
			warnx("ignored, same pos");
			return;
		}
	}
#else
	oldpos = 0.0;
	pos = 0.0;
#endif

	/*
	 * Wraparound if we're at array limit or if time-based position
	 * turns over.
	 */
	if (gauge->index == ARRLEN(gauge->value) || oldpos > pos)
		gauge->index = 0;

	v = &gauge->value[gauge->index];
	gauge->index++;
	if (gauge->index > gauge->nvalue)
		gauge->nvalue = gauge->index;

	old_val = v->val;
	v->time = t;
	v->val = val;

	if (old_val == gauge->maxval) {
		/*
		 * If we're overwriting old maxvalue, find if we've
		 * got other entry at same value or next highest.
		 */
		old_maxval = gauge->maxval;
		for (i = 0; i < gauge->nvalue; i++)
			if (gauge->value[i].val >= gauge->maxval)
				gauge->maxval = gauge->value[i].val;

		if (old_maxval != gauge->maxval)
			gauge_refresh_view(gauge);
	} else if (v->val > gauge->maxval) {
		gauge->maxval = v->val;
		gauge_refresh_view(gauge);
	}
	draw_value(gauge, v, pos);
}

void
gauge_refresh_view(struct gauge *gauge)
{
	struct value *v;
	size_t i;
	double old_pos = 0.0, pos;

	/*
	 * This is required in case of floating point errors where
	 * x axis does not align up with previous content.
	 */
	gaugeview_clear(gauge->view);

	for (i = 0; i < gauge->nvalue; i++) {
		v = &gauge->value[i];
		pos = mkpos(v->time, gauge->bound);
		if (pos < old_pos) {	/* Wraparound. */
			break;
		}
		draw_value(gauge, v, pos);
		old_pos = pos;
	}
}

/*
 * direction == 0 recalculates bound.
 */
void
gauge_zoom(struct gauge *gauge, int direction)
{	
	if (direction < 0)
		gauge->zoom_level /= 4;
	else if (direction > 0)
		gauge->zoom_level *= 4;
	if (gauge->zoom_level <= 0.0)
		gauge->zoom_level = DEFAULT_ZOOM_LEVEL;

	gauge->bound = (gauge->zoom_level / 24.0) * (double) DAY_SECS;
	gauge_refresh_view(gauge);
}
