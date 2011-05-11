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

class BoardToken {
public:
	wxBitmap token;
	wxBitmap rotated;
	wxPoint position;
	wxSize token_size;
	int choosenRotation;
	void rotate (int angle) {
		while (angle > 360) angle -= 360;
		while (angle < 0) angle += 360;
		rotated = wxBitmap (token . ConvertToImage () . Rotate (M_PI * (double) angle / 180.0, wxPoint (token . GetWidth () / 2, token . GetHeight () / 2)));
		token_size = wxSize (rotated . GetWidth (), rotated . GetHeight ());
		choosenRotation = angle;
	}
	void rotateRight (void) {
		int rotations [] = {315, 300, 270, 240, 225, 210, 180, 150, 135, 120, 90, 60, 45, 30, 0};
		for (int ind = 0; ind < 15; ind++) {
			if (choosenRotation > rotations [ind]) return rotate (rotations [ind]);
		}
		rotate (330);
	}
	void rotateLeft (void) {
		int rotations [] = {30, 45, 60, 90, 120, 135, 150, 180, 210, 225, 240, 270, 300, 315, 330};
		for (int ind = 0; ind < 15; ind++) {
			if (choosenRotation < rotations [ind]) return rotate (rotations [ind]);
		}
		rotate (0);
	}
	void move (wxPoint & delta) {
		position += delta;
	}
	void moveAll (wxPoint & delta) {
		position += delta;
		if (next != 0) next -> moveAll (delta);
	}
	BoardToken * draw (wxDC & dc) {
		dc . DrawBitmap (rotated, position, true);
		return next;
	}
	BoardToken * hitFind (wxPoint position) {
		if (position . x >= this -> position . x
			&& position . x < this -> position . x + token_size . GetWidth ()
			&& position . y >= this -> position . y
			&& position . y < this -> position . y + token_size . GetHeight ()
			) return this;
		if (next == 0) return 0;
		return next -> hitFind (position);
	}
	BoardToken (wxString file_name, wxPoint position, BoardToken * next = 0) {
		token . LoadFile (file_name, wxBITMAP_TYPE_PNG);
		rotate (0);
		this -> position = position;
		this -> next = next;
	}
	BoardToken * next;
};

