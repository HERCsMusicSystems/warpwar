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

class BoardToken {
public:
	wxBitmap token;
	wxBitmap toDraw;
	wxPoint position;
	wxPoint positionShift;
	wxSize token_size;
	int choosenRotation; // rotation for token, different grids for grid (square, vertical hex 1 and 2, horizontal hex 1 and 2
	bool isGrid;
	bool isSelectable;
	int gridSide;
	wxSize gridSize;
	wxPoint gridStart;
	bool gridIndexing;
	void buildSquareGrid (void) {
		token_size = wxSize (gridSide * gridSize . x + 1, gridSide * gridSize . y + 1);
		toDraw = wxBitmap (token_size . x, token_size . y); //wxBitmap (gridSide * gridSize . x + 1, gridSide * gridSize . y + 1);
		wxMemoryDC gridDC (toDraw);
		gridDC . SetBackground (* wxBLACK);
		gridDC . Clear ();
		gridDC . SetTextForeground (wxColour (255, 255, 255));
		wxFont f = gridDC . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (8);
		gridDC . SetFont (f);
		gridDC . SetPen (wxPen (wxColour (255, 255, 255)));
		for (int x = 0; x <= gridSize . x; x++) {
			gridDC . DrawLine (x * gridSide, 0, x * gridSide, gridSize . y * gridSide);
		}
		for (int y = 0; y <= gridSize . y; y++) {
			gridDC . DrawLine (0, y * gridSide, gridSize . x * gridSide, y * gridSide);
		}
		if (gridIndexing) {
			for (int x = 0; x < gridSize . x; x++) {
				for (int y = 0; y < gridSize . y; y++) {
					gridDC . DrawText (wxString :: Format (_T ("%02i%02i"), x + gridStart . x, y + gridStart . y), x * gridSide + 1, y * gridSide + 1);
				}
			}
		}
		toDraw . SetMask (new wxMask (toDraw, * wxBLACK));
	}
	void buildVerticalHexGrid (bool initial) {
		double gdrs = (double) gridSide * 0.5;
		double H = gdrs * 0.866025404;
		double half = gdrs * 0.5;
		token_size = wxSize (gdrs * 1.5 * (double) gridSize . x + half + 1, H * 2.0 * gridSize . y + 1 + H);
		toDraw = wxBitmap (token_size . x, token_size . y);
		wxMemoryDC gridDC (toDraw);
		gridDC . SetBackground (* wxBLACK);
		gridDC . Clear ();
		gridDC . SetTextForeground (wxColour (255, 255, 255));
		wxFont f = gridDC . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (8);
		gridDC . SetFont (f);
		gridDC . SetPen (wxPen (wxColour (255, 255, 255)));
		double vertical_shift = initial ? H : 0.0;
		for (int x = 0; x < gridSize . x; x++) {
			double xx = half + (double) x * gdrs * 1.5;
			double yy = vertical_shift;
			if (vertical_shift == 0.0 && x < gridSize . x - 1) gridDC . DrawLine (xx + gdrs, yy, xx + gdrs + half, yy + H);
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
			if (vertical_shift != 0.0 && x < gridSize . x - 1) gridDC . DrawLine (xx + gdrs, yy, xx + gdrs + half, yy - H);
			vertical_shift = vertical_shift == 0.0 ? H : 0.0;
		}
		toDraw . SetMask (new wxMask (toDraw, * wxBLACK));
	}
	void buildHorizontalHexGrid (bool initial) {
		double gdrs = (double) gridSide * 0.5;
		double H = gdrs * 0.866025404;
		double half = gdrs * 0.5;
		token_size = wxSize (H * 2.0 * gridSize . x + 1 + H, gdrs * 1.5 * (double) gridSize . y + half + 1);
		toDraw = wxBitmap (token_size . x, token_size . y);
		wxMemoryDC gridDC (toDraw);
		gridDC . SetBackground (* wxBLACK);
		gridDC . Clear ();
		gridDC . SetTextForeground (wxColour (255, 255, 255));
		wxFont f = gridDC . GetFont ();
		f . SetFaceName (_T ("arial"));
		f . SetPointSize (8);
		gridDC . SetFont (f);
		gridDC . SetPen (wxPen (wxColour (255, 255, 255)));
		double horizontal_shift = initial ? H : 0.0;
		for (int y = 0; y < gridSize . y; y++) {
			double xx = horizontal_shift;
			double yy = half + (double) y * gdrs * 1.5;
			if (horizontal_shift == 0.0 && y < gridSize . y - 1) gridDC . DrawLine (xx, yy + gdrs, xx + H, yy + gdrs + half);
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
			if (horizontal_shift != 0.0 && y < gridSize . y - 1) gridDC . DrawLine (xx, yy + gdrs, xx - H, yy + gdrs + half);
			horizontal_shift = horizontal_shift == 0.0 ? H : 0.0;
		}
		toDraw . SetMask (new wxMask (toDraw, * wxBLACK));
	}
	void buildGrid (void) {
		if (gridSize . x < 1) gridSize . x = 1;
		if (gridSize . y < 1) gridSize . y = 1;
		switch (choosenRotation) {
		case 0: buildSquareGrid (); break;
		case 1: buildVerticalHexGrid (false); break;
		case 2: buildVerticalHexGrid (true); break;
		case 3: buildHorizontalHexGrid (false); break;
		case 4: buildHorizontalHexGrid (true); break;
		default: buildSquareGrid (); break;
		}
	}
	void rotate (int angle) {
		if (isGrid) {
			this -> choosenRotation = angle;
			buildGrid ();
			return;
		}
		while (angle > 360) angle -= 360;
		while (angle < 0) angle += 360;
		toDraw = wxBitmap (token . ConvertToImage () . Rotate (M_PI * (double) angle / 180.0, wxPoint (token . GetWidth () / 2, token . GetHeight () / 2)));
		token_size = wxSize (toDraw . GetWidth (), toDraw . GetHeight ());
		positionShift = wxPoint ((token . GetWidth () - toDraw . GetWidth ()) / 2, (token . GetHeight () - toDraw . GetHeight ()) / 2);
		choosenRotation = angle;
	}
	void rotateRight (void) {
		if (isGrid) {
			choosenRotation++;
			if (choosenRotation > 4) choosenRotation = 0;
			buildGrid ();
			return;
		}
		int rotations [] = {315, 300, 270, 240, 225, 210, 180, 150, 135, 120, 90, 60, 45, 30, 0};
		for (int ind = 0; ind < 15; ind++) {
			if (choosenRotation > rotations [ind]) return rotate (rotations [ind]);
		}
		rotate (330);
	}
	void rotateLeft (void) {
		if (isGrid) {
			choosenRotation--;
			if (choosenRotation < 0) choosenRotation = 4;
			buildGrid ();
			return;
		}
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
		dc . DrawBitmap (toDraw, position + positionShift, true);
		return next;
	}
	BoardToken * drawBorder (wxDC & dc) {
		dc . DrawRectangle (position + positionShift, token_size);
		return next;
	}
	BoardToken * hitFind (wxPoint position) {
		if (isSelectable
			&& position . x >= this -> position . x
			&& position . x < this -> position . x + token_size . GetWidth ()
			&& position . y >= this -> position . y
			&& position . y < this -> position . y + token_size . GetHeight ()
			) return this;
		if (next == 0) return 0;
		return next -> hitFind (position);
	}
	BoardToken (wxString file_name, wxPoint position, BoardToken * next = 0) {
		this -> isGrid = false;
		this -> isSelectable = true;
		this -> next = next;
		token . LoadFile (file_name, wxBITMAP_TYPE_PNG);
		this -> position = position;
		position -= wxPoint (token . GetWidth () / 2, token . GetHeight () / 2);
		rotate (0);
	}
	BoardToken (wxPoint position, BoardToken * next = 0) {
		this -> isGrid = true;
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
	void changeGridSide (int change) {
		if (! isGrid) return;
		gridSide += change;
		if (gridSide < 10) gridSide = 10;
		buildGrid ();
	}
	void setIndexedGrid (void) {if (! isGrid) return; gridIndexing = ! gridIndexing; buildGrid ();}
	void changeGridIndexing (wxPoint change) {if (! isGrid) return; gridStart += change; buildGrid ();}
	void changeRows (wxSize change) {if (! isGrid) return; gridSize += change; buildGrid ();}
	BoardToken * next;
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
	wxBitmap board;
	wxPoint boardLocation;
	BoardToken * tokens;
	wxPoint lastRightClickPosition;
	BoardToken * dragToken;
	wxPoint capturedPosition;
	bool moveGrid;
	bool moveBoard;
	bool moveTokens;
	BoardWindow (wxWindow * parent, wxWindowID id) : wxWindow (parent, id) {
		moveGrid = moveBoard = moveTokens = true;
		boardLocation = wxPoint (0, 0);
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		tokens = 0;
		dragToken = 0;
		lastRightClickPosition = capturedPosition = boardLocation = wxPoint (10, 10);
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
//		dc . DrawBitmap (grid, gridLocation . x, gridLocation . y, true);
		//dc . Blit (0, 0, 300, 200, & gridDC, 0, 0);
		// draw tokens
		BoardToken * tkp = tokens;
		while (tkp != 0) {tkp = tkp -> draw (dc);}
		// draw selected square
		if (dragToken != 0) {
			dc . SetPen (wxPen (wxColour (255, 255, 255), 1, wxDOT));
			dc . SetBrush (* wxTRANSPARENT_BRUSH);
			dragToken -> drawBorder (dc);
		}
	}
	void OnLeftDown (wxMouseEvent & event) {
		CaptureMouse ();
		capturedPosition = event . GetPosition ();
		if (tokens == 0) {dragToken = 0; return;}
		if (dragToken != 0) {
			if (dragToken -> next == 0) dragToken = 0;
			else dragToken = dragToken -> next -> hitFind (capturedPosition);
		}
		if (dragToken == 0) {
			dragToken = tokens -> hitFind (capturedPosition);
		}
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
//			if (moveGrid) gridLocation += delta;
		} else dragToken -> move (delta);
		capturedPosition = p;
		Refresh ();
	}
	void OnRightDown (wxMouseEvent & event) {
		lastRightClickPosition = event . GetPosition ();
		dragToken = tokens == 0 ? 0 : tokens -> hitFind (lastRightClickPosition);
		wxMenu menu;
		if (dragToken == 0) menu . Append (4001, _T ("New Token"));
		if (dragToken == 0) menu . Append (4002, _T ("New Grid"));
		if (dragToken != 0) menu . Append (4101, _T ("Rotate right"));
		if (dragToken != 0) menu . Append (4102, _T ("Rotate left"));
		PopupMenu (& menu, lastRightClickPosition);
	}
	void OnNewToken (wxCommandEvent & event) {
		wxFileDialog picker (this);
		picker . SetWildcard (_T ("PNG pictures (*.png)|*.png"));
		if (picker . ShowModal () == wxID_OK) {
			dragToken = tokens = new BoardToken (picker . GetDirectory () + _T ("/") + picker . GetFilename (), lastRightClickPosition, tokens);
			Refresh ();
		}
	}
	void insertNewToken (wxPoint location, wxString file_name) {
		dragToken = tokens = new BoardToken (file_name, location, tokens);
		Refresh ();
	}
	void OnNewGrid (wxCommandEvent & event) {dragToken = tokens = new BoardToken (lastRightClickPosition, tokens); Refresh ();}
	void rotateRight (void) {
		if (dragToken != 0) dragToken -> rotateRight ();
		Refresh ();
	}
	void OnRotateRight (wxCommandEvent & event) {rotateRight ();}
	void rotateLeft (void) {
		if (dragToken != 0) dragToken -> rotateLeft ();
		Refresh ();
	}
	void OnRotateLeft (wxCommandEvent & event) {rotateLeft ();}
	void OnNewBoard (wxCommandEvent & event) {
		boardLocation = wxPoint (10, 10);
		wxFileDialog picker (this);
		picker . SetWildcard (_T ("PNG pictures (*.png)|*.png"));
		if (picker . ShowModal () == wxID_OK) {
			board . LoadFile (picker . GetDirectory () + _T ("/") + picker . GetFilename (), wxBITMAP_TYPE_PNG);
			Refresh ();			
		}
	}
	void changeGridSide (int change) {
		if (dragToken == 0) return;
		dragToken -> changeGridSide (change);
		Refresh ();
	}
	void setIndexedGrid (void) {
		if (dragToken == 0) return;
		dragToken -> setIndexedGrid ();
		Refresh ();
	}
	void changeGridIndexing (wxPoint change) {
		if (dragToken == 0) return;
		dragToken -> changeGridIndexing (change);
		Refresh ();
	}
	void changeRows (wxSize change) {
		if (dragToken == 0) return;
		dragToken -> changeRows (change);
		Refresh ();
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
EVT_MENU(4002, BoardWindow :: OnNewGrid)
EVT_MENU(4101, BoardWindow :: OnRotateRight)
EVT_MENU(4102, BoardWindow :: OnRotateLeft)
END_EVENT_TABLE()

class BoardFrame : public wxFrame {
public:
	int gridControlType;
	BoardWindow * board;
	BoardFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("TABLE TOP"), BOARD_POSITION, BOARD_SIZE) {
		gridControlType = 1;
		board = new BoardWindow (this, -1);
		board -> SetDropTarget (new FileReceiver (this));
		board -> SetFocus ();

		wxMenuBar * bar = new wxMenuBar ();

		wxMenu * file_menu = new wxMenu ();
		file_menu -> Append (6101, _T ("EXIT	Q"));
		bar -> Append (file_menu, _T ("File"));

		wxMenu * control_menu = new wxMenu ();
		control_menu -> AppendRadioItem (6001, _T ("Grid size	F5"));
		control_menu -> AppendRadioItem (6002, _T ("Grid indexing	F6"));
		control_menu -> AppendRadioItem (6003, _T ("Grid cell size	F7"));
		control_menu -> AppendRadioItem (6004, _T ("Rotation	F8"));
		control_menu -> Append (6205, _T ("Change Indexing	I"));
		bar -> Append (control_menu, _T ("Control"));

//		wxMenu * grid_menu = new wxMenu ();
//		grid_menu -> AppendRadioItem (6201, _T ("No grid	F1"));
//		grid_menu -> AppendRadioItem (6202, _T ("Square grid	F2"));
//		grid_menu -> AppendRadioItem (6203, _T ("Vertical Hex grid	F3"));
//		grid_menu -> AppendRadioItem (6204, _T ("Horizontal Hex grid	F4"));
//		grid_menu -> AppendCheckItem (6205, _T ("Indexed	I"));
//		bar -> Append (grid_menu, _T ("Grid"));

		SetMenuBar (bar);

//		bar -> Check (6205, board -> indexedGrid);
		
	}
	~ BoardFrame (void) {
	}
	void OnRotateRight (wxCommandEvent & event) {board -> OnRotateRight (event);}
	void OnRotateLeft (wxCommandEvent & event) {board -> OnRotateLeft (event);}
	void OnNewBoard (wxCommandEvent & event) {board -> OnNewBoard (event);}
	void OnQuit (wxCommandEvent & event) {OnEscape ();}
	//void OnEscape (void) {if (wxYES == wxMessageBox (_T ("EXIT?"), _T ("INFO"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION, this)) delete this;}
	void OnEscape (void) {delete this;}
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
		default: break;
		}
	}
	void OnGridSizeControl (wxCommandEvent & event) {gridControlType = 1;}
	void OnGridIndexControl (wxCommandEvent & event) {gridControlType = 2;}
	void OnGridCellSizeControl (wxCommandEvent & event) {gridControlType = 3;}
	void OnRotateControl (wxCommandEvent & event) {gridControlType = 4;}
