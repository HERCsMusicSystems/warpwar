
#define PROTECT

#include "hercs_prolog_sdk.h"

#include "wx/wx.h"
#include "wx/dcbuffer.h"

class tank {
public:
	double x, y;
	tank * next;
	tank * locate (double x, double y) {
		this -> x = x;
		this -> y = y;
		return this;
	}
	tank (tank * next) {
		this -> next = next;
	}
	tank (void) {
		this -> next = 0;
	}
	~ tank (void) {
		if (next == 0) return;
		delete next;
	}
};

static tank * tankroot = 0;

static tank * insert_tank (double x, double y) {
	return tankroot = (new tank (tankroot)) -> locate (x, y);
}

static void remove_tanks (void) {
	if (tankroot != 0) delete tankroot;
}

static double current_transformation [9];
static double current_position [3];
static int previous_x = 0;
static int previous_y = 0;
static int perspective_centre = -2048;
static int perspective_screen = -1024;
static int centre_x = 0;
static int centre_y = 0;

static double transformation_stack [128] [14];
static int stack_pointer = 0;
static void PushTransformation (void) {
	if (stack_pointer >= 127) return;
	for (int ind = 0; ind < 9; ind++) {transformation_stack [stack_pointer] [ind] = current_transformation [ind];}
	for (int ind = 0; ind < 3; ind++) {transformation_stack [stack_pointer] [9 + ind] = current_position [ind];}
	transformation_stack [stack_pointer] [12] = (double) previous_x;
	transformation_stack [stack_pointer] [13] = (double) previous_y;
	stack_pointer++;
}
static void PopTransformation (void) {
	if (stack_pointer < 1) return;
	stack_pointer--;
	for (int ind = 0; ind < 9; ind++) {current_transformation [ind] = transformation_stack [stack_pointer] [ind];}
	for (int ind = 0; ind < 3; ind++) {current_position [ind] = transformation_stack [stack_pointer] [9 + ind];}
	previous_x = (int) transformation_stack [stack_pointer] [12];
	previous_y = (int) transformation_stack [stack_pointer] [13];
}

static void reset_transformation (double * t) {
	t [0] = t [4] = t [8] = 1.0;
	t [1] = t [2] = t [3] = t [5] = t [6] = t [7] = 0.0;
}
static void reset_position (double * p) {p [0] = p [1] = p [2] = 0.0;}
static void reset_transformation (void) {
	reset_transformation (current_transformation);
	reset_position (current_position);
}

static void multiply_transformations (double * d1, double * d2, double * res) {
	double t [9];
	for (int y = 0; y < 9; y += 3) {
		for (int x = 0; x < 3; x++) {
			t [y + x] = d1 [y + 0] * d2 [x + 0] + d1 [y + 1] * d2 [x + 3] + d1 [y + 2] * d2 [x + 6];
		}
	}
	for (int ind = 0; ind < 9; ind++) res [ind] = t [ind];
}

static void rollx (double angle) {
	double roll [] = {1.0, 0.0, 0.0, 0.0, cos (angle), - sin (angle), 0.0, sin (angle), cos (angle)};
	multiply_transformations (roll, current_transformation, current_transformation);
}

static void rolly (double angle) {
	double roll [] = {cos (angle), 0.0, - sin (angle), 0.0, 1.0, 0.0, sin (angle), 0.0, cos (angle)};
	multiply_transformations (roll, current_transformation, current_transformation);
}

static void rollz (double angle) {
	double roll [] = {cos (angle), - sin (angle), 0.0, sin (angle), cos (angle), 0.0, 0.0, 0.0, 1.0};
	multiply_transformations (roll, current_transformation, current_transformation);
}

static void move (double x, double y, double z) {
	current_position [0] += x * current_transformation [0] + y * current_transformation [1] + z * current_transformation [2];
	current_position [1] += x * current_transformation [3] + y * current_transformation [4] + z * current_transformation [5];
	current_position [2] += x * current_transformation [6] + y * current_transformation [7] + z * current_transformation [8];
}