class BoardWindow : public wxWindow {
public:
	wxBitmap board;
	wxPoint boardLocation;
	BoardToken * tokens;
	wxPoint lastRightClickPosition;
	BoardToken * dragToken;
	wxPoint capturedPosition;
	BoardWindow (wxWindow * parent, wxWindowID id) : wxWindow (parent, id) {
		boardLocation = wxPoint (0, 0);
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		tokens = 0;
		dragToken = 0;
		//board . LoadFile (_T ("board.png"), wxBITMAP_TYPE_PNG);
		//tokens = new BoardToken (_T ("token.png"), wxPoint (20, 20));
		lastRightClickPosition = capturedPosition = wxPoint (10, 10);
	}
	~ BoardWindow (void) {
	}
	void OnPaint (wxPaintEvent & event) {
		wxBufferedPaintDC dc (this);
		dc . SetBackground (wxBrush (wxColour (0, 0, 255)));
		dc . Clear ();
		dc . DrawBitmap (board, boardLocation . x, boardLocation . y, true);
		BoardToken * tkp = tokens;
		while (tkp != 0) {tkp = tkp -> draw (dc);}
		if (dragToken != 0) {
			dc . SetPen (wxPen (wxColour (255, 255, 255), 1, wxDOT));
			dc . SetBrush (* wxTRANSPARENT_BRUSH);
			dc . DrawRectangle (dragToken -> position, dragToken -> token_size);
		}
	}
	void OnLeftDown (wxMouseEvent & event) {
		CaptureMouse ();
		capturedPosition = event . GetPosition ();
		if (tokens == 0) {dragToken = 0; return;}
		dragToken = tokens -> hitFind (capturedPosition);
		Refresh ();
	}
	void OnLeftUp (wxMouseEvent & event) {
		ReleaseMouse ();
	}
	void OnMotion (wxMouseEvent & event) {
		if (! event . Dragging ()) return;
		wxPoint p = event . GetPosition ();
		wxPoint delta = p - capturedPosition;
		if (dragToken == 0) {
			if (tokens != 0) tokens -> moveAll (delta);
			boardLocation += delta;
		} else dragToken -> move (delta);
		capturedPosition = p;
		Refresh ();
	}
	void OnRightDown (wxMouseEvent & event) {
		lastRightClickPosition = event . GetPosition ();
		dragToken = tokens == 0 ? 0 : tokens -> hitFind (lastRightClickPosition);
		wxMenu menu;
		if (dragToken == 0) menu . Append (4001, _T ("New Token"));
		if (dragToken != 0) menu . Append (4101, _T ("Rotate right"));
		if (dragToken != 0) menu . Append (4102, _T ("Rotate left"));
		PopupMenu (& menu, lastRightClickPosition);
	}
	void OnNewToken (wxCommandEvent & event) {
		wxFileDialog picker (this);
		picker . SetWildcard (_T ("PNG pictures (*.png)|*.png"));
		if (picker . ShowModal () == wxID_OK) {
			tokens = new BoardToken (picker . GetDirectory () + _T ("/") + picker . GetFilename (), lastRightClickPosition, tokens);
			Refresh ();
		}
	}
	void OnRotateRight (wxCommandEvent & event) {
		if (dragToken != 0) dragToken -> rotateRight ();
		Refresh ();
	}
	void OnRotateLeft (wxCommandEvent & event) {
		if (dragToken != 0) dragToken -> rotateLeft ();
		Refresh ();
	}
	void OnNewBoard (wxCommandEvent & event) {
		wxFileDialog picker (this);
		picker . SetWildcard (_T ("PNG pictures (*.png)|*.png"));
		if (picker . ShowModal () == wxID_OK) {
			board . LoadFile (picker . GetDirectory () + _T ("/") + picker . GetFilename (), wxBITMAP_TYPE_PNG);
			Refresh ();			
		}
	}
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
EVT_MENU(4101, BoardWindow :: OnRotateRight)
EVT_MENU(4102, BoardWindow :: OnRotateLeft)
END_EVENT_TABLE()

class BoardFrame : public wxFrame {
public:
	BoardWindow * board;
	BoardFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("TABLE TOP"), BOARD_POSITION, BOARD_SIZE) {
		board = new BoardWindow (this, -1);
		wxMenuBar * bar = new wxMenuBar ();
		wxMenu * board_menu = new wxMenu ();
		board_menu -> Append (4201, _T ("New board	B"));
		bar -> Append (board_menu, _T ("Board"));
		wxMenu * token_menu = new wxMenu ();
		token_menu -> Append (4101, _T ("Rotate right	]"));
		token_menu -> Append (4102, _T ("Rotate left	["));
		bar -> Append (token_menu, _T ("Token"));
		SetMenuBar (bar);
	}
	~ BoardFrame (void) {
	}
	void OnRotateRight (wxCommandEvent & event) {board -> OnRotateRight (event);}
	void OnRotateLeft (wxCommandEvent & event) {board -> OnRotateLeft (event);}
	void OnNewBoard (wxCommandEvent & event) {board -> OnNewBoard (event);}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(BoardFrame, wxFrame)
EVT_MENU(4101, BoardFrame :: OnRotateRight)
EVT_MENU(4102, BoardFrame :: OnRotateLeft)
EVT_MENU(4201, BoardFrame :: OnNewBoard)
END_EVENT_TABLE()

class BoardClass : public wxApp {
public:
	bool OnInit (void) {
		wxInitAllImageHandlers ();
		(new BoardFrame (0)) -> Show ();
		return true;
	}
	int OnExit (void) {
		return wxApp :: OnExit ();
	}
};

IMPLEMENT_APP(BoardClass);
