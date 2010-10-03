
#include "wx/wx.h"
#include "wx/dcbuffer.h"


#include "prolog.h"
#include "prolog_studio.h"

#include <string.h>

#include "neural.h"

extern char resource_0 [];
extern char resource_1 [];
extern char resource_2 [];
extern char resource_3 [];
extern char resource_4 [];
extern char resource_5 [];
class resource_loader_class : public PrologResourceLoader {
public:
	char * load (char * name) {
		char * ret = NULL;
		if (strcmp (name, "studio.prc") == 0) ret = resource_0;
		if (strcmp (name, "store.prc") == 0) ret = resource_1;
		if (strcmp (name, "f1.prc") == 0) ret = resource_2;
		if (strcmp (name, "help.prc") == 0) ret = resource_3;
		if (strcmp (name, "record.prc") == 0) ret = resource_4;
		if (strcmp (name, "neural.prc") == 0) ret = resource_5;
		return ret;
	}
} resource_loader;

extern PrologServiceClass * galaxy_return (void);

class service_class_loader_class : public PrologServiceClassLoader {
public:
	PrologServiceClass * load (char * name) {
		if (strcmp (name, "neural") == 0) return new neural_service ();
		if (strcmp (name, "galaxy") == 0) return galaxy_return ();
		return NULL;
	}
} service_class_loader;

static PrologRoot * root = NULL;

class PrologThread : public wxThread {
public:
	PrologThread (void) {this -> Create (65536); this -> Run ();}
	~ PrologThread (void) {}
	virtual ExitCode Entry (void) {
		root = new PrologRoot ();
		root -> setResourceLoader (& resource_loader);
		root -> setServiceClassLoader (& service_class_loader);
		root -> set_uap32_captions ();
		PrologLinuxConsole * console = new PrologLinuxConsole (10);
		console -> open ();
		root -> insertCommander (console);
		root -> resolution ("warpwar.prc");
		root = NULL;
		console -> stop ();
		Sleep (1000);
		Exit ();
		return Wait ();
	}
};

