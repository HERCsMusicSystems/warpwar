
#include "boarder.h"
#include <stdlib.h>

///////////////////////
// ROUNDED RECTANGLE //
///////////////////////

void cairo_rounded_rectangle (cairo_t * cr, double x, double y, double width, double height, double radius) {
	cairo_new_sub_path (cr);
	double half = M_PI * 0.5;
	cairo_arc (cr, x + width - radius, y + radius, radius, - half, 0);
	cairo_arc (cr, x + width - radius, y + height - radius, radius, 0, half);
	cairo_arc (cr, x + radius, y + height - radius, radius, half, M_PI);
	cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI + half);
	cairo_close_path (cr);
}

#ifndef WINDOWS_OPERATING_SYSTEM
double abs (double value) {return value >= 0.0 ? value : - value;}
#endif

///////////////////
// TOKEN COUNTER //
///////////////////

int token_counter = 0;

///////////
// BOARD //
///////////

boarder :: boarder (PrologRoot * root) {
	background_colour = colour (0, 0, 255);
	viewports = 0;
	tokens = 0;
	deck = 0;
	this -> root = root;
	not_ready_for_drop = true;
	default_dice_foreground_colour = colour (0, 0, 0); default_dice_background_colour = colour (255, 255, 255);
	default_tetrahedron_foreground_colour = colour (255, 255, 255); default_tetrahedron_background_colour = colour (255, 0, 0);
	default_cube_foreground_colour = colour (255, 255, 255); default_cube_background_colour = colour (0, 0, 255);
	default_octahedron_foreground_colour = colour (255, 255, 255); default_octahedron_background_colour = colour (0, 255, 0);
	default_deltahedron_foreground_colour = colour (255, 255, 255); default_deltahedron_background_colour = colour (0xff, 0x8c, 0x00);
	default_deltahedron_10_foreground_colour = colour (255, 255, 255); default_deltahedron_10_background_colour = colour (0x8a, 0x2b, 0xe2);
	default_dodecahedron_foreground_colour = colour (255, 255, 255); default_dodecahedron_background_colour = colour (0x80, 0x80, 0x80);
	default_icosahedron_foreground_colour = colour (255, 255, 255); default_icosahedron_background_colour = colour (255, 0, 0);
	default_rectangle_foreground_colour = default_circle_foreground_colour = colour (255, 255, 0);
	default_rectangle_background_colour = default_circle_background_colour = colour (0, 0, 255);
	default_picture_foreground_colour = default_text_foreground_colour = default_deck_foreground_colour = default_grid_foreground_colour = colour (255, 255, 0);
	default_picture_background_colour = default_text_background_colour = default_deck_background_colour = default_grid_background_colour = colour (0, 0, 255);
}

boarder :: ~ boarder (void) {
	if (viewports != 0) delete viewports; viewports = 0;
	erase ();
}

static void save_colour (FILE *tc, char * atom, char * command, colour c, colour default_c) {
	if (c == default_c) return;
	if (c . alpha == 1.0) fprintf (tc, "[%s %s %i %i %i]\n", atom, command, INTCOLOUR (c));
	else fprintf (tc, "[%s %s %i %i %i %i]\n", atom, command, AINTCOLOUR (c));
}

static void save_colour (FILE *tc, char * command, colour c, colour default_c) {
	if (c == default_c) return;
	if (c . alpha == 1.0) fprintf (tc, "[%s %i %i %i]\n", command, INTCOLOUR (c));
	else fprintf (tc, "[%s %i %i %i %i]\n", command, AINTCOLOUR (c));
}

void boarder :: erase (void) {if (tokens != 0) delete tokens; tokens = 0;}
void boarder :: erase_selection (void) {
	while (tokens != 0 && tokens -> selected) {
		boarder_token * token = tokens;
		tokens = token -> next;
		token -> next = 0;
		delete token;
	}
	if (tokens == 0) return;
	boarder_token * ts = tokens;
	while (ts -> next != 0) {
		if (ts -> next -> selected) {
			boarder_token * token = ts -> next;
			ts -> next = token -> next;
			token -> next = 0;
			delete token;
		} else ts = ts -> next;
	}
}

void boarder :: apply_colour_to_selection (int red, int green, int blue, bool foreground) {
	boarder_token * token = tokens;
	while (token != 0) {
		if (token -> selected) {
			if (foreground) token -> foreground_colour = colour (red, green, blue, colour_to_int (token -> foreground_colour . alpha));
			else token -> background_colour = colour (red, green, blue, colour_to_int (token -> background_colour . alpha));
		}
		token = token -> next;
	}
}

