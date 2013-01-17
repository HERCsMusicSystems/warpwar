
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
	if (tokens != 0) delete tokens; tokens = 0;
}

boarder_viewport * boarder :: insert_viewport (PrologAtom * atom, char * name, rect location) {
	viewports = new boarder_viewport (this, atom, name, location, viewports);
	return viewports;
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
	token -> next = tokens;
	tokens = token;
	return token;
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
	printf ("DELETING VIEWPORT [%s] <%i %i>\n", name, board_position . x, board_position . y);
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

void boarder_token :: draw (cairo_t * cr, boarder_viewport * viewport) {}

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

circle_token :: circle_token (PrologAtom * atom) : boarder_token (atom) {}
circle_token :: ~ circle_token (void) {
	printf ("	DELETING CIRCLE\n");
}

void circle_token :: draw (cairo_t * cr, boarder_viewport * viewport) {
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	rect r (location . position - viewport -> board_position, location . size);
	cairo_stroke (cr);
}



























































