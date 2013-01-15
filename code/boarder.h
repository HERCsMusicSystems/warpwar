
#ifndef _BOARDER_
#define _BOARDER_

#include "prolog.h"
#include <gtk/gtk.h>

class point;
class rect;
class colour;
class boarder;
class boarder_viewport;
class boarder_token;
class text_token;
class rectangle_token;

class point {
public:
	double x, y;
	point (double x, double y);
	point (double locations [2]);
	point (void);
};

#define RECT(r) r . position . x, r . position . y, r . size . x, r . size . y
class rect {
public:
	point position;
	point size;
	rect (point offset, point size);
	rect (double x, double y, double width, double height);
	rect (double locations [4]);
	rect (void);
};

#define COLOUR(c) c . red, c . green, c . blue
#define ACOLOUR(c) c . red, c . green, c . blue, c . alpha

double int_to_colour (int c);
int colour_to_int (double c);
class colour {
public:
	double red, green, blue, alpha;
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
	void draw (cairo_t * cr, boarder_viewport * viewport);
	boarder (void);
	~ boarder (void);
};

class boarder_token {
public:
	rect location;
	bool selected;
	colour foreground_colour;
	colour background_colour;
	PrologAtom * atom;
	boarder_token * next;
	virtual void draw (cairo_t * cr, boarder_viewport * viewport);
	boarder_token (void);
	virtual ~ boarder_token (void);
};

class boarder_viewport {
public:
	boarder * board;
	GtkWidget * window;
	PrologAtom * atom;
	PROLOG_STRING name;
	rect location;
	boarder_viewport * next;
	point board_position;
	void setWindowLocation (rect location);
	void setBoardPosition (point position);
	boarder_viewport (boarder * board, PrologAtom * atom, char * name, rect location, boarder_viewport * next);	
	~ boarder_viewport (void);
};

class text_token : public boarder_token {
};

class rectangle_token : public boarder_token {
public:
	virtual void draw (cairo_t * cr, boarder_viewport * viewport);
	rectangle_token (void);
};

#endif