bool boarder :: save (char * location) {
		#ifdef WIN32
	FILE * tc = fopen (location, "wt");
		#else
	FILE * tc = fopen (location, "wb");
		#endif
	if (tc == 0) return false;
	fprintf (tc, "[auto_atoms]\n\n");
	fprintf (tc, "[%s]\n", ERASE);
	save_colour (tc, BACKGROUND_COLOUR, background_colour, colour (0, 0, 255));
	save_colour (tc, DEFAULT_DICE_FOREGROUND, default_dice_foreground_colour, colour (0, 0, 0));
	save_colour (tc, DEFAULT_DICE_BACKGROUND, default_dice_background_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_TETRAHEDRON_FOREGROUND, default_tetrahedron_foreground_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_TETRAHEDRON_BACKGROUND, default_tetrahedron_background_colour, colour (255, 0, 0));
	save_colour (tc, DEFAULT_CUBE_FOREGROUND, default_cube_foreground_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_CUBE_BACKGROUND, default_cube_background_colour, colour (0, 0, 255));
	save_colour (tc, DEFAULT_OCTAHEDRON_FOREGROUND, default_octahedron_foreground_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_OCTAHEDRON_BACKGROUND, default_octahedron_background_colour, colour (0, 255, 0));
	save_colour (tc, DEFAULT_DELTAHEDRON_FOREGROUND, default_deltahedron_foreground_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_DELTAHEDRON_BACKGROUND, default_deltahedron_background_colour, colour (0xff, 0x8c, 0x00));
	save_colour (tc, DEFAULT_DELTAHEDRON10_FOREGROUND, default_deltahedron_10_foreground_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_DELTAHEDRON10_BACKGROUND, default_deltahedron_10_background_colour, colour (0x8a, 0x2b, 0xe2));
	save_colour (tc, DEFAULT_DODECAHEDRON_FOREGROUND, default_dodecahedron_foreground_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_DODECAHEDRON_BACKGROUND, default_dodecahedron_background_colour, colour (0x80, 0x80, 0x80));
	save_colour (tc, DEFAULT_ICOSAHEDRON_FOREGROUND, default_icosahedron_foreground_colour, colour (255, 255, 255));
	save_colour (tc, DEFAULT_ICOSAHEDRON_BACKGROUND, default_icosahedron_background_colour, colour (255, 0, 0));
	save_colour (tc, DEFAULT_RECTANGLE_FOREGROUND, default_rectangle_foreground_colour, colour (255, 255, 0));
	save_colour (tc, DEFAULT_RECTANGLE_BACKGROUND, default_rectangle_background_colour, colour (0, 0, 255));
	save_colour (tc, DEFAULT_CIRCLE_FOREGROUND, default_circle_foreground_colour, colour (255, 255, 0));
	save_colour (tc, DEFAULT_CIRCLE_BACKGROUND, default_circle_background_colour, colour (0, 0, 255));
	save_colour (tc, DEFAULT_PICTURE_FOREGROUND, default_picture_foreground_colour, colour (255, 255, 0));
	save_colour (tc, DEFAULT_PICTURE_BACKGROUND, default_picture_background_colour, colour (0, 0, 255));
	save_colour (tc, DEFAULT_TEXT_FOREGROUND, default_text_foreground_colour, colour (255, 255, 0));
	save_colour (tc, DEFAULT_TEXT_BACKGROUND, default_text_background_colour, colour (0, 0, 255));
	save_colour (tc, DEFAULT_DECK_FOREGROUND, default_deck_foreground_colour, colour (255, 255, 0));
	save_colour (tc, DEFAULT_DECK_BACKGROUND, default_deck_background_colour, colour (0, 0, 255));
	save_colour (tc, DEFAULT_GRID_FOREGROUND, default_grid_foreground_colour, colour (255, 255, 0));
	save_colour (tc, DEFAULT_GRID_BACKGROUND, default_grid_background_colour, colour (0, 0, 255));
	fprintf (tc, "\n");
	if (tokens) tokens -> save (this, tc);
	if (viewports) viewports -> save (tc);
	fprintf (tc, "[wait 100]\n");
	fprintf (tc, "[%s]\n\n", CLEAN);
	fprintf (tc, "[exit]\n\n");
	fclose (tc);
	return true;
}

