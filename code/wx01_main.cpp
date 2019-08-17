///////////////////////////////////////
// Copyright (C) 2014 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#include "wx/wx.h"
#include "wx/dcbuffer.h"

class TwoDView : public wxWindow {
public:
	int scaling_factor;
	TwoDView (wxWindow * parent, wxWindowID id) : wxWindow (parent, id) {
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		scaling_factor = 0;
	}
	void OnPaint (wxPaintEvent & event) {
		wxBufferedPaintDC dc (this);
		wxSize size = dc . GetSize ();
		dc . Clear ();
		int side = (double) (100.0 * pow (2.0, (double) scaling_factor / 2400.0));
		dc . DrawRoundedRectangle (20, 20, side, side, 8);
		//printf ("size [%i %i]\n", size . GetWidth (), size . GetHeight ());
	}
	void OnMouseLeftDown (wxMouseEvent & event) {
		wxPoint p = event . GetPosition ();
		//printf ("position [%i %i]\n", p . x, p . y);
	}
	void OnMouseWheel(wxMouseEvent & event) {
		//printf ("wheel [%i]\n", event . GetWheelRotation ());
		scaling_factor += event . GetWheelRotation ();
		Refresh ();
	}
private: DECLARE_EVENT_TABLE();
};
BEGIN_EVENT_TABLE (TwoDView, wxWindow)
EVT_PAINT (TwoDView :: OnPaint)
EVT_LEFT_DOWN (TwoDView :: OnMouseLeftDown)
EVT_MOUSEWHEEL (TwoDView :: OnMouseWheel)
END_EVENT_TABLE ()

class Wx01Frame : public wxFrame {
public:
	TwoDView * twoDView;
	Wx01Frame (void) : wxFrame (0, -1, _T ("TRACK BUILDER"), wxPoint (20, 100), wxSize (400, 200), wxDEFAULT_FRAME_STYLE) {
		twoDView = new TwoDView (this, -1);
	}
private: DECLARE_EVENT_TABLE();
};
BEGIN_EVENT_TABLE (Wx01Frame, wxFrame)
END_EVENT_TABLE()

class Wx01MainClass : public wxApp {
public:
	Wx01Frame * wx01Frame;
	Wx01MainClass (void) {wx01Frame = 0;}
	bool OnInit (void) {
		printf ("OnInit []\n");
		wx01Frame = new Wx01Frame ();
		wx01Frame -> Show ();
		return true;
	}
	int OnExit (void) {printf ("OnExit []\n"); return wxApp :: OnExit ();}
};

IMPLEMENT_APP (Wx01MainClass);