//	void OnNoGrid (wxCommandEvent & event) {setGridType (0);}
//	void OnSquareGrid (wxCommandEvent & event) {setGridType (1);}
//	void OnVerticalHexGrid (wxCommandEvent & event) {setGridType (2);}
//	void OnHorizontalHexGrid (wxCommandEvent & event) {setGridType (3);}
	void OnIndexed (wxCommandEvent & event) {
		if (board == 0) return;
		wxMenuBar * bar = GetMenuBar ();
		if (bar == 0) return;
		board -> setIndexedGrid ();
	}
	void insertNewToken (wxPoint location, wxString file_name) {
		if (board == 0) return;
		board -> insertNewToken (location, file_name);
	}
private:
	DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(BoardFrame, wxFrame)
EVT_MENU(6001, BoardFrame :: OnGridSizeControl)
EVT_MENU(6002, BoardFrame :: OnGridIndexControl)
EVT_MENU(6003, BoardFrame :: OnGridCellSizeControl)
EVT_MENU(6004, BoardFrame :: OnRotateControl)
EVT_MENU(6101, BoardFrame :: OnQuit)
//EVT_MENU(6201, BoardFrame :: OnNoGrid)
//EVT_MENU(6202, BoardFrame :: OnSquareGrid)
//EVT_MENU(6203, BoardFrame :: OnVerticalHexGrid)
//EVT_MENU(6204, BoardFrame :: OnHorizontalHexGrid)
EVT_MENU(6205, BoardFrame :: OnIndexed)
//EVT_MENU(5001, BoardFrame :: OnArrow)
//EVT_MENU(5002, BoardFrame :: OnArrow)
//EVT_MENU(5003, BoardFrame :: OnArrow)
//EVT_MENU(5004, BoardFrame :: OnArrow)
END_EVENT_TABLE()

