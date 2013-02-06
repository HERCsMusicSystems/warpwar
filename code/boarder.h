
#ifndef _BOARDER_
#define _BOARDER_

#include <math.h>

#include "prolog.h"
#include <gtk/gtk.h>

#define LOCATION "Location"
#define SIZE "Size"
#define POSITION "Position"
#define SCALING "Scaling"
#define ROTATION "Rotation"
#define SIDES "Sides"
#define SIDE "Side"
#define ROLL "Roll"
#define BACKGROUND_COLOUR "BackgroundColour"
#define FOREGROUND_COLOUR "ForegroundColour"
#define LOCK "Lock"
#define UNLOCK "Unlock"
#define IS_LOCKED "Locked?"
#define SELECT "Select"
#define DESELECT "Deselect"
#define IS_SELECTED "Selected?"
#define VIEWPORT "Viewport"
#define REPAINT "Repaint"
#define SAVE_BOARD "SaveBoard"
#define CLEAN "Clean"
#define IS_CLEAN "Clean?"
#define ERASE "Erase"
#define INDEXING "Indexing"
#define NO_INDEXING "NoIndexing"
#define INDEXED "Indexed?"
#define CREATE_RECTANGLE "CreateRectangle"
#define CREATE_CIRCLE "CreateCircle"
#define CREATE_PICTURE "CreatePicture"
#define CREATE_TEXT "CreateText"
#define CREATE_DICE "CreateDice"
#define CREATE_GRID "CreateGrid"

class point;
class rect;
class colour;
class boarder;
class boarder_viewport;
class boarder_token;
class text_token;
class rectangle_token;

#define POINT(p) p . x, p . y

class point {
public:
	double x, y;
	point (double x, double y);
	point (double locations [2]);
	point (void);
	point operator + (const point & p) const;
	point operator - (const point & p) const;
	point operator - (void) const;
	point operator * (const double & scale) const;
	point operator / (const double & scale) const;
	bool operator == (const point & p) const;
	bool operator != (const point & p) const;
	point operator += (const point & p);
	point operator *= (const double & d);
	point operator *= (const point & p);
	point half (void);
	void round (void);
};

#define RECT(r) r . position . x, r . position . y, r . size . x, r . size . y
class rect {
public:
	point position;
	point size;
	point centre (void);
	point centre (double scaling);
	bool overlap (rect area);
	void positivise (void);
	bool operator == (const rect & r) const;
	bool operator != (const rect & r) const;
	rect (point offset, point size);
	rect (double x, double y, double width, double height);
	rect (double locations [4]);
	rect (void);
};

#define COLOUR(c) c . red, c . green, c . blue
#define ACOLOUR(c) c . red, c . green, c . blue, c . alpha
#define INTCOLOUR(c) colour_to_int (c . red), colour_to_int (c . green), colour_to_int (c . blue)
#define AINTCOLOUR(c) colour_to_int (c . red), colour_to_int (c . green), colour_to_int (c . blue), colour_to_int (c . alpha)

double int_to_colour (int c);
int colour_to_int (double c);
class colour {
public:
	double red, green, blue, alpha;
	bool operator == (const colour & c) const;
	bool operator != (const colour & c) const;
	colour (int red, int green, int blue, int alpha = 255);
	colour (void);
};

class boarder {
public:
	colour background_colour;
	boarder_viewport * viewports;
	boarder_token * tokens;
	boarder_viewport * insert_viewport (PrologAtom * atom, char * name, rect location);
	void remove_viewport (boarder_viewport * viewport);
	boarder_token * insert_token (boarder_token * token);
	void remove_token (boarder_token * token);
	void erase (void);
	bool save (char * location);
	void draw (cairo_t * cr, boarder_viewport * viewport);
	void clear_selection (bool select = false);
	boarder_token * hit_test (rect area);
	void repaint (void);
	void move_selection (point delta);
	boarder (void);
	~ boarder (void);
};

class boarder_viewport {
public:
	boarder * board;
	GtkWidget * window;
	PrologAtom * atom;
	PROLOG_STRING name;
	rect location;
	point board_position;
	double scaling;
	boarder_viewport * next;
	void setWindowLocation (rect location);
	void setWindowSize (point size);
	void setBoardPosition (point position);
	void save (FILE * tc);
	boarder_viewport (boarder * board, PrologAtom * atom, char * name, rect location, boarder_viewport * next);	
	~ boarder_viewport (void);
};

class boarder_token {
protected:
	rect location;
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport) = 0;
public:
	colour foreground_colour;
	colour background_colour;
	double scaling;
	double rotation;
	int side;
	rect indexing;
	bool no_indexing;
	bool locked;
	bool selected;
	PrologAtom * atom;
	boarder_token * next;
	void draw (cairo_t * cr, boarder_viewport * viewport);
	void draw_selection (cairo_t * cr, boarder_viewport * viewport);
	virtual void creation_call (FILE * tc) = 0;
	virtual bool should_save_size (void);
	virtual double default_scaling (void);
	virtual void set_position (point position);
	virtual void set_size (point size);
	virtual void set_location (rect location);
	virtual rect get_location (void);
	virtual rect get_bounding_box (void);
	virtual int randomize_side (void);
	boarder_token * hit_test (rect area);
	boarder_token * hit_test_next (rect area);
	void save (FILE * tc);
	boarder_token (PrologAtom * atom);
	virtual ~ boarder_token (void);
};

class text_token : public boarder_token {
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	char * text;
	virtual void creation_call (FILE * tc);
	virtual bool should_save_size (void);
	virtual double default_scaling (void);
	virtual rect get_bounding_box (void);
	text_token (PrologAtom * atom, char * text);
	virtual ~ text_token (void);
};

class rectangle_token : public boarder_token {
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	virtual void creation_call (FILE * tc);
	rectangle_token (PrologAtom * atom);
	virtual ~ rectangle_token (void);
};

class circle_token : public boarder_token {
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	virtual void creation_call (FILE * tc);
	circle_token (PrologAtom * atom);
	virtual ~ circle_token (void);
};

class picture_token : public boarder_token {
private:
	int sides;
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	cairo_surface_t * surface;
	char * picture_location;
	virtual void creation_call (FILE * tc);
	virtual bool should_save_size (void);
	virtual void set_size (point size);
	virtual void set_location (rect size);
	picture_token (PrologAtom * atom, char * picture_location, int sides = 1);
	virtual ~ picture_token (void);
};

class dice_token : public boarder_token {
protected:
	int sides, shift, multiplier;
	void draw_dice (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_tetrahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_cube (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_octahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_deltahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_dodecahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_icosahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	virtual void creation_call (FILE * tc);
	virtual bool should_save_size (void);
	virtual double default_scaling (void);
	virtual rect get_bounding_box (void);
	virtual int randomize_side (void);
	dice_token (PrologAtom * atom);
	dice_token (PrologAtom * atom, int sides);
	dice_token (PrologAtom * atom, int sides, int shift);
	dice_token (PrologAtom * atom, int sides, int shift, int multiplier);
	virtual ~ dice_token (void);
};

class grid_token : public boarder_token {
protected:
	void draw_square_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_vertical_hex_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre, bool initial);
	void draw_horizontal_hex_grid (cairo_t * cr, boarder_viewport * viewport, rect r, point centre, bool initial);
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	virtual void creation_call (FILE * tc);
	virtual bool should_save_size (void);
	virtual double default_scaling (void);
	virtual rect get_bounding_box (void);
	grid_token (PrologAtom * atom);
	virtual ~ grid_token (void);
};

#endif











































