
#include "boarder.h"

extern double abs (double value);

grid_token :: grid_token (PrologAtom * atom) : boarder_token (atom) {}

grid_token :: ~ grid_token (void) {
	printf ("	DELETING GRID [%s]\n", atom -> name ());
}

void grid_token :: creation_call (boarder * board, FILE * tc) {fprintf (tc, "[%s %s]\n", CREATE_GRID, atom -> name ());}

void grid_token :: draw_square_grid (cairo_t * cr, boarder_viewport * viewport) {
	for (int ind = 0; ind <= indexing . size . y; ind++) {cairo_move_to (cr, 0.0, ind); cairo_line_to (cr, indexing . size . x, ind);}
	for (int ind = 0; ind <= indexing . size . x; ind++) {cairo_move_to (cr, ind, 0.0); cairo_line_to (cr, ind, indexing . size . y);}
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
			cairo_move_to (cr, x +0.08, y + 0.2);
			char command [24];
			sprintf (command, "%02i%02i", (int) (x + indexing . position . x), (int) (y + indexing . position . y));
			cairo_show_text (cr, command);
		}
	}
}

void grid_token :: draw_vertical_hex_grid (cairo_t * cr, boarder_viewport * viewport, bool initial) {
	double H = 0.5 * 0.866025404;
	double vertical_shift = initial ? 0.5 : 0.5 - H;
	for (int x = 0; x < indexing . size . x; x++) {
		double xx = 0.25 + (double) x * 0.75;
		double yy = vertical_shift;
		if (vertical_shift < 0.5 && x < indexing . size . x - 1) {cairo_move_to (cr, xx + 0.5, yy); cairo_line_to (cr, xx + 0.75, yy + H);}
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
		if (vertical_shift == 0.5) cairo_line_to (cr, xx + 0.75, yy - H);
		vertical_shift = vertical_shift == 0.5 ? 0.5 - H : 0.5;
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
			cairo_move_to (cr, x * 0.75 + 0.26, y * (H + H) + 0.28 + vertical_shift);
			char command [24];
			sprintf (command, "%02i%02i", (int) (x + indexing . position . x), (int) (y + indexing . position . y));
			cairo_show_text (cr, command);
		}
		vertical_shift = vertical_shift == 0.0 ? H : 0.0;
	}
}

void grid_token :: draw_horizontal_hex_grid (cairo_t * cr, boarder_viewport * viewport, bool initial) {
	double H = 0.5 * 0.866025404;
	double horizontal_shift = initial ? 0.5 : 0.5 - H;
	for (int y = 0; y < indexing . size . y; y++) {
		double yy = 0.25 + (double) y * 0.75;
		double xx = horizontal_shift;
		if (horizontal_shift < 0.5 && y < indexing . size . y - 1) {cairo_move_to (cr, xx, yy + 0.5); cairo_line_to (cr, xx + H, yy + 0.75);}
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
		if (horizontal_shift == 0.5) cairo_line_to (cr, xx - H, yy + 0.75);
		horizontal_shift = horizontal_shift == 0.5 ? 0.5 - H : 0.5;
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
			cairo_move_to (cr, x * (H + H) + 0.14 + horizontal_shift, y * 0.75 + 0.40);
			char command [24];
			sprintf (command, "%02i%02i", (int) (x + indexing . position . x), (int) (y + indexing . position . y));
			cairo_show_text (cr, command);
		}
		horizontal_shift = horizontal_shift == 0.0 ? H : 0.0;
	}
}

void grid_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	point position = location . position - viewport -> board_position;
	cairo_translate (cr, position . x, position . y);
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
	point size = location . size * scaling * viewport -> scaling;
	cairo_scale (cr, size . x, size . y);
	switch (side) {
	case 0: draw_square_grid (cr, viewport); break;
	case 1: draw_vertical_hex_grid (cr, viewport, false); break;
	case 2: draw_vertical_hex_grid (cr, viewport, true); break;
	case 3: draw_horizontal_hex_grid (cr, viewport, false); break;
	case 4: draw_horizontal_hex_grid (cr, viewport, true); break;
	default: draw_square_grid (cr, viewport); break;
	}
	cairo_identity_matrix (cr);
}

double positivise (double d) {return d >= 0.0 ? d : 0.0;}

rect grid_token :: get_bounding_box (void) {
	point size = location . size;
	switch (side) {
	case 0: size *= indexing . size; break;
	case 1: case 2:
		size *= point (0.75, 0.866025404) * (indexing . size - point (1, 0));
		size += location . size * point (1.0, 0.5);
		break;
	case 3: case 4:
		size *= point (0.866025404, 0.75) * (indexing . size - point (0, 1));
		size += location . size * point (0.5, 1.0);
		break;
	default: break;
	}
	size *= scaling;
	if (rotation == 0.0) return rect (location . position, size);
	double angle = rotation * M_PI / 12.0;
	double absin = abs (sin (angle)), abcos = abs (cos (angle));
	double psin = positivise (sin (angle)), pmsin = positivise (- sin (angle)), pmcos = positivise (- cos (angle));
	return rect (location . position - size . swap () * point (psin, pmsin) - size * pmcos,
		point (size . x * abcos + size . y * absin, size . y * abcos + size . x * absin));
}

colour grid_token :: default_foreground_colour (boarder * board) {return board ? board -> default_grid_foreground_colour : default_foreground ();}
colour grid_token :: default_background_colour (boarder * board) {return board ? board -> default_grid_background_colour : default_background ();}

bool grid_token :: moveOnGrid (boarder_token * token, point position) {
	if (token == 0) return false;
	position -= indexing . position;
	switch (side) {
	case 0:
		position *= location . size;
		break;
	case 1:
		if ((int) position . x % 2 != 0) position . y += 0.5;
		position *= point (0.75, 0.866025404) * location . size;
		break;
	case 2:
		if ((int) position . x % 2 == 0) position . y += 0.5;
		position *= point (0.75, 0.866025404) * location . size;
		break;
	case 3:
		if ((int) position . y % 2 != 0) position . x += 0.5;
		position *= point (0.866025404, 0.75) * location . size;
		break;
	case 4:
		if ((int) position . y % 2 == 0) position . x += 0.5;
		position *= point (0.866025404, 0.75) * location . size;
		break;
	default: break;
	}
	position += location . size . half ();
	position = position . rotate (rotation * M_PI / 12.0) + get_location () . position;
	position -= token -> get_bounding_box () . size . half ();
	token -> set_position (position);
	return true;
}








