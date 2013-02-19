
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
	cairo_matrix_t matrix;
	cairo_get_matrix (cr, & matrix);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
	if (no_indexing) return;
	cairo_set_font_size (cr, 0.18);
	cairo_set_matrix (cr, & matrix);
	for (int x = 0; x < indexing . size . x; x++) {
		for (int y = 0; y < indexing . size . y; y++) {
			cairo_move_to (cr, x - 0.42, y - 0.3);
			char command [24];
			sprintf (command, "%02i%02i", (int) (x + indexing . position . x), (int) (y + indexing . position . y));
			cairo_show_text (cr, command);
		}
	}
}

void grid_token :: draw_vertical_hex_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre, bool initial) {
	double H = 0.5 * 0.866025404;
	double vertical_shift = initial ? 0.0 : - H;
	for (int x = 0; x < indexing . size . x; x++) {
		double xx = -0.25 + (double) x * 0.75;
		double yy = vertical_shift;
		if (vertical_shift < 0.0 && x < indexing . size . x - 1) {cairo_move_to (cr, xx + 0.5, yy); cairo_line_to (cr, xx + 0.75, yy + H);}
		for (int y = 0; y < indexing . size . y; y++) {
			cairo_move_to (cr, xx + 0.5, yy);
			cairo_line_to (cr, xx, yy);
			cairo_line_to (cr, xx - 0.25, yy + H);
			cairo_line_to (cr, xx, yy + H + H);
			if (x == indexing . size . x - 1) {
				cairo_move_to (cr, xx + 0.5, yy);
				cairo_line_to (cr, xx + 0.75, yy + H);
				cairo_line_to (cr, xx + 0.5, yy + H + H);
			}
			yy += H + H;
		}
		cairo_move_to (cr, xx, yy);
		cairo_line_to (cr, xx + 0.5, yy);
		if (vertical_shift == 0.0) cairo_line_to (cr, xx + 0.75, yy - H);
		vertical_shift = vertical_shift == 0.0 ? - H : 0.0;
	}
	cairo_matrix_t matrix;
	cairo_get_matrix (cr, & matrix);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
	if (no_indexing) return;
	cairo_set_font_size (cr, 0.16);
	cairo_set_matrix (cr, & matrix);
	vertical_shift = initial ? H : 0.0;
	for (int x = 0; x < indexing . size . x; x++) {
		for (int y = 0; y < indexing . size . y; y++) {
			cairo_move_to (cr, x * 0.75 - 0.24, y * (H + H) - 0.22 + vertical_shift);
			char command [24];
			sprintf (command, "%02i%02i", (int) (x + indexing . position . x), (int) (y + indexing . position . y));
			cairo_show_text (cr, command);
		}
		vertical_shift = vertical_shift == 0.0 ? H : 0.0;
	}
}

void grid_token :: draw_horizontal_hex_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre, bool initial) {
	double H = 0.5 * 0.866025404;
	double horizontal_shift = initial ? 0.0 : - H;
	for (int y = 0; y < indexing . size . y; y++) {
		double yy = -0.25 + (double) y * 0.75;
		double xx = horizontal_shift;
		if (horizontal_shift < 0.0 && y < indexing . size . y - 1) {cairo_move_to (cr, xx, yy + 0.5); cairo_line_to (cr, xx + H, yy + 0.75);}
		for (int x = 0; x < indexing . size . x; x++) {
			cairo_move_to (cr, xx, yy + 0.5);
			cairo_line_to (cr, xx, yy);
			cairo_line_to (cr, xx + H, yy - 0.25);
			cairo_line_to (cr, xx + H + H, yy);
			if (y == indexing . size . y - 1) {
				cairo_move_to (cr, xx, yy + 0.5);
				cairo_line_to (cr, xx + H, yy + 0.75);
				cairo_line_to (cr, xx + H + H, yy + 0.5);
			}
			xx += H + H;
		}
		cairo_move_to (cr, xx, yy);
		cairo_line_to (cr, xx, yy + 0.5);
		if (horizontal_shift == 0.0) cairo_line_to (cr, xx - H, yy + 0.75);
		horizontal_shift = horizontal_shift == 0.0 ? - H : 0.0;
	}
	cairo_matrix_t matrix;
	cairo_get_matrix (cr, & matrix);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
	if (no_indexing) return;
	cairo_set_font_size (cr, 0.16);
	cairo_set_matrix (cr, & matrix);
	horizontal_shift = initial ? H : 0.0;
	for (int y = 0; y < indexing . size . y; y++) {
		for (int x = 0; x < indexing . size . x; x++) {
			cairo_move_to (cr, x * (H + H) - 0.36 + horizontal_shift, y * 0.75 - 0.10);
			char command [24];
			sprintf (command, "%02i%02i", (int) (x + indexing . position . x), (int) (y + indexing . position . y));
			cairo_show_text (cr, command);
		}
		horizontal_shift = horizontal_shift == 0.0 ? H : 0.0;
	}
}

void grid_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	location . size = point (scaling, scaling);
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * viewport -> scaling);
	point centre = r . centre ();
	cairo_translate (cr, centre . x, centre . y);
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
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
	point factor (1, 1);
	switch (side) {
	case 1: case 2: if (indexing . size . x > 1) factor . x = (0.75 * (indexing . size . x - 1) + 1.0) / indexing . size . x; break;
	case 3: case 4: if (indexing . size . y > 1) factor . y = (0.75 * (indexing . size . y - 1) + 1.0) / indexing . size . y; break;
	default: break;
	}
	double angle = rotation * M_PI / 12.0;
	double cell_size = (abs (cos (angle)) + abs (sin (angle))) * scaling;
	ret . size = point (indexing . size . x * abs (cos (angle)) + indexing . size . y * abs (sin (angle)), indexing . size . y * abs (cos (angle)) + indexing . size . x * abs (sin (angle))) * scaling;
	ret . size *= factor;
	ret . position . x += 0.5 * (location . size . x - cell_size);
	ret . position . y += 0.5 * (location . size . y - cell_size);
	ret . position . x -= scaling * positivise (sin (angle)) * (indexing . size . y - 1) * factor . y;
	ret . position . x -= scaling * positivise (- cos (angle)) * (indexing . size . x - 1) * factor . x;
	ret . position . y -= scaling * positivise (- sin (angle)) * (indexing . size . x - 1) * factor . x;
	ret . position . y -= scaling * positivise (- cos (angle)) * (indexing . size . y - 1) * factor . y;
	return ret;
}







