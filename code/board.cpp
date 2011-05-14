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

class BoardToken {
public:
	wxBitmap token;
	wxBitmap rotated;
	wxPoint position;
	wxPoint rotatedShift;
	wxSize token_size;
	int choosenRotation;
	void rotate (int angle) {
		while (angle > 360) angle -= 360;
		while (angle < 0) angle += 360;
		rotated = wxBitmap (token . ConvertToImage () . Rotate (M_PI * (double) angle / 180.0, wxPoint (token . GetWidth () / 2, token . GetHeight () / 2)));
		token_size = wxSize (rotated . GetWidth (), rotated . GetHeight ());
		//rotatedPosition = wxPoint (token_size . GetWidth () / 2, token_size . GetHeight () / 2);
		rotatedShift = wxPoint ((token . GetWidth () - rotated . GetWidth ()) / 2, (token . GetHeight () - rotated . GetHeight ()) / 2);
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
		dc . DrawBitmap (rotated, position + rotatedShift, true);
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
		this -> position = position;
		this -> next = next;
		token . LoadFile (file_name, wxBITMAP_TYPE_PNG);
		rotate (0);
	}
	BoardToken * next;
};

class BoardWindow;

class GridMenuDialogClass : public wxDialog {
public:
	BoardWindow * board;
	wxComboBox * gridType;
	wxSpinCtrl * sideCtrl;
	GridMenuDialogClass (BoardWindow * board);
	void OnSize (wxSpinEvent & event);
private: DECLARE_EVENT_TABLE();
};
BEGIN_EVENT_TABLE(GridMenuDialogClass, wxDialog)
EVT_SPINCTRL(102, GridMenuDialogClass :: OnSize)
END_EVENT_TABLE()

