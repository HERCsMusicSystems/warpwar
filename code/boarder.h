
#ifndef _BOARDER_
#define _BOARDER_

#define _USE_MATH_DEFINES
#include <math.h>
#include "graphics2d.h"

#include "prolog.h"
#include <gtk/gtk.h>

#define DEFAULT_RECTANGLE_FOREGROUND "DefaultRectangleForeground"
#define DEFAULT_RECTANGLE_BACKGROUND "DefaultRectangleBackground"
#define DEFAULT_CIRCLE_FOREGROUND "DefaultCircleForeground"
#define DEFAULT_CIRCLE_BACKGROUND "DefaultCircleBackground"
#define DEFAULT_PICTURE_FOREGROUND "DefaultPictureForeground"
#define DEFAULT_PICTURE_BACKGROUND "DefaultPictureBackground"
#define DEFAULT_TEXT_FOREGROUND "DefaultTextForeground"
#define DEFAULT_TEXT_BACKGROUND "DefaultTextBackground"
#define DEFAULT_DECK_FOREGROUND "DefaultDeckForeground"
#define DEFAULT_DECK_BACKGROUND "DefaultDeckBackground"
#define DEFAULT_GRID_FOREGROUND "DefaultGridForeground"
#define DEFAULT_GRID_BACKGROUND "DefaultGridBackground"
#define DEFAULT_DICE_FOREGROUND "DefaultDiceForeground"
#define DEFAULT_DICE_BACKGROUND "DefaultDiceBackground"
#define DEFAULT_TETRAHEDRON_FOREGROUND "DefaultTetrahedronForeground"
#define DEFAULT_TETRAHEDRON_BACKGROUND "DefaultTetrahedronBackground"
#define DEFAULT_CUBE_FOREGROUND "DefaultCubeForeground"
#define DEFAULT_CUBE_BACKGROUND "DefaultCubeBackground"
#define DEFAULT_OCTAHEDRON_FOREGROUND "DefaultOctahedronForeground"
#define DEFAULT_OCTAHEDRON_BACKGROUND "DefaultOctahedronBackground"
#define DEFAULT_DELTAHEDRON_FOREGROUND "DefaultDeltahedronForeground"
#define DEFAULT_DELTAHEDRON_BACKGROUND "DefaultDeltahedronBackground"
#define DEFAULT_DELTAHEDRON10_FOREGROUND "DefaultDeltahedron10Foreground"
#define DEFAULT_DELTAHEDRON10_BACKGROUND "DefaultDeltahedron10Background"
#define DEFAULT_DODECAHEDRON_FOREGROUND "DefaultDodecahedronForeground"
#define DEFAULT_DODECAHEDRON_BACKGROUND "DefaultDodecahedronBackground"
#define DEFAULT_ICOSAHEDRON_FOREGROUND "DefaultIcosahedronForeground"
#define DEFAULT_ICOSAHEDRON_BACKGROUND "DefaultIcosahedronBackground"

#define TOKEN_TYPE "TokenType"
#define LOCATION "Location"
#define SIZE "Size"
#define POSITION "Position"
#define SCALING "Scaling"
#define ROTATION "Rotation"
#define SIDES "Sides"
#define TEXT_ATOM "Text"
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
#define MODE "Mode"
#define SAVE_BOARD "SaveBoard"
#define CLEAN "Clean"
#define IS_CLEAN "Clean?"
#define ERASE "Erase"
#define INDEXING "Indexing"
#define NO_INDEXING "NoIndexing"
#define INDEXED "Indexed?"
#define SELECT_DECK "SelectDeck"
#define SHUFFLE "Shuffle"
#define INSERT "Insert"
#define RELEASE "Release"
#define RELEASE_RANDOM "ReleaseRandom"
#define CREATE_RECTANGLE "CreateRectangle"
#define CREATE_CIRCLE "CreateCircle"
#define CREATE_PICTURE "CreatePicture"
#define CREATE_TEXT "CreateText"
#define CREATE_DICE "CreateDice"
#define CREATE_GRID "CreateGrid"
#define CREATE_DECK "CreateDeck"
#define HIT_TEST "HitTest"
#define SELECTION "Selection"
#define IS_DECK "IsDeck?"

class boarder;
class boarder_viewport;
class boarder_token;
class text_token;
class rectangle_token;