class WarpwarWindow : public wxWindow {
public:
	wxBitmap * galaxy_bitmap;
	wxMemoryDC * galaxy_dc;
	int galaxy_width, galaxy_height;
	int cell_side;
	int zero_x, zero_y;
	int bitmap_position_x, bitmap_position_y;
	int captured_x, captured_y;
	bool orientation;
	void setStarColour (int r, int g, int b) {
		galaxy_dc -> SetPen (wxPen (wxColour (r, g, b)));
		galaxy_dc -> SetBrush (wxBrush (wxColour (r, g, b)));
	}
	void deleteGalaxy (void) {
		if (galaxy_dc != NULL) delete galaxy_dc; galaxy_dc = NULL;
		if (galaxy_bitmap != NULL) delete galaxy_bitmap; galaxy_bitmap = NULL;
	}
	void reSizeGalaxy (int x, int y) {
		deleteGalaxy ();
		galaxy_width = x; galaxy_height = y;
		galaxy_bitmap = new wxBitmap (x, y, 32);
		galaxy_dc = new wxMemoryDC (* galaxy_bitmap);
	}
	void setZero (double x, double y) {
		zero_x = (int) ((double) cell_side + (double) cell_side * x * 1.5);
		zero_y = (int) ((double) cell_side * 0.866025404 + (double) cell_side * y * 0.866025404 * 2.0);
	}
	void setCellSide (int side) {this -> cell_side = side; setZero (0.0, 0.0);}
	WarpwarWindow (wxWindow * parent);
	~ WarpwarWindow (void);
	void draw_cell (double x, double y) {
		double shift60 = (double) cell_side * 0.866025404;
		double shift30 = (double) cell_side * 0.5;
		if (orientation) {
			int x1 = (int) (x - shift60);
			int x2 = (int) x;
			int x3 = (int) (x + shift60);
			int y1 = (int) (y + shift30);
			int y2 = (int) (y - shift30);
			int y3 = (int) (y - (double) cell_side);
			galaxy_dc -> DrawLine (x1, y1, x1, y2);
			galaxy_dc -> DrawLine (x1, y2, x2, y3);
			galaxy_dc -> DrawLine (x2, y3, x3, y2);
		} else {
			int x1 = (int) (x - (double) cell_side);
			int x2 = (int) (x - shift30);
			int x3 = (int) (x + shift30);
			int y1 = (int) (y + shift60);
			int y2 = (int) y;
			int y3 = (int) (y - shift60);
			galaxy_dc -> DrawLine (x1, y2, x2, y1);
			galaxy_dc -> DrawLine (x1, y2, x2, y3);
			galaxy_dc -> DrawLine (x2, y3, x3, y3);
		}
	}
	int getX (int ind) {return (int) ((double) zero_x + (double) ind * (double) cell_side * 1.5);}
	int getY (int y, int x) {return (int) ((double) zero_y + (double) y * cell_side * 0.866025404 * 2.0 - (double) x * cell_side * 0.866025404);}
	void draw_cells (int rows, int columns) {
		double shift60 = (double) cell_side * 0.866025404;
		double shift30 = (double) cell_side * 0.5;
		for (int column = 0; column < columns; column++) {
			for (int row = 0; row < rows; row++) {
				int location_x = orientation ? getY (row, column) : getX (column); //(double) zero_x + (double) column * (double) cell_side * 1.5;
				int location_y = orientation ? getX (column) : getY (row, column); //(double) zero_y + (double) row * (shift60 + shift60) - (double) column * shift60;
				draw_cell (location_x, location_y);
				wxString grid = wxString :: Format (_T ("%02i%02i"), row, column);
				wxSize extent = galaxy_dc -> GetTextExtent (grid);
				galaxy_dc -> DrawText (grid, location_x - extent . x / 2, location_y - (int) shift60);
			}
		}
	}
	void draw_star (char * name, int x, int y) {
		if (galaxy_dc == NULL) return;
		int xx = orientation ? getY (y, x) : getX (x);
		int yy = orientation ? getX (x) : getY (y, x);
		galaxy_dc -> DrawCircle (xx, yy, 7);
		wxString star = wxString :: From8BitData (name);
		wxSize extent = galaxy_dc -> GetTextExtent (star);
		galaxy_dc -> SetTextForeground (wxColour (128, 128, 128));
		galaxy_dc -> DrawText (star, xx - extent . x / 2, yy + 9);
	}
	void draw_route (int x1, int y1, int x2, int y2) {
		if (galaxy_dc == NULL) return;
		int xx1 = orientation ? getY (y1, x1) : getX (x1);
		int yy1 = orientation ? getX (x1) : getY (y1, x1);
		int xx2 = orientation ? getY (y2, x2) : getX (x2);
		int yy2 = orientation ? getX (x2) : getY (y2, x2);
		galaxy_dc -> DrawLine (xx1, yy1, xx2, yy2);
	}
	void clear (void) {
		galaxy_dc -> SetBackground (wxBrush (wxColour (0, 0, 0)));
		galaxy_dc -> SetPen (wxPen (wxColour (32, 32, 32)));
		galaxy_dc -> Clear ();
	}
	void draw_grid (void) {
		wxFont f = galaxy_dc -> GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (7);
		galaxy_dc -> SetFont (f);
		galaxy_dc -> SetTextForeground (wxColour (32, 32, 32));
		draw_cells (26, 26);
	}
	void OnPaint (wxPaintEvent & event) {
		wxBufferedPaintDC dc (this);
		dc . SetBackground (wxBrush (wxColour (0, 0, 0)));
		dc . Clear ();
		dc . DrawBitmap (* galaxy_bitmap, bitmap_position_x, bitmap_position_y, true);
		//if (galaxy_dc == NULL) return;
		//dc . Blit (0, 0, galaxy_width, galaxy_height, galaxy_dc, 0, 0);
	}
	void OnMouse (wxMouseEvent & event) {
		if (! event . Dragging ()) return;
		wxPoint px = event . GetPosition ();
		bitmap_position_x += px . x - captured_x;
		//if (bitmap_position_x < 0) bitmap_position_x = 0;
		captured_x = px . x;
		bitmap_position_y += px . y - captured_y;
		//if (bitmap_position_y < 0) bitmap_position_y = 0;
		captured_y = px . y;
		Refresh ();
		//printf ("<%i %i>\n", bitmap_position_x, bitmap_position_y);
	}
	void OnLeftDown (wxMouseEvent & event) {
		CaptureMouse ();
		wxPoint px = event . GetPosition ();
		captured_x = px . x; captured_y = px . y;
		//printf ("[%i %i]\n", px . x, px . y);
	}
	void OnLeftUp (wxMouseEvent & event) {
		ReleaseMouse ();
	}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(WarpwarWindow, wxWindow)
EVT_PAINT(WarpwarWindow :: OnPaint)
EVT_MOTION(WarpwarWindow :: OnMouse)
EVT_LEFT_UP(WarpwarWindow :: OnLeftUp)
EVT_LEFT_DOWN(WarpwarWindow :: OnLeftDown)
END_EVENT_TABLE()

static WarpwarWindow * galaxy = NULL;

class erase_galaxy : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		galaxy -> clear ();
		return true;
	}
};

