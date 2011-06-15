///////////////////////////////////////
// Copyright (C) 2011 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

//#define PROTECT

#ifdef PROTECT
#define BOARD_POSITION wxPoint (1450, 900)
#define BOARD_SIZE wxSize (120, 120)
#else
#define BOARD_POSITION wxPoint (100, 100)
#define BOARD_SIZE wxSize (800, 500)
#endif

#include "wx/wx.h"
#include "wx/dcbuffer.h"
#define USE_MATH_DEFINES
#include <math.h>
#include "wx/spinctrl.h"
#include "wx/dnd.h"
#include "wx/colordlg.h"

//#include <sys/time.h>

void toCommand (char * command, wxString & file_name) {
	char * cp = command;
	for (unsigned int ind = 0; ind < file_name . size (); ind++) {
		* cp++ = (char) file_name [ind];
	}
	* cp++ = '\0';
}

#define SYMBOL_SIZE 1000
class SetupFileReader {
public:
	int symbol_control;
		// 0: EOF
		// 1: [
		// 2: ]
		// 3: identifier
		// 4: string
		// 5: integer
		// 6: float
		// 7: unknown
	int int_symbol;
	double float_symbol;
	char symbol [SYMBOL_SIZE];
	FILE * setup_file;
	int act_char;
	SetupFileReader (char * file_name) {
		setup_file = NULL;
		setup_file = fopen (file_name, "rb");
		act_char = 0;
	}
	~ SetupFileReader (void) {close ();}
	void close (void) {
		if (setup_file == NULL) return;
		fclose (setup_file);
		setup_file = NULL;
	}
	bool file_not_found (void) {
		if (setup_file == NULL) return true;
		return false;
	}
	int get_char (void) {
		act_char = fgetc (setup_file);
		return act_char;
	}
	void get_symbol (void) {
		bool negative = false;
		char * symbol_pointer;
		double fraction;
		if (setup_file == NULL) {
			symbol_control = 0;
			return;
		}
		while (act_char < 33) {
			if (act_char == EOF) {
				symbol_control = 0;
				close ();
				return;
			}
			get_char ();
		}
		if (act_char == '[') {
			symbol_control = 1;
			get_char ();
			return;
		}
		if (act_char == ']') {
			symbol_control = 2;
			get_char ();
			return;
		}
		if ((act_char >= 'a' && act_char <= 'z') || act_char == '_' || (act_char >= 'A' && act_char <= 'Z')) {
			symbol_pointer = symbol;
			while ((act_char >= 'a' && act_char <= 'z') || act_char == '_' || (act_char >= 'A' && act_char <= 'Z') || (act_char >= '0' && act_char <= '9')) {
				* (symbol_pointer++) = act_char;
				get_char ();
			}
			symbol_control = 3;
			* symbol_pointer = '\0';
			return;
		}
		if (act_char == '-') {
			negative = true;
			get_char ();
		}
		if (act_char >= '0' && act_char <= '9') {
			int_symbol = act_char - '0';
			get_char ();
			while (act_char >= '0' && act_char <= '9') {
				int_symbol *= 10;
				int_symbol += act_char - '0';
				get_char ();
			}
			if (act_char == 'e') {
				if (negative) int_symbol = - int_symbol;
				negative = false;
				float_symbol = int_symbol;
				symbol_control = 6;
				int shift = 0;
				get_char ();
				if (act_char == '-') {negative = true; get_char ();}
				while (act_char >= '0' && act_char <= '9') {
					shift *= 10;
					shift += act_char - '0';
					get_char ();
				}
				while (shift > 0) {
					if (negative) float_symbol *= 0.1;
					else float_symbol *= 10.0;
					shift--;
				}
				return;
			}
			if (act_char == '.') {
				get_char ();
				float_symbol = int_symbol;
				symbol_control = 6;
				fraction = 1.0 / 10.0;
				int_symbol = 0;
				while (act_char >= '0' && act_char <= '9') {
					float_symbol += ((double) (act_char - '0')) * fraction;
					fraction /= 10.0;
					get_char ();
				}
				if (negative) float_symbol = - float_symbol;
				if (act_char == 'e') {
					int shift = 0;
					get_char ();
					negative = false;
					if (act_char == '-') {negative = true; get_char ();}
					while (act_char >= '0' && act_char <= '9') {
						shift *= 10;
						shift += act_char - '0';
						get_char ();
					}
					while (shift > 0) {
						if (negative) float_symbol *= 0.1;
						else float_symbol *= 10.0;
						shift--;
					}
				}
				return;
			}
			symbol_control = 5;
			if (negative) int_symbol = 0 - int_symbol;
			float_symbol = (double) int_symbol;
			return;
		}
		if (negative) {
			symbol_control = 7;
			return;
		}
		if (act_char == '"') {
			get_char ();
			symbol_pointer = symbol;
			while (act_char > 0 && act_char != '"') {
				if (act_char == '\\')
					get_char ();
				* (symbol_pointer++) = act_char;
				get_char ();
			}
			* symbol_pointer = '\0';
			if (act_char <= 0) {
				symbol_control = 0;
				return;
			}
			symbol_control = 4;
			get_char ();
			return;
		}
		get_char ();
		symbol_control = 7;
	}
	void skip (void) {
		int ind = 1;
		while (ind > 0) {
			get_symbol ();
			switch (symbol_control) {
			case 0: ind = 0; break;
			case 1: ind++; break;
			case 2: ind--; break;
			default: break;
			}
		}
	}
	bool id (char * name) {
		if (symbol_control != 1) return false;
		return strcmp (symbol, name) == 0;
	}
	bool get_string (void) {
		get_symbol ();
		return symbol_control == 4;
	}
	bool get_int (void) {
		get_symbol ();
		return symbol_control == 5;
	}
	bool get_float (void) {
		get_symbol ();
		return symbol_control == 6;
	}
	bool get_id (void) {
		get_symbol ();
		if (symbol_control != 3) return false;
		get_symbol ();
		return symbol_control == 1;
	}
	bool get_id (char * name) {
		get_symbol ();
		if (symbol_control != 3 || strcmp (symbol, name) != 0) return false;
		get_symbol ();
		return symbol_control == 1;
	}
};

static bool stop_threads = false;