class BoardWindow : public wxWindow {
public:
	wxBitmap board;
	wxPoint boardLocation;
	BoardToken * tokens;
	wxPoint lastRightClickPosition;
	BoardToken * dragToken;
	wxPoint capturedPosition;
	wxBitmap grid;
	int gridType;
	int gridSide;
	int gridWidth, gridHeight, gridStartX, gridStartY;
	bool gridIndexing;
	BoardWindow (wxWindow * parent, wxWindowID id) : wxWindow (parent, id) {
		gridType = 2;
		gridSide = 100;
		gridWidth = gridHeight = 10;
		gridStartX = gridStartY = 0;
		gridIndexing = true;
		boardLocation = wxPoint (0, 0);
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		tokens = 0;
		dragToken = 0;
		lastRightClickPosition = capturedPosition = wxPoint (10, 10);

		grid = wxBitmap (300, 200, 32);
		wxMemoryDC gridDC (grid);
		gridDC . SetBackground (* wxBLACK);
		gridDC . Clear ();
		gridDC . SetPen (wxPen (wxColour (255, 0, 0)));
		gridDC . DrawLine (0, 0, 200, 200);
		grid . SetMask (new wxMask (grid, wxColour (0, 0, 0)));
	}
	~ BoardWindow (void) {
	}
	void OnPaint (wxPaintEvent & event) {
		wxBufferedPaintDC dc (this);
		dc . SetBackground (wxBrush (wxColour (0, 0, 255)));
		dc . Clear ();
		// draw board
		dc . DrawBitmap (board, boardLocation . x, boardLocation . y, true);
		// draw grid
		dc . DrawBitmap (grid, 0, 0, true);
		//dc . Blit (0, 0, 300, 200, & gridDC, 0, 0);
		// draw tokens
		BoardToken * tkp = tokens;
		while (tkp != 0) {tkp = tkp -> draw (dc);}
		// draw selected square
		if (dragToken != 0) {
			dc . SetPen (wxPen (wxColour (255, 255, 255), 1, wxDOT));
			dc . SetBrush (* wxTRANSPARENT_BRUSH);
			dc . DrawRectangle (dragToken -> position + dragToken -> rotatedShift, dragToken -> token_size);
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
		boardLocation = wxPoint (10, 10);
		wxFileDialog picker (this);
		picker . SetWildcard (_T ("PNG pictures (*.png)|*.png"));
		if (picker . ShowModal () == wxID_OK) {
			board . LoadFile (picker . GetDirectory () + _T ("/") + picker . GetFilename (), wxBITMAP_TYPE_PNG);
			Refresh ();			
		}
	}
	void OnGridMenu (wxCommandEvent & event) {
		GridMenuDialogClass dialog (this);
		dialog . ShowModal ();
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

GridMenuDialogClass :: GridMenuDialogClass (BoardWindow * board) : wxDialog (board, -1, _T ("Grid dialog.")) {
	this -> board = board;
	wxFlexGridSizer * grid = new wxFlexGridSizer (6, 2, 0, 0);
	grid -> Add (new wxStaticText (this, -1, _T ("Grid")));
	gridType = new wxComboBox (this, 101,  _T (""), wxDefaultPosition, wxSize (-1, -1), 0, NULL, wxCB_READONLY);
	gridType -> Append (_T ("NONE"));
	gridType -> Append (_T ("SQUARE"));
	gridType -> Append (_T ("HORIZONTAL HEX"));
	gridType -> Append (_T ("VERTICAL HEX"));
	gridType -> SetSelection (board -> gridType);
	grid -> Add (gridType);
	grid -> Add (new wxStaticText (this, -1, _T ("SIDE")));
	sideCtrl = new wxSpinCtrl (this, 102);
	sideCtrl -> SetRange (10, 200);
	sideCtrl -> SetValue (board -> gridSide);
	grid -> Add (sideCtrl);
	grid -> Add (new wxStaticText (this, -1, _T ("START X")));
	wxSpinCtrl * startXCtrl = new wxSpinCtrl (this, 103);
	startXCtrl -> SetRange (-200, 200);
	startXCtrl -> SetValue (board -> gridStartX);
	grid -> Add (startXCtrl);
	grid -> Add (new wxStaticText (this, -1, _T ("START Y")));
	wxSpinCtrl * startYCtrl = new wxSpinCtrl (this, 104);
	startYCtrl -> SetRange (-200, 200);
	startYCtrl -> SetValue (board -> gridStartY);
	grid -> Add (startYCtrl);
	grid -> Add (new wxStaticText (this, -1, _T ("GRID SIZE X")));
	wxSpinCtrl * sizeXCtrl = new wxSpinCtrl (this, 105);
	sizeXCtrl -> SetRange (0, 400);
	sizeXCtrl -> SetValue (board -> gridWidth);
	grid -> Add (sizeXCtrl);
	grid -> Add (new wxStaticText (this, -1, _T ("GRID SIZE Y")));
	wxSpinCtrl * sizeYCtrl = new wxSpinCtrl (this, 106);
	sizeYCtrl -> SetRange (0, 400);
	sizeYCtrl -> SetValue (board -> gridHeight);
	grid -> Add (sizeYCtrl);
	SetSizer (grid);
	grid -> SetSizeHints (this);
	Centre ();
}

void GridMenuDialogClass :: OnSize (wxSpinEvent & event) {
	board -> gridSide = sideCtrl -> GetValue ();
}

class BoardFrame : public wxFrame {
public:
	BoardWindow * board;
	BoardFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("TABLE TOP"), BOARD_POSITION, BOARD_SIZE) {
		board = new BoardWindow (this, -1);
		wxMenuBar * bar = new wxMenuBar ();

		wxMenu * file_menu = new wxMenu ();
		file_menu -> Append (4301, _T ("Load	F1"));
		file_menu -> Append (4302, _T ("Save	CTRL+S"));
		file_menu -> Append (4303, _T ("Save as	CTRL+A"));
		file_menu -> Append (4304, _T ("Exit	Q"));
		bar -> Append (file_menu, _T ("File"));

		wxMenu * grid_menu = new wxMenu ();
		grid_menu -> Append (4401, _T ("Grid Setup	F3"));
		grid_menu -> Append (4402, _T ("Square grid	F4"));
		grid_menu -> Append (4403, _T ("Hex grid (horizontal)	F5"));
		grid_menu -> Append (4404, _T ("Hex grid (vertical)	F6"));
		grid_menu -> Append (4405, _T ("Lock grid	F7"));
		bar -> Append (grid_menu, _T ("Grid"));

		wxMenu * board_menu = new wxMenu ();
		board_menu -> Append (4201, _T ("New board	CTRL+B"));
		board_menu -> Append (4202, _T ("Lock board	F8"));
		bar -> Append (board_menu, _T ("Board"));

		wxMenu * token_menu = new wxMenu ();
		token_menu -> Append (4101, _T ("Rotate right	]"));
		token_menu -> Append (4102, _T ("Rotate left	["));
		token_menu -> Append (4103, _T ("Lock tokens	F9"));
		bar -> Append (token_menu, _T ("Token"));

		SetMenuBar (bar);
	}
	~ BoardFrame (void) {
	}
	void OnRotateRight (wxCommandEvent & event) {board -> OnRotateRight (event);}
	void OnRotateLeft (wxCommandEvent & event) {board -> OnRotateLeft (event);}
	void OnNewBoard (wxCommandEvent & event) {board -> OnNewBoard (event);}
	void OnQuit (wxCommandEvent & event) {if (wxYES == wxMessageBox (_T ("EXIT?"), _T ("INFO"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION, this)) delete this;}
	void OnGridMenu (wxCommandEvent & event) {board -> OnGridMenu (event);}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(BoardFrame, wxFrame)
EVT_MENU(4101, BoardFrame :: OnRotateRight)
EVT_MENU(4102, BoardFrame :: OnRotateLeft)
EVT_MENU(4201, BoardFrame :: OnNewBoard)
EVT_MENU(4304, BoardFrame :: OnQuit)
EVT_MENU(4401, BoardFrame :: OnGridMenu)
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