boarder_viewport * boarder :: insert_viewport (PrologAtom * atom, char * name, rect location) {return viewports = new boarder_viewport (this, atom, name, location, viewports);}

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
	if (token == 0) return 0;
	if (deck != 0) {
		if (! deck -> can_insert ()) return 0;
		return deck -> insert (token);
	}
	token -> next = tokens;
	return tokens = token;
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
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	cairo_rectangle (cr, 0.0, 0.0, viewport -> location . size . x, viewport -> location . size . y);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	cairo_fill (cr);
	if (tokens == 0) return;
	tokens -> draw (cr, viewport);
	double dashes [] = {10.0};
	cairo_set_dash (cr, dashes, 1, 0.0);
	cairo_set_source_rgba (cr, 1.0, 0.0, 0.0, 1.0);
	tokens -> draw_selection (cr, viewport);
}

void boarder :: clear_selection (bool select) {
	boarder_token * token = tokens;
	while (token != 0) {
		token -> selected = select;
		token = token -> next;
	}
}

boarder_token * boarder :: hit_test (rect area) {if (tokens) return tokens -> hit_test (area); return 0;}

void boarder :: repaint (void) {
	boarder_viewport * viewport = viewports;
	while (viewport != 0) {
		gtk_widget_queue_draw (viewport -> window);
		viewport = viewport -> next;
	}
}

void boarder :: move_selection (point delta) {
	boarder_token * token = tokens;
	while (token != 0) {
		//if (token -> selected) token -> set_position (delta + token -> get_location () . position);
		if (token -> selected) token -> move_position (delta);
		token = token -> next;
	}
}

void boarder :: resize_selection (point delta, bool minimise, bool maximise) {
	boarder_token * token = tokens;
	while (token != 0) {
		if (token -> selected) {
			point size = token -> get_location () . size;
			size += delta;
			if (maximise) size . maximise ();
			if (minimise) size . minimise ();
			token -> set_size (size);
		}
		token = token -> next;
	}
}

void boarder :: reindex_selection (void) {
	boarder_token * token = tokens;
	while (token != 0) {
		if (token -> selected) token -> no_indexing = ! token -> no_indexing;
		token = token -> next;
	}
}

void boarder :: reindex_selection (rect reindex) {
	boarder_token * token = tokens;
	while (token != 0) {
		if (token -> selected) {
			token -> indexing . position += reindex . position;
			token -> indexing . size += reindex . size;
		}
		token = token -> next;
	}
}

void boarder :: rotate_selection (double step, bool free_rotation) {
	boarder_token * token = tokens;
	while (token != 0) {
		if (token -> selected) {
			token -> rotation += step;
			if (! free_rotation) token -> rotation = (double) ((int) token -> rotation);
		}
		token = token -> next;
	}
}

void boarder :: reside_selection (int step, bool sides) {
	boarder_token * token = tokens;
	while (token != 0) {
		if (token -> selected) {
			if (sides) token -> set_sides (token -> get_sides () + step);
			else token -> side += step;
		}
		token = token -> next;
	}
}

void boarder :: rescale_selection (double step) {
	boarder_token * token = tokens;
	while (token != 0) {
		if (token -> selected) token -> scaling *= step;
		token = token -> next;
	}
}

void boarder :: reorder_selection_to_back (void) {
	if (tokens == 0) return;
	if (tokens -> next == 0) return;
	if (tokens -> selected) {
		boarder_token * selected = tokens;
		tokens = selected -> next;
		selected -> next = tokens -> next;
		tokens -> next = selected;
		return;
	}
	boarder_token * token = tokens;
	while (token -> next != 0 && ! token -> next -> selected) token = token -> next;
	boarder_token * selected = token -> next;
	if (selected == 0) return;
	if (selected -> next == 0) return;
	token -> next = selected -> next;
	selected -> next = selected -> next -> next;
	token -> next -> next = selected;
}

void boarder :: reorder_selection_to_front (void) {
	if (tokens == 0) return;
	if (tokens -> selected) return;
	if (tokens -> next == 0) return;
	if (tokens -> next -> selected) {
		boarder_token * selected = tokens -> next;
		tokens -> next = selected -> next;
		selected -> next = tokens;
		tokens = selected;
		return;
	}
	boarder_token * token = tokens;
	while (token -> next != 0 && token -> next -> next != 0 && ! token -> next -> next -> selected) token = token -> next;
	boarder_token * selected = token -> next;
	if (selected == 0) return;
	selected = selected -> next;
	if (selected == 0) return;
	token -> next -> next = selected -> next;
	selected -> next = token -> next;
	token -> next = selected;
}

void boarder :: reorder_selection_to_very_back (void) {
	if (tokens == 0) return;
	boarder_token * token = tokens;
	if (token -> selected) {
		while (token -> next != 0) token = token -> next;
		boarder_token * selected = tokens;
		tokens = selected -> next;
		selected -> next = 0;
		token -> next = selected;
		return;
	}
	while (token -> next != 0 && ! token -> next -> selected) token = token -> next;
	boarder_token * selected = token -> next;
	if (selected == 0) return;
	token -> next = selected -> next;
	while (token -> next != 0) token = token -> next;
	token -> next = selected;
	selected -> next = 0;
}

