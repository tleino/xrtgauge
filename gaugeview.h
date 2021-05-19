#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

struct gfxctx;
struct gaugeview;
struct gauge;

void gaugeview_draw_value(struct gaugeview *, double, double);
struct gaugeview* gaugeview_open(struct gauge *, struct gfxctx *);
void gaugeview_clear(struct gaugeview *);

#endif