class draw_grid : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		galaxy -> draw_grid ();
		return true;
	}
};

class orientation : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		galaxy -> orientation = ! galaxy -> orientation;
		return true;
	}
};

class hexside : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isInteger ()) return false;
		galaxy -> setCellSide (parameters -> getInteger ());
		return true;
	}
};

class zero : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * x = parameters -> getLeft ();
		if (! x -> isDouble ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * y = parameters -> getLeft ();
		if (! y -> isDouble ()) return false;
		galaxy -> setZero (x -> getDouble (), y -> getDouble ());
		return true;
	}
};

class draw_star : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * name = parameters -> getLeft ();
		if (! name -> isText () && ! name -> isAtom ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * x = parameters -> getLeft ();
		if (! x -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * y = parameters -> getLeft ();
		if (! y -> isInteger ()) return false;
		galaxy -> draw_star (name -> isText () ? name -> getText () : name -> getAtom () -> name (), x -> getInteger (), y -> getInteger ());
		return true;
	}
};

class draw_route : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * l1 = parameters -> getLeft ();
		if (! l1 -> isPair ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * l2 = parameters -> getLeft ();
		if (! l2 -> isPair ()) return false;
		PrologElement * x1 = l1 -> getLeft ();
		if (! x1 -> isInteger ()) return false;
		l1 = l1 -> getRight ();
		if (! l1 -> isPair ()) return false;
		PrologElement * y1 = l1 -> getLeft ();
		if (! y1 -> isInteger ()) return false;
		PrologElement * x2 = l2 -> getLeft ();
		if (! x2 -> isInteger ()) return false;
		l2 = l2 -> getRight ();
		if (! l2 -> isPair ()) return false;
		PrologElement * y2 = l2 -> getLeft ();
		if (! y2 -> isInteger ()) return false;
		galaxy -> draw_route (x1 -> getInteger (), y1 -> getInteger (), x2 -> getInteger (), y2 -> getInteger ());
		return true;
	}
};

class star_colour : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (galaxy == NULL) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * red = parameters -> getLeft ();
		if (! red -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * green = parameters -> getLeft ();
		if (! green -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * blue = parameters -> getLeft ();
		if (! blue -> isInteger ()) return false;
		galaxy -> setStarColour (red -> getInteger (), green -> getInteger (), blue -> getInteger ());
		return true;
	}
};

class galaxy_service_class : public PrologServiceClass {
public:
	PrologRoot * root;
	virtual void init (PrologRoot * root) {this -> root = root;}
	virtual PrologNativeCode * getNativeCode (char * name) {
		if (strcmp (name, "erase") == 0) return new erase_galaxy ();
		if (strcmp (name, "grid") == 0) return new draw_grid ();
		if (strcmp (name, "orientation") == 0) return new orientation ();
		if (strcmp (name, "hexside") == 0) return new hexside ();
		if (strcmp (name, "zero") == 0) return new zero ();
		if (strcmp (name, "draw_star") == 0) return new draw_star ();
		if (strcmp (name, "draw_route") == 0) return new draw_route ();
		if (strcmp (name, "star_colour") == 0) return new star_colour ();
		return NULL;
	}
	galaxy_service_class (void) {}
	~ galaxy_service_class (void) {}
};

PrologServiceClass * galaxy_return (void) {return new galaxy_service_class ();}

WarpwarWindow :: WarpwarWindow (wxWindow * parent) : wxWindow (parent, -1) {
	orientation = false;
	captured_x = captured_y = bitmap_position_x = bitmap_position_y = 0;
	galaxy = this;
	setCellSide (30);
	setZero (0.0, 13.0);
	reSizeGalaxy (2048, 2048);
	new PrologThread ();
}
WarpwarWindow :: ~ WarpwarWindow (void) {galaxy = NULL; deleteGalaxy ();}

class WarpwarFrame : public wxFrame {
public:
	WarpwarFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("GALAXY"), wxPoint (100, 100), wxSize (600, 500)) {
		new WarpwarWindow (this);
	}
	~ WarpwarFrame (void) {}
};

class WarpwarApp : public wxApp {
	bool OnInit (void) {
		(new WarpwarFrame (NULL)) -> Show ();
		return true;
	}
	int OnExit (void) {
		//wxMessageBox (_T ("EXIT"), _T ("INFO"), wxOK, NULL);
		return wxApp :: OnExit ();
	}
};


IMPLEMENT_APP (WarpwarApp);