static void move_to (double x, double y, double z) {
	current_position [0] = x * current_transformation [0] + y * current_transformation [1] + z * current_transformation [2];
	current_position [1] = x * current_transformation [3] + y * current_transformation [4] + z * current_transformation [5];
	current_position [2] = x * current_transformation [6] + y * current_transformation [7] + z * current_transformation [8];
}

static double perspective (double x, double z) {
	if (z < perspective_screen) z = perspective_screen;
//	double d1 = z - (double) perspective_centre;
//	double d2 = (double) - perspective_screen;
//	double direction = x / d1;
//	return direction * d2;
	return x * (double) - perspective_screen / (z - (double) perspective_centre);
}

static void MoveBy (double x, double y, double z) {
	move (x, - y, z);
	previous_x = perspective (current_position [0], current_position [2]);
	previous_y = perspective (current_position [1], current_position [2]);
}

static void MoveTo (double x, double y, double z) {
	move_to (x, - y, z);
	previous_x = perspective (current_position [0], current_position [2]);
	previous_y = perspective (current_position [1], current_position [2]);
}

static void Draw (wxDC & dc, double x, double y, double z) {
	move (x, - y, z);
	double cx = perspective (current_position [0], current_position [2]);
	double cy = perspective (current_position [1], current_position [2]);
	dc . DrawLine (centre_x + previous_x, centre_y + previous_y, centre_x + cx, centre_y + cy);
	previous_x = cx;
	previous_y = cy;
}

static void DrawTo (wxDC & dc, double x, double y, double z) {
	move_to (x, - y, z);
	double cx = perspective (current_position [0], current_position [2]);
	double cy = perspective (current_position [1], current_position [2]);
	dc . DrawLine (centre_x + previous_x, centre_y + previous_y, centre_x + cx, centre_y + cy);
	previous_x = cx;
	previous_y = cy;
}

#ifdef LINUX_OPERATING_SYSTEM
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
#endif

