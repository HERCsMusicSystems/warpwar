
#include "boarder.h"

///////////////////////
// POINT RECT COLOUR //
///////////////////////

point :: point (void) {x = y = 0.0;}
point :: point (double x, double y) {this -> x = x; this -> y = y;}
point :: point (double locations [2]) {this -> x = locations [0]; this -> y = locations [1];}

point point :: operator + (const point & p) const {return point (x + p . x, y + p . y);}
point point :: operator - (const point & p) const {return point (x - p . x, y - p . y);}
point point :: operator * (const double & scale) const {return point (x * scale, y * scale);}

rect :: rect (void) {position = size = point (0.0, 0.0);}
rect :: rect (point position, point size) {this -> position = position; this -> size = size;}
rect :: rect (double locations [4]) {position = point (locations [0], locations [1]); size = point (locations [2], locations [3]);}
rect :: rect (double x, double y, double width, double height) {position = point (x, y); size = point (width, height);}

double int_to_colour (int c) {return c >= 255 ? 1.0 : (double) c / 256.0;}
int colour_to_int (double c) {return c >= 1.0 ? 255 : (int) (256.0 * c);}
colour :: colour (int red, int green, int blue, int alpha) {this -> red = int_to_colour (red); this -> green = int_to_colour (green); this -> blue = int_to_colour (blue); this -> alpha = int_to_colour (alpha);}
colour :: colour (void) {red = green = blue = 0.0; alpha = 1.0;}


///////////
// BOARD //
///////////

boarder :: boarder (void) {
	background_colour = colour (0, 0, 255);
	viewports = 0;
	tokens = 0;
}

boarder :: ~ boarder (void) {
	if (viewports != 0) delete viewports; viewports = 0;
	erase ();
}

void boarder :: erase (void) {if (tokens != 0) delete tokens; tokens = 0;}

bool boarder :: save (char * location) {
	FILE * tc = fopen (location, "wb");
	if (tc == 0) return false;
	fprintf (tc, "[auto_atoms]\n\n");
	fprintf (tc, "[%s]\n\n", ERASE);
	boarder_token * token = tokens;
	while (token != 0) {
		token -> creation_call (tc);
		fprintf (tc, "[%s %s %i %i %i %i]\n", token -> atom -> name (), LOCATION, (int) token -> location . position . x, (int) token -> location . position . y, (int) token -> location . size . x, (int) token -> location . size . y);
		if (token -> background_colour . alpha == 1.0) fprintf (tc, "[%s %s %i %i %i]\n", token -> atom -> name (), BACKGROUND_COLOUR, INTCOLOUR (token -> background_colour));
		else fprintf (tc, "[%s %s %i %i %i %i]\n", token -> atom -> name (), BACKGROUND_COLOUR, AINTCOLOUR (token -> background_colour));
		if (token -> foreground_colour . alpha == 1.0) fprintf (tc, "[%s %s %i %i %i]\n", token -> atom -> name (), FOREGROUND_COLOUR, INTCOLOUR (token -> foreground_colour));
		else fprintf (tc, "[%s %s %i %i %i %i]\n", token -> atom -> name (), FOREGROUND_COLOUR, AINTCOLOUR (token -> foreground_colour));
		if (token -> scaling != 1.0) fprintf (tc, "[%s %s %g]\n", token -> atom -> name (), SCALING, token -> scaling);
		if (token -> locked) fprintf (tc, "[%s %s]\n", token -> atom -> name (), LOCK);
		fprintf (tc, "\n");
		token = token -> next;
	}
	boarder_viewport * viewport = viewports;
	while (viewport != 0) {
		fprintf (tc, "[%s %s \"%s\" %i %i %i %i]\n", VIEWPORT, viewport -> atom -> name (), viewport -> name, (int) viewport -> location . position . x, (int) viewport -> location . position . y, (int) viewport -> location . size . x, (int) viewport -> location . size . y);
		fprintf (tc, "[%s %s %i %i]\n", viewport -> atom -> name (), POSITION, (int) viewport -> board_position . x, (int) viewport -> board_position . y);
		if (viewport -> scaling != 1.0) fprintf (tc, "[%s %s %g]\n", viewport -> atom -> name (), SCALING, viewport -> scaling);
		fprintf (tc, "\n");
		viewport = viewport -> next;
	}
	fprintf (tc, "[%s]\n\n", CLEAN);
	fprintf (tc, "[exit]\n\n");
	fclose (tc);
	return true;
}

boarder_viewport * boarder :: insert_viewport (PrologAtom * atom, char * name, rect location) {
	boarder_viewport * viewport = new boarder_viewport (this, atom, name, location, viewports);
	if (viewports == 0) return viewports = viewport;
	boarder_viewport * vp = viewports;
	while (vp -> next != 0) vp = vp -> next;
	return vp -> next = viewport;
}

void boarder :: remove_viewport (boarder_viewport * viewport) {
	if (viewport == 0) return;
	if (viewports == viewport) {
		viewports = viewport -> next;
		viewport -> next = 0;
		delete viewport;
		return;
	}
	boarder_viewport * bvp = viewports;
	while (bvp -> next != 0) {
		if (bvp -> next == viewport) {
			bvp -> next = bvp -> next -> next;
			viewport -> next = 0;
			delete viewport;
			return;
		}
		bvp = bvp -> next;
	}
}

