
#include "boarder.h"
#include <stdlib.h>

extern void cairo_rounded_rectangle (cairo_t * cr, double x, double y, double width, double height, double radius);
extern double abs (double d);

dice_token :: dice_token (PrologAtom * atom) : boarder_token (atom) {sides = 0; side = shift = multiplier = 1; scaling = default_scaling ();}
dice_token :: dice_token (PrologAtom * atom, int sides) : boarder_token (atom) {this -> sides = sides; side = shift = 1; multiplier = 1; scaling = default_scaling ();}
dice_token :: dice_token (PrologAtom * atom, int sides, int shift) : boarder_token (atom) {this -> sides = sides; side = this -> shift = shift; multiplier = 1; scaling = default_scaling ();}
dice_token :: dice_token (PrologAtom * atom, int sides, int shift, int multiplier) : boarder_token (atom) {this -> sides = sides; side = this -> shift = shift; this -> multiplier = multiplier; scaling = default_scaling ();}
dice_token :: ~ dice_token (void) {printf ("	DELETING DICE [%i]\n", sides);}

bool dice_token :: should_save_size (void) {return false;}
double dice_token :: default_scaling (void) {return 64.0;}
void dice_token :: creation_call (FILE * tc) {
	if (sides == 0) {fprintf (tc, "[%s %s]\n", CREATE_DICE, atom -> name ()); return;}
	if (multiplier != 1) {fprintf (tc, "[%s %s %i %i %i]\n", CREATE_DICE, atom -> name (), sides, shift, multiplier); return;}
	if (shift != 1) {fprintf (tc, "[%s %s %i %i]\n", CREATE_DICE, atom -> name (), sides, shift); return;}
	fprintf (tc, "[%s %s %i]\n", CREATE_DICE, atom -> name (), sides);
}