#ifdef WINDOWS_OPERATING_SYSTEM
#include "resource.h"
class resource_loader_class : public PrologResourceLoader {
public:
	char * load (char * name) {
		HRSRC resource = NULL;
		if (strcmp (name, "studio.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STUDIO_PRC), RT_RCDATA);
		if (strcmp (name, "store.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STORE_PRC), RT_RCDATA);
		if (strcmp (name, "help.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HELP_PRC), RT_RCDATA);
		if (strcmp (name, "keyboard.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (KEYBOARD_PRC), RT_RCDATA);
		if (strcmp (name, "record.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (RECORD_PRC), RT_RCDATA);
		if (strcmp (name, "scala_reader.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (SCALA_READER_PRC), RT_RCDATA);
		if (strcmp (name, "neural.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (NEURAL_PRC), RT_RCDATA);
		if (strcmp (name, "f1.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (F1_PRC), RT_RCDATA);
		if (! resource) return NULL;
		HGLOBAL loader = LoadResource (NULL, resource);
		if (! loader) return NULL;
		return (char *) LockResource (loader);
	}
} resource_loader;
#endif

class polygoon_service_class : public PrologServiceClass {
public:
	virtual void init (PrologRoot * root);
	virtual PrologNativeCode * getNativeCode (char * name);
	polygoon_service_class (void);
	~ polygoon_service_class (void);
};

class service_class_loader_class : public PrologServiceClassLoader {
public:
	PrologServiceClass * load (char * name) {
//		if (strcmp (name, "galaxy") == 0) return galaxy_return ();
		if (strcmp (name, "polygoon") == 0) return new polygoon_service_class ();
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
#ifdef LINUX_OPERATING_SYSTEM
		PrologLinuxConsole * console = new PrologLinuxConsole (10);
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
		PrologWindowsConsole * console = new PrologWindowsConsole (10);
#endif
#ifndef PROTECT
		console -> open ();
#endif
		root -> insertCommander (console);
		root -> resolution ("polygoon.prc");
		root = NULL;
		console -> stop ();
		Sleep (1000);
		Exit ();
		return Wait ();
	}
};

static int triangles_horizontal = 4;
static int triangles_vertical = 4;
static double triangle_side = 100.0;
static double triangle_grid [1024] [1024];

static int tx = -1, ty = -1;
static double txd = -1.0, tyd = -1.0;
static double getTerrainHeight (double x, double y) {
	double h = sqrt (0.75) * triangle_side;
	double initial_y = triangles_horizontal * -0.5 * triangle_side;
	double initial_x = triangles_vertical * -0.5 * h;
	tx = x - initial_x;
	if (tx >= 0) {
		tx /= h;
		txd = x - (double) tx * h;
		txd /= h;
		tx <<= 1;
	} else {tx = -127; txd = -127.0;}
	ty = y - initial_y;
	if (ty >= 0) {
		ty /= triangle_side;
		tyd = y - (double) ty * triangle_side;
		tyd /= triangle_side;
	} else {ty = -127; tyd = -127.0;}
	return 0.0;
}

static void reset_triangle_grid (int horizontal = 4, int vertical = 4, double range = 0.0) {
	triangles_horizontal = horizontal;
	triangles_vertical = vertical;
	double shift = -0.5 * range;
	for (int x = 0; x <= horizontal; x++) {
		for (int y = 0; y <= vertical; y++) {
			triangle_grid [x] [y] = shift + range * (double) rand () / (double) RAND_MAX;
		}
	}
}

static void dip_triangle_grid (int horizontal = 4, int vertical = 4, double dip = -60.0) {
	reset_triangle_grid (horizontal, vertical, 0.0);
	int half_index = triangles_vertical / 2;
	for (int ind = 0; ind <= horizontal; ind++) {
		triangle_grid [ind] [half_index] = dip;
	}
}

static void DrawTriangles (wxDC & dc, double h, double half_side, int index, int t_shift) {
	int median_index = index + 1;
	double y = triangle_grid [0] [index];
	MoveBy (0, y, 0);
	for (int ind = 1; ind <= triangles_horizontal; ind++) {
		double yy = triangle_grid [ind] [index];
		Draw (dc, 0, yy - y, triangle_side);
		y = yy;
	}
	for (int ind = triangles_horizontal - 1; ind >= 0; ind--) {
		double yy = triangle_grid [ind] [index];
		double median = triangle_grid [ind + t_shift] [median_index];
		Draw (dc, h, median - y, - half_side);

//		PushTransformation ();
//		double a1 = h, a2 = median - y, a3 = - half_side;
//		double b1 = - h, b2 = yy - median, b3 = - half_side;
//		double k1 = a2 * b3 - a3 * b2, k2 = a3 * b1 - a1 * b3, k3 = a1 * b2 - a2 * b1;
//		Draw (dc, k1 / (double) triangle_side, k2 / (double) triangle_side, k3 / (double) triangle_side);
//		PopTransformation ();

		Draw (dc, - h, yy - median, - half_side);
		y = yy;
	}
}

class GridWindow : public wxWindow {
public:
	bool resizing;
	int captured_x, captured_y;
	int bitmap_position_x, bitmap_position_y;
	GridWindow (wxWindow * parent) : wxWindow (parent, -1) {
		SetBackgroundStyle (wxBG_STYLE_CUSTOM);
		resizing = false;
		captured_x = captured_y = 0;
		bitmap_position_x = bitmap_position_y = 0;
		new PrologThread ();
	}
	void OnPaint (wxPaintEvent & event) {
		double rotation = 3.1415 * (double) bitmap_position_x / 1024.0;
		double elevation = 3.1415 * (double) bitmap_position_y / 1024.0;
		wxSize ws = GetSize ();
		centre_x = ws . x / 2;
		centre_y = ws . y / 2;
		previous_x = previous_y = 0.0;
		wxBufferedPaintDC dc (this);
		dc . SetBackground (wxBrush (wxColour (0, 0, 0)));
		dc . Clear ();
//		dc . SetPen (wxPen (wxColour (255, 255, 0)));
		reset_transformation ();
		//moveTo (centre_x, centre_y, 0);
		//moveTo (0, 0, 0);
		rolly (rotation);
		rollx (elevation);
//		DrawTo (dc, 400, 0, 0);
//		DrawTo (dc, 400, 400, 0);
//		DrawTo (dc, 0, 400, 0);
//		DrawTo (dc, 0, 0, 0);
//		DrawTo (dc, 400, 0, 0);
//		DrawTo (dc, -400, 100, 0);
//		DrawTo (dc, 0, -100, 0);
//		DrawTo (dc, 0, 0, 200);
//		DrawTo (dc, 0, 100, -200);
//		MoveTo (0, -100, 200);
//		DrawTo (dc, 400, 0, -200);
		dc . SetPen (wxPen (wxColour (255, 255, 0)));
		// 1 = 0.5^2 + h^2 => h^2 = 1 - .5^2 = 1 - 0.25 = 0.75
		double h = sqrt (0.75) * triangle_side;
		double half_side = triangle_side * 0.5;
		MoveBy (- h * triangles_vertical * 0.5, 0, - triangle_side * triangles_horizontal * 0.5);
		double hh = - half_side;
		PushTransformation ();
		int t_shift = 1;
		for (int ind = 0; ind < triangles_vertical; ind++) {
			PushTransformation (); DrawTriangles (dc, h, half_side, ind, t_shift); PopTransformation ();
			MoveBy (h, 0, hh);
			hh = - hh;
			t_shift = t_shift == 0 ? 1 : 0;
		}
		PopTransformation ();
		dc . SetPen (wxPen (wxColour (255, 0, 0)));
		dc . SetTextForeground (wxColour (255, 0, 0));
		tank * t = tankroot;
		int tank_index = 0;
		while (t != 0) {
			MoveTo (t -> x, 0, t -> y);
			double height = getTerrainHeight (t -> x, t -> y);
			dc . DrawText (wxString :: Format (_T ("%i [%i %i] <%d %d>"), tank_index++, tx, ty, txd, tyd), centre_x + previous_x, centre_y + previous_y);
			Draw (dc, 0, 100, 0);
			t = t -> next;
		}
	}
	void OnMouse (wxMouseEvent & event) {
		if (! event . Dragging ()) return;
		wxPoint px = event . GetPosition ();
//		if (offseting_wave) {
//			if (px . y < captured_y) {wave_offset += 10; if (wave_offset + 128 >= max_yy) wave_offset = max_yy + 127;}
//			if (px . y > captured_y) {wave_offset -= 10; if (wave_offset < 0) wave_offset = 0;}
//			if (px . x > captured_x) {wave_offset += 10; if (wave_offset + 128 >= max_yy) wave_offset = max_yy + 127;}
//			if (px . x < captured_x) {wave_offset -= 10; if (wave_offset < 0) wave_offset = 0;}
//			captured_x = px . x;
//			captured_y = px . y;
//			Refresh ();
//			return;
//		}
		if (resizing) {
//			double rotation = 3.1415 * (double) bitmap_position_x / 1024.0;
//			double x = (double) (px . x - captured_x);
//			double y = (double) (px . y - captured_y);
//			vertical += x * cos (rotation) - y * sin (rotation);
//			horizontal -= y * cos (rotation) + x * sin (rotation);
		} else {bitmap_position_x += px . x - captured_x; bitmap_position_y += px . y - captured_y;}
		captured_x = px . x;
		captured_y = px . y;
		Refresh ();
	}
	void OnLeftDown (wxMouseEvent & event) {
		resizing = false;
		CaptureMouse ();
		wxPoint px = event . GetPosition ();
		captured_x = px . x; captured_y = px . y;
	}
	void OnLeftUp (wxMouseEvent & event) {
		ReleaseMouse ();
	}
	void OnRightDown (wxMouseEvent & event) {
		resizing = true;
		CaptureMouse ();
		wxPoint px = event . GetPosition ();
		captured_x = px . x; captured_y = px . y;
	}
	void OnRightUp (wxMouseEvent & event) {
		ReleaseMouse ();
	}
	void OnWheel (wxMouseEvent & event) {perspective_centre += (int) ((double) event . GetWheelRotation () * 0.1); Refresh ();}
private: DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(GridWindow, wxWindow)
EVT_PAINT(GridWindow :: OnPaint)
EVT_LEFT_UP(GridWindow :: OnLeftUp)
EVT_LEFT_DOWN(GridWindow :: OnLeftDown)
EVT_RIGHT_UP(GridWindow :: OnRightUp)
EVT_RIGHT_DOWN(GridWindow :: OnRightDown)
EVT_MOTION(GridWindow :: OnMouse)
EVT_MOUSEWHEEL(GridWindow :: OnWheel)
END_EVENT_TABLE()

class grid_class : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * h = parameters -> getLeft ();
		if (! h -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * v = parameters -> getLeft ();
		if (! v -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		double range = 0.0;
		if (parameters -> isPair ()) {
			PrologElement * r = parameters -> getLeft ();
			if (r -> isDouble ()) range = r -> getDouble ();
			if (r -> isInteger ()) range = (double) r -> getInteger ();
		}
		reset_triangle_grid (h -> getInteger (), v -> getInteger (), range);
		return true;
	}
};

class dip_class : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * h = parameters -> getLeft ();
		if (! h -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * v = parameters -> getLeft ();
		if (! v -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		double dip = 0.0;
		if (parameters -> isPair ()) {
			PrologElement * d = parameters -> getLeft ();
			if (d -> isDouble ()) dip = d -> getDouble ();
			if (d -> isInteger ()) dip = (double) d -> getInteger ();
		}
		dip_triangle_grid (h -> getInteger (), v -> getInteger (), dip);
		return true;
	}
};

class insert_tank_class : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * x = parameters -> getLeft ();
		if (! x -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * y = parameters -> getLeft ();
		if (! y -> isInteger ()) return false;
		insert_tank (x -> getInteger (), y -> getInteger ());
		return true;
	}
};

void polygoon_service_class :: init (PrologRoot * root) {}
PrologNativeCode * polygoon_service_class :: getNativeCode (char * name) {
	if (strcmp (name, "grid") == 0) return new grid_class ();
	if (strcmp (name, "dip") == 0) return new dip_class ();
	if (strcmp (name, "insert_tank") == 0) return new insert_tank_class ();
	return NULL;
}
polygoon_service_class :: polygoon_service_class (void) {}
polygoon_service_class :: ~ polygoon_service_class (void) {}

class GridFrame : public wxFrame {
public:
#ifndef PROTECT
	GridFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("GRID"), wxPoint (100, 100), wxSize (1024, 512)) {new GridWindow (this);}
#else
	GridFrame (wxWindow * parent) : wxFrame (parent, -1, _T ("GRID"), wxPoint (1450, 900), wxSize (200, 100)) {new GridWindow (this);}
#endif
	~ GridFrame (void) {}
};

class gridder : public wxApp {
	bool OnInit (void) {
		reset_triangle_grid (4, 4, 60);
		//dip_triangle_grid ();
		(new GridFrame (NULL)) -> Show ();
		return true;
	}
	int OnExit (void) {
		remove_tanks ();
		return wxApp :: OnExit ();
	}
};

IMPLEMENT_APP(gridder);