BoardFrame * boardFrame = 0;

FileReceiver :: FileReceiver (BoardFrame * frame) {this -> frame = frame;}

bool FileReceiver :: OnDropFiles (wxCoord x, wxCoord y, const wxArrayString & files) {
	if (frame == NULL) return true;
	if ((int) files . GetCount () < 1) return true;
	for (int ind = 0; ind < (int) files . GetCount (); ind++) {
		frame -> insertNewToken (wxPoint (x, y) + wxPoint (20 *ind, 20 * ind), files [ind]);
	}
	return true;
}

class BoardClass : public wxApp {
public:
	int previous_key_down;
	bool OnInit (void) {
		previous_key_down = -1;
		wxInitAllImageHandlers ();
		(boardFrame = new BoardFrame (0)) -> Show ();
		return true;
	}
	int OnExit (void) {
		return wxApp :: OnExit ();
	}
	int FilterEvent (wxEvent & event) {
		if (boardFrame == 0) return -1;
		if (event . GetEventType () != wxEVT_KEY_DOWN) return -1;
		int key = ((wxKeyEvent &) event) . GetKeyCode ();
		if (key == previous_key_down) {previous_key_down = -1; return -1;}
		previous_key_down = key;
		switch (key) {
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_UP:
		case WXK_DOWN:
			boardFrame -> OnArrow (key);
			break;
		default: break;
		}
		return -1;
	}
};

IMPLEMENT_APP(BoardClass);