void dice_token :: draw_cube (cairo_t * cr, boarder_viewport * viewport, rect r, point centre) {
	cairo_translate (cr, POINT (centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 6.0);
	cairo_scale (cr, r . size . x, r . size . y);
	cairo_rounded_rectangle (cr, -0.5, -0.5, 1, 1, 0.125);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	//if (background_colour == foreground_colour) {
	//	cairo_fill (cr);
	//	return;
	//} else {
		cairo_fill_preserve (cr);
		cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
		cairo_stroke (cr);
	//}
	char command [16];
	if (multiplier == 10) sprintf (command, "%02i", side);
	else sprintf (command, "%i", side);
	cairo_text_extents_t extent;
	cairo_set_font_size (cr, scaling * viewport -> scaling * 0.75);
	cairo_text_extents (cr, command, & extent);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_translate (cr, POINT (centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 6.0);
	cairo_translate (cr, extent . width * -0.5, extent . height * 0.5);
	cairo_show_text (cr, command);
	cairo_identity_matrix (cr);
}

void dice_token :: draw_dice (cairo_t * cr, boarder_viewport * viewport, rect r, point centre) {
	cairo_translate (cr, POINT (centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 6.0);
	cairo_scale (cr, r . size . x, r . size . y);
	cairo_rounded_rectangle (cr, -0.5, -0.5, 1, 1, 0.125);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	cairo_fill (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	double arc = M_PI + M_PI;
	switch (side) {
	case 1:
		cairo_arc (cr, 0, 0, 0.125, 0, arc); cairo_fill (cr);
		break;
	case 2:
		cairo_arc (cr, -0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		break;
	case 3:
		cairo_arc (cr, -0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0, 0, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		break;
	case 4:
		cairo_arc (cr, -0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, -0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		break;
	case 5:
		cairo_arc (cr, -0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, -0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0, 0, 0.125, 0, arc); cairo_fill (cr);
		break;
	case 6:
		cairo_arc (cr, -0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, -0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, -0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, 0.3, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, -0.3, 0, 0.125, 0, arc); cairo_fill (cr);
		cairo_arc (cr, 0.3, 0, 0.125, 0, arc); cairo_fill (cr);
		break;
	default: break;
	}
	cairo_rounded_rectangle (cr, -0.5, -0.5, 1, 1, 0.125);
	cairo_identity_matrix (cr);
	cairo_stroke (cr);
}

void dice_token :: draw_tetrahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre) {
	cairo_translate (cr, POINT (centre));
	double hour = M_PI / 6.0;
	if (rotation != 0.0) cairo_rotate (cr, rotation * hour);
	cairo_scale (cr, r . size . x, r . size . y);
	double angle = hour * -3;
	cairo_move_to (cr, 0.5 * cos (angle), 0.5 * sin (angle)); angle += 4.0 * hour;
	cairo_line_to (cr, 0.5 * cos (angle), 0.5 * sin (angle)); angle += 4.0 * hour;
	cairo_line_to (cr, 0.5 * cos (angle), 0.5 * sin (angle));
	cairo_close_path (cr);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	//if (foreground_colour == background_colour) {
	//	cairo_fill (cr);
	//	return;
	//} else {
		cairo_fill_preserve (cr);
		cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
		cairo_stroke (cr);
	//}
	char command [16];
	sprintf (command, "%i", side);
	cairo_text_extents_t extent;
	cairo_set_font_size (cr, scaling * viewport -> scaling * 0.5);
	cairo_text_extents (cr, command, & extent);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_translate (cr, POINT (centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 6.0);
	cairo_translate (cr, extent . width * -0.5, extent . height * 0.5);
	cairo_show_text (cr, command);
	cairo_identity_matrix (cr);
}

void dice_token :: draw_octahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre) {
	cairo_translate (cr, POINT (centre));
	double hour = M_PI / 6.0;
	if (rotation != 0.0) cairo_rotate (cr, rotation * hour);
	cairo_scale (cr, r . size . x, r . size . y);
	double angle = hour * -3;
	cairo_move_to (cr, 0.5 * cos (angle), 0.5 * sin (angle)); angle += 2.0 * hour;
	cairo_line_to (cr, 0.5 * cos (angle), 0.5 * sin (angle)); angle += 2.0 * hour;
	cairo_line_to (cr, 0.5 * cos (angle), 0.5 * sin (angle)); angle += 2.0 * hour;
	cairo_line_to (cr, 0.5 * cos (angle), 0.5 * sin (angle)); angle += 2.0 * hour;
	cairo_line_to (cr, 0.5 * cos (angle), 0.5 * sin (angle)); angle += 2.0 * hour;
	cairo_line_to (cr, 0.5 * cos (angle), 0.5 * sin (angle));
	cairo_close_path (cr);
	cairo_line_to (cr, 0.5 * cos (hour), 0.5 * sin (hour));
	cairo_line_to (cr, 0.5 * cos (hour * 5), 0.5 * sin (hour * 5));
	cairo_close_path (cr);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	//if (foreground_colour == background_colour) {
	//	cairo_fill (cr);
	//	return;
	//} else {
		cairo_fill_preserve (cr);
		cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
		cairo_stroke (cr);
	//}
	char command [16];
	sprintf (command, "%i", side);
	cairo_text_extents_t extent;
	cairo_set_font_size (cr, scaling * viewport -> scaling * 0.5);
	cairo_text_extents (cr, command, & extent);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_translate (cr, POINT (centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 6.0);
	cairo_translate (cr, extent . width * -0.5, extent . height * 0.5);
	cairo_show_text (cr, command);
	cairo_identity_matrix (cr);
}

void dice_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	location . size = point (scaling, scaling);
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * viewport -> scaling);
	point centre = r . centre ();
	switch (sides) {
	case 0: draw_dice (cr, viewport, r, centre); break;
	case 4: draw_tetrahedron (cr, viewport, r, centre); break;
	case 6: draw_cube (cr, viewport, r, centre); break;
	case 8: draw_octahedron (cr, viewport, r, centre); break;
	default: draw_cube (cr, viewport, r, centre); break;
	}
}

rect dice_token :: get_bounding_box (void) {
	if (rotation == 0.0) return location;
	rect ret = location;
	double angle = rotation * M_PI / 6.0;
	ret . size = point (abs (scaling * cos (angle)) + abs (scaling * sin (angle)), abs (scaling * cos (angle)) + abs (scaling * sin (angle)));
	ret . position . x += location . size . x * 0.5 - ret . size . x * 0.5;
	ret . position . y += location . size . y * 0.5 - ret . size . y * 0.5;
	return ret;
}

int dice_token :: randomize_side (void) {if (sides > 0) return side = multiplier * (rand () % sides + shift); return side = rand () % 6 + 1;}
































