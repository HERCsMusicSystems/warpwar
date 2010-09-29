
#include "wx/wx.h"
#include "wx/dcbuffer.h"

class WarpwarWindow : public wxWindow {
public:
	wxBitmap * galaxy_bitmap;
	wxMemoryDC * galaxy_dc;
	int galaxy_width, galaxy_height;
	int cell_side;
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
	void setCellSide (int side) {this -> cell_side = side;}
	WarpwarWindow (wxWindow * parent) : wxWindow (parent, -1) {reSizeGalaxy (300, 200); setCellSide (50);}
	~ WarpwarWindow (void) {deleteGalaxy ();}
	void draw_cell (double x, double y) {
		double shift60 = (double) cell_side * 0.866025404;
		double shift30 = (double) cell_side * 0.5;
		int x1 = (int) (x - (double) cell_side);
		int x2 = (int) (x - shift30);
		int x3 = (int) (x + shift30);
		int y1 = (int) (y + shift60);
		int y2 = (int) y;
		int y3 = (int) (y - shift60);
		galaxy_dc -> DrawLine (x1, y2, x2, y1);
		galaxy_dc -> DrawLine (x1, y2, x2, y3);
		galaxy_dc -> DrawLine (x2, y3, x3, y3);
//		galaxy_dc -> DrawText (wxString :: Format (_T ("%02i%02i"), row, column), x2, y3);
	}
	void draw_cells (int rows, int columns) {
		double shift60 = (double) cell_side * 0.866025404;
		double shift30 = (double) cell_side * 0.5;
		for (int column = 0; column < columns; column++) {
			for (int row = 0; row < rows; row++) {
				int location_x = (double) cell_side + (double) column * (double) cell_side * 1.5;
				int location_y = 200.0 + (double) row * (shift60 + shift60) - (double) column * shift60;
				draw_cell (location_x, location_y);
				galaxy_dc -> DrawText (wxString :: Format (_T ("%02i%02i"), row, column), location_x - (int) shift30, location_y - (int) shift60);
			}
		}
	}
	void draw (void) {
		galaxy_dc -> SetBackground (wxBrush (wxColour (0, 0, 0)));
		galaxy_dc -> SetPen (wxPen (wxColour (128, 128, 128)));
		galaxy_dc -> Clear ();
		wxFont f = galaxy_dc -> GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (12);
		galaxy_dc -> SetFont (f);
		galaxy_dc -> SetTextForeground (wxColour (128, 128, 128));
//		dc . SetTextForeground (reversed ? wxColour (red_lp -> r0, red_lp -> g0, red_lp -> b0) : wxColour (lcp -> r0, lcp -> g0, lcp -> b0));
		draw_cells (30, 30);
	}
	void OnPaint (wxPaintEvent & event) {
		reSizeGalaxy (this -> GetSize() . x, this -> GetSize () . y);
		draw ();
		wxBufferedPaintDC dc (this);
		dc . Blit (0, 0, galaxy_width, galaxy_height, galaxy_dc, 0, 0);
	}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(WarpwarWindow, wxWindow)
EVT_PAINT(WarpwarWindow :: OnPaint)
END_EVENT_TABLE()

class WarpwarFrame : public wxFrame {
public:
	WarpwarFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("GALAXY"), wxPoint (100, 100), wxSize (600, 500)) {
		new WarpwarWindow (this);
	}
	~ WarpwarFrame (void) {}
};

class WarpwarApp : public wxApp {
	bool OnInit (void) {
		//wxMessageBox (_T ("ENTER"), _T ("INFO"), wxOK, NULL);
		(new WarpwarFrame (NULL)) -> Show ();
		return true;
	}
	int OnExit (void) {
		//wxMessageBox (_T ("EXIT"), _T ("INFO"), wxOK, NULL);
		return wxApp :: OnExit ();
	}
};


IMPLEMENT_APP (WarpwarApp);