class boarder {
public:
	colour background_colour;
	boarder_viewport * viewports;
	boarder_token * tokens;
	boarder_token * deck;
	PrologRoot * root;
	bool not_ready_for_drop;
public:
	colour default_rectangle_foreground_colour, default_rectangle_background_colour;
	colour default_circle_foreground_colour, default_circle_background_colour;
	colour default_picture_foreground_colour, default_picture_background_colour;
	colour default_text_foreground_colour, default_text_background_colour;
	colour default_deck_foreground_colour, default_deck_background_colour;
	colour default_grid_foreground_colour, default_grid_background_colour;
	colour default_dice_foreground_colour, default_dice_background_colour;
	colour default_tetrahedron_foreground_colour, default_tetrahedron_background_colour;
	colour default_cube_foreground_colour, default_cube_background_colour;
	colour default_octahedron_foreground_colour, default_octahedron_background_colour;
	colour default_deltahedron_foreground_colour, default_deltahedron_background_colour;
	colour default_deltahedron_10_foreground_colour, default_deltahedron_10_background_colour;
	colour default_dodecahedron_foreground_colour, default_dodecahedron_background_colour;
	colour default_icosahedron_foreground_colour, default_icosahedron_background_colour;
public:
	boarder_viewport * insert_viewport (PrologAtom * atom, char * name, rect location);
	void remove_viewport (boarder_viewport * viewport);
	boarder_token * insert_token (boarder_token * token);
	void remove_token (boarder_token * token);
	bool transfer_token_to_deck (boarder_token * deck, boarder_token * token);
	void transfer_selection_to_deck (boarder_token * deck);
	boarder_token * release_token_from_deck (boarder_token * deck);
	boarder_token * release_token_from_selection (void);
	boarder_token * release_random_token_from_deck (boarder_token * deck);
	bool randomise_selected_dices (void);
	void erase (void);
	void erase_selection (void);
	bool save (char * location);
	void draw (cairo_t * cr, boarder_viewport * viewport);
	void clear_selection (bool select = false);
	boarder_token * hit_test (rect area);
	void repaint (void);
	void move_selection (point delta);
	void resize_selection (point delta, bool minimise, bool maximise);
	void reindex_selection (void);
	void reindex_selection (rect reindexing);
	void rotate_selection (double step, bool free_rotation);
	void reside_selection (int step, bool sides = false);
	void rescale_selection (double step);
	void reorder_selection_to_front (void);
	void reorder_selection_to_back (void);
	void reorder_selection_to_very_front (void);
	void reorder_selection_to_very_back (void);
	void apply_colour_to_selection (int red, int green, int blue, bool foreground);
	boarder (PrologRoot * root);
	~ boarder (void);
};

class boarder_viewport {
public:
	enum edit_modes {
		move = 0, select,
		create_rectangle, create_circle,
		create_tetrahedron, create_cube, create_dice,
		create_octahedron, create_deltahedron, create_deltahedron_10,
		create_dodecahedron, create_icosahedron,
		create_text, create_grid, create_deck,
		edit_size, edit_indexing, edit_rotation, edit_side, edit_scaling, edit_ordering,
		edit
	};
public:
	boarder * board;
	GtkWidget * window;
	PrologAtom * atom;
	PROLOG_STRING name;
	rect location;
	point board_position;
	double scaling;
	edit_modes edit_mode;
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
	virtual void creation_call (boarder * board, FILE * tc) = 0;
	virtual bool should_save_size (void);
	virtual double default_scaling (void);
	virtual void set_position (point position);
	virtual void move_position (point delta);
	virtual void set_size (point size);
	virtual void set_location (rect location);
	virtual rect get_location (void);
	virtual rect get_bounding_box (void);
	virtual int randomise_side (void);
	virtual bool set_text (char * text);
	virtual char * get_text (void);
	virtual bool set_sides (int sides);
	virtual int get_sides (void);
	virtual bool can_insert (void);
	virtual boarder_token * insert (boarder_token * token);
	virtual boarder_token * release (void);
	virtual boarder_token * release_random (void);
	virtual bool shuffle (void);
	virtual bool moveOnGrid (boarder_token * token, point position);
	colour default_foreground (void);
	colour default_background (void);
	virtual colour default_foreground_colour (boarder * board) = 0;
	virtual colour default_background_colour (boarder * board) = 0;
	boarder_token * hit_test (rect area);
	boarder_token * hit_test_next (rect area);
	void save (boarder * board, FILE * tc);
	boarder_token (PrologAtom * atom);
	virtual ~ boarder_token (void);
};