class BoardToken {
public:
	enum TokenType {PictureToken = 0, GridToken, DiceToken};
	wxString original_file;
	wxBitmap token;
	wxBitmap rotatedToken;
	wxPoint position;
	wxPoint positionShift;
	wxSize token_size;
	int choosenRotation; // rotation for token, different grids for grid (square, vertical hex 1 and 2, horizontal hex 1 and 2
	//bool isGrid;
	TokenType tokenType;
	bool isSelectable;
	int gridSide;
	wxSize gridSize;
	wxPoint gridStart;
	bool gridIndexing;
	wxColour gridColour;
	wxColour backgroundColour;
	int diceValue;
	int diceMultiplier;
	int diceShift;
	BoardToken * next;
	void Save (FILE * fw) {
		if (next != 0) next -> Save (fw);
		switch (tokenType) {
		case GridToken:
			fprintf (fw, "	grid [\n");
			fprintf (fw, "		type [%i]\n", choosenRotation);
			fprintf (fw, "		side [%i]\n", gridSide);
			fprintf (fw, "		size [%i %i]\n", gridSize . GetWidth (), gridSize . GetHeight ());
			fprintf (fw, "		index [%i %i]\n", gridStart . x, gridStart . y);
			fprintf (fw, "		colour [%i %i %i]\n", gridColour . Red (), gridColour . Green (), gridColour . Blue ());
			if (gridIndexing) fprintf (fw, "		indexed []\n");
			break;
		case PictureToken:
			fprintf (fw, "	token [\n");
			char command [1024];
			toCommand (command, original_file);
			fprintf (fw, "		location [\"%s\"]\n", command);
			fprintf (fw, "		rotation [%i]\n", choosenRotation);
			break;
		case DiceToken:
			fprintf (fw, "	dice [\n");
			fprintf (fw, "		type [%i]\n", choosenRotation);
			fprintf (fw, "		side [%i]\n", gridSide);
			fprintf (fw, "		shift [%i]\n", diceShift);
			fprintf (fw, "		multiplier [%i]\n", diceMultiplier);
			fprintf (fw, "		value [%i]\n", diceValue);
			fprintf (fw, "		colour [%i %i %i]\n", gridColour . Red (), gridColour . Green (), gridColour . Blue ());
			fprintf (fw, "		background [%i %i %i]\n", backgroundColour . Red (), backgroundColour . Green (), backgroundColour . Blue ());
			if (gridIndexing) fprintf (fw, "		indexed []\n");
			break;
		default:
			fprintf (fw, "	unknown [\n");
			break;
		}
		fprintf (fw, "		position [%i %i]\n", position . x, position . y);
		if (isSelectable) fprintf (fw, "		selectable []\n");
		fprintf (fw, "	]\n");
	}
	void drawSquareGrid (wxDC & gridDC, wxPoint location) {
		token_size = wxSize (gridSide * gridSize . x + 1, gridSide * gridSize . y + 1);
		wxFont f = gridDC . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (8);
		gridDC . SetFont (f);
		gridDC . SetTextForeground (gridColour);
		gridDC . SetPen (wxPen (gridColour));
		int xx = location . x;
		int yyFrom = location . y;
		int yyTo = yyFrom + gridSize . y * gridSide;
		for (int x = 0; x <= gridSize . x; x++) {
			gridDC . DrawLine (xx, yyFrom, xx, yyTo);
			xx += gridSide;
		}
		int yy = location . y;
		int xxFrom = location . x;
		int xxTo = xxFrom + gridSize . x * gridSide;
		for (int y = 0; y <= gridSize . y; y++) {
			gridDC . DrawLine (xxFrom, yy, xxTo, yy);
			yy += gridSide;
		}
		if (gridIndexing) {
			int xx = location . x + 1;
			for (int x = 0; x < gridSize . x; x++) {
				int yy = location . y + 1;
				for (int y = 0; y < gridSize . y; y++) {
					gridDC . DrawText (wxString :: Format (_T ("%02i%02i"), x + gridStart . x, y + gridStart . y), xx, yy);
					yy += gridSide;
				}
				xx += gridSide;
			}
		}
	}
	void drawVerticalHexGrid (wxDC & gridDC, wxPoint location, bool initial) {
		double gdrs = (double) gridSide * 0.5;
		double H = gdrs * 0.866025404;
		double half = gdrs * 0.5;
		token_size = wxSize (gdrs * 1.5 * (double) gridSize . x + half + 1, H * 2.0 * gridSize . y + 1 + H);
		wxFont f = gridDC . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (8);
		gridDC . SetFont (f);
		gridDC . SetTextForeground (gridColour);
		gridDC . SetPen (wxPen (gridColour));
		double vertical_shift = initial ? location . y + H : location . y;
		for (int x = 0; x < gridSize . x; x++) {
			double xx = location . x + half + (double) x * gdrs * 1.5;
			double yy = vertical_shift;
			if (vertical_shift == location . y && x < gridSize . x - 1) gridDC . DrawLine (xx + gdrs, yy, xx + gdrs + half, yy + H);
			for (int y = 0; y < gridSize . y; y++) {
				gridDC . DrawLine (xx, yy, xx + gdrs, yy);
				gridDC . DrawLine (xx, yy, xx - half, yy + H);
				gridDC . DrawLine (xx - half, yy + H, xx, yy + H + H);
				if (x == gridSize . x - 1) {
					gridDC . DrawLine (xx + gdrs, yy, xx + gdrs + half, yy + H);
					gridDC . DrawLine (xx + gdrs, yy + H + H, xx + gdrs + half, yy + H);
				}
				if (gridIndexing) gridDC . DrawText (wxString :: Format (_T ("%02i%02i"), x + gridStart . x, y + gridStart . y), xx + 1, yy + 1);
				yy += H * 2.0;
			}
			gridDC . DrawLine (xx, yy, xx + gdrs, yy);
			if (vertical_shift != location . y && x < gridSize . x - 1) gridDC . DrawLine (xx + gdrs, yy, xx + gdrs + half, yy - H);
			vertical_shift = vertical_shift == location . y ? location . y + H : location . y;
		}
	}
	void drawHorizontalHexGrid (wxDC & gridDC, wxPoint location, bool initial) {
		double gdrs = (double) gridSide * 0.5;
		double H = gdrs * 0.866025404;
		double half = gdrs * 0.5;
		token_size = wxSize (H * 2.0 * gridSize . x + 1 + H, gdrs * 1.5 * (double) gridSize . y + half + 1);
		wxFont f = gridDC . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (8);
		gridDC . SetFont (f);
		gridDC . SetTextForeground (gridColour);
		gridDC . SetPen (wxPen (gridColour));
		double horizontal_shift = initial ? location . x + H : location . x;
		for (int y = 0; y < gridSize . y; y++) {
			double xx = horizontal_shift;
			double yy = location . y + half + (double) y * gdrs * 1.5;
			if (horizontal_shift == location . x && y < gridSize . y - 1) gridDC . DrawLine (xx, yy + gdrs, xx + H, yy + gdrs + half);
			for (int x = 0; x < gridSize . x; x++) {
				gridDC . DrawLine (xx, yy, xx, yy + gdrs);
				gridDC . DrawLine (xx, yy, xx + H, yy - half);
				gridDC . DrawLine (xx + H, yy - half, xx + H + H, yy);
				if (y == gridSize . y - 1) {
					gridDC . DrawLine (xx, yy + gdrs, xx + H, yy + gdrs + half);
					gridDC . DrawLine (xx + H, yy + gdrs + half, xx + H + H, yy + gdrs);
				}
				if (gridIndexing) gridDC . DrawText (wxString :: Format (_T ("%02i%02i"), x + gridStart . x, y + gridStart . y), xx + 1, yy + 1);
				xx += H * 2.0;
			}
			gridDC . DrawLine (xx, yy, xx, yy + gdrs);
			if (horizontal_shift != location . x && y < gridSize . y - 1) gridDC . DrawLine (xx, yy + gdrs, xx - H, yy + gdrs + half);
			horizontal_shift = horizontal_shift == location . x ? location . x + H : location . x;
		}
	}
	void drawRegularDice (wxDC & dc) {
		dc . SetPen (wxPen (gridColour));
		dc . SetBrush (wxBrush (backgroundColour));
		int shift = gridSide * 2; shift /= 30;
		dc . DrawRoundedRectangle (position . x + shift, position . y + shift, gridSide - shift - shift, gridSide - shift - shift, 6);
		dc . SetBrush (wxBrush (gridColour));
		int middle = gridSide / 2;
		int edge = gridSide / 4;
		int radius = gridSide / 10;
		switch (diceValue) {
		case 0: dc . DrawCircle (position . x + middle, position . y + middle, radius); break;
		case 1:
			dc . DrawCircle (position . x + middle - edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle + edge, radius);
			break;
		case 2:
			dc . DrawCircle (position . x + middle, position . y + middle, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle - edge, position . y + middle + edge, radius);
			break;
		case 3:
			dc . DrawCircle (position . x + middle - edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle - edge, position . y + middle + edge, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle + edge, radius);
			break;
		case 4:
			dc . DrawCircle (position . x + middle, position . y + middle, radius);
			dc . DrawCircle (position . x + middle - edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle - edge, position . y + middle + edge, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle + edge, radius);
			break;
		case 5:
			dc . DrawCircle (position . x + middle - edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle - edge, radius);
			dc . DrawCircle (position . x + middle - edge, position . y + middle + edge, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle + edge, radius);
			dc . DrawCircle (position . x + middle - edge, position . y + middle, radius);
			dc . DrawCircle (position . x + middle + edge, position . y + middle, radius);
			break;
		default: break;
		}
	}
	void drawOtherDice (wxDC & dc) {
		dc . SetPen (wxPen (gridColour));
		dc . SetBrush (wxBrush (backgroundColour));
		int shift = gridSide * 2; shift /= 30;
		dc . DrawRoundedRectangle (position . x + shift, position . y + shift, gridSide - shift - shift, gridSide - shift - shift, 6);
		wxFont f = dc . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (gridSide / 2);
		dc . SetFont (f);
		dc . SetTextForeground (gridColour);
		wxString text = wxString :: Format (diceMultiplier * choosenRotation > 10 ? _T ("%02i") : _T ("%i"), diceShift + diceValue * diceMultiplier);
		wxSize extent = dc . GetTextExtent (text);
		dc . DrawText (text, position . x + gridSide / 2 - extent . x / 2, position . y + gridSide / 2 - extent . y / 2);
	}
	void drawTetrahedron (wxDC & dc) {
		dc . SetPen (wxPen (gridColour));
		dc . SetBrush (wxBrush (backgroundColour));
		double half = (double) gridSide * 0.5;
		double quarter = half * 0.5;
		double H = half * 0.866025404;
		wxPoint centre = position + wxPoint (half, half);
		wxPoint corners [3];
		switch (diceValue) {
		case 0:
			corners [0] = centre + wxPoint (0, - half);
			corners [1] = centre + wxPoint (H, quarter);
			corners [2] = centre + wxPoint (-H, quarter);
			break;
		case 1:
			corners [0] = centre + wxPoint (half, 0);
			corners [1] = centre + wxPoint (- quarter, H);
			corners [2] = centre + wxPoint (- quarter, - H);
			break;
		case 2:
			corners [0] = centre + wxPoint (0, half);
			corners [1] = centre + wxPoint (- H, - quarter);
			corners [2] = centre + wxPoint (H, - quarter);
			break;
		case 3:
			corners [0] = centre + wxPoint (- half, 0);
			corners [1] = centre + wxPoint (quarter, - H);
			corners [2] = centre + wxPoint (quarter, H);
			break;
		}
		dc . DrawPolygon (3, corners);
		wxFont f = dc . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (gridSide / 3);
		dc . SetFont (f);
		dc . SetTextForeground (gridColour);
		wxString text = wxString :: Format (_T ("%i"), diceShift + diceValue * diceMultiplier);
		wxSize extent = dc . GetTextExtent (text);
		dc . DrawText (text, position . x + half - extent . x / 2, position . y + half - extent . y / 2);
	}
	void drawOctahedron (wxDC & dc) {
		dc . SetPen (wxPen (gridColour));
		dc . SetBrush (wxBrush (backgroundColour));
		double half = (double) gridSide * 0.5;
		double quarter = half * 0.5;
		double H = half * 0.866025404;
		wxPoint centre = position + wxPoint (half, half);
		wxPoint corners [6];
		switch (diceValue) {
		case 0: case 2: case 4: case 6:
			corners [0] = centre + wxPoint (0, - half);
			corners [1] = centre + wxPoint (H, - quarter);
			corners [2] = centre + wxPoint (H, quarter);
			corners [3] = centre + wxPoint (0, half);
			corners [4] = centre + wxPoint (-H, quarter);
			corners [5] = centre + wxPoint (-H, - quarter);
			break;
		default:
			corners [0] = centre + wxPoint (- half, 0);
			corners [1] = centre + wxPoint (- quarter, - H);
			corners [2] = centre + wxPoint (quarter, - H);
			corners [3] = centre + wxPoint (half, 0);
			corners [4] = centre + wxPoint (quarter, H);
			corners [5] = centre + wxPoint (- quarter, H);
			break;
		};
		dc . DrawPolygon (6, corners);
		switch (diceValue) {
		case 0: case 1: case 2: case 3: corners [1] = corners [2]; corners [2] = corners [4]; break;
		default: corners [0] = corners [1]; corners [1] = corners [3]; corners [2] = corners [5]; break;
		}
		dc . DrawPolygon (3, corners);
		wxFont f = dc . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (gridSide / 3);
		dc . SetFont (f);
		dc . SetTextForeground (gridColour);
		wxString text = wxString :: Format (_T ("%i"), diceShift + diceValue * diceMultiplier);
		wxSize extent = dc . GetTextExtent (text);
		dc . DrawText (text, position . x + half - extent . x / 2, position . y + half - extent . y / 2);
	}
	void drawDeltahedron (wxDC & dc) {
		// side = 1.0
		// big delta = V 2 = 1.414213562
		// delta = 1.414213562 / 1.6180339887498948482045868343656 = 0.874032049
		// half delta = 0.437016024
		// other side = 0.485868271
		// across 1 = 0.89945372
		// across 2 = 0.21233222
		// across = across 1 + across 2 = 1.11178594
		// half angle = 25.913646158 degrees
		// angle = 51.827292316 degrees
		dc . SetPen (wxPen (gridColour));
		dc . SetBrush (wxBrush (backgroundColour));
		double half = (double) gridSide * 0.5;
		wxPoint p [4];
		p [0] = position;
		p [1] = position + wxPoint ((double) gridSide * 0.485868271, 0);
		p [2] = position + wxPoint ((double) gridSide * sin (51.827292316 * M_PI / 180.0), (double) gridSide - (double) gridSide * cos (51.827292316 * M_PI / 180.0));
		p [3] = position + wxPoint (0, gridSide);
		dc . DrawPolygon (4, p);
		wxFont f = dc . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (gridSide / 2);
		dc . SetFont (f);
		dc . SetTextForeground (gridColour);
		wxString text = wxString :: Format (diceMultiplier * choosenRotation > 10 ? _T ("%02i") : _T ("%i"), diceShift + diceValue * diceMultiplier);
		wxSize extent = dc . GetTextExtent (text);
		dc . DrawText (text, position . x + half - extent . x / 2, position . y + half - extent . y / 2);
	}
	void drawDodecahedron (wxDC & dc) {
		dc . SetPen (wxPen (gridColour));
		dc . SetBrush (wxBrush (backgroundColour));
		double half = (double) gridSide * 0.5;
		double smaller = half / 1.6180339887498948482045868343656;
		wxPoint centre = position + wxPoint (half, half);
		wxPoint externals [10];
		wxPoint internals [5];
		double angleShift = (double) diceValue * 2.0 * M_PI / 12.0;
		for (int ind = 0; ind < 10; ind++) {
			double angle = angleShift + (double) ind * 2.0 * M_PI / 10.0;
			externals [ind] = centre + wxPoint (0.499 + half * sin (angle), 0.499 - half * cos (angle));
		}
		for (int ind = 0; ind < 5; ind++) {
			double angle = angleShift + (double) ind * 2.0 * M_PI / 5.0;
			internals [ind] = centre + wxPoint (0.499 + smaller * sin (angle), 0.499 - smaller * cos (angle));
		}
		dc . DrawPolygon (10, externals);
		dc . DrawPolygon (5, internals);
		for (int ind = 0; ind < 5; ind++) {
			dc . DrawLine (internals [ind], externals [ind << 1]);
		}
		wxFont f = dc . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (gridSide / 4);
		dc . SetFont (f);
		dc . SetTextForeground (gridColour);
		wxString text = wxString :: Format (_T ("%i"), diceShift + diceValue * diceMultiplier);
		wxSize extent = dc . GetTextExtent (text);
		dc . DrawText (text, position . x + half - extent . x / 2, position . y + half - extent . y / 2);
	}
	void drawIcosahedron (wxDC & dc) {
		dc . SetPen (wxPen (gridColour));
		dc . SetBrush (wxBrush (backgroundColour));
		double half = (double) gridSide * 0.5;
		double smaller = half / 1.6180339887498948482045868343656;
		wxPoint centre = position + wxPoint (half, half);
		wxPoint externals [6];
		wxPoint internals [3];
		double angleShift = (double) diceValue * 2.0 * M_PI / 20.0;
		for (int ind = 0; ind < 6; ind++) {
			double angle = angleShift + (double) ind * 2.0 * M_PI / 6.0;
			externals [ind] = centre + wxPoint (0.499 + half * sin (angle), 0.499 - half * cos (angle));
		}
		for (int ind = 0; ind < 3; ind++) {
			double angle = angleShift + (double) ind * 2.0 * M_PI / 3.0;
			internals [ind] = centre + wxPoint (0.499 + smaller * sin (angle), 0.499 - smaller * cos (angle));
		}
		dc . DrawPolygon (6, externals);
		dc . DrawLine (internals [0], externals [0]);
		dc . DrawLine (internals [0], externals [1]);
		dc . DrawLine (internals [0], internals [1]);
		dc . DrawLine (internals [1], externals [1]);
		dc . DrawLine (internals [1], externals [2]);
		dc . DrawLine (internals [1], externals [3]);
		dc . DrawLine (internals [1], internals [2]);
		dc . DrawLine (internals [2], externals [3]);
		dc . DrawLine (internals [2], externals [4]);
		dc . DrawLine (internals [2], externals [5]);
		dc . DrawLine (internals [0], externals [5]);
		dc . DrawLine (internals [2], internals [0]);
		wxFont f = dc . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (gridSide / 6);
		dc . SetFont (f);
		dc . SetTextForeground (gridColour);
		wxString text = wxString :: Format (_T ("%i"), diceShift + diceValue * diceMultiplier);
		wxSize extent = dc . GetTextExtent (text);
		dc . DrawText (text, position . x + half - extent . x / 2, position . y + half - extent . y / 2);
	}
	void drawDice (wxDC & dc) {
		switch (choosenRotation) {
		case 4: drawTetrahedron (dc); break;
		case 6: if (gridIndexing) drawOtherDice (dc); else drawRegularDice (dc); break;
		case 8: drawOctahedron (dc); break;
		case 10: drawDeltahedron (dc); break;
		case 12: drawDodecahedron (dc); break;
		case 20: drawIcosahedron (dc); break;
		default: drawOtherDice (dc); break;
		}
	}
	void rotate (int angle) {
		switch (tokenType) {
		case GridToken: this -> choosenRotation = angle; break;
		case PictureToken:
			while (angle > 360) angle -= 360;
			while (angle < 0) angle += 360;
			rotatedToken = wxBitmap (token . ConvertToImage () . Rotate (M_PI * (double) angle / 180.0, wxPoint (token . GetWidth () / 2, token . GetHeight () / 2)));
			token_size = wxSize (rotatedToken . GetWidth (), rotatedToken . GetHeight ());
			positionShift = wxPoint ((token . GetWidth () - rotatedToken . GetWidth ()) / 2, (token . GetHeight () - rotatedToken . GetHeight ()) / 2);
			choosenRotation = angle;
			break;
		case DiceToken:
			if (angle < 1) angle = 1;
			choosenRotation = angle;
			//diceValue = diceMultiplier * (choosenRotation - 1) + diceShift;
			diceValue = choosenRotation - 1;
			break;
		default: break;
		}
	}
	void rotateRight (void) {
		int rotations [] = {315, 300, 270, 240, 225, 210, 180, 150, 135, 120, 90, 60, 45, 30, 0};
		switch (tokenType) {
		case GridToken: choosenRotation++; if (choosenRotation > 4) choosenRotation = 0; break;
		case PictureToken:
			for (int ind = 0; ind < 15; ind++) {
				if (choosenRotation > rotations [ind]) return rotate (rotations [ind]);
			}
			rotate (330);
			break;
		case DiceToken:
			choosenRotation++;
			//diceValue = diceMultiplier * (choosenRotation - 1) + diceShift;
			diceValue = choosenRotation - 1;
			break;
		default: break;
		}
	}
	void rotateLeft (void) {
		int rotations [] = {30, 45, 60, 90, 120, 135, 150, 180, 210, 225, 240, 270, 300, 315, 330};
		switch (tokenType) {
		case GridToken: choosenRotation--; if (choosenRotation < 0) choosenRotation = 4; break;
		case PictureToken:
			for (int ind = 0; ind < 15; ind++) {
				if (choosenRotation < rotations [ind]) return rotate (rotations [ind]);
			}
			rotate (0);
			break;
		case DiceToken:
			choosenRotation--;
			if (choosenRotation < 1) choosenRotation = 1;
			//diceValue = diceMultiplier * (choosenRotation - 1) + diceShift;
			diceValue = choosenRotation - 1;
			break;
		default: break;
		}
	}
	void move (wxPoint & delta) {
		position += delta;
	}
	void moveAll (wxPoint & delta) {
		position += delta;
		if (next != 0) next -> moveAll (delta);
	}
	void draw (wxDC & dc) {
		if (next != 0) next -> draw (dc);
		switch (tokenType) {
		case GridToken:
			switch (choosenRotation) {
			case 0: drawSquareGrid (dc, position); break;
			case 1: drawVerticalHexGrid (dc, position, false); break;
			case 2: drawVerticalHexGrid (dc, position, true); break;
			case 3: drawHorizontalHexGrid (dc, position, false); break;
			case 4: drawHorizontalHexGrid (dc, position, true); break;
			default: drawSquareGrid (dc, position); break;
			}
			break;
		case PictureToken: dc . DrawBitmap (rotatedToken, position + positionShift, true); break;
		case DiceToken: drawDice (dc); break;
		default: break;
		}
	}
	BoardToken * drawBorder (wxDC & dc) {
		dc . DrawRectangle (position + positionShift, token_size);
		return next;
	}
	BoardToken * hitFind (wxPoint position, bool selectableOverride) {
		if ((isSelectable || selectableOverride)
			&& position . x >= this -> position . x
			&& position . x < this -> position . x + token_size . GetWidth ()
			&& position . y >= this -> position . y
			&& position . y < this -> position . y + token_size . GetHeight ()
			) return this;
		if (next == 0) return 0;
		return next -> hitFind (position, selectableOverride);
	}
	void roll (void) {
		if (tokenType != DiceToken) return;
		this -> diceValue = rand () % choosenRotation;
		//int roller = rand () % choosenRotation;
		//this -> diceValue = diceShift + roller * diceMultiplier;
	}
	void rollAll (void) {
		roll ();
		if (next != 0) next -> rollAll ();
	}
	void rollNext (void) {
		if (tokenType != DiceToken) return;
		this -> diceValue++;
		if (this -> diceValue > choosenRotation - 1) this -> diceValue = 0;
	}
		//this -> diceValue += diceMultiplier;
		//if (this -> diceValue > diceMultiplier * (choosenRotation - 1) + diceShift) this -> diceValue = diceShift;
//	}
	void rollAllNext (void) {
		rollNext ();
		if (next != 0) next -> rollAllNext ();
	}
	void rollAllNextBy (int ind) {
		int top = choosenRotation;
		while (top > ind) {rollNext (); top--;}
		if (next != 0) next -> rollAllNextBy (ind);
	}
	BoardToken (wxString file_name, wxPoint position, bool centered, BoardToken * next = 0) {
		this -> tokenType = PictureToken;
		this -> gridColour = * wxWHITE;
		this -> isSelectable = true;
		this -> next = next;
		this -> original_file = file_name;
		token . LoadFile (file_name, wxBITMAP_TYPE_PNG);
		this -> position = position;
		if (centered) this -> position -= wxPoint (token . GetWidth () / 2, token . GetHeight () / 2);
		rotate (0);
	}
	BoardToken (wxPoint position, BoardToken * next = 0) {
		this -> tokenType = GridToken;
		this -> gridColour = * wxWHITE;
		this -> isSelectable = true;
		this -> position = position - wxPoint (120, 120);
		this -> next = next;
		choosenRotation = 0;
		gridSide = 60;
		gridSize = wxSize (4, 4);
		gridStart = wxPoint (0, 0);
		gridIndexing = true;
		rotate (0);
	}
	BoardToken (wxPoint position, int type, int side, wxSize size, wxPoint start, bool indexing, bool selectable, wxColour colour, BoardToken * next = 0) {
		this -> tokenType = GridToken;
		this -> gridColour = colour;
		this -> isSelectable = selectable;
		this -> position = position;
		this -> next = next;
		choosenRotation = type;
		gridSide = side;
		gridSize = size;
		gridStart = start;
		gridIndexing = indexing;
	}
	BoardToken (wxPoint position, int diceType, int side, wxColour foreground, wxColour background, bool centered, bool selectable, BoardToken * next = 0) {
		this -> tokenType = DiceToken;
		this -> choosenRotation = diceType;
		this -> gridIndexing = true;
		this -> diceMultiplier = 1;
		this -> diceShift = 1;
		roll ();
		this -> gridSide = side;
		this -> gridColour = foreground;
		this -> backgroundColour = background;
		this -> next = next;
		this -> position = position;
		if (centered) this -> position -= wxPoint (side / 2, side / 2);
		this -> token_size = wxSize (side, side);
		this -> isSelectable = selectable;
	}
	~ BoardToken (void) {
		stop_threads = true;
		if (next != 0) delete next;
	}
	void changeGridSide (int change) {
		if (tokenType == PictureToken) return;
		gridSide += change;
		if (gridSide < 10) gridSide = 10;
		if (tokenType == DiceToken) token_size = wxSize (gridSide, gridSide);
	}
	void setIndexedGrid (void) {
		if (tokenType == PictureToken) return;
		gridIndexing = ! gridIndexing;
	}
	void changeGridIndexing (wxPoint change) {
		switch (tokenType) {
		case GridToken: gridStart += change; break;
		case DiceToken: diceShift += change . x + change . y; diceValue = choosenRotation - 1; break; //diceValue = diceMultiplier * (choosenRotation - 1) + diceShift; break;
		default: break;
		}
	}
	void changeRows (wxSize change) {
		switch (tokenType) {
		case GridToken: gridSize += change; break;
		case DiceToken: diceMultiplier += change . x + change . y; diceValue = choosenRotation - 1; break; //diceValue = diceMultiplier * (choosenRotation - 1) + diceShift; break;
		default: break;
		}
	}
};

