#ifndef GFXCTX_H
#define GFXCTX_H

struct gfxctx;

/*
 * gfxctx_open: open graphics context.
 */
struct gfxctx*
gfxctx_open(
	int *,           /* argc */
	char **          /* argv */
);

struct gfxwin;

int
gfxwin_textwidth(struct gfxwin *win, char *buf);

void
gfxwin_process_events(
	struct gfxctx *
);

void
gfxwin_set_draw_callback(
	struct gfxwin *,
	void (*)(struct gfxwin *)
);

void*
gfxwin_data(
	struct gfxwin *
);

void
gfxctx_flush(
	struct gfxctx *
);

/*
 * gfxwin_create: create window.
 */
struct gfxwin*
gfxwin_create(
	struct gfxctx *,
	int,             /* x */
	int,             /* y */
	unsigned int,    /* width */
	unsigned int,    /* height */
	const char *,    /* background color spec */
	void *           /* optional user data */
);

/*
 * What we need:
 *
 * window = create_window
 * draw
 *   draw_graph
 *     draw_graph_line(window, x, fraction);
 *   draw_labels
 *     draw_ylabels
 *     draw_xlabels
 *       draw_label_text(window, x, y, text, &width, &height)
 */

/*
 * gfxwin_draw_line: draw line.
 */
void
gfxwin_draw_line(
	struct gfxwin *,
	int,             /* x1 */
	int,             /* y1 */
	int,             /* x2 */
	int              /* y2 */
);

void
gfxwin_clear(
	struct gfxwin *,
	int,             /* x */
	int,             /* y */
	unsigned int,    /* width */
	unsigned int     /* height */
);

unsigned int
gfxwin_height(
	struct gfxwin *
);

unsigned int
gfxwin_width(
	struct gfxwin *
);

int
gfxctx_fd(
	struct gfxctx *
);

#endif
