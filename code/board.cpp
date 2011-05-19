///////////////////////////////////////
// Copyright (C) 2011 Robert P. Wolf //
//        ALL RIGHTS RESERVED        //
///////////////////////////////////////

#define PROTECT

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

class BoardWindow : public wxWindow {
public:
	wxBitmap board;
	wxPoint boardLocation;
	wxPoint gridLocation;
	BoardToken * tokens;
	wxPoint lastRightClickPosition;
	BoardToken * dragToken;
	wxPoint capturedPosition;
	wxBitmap grid;
	int gridType;
	int gridSide;
	int gridWidth, gridHeight, gridStartX, gridStartY;
	bool gridIndexing;
	bool moveGrid;
	bool moveBoard;
	bool moveTokens;
	void buildSquareGrid (void) {
		grid = wxBitmap (gridSide * gridWidth + 1, gridSide * gridHeight + 1);
		wxMemoryDC gridDC (grid);
		gridDC . SetBackground (* wxBLACK);
		gridDC . Clear ();
		gridDC . SetPen (wxPen (wxColour (255, 255, 255)));
		for (int x = 0; x <= gridWidth; x++) {
			gridDC . DrawLine (x * gridSide, 0, x * gridSide, gridHeight * gridSide);
		}
		for (int y = 0; y <= gridHeight; y++) {
			gridDC . DrawLine (0, y * gridSide, gridWidth * gridSide, y * gridSide);
		}
		grid . SetMask (new wxMask (grid, * wxBLACK));
	}
	void buildVerticalHexGrid (void) {
		double gdrs = (double) gridSide * 0.5;
		double H = gdrs * 0.866025404;
		double half = gdrs * 0.5;
		grid = wxBitmap (gridSide * gridWidth + 1, H * 2.0 * gridHeight + 30);
		wxMemoryDC gridDC (grid);
		gridDC . SetBackground (* wxBLACK);
		gridDC . Clear ();
		gridDC . SetPen (wxPen (wxColour (255, 255, 255)));
		int x = 0;
		for (int y = 0; y < gridHeight; y++) {
			int xx = (int) (half + (double) x * gdrs);
			int yy = (int) ((double) y * H * 2.0);
			gridDC . DrawLine (xx, yy, xx + gdrs, yy);
			gridDC . DrawLine (xx, yy, xx - half, yy + H);
			gridDC . DrawLine (xx - half, yy + H, xx, yy + H + H);
		}
	}
	void buildGrid (void) {
		if (gridWidth < 1) gridWidth = 1;
		if (gridHeight < 1) gridHeight = 1;
		//buildSquareGrid ();
		buildVerticalHexGrid ();
	}
	BoardWindow (wxWindow * parent, wxWindowID id) : wxWindow (parent, id) {
		moveGrid = moveBoard = moveTokens = true;
		gridType = 2;
		gridSide = 60;
		gridWidth = gridHeight = 4;
		gridStartX = gridStartY = 0;
		gridIndexing = true;
		boardLocation = wxPoint (0, 0);
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		tokens = 0;
		dragToken = 0;
		lastRightClickPosition = capturedPosition = boardLocation = gridLocation = wxPoint (10, 10);
		//buildSquareGrid ();
		buildVerticalHexGrid ();
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
		dc . DrawBitmap (grid, gridLocation . x, gridLocation . y, true);
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
			if (moveTokens && tokens != 0) tokens -> moveAll (delta);
			if (moveBoard) boardLocation += delta;
			if (moveGrid) gridLocation += delta;
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

		wxMenu * file_menu = new wxMenu ();
		file_menu -> Append (4301, _T ("Load	F1"));
		file_menu -> Append (4302, _T ("Save	CTRL+S"));
		file_menu -> Append (4303, _T ("Save as	CTRL+A"));
		file_menu -> Append (4304, _T ("Exit	Q"));
		bar -> Append (file_menu, _T ("File"));

		wxMenu * grid_menu = new wxMenu ();
		grid_menu -> Append (4401, _T ("Show indices"));
		grid_menu -> Append (4402, _T ("Add row	DOWN"));
		grid_menu -> Append (4403, _T ("Add 4 rows	CTRL+DOWN"));
		grid_menu -> Append (4404, _T ("Remove row	UP"));
		grid_menu -> Append (4405, _T ("Remove 4 rows	CTRL+UP"));
		grid_menu -> Append (4406, _T ("Add column	RGHT"));
		grid_menu -> Append (4407, _T ("Add 4 columns	CTRL+RIGHT"));
		grid_menu -> Append (4408, _T ("Remove column	LEFT"));
		grid_menu -> Append (4409, _T ("Remove 4 columns	CTRL+LEFT"));
		grid_menu -> Append (4410, _T ("Increase size by 1 pixel"));
		grid_menu -> Append (4411, _T ("Increase size by 8 pixels"));
		grid_menu -> Append (4412, _T ("Decrease size by 1 pixel"));
		grid_menu -> Append (4413, _T ("Decrease size by 8 pixels"));
		grid_menu -> Append (4414, _T ("Increase index horizontally by 1"));
		grid_menu -> Append (4415, _T ("Increase index horizontally by 4"));
		grid_menu -> Append (4416, _T ("Decrease index horizontally by 1"));
		grid_menu -> Append (4417, _T ("Decrease index horizontally by 4"));
		grid_menu -> Append (4418, _T ("Increase index vertically by 1"));
		grid_menu -> Append (4419, _T ("Increase index vertically by 4"));
		grid_menu -> Append (4420, _T ("Decrease index vertically by 1"));
		grid_menu -> Append (4421, _T ("Decrease index vertically by 4"));
		bar -> Append (grid_menu, _T ("Grid"));

		wxMenu * board_menu = new wxMenu ();
		board_menu -> Append (4201, _T ("New board	CTRL+B"));
		board_menu -> AppendCheckItem (4202, _T ("Lock grid	F7"));
		board_menu -> AppendCheckItem (4203, _T ("Lock board	F8"));
		board_menu -> AppendCheckItem (4204, _T ("Lock tokens	F9"));
		board_menu -> Append (4205, _T ("Square grid	F4"));
		board_menu -> Append (4206, _T ("Hex grid (horizontal)	F5"));
		board_menu -> Append (4207, _T ("Hex grid (vertical)	F6"));
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
	void OnQuit (wxCommandEvent & event) {if (wxYES == wxMessageBox (_T ("EXIT?"), _T ("INFO"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION, this)) delete this;}
	void OnLockGrid (wxCommandEvent & event) {
		wxMenuBar * bar = GetMenuBar ();
		bar -> Check (4202, board -> moveGrid);
		board -> moveGrid = ! board -> moveGrid;
		Refresh ();
	}
	void OnLockBoard (wxCommandEvent & event) {
		wxMenuBar * bar = GetMenuBar ();
		bar -> Check (4203, board -> moveBoard);
		board -> moveBoard = ! board -> moveBoard;
		Refresh ();
	}
	void OnLockTokens (wxCommandEvent & event) {
		wxMenuBar * bar = GetMenuBar ();
		bar -> Check (4204, board -> moveTokens);
		board -> moveTokens = ! board -> moveTokens;
		Refresh ();
	}
	void ChangeRows (int columns, int rows) {
		board -> gridWidth += rows;
		board -> gridHeight += columns;
		board -> buildGrid ();
		Refresh ();
	}
	void OnAddRow (wxCommandEvent & event) {ChangeRows (1, 0);}
	void OnAdd4Rows (wxCommandEvent & event) {ChangeRows (4, 0);}
	void OnRemoveRow (wxCommandEvent & event) {ChangeRows (-1, 0);}
	void OnRemove4Rows (wxCommandEvent & event) {ChangeRows (-4, 0);}
	void OnAddColumn (wxCommandEvent & event) {ChangeRows (0, 1);}
	void OnAdd4Columns (wxCommandEvent & event) {ChangeRows (0, 4);}
	void OnRemoveColumn (wxCommandEvent & event) {ChangeRows (0, -1);}
	void OnRemove4Columns (wxCommandEvent & event) {ChangeRows (0, -4);}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(BoardFrame, wxFrame)
EVT_MENU(4101, BoardFrame :: OnRotateRight)
EVT_MENU(4102, BoardFrame :: OnRotateLeft)
EVT_MENU(4201, BoardFrame :: OnNewBoard)
EVT_MENU(4202, BoardFrame :: OnLockGrid)
EVT_MENU(4203, BoardFrame :: OnLockBoard)
EVT_MENU(4204, BoardFrame :: OnLockTokens)
EVT_MENU(4304, BoardFrame :: OnQuit)
EVT_MENU(4402, BoardFrame :: OnAddRow)
EVT_MENU(4403, BoardFrame :: OnAdd4Rows)
EVT_MENU(4404, BoardFrame :: OnRemoveRow)
EVT_MENU(4405, BoardFrame :: OnRemove4Rows)
EVT_MENU(4406, BoardFrame :: OnAddColumn)
EVT_MENU(4407, BoardFrame :: OnAdd4Columns)
EVT_MENU(4408, BoardFrame :: OnRemoveColumn)
EVT_MENU(4409, BoardFrame :: OnRemove4Columns)
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