boarder_token * boarder :: insert_token (boarder_token * token) {
	if (tokens == 0) return tokens = token;
	boarder_token * bp = tokens;
	while (bp -> next != 0) bp = bp -> next;
	return bp -> next = token;
}

void boarder :: remove_token (boarder_token * token) {
	if (token == 0) return;
	if (tokens == token) {
		tokens = token -> next;
		token -> next = 0;
		delete token;
		return;
	}
	boarder_token * btp = tokens;
	while (btp -> next != 0) {
		if (btp -> next == token) {
			btp -> next = btp -> next -> next;
			token -> next = 0;
			delete token;
			return;
		}
		btp = btp -> next;
	}
}

void boarder :: draw (cairo_t * cr, boarder_viewport * viewport) {
	cairo_rectangle (cr, 0.0, 0.0, viewport -> location . size . x, viewport -> location . size . y);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	cairo_fill (cr);
	boarder_token * token = tokens;
	while (token) {
		token -> draw (cr, viewport);
		token = token -> next;
	}
}

//////////////
// VIEWPORT //
//////////////

boarder_viewport :: boarder_viewport (boarder * board, PrologAtom * atom, char * name, rect location, boarder_viewport * next) {
	this -> board = board;
	window = 0;
	this -> atom = atom;
	atom -> inc ();
	prolog_string_copy (this -> name, name);
	this -> location = location;
	this -> next = next;
	board_position = point (0, 0);
	scaling = 1.0;
}

boarder_viewport :: ~ boarder_viewport (void) {
	if (atom) atom -> removeAtom ();
	printf ("DELETING VIEWPORT [%s] <%i %i>\n", name, (int) board_position . x, (int) board_position . y);
	if (next) delete next; next = 0;
}

void boarder_viewport :: setWindowLocation (rect location) {
	this -> location = location;
	if (window == 0) return;
	gtk_window_move (GTK_WINDOW (window), location . position . x, location . position . y);
	gtk_window_resize (GTK_WINDOW (window), location . size . x, location . size . y);
}
void boarder_viewport :: setWindowSize (point size) {
	this -> location . size = size;
	if (window == 0) return;
	gtk_window_resize (GTK_WINDOW (window), location . size . x, location . size . y);
}
void boarder_viewport :: setBoardPosition (point position) {this -> board_position = position;}

////////////
// TOKENS //
////////////

boarder_token :: boarder_token (PrologAtom * atom) {
	location = rect (point (10, 10), point (200, 100));
	selected = false;
	locked = false;
	foreground_colour = colour (255, 255, 0);
	background_colour = colour (0, 0, 255);
	scaling = 1.0;
	this -> atom = atom;
	atom -> inc ();
	next = 0;
}
boarder_token :: ~ boarder_token (void) {
	if (atom) atom -> removeAtom ();
	printf ("DELETING TOKEN\n");
	if (next) delete next; next = 0;
}

rectangle_token :: rectangle_token (PrologAtom * atom) : boarder_token (atom) {}
rectangle_token :: ~ rectangle_token (void) {
	printf ("	DELEING RECTANGLE\n");
}

void rectangle_token :: draw (cairo_t * cr, boarder_viewport * viewport) {
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	//cairo_set_line_width (cr, 1.0);
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * (scaling * viewport -> scaling));
	cairo_rectangle (cr, RECT (r));
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	cairo_fill_preserve (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
}

char * rectangle_token :: creation_call (FILE * tc) {fprintf (tc, "[%s %s]\n", CREATE_RECTANGLE, atom -> name ());}

circle_token :: circle_token (PrologAtom * atom) : boarder_token (atom) {}
circle_token :: ~ circle_token (void) {
	printf ("	DELETING CIRCLE\n");
}

void circle_token :: draw (cairo_t * cr, boarder_viewport * viewport) {
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	rect r (location . position - viewport -> board_position, location . size);
	cairo_stroke (cr);
}

char * circle_token :: creation_call (FILE * tc) {fprintf (tc, "[%s %s]\n", CREATE_CIRCLE, atom -> name ());}

picture_token :: picture_token (PrologAtom * atom, char * picture_location) : boarder_token (atom) {
	this -> picture_location = create_text (picture_location);
	surface = cairo_image_surface_create_from_png (picture_location);
}
picture_token :: ~ picture_token (void) {
	printf ("	DELETING PICTURE\n");
	delete_text (picture_location);
	cairo_surface_destroy (surface);
}

void picture_token :: draw (cairo_t * cr, boarder_viewport * viewport) {
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	//cairo_set_line_width (cr, 1.0);
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * (scaling * viewport -> scaling));
	cairo_rectangle (cr, RECT (r));
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	cairo_fill_preserve (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
	cairo_set_source_surface (cr, surface, location . position . x, location . position . y);
	cairo_paint (cr);
}

char * picture_token :: creation_call (FILE * tc) {fprintf (tc, "[%s %s \"%s\"]\n", CREATE_PICTURE, atom -> name (), picture_location);}


























