class text_token : public boarder_token {
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	char * text;
	virtual void creation_call (boarder * board, FILE * tc);
	virtual bool should_save_size (void);
	virtual double default_scaling (void);
	virtual rect get_bounding_box (void);
	virtual colour default_foreground_colour (boarder * board);
	virtual colour default_background_colour (boarder * board);
	virtual bool set_text (char * text);
	virtual char * get_text (void);
	text_token (PrologAtom * atom, char * text);
	virtual ~ text_token (void);
};

class deck_token : public boarder_token {
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
	boarder_token * tokens;
	int get_token_count (void);
	boarder_token * get_from_deck (int position);
public:
	char * text;
	virtual void creation_call (boarder * board, FILE * tc);
	virtual bool can_insert (void);
	virtual boarder_token * insert (boarder_token * token);
	virtual boarder_token * release (void);
	virtual boarder_token * release_random (void);
	virtual bool shuffle (void);
	virtual colour default_foreground_colour (boarder * board);
	virtual colour default_background_colour (boarder * board);
	virtual bool set_text (char * text);
	virtual char * get_text (void);
	deck_token (PrologAtom * atom, char * text);
	virtual ~ deck_token (void);
};

class rectangle_token : public boarder_token {
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	virtual void creation_call (boarder * board, FILE * tc);
	virtual colour default_foreground_colour (boarder * board);
	virtual colour default_background_colour (boarder * board);
	rectangle_token (PrologAtom * atom);
	virtual ~ rectangle_token (void);
};

class circle_token : public boarder_token {
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	virtual void creation_call (boarder * board, FILE * tc);
	virtual colour default_foreground_colour (boarder * board);
	virtual colour default_background_colour (boarder * board);
	circle_token (PrologAtom * atom);
	virtual ~ circle_token (void);
};

class picture_token : public boarder_token {
private:
	int sides;
	point picture_size;
	void resize (void);
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	cairo_surface_t * surface;
	char * picture_location;
	virtual void creation_call (boarder * board, FILE * tc);
	virtual bool should_save_size (void);
	virtual void set_size (point size);
	virtual void set_location (rect size);
	virtual colour default_foreground_colour (boarder * board);
	virtual colour default_background_colour (boarder * board);
	virtual bool set_sides (int sides);
	virtual int get_sides (void);
	picture_token (PrologAtom * atom, char * picture_location, int sides = 1);
	virtual ~ picture_token (void);
};

class dice_token : public boarder_token {
private:
	void draw_dice (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_tetrahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_cube (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_octahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_deltahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_dodecahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
	void draw_icosahedron (cairo_t * cr, boarder_viewport * viewport, rect r, point centre);
protected:
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	int sides, shift, multiplier;
	virtual void creation_call (boarder * board, FILE * tc);
	virtual bool should_save_size (void);
	virtual double default_scaling (void);
	virtual rect get_bounding_box (void);
	virtual int randomise_side (void);
	virtual colour default_foreground_colour (boarder * board);
	virtual colour default_background_colour (boarder * board);
	virtual bool set_sides (int sides);
	virtual int get_sides (void);
	dice_token (PrologAtom * atom);
	dice_token (PrologAtom * atom, int sides);
	dice_token (PrologAtom * atom, int sides, int shift);
	dice_token (PrologAtom * atom, int sides, int shift, int multiplier);
	virtual ~ dice_token (void);
};

class grid_token : public boarder_token {
protected:
	void draw_square_grid (cairo_t * cr, boarder_viewport * viewport);
	void draw_vertical_hex_grid (cairo_t * cr, boarder_viewport * viewport, bool initial);
	void draw_horizontal_hex_grid (cairo_t * cr, boarder_viewport * viewport, bool initial);
	virtual void internal_draw (cairo_t * cr, boarder_viewport * viewport);
public:
	virtual void creation_call (boarder * board, FILE * tc);
	virtual rect get_bounding_box (void);
	virtual colour default_foreground_colour (boarder * board);
	virtual colour default_background_colour (boarder * board);
	virtual bool moveOnGrid (boarder_token * token, point position);
	grid_token (PrologAtom * atom);
	virtual ~ grid_token (void);
};

#endif











































