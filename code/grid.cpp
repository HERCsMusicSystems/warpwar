
#include "boarder.h"

extern double abs (double value);

grid_token :: grid_token (PrologAtom * atom) : boarder_token (atom) {
	scaling = default_scaling ();
}

grid_token :: ~ grid_token (void) {
	printf ("	DELETE GRID\n");
}

void grid_token :: creation_call (FILE * tc) {fprintf (tc, "[%s %s]\n", CREATE_GRID, atom -> name ());}
bool grid_token :: should_save_size (void) {return false;}
double grid_token :: default_scaling (void) {return 64.0;}

void grid_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	location . size = point (scaling, scaling);
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * viewport -> scaling);
	point centre = r . centre ();
	cairo_translate (cr, centre . x, centre . y);
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 6.0);
	cairo_scale (cr, r . size . x, r . size . y);
	cairo_rectangle (cr, -0.5, -0.5, 1, 1);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
}

rect grid_token :: get_bounding_box (void) {
	if (rotation == 0.0) return location;
	rect ret = location;
	double angle = rotation * M_PI / 6.0;
	ret . size = point (abs (scaling * cos (angle)) + abs (scaling * sin (angle)), abs (scaling * cos (angle)) + abs (scaling * sin (angle)));
	ret . position . x += location . size . x * 0.5 - ret . size . x * 0.5;
	ret . position . y += location . size . y * 0.5 - ret . size . y * 0.5;
	return ret;
}








