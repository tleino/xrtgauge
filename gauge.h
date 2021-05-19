#ifndef GRAPH_H
#define GRAPH_H

#include <time.h>

struct gfxctx;
struct gauge;

struct gauge* gauge_create(struct gfxctx *, double maxval);
void gauge_add_data(struct gauge *, time_t, double);
void gauge_refresh_view(struct gauge *);
void gauge_zoom(struct gauge *, int);

#endif