static bool idleRepaint = false;

class AnimateDiceThread : public wxThread {
public:
	BoardToken * token;
	AnimateDiceThread (BoardToken * token) {this -> token = token; this -> Create (16384); this -> Run ();}
	virtual ExitCode Entry (void) {
		stop_threads = false;
		idleRepaint = true;
		wxWakeUpIdle ();
		int ind = token -> choosenRotation;
		while (ind > 6) {token -> rollNext (); ind--;}
		while (ind > 0) {
			Sleep (100);
			if (stop_threads) {Exit (); return Wait ();}
			token -> rollNext ();
			idleRepaint = true;
			wxWakeUpIdle ();
			ind--;
		}
		Exit ();
		return Wait ();
	}
};

class AnimateAllDiceThread : public wxThread {
public:
	BoardToken * tokens;
	AnimateAllDiceThread (BoardToken * tokens) {this -> tokens = tokens; this -> Create (16384); this -> Run ();}
	virtual ExitCode Entry (void) {
		stop_threads = false;
		idleRepaint = true;
		wxWakeUpIdle ();
		int ind = 5;
		tokens -> rollAllNextBy (ind);
		while (ind > 0) {
			Sleep (100);
			if (stop_threads) {Exit (); return Wait ();}
			tokens -> rollAllNext ();
			idleRepaint = true;
			wxWakeUpIdle ();
			ind--;
		}
		Exit ();
		return Wait ();
	}
};