void boarder :: reorder_selection_to_very_front (void) {
	if (tokens == 0) return;
	boarder_token * token = tokens;
	while (token -> next != 0 && ! token -> next -> selected) token = token -> next;
	boarder_token * selected = token -> next;
	if (selected == 0) return;
	token -> next = selected -> next;
	selected -> next = tokens;
	tokens = selected;
}

bool boarder :: transfer_token_to_deck (boarder_token * deck, boarder_token * token) {
	if (! deck -> can_insert ()) return false;
	boarder_token * btp = tokens;
	if (btp == token) {tokens = btp -> next; deck -> insert (token); return true;}
	while (btp -> next != 0) {
		if (btp -> next == token) {btp -> next = token -> next; deck -> insert (token); return true;}
		btp = btp -> next;
	}
	return false;
}

void boarder :: transfer_selection_to_deck (boarder_token * deck) {
	boarder_token * token = tokens;
	while (token != 0) {
		boarder_token * next = token -> next;
		if (token -> selected && token != deck) transfer_token_to_deck (deck, token);
		token = next;
	}
}

boarder_token * boarder :: release_token_from_deck (boarder_token * deck) {
	if (! deck -> can_insert ()) return 0;
	boarder_token * btp = deck -> release ();
	if (btp == 0) return 0;
	btp -> next = tokens;
	tokens = btp;
	btp -> set_position (deck -> get_location () . position);
	return btp;
}

boarder_token * boarder :: release_token_from_selection (bool random) {
	if (tokens == 0) return 0;
	boarder_token * token = tokens;
	while (token != 0 && (! token -> selected || ! token -> can_insert ())) token = token -> next;
	if (token == 0) return 0;
	if (random) return release_random_token_from_deck (token);
	return release_token_from_deck (token);
}

bool boarder :: shuffle_deck_from_selection (void) {
	if (tokens == 0) return false;
	boarder_token * token = tokens;
	while (token != 0 && (! token -> selected || ! token -> can_insert ())) token = token -> next;
	if (token != 0) return token -> shuffle ();
	return false;
}

bool boarder :: randomise_selected_dices (void) {
	boarder_token * token = tokens;
	bool randomised = false;
	while (token != 0) {
		if (token -> selected) {token -> randomise_side (); randomised = true;}
		token = token -> next;
	}
	return randomised;
}

boarder_token * boarder :: release_random_token_from_deck (boarder_token * deck) {
	if (! deck -> can_insert ()) return 0;
	boarder_token * btp = deck -> release_random ();
	if (btp == 0) return 0;
	btp -> next = tokens;
	tokens = btp;
	btp -> set_position (deck -> get_location () . position);
	return btp;
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
	board_position = point (0.0, 0.0);
	scaling = 1.0;
	edit_mode = boarder_viewport :: move;
	token_counter++;
}

boarder_viewport :: ~ boarder_viewport (void) {
	printf ("DELETING VIEWPORT [%s %s] <%i %i>\n", atom -> name (), name, (int) board_position . x, (int) board_position . y);
	if (atom) atom -> removeAtom ();
	if (next) delete next; next = 0;
	token_counter--;
}

void boarder_viewport :: save (FILE * tc) {
	if (next) next -> save (tc);
	fprintf (tc, "[%s %s \"%s\" %i %i %i %i]\n", VIEWPORT, atom -> name (), name,
		(int) location . position . x, (int) location . position . y, (int) location . size . x, (int) location . size . y);
	fprintf (tc, "[%s %s %i %i]\n", atom -> name (), POSITION, (int) board_position . x, (int) board_position . y);
	if (scaling != 1.0) fprintf (tc, "[%s %s %g]\n", atom -> name (), SCALING, scaling);
	if (edit_mode != 0) fprintf (tc, "[%s %s %i]\n", atom -> name (), MODE, (int) edit_mode);
	fprintf (tc, "\n");
}

void boarder_viewport :: setWindowLocation (rect location) {
	this -> location = location;
	if (window == 0) return;
	gtk_window_move (GTK_WINDOW (window), (int) location . position . x, (int) location . position . y);
	gtk_window_resize (GTK_WINDOW (window), (int) location . size . x, (int) location . size . y);
}
void boarder_viewport :: setWindowSize (point size) {
	this -> location . size = size;
	if (window == 0) return;
	gtk_window_resize (GTK_WINDOW (window), (int) location . size . x, (int) location . size . y);
}
void boarder_viewport :: setBoardPosition (point position) {this -> board_position = position;}

