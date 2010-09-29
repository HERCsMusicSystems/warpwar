
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
	WarpwarWindow (wxWindow * parent) : wxWindow (parent, -1) {reSizeGalaxy (300, 200); setCellSide (30);}
	~ WarpwarWindow (void) {deleteGalaxy ();}
	void draw_cell (double x, double y) {
		int point1 = (int) y - cell_side;
		int point2 = point1 - (cell_side / 2);
		int point3 = (int) (x + (double) cell_side * 0.866025404);
		int point4 = (int) (x + (double) cell_side * 2.0 * 0.866025404);
		galaxy_dc -> DrawLine ((int) x, (int) y, (int) x, point1);
		galaxy_dc -> DrawLine ((int) x, point1, point3, point2);
		galaxy_dc -> DrawLine (point3, point2, point4, point1);
	}
	void draw_cells (void) {
		for (int row = 0; row < 10; row++) {
			for (int ind = 0; ind < 10 + row; ind++) {
				draw_cell (200.0 + (double) ind * 1.732050808 * (double) cell_side - (double) row * (double) cell_side * 0.866025404, 50.0 + (double) cell_side * 1.5 * (double) row);
			}
		}
	}
	void draw (void) {
		galaxy_dc -> SetBackground (wxBrush (wxColour (0, 0, 255)));
		galaxy_dc -> Clear ();
		//galaxy_dc -> DrawRectangle (100, 100, 40, 40);
		draw_cells ();
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