class BoardFrame;

class FileReceiver : public wxFileDropTarget {
public:
	BoardFrame * frame;
	FileReceiver (BoardFrame * frame);
	virtual bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString & files);
};

class BoardWindow : public wxWindow {
public:
	wxColour backgroundColour;
	BoardToken * tokens;
	wxPoint lastRightClickPosition;
	BoardToken * dragToken;
	wxPoint capturedPosition;
	bool moveGrid;
	bool moveBoard;
	bool moveTokens;
	bool notMoved;
	bool possibleTokenCirculation;
	bool unlocked;
	bool modified;
	wxColour dicePenColour, diceBrushColour;
	wxColour tetrahedronPenColour, tetrahedronBrushColour;
	wxColour cubePenColour, cubeBrushColour;
	wxColour octahedronPenColour, octahedronBrushColour;
	wxColour deltahedronPenColour, deltahedronBrushColour;
	wxColour deltahedron10PenColour, deltahedron10BrushColour;
	wxColour dodecahedronPenColour, dodecahedronBrushColour;
	wxColour icosahedronPenColour, icosahedronBrushColour;
	BoardWindow (wxWindow * parent, wxWindowID id) : wxWindow (parent, id) {
		modified = false;
		unlocked = false;
		notMoved = true;
		possibleTokenCirculation = false;
		backgroundColour = * wxBLUE;
		moveGrid = moveBoard = moveTokens = true;
//		boardLocation = wxPoint (0, 0);
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		tokens = 0;
		dragToken = 0;
//		lastRightClickPosition = capturedPosition = boardLocation = wxPoint (10, 10);
		lastRightClickPosition = capturedPosition = wxPoint (10, 10);
		dicePenColour = * wxBLACK; diceBrushColour = * wxWHITE;
		tetrahedronPenColour = * wxWHITE; tetrahedronBrushColour = * wxRED;
		cubePenColour = * wxWHITE; cubeBrushColour = * wxBLUE;
		octahedronPenColour = * wxWHITE; octahedronBrushColour = * wxGREEN;
		deltahedronPenColour = * wxWHITE; deltahedronBrushColour = wxColour (0xff, 0x8c, 0x00);
		deltahedron10PenColour = * wxWHITE; deltahedron10BrushColour = wxColour (0x8a, 0x2b, 0xe2);
		dodecahedronPenColour = * wxWHITE; dodecahedronBrushColour = wxColour (0x80, 0x80, 0x80);
		icosahedronPenColour = * wxWHITE; icosahedronBrushColour = * wxRED;
		idleRepaint = false;
	}
	~ BoardWindow (void) {
		stop_threads = true;
	}
	void SaveTokens (FILE * fw) {
		if (tokens != 0) tokens -> Save (fw);
	}
	void OnPaint (wxPaintEvent & event) {
		wxBufferedPaintDC dc (this);
		dc . SetBackground (wxBrush (backgroundColour));
		dc . Clear ();
		// draw board
//		dc . DrawBitmap (board, boardLocation . x, boardLocation . y, true);
		// draw grid
//		dc . DrawBitmap (grid, gridLocation . x, gridLocation . y, true);
		//dc . Blit (0, 0, 300, 200, & gridDC, 0, 0);
		// draw tokens
		if (tokens != 0) tokens -> draw (dc);
		//BoardToken * tkp = tokens;
		//while (tkp != 0) {tkp = tkp -> draw (dc);}
		// draw selected square
		if (dragToken != 0) {
			dc . SetPen (wxPen (wxColour (255, 255, 255), 1, wxDOT));
			dc . SetBrush (* wxTRANSPARENT_BRUSH);
			dragToken -> drawBorder (dc);
		}
	}
	void RollDice (void) {
		if (dragToken == 0) return;
		if (dragToken -> tokenType != BoardToken :: DiceToken) return;
		dragToken -> roll ();
		new AnimateDiceThread (dragToken);
	}
	void RollAllDices (void) {
		if (tokens == 0) return;
		tokens -> rollAll ();
		new AnimateAllDiceThread (tokens);
	}
	void OnLeftDown (wxMouseEvent & event) {
		CaptureMouse ();
		capturedPosition = event . GetPosition ();
		notMoved = true;
		possibleTokenCirculation = false;
		if (tokens == 0) {dragToken = 0; return;}
		if (dragToken != 0) {
			BoardToken * bp = dragToken -> hitFind (capturedPosition, unlocked);
			if (bp == dragToken) possibleTokenCirculation = true;
			else dragToken = 0;
		}
		if (dragToken == 0) dragToken = tokens -> hitFind (capturedPosition, unlocked);
//		if (tokens == 0) {dragToken = 0; return;}
//		if (dragToken != 0) {
//			if (dragToken -> next == 0) dragToken = 0;
//			else dragToken = dragToken -> next -> hitFind (capturedPosition);
//		}
//		if (dragToken == 0) dragToken = tokens -> hitFind (capturedPosition);
		Refresh ();
	}
	void OnLeftUp (wxMouseEvent & event) {
		if (notMoved && possibleTokenCirculation) {
			if (tokens != 0) {
//				BoardToken * tokenToRoll = dragToken;
				if (dragToken != 0) {
					if (dragToken -> next == 0) dragToken = 0;
					else dragToken = dragToken -> next -> hitFind (capturedPosition, unlocked);
				}
				if (dragToken == 0) dragToken = tokens -> hitFind (capturedPosition, unlocked);
//				if (dragToken != 0) {
//					if (tokenToRoll == dragToken) new AnimateDiceThread (tokenToRoll, this); //tokenToRoll -> roll ();
//				}
				Refresh ();
			}
		}
		notMoved = true; possibleTokenCirculation = false;
		ReleaseMouse ();
	}
	void OnMotion (wxMouseEvent & event) {
		if (! event . Dragging ()) return;
		notMoved = false;
		wxPoint p = event . GetPosition ();
		wxPoint delta = p - capturedPosition;
		if (dragToken == 0) {
			if (moveTokens && tokens != 0) tokens -> moveAll (delta);
//			if (moveBoard) boardLocation += delta;
//			if (moveGrid) gridLocation += delta;
		} else dragToken -> move (delta);
		capturedPosition = p;
		modified = true;
		Refresh ();
	}
	void OnRightDown (wxMouseEvent & event) {
		lastRightClickPosition = event . GetPosition ();
		bool onToken = dragToken == 0 ? false : dragToken == dragToken -> hitFind (lastRightClickPosition, unlocked);
		wxMenu menu;
		if (! onToken) menu . Append (4001, _T ("New token"));
		if (! onToken) menu . Append (4002, _T ("New grid"));
		if (! onToken) menu . Append (4201, _T ("New dice"));
		if (! onToken) menu . Append (4202, _T ("New tetrahedron"));
		if (! onToken) menu . Append (4203, _T ("New cube"));
		if (! onToken) menu . Append (4204, _T ("New octahedron"));
		if (! onToken) menu . Append (4205, _T ("New deltahedron"));
		if (! onToken) menu . Append (4206, _T ("New deltahedron 10"));
		if (! onToken) menu . Append (4207, _T ("New dodecahedron"));
		if (! onToken) menu . Append (4208, _T ("New icosahedron"));
		if (onToken) menu . Append (4101, _T ("Rotate right"));
		if (onToken) menu . Append (4102, _T ("Rotate left"));
		if (onToken) menu . Append (4103, _T ("Delete token"));
		PopupMenu (& menu, lastRightClickPosition);
	}
	void OnNewToken (wxCommandEvent & event) {
		wxFileDialog picker (this);
		picker . SetWildcard (_T ("PNG pictures (*.png)|*.png"));
		if (picker . ShowModal () == wxID_OK) {
			wxString file_name = picker . GetDirectory () + _T ("/") + picker . GetFilename ();
			file_name . Replace (_T ("\\"), _T ("/"));
			dragToken = tokens = new BoardToken (file_name, lastRightClickPosition, true, tokens);
			modified = true;
			Refresh ();
		}
	}
	void insertNewToken (wxPoint location, wxString file_name) {
		dragToken = tokens = new BoardToken (file_name, location, true, tokens);
		modified = true;
		Refresh ();
	}
	void OnNewGrid (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, tokens);
		modified = true;
		Refresh ();
	}
	void OnNewDice (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 6, 60, dicePenColour, diceBrushColour, true, true, tokens);
		dragToken -> gridIndexing = false;
		modified = true;
		Refresh ();
	}
	void OnNewTetrahedron (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 4, 60, tetrahedronPenColour, tetrahedronBrushColour, true, true, tokens);
		modified = true;
		Refresh ();
	}
	void OnNewCube (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 6, 60, cubePenColour, cubeBrushColour, true, true, tokens);
		modified = true;
		Refresh ();
	}
	void OnNewOctahedron (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 8, 60, octahedronPenColour, octahedronBrushColour, true, true, tokens);
		modified = true;
		Refresh ();
	}
	void OnNewDeltahedron (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 10, 60, deltahedronPenColour, deltahedronBrushColour, true, true, tokens);
		dragToken -> diceShift = 0;
		dragToken -> roll ();
		modified = true;
		Refresh ();
	}
	void OnNewDeltahedron10 (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 10, 60, deltahedron10PenColour, deltahedron10BrushColour, true, true, tokens);
		dragToken -> diceShift = 0;
		dragToken -> diceMultiplier = 10;
		dragToken -> roll ();
		modified = true;
		Refresh ();
	}
	void OnNewDodecahedron (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 12, 60, dodecahedronPenColour, dodecahedronBrushColour, true, true, tokens);
		modified = true;
		Refresh ();
	}
	void OnNewIcosahedron (wxCommandEvent & event) {
		dragToken = tokens = new BoardToken (lastRightClickPosition, 20, 60, icosahedronPenColour, icosahedronBrushColour, true, true, tokens);
		modified = true;
		Refresh ();
	}
	void rotateRight (void) {
		if (dragToken != 0) dragToken -> rotateRight ();
		modified = true;
		Refresh ();
	}
	void OnRotateRight (wxCommandEvent & event) {rotateRight ();}
	void rotateLeft (void) {
		if (dragToken != 0) dragToken -> rotateLeft ();
		modified = true;
		Refresh ();
	}
	void OnRotateLeft (wxCommandEvent & event) {rotateLeft ();}
	void changeGridSide (int change) {
		if (dragToken == 0) return;
		dragToken -> changeGridSide (change);
		modified = true;
		Refresh ();
	}
	void setIndexedGrid (void) {
		if (dragToken == 0) return;
		dragToken -> setIndexedGrid ();
		modified = true;
		Refresh ();
	}
	void changeGridIndexing (wxPoint change) {
		if (dragToken == 0) return;
		dragToken -> changeGridIndexing (change);
		modified = true;
		Refresh ();
	}
	void changeRows (wxSize change) {
		if (dragToken == 0) return;
		dragToken -> changeRows (change);
		modified = true;
		Refresh ();
	}
	BoardToken * getNextSelectableToken (BoardToken * bp, bool unlocked) {
		if (unlocked) return bp;
		while (bp != 0 && ! bp -> isSelectable) bp = bp -> next;
		return bp;
	}
	void TabForward (bool unlocked) {
		if (dragToken == 0) {dragToken = getNextSelectableToken (tokens, unlocked); Refresh (); return;}
		dragToken = getNextSelectableToken (dragToken -> next, unlocked);
		if (dragToken == 0) dragToken = getNextSelectableToken (tokens, unlocked);
		Refresh ();
	}
	void TabBackward (bool unlocked) {
		if (dragToken == 0) {dragToken = getNextSelectableToken (tokens, unlocked); Refresh (); return;}
		if (dragToken == tokens) {
			while (getNextSelectableToken (dragToken -> next, unlocked) != 0) dragToken = getNextSelectableToken (dragToken -> next, unlocked);
		} else {
			BoardToken * bp = getNextSelectableToken (tokens, unlocked);
			while (bp != 0 && bp -> next != 0 && getNextSelectableToken (bp -> next, unlocked) != dragToken) {
				bp = getNextSelectableToken (bp -> next, unlocked);
			}
			dragToken = bp;
			if (dragToken == 0) dragToken = getNextSelectableToken (tokens, unlocked);
		}
		Refresh ();
	}
	void TokenToVeryFront (void) {
		if (tokens == 0) return;
		if (dragToken == 0) return;
		if (dragToken -> next == 0) return;
		if (dragToken == tokens) {
			tokens = dragToken -> next;
			dragToken -> next = 0;
			BoardToken * bp = tokens;
			while (bp -> next != 0) bp = bp -> next;
			bp -> next = dragToken;
			modified = true;
			Refresh ();
			return;
		}
		BoardToken * bp = tokens;
		while (bp -> next != dragToken) bp = bp -> next;
		bp -> next = dragToken -> next;
		dragToken -> next = 0;

		while (bp -> next != 0) bp = bp -> next;
		bp -> next = dragToken;
		modified = true;
		Refresh ();
		return;
	}
	void TokenToVeryBack (void) {
		if (tokens == 0) return;
		if (dragToken == 0) return;
		if (dragToken == tokens) return;
		BoardToken * bp = tokens;
		while (bp -> next != dragToken) bp = bp -> next;
		bp -> next = dragToken -> next;
		dragToken -> next = tokens;
		tokens = dragToken;
		modified = true;
		Refresh ();
		return;
	}
	void TokenToFront (void) {
		if (tokens == 0) return;
		if (dragToken == 0) return;
		if (dragToken -> next == 0) return;
		if (dragToken == tokens) {
			BoardToken * bp = tokens -> next;
			tokens -> next = tokens -> next -> next;
			bp -> next = tokens;
			tokens = bp;
			modified = true;
			Refresh ();
			return;
		}
		BoardToken * bp = tokens;
		while (bp -> next != dragToken) bp = bp -> next;
		bp -> next = dragToken -> next;
		bp = bp -> next;
		dragToken -> next = dragToken -> next -> next;
		bp -> next = dragToken;
		modified = true;
		Refresh ();
	}
	void TokenToBack (void) {
		if (tokens == 0) return;
		if (dragToken == 0) return;
		if (dragToken == tokens) return;
		if (dragToken == tokens -> next) {
			BoardToken * bp = tokens;
			tokens = tokens -> next;
			bp -> next = tokens -> next;
			tokens -> next = bp;
			modified = true;
			Refresh ();
			return;
		}
		BoardToken * bp = tokens;
		while (bp -> next -> next != dragToken) bp = bp -> next;
		bp -> next -> next = dragToken -> next;
		dragToken -> next = bp -> next;
		bp -> next = dragToken;
		modified = true;
		Refresh ();
		return;
	}
	void deleteToken (void) {
		if (tokens == 0) return;
		if (dragToken == 0) return;
		if (dragToken == tokens) {
			tokens = dragToken -> next;
			dragToken -> next = 0;
			delete dragToken;
			dragToken = 0;
			modified = true;
			Refresh ();
			return;
		}
		BoardToken * bp = tokens;
		while (bp != 0 && bp -> next != dragToken) bp = bp -> next;
		bp -> next = dragToken -> next;
		dragToken -> next = 0;
		delete dragToken;
		dragToken = 0;
		modified = true;
		Refresh ();
		return;
	}
	void OnDeleteToken (wxCommandEvent & event) {deleteToken ();}
	void OnIdle (wxIdleEvent & event) {if (idleRepaint) Refresh (); idleRepaint = false;}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(BoardWindow, wxWindow)