///////////////////
// GENERIC TOKEN //
///////////////////

boarder_token :: boarder_token (PrologAtom * atom) {
	location = rect (point (10, 10), point (64, 64));
	selected = false;
	locked = false;
	foreground_colour = default_foreground ();
	background_colour = default_background ();
	scaling = default_scaling ();
	rotation = 0.0;
	this -> atom = atom;
	atom -> inc ();
	side = 0;
	indexing = rect (0, 0, 4, 4);
	no_indexing = false;
	next = 0;
	token_counter++;
}
boarder_token :: ~ boarder_token (void) {
	//printf ("DELETING TOKEN [%s]\n", atom -> name ());
	if (atom) {
		PrologNativeCode * code = atom -> getMachine ();
		if (code) delete code;
		//atom -> unProtect ();
		atom -> setMachine (0);
		//atom -> unProtect ();
		atom -> removeAtom ();
	}
	atom = 0;
	if (next) delete next; next = 0;
	token_counter--;
}

bool boarder_token :: can_insert (void) {return false;}
boarder_token * boarder_token :: insert (boarder_token * token) {return 0;}
boarder_token * boarder_token :: release (void) {return 0;}
boarder_token * boarder_token :: release_random (void) {return 0;}
bool boarder_token :: shuffle (void) {return false;}
bool boarder_token :: moveOnGrid (boarder_token * token, point position) {return false;}

void boarder_token :: save (boarder * board, FILE * tc) {
	if (next) next -> save (board, tc);
	creation_call (board, tc);
	rect token_location = get_location ();
	if (should_save_size ()) fprintf (tc, "[%s %s %i %i %i %i]\n", atom -> name (), LOCATION, (int) token_location . position . x, (int) token_location . position . y, (int) token_location . size . x, (int) token_location . size . y);
	else {
		if (token_location . position != point (10, 10)) fprintf (tc, "[%s %s %i %i]\n", atom -> name (), POSITION, (int) token_location . position . x, (int) token_location . position . y);
	}
	if (background_colour != default_background_colour (board)) {
		if (background_colour . alpha == 1.0) fprintf (tc, "[%s %s %i %i %i]\n", atom -> name (), BACKGROUND_COLOUR, INTCOLOUR (background_colour));
		else fprintf (tc, "[%s %s %i %i %i %i]\n", atom -> name (), BACKGROUND_COLOUR, AINTCOLOUR (background_colour));
	}
	if (foreground_colour != default_foreground_colour (board)) {
		if (foreground_colour . alpha == 1.0) fprintf (tc, "[%s %s %i %i %i]\n", atom -> name (), FOREGROUND_COLOUR, INTCOLOUR (foreground_colour));
		else fprintf (tc, "[%s %s %i %i %i %i]\n", atom -> name (), FOREGROUND_COLOUR, AINTCOLOUR (foreground_colour));
	}
	if (side != 0) fprintf (tc, "[%s %s %i]\n", atom -> name (), SIDE, side);
	if (rotation != 0.0) fprintf (tc, "[%s %s %g]\n", atom -> name (), ROTATION, rotation);
	if (scaling != default_scaling ()) fprintf (tc, "[%s %s %g]\n", atom -> name (), SCALING, scaling);
	if (indexing != rect (0, 0, 4, 4)) fprintf (tc, "[%s %s %i %i %i %i]\n", atom -> name (), INDEXING, (int) indexing . position . x, (int) indexing . position . y, (int) indexing . size . x, (int) indexing . size . y);
	if (no_indexing) fprintf (tc, "[%s %s]\n", atom -> name (), NO_INDEXING);
	if (locked) fprintf (tc, "[%s %s]\n", atom -> name (), LOCK);
	fprintf (tc, "\n");
}

bool boarder_token :: should_save_size (void) {return true;}
double boarder_token :: default_scaling (void) {return 1.0;}

void boarder_token :: draw (cairo_t * cr, boarder_viewport * viewport) {if (next) next -> draw (cr, viewport); internal_draw (cr, viewport);}

void boarder_token :: draw_selection ( cairo_t * cr, boarder_viewport * viewport) {
	if (next) next -> draw_selection (cr, viewport);
	if (! selected) return;
	rect location = get_bounding_box ();
	location = rect (location . position - point (2, 2), location . size + point (4, 4));
	location = rect ((location . position - viewport -> board_position) * viewport -> scaling, location . size * viewport -> scaling);
	cairo_rectangle (cr, RECT (location));
	cairo_stroke (cr);
}

