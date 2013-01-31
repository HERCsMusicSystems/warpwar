
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

void grid_token :: draw_square_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre) {
	for (int ind = 0; ind <= indexing . size . y; ind++) {cairo_move_to (cr, -0.5, -0.5 + ind); cairo_line_to (cr, -0.5 + indexing . size . x, -0.5 + ind);}
	for (int ind = 0; ind <= indexing . size . x; ind++) {cairo_move_to (cr, -0.5 + ind, -0.5); cairo_line_to (cr, -0.5 + ind, -0.5 + indexing . size . y);}
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
}

void grid_token :: draw_vertical_hex_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre, bool initial) {
	cairo_rectangle (cr, -0.5, -0.5, 1, 1);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
}

void grid_token :: draw_horizontal_hex_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre, bool initial) {
	cairo_rectangle (cr, -0.5, -0.5, 1, 1);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
}

void grid_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	location . size = point (scaling, scaling);
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * viewport -> scaling);
	point centre = r . centre ();
	cairo_translate (cr, centre . x, centre . y);
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 6.0);
	cairo_scale (cr, r . size . x, r . size . y);
	switch (side) {
	case 0: draw_square_grid (cr, viewport, r, centre); break;
	case 1: draw_vertical_hex_grid (cr, viewport, r, centre, false); break;
	case 2: draw_vertical_hex_grid (cr, viewport, r, centre, true); break;
	case 3: draw_horizontal_hex_grid (cr, viewport, r, centre, false); break;
	case 4: draw_horizontal_hex_grid (cr, viewport, r, centre, true); break;
	default: draw_square_grid (cr, viewport, r, centre); break;
	}
}

double positivise (double d) {return d >= 0.0 ? d : 0.0;}

rect grid_token :: get_bounding_box (void) {
	rect ret = location;
	double angle = rotation * M_PI / 6.0;
	double cell_size = (abs (cos (angle)) + abs (sin (angle))) * scaling;
	ret . size = point (indexing . size . x * abs (cos (angle)) + indexing . size . y * abs (sin (angle)), indexing . size . y * abs (cos (angle)) + indexing . size . x * abs (sin (angle))) * scaling;
	ret . position . x += 0.5 * (location . size . x - cell_size);
	ret . position . y += 0.5 * (location . size . y - cell_size);
	ret . position . x -= scaling * positivise (sin (angle)) * (indexing . size . y - 1);
	ret . position . x -= scaling * positivise (- cos (angle)) * (indexing . size . x - 1);
	ret . position . y -= scaling * positivise (- sin (angle)) * (indexing . size . x - 1);
	ret . position . y -= scaling * positivise (- cos (angle)) * (indexing . size . y - 1);
	return ret;
}
/*
rect grid_token :: get_bounding_box (void) {
	rect ret = location;
	//if (rotation != 0.0) {
		double angle = rotation * M_PI / 6.0;
		ret . size = point (abs (scaling * cos (angle)) + abs (scaling * sin (angle)), abs (scaling * cos (angle)) + abs (scaling * sin (angle)));
	ret . position . x += location . size . x * 0.5 - ret . size . x * 0.5;
	ret . position . y += location . size . y * 0.5 - ret . size . y * 0.5;
		ret . size = point (abs (indexing . size . x * (scaling * cos (angle)) + abs (scaling * sin (angle))), indexing . size . y * (abs (scaling * cos (angle)) + abs (scaling * sin (angle))));
	//}
	return ret;
}
*/