EVT_PAINT(BoardWindow :: OnPaint)
EVT_LEFT_DOWN(BoardWindow :: OnLeftDown)
EVT_LEFT_UP(BoardWindow :: OnLeftUp)
EVT_MOTION(BoardWindow :: OnMotion)
EVT_RIGHT_DOWN(BoardWindow :: OnRightDown)
EVT_MENU(4001, BoardWindow :: OnNewToken)
EVT_MENU(4002, BoardWindow :: OnNewGrid)
EVT_MENU(4101, BoardWindow :: OnRotateRight)
EVT_MENU(4102, BoardWindow :: OnRotateLeft)
EVT_MENU(4103, BoardWindow :: OnDeleteToken)
EVT_MENU(4201, BoardWindow :: OnNewDice)
EVT_MENU(4202, BoardWindow :: OnNewTetrahedron)
EVT_MENU(4203, BoardWindow :: OnNewCube)
EVT_MENU(4204, BoardWindow :: OnNewOctahedron)
EVT_MENU(4205, BoardWindow :: OnNewDeltahedron)
EVT_MENU(4206, BoardWindow :: OnNewDeltahedron10)
EVT_MENU(4207, BoardWindow :: OnNewDodecahedron)
EVT_MENU(4208, BoardWindow :: OnNewIcosahedron)
EVT_IDLE(BoardWindow :: OnIdle)
END_EVENT_TABLE()