void boarder_token :: set_position (point position) {this -> location . position = position;}
void boarder_token :: move_position (point delta) {this -> location . position += delta;}
void boarder_token :: set_size (point size) {this -> location . size = size;}
void boarder_token :: set_location (rect location) {this -> location = location;}
rect boarder_token :: get_location (void) {return location;}
rect boarder_token :: get_bounding_box (void) {
	if (rotation == 0.0 && scaling == 1.0) return location;
	rect ret = location;
	ret . size = ret . size * scaling;
	if (rotation != 0.0) {
		double angle = rotation * M_PI / 12.0;
		ret . size = point (abs (ret . size . x * cos (angle)) + abs (ret . size . y * sin (angle)), abs (ret . size . y * cos (angle)) + abs (ret . size . x * sin (angle)));
		ret . position . x += location . size . x * 0.5 * scaling - ret . size . x * 0.5;
		ret . position . y += location . size . y * 0.5 * scaling - ret . size . y * 0.5;
	}
	return ret;
}
boarder_token * boarder_token :: hit_test (rect area) {
	if (get_bounding_box () . overlap (area)) return this;
	if (next) return next -> hit_test (area);
	return 0;
}
boarder_token * boarder_token :: hit_test_next (rect area) {
	if (next == 0) return 0;
	return next -> hit_test (area);
}

int boarder_token :: randomise_side (void) {return side;}
bool boarder_token :: set_text (char * text) {return false;}
char * boarder_token :: get_text (void) {return "";}
bool boarder_token :: set_sides (int sides) {return false;}
int boarder_token :: get_sides (void) {return 0;}

colour boarder_token :: default_foreground (void) {return colour (255, 255, 0);}
colour boarder_token :: default_background (void) {return colour (0, 0, 255);}

////////////
// TOKENS //
////////////

// RECTANGLE

rectangle_token :: rectangle_token (PrologAtom * atom) : boarder_token (atom) {}
rectangle_token :: ~ rectangle_token (void) {
	printf ("	DELETING RECTANGLE [%s]\n", atom -> name ());
}

void rectangle_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * (scaling * viewport -> scaling));
	point centre = r . centre ();
	cairo_translate (cr, centre . x, centre . y);
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
	cairo_scale (cr, r . size . x, r . size . y);
	cairo_rectangle (cr, -0.5, -0.5, 1, 1);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	if (background_colour == foreground_colour) {cairo_fill (cr); return;}
	cairo_fill_preserve (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
}

void rectangle_token :: creation_call (boarder * board, FILE * tc) {fprintf (tc, "[%s %s]\n", CREATE_RECTANGLE, atom -> name ());}

colour rectangle_token :: default_foreground_colour (boarder * board) {return board ? board -> default_rectangle_foreground_colour : default_foreground ();}
colour rectangle_token :: default_background_colour (boarder * board) {return board ? board -> default_rectangle_background_colour : default_background ();}

// CIRCLE

circle_token :: circle_token (PrologAtom * atom) : boarder_token (atom) {}
circle_token :: ~ circle_token (void) {
	printf ("	DELETING CIRCLE [%s]\n", atom -> name ());
}

void circle_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * (scaling * viewport -> scaling));
	point centre = r . centre ();
	cairo_translate (cr, centre . x, centre . y);
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
	cairo_scale (cr, r . size . x, r . size . y);
	cairo_arc (cr, 0, 0, 0.5, 0, 2 * M_PI);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	if (background_colour == foreground_colour) {cairo_fill (cr); return;}
	cairo_fill_preserve (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
}

void circle_token :: creation_call (boarder * board, FILE * tc) {fprintf (tc, "[%s %s]\n", CREATE_CIRCLE, atom -> name ());}

colour circle_token :: default_foreground_colour (boarder * board) {return board ? board -> default_circle_foreground_colour : default_foreground ();}
colour circle_token :: default_background_colour (boarder * board) {return board ? board -> default_circle_background_colour : default_background ();}

// PICTURE

picture_token :: picture_token (PrologAtom * atom, char * picture_location, int sides) : boarder_token (atom) {
	this -> picture_location = create_text (picture_location);
	surface = cairo_image_surface_create_from_png (picture_location);
	if (cairo_surface_status (surface) != CAIRO_STATUS_SUCCESS) {picture_size = point (16, 16); cairo_surface_destroy (surface); surface = 0;}
	else picture_size = point (cairo_image_surface_get_width (surface), cairo_image_surface_get_height (surface));
	this -> sides = sides;
	resize ();
}

picture_token :: ~ picture_token (void) {
	printf ("	DELETING PICTURE [%s %s]\n", atom -> name (), picture_location);
	delete_text (picture_location);
	if (surface != 0) cairo_surface_destroy (surface);
}

