///////////////////////////////////////
// Copyright (C) 2011 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#define PROTECT

#ifdef PROTECT
#define BOARD_POSITION wxPoint (1450, 900)
#define BOARD_SIZE wxSize (120, 120)
//#define BOARD_POSITION wxPoint (2000, 300)
//#define BOARD_SIZE wxSize (600, 400)
#else
#define BOARD_POSITION wxPoint (100, 100)
#define BOARD_SIZE wxSize (800, 500)
#endif

#include "wx/wx.h"
#include "wx/dcbuffer.h"

static int numbering [] = {0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36, 11, 30, 8, 23, 10, 5, 24, 16, 33, 1, 20, 14, 31, 9, 22, 18, 29, 7, 28, 12, 35, 3, 26};

wxPoint MP (wxPoint in, double scale) {
	in . x = (double) in . x * scale + 0.4999;
	in . y = (double) in . y * scale + 0.4999;
	return in;
}

class RouletteWheel {
public:
	wxPoint location;
	int radius;
	double angle;
	double omega;
	double ySin;
	wxPoint ballLocation;
	wxPoint ballAccelerationVector;
	double ballAccelerationDumping;
	double ballAccelerationAmplification;
	int ballTarget;
	RouletteWheel (wxPoint location, int radius) {
		this -> ballLocation = this -> location = location;
		ballAccelerationVector = wxPoint (0, 0);
		ballAccelerationDumping = 0.99;
		ballAccelerationAmplification = 0.08;
		this -> radius = radius;
		angle = 0.0;
		omega = M_PI / 180.0;
		ySin = (double) radius * sin (M_PI / 18.5);
		ballTarget = -1;
	}
	void throwBall (void) {
		ballTarget = 0;
		ballAccelerationVector = MP (location - ballLocation, 3.0);
		ballTarget = rand () % 37;
	}
	void moveBall (void) {
		ballLocation += MP (ballAccelerationVector, ballAccelerationAmplification);
		ballAccelerationVector = MP (ballAccelerationVector, ballAccelerationDumping);
		if (ballTarget < 0) return;
		double targetAngle = angle + ballTarget * M_PI / 18.5;
		wxPoint target ((double) radius * cos (targetAngle), (double) radius * sin (targetAngle));
		ballAccelerationVector += location + target - ballLocation;
	}
	void move (void) {
		angle += omega;
		moveBall ();
	}
	void draw (wxDC & dc) {
		dc . SetTextForeground (* wxWHITE);
		wxFont f = dc . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (24);
		dc . SetFont (f);
		bool red = true;
		for (int ind = 0; ind < 37; ind++) {
			wxString text = wxString :: Format (_T ("%i"), numbering [ind]);
			wxSize extent = dc . GetTextExtent (text);
			double current_angle = angle + (double) ind * M_PI / 18.5;
			double current_angle_degrees = current_angle * 180.0 / M_PI - 90.0 + 4.86;
			double shift_angle = current_angle - M_PI * 0.5 + M_PI / 37.0;
			wxPoint ray_location = location - wxPoint ((double) radius * cos (current_angle) + 0.4999, (double) radius * sin (current_angle) + 0.4999);
			double correction = (ySin - (double) extent . x) * 0.5;
			wxPoint text_shift = ray_location + wxPoint (correction * cos (shift_angle) + 0.4999, correction * sin (shift_angle) + 0.4999);
			//dc . DrawLine (location, ray_location);
			//dc . DrawLine (ray_location, text_shift);
			wxPoint area [4];
			area [0] = ray_location;
			area [1] = ray_location + wxPoint (ySin * cos (shift_angle) + 0.4999, ySin * sin (shift_angle) + 0.4999);
			area [2] = MP (location, 0.2) + MP (area [1], 0.8);
			area [3] = MP (location, 0.2) + MP (area [0], 0.8);
			if (ind == 0) dc . SetBrush (wxBrush (* wxGREEN));
			else {dc . SetBrush (wxBrush (red ? * wxRED : * wxBLACK)); red = ! red;}
			dc . DrawPolygon (4, area);
			dc . DrawRotatedText (text, text_shift, - current_angle_degrees);
		}
		dc . SetPen (wxPen (* wxWHITE));
		dc . SetBrush (wxBrush (* wxWHITE));
		dc . DrawCircle (ballLocation, 6);
	}
};

static bool idleRepaint = false;

class RouletteWindow;

class WheelThread : public wxThread {
public:
	RouletteWindow * window;
	WheelThread (RouletteWindow * window) {this -> window = window; this -> Create (16384); this -> Run ();}
	virtual ExitCode Entry (void);
};

class RouletteWindow : public wxWindow {
public:
	RouletteWheel wheel;
	RouletteWindow (wxWindow * parent) : wxWindow (parent, -1), wheel (wxPoint (300, 300), 260) {
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		new WheelThread (this);
	}
	void OnPaint (wxPaintEvent & event) {
		wxBufferedPaintDC dc (this);
		dc . SetBackground (* wxBLUE);
		dc . Clear ();
		wheel . draw (dc);
	}
	void ThrowBall (void) {
		wheel . throwBall ();
	}
	void OnLeftDown (wxMouseEvent & event) {
		CaptureMouse ();
		ThrowBall ();
	}
	void OnLeftUp (wxMouseEvent & event) {
		ReleaseMouse ();
	}
	void OnIdle (wxIdleEvent & event) {if (idleRepaint) Refresh (); idleRepaint = false;}
private:
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(RouletteWindow, wxWindow)
EVT_PAINT(RouletteWindow :: OnPaint)
EVT_IDLE(RouletteWindow :: OnIdle)
EVT_LEFT_DOWN(RouletteWindow :: OnLeftDown)
EVT_LEFT_UP(RouletteWindow :: OnLeftUp)
END_EVENT_TABLE()

wxThread :: ExitCode WheelThread :: Entry (void) {
	while (true) {
		window -> wheel . move ();
//		window -> wheel . angle += window -> wheel . omega;
//		window -> wheel . moveBall ();
		idleRepaint = true;
		wxWakeUpIdle ();
		Sleep (50);
	}
	Exit ();
	return Wait ();
}

class RouletteFrame : public wxFrame {
public:
	RouletteFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("FRENCH ROULETTE"), BOARD_POSITION, BOARD_SIZE) {
		new RouletteWindow (this);
	}
};

class RouletteClass : public wxApp {
public:
	bool OnInit (void) {
		wxInitAllImageHandlers ();
		(new RouletteFrame (0)) -> Show ();
		return true;
	}
	int OnExit (void) {return wxApp :: OnExit ();}
};

IMPLEMENT_APP(RouletteClass);






