class BoardFrame : public wxFrame {
public:
	int gridControlType;
	BoardWindow * board;
	wxString file_name;
	BoardFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("TABLE TOP"), BOARD_POSITION, BOARD_SIZE) {
		file_name = _T ("");
		gridControlType = 5;
		board = new BoardWindow (this, -1);
		board -> SetDropTarget (new FileReceiver (this));
		board -> SetFocus ();

		wxMenuBar * bar = new wxMenuBar ();

		wxMenu * file_menu = new wxMenu ();
		file_menu -> Append (6102, _T ("Load	L"));
		file_menu -> Append (6105, _T ("Reload	E"));
		file_menu -> Append (6103, _T ("Save	S"));
		file_menu -> Append (6104, _T ("Save As	A"));
		file_menu -> AppendSeparator ();
		file_menu -> Append (6121, _T ("New token	N"));
		file_menu -> Append (6122, _T ("New grid	M"));
		file_menu -> Append (6123, _T ("Delete token / grid	R"));
		file_menu -> AppendSeparator ();
		file_menu -> Append (6101, _T ("EXIT	Q"));
		bar -> Append (file_menu, _T ("File"));

		wxMenu * control_menu = new wxMenu ();
		control_menu -> AppendRadioItem (6005, _T ("Toker ordering	F4"));
		control_menu -> AppendRadioItem (6004, _T ("Rotation	F5"));
		control_menu -> AppendRadioItem (6001, _T ("Grid size	F6"));
		control_menu -> AppendRadioItem (6002, _T ("Grid indexing	F7"));
		control_menu -> AppendRadioItem (6003, _T ("Grid cell size	F8"));
		control_menu -> AppendSeparator ();
		control_menu -> Append (6205, _T ("Change indexing	I"));
		control_menu -> Append (6206, _T ("Roll dice	D"));
		bar -> Append (control_menu, _T ("Control"));

		wxMenu * lock_menu = new wxMenu ();
		lock_menu -> AppendCheckItem (6701, _T ("Unlocked	F2"));
		lock_menu -> Append (6703, _T ("Lock / unlock token	T"));
		bar -> Append (lock_menu, _T ("Locking"));

		wxMenu * colour_menu = new wxMenu ();
		colour_menu -> Append (6601, _T ("Token brush colour	B"));
		colour_menu -> Append (6602, _T ("Token pen colour	G"));
		bar -> Append (colour_menu, _T ("Colours"));

		SetMenuBar (bar);

//		bar -> Check (6205, board -> indexedGrid);
		bar -> Enable (6103, false);
		bar -> Enable (6105, false);
		bar -> Check (6005, true);
		bar -> Check (6701, board -> unlocked);
		
	}
	void OnBrushColour (wxCommandEvent & event) {
		if (board == 0) return;
		wxColourDialog picker (this);
		if (picker . ShowModal () == wxID_OK) {
			if (board -> dragToken != 0) {
				board -> dragToken -> backgroundColour = picker . GetColourData () . GetColour ();
				if (board -> dragToken -> tokenType == BoardToken :: DiceToken) {
				switch (board -> dragToken -> choosenRotation) {
				case 4: board -> tetrahedronBrushColour = board -> dragToken -> gridColour; break;
				case 6: if (board -> dragToken -> gridIndexing) board -> cubeBrushColour = board -> dragToken -> gridColour; else board -> diceBrushColour = board -> dragToken -> gridColour; break;
				case 8: board -> octahedronBrushColour = board -> dragToken -> gridColour; break;
				case 10: if (board -> dragToken -> diceMultiplier == 1) board -> deltahedronBrushColour = board -> dragToken -> gridColour; else board -> deltahedron10BrushColour = board -> dragToken -> gridColour; break;
				case 12: board -> dodecahedronBrushColour = board -> dragToken -> gridColour; break;
				case 20: board -> icosahedronBrushColour = board -> dragToken -> gridColour; break;
				default: break;
				}
				}
			} else board -> backgroundColour = picker . GetColourData () . GetColour ();
			board -> modified = true;
			Refresh ();
		}
	}
	void OnPenColour (wxCommandEvent & event) {
		if (board == 0) return;
		if (board -> dragToken == 0) return;
		if (board -> dragToken -> tokenType == BoardToken :: PictureToken) return;
		wxColourDialog picker (this);
		if (picker . ShowModal () == wxID_OK) {
			board -> dragToken -> gridColour = picker . GetColourData () . GetColour ();
			if (board -> dragToken -> tokenType == BoardToken :: DiceToken) {
				switch (board -> dragToken -> choosenRotation) {
				case 4: board -> tetrahedronPenColour = board -> dragToken -> gridColour; break;
				case 6: if (board -> dragToken -> gridIndexing) board -> cubePenColour = board -> dragToken -> gridColour; else board -> dicePenColour = board -> dragToken -> gridColour; break;
				case 8: board -> octahedronPenColour = board -> dragToken -> gridColour; break;
				case 10: if (board -> dragToken -> diceMultiplier == 1) board -> deltahedronPenColour = board -> dragToken -> gridColour; else board -> deltahedron10PenColour = board -> dragToken -> gridColour; break;
				case 12: board -> dodecahedronPenColour = board -> dragToken -> gridColour; break;
				case 20: board -> icosahedronPenColour = board -> dragToken -> gridColour; break;
				default: break;
				}
			}
			board -> modified = true;
			Refresh ();
		}
	}
	void LoadGrid (char * file_name) {
		if (board == 0) return;
		SetupFileReader fr (file_name);
		if (fr . file_not_found ()) return;
		if (! fr . get_id ("board")) return;
		if (board -> tokens != 0) delete board -> tokens; board -> dragToken = board -> tokens = 0;
		board -> modified = false;
		while (fr . get_id ()) {
			bool selectable = false;
			wxPoint position (0, 0);
			if (fr . id ("background")) {
				if (! fr . get_int ()) return; int red = fr . int_symbol;
				if (! fr . get_int ()) return; int green = fr . int_symbol;
				if (! fr . get_int ()) return; int blue = fr . int_symbol;
				board -> backgroundColour = wxColour (red, green, blue);
				fr . skip ();
			}
			if (fr . id ("defaults")) {
				while (fr . get_id ()) {
					if (fr . id ("dice")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> dicePenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> diceBrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
					if (fr . id ("tetrahedron")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> tetrahedronPenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> tetrahedronBrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
					if (fr . id ("cube")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> cubePenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> cubeBrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
					if (fr . id ("octahedron")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> octahedronPenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> octahedronBrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
					if (fr . id ("deltahedron")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> deltahedronPenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> deltahedronBrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
					if (fr . id ("deltahedron10")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> deltahedron10PenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> deltahedron10BrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
					if (fr . id ("dodecahedron")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> dodecahedronPenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> dodecahedronBrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
					if (fr . id ("icosahedron")) {
						while (fr . get_id ()) {
							if (fr . id ("colour")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> icosahedronPenColour = wxColour (red, green, blue);
							}
							if (fr . id ("background")) {
								if (! fr . get_int ()) return; int red = fr . int_symbol;
								if (! fr . get_int ()) return; int green = fr . int_symbol;
								if (! fr . get_int ()) return; int blue = fr . int_symbol;
								board -> icosahedronBrushColour = wxColour (red, green, blue);
							}
							fr . skip ();
						}
					}
				}
			}
			if (fr . id ("grid")) {
				int type = 0;
				int side = 60;
				wxSize size (4, 4);
				wxPoint index (0, 0);
				bool indexed = false;
				int red = 255, green = 255, blue = 255;
				while (fr . get_id ()) {
					if (fr . id ("type")) {if (! fr . get_int ()) return; type = fr . int_symbol;}
					if (fr . id ("side")) {if (! fr . get_int ()) return; side = fr . int_symbol;}
					if (fr . id ("size")) {
						if (! fr . get_int ()) return; size . x = fr . int_symbol;
						if (! fr . get_int ()) return; size . y = fr . int_symbol;
					}
					if (fr . id ("index")) {
						if (! fr . get_int ()) return; index . x = fr . int_symbol;
						if (! fr . get_int ()) return; index . y = fr . int_symbol;
					}
					if (fr . id ("colour")) {
						if (! fr . get_int ()) return; red = fr . int_symbol;
						if (! fr . get_int ()) return; green = fr . int_symbol;
						if (! fr . get_int ()) return; blue = fr . int_symbol;
					}
					if (fr . id ("indexed")) indexed = true;
					if (fr . id ("position")) {
						if (! fr . get_int ()) return; position . x = fr . int_symbol;
						if (! fr . get_int ()) return; position . y = fr . int_symbol;
					}
					if (fr . id ("selectable")) selectable = true;
					fr . skip ();
				}
				board -> tokens = new BoardToken (position, type, side, size, index, indexed, selectable, wxColour (red, green, blue), board -> tokens);
			}
			if (fr . id ("dice")) {
				int type = 0;
				int side = 60;
				int shift = 1;
				int multiplier = 1;
				int value = 1;
				int fred = 255, fgreen = 255, fblue = 255;
				int bred = 0, bgreen = 0, bblue = 0;
				bool indexed = false;
				while (fr . get_id ()) {
					if (fr . id ("type")) {if (! fr . get_int ()) return; type = fr . int_symbol;}
					if (fr . id ("side")) {if (! fr . get_int ()) return; side = fr . int_symbol;}
					if (fr . id ("shift")) {if (! fr . get_int ()) return; shift = fr . int_symbol;}
					if (fr . id ("multiplier")) {if (! fr . get_int ()) return; multiplier = fr . int_symbol;}
					if (fr . id ("value")) {if (! fr . get_int ()) return; value = fr . int_symbol;}
					if (fr . id ("colour")) {
						if (! fr . get_int ()) return; fred = fr . int_symbol;
						if (! fr . get_int ()) return; fgreen = fr . int_symbol;
						if (! fr . get_int ()) return; fblue = fr . int_symbol;
					}
					if (fr . id ("background")) {
						if (! fr . get_int ()) return; bred = fr . int_symbol;
						if (! fr . get_int ()) return; bgreen = fr . int_symbol;
						if (! fr . get_int ()) return; bblue = fr . int_symbol;
					}
					if (fr . id ("indexed")) indexed = true;
					if (fr . id ("position")) {
						if (! fr . get_int ()) return; position . x = fr . int_symbol;
						if (! fr . get_int ()) return; position . y = fr . int_symbol;
					}
					if (fr . id ("selectable")) selectable = true;
					fr . skip ();
				}
				board -> tokens = new BoardToken (position, type, side, wxColour (fred, fgreen, fblue), wxColour (bred, bgreen, bblue), false, selectable, board -> tokens);
				board -> tokens -> diceShift = shift;
				board -> tokens -> diceMultiplier = multiplier;
				board -> tokens -> gridIndexing = indexed;
				board -> tokens -> diceValue = value;
			}
			if (fr . id ("token")) {
				char command [1024];
				int rotation = 0;
				while (fr . get_id ()) {
					if (fr . id ("location")) {
						if (! fr . get_string ()) return; strcpy (command, fr . symbol);
					}
					if (fr . id ("position")) {
						if (! fr . get_int ()) return; position . x = fr . int_symbol;
						if (! fr . get_int ()) return; position . y = fr . int_symbol;
					}
					if (fr . id ("rotation")) {
						if (! fr . get_int ()) return; rotation = fr . int_symbol;
					}
					if (fr . id ("selectable")) selectable = true;
					fr . skip ();
				}
				board -> tokens = new BoardToken (wxString :: From8BitData (command), position, false, board -> tokens);
				board -> tokens -> rotate (rotation);
				board -> tokens -> isSelectable = selectable;
			}
		}
		this -> file_name = wxString :: From8BitData (file_name); //Format (_T ("%s"), file_name);
		wxMenuBar * bar = GetMenuBar ();
		if (bar != 0) {bar -> Enable (6103, true); bar -> Enable (6105, true);}
	}
	void LoadGrid (void) {
		char command [1024];
		toCommand (command, file_name);
		LoadGrid (command);
		Refresh ();
	}
	void OnLoad (wxCommandEvent & event) {
		wxFileDialog picker (this);
		picker . SetWildcard (_T ("Grid files (*.grid)|*.grid"));
		if (picker . ShowModal () == wxID_OK) {
			file_name = picker . GetDirectory () + _T ( "/") + picker . GetFilename ();
			file_name . Replace (_T ("\\"), _T ("/"));
			LoadGrid ();
		}
	}
	void OnReload (wxCommandEvent & event) {LoadGrid ();}
	void SaveGrid (char * file_name) {
		FILE * fw = fopen (file_name, "wt");
		if (fw == 0) return;
		fprintf (fw, "board [\n");
		if (board != 0) {
			fprintf (fw, "	background [%i %i %i]\n", board -> backgroundColour . Red (), board -> backgroundColour . Green (), board -> backgroundColour . Blue ());
			fprintf (fw, "	defaults [\n");
			fprintf (fw, "		dice [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> dicePenColour . Red (), board -> dicePenColour . Green (), board -> dicePenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> diceBrushColour . Red (), board -> diceBrushColour . Green (), board -> diceBrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "		tetrahedron [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> tetrahedronPenColour . Red (), board -> tetrahedronPenColour . Green (), board -> tetrahedronPenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> tetrahedronBrushColour . Red (), board -> tetrahedronBrushColour . Green (), board -> tetrahedronBrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "		cube [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> cubePenColour . Red (), board -> cubePenColour . Green (), board -> cubePenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> cubeBrushColour . Red (), board -> cubeBrushColour . Green (), board -> cubeBrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "		octahedron [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> octahedronPenColour . Red (), board -> octahedronPenColour . Green (), board -> octahedronPenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> octahedronBrushColour . Red (), board -> octahedronBrushColour . Green (), board -> octahedronBrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "		deltahedron [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> deltahedronPenColour . Red (), board -> deltahedronPenColour . Green (), board -> deltahedronPenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> deltahedronBrushColour . Red (), board -> deltahedronBrushColour . Green (), board -> deltahedronBrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "		deltahedron10 [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> deltahedron10PenColour . Red (), board -> deltahedron10PenColour . Green (), board -> deltahedron10PenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> deltahedron10BrushColour . Red (), board -> deltahedron10BrushColour . Green (), board -> deltahedron10BrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "		dodecahedron [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> dodecahedronPenColour . Red (), board -> dodecahedronPenColour . Green (), board -> dodecahedronPenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> dodecahedronBrushColour . Red (), board -> dodecahedronBrushColour . Green (), board -> dodecahedronBrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "		icosahedron [\n");
			fprintf (fw, "			colour [%i %i %i]\n", board -> icosahedronPenColour . Red (), board -> icosahedronPenColour . Green (), board -> icosahedronPenColour . Blue ());
			fprintf (fw, "			background [%i %i %i]\n", board -> icosahedronBrushColour . Red (), board -> icosahedronBrushColour . Green (), board -> icosahedronBrushColour . Blue ());
			fprintf (fw, "		]\n");
			fprintf (fw, "	]\n");
			board -> SaveTokens (fw);
			board -> modified = false;
		}
		fprintf (fw, "]\n");
		fclose (fw);
		wxMenuBar * bar = GetMenuBar ();
		if (bar != 0) {bar -> Enable (6103, true); bar -> Enable (6105, true);}
	}
	void SaveGrid (void) {
		char command [1024];
		toCommand (command, file_name);
		SaveGrid (command);
		Refresh ();
	}
	void OnSave (wxCommandEvent & event) {
		if (file_name . length () > 6) {
			if (wxYES == wxMessageBox (_T ("SAVE ?"), _T ("INFO"), wxYES_NO | wxYES_DEFAULT | wxICON_EXCLAMATION, this)) SaveGrid ();
		}
	}
	void SaveAs (void) {
		wxFileDialog picker (this);
		picker . SetWindowStyle (picker . GetWindowStyle () | wxFD_SAVE);
		picker . SetWildcard (_T ("Grid files (*.grid)|*.grid"));
		if (picker . ShowModal () == wxID_OK) {
			file_name = picker . GetDirectory () + _T ("/") + picker . GetFilename ();
			file_name . Replace (_T ("\\"), _T ("/"));
			SaveGrid ();
		}
	}
	void OnSaveAs (wxCommandEvent & event) {SaveAs ();}
	void OnRotateRight (wxCommandEvent & event) {board -> OnRotateRight (event);}
	void OnRotateLeft (wxCommandEvent & event) {board -> OnRotateLeft (event);}
//	void OnNewBoard (wxCommandEvent & event) {board -> OnNewBoard (event);}
	void OnQuit (wxCommandEvent & event) {OnEscape ();}
	//void OnEscape (void) {if (wxYES == wxMessageBox (_T ("EXIT?"), _T ("INFO"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION, this)) delete this;}
	void OnEscape (void) {delete this;}
	void OnArrow (int key) {
		if (board == 0) return;
		switch (gridControlType) {
		case 1:
			switch (key) {
			case WXK_LEFT: board -> changeRows (wxSize (-1, 0)); break;
			case WXK_RIGHT: board -> changeRows (wxSize (1, 0)); break;
			case WXK_UP: board -> changeRows (wxSize (0, -1)); break;
			case WXK_DOWN: board -> changeRows (wxSize (0, 1)); break;
			default: break;
			}
			break;
		case 2:
			switch (key) {
			case WXK_LEFT: board -> changeGridIndexing (wxPoint (-1, 0)); break;
			case WXK_RIGHT: board -> changeGridIndexing (wxPoint (1, 0)); break;
			case WXK_UP: board -> changeGridIndexing (wxPoint (0, -1)); break;
			case WXK_DOWN: board -> changeGridIndexing (wxPoint (0, 1)); break;
			default: break;
			}
			break;
		case 3:
			switch (key) {
			case WXK_LEFT: board -> changeGridSide (-1); break;
			case WXK_RIGHT: board -> changeGridSide (1); break;
			case WXK_UP: board -> changeGridSide (6); break;
			case WXK_DOWN: board -> changeGridSide (-6); break;
			default: break;
			}
			break;
		case 4:
			switch (key) {
			case WXK_LEFT: case WXK_DOWN: board -> rotateLeft (); break;
			case WXK_RIGHT: case WXK_UP: board -> rotateRight (); break;
			default: break;
			}
			break;
		case 5:
			switch (key) {
			case WXK_LEFT: board -> TokenToFront (); break;
			case WXK_RIGHT: board -> TokenToBack (); break;
			case WXK_DOWN: board -> TokenToVeryFront (); break;
			case WXK_UP: board -> TokenToVeryBack (); break;
			default: break;
			}
		default: break;
		}
	}
	void OnGridSizeControl (wxCommandEvent & event) {gridControlType = 1;}
	void OnGridIndexControl (wxCommandEvent & event) {gridControlType = 2;}
	void OnGridCellSizeControl (wxCommandEvent & event) {gridControlType = 3;}
	void OnRotateControl (wxCommandEvent & event) {gridControlType = 4;}
	void OnTokenOrdering (wxCommandEvent & event) {gridControlType = 5;}
	void OnIndexed (wxCommandEvent & event) {
		if (board == 0) return;
		wxMenuBar * bar = GetMenuBar ();
		if (bar == 0) return;
		board -> setIndexedGrid ();
	}
	void RollDice (void) {
		if (board == 0) return;
		board -> RollDice ();
	}
	void RollAllDices (void) {
		if (board == 0) return;
		board -> RollAllDices ();
	}
	void OnRollDice (wxCommandEvent & event) {RollDice ();}
	void insertNewToken (wxPoint location, wxString file_name) {
		if (board == 0) return;
		file_name . Replace (_T ("\\"), _T ("/"));
		board -> insertNewToken (location, file_name);
	}
	void TabForward (void) {if (board != 0) board -> TabForward (board -> unlocked);}
	void TabBackward (void) {if (board != 0) board -> TabBackward (board -> unlocked);}
	void OnLock (wxCommandEvent & event) {
		if (board == 0) return;
		wxMenuBar * bar = GetMenuBar ();
		if (bar == 0) return;
		board -> unlocked = bar -> IsChecked (6701);
		board -> dragToken = 0;
		Refresh ();
	}
	void OnLockToken (wxCommandEvent & event) {
		if (board == 0) return;
		if (board -> dragToken == 0) return;
		board -> dragToken -> isSelectable = ! board -> dragToken -> isSelectable;
		board -> dragToken = 0;
		board -> modified = true;
		Refresh ();
	}
	void OnNewToken (wxCommandEvent & event) {if (board != 0) board -> OnNewToken (event);}
	void OnNewGrid (wxCommandEvent & event) {if (board != 0) board -> OnNewGrid (event);}
	void OnDeleteToken (wxCommandEvent & event) {if (board != 0) board -> OnDeleteToken (event);}
	~ BoardFrame (void) {
		stop_threads = true;
		if (board != 0) {
			if (board -> modified) {
				if (wxYES == wxMessageBox (_T ("SAVE CHANGES ?"), _T ("INFO"), wxYES_NO | wxYES_DEFAULT | wxICON_EXCLAMATION, this)) {
					if (file_name . length () > 6) SaveGrid (); else SaveAs ();
				}
			}
		}
	}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(BoardFrame, wxFrame)
EVT_MENU(6001, BoardFrame :: OnGridSizeControl)
EVT_MENU(6002, BoardFrame :: OnGridIndexControl)
EVT_MENU(6003, BoardFrame :: OnGridCellSizeControl)
EVT_MENU(6004, BoardFrame :: OnRotateControl)
EVT_MENU(6005, BoardFrame :: OnTokenOrdering)
EVT_MENU(6101, BoardFrame :: OnQuit)
//EVT_MENU(6201, BoardFrame :: OnNoGrid)
//EVT_MENU(6202, BoardFrame :: OnSquareGrid)
//EVT_MENU(6203, BoardFrame :: OnVerticalHexGrid)
//EVT_MENU(6204, BoardFrame :: OnHorizontalHexGrid)
EVT_MENU(6205, BoardFrame :: OnIndexed)
EVT_MENU(6206, BoardFrame :: OnRollDice)
//EVT_MENU(5001, BoardFrame :: OnArrow)
//EVT_MENU(5002, BoardFrame :: OnArrow)
//EVT_MENU(5003, BoardFrame :: OnArrow)
//EVT_MENU(5004, BoardFrame :: OnArrow)
EVT_MENU(6102, BoardFrame :: OnLoad)
EVT_MENU(6103, BoardFrame :: OnSave)
EVT_MENU(6104, BoardFrame :: OnSaveAs)
EVT_MENU(6105, BoardFrame :: OnReload)
EVT_MENU(6601, BoardFrame :: OnBrushColour)
EVT_MENU(6602, BoardFrame :: OnPenColour)
EVT_MENU(6701, BoardFrame :: OnLock)
EVT_MENU(6703, BoardFrame :: OnLockToken)
EVT_MENU(6121, BoardFrame :: OnNewToken)
EVT_MENU(6122, BoardFrame :: OnNewGrid)
EVT_MENU(6123, BoardFrame :: OnDeleteToken)
END_EVENT_TABLE()

BoardFrame * boardFrame = 0;

FileReceiver :: FileReceiver (BoardFrame * frame) {this -> frame = frame;}

bool FileReceiver :: OnDropFiles (wxCoord x, wxCoord y, const wxArrayString & files) {
	if (frame == NULL) return true;
	if ((int) files . GetCount () < 1) return true;
	for (int ind = 0; ind < (int) files . GetCount (); ind++) {
		if (files [ind] . Lower () . Find (_T (".png")) >= 0) frame -> insertNewToken (wxPoint (x, y) + wxPoint (20 *ind, 20 * ind), files [ind]);
		if (files [ind] . Lower () . Find (_T (".grid")) >= 0) {frame -> file_name = files [ind]; frame -> LoadGrid ();}
	}
	return true;
}

class BoardClass : public wxApp {
public:
	int previous_key_down;
	bool shiftDown;
	bool OnInit (void) {
		srand (time (0));
		shiftDown = false;
		previous_key_down = -1;
		wxInitAllImageHandlers ();
		(boardFrame = new BoardFrame (0)) -> Show ();
		if (argc > 1) {
			char command [1024];
			wxChar * cp = argv [1];
			int ind = 0;
			while (* cp != '\0') {command [ind++] = (char) * cp++;} command [ind++] = '\0';
			boardFrame -> LoadGrid (command);
		}
		return true;
	}
	int OnExit (void) {
		return wxApp :: OnExit ();
	}
	int FilterEvent (wxEvent & event) {
		if (boardFrame == 0) return -1;
		int key = ((wxKeyEvent &) event) . GetKeyCode ();
		if (event . GetEventType () == wxEVT_KEY_UP) {
			if (key == WXK_SHIFT) shiftDown = false;
			return -1;
		}
		if (event . GetEventType () != wxEVT_KEY_DOWN) return -1;
		if (key == previous_key_down) {previous_key_down = -1; return -1;}
		previous_key_down = key;
		switch (key) {
		case WXK_SHIFT: shiftDown = true; break;
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_UP:
		case WXK_DOWN: boardFrame -> OnArrow (key); break;
		case WXK_TAB: if (shiftDown) boardFrame -> TabBackward (); else boardFrame -> TabForward (); break;
		case WXK_DELETE: case WXK_BACK: boardFrame -> board -> deleteToken (); break;
		case WXK_RETURN: boardFrame -> RollDice (); break;
		case WXK_SPACE: boardFrame -> RollAllDices (); break;
		default: break;
		}
		return -1;
	}
};

IMPLEMENT_APP(BoardClass);