void picture_token :: resize (void) {
	if (sides < 1) sides = 1;
	if (sides > (int) picture_size . y) sides = (int) picture_size . y;
	location . size = point (picture_size . x, picture_size . y / sides);
}

bool picture_token :: set_sides (int sides) {this -> sides = sides; resize (); return true;}
int picture_token :: get_sides (void) {return this -> sides;}

void picture_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	if (surface == 0) {
		point beginning = (location . position - viewport -> board_position) * viewport -> scaling;
		cairo_rectangle (cr, POINT (beginning), POINT (location . size));
		cairo_set_source_rgba (cr, 1.0, 0.0, 0.0, 1.0);
		cairo_stroke (cr);
		return;
	}
	point half = - location . size . half ();
	point centre = (location . centre (scaling) - viewport -> board_position) * viewport -> scaling;
	cairo_translate (cr, POINT (centre));
	double scale = scaling * viewport -> scaling;
	if (scale != 1.0) cairo_scale (cr, scale, scale);
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
	cairo_rectangle (cr, POINT (half), POINT (location . size));
	if (side != 0) half . y -= location . size . y * side;
	if (0 <= side && side < sides) cairo_set_source_surface (cr, surface, POINT (half));
	else cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_fill (cr);
	cairo_identity_matrix (cr);
}

bool picture_token :: should_save_size (void) {return false;}
void picture_token :: set_size (point size) {}
void picture_token :: set_location (rect location) {this -> location . position = location . position;}
void picture_token :: creation_call (boarder * board, FILE * tc) {
	if (sides == 1) fprintf (tc, "[%s %s \"%s\"]\n", CREATE_PICTURE, atom -> name (), picture_location);
	else fprintf (tc, "[%s %s \"%s\" %i]\n", CREATE_PICTURE, atom -> name (), picture_location, sides);
}

colour picture_token :: default_foreground_colour (boarder * board) {return board ? board -> default_picture_foreground_colour : default_foreground ();}
colour picture_token :: default_background_colour (boarder * board) {return board ? board -> default_picture_background_colour : default_background ();}

// TEXT

text_token :: text_token (PrologAtom * atom, char * text) : boarder_token (atom) {
	this -> text = create_text (text);
	scaling = default_scaling ();
}

text_token :: ~ text_token (void) {
	printf ("	DELETING TEXT [%s %s]\n", atom -> name (), text);
	delete_text (text);
}

bool text_token :: set_text (char * text) {
	if (this -> text != 0) delete this -> text;
	this -> text = create_text (text);
	return true;
}

char * text_token :: get_text (void) {if (text == 0) return ""; return text;}

void text_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	cairo_identity_matrix (cr);
	cairo_move_to (cr, 0, 0);
	cairo_text_extents_t extent;
	cairo_set_font_size (cr, scaling * viewport -> scaling);
	cairo_text_extents (cr, text, & extent);
	if (viewport -> scaling != 0.0) location . size = point (extent . width, extent . height) / viewport -> scaling;
	else location . size = point (extent . width, extent . height);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	rect Location (point (location . position - viewport -> board_position) * viewport -> scaling, location . size * viewport -> scaling);
	point centre = Location . centre ();
	cairo_translate (cr, POINT (centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
	cairo_translate (cr, Location . size . x * -0.5, Location . size . y * 0.5);
	cairo_move_to (cr, 0, 0);
	cairo_show_text (cr, text);
	cairo_identity_matrix (cr);
}

void text_token :: creation_call (boarder * board, FILE * tc) {fprintf (tc, "[%s %s \"%s\"]\n", CREATE_TEXT, atom -> name (), text);}
bool text_token :: should_save_size (void) {return false;}
double text_token :: default_scaling (void) {return 24.0;}

rect text_token :: get_bounding_box (void) {
	if (rotation == 0.0) return location;
	rect ret = location;
	double angle = rotation * M_PI / 12.0;
	ret . size = point (abs (ret . size . x * cos (angle)) + abs (ret . size . y * sin (angle)), abs (ret . size . y * cos (angle)) + abs (ret . size . x * sin (angle)));
	ret . position . x += location . size . x * 0.5 - ret . size . x * 0.5;
	ret . position . y += location . size . y * 0.5 - ret . size . y * 0.5;
	return ret;
}

colour text_token :: default_foreground_colour (boarder * board) {return board ? board -> default_text_foreground_colour : default_foreground ();}
colour text_token :: default_background_colour (boarder * board) {return board ? board -> default_text_background_colour : default_background ();}

// DECK

deck_token :: deck_token (PrologAtom * atom, char * text) : boarder_token (atom) {
	this -> text = text ? create_text (text) : 0;
	tokens = 0;
}

deck_token :: ~ deck_token (void) {
	printf ("	DELETING DECK [%s %s]\n", atom -> name (), text);
	if (text) delete_text (text); text = 0;
	if (tokens) delete tokens; tokens = 0;
}

bool deck_token :: set_text (char * text) {
	if (this -> text != 0) delete this -> text;
	this -> text = create_text (text);
	return true;
}

char * deck_token :: get_text (void) {if (text == 0) return ""; return text;}

void deck_token :: creation_call (boarder * board, FILE * tc) {
	if (text == 0) fprintf (tc, "[%s %s]\n", CREATE_DECK, atom -> name ());
	else fprintf (tc, "[%s %s \"%s\"]\n", CREATE_DECK, atom -> name (), text);
	if (tokens) {
		fprintf (tc, "[%s %s]\n", atom -> name (), SELECT_DECK);
		tokens -> save (board, tc);
		fprintf (tc, "[%s]\n", SELECT_DECK);
	}
}

void deck_token :: internal_draw (cairo_t * cr, boarder_viewport * viewport) {
	rect r ((location . position - viewport -> board_position) * viewport -> scaling, location . size * (scaling * viewport -> scaling));
	point centre = r . centre ();
	cairo_translate (cr, POINT (centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
	cairo_scale (cr, POINT (r . size));
	cairo_rectangle (cr, -0.5, -0.5, 1, 1);
	cairo_identity_matrix (cr);
	cairo_set_source_rgba (cr, ACOLOUR (background_colour));
	if (background_colour == foreground_colour) {cairo_fill (cr); return;}
	cairo_fill_preserve (cr);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	cairo_stroke (cr);
	if (text == 0 || no_indexing) return;
	cairo_move_to (cr, 0, 0);
	cairo_text_extents_t extent;
	cairo_set_font_size (cr, indexing . size . y * viewport -> scaling * scaling);
	cairo_text_extents (cr, text, & extent);
	point size;
	if (viewport -> scaling != 0.0) size = point (extent . width, extent . height) / viewport -> scaling;
	else size = point (extent . width, extent . height);
	cairo_set_source_rgba (cr, ACOLOUR (foreground_colour));
	rect Location (point (location . position + location . size * 0.5 * scaling - size * 0.5 - viewport -> board_position) * viewport -> scaling, size * viewport -> scaling);
	point Centre = Location . centre ();
	cairo_translate (cr, POINT (Centre));
	if (rotation != 0.0) cairo_rotate (cr, rotation * M_PI / 12.0);
	point shift = indexing . position * viewport -> scaling * scaling;
	cairo_translate (cr, Location . size . x * -0.5 + shift . x, Location . size . y * 0.5 + shift . y);
	cairo_move_to (cr, 0, 0);
	cairo_show_text (cr, text);
	cairo_identity_matrix (cr);
}

bool deck_token :: can_insert (void) {return true;}
boarder_token * deck_token :: insert (boarder_token * token) {if (token == 0) return 0; token -> next = tokens; return tokens = token;}
boarder_token * deck_token :: release (void) {boarder_token * ret = tokens; if (tokens) tokens = tokens -> next; if (ret) ret -> next = 0; return ret;}
bool deck_token :: shuffle (void) {
	if (tokens == 0) return true;
	boarder_token * accumulator = 0;
	boarder_token * bp = release_random ();
	while (bp != 0) {bp -> next = accumulator; accumulator = bp; bp = release_random ();}
	tokens = accumulator;
	return true;
}

boarder_token * deck_token :: release_random (void) {
	if (tokens == 0) return 0;
	return get_from_deck (rand () % get_token_count ());
}

int deck_token :: get_token_count (void) {
	boarder_token * bp = tokens;
	int counter = 0;
	while (bp) {bp = bp -> next; counter++;}
	return counter;
}

boarder_token * deck_token :: get_from_deck (int position) {
	if (tokens == 0) return 0;
	boarder_token * bp = tokens;
	if (position < 1) {tokens = tokens -> next; return bp;}
	while (bp != 0 && position > 1) {bp = bp -> next; position--;}
	if (bp != 0) {boarder_token * ret = bp -> next; if (ret != 0) bp -> next = ret -> next; else bp -> next = 0; return ret;}
	return 0;
}

colour deck_token :: default_foreground_colour (boarder * board) {return board ? board -> default_deck_foreground_colour : default_foreground ();}
colour deck_token :: default_background_colour (boarder * board) {return board ? board -> default_deck_background_colour : default_background ();}






















































