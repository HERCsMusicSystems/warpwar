#include <string.h>
#include "boarder.h"
#include "boarder_service.h"

#include <gtk/gtk.h>
#include <cairo.h>

#include "prolog_linux_console.h"

static char * edit_mode_none = "None";
static char * create_square_mode = "Create Square";
static char * create_rectangle_mode = "Create Rectangle";
static char * create_circle_mode = "Create Circle";
static char * create_ellipse_mode = "Create Ellipse";
static char * create_tetrahedron_mode = "Create Tetrahedron";
static char * create_cube_mode = "Create Cube";
static char * create_dice_mode = "Create Dice";
static char * create_octahedron_mode = "Create Octahedron";
static char * create_deltahedron_mode = "Create Deltahedron";
static char * create_deltahedron_10_mode = "Create Deltahedron 10";
static char * create_dodecahedron_mode = "Create Dodecahedron";
static char * create_icosahedron_mode = "Create Icosahedron";

static boarder * board = 0;
static bool boarder_clean = true;

extern PrologRoot * root;
extern PrologCommand * console;
static gboolean viewport_delete_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	if (board == 0) return FALSE;
	if (viewport == 0) return FALSE;
	if (viewport -> atom == 0) return FALSE;
	PrologNativeCode * machine = viewport -> atom -> getMachine ();
	viewport -> atom -> setMachine (0);
	board -> remove_viewport (viewport);
	if (machine != 0) delete machine;
	boarder_clean = false;
	return FALSE;
}
static gboolean RemoveViewportIdleCode (GtkWidget * viewport) {gtk_widget_destroy (viewport); return FALSE;}

class viewport_action : public PrologNativeCode {
public:
	PrologDirectory * directory;
	PrologAtom * location_atom;
	PrologAtom * size_atom;
	PrologAtom * position_atom;
	PrologAtom * scaling_atom;
	PrologAtom * repaint_atom;
	boarder_viewport * viewport;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (viewport == 0) return false;
		if (parameters -> isEarth ()) {
			//viewport -> atom -> unProtect ();
			viewport -> atom -> setMachine (0);
			//viewport -> atom -> unProtect ();
			g_idle_add ((GSourceFunc) RemoveViewportIdleCode, viewport -> window); //gtk_widget_destroy (viewport -> window);
			board -> remove_viewport (viewport);
			delete this;
			boarder_clean = false;
			return true;
		}
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft (); parameters = parameters -> getRight ();
		if (! atom -> isAtom ()) return false;
		if (atom -> getAtom () == location_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> location . position . x);
				parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> location . position . y);
				parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> location . size . x);
				parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> location . size . y);
				return true;
			}
			if (! parameters -> isPair ()) return false;
			PrologElement * x = parameters -> getLeft (); if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			PrologElement * y = parameters -> getLeft (); if (! y -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			PrologElement * width = parameters -> getLeft (); if (! width -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			PrologElement * height = parameters -> getLeft (); if (! height -> isInteger ()) return false; parameters = parameters -> getRight ();
			viewport -> setWindowLocation (rect (x -> getInteger (), y -> getInteger (), width -> getInteger (), height -> getInteger ()));
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == position_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> board_position . x);
				parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> board_position . y);
				return true;
			}
			if (! parameters -> isPair ()) return false;
			PrologElement * x = parameters -> getLeft (); if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			PrologElement * y = parameters -> getLeft (); if (! y -> isInteger ()) return false; parameters = parameters -> getRight ();
			viewport -> setBoardPosition (point (x -> getInteger (), y -> getInteger ()));
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == size_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> location . size . x);
				parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) viewport -> location . size . y);
				return true;
			}
			if (! parameters -> isPair ()) return false;
			PrologElement * width = parameters -> getLeft (); if (! width -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			PrologElement * height = parameters -> getLeft (); if (! height -> isInteger ()) return false; parameters = parameters -> getRight ();
			viewport -> setWindowSize (point (width -> getInteger (), height -> getInteger ()));
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == scaling_atom) {
			if (parameters -> isVar ()) {parameters -> setPair (); parameters -> getLeft () -> setDouble (viewport -> scaling); return true;}
			if (! parameters -> isPair ()) return false;
			PrologElement * scaling = parameters -> getLeft ();
			if (scaling -> isDouble ()) {viewport -> scaling = scaling -> getDouble (); boarder_clean = false; return true;}
			if (scaling -> isInteger ()) {viewport -> scaling = (int) scaling -> getInteger (); boarder_clean = false; return true;}
			return false;
		}
		if (atom -> getAtom () == repaint_atom) {gtk_widget_queue_draw (viewport -> window); return true;}
		return false;
	}
	viewport_action (PrologDirectory * directory) {
		this -> directory = directory;
		location_atom = size_atom = position_atom = scaling_atom = repaint_atom = 0;
		this -> viewport = 0;
		if (directory) {
			location_atom = directory -> searchAtom (LOCATION);
			size_atom = directory -> searchAtom (SIZE);
			position_atom = directory -> searchAtom (POSITION);
			scaling_atom = directory -> searchAtom (SCALING);
			repaint_atom = directory -> searchAtom (REPAINT);
		}
	}
};

static point click_init_point (0, 0);
static point click_point (0, 0);
static int click_button = 0;
static bool has_selection = false;
static bool moved = false;
static char * edit_mode = edit_mode_none;
static boarder_token * edited_token = 0;

static gboolean viewport_draw_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	if (viewport == 0) return FALSE;
	if (viewport -> board == 0) return FALSE;
	cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
	board -> draw (cr, viewport);
	/*if (click_button == 3 && click_init_point != click_point) {
		rect location (click_init_point * viewport -> scaling, (click_point - click_init_point) * viewport -> scaling);
		cairo_rectangle (cr, RECT (location));
		cairo_stroke (cr);
	}*/
	cairo_destroy (cr);
	return FALSE;
}
static gboolean viewport_configure_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	viewport -> location . size = point (widget -> allocation . width, widget -> allocation . height); boarder_clean = false; return FALSE;}
static gboolean window_configure_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	viewport -> location . position = point (event -> configure . x, event -> configure . y); boarder_clean = false; return FALSE;}

static void CreateDiceCommand (int order, bool extended = false) {
	if (board == 0) return;
	PrologRoot * root = board -> root;
	if (root == 0) return;
	PrologElement * location_query = root -> pair (root -> var (0),
		root -> pair (root -> atom ("boarder", "Position"),
		root -> pair (root -> integer ((int) click_init_point . x),
		root -> pair (root -> integer ((int) click_init_point . y),
		root -> earth ()))));
	PrologElement * creation_query = root -> pair (root -> atom ("boarder", "CreateDice"),
		root -> pair (root -> var (0),
		root -> pair (root -> integer (order),
		root -> earth ())));
	PrologElement * query = root -> pair (root -> earth (), root -> pair (creation_query, root -> pair (location_query, root -> earth ())));
	root -> resolution (query);
	char command [1024];
	root -> getValue (query, command, 0);
	printf ("command => %s\n", command);
	delete query;
	board -> repaint ();
}

static void CreateRectangleCommand (void);

static void create_response (char * command) {
	printf ("action [%s]\n", command);
	if (command == edit_mode_none) {edit_mode = edit_mode_none; return;}
	if (command == create_rectangle_mode) {edit_mode = create_rectangle_mode; return;}
	if (command == create_tetrahedron_mode) {CreateDiceCommand (4); edit_mode = edit_mode_none; return;}
	if (command == create_cube_mode) {CreateDiceCommand (6); edit_mode = edit_mode_none; return;}
	if (command == create_dice_mode) {CreateDiceCommand (6, true); edit_mode = edit_mode_none; return;}
	if (command == create_octahedron_mode) {CreateDiceCommand (8); edit_mode = edit_mode_none; return;}
	if (command == create_deltahedron_mode) {CreateDiceCommand (10); edit_mode = edit_mode_none; return;}
	if (command == create_deltahedron_10_mode) {CreateDiceCommand (10, true); edit_mode = edit_mode_none; return;}
	if (command == create_dodecahedron_mode) {CreateDiceCommand (12); edit_mode = edit_mode_none; return;}
	if (command == create_icosahedron_mode) {CreateDiceCommand (20); edit_mode = edit_mode_none; return;}
}
static GtkWidget * right_click_menu = 0;
static GtkWidget * AddMenu (GtkWidget * menu, char * label, void action (char *)) {
	GtkWidget * item = gtk_menu_item_new_with_label (label);
	gtk_menu_append (GTK_MENU (menu), item);
	gtk_signal_connect_object (GTK_OBJECT (item), "activate", GTK_SIGNAL_FUNC (action), (gpointer) label);
	return item;
}
static GtkWidget * AddSubMenu (GtkWidget * menu, char * label) {
	GtkWidget * sub_menu = gtk_menu_new ();
	GtkWidget * item = gtk_menu_item_new_with_label (label);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), sub_menu);
	gtk_menu_append (GTK_MENU (menu), item);
	return sub_menu;
}
static void CreateRightClickMenu (void) {
	if (right_click_menu != 0) return;
	right_click_menu = gtk_menu_new ();
	AddMenu (right_click_menu, edit_mode_none, create_response);
	AddMenu (right_click_menu, create_rectangle_mode, create_response);
	AddMenu (right_click_menu, create_square_mode, create_response);
	AddMenu (right_click_menu, create_ellipse_mode, create_response);
	AddMenu (right_click_menu, create_circle_mode, create_response);
	GtkWidget * dice_submenu = AddSubMenu (right_click_menu, create_dice_mode);
	AddMenu (dice_submenu, create_tetrahedron_mode, create_response);
	AddMenu (dice_submenu, create_cube_mode, create_response);
	AddMenu (dice_submenu, create_dice_mode, create_response);
	AddMenu (dice_submenu, create_octahedron_mode, create_response);
	AddMenu (dice_submenu, create_deltahedron_mode, create_response);
	AddMenu (dice_submenu, create_deltahedron_10_mode, create_response);
	AddMenu (dice_submenu, create_dodecahedron_mode, create_response);
	AddMenu (dice_submenu, create_icosahedron_mode, create_response);
}

static gint window_button_down_event (GtkWidget * widget, GdkEventButton * event, boarder_viewport * viewport) {
	edited_token = 0;
	moved = false;
	if (board == 0) return TRUE;
	rect area (point (event -> x, event -> y) / viewport -> scaling, point (0, 0));
	rect hit_test_area = area; hit_test_area . position = hit_test_area . position + viewport -> board_position;
	area . position . round ();
	//printf ("CLICKED [%i %g %g]\n", event -> button, area . position . x, area . position . y);
	boarder_token * token = board -> hit_test (hit_test_area);
	click_init_point = click_point = area . position;
	switch (event -> button) {
	case 1:
		board -> clear_selection ();
		has_selection = false;
		if (edit_mode == create_rectangle_mode) CreateRectangleCommand ();
		if (token) {token -> selected = true; has_selection = true;}
		break;
	case 3:
		if (token) {
			if (! board -> release_token_from_deck (token)) break;
			token -> selected = true;
			has_selection = true;
			break;
		} else {
			CreateRightClickMenu ();
			gtk_widget_show_all (right_click_menu);
			gtk_menu_popup (GTK_MENU (right_click_menu), 0, 0, 0, 0, 3, gdk_event_get_time ((GdkEvent *) event));
			break;
		}
		break;
	default: break;
	}
	board -> repaint ();
	click_button = event -> button;
	return TRUE;
}

static gint window_button_up_event (GtkWidget * widget, GdkEventButton * event, boarder_viewport * viewport) {
	edited_token = 0;
	if (board == 0) return TRUE;
	rect area (point (event -> x, event -> y) / viewport -> scaling, point (0, 0));
	area . position . round ();
	//printf ("RELEASED [%i %g %g]\n", event -> button, area . position . x, area . position . y);
	if (has_selection && click_button > 0 && moved) {
		boarder_token * deck = board -> hit_test (area);
		while (deck != 0 && ! deck -> can_insert ()) deck = deck -> hit_test_next (area);
		if (deck != 0) board -> transfer_selection_to_deck (deck);
	}
	if (click_button == 3 && click_init_point != click_point) {
		rect location (click_init_point + viewport -> board_position, click_point - click_init_point);
		location . positivise ();
		boarder_token * token = board -> hit_test (location);
		while (token != 0) {
			token -> selected = true;
			token = token -> hit_test_next (location);
		}
	}
	click_button = 0;
	board -> repaint ();
	return TRUE;
}

static gint window_button_motion_event (GtkWidget * window, GdkEventButton * event, boarder_viewport * viewport) {
	moved = true;
	if (board == 0) return TRUE;
	rect area (point (event -> x, event -> y) / viewport -> scaling, point (0, 0));
	area . position . round ();
	//printf ("MOVE [%i %g %g]\n", event -> button, area . position . x, area . position . y);
	if (edited_token == 0) {
		if (has_selection && click_button > 0) {
			board -> move_selection (area . position - click_point);
		}
		if (! has_selection && click_button == 1) {
			viewport -> board_position = viewport -> board_position - area . position + click_point;
		}
	}
	click_point = area . position;
	if (edited_token != 0) {
		point size = click_point - click_init_point;
		//if (size . x > 1.0 && size . y > 1.0)
		edited_token -> set_size (size);
	}
	if (click_button > 0) board -> repaint ();
	return TRUE;
}

void DnDreceive (GtkWidget *widget, GdkDragContext *context, gint x, gint y, 
				 GtkSelectionData *data, guint ttype, guint time, gpointer *NA) {
	gchar * ptr = (char *) data -> data;
	if (board == 0) return;
	PrologRoot * root = board -> root;
	if (root == 0) return;
	char command [4096];
	PrologElement * query = root -> earth ();
	while (strncmp (ptr, "file:///", 8) == 0) {
		ptr += 7;
		char * cp = command;
		while (* ptr >= ' ') {
			* cp++ = * ptr++;
		}
		* cp = '\0';
		query = root -> pair (root -> text (command), query);
		while (* ptr > '\0' && * ptr <= ' ') ptr++;
	}
	query = root -> pair (root -> integer ((int) y), query);
	query = root -> pair (root -> integer ((int) x), query);
	query = root -> pair (root -> atom ("DragAndDrop"), query);
	query = root -> pair (root -> earth (), root -> pair (query, root -> earth ()));
	root -> resolution (query);
	delete query;
}

gboolean DnDdrop (GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer *NA) {
	GdkAtom target_type;
	if(context -> targets) {
		target_type = GDK_POINTER_TO_ATOM (g_list_nth_data (context -> targets, 0));
		gtk_drag_get_data (widget, context, target_type, time);
	} else return FALSE;
	return TRUE;
}

void DnDleave (GtkWidget *widget, GdkDragContext *context, guint time, gpointer *NA) {
//	printf ("LEAVE\n");
}

gboolean DnDmotion (GtkWidget *widget, GdkDragContext *context, gint x, gint y, 
					GtkSelectionData *seld, guint ttype, guint time, gpointer *NA) {
	//printf ("MOTION\n");
	return TRUE;
}

/*
// In a dedicated thread:
while (...) {
    Package*  package = do_read ();  // This call is slow or blocks.
    if (package)
        g_idle_add ((GSourceFunc) process_package, package);
}

// This is called in the main thread.  Should be fast to not freeze GUI.
gboolean
process_package (Package* package)
{
    ...
    package_free (package);
}
*/

//class viewport : public PrologNativeCode {
//public:
//	PrologDirectory * directory;
//void CreateViewportCode (PrologDirectory * directory, PrologElement * parameters) {

static gboolean CreateViewportIdleCode (boarder_viewport * viewport) {
	GtkWidget * window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), viewport -> name);
	g_signal_connect (window, "delete-event", G_CALLBACK (viewport_delete_event), viewport);
	GtkWidget * drawing_area = gtk_drawing_area_new ();
	gtk_container_add (GTK_CONTAINER (window), drawing_area);
	g_signal_connect (G_OBJECT (drawing_area), "expose-event", G_CALLBACK (viewport_draw_event), viewport);
	g_signal_connect (G_OBJECT (drawing_area), "configure-event", G_CALLBACK (viewport_configure_event), viewport);
	g_signal_connect (G_OBJECT (window), "configure-event", G_CALLBACK (window_configure_event), viewport);
	gtk_widget_add_events (window, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect (G_OBJECT (window), "button_press_event", G_CALLBACK (window_button_down_event), viewport);
	g_signal_connect (G_OBJECT (window), "button_release_event", G_CALLBACK (window_button_up_event), viewport);
	g_signal_connect (G_OBJECT (window), "motion_notify_event", G_CALLBACK (window_button_motion_event), viewport);
	gtk_window_move (GTK_WINDOW (window), (int) viewport -> location . position . x, (int) viewport -> location . position . y);
	gtk_window_resize (GTK_WINDOW (window), (int) viewport -> location . size . x, (int) viewport -> location . size . y);
		
		//gtk_drag_dest_set(drawing_area,GTK_DEST_DEFAULT_ALL,NULL,0,GDK_ACTION_COPY);
		//gtk_drag_dest_add_text_targets(drawing_area);
		//gtk_drag_dest_add_uri_targets(drawing_area);
		const GtkTargetEntry targets[2] = { {"text/plain",0,0}, { "application/x-rootwindow-drop",0,0 } };
		gtk_drag_dest_set(drawing_area,GTK_DEST_DEFAULT_ALL,targets,2,GDK_ACTION_COPY);
		g_signal_connect(drawing_area,"drag-drop",G_CALLBACK(DnDdrop),NULL);
		g_signal_connect(drawing_area,"drag-motion",G_CALLBACK(DnDmotion),NULL);
		g_signal_connect(drawing_area,"drag-data-received",G_CALLBACK(DnDreceive),NULL);
        g_signal_connect (drawing_area, "drag-leave",G_CALLBACK(DnDleave),NULL);

		
		//gtk_window_move (GTK_WINDOW (window), machine -> viewport -> location . position . x, machine -> viewport -> location . position . y);
		//gtk_window_resize (GTK_WINDOW (window), machine -> viewport -> location . size . x, machine -> viewport -> location . size . y);
	gtk_widget_show_all (window);
	viewport -> window = window;
	boarder_clean = false;
	return FALSE; // to be called once only!
}

class viewport : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		PrologElement * name = 0;
		PrologElement * atom = 0;
		double locations [4] = {100.0, 100.0, 160.0, 90.0};
		int location_index = 0;
		while (parameters -> isPair ()) {
			PrologElement * el = parameters -> getLeft ();
			if (el -> isAtom ()) atom = el;
			if (el -> isVar ()) atom = el;
			if (el -> isText ()) name = el;
			if (el -> isInteger () && location_index < 4) locations [location_index++] = el -> getInteger ();
			parameters = parameters -> getRight ();
		}
		if (atom == 0) return false;
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		viewport_action * machine = new viewport_action (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		PROLOG_STRING viewport_name;
		if (name == 0) prolog_string_copy (viewport_name, "VIEWPORT");
		else prolog_string_copy (viewport_name, name -> getText ());
		machine -> viewport = board -> insert_viewport (atom -> getAtom (), viewport_name, rect (locations));
		//CreateViewportIdleCode (machine -> viewport);
		g_idle_add ((GSourceFunc) CreateViewportIdleCode, machine -> viewport);
		return true;
	}
	viewport (PrologDirectory * directory) {this -> directory = directory;}
};

static char * token_action_code = "Token Action Code";

class token_actions : public PrologNativeCode {
public:
	PrologDirectory * directory;
	PrologAtom * location_atom;
	PrologAtom * size_atom;
	PrologAtom * position_atom;
	PrologAtom * scaling_atom;
	PrologAtom * background_colour_atom;
	PrologAtom * foreground_colour_atom;
	PrologAtom * lock_atom, * unlock_atom, * is_locked_atom;
	PrologAtom * select_atom, * deselect_atom, * is_selected_atom;
	PrologAtom * rotation_atom;
	PrologAtom * side_atom, * roll_atom;
	PrologAtom * indexing_atom, * no_indexing_atom, * indexed_atom;
	PrologAtom * shuffle_atom, * insert_atom, * release_atom, *release_random_atom, * select_deck_atom, * is_deck_atom;
	boarder_token * token;
	static char * name (void) {return token_action_code;}
	char * codeName (void) {return token_action_code;}
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (token == 0) return false;
		if (parameters -> isEarth ()) {
			token -> atom -> unProtect ();
			token -> atom -> setMachine (0);
			token -> atom -> unProtect ();
			board -> remove_token (token);
			delete this;
			boarder_clean = false;
			return true;
		}
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft (); parameters = parameters -> getRight ();
		if (! atom -> isAtom ()) return false;
		if (atom -> getAtom () == location_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair ();
				rect token_location = token -> get_location ();
				parameters -> getLeft () -> setInteger ((int) token_location . position . x); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token_location . position . y); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token_location . size . x); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token_location . size . y);
				return true;
			}
			if (! parameters -> isPair ()) return false; PrologElement * x = parameters -> getLeft (); if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * y = parameters -> getLeft (); if (! y -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * width = parameters -> getLeft (); if (! width -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (parameters -> isEarth ()) {token -> set_location (rect (x -> getInteger (), y -> getInteger (), width -> getInteger (), width -> getInteger ())); boarder_clean = false; return true;}
			if (! parameters -> isPair ()) return false; PrologElement * height = parameters -> getLeft (); if (! height -> isInteger ()) return false; parameters = parameters -> getRight ();
			token -> set_location (rect (x -> getInteger (), y -> getInteger (), width -> getInteger (), height -> getInteger ()));
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == position_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair ();
				rect token_location = token -> get_location ();
				parameters -> getLeft () -> setInteger ((int) token_location . position . x); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token_location . position . y);
				return true;
			}
			if (! parameters -> isPair ()) return false; PrologElement * x = parameters -> getLeft (); if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * y = parameters -> getLeft (); if (! y -> isInteger ()) return false; parameters = parameters -> getRight ();
			token -> set_position (point (x -> getInteger (), y -> getInteger ()));
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == size_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair ();
				rect token_location = token -> get_location ();
				parameters -> getLeft () -> setInteger ((int) token_location . size . x); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token_location . size . y);
				return true;
			}
			if (! parameters -> isPair ()) return false; PrologElement * width = parameters -> getLeft (); if (! width -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (parameters -> isEarth ()) {token -> set_size (point (width -> getInteger (), width -> getInteger ())); boarder_clean = false; return true;}
			if (! parameters -> isPair ()) return false; PrologElement * height = parameters -> getLeft (); if (! height -> isInteger ()) return false; parameters = parameters -> getRight ();
			token -> set_size (point (width -> getInteger (), height -> getInteger ()));
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == background_colour_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> background_colour . red)); parameters = parameters -> getRight ();
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> background_colour . green)); parameters = parameters -> getRight ();
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> background_colour . blue)); parameters = parameters -> getRight ();
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> background_colour . alpha)); parameters = parameters -> getRight ();
				return true;
			}
			if (! parameters -> isPair ()) return false; PrologElement * red = parameters -> getLeft (); if (! red -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * green = parameters -> getLeft (); if (! green -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * blue = parameters -> getLeft (); if (! blue -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (parameters -> isPair ()) {
				PrologElement * alpha = parameters -> getLeft (); if (! alpha -> isInteger ()) return false;
				token -> background_colour = colour (red -> getInteger (), green -> getInteger (), blue -> getInteger (), alpha -> getInteger ());
			} else token -> background_colour = colour (red -> getInteger (), green -> getInteger (), blue -> getInteger ());
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == foreground_colour_atom) {
			if (parameters -> isVar ()) {
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> foreground_colour . red)); parameters = parameters -> getRight ();
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> foreground_colour . green)); parameters = parameters -> getRight ();
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> foreground_colour . blue)); parameters = parameters -> getRight ();
				parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (token -> foreground_colour . alpha)); parameters = parameters -> getRight ();
				return true;
			}
			if (! parameters -> isPair ()) return false; PrologElement * red = parameters -> getLeft (); if (! red -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * green = parameters -> getLeft (); if (! green -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * blue = parameters -> getLeft (); if (! blue -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (parameters -> isPair ()) {
				PrologElement * alpha = parameters -> getLeft (); if (! alpha -> isInteger ()) return false;
				token -> foreground_colour = colour (red -> getInteger (), green -> getInteger (), blue -> getInteger (), alpha -> getInteger ());
			} else token -> foreground_colour = colour (red -> getInteger (), green -> getInteger (), blue -> getInteger ());
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == scaling_atom) {
			if (parameters -> isVar ()) {parameters -> setPair (); parameters -> getLeft () -> setDouble (token -> scaling); return true;}
			if (! parameters -> isPair ()) return false;
			PrologElement * scaling = parameters -> getLeft ();
			if (scaling -> isDouble ()) {token -> scaling = scaling -> getDouble (); boarder_clean = false; return true;}
			if (scaling -> isInteger ()) {token -> scaling = (double) scaling -> getInteger (); boarder_clean = false; return true;}
			return false;
		}
		if (atom -> getAtom () == rotation_atom) {
			if (parameters -> isVar ()) {parameters -> setPair (); parameters -> getLeft () -> setDouble (token -> rotation); return true;}
			if (! parameters -> isPair ()) return false;
			PrologElement * rotation = parameters -> getLeft ();
			if (rotation -> isDouble ()) {token -> rotation = rotation -> getDouble (); boarder_clean = false; return true;}
			if (rotation -> isInteger ()) {token -> rotation = (double) rotation -> getInteger (); boarder_clean = false; return true;}
			return false;
		}
		if (atom -> getAtom () == side_atom) {
			if (parameters -> isVar ()) {parameters -> setPair (); parameters -> getLeft () -> setInteger (token -> side); return true;}
			if (! parameters -> isPair ()) return false;
			PrologElement * side = parameters -> getLeft ();
			if (! side -> isInteger ()) return false;
			token -> side = side -> getInteger ();
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == indexing_atom) {
			if (parameters -> isEarth ()) {token -> no_indexing = false; boarder_clean = false; return true;}
			if (parameters -> isVar ()) {
				parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token -> indexing . position . x); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token -> indexing . position . y); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token -> indexing . size . x); parameters = parameters -> getRight (); parameters -> setPair ();
				parameters -> getLeft () -> setInteger ((int) token -> indexing . size . y);
				return true;
			}
			if (! parameters -> isPair ()) return false; PrologElement * x = parameters -> getLeft (); if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * y = parameters -> getLeft (); if (! y -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * width = parameters -> getLeft (); if (! width -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false; PrologElement * height = parameters -> getLeft (); if (! height -> isInteger ()) return false; parameters = parameters -> getRight ();
			token -> indexing = rect (x -> getInteger (), y -> getInteger (), width -> getInteger (), height -> getInteger ());
			boarder_clean = false;
			return true;
		}
		if (atom -> getAtom () == no_indexing_atom) {token -> no_indexing = true; boarder_clean = false; return true;}
		if (atom -> getAtom () == indexed_atom) return ! token -> no_indexing;
		if (atom -> getAtom () == roll_atom) {
			int ret = token -> randomize_side ();
			boarder_clean = false;
			if (parameters -> isEarth ()) return true;
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) {parameters -> setInteger (ret); return true;}
			return false;
		}
		if (atom -> getAtom () == lock_atom) {token -> locked = true; boarder_clean = false; return true;}
		if (atom -> getAtom () == unlock_atom) {token -> locked = false; boarder_clean = false; return true;}
		if (atom -> getAtom () == is_locked_atom) {return token -> locked;}
		if (atom -> getAtom () == select_atom) {token -> selected = true; return true;}
		if (atom -> getAtom () == deselect_atom) {token -> selected = false; return true;}
		if (atom -> getAtom () == is_selected_atom) {return token -> selected;}
		if (atom -> getAtom () == select_deck_atom) {if (! token -> can_insert ()) return false; board -> deck = token; return true;}
		if (atom -> getAtom () == shuffle_atom) {return token -> shuffle ();}
		if (atom -> getAtom () == insert_atom) {
			if (parameters -> isEarth ()) {
				if (board -> deck == 0) return false;
				return board -> transfer_token_to_deck (board -> deck, token);
			}
			if (! parameters -> isPair ()) return false;
			PrologElement * deck_element = parameters -> getLeft (); if (! deck_element -> isAtom ()) return false;
			PrologAtom * deck_atom = deck_element -> getAtom (); if (deck_atom == 0) return false;
			PrologNativeCode * deck_machine = deck_atom -> getMachine (); if (deck_machine == 0) return false;
			if (deck_machine -> codeName () != token_actions :: name ()) return false;
			boarder_token * deck_token = ((token_actions *) deck_machine) -> token; if (deck_token == 0) return false;
			if (deck_token -> can_insert ()) return board -> transfer_token_to_deck (deck_token, token);
			if (token -> can_insert ()) return board -> transfer_token_to_deck (token, deck_token);
			return false;
		}
		if (atom -> getAtom () == release_atom) {
			boarder_token * btp = board -> release_token_from_deck (token);
			if (btp == 0) return false;
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) parameters -> setAtom (btp -> atom);
			return true;
		}
		if (atom -> getAtom () == release_random_atom) {
			boarder_token * btp = board -> release_random_token_from_deck (token);
			if (btp == 0) return false;
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) parameters -> setAtom (btp -> atom);
			return true;
		}
		if (atom -> getAtom () == is_deck_atom) {return token -> can_insert ();}
		return false;
	}
	token_actions (PrologDirectory * directory) {
		this -> directory = directory;
		location_atom = size_atom = position_atom = scaling_atom = background_colour_atom = foreground_colour_atom = 0;
		lock_atom = unlock_atom = is_locked_atom = select_atom = deselect_atom = is_selected_atom = 0;
		rotation_atom = 0;
		side_atom = roll_atom = 0;
		indexing_atom = no_indexing_atom = indexed_atom = 0;
		shuffle_atom = insert_atom = release_atom = release_random_atom = select_deck_atom = is_deck_atom = 0;
		token = 0;
		if (directory) {
			location_atom = directory -> searchAtom (LOCATION);
			size_atom = directory -> searchAtom (SIZE);
			position_atom = directory -> searchAtom (POSITION);
			scaling_atom = directory -> searchAtom (SCALING);
			background_colour_atom = directory -> searchAtom (BACKGROUND_COLOUR);
			foreground_colour_atom = directory -> searchAtom (FOREGROUND_COLOUR);
			lock_atom = directory -> searchAtom (LOCK);
			unlock_atom = directory -> searchAtom (UNLOCK);
			is_locked_atom = directory -> searchAtom (IS_LOCKED);
			select_atom = directory -> searchAtom (SELECT);
			deselect_atom = directory -> searchAtom (DESELECT);
			is_selected_atom = directory -> searchAtom (IS_SELECTED);
			rotation_atom = directory -> searchAtom (ROTATION);
			side_atom = directory -> searchAtom (SIDE);
			roll_atom = directory -> searchAtom (ROLL);
			indexing_atom = directory -> searchAtom (INDEXING);
			no_indexing_atom = directory -> searchAtom (NO_INDEXING);
			indexed_atom = directory -> searchAtom (INDEXED);
			shuffle_atom = directory -> searchAtom (SHUFFLE);
			insert_atom = directory -> searchAtom (INSERT);
			release_atom = directory -> searchAtom (RELEASE);
			release_random_atom = directory -> searchAtom (RELEASE_RANDOM);
			select_deck_atom = directory -> searchAtom (SELECT_DECK);
			is_deck_atom = directory -> searchAtom (IS_DECK);
		}
	}
};

static void CreateRectangleCommand (void) {
	if (board == 0) return;
	PrologRoot * root = board -> root;
	if (root == 0) return;
	PrologElement * location_query = root -> pair (root -> var (0),
		root -> pair (root -> atom ("boarder", "Location"),
		root -> pair (root -> integer ((int) click_init_point . x),
		root -> pair (root -> integer ((int) click_init_point . y),
		root -> pair (root -> integer (0),
		root -> pair (root -> integer (0),
		root -> earth ()))))));
	PrologElement * creation_query = root -> pair (root -> atom ("boarder", "CreateRectangle"),
		root -> pair (root -> var (0),
		root -> earth ()));
	PrologElement * query = root -> pair (root -> pair (root -> var (0), root -> earth ()),
		root -> pair (creation_query, root -> pair (location_query, root -> earth ())));
	root -> resolution (query);
	char command [1024];
	root -> getValue (query, command, 0);
	printf ("command => %s\n", command);
	PrologElement * return_atom = query;
	if (return_atom -> isPair ()) return_atom = return_atom -> getLeft ();
	if (return_atom -> isPair ()) return_atom = return_atom -> getLeft ();
	if (return_atom -> isAtom ()) {
		printf ("atom => %s\n", return_atom -> getAtom () -> name ());
		PrologNativeCode * machine = return_atom -> getAtom () -> getMachine ();
		if (machine != 0) {
			if (machine -> codeName () == token_actions :: name ()) {
				edited_token = ((token_actions *) machine) -> token;
			}
		}
	}
	delete query;
}

class create_rectangle : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft ();
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		token_actions * machine = new token_actions (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		machine -> token = new rectangle_token (atom -> getAtom ());
		machine -> token -> foreground_colour = board -> default_rectangle_foreground_colour;
		machine -> token -> background_colour = board -> default_rectangle_background_colour;
		board -> insert_token (machine -> token);
		boarder_clean = false;
		return true;
	}
	create_rectangle (PrologDirectory * directory) {this -> directory = directory;}
};

class create_circle : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft ();
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		token_actions * machine = new token_actions (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		machine -> token = new circle_token (atom -> getAtom ());
		machine -> token -> foreground_colour = board -> default_circle_foreground_colour;
		machine -> token -> background_colour = board -> default_circle_background_colour;
		board -> insert_token (machine -> token);
		boarder_clean = false;
		return true;
	}
	create_circle (PrologDirectory * directory) {this -> directory = directory;}
};

class create_picture : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft ();
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		parameters = parameters -> getRight ();
		PrologElement * picture_location = 0;
		PrologElement * sides = 0;
		while (parameters -> isPair ()) {
			PrologElement * left = parameters -> getLeft ();
			if (left -> isInteger ()) sides = left;
			else if (left -> isText ()) picture_location = left;
			else return false;
			parameters = parameters -> getRight ();
		}
		char area [128];
		sprintf (area, "%s.png", atom -> getAtom () -> name ());
		token_actions * machine = new token_actions (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		machine -> token = new picture_token (atom -> getAtom (), picture_location ? picture_location -> getText () : area, sides ? sides -> getInteger () : 1);
		machine -> token -> foreground_colour = board -> default_picture_foreground_colour;
		machine -> token -> background_colour = board -> default_picture_background_colour;
		board -> insert_token (machine -> token);
		boarder_clean = false;
		return true;
	}
	create_picture (PrologDirectory * directory) {this -> directory = directory;}
};

class create_dice : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft ();
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		parameters = parameters -> getRight ();
		PrologElement * sides = 0;
		PrologElement * shift = 0;
		PrologElement * multiplier = 0;
		if (parameters -> isPair ()) {
			sides = parameters -> getLeft ();
			if (! sides -> isInteger ()) return false;
			parameters = parameters -> getRight ();
			if (parameters -> isPair ()) {
				shift = parameters -> getLeft ();
				if (! shift -> isInteger ()) return false;
				parameters = parameters -> getRight ();
				if (parameters -> isPair ()) {
					multiplier = parameters -> getLeft ();
					if (! multiplier -> isInteger ()) return false;
				}
			}
		}
		token_actions * machine = new token_actions (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		dice_token * dice = 0;
		if (multiplier) dice = new dice_token (atom -> getAtom (), sides -> getInteger (), shift -> getInteger (), multiplier -> getInteger ());
		else if (shift) dice = new dice_token (atom -> getAtom (), sides -> getInteger (), shift -> getInteger ());
		else if (sides) dice = new dice_token (atom -> getAtom (), sides -> getInteger ());
		else dice = new dice_token (atom -> getAtom ());
		switch (dice -> sides) {
		case 0: dice -> foreground_colour = board -> default_dice_foreground_colour; dice -> background_colour = board -> default_dice_background_colour; break;
		case 4: dice -> foreground_colour = board -> default_tetrahedron_foreground_colour; dice -> background_colour = board -> default_tetrahedron_background_colour; break;
		case 6: dice -> foreground_colour = board -> default_cube_foreground_colour; dice -> background_colour = board -> default_cube_background_colour; break;
		case 8: dice -> foreground_colour = board -> default_octahedron_foreground_colour; dice -> background_colour = board -> default_octahedron_background_colour; break;
		case 10:
			if (dice -> multiplier == 1) {
				dice -> foreground_colour = board -> default_deltahedron_foreground_colour; dice -> background_colour = board -> default_deltahedron_background_colour;
			} else {
				dice -> foreground_colour = board -> default_deltahedron_10_foreground_colour; dice -> background_colour = board -> default_deltahedron_10_background_colour;
			} break;
		case 12: dice -> foreground_colour = board -> default_dodecahedron_foreground_colour; dice -> background_colour = board -> default_dodecahedron_background_colour; break;
		case 20: dice -> foreground_colour = board -> default_icosahedron_foreground_colour; dice -> background_colour = board -> default_icosahedron_background_colour; break;
		default: dice -> foreground_colour = board -> default_dice_foreground_colour; dice -> background_colour = board -> default_dice_background_colour; break;
		}
		machine -> token = dice;
		board -> insert_token (machine -> token);
		boarder_clean = false;
		return true;
	}
	create_dice (PrologDirectory * directory) {this -> directory = directory;}
};

class create_grid : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft ();
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		token_actions * machine = new token_actions (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		board -> insert_token (machine -> token = new grid_token (atom -> getAtom ()));
		machine -> token -> foreground_colour = board -> default_grid_foreground_colour;
		machine -> token -> background_colour = board -> default_grid_background_colour;
		boarder_clean = false;
		return true;
	}
	create_grid (PrologDirectory * directory) {this -> directory = directory;}
};

class create_text_token : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft ();
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * text = parameters -> getLeft ();
		if (! text -> isText ()) return false;
		token_actions * machine = new token_actions (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		machine -> token = new text_token (atom -> getAtom (), text -> getText ());
		machine -> token -> foreground_colour = board -> default_text_foreground_colour;
		machine -> token -> background_colour = board -> default_text_background_colour;
		board -> insert_token (machine -> token);
		boarder_clean = false;
		return true;
	}
	create_text_token (PrologDirectory * directory) {this -> directory = directory;}
};

class create_deck : public PrologNativeCode {
public:
	PrologDirectory * directory;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * atom = parameters -> getLeft ();
		if (atom -> isVar ()) atom -> setAtom (new PrologAtom ());
		if (! atom -> isAtom ()) return false;
		parameters = parameters -> getRight ();
		PrologElement * text = 0;
		if (parameters -> isPair ()) {
			text = parameters -> getLeft ();
			if (! text -> isText ()) return false;
		}
		token_actions * machine = new token_actions (directory);
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}
		machine -> token = new deck_token (atom -> getAtom (), text ? text -> getText () : 0);
		machine -> token -> foreground_colour = board -> default_deck_foreground_colour;
		machine -> token -> background_colour = board -> default_deck_background_colour;
		board -> insert_token (machine -> token);
		boarder_clean = false;
		return true;
	}
	create_deck (PrologDirectory * directory) {this -> directory = directory;}
};

class hit_test : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * x = parameters -> getLeft (); if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * y = parameters -> getLeft (); if (! y -> isInteger ()) return false; parameters = parameters -> getRight ();
		point location = point ((double) x -> getInteger (), (double) y -> getInteger ());
		point size;
		if (parameters -> isPair ()) {
			PrologElement * width = parameters -> getLeft (); if (! width -> isInteger ()) return false; parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			PrologElement * height = parameters -> getLeft (); if (! height -> isInteger ()) return false; parameters = parameters -> getRight ();
			size = point ((double) width -> getInteger (), (double) height -> getInteger ());
		} else size = point ();
		if (! parameters -> isVar ()) return false;
		rect area = rect (location, size);
		boarder_token * token = board -> hit_test (area);
		if (token == 0) {parameters -> setEarth (); return true;}
		while (token != 0) {
			if (token -> atom != 0) {
				parameters -> setPair ();
				parameters -> getLeft () -> setAtom (token -> atom);
				parameters = parameters -> getRight ();
			}
			token = token -> hit_test_next (area);
		}
		return true;
	}
};

class selection : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isVar ()) return false;
		boarder_token * token = board -> tokens;
		if (token == 0) {parameters -> setEarth (); return true;}
		while (token != 0) {
			if (token -> atom != 0 && token -> selected) {
				parameters -> setPair ();
				parameters -> getLeft () -> setAtom (token -> atom);
				parameters = parameters -> getRight ();
			}
			token = token -> next;
		}
		return true;
	}
};

class is_deck : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isAtom ()) return false;
		PrologAtom * atom = parameters -> getAtom ();
		if (atom == 0) return false;
		PrologNativeCode * machine = atom -> getMachine ();
		if (machine == 0) return false;
		if (machine -> codeName () != token_actions :: name ()) return false;
		boarder_token * token = ((token_actions *) machine) -> token;
		if (token == 0) return false;
		if (token -> can_insert ()) return true;
		return false;
	}
};

class repaint : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		board -> repaint ();
		return true;
	}
};

class save_board : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isText ()) return false;
		return boarder_clean = board -> save (parameters -> getText ());
	}
};

class clean : public PrologNativeCode {
public: bool code (PrologElement * parameters, PrologResolution * resolution) {boarder_clean = true; return true;}
};

class is_clean : public PrologNativeCode {
public: bool code (PrologElement * parameters, PrologResolution * resolution) {return boarder_clean;}
};

class erase : public PrologNativeCode {
public: bool code (PrologElement * parameters, PrologResolution * resolution) {if (board == 0) return false; board -> erase (); boarder_clean = true; return true;}
};

class select_deck : public PrologNativeCode {
public: bool code (PrologElement * parameters, PrologResolution * resolution) {if (board == 0) return false; board -> deck = 0; return true;}
};

class default_colour : public PrologNativeCode {
public:
	colour * c;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (parameters -> isVar ()) {
			parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (c -> red)); parameters = parameters -> getRight ();
			parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (c -> green)); parameters = parameters -> getRight ();
			parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (c -> blue)); parameters = parameters -> getRight ();
			parameters -> setPair (); parameters -> getLeft () -> setInteger (colour_to_int (c -> alpha)); parameters = parameters -> getRight ();
			return true;
		}
		if (! parameters -> isPair ()) return false;
		PrologElement * red = parameters -> getLeft (); if (! red -> isInteger ()) return false; parameters = parameters -> getRight (); if (! parameters -> isPair ()) return false;
		PrologElement * green = parameters -> getLeft (); if (! green -> isInteger ()) return false; parameters = parameters -> getRight (); if (! parameters -> isPair ()) return false;
		PrologElement * blue = parameters -> getLeft (); if (! blue -> isInteger ()) return false; parameters = parameters -> getRight ();
		PrologElement * alpha = 0; if (parameters -> isPair ()) {alpha = parameters -> getLeft (); if (! alpha -> isInteger ()) return false;}
		* c = colour (red -> getInteger (), green -> getInteger (), blue -> getInteger (), alpha ? alpha -> getInteger () : 255);
		return true;
	}
	default_colour (colour * c) {this -> c = c;}
};

class diagnostics : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		boarder_viewport * viewport = board -> viewports;
		printf ("DIAGNOSTICS:\n");
		while (viewport != 0) {
			printf ("	viewport [%s %s] <%i %i>\n", viewport -> atom -> name (), viewport -> name, (int) viewport -> board_position . x, (int) viewport -> board_position . y);
			viewport = viewport -> next;
		}
		return true;
	}
};

void boarder_service_class :: init (PrologRoot * root) {
	board = new boarder (root);
	this -> root = root;
}

PrologNativeCode * boarder_service_class :: getNativeCode (char * name) {
	PrologDirectory * dir = root -> searchDirectory ("boarder");
	if (strcmp (name, VIEWPORT) == 0) return new viewport (dir);
	if (strcmp (name, BACKGROUND_COLOUR) == 0) return new default_colour (& board -> background_colour);
	if (strcmp (name, REPAINT) == 0) return new repaint ();
	if (strcmp (name, SAVE_BOARD) == 0) return new save_board ();
	if (strcmp (name, CLEAN) == 0) return new clean ();
	if (strcmp (name, IS_CLEAN) == 0) return new is_clean ();
	if (strcmp (name, ERASE) == 0) return new erase ();
	if (strcmp (name, SELECT_DECK) == 0) return new select_deck ();
	if (strcmp (name, CREATE_RECTANGLE) == 0) return new create_rectangle (dir);
	if (strcmp (name, CREATE_CIRCLE) == 0) return new create_circle (dir);
	if (strcmp (name, CREATE_PICTURE) == 0) return new create_picture (dir);
	if (strcmp (name, CREATE_TEXT) == 0) return new create_text_token (dir);
	if (strcmp (name, CREATE_DICE) == 0) return new create_dice (dir);
	if (strcmp (name, CREATE_GRID) == 0) return new create_grid (dir);
	if (strcmp (name, CREATE_DECK) == 0) return new create_deck (dir);
	if (strcmp (name, HIT_TEST) == 0) return new hit_test ();
	if (strcmp (name, SELECTION) == 0) return new selection ();
	if (strcmp (name, IS_DECK) == 0) return new is_deck ();
	if (strcmp (name, DEFAULT_RECTANGLE_FOREGROUND) == 0) return new default_colour (& board -> default_rectangle_foreground_colour);
	if (strcmp (name, DEFAULT_RECTANGLE_BACKGROUND) == 0) return new default_colour (& board -> default_rectangle_background_colour);
	if (strcmp (name, DEFAULT_CIRCLE_FOREGROUND) == 0) return new default_colour (& board -> default_circle_foreground_colour);
	if (strcmp (name, DEFAULT_CIRCLE_BACKGROUND) == 0) return new default_colour (& board -> default_circle_background_colour);
	if (strcmp (name, DEFAULT_PICTURE_FOREGROUND) == 0) return new default_colour (& board -> default_picture_foreground_colour);
	if (strcmp (name, DEFAULT_PICTURE_BACKGROUND) == 0) return new default_colour (& board -> default_picture_background_colour);
	if (strcmp (name, DEFAULT_TEXT_FOREGROUND) == 0) return new default_colour (& board -> default_text_foreground_colour);
	if (strcmp (name, DEFAULT_TEXT_BACKGROUND) == 0) return new default_colour (& board -> default_text_background_colour);
	if (strcmp (name, DEFAULT_DECK_FOREGROUND) == 0) return new default_colour (& board -> default_deck_foreground_colour);
	if (strcmp (name, DEFAULT_DECK_BACKGROUND) == 0) return new default_colour (& board -> default_deck_background_colour);
	if (strcmp (name, DEFAULT_GRID_FOREGROUND) == 0) return new default_colour (& board -> default_grid_foreground_colour);
	if (strcmp (name, DEFAULT_GRID_BACKGROUND) == 0) return new default_colour (& board -> default_grid_background_colour);
	if (strcmp (name, DEFAULT_DICE_FOREGROUND) == 0) return new default_colour (& board -> default_dice_foreground_colour);
	if (strcmp (name, DEFAULT_DICE_BACKGROUND) == 0) return new default_colour (& board -> default_dice_background_colour);
	if (strcmp (name, DEFAULT_TETRAHEDRON_FOREGROUND) == 0) return new default_colour (& board -> default_tetrahedron_foreground_colour);
	if (strcmp (name, DEFAULT_TETRAHEDRON_BACKGROUND) == 0) return new default_colour (& board -> default_tetrahedron_background_colour);
	if (strcmp (name, DEFAULT_CUBE_FOREGROUND) == 0) return new default_colour (& board -> default_cube_foreground_colour);
	if (strcmp (name, DEFAULT_CUBE_BACKGROUND) == 0) return new default_colour (& board -> default_cube_background_colour);
	if (strcmp (name, DEFAULT_OCTAHEDRON_FOREGROUND) == 0) return new default_colour (& board -> default_octahedron_foreground_colour);
	if (strcmp (name, DEFAULT_OCTAHEDRON_BACKGROUND) == 0) return new default_colour (& board -> default_octahedron_background_colour);
	if (strcmp (name, DEFAULT_DELTAHEDRON_FOREGROUND) == 0) return new default_colour (& board -> default_deltahedron_foreground_colour);
	if (strcmp (name, DEFAULT_DELTAHEDRON_BACKGROUND) == 0) return new default_colour (& board -> default_deltahedron_background_colour);
	if (strcmp (name, DEFAULT_DELTAHEDRON10_FOREGROUND) == 0) return new default_colour (& board -> default_deltahedron_10_foreground_colour);
	if (strcmp (name, DEFAULT_DELTAHEDRON10_BACKGROUND) == 0) return new default_colour (& board -> default_deltahedron_10_background_colour);
	if (strcmp (name, DEFAULT_DODECAHEDRON_FOREGROUND) == 0) return new default_colour (& board -> default_dodecahedron_foreground_colour);
	if (strcmp (name, DEFAULT_DODECAHEDRON_BACKGROUND) == 0) return new default_colour (& board -> default_dodecahedron_background_colour);
	if (strcmp (name, DEFAULT_ICOSAHEDRON_FOREGROUND) == 0) return new default_colour (& board -> default_icosahedron_foreground_colour);
	if (strcmp (name, DEFAULT_ICOSAHEDRON_BACKGROUND) == 0) return new default_colour (& board -> default_icosahedron_background_colour);
	if (strcmp (name, "diagnostics") == 0) return new diagnostics ();
	return NULL;
}

boarder_service_class :: ~ boarder_service_class (void) {
	if (board != 0) delete board; board = 0;
}

/*
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

/* four signal handler callbacks *
void DnDreceive (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint ttype, guint time, gpointer *NA); 
gboolean DnDdrop (GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer *NA); 
void DnDleave (GtkWidget *widget, GdkDragContext *context, guint time, gpointer *NA);
gboolean DnDmotion (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *seld, guint ttype, guint time, gpointer *NA); 

gboolean DND=FALSE; /* "in play" flag *

int main (int argc, char *argv[]) {
	GtkWidget *window, *textarea;
	/* this may not matter too much, they are MIME types; I found these in the gedit source (C) Maggi, Borelli, et. al. *
	/* (amongst other places) *
	const GtkTargetEntry targets[2] = { {"text/plain",0,0}, { "application/x-rootwindow-drop",0,0 } };
    
	gtk_init (&argc, &argv);
    
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	gtk_window_set_title(GTK_WINDOW (window), "Drag N' Drop");
	g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	textarea = gtk_text_view_new();       /* read-only text view *
	gtk_widget_set_size_request(GTK_WIDGET (textarea), 300, 500);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textarea),FALSE);
	gtk_container_add (GTK_CONTAINER (window), textarea);
	gtk_drag_dest_set(textarea,GTK_DEST_DEFAULT_ALL,targets,2,GDK_ACTION_COPY);
	g_signal_connect(textarea,"drag-drop",G_CALLBACK(DnDdrop),NULL);
	g_signal_connect(textarea,"drag-motion",G_CALLBACK(DnDmotion),NULL);
	g_signal_connect(textarea,"drag-data-received",G_CALLBACK(DnDreceive),NULL);
	g_signal_connect (textarea, "drag-leave",G_CALLBACK(DnDleave),NULL);
	
	gtk_widget_show_all(window);
	gtk_main ();
	return 0;
}


void DnDreceive (GtkWidget *widget, GdkDragContext *context, gint x, gint y, 
				 GtkSelectionData *data, guint ttype, guint time, gpointer *NA) {
	GtkTextBuffer *text = gtk_text_view_get_buffer(GTK_TEXT_VIEW (widget));
	struct stat info;
	FILE *fh;   /* we have to read the file ourself *
	int len;
	gboolean got=TRUE;   /* to bounce inappropriate data *
	gchar *ptr=(char*)data->data, *buffer;
	/* ^ treat everything as char, then test... *
	if ((strncmp(ptr,"file:///",8)!=0) || (strlen(ptr)>4096)) got=FALSE;
	gtk_drag_finish (context, got, FALSE, time);   /* tell source app not to erase the file *
	if (!(got)) return;
	
	ptr+=(7*sizeof(char));   /* making this an absolute path... *
	len=strlen(ptr)-1;
	/* some systems may terminate with network proper \r\n, etc, so defluff *
	while ((ptr[len]==' ') || (ptr[len]=='\n') || (ptr[len]=='\r')) 
	{ ptr[len]='\0'; len--; }
	/* print the file contents into the text area *
	if (stat(ptr,&info)<0) { perror("stat"); return; }
	if (!(fh=fopen(ptr,"r"))) { perror("fopen"); return; }   
	len=(int)info.st_size;
	buffer=malloc(len);
	if (fread(buffer,1,len,fh)!=len) perror("fread");         
	fclose(fh);
	gtk_text_buffer_set_text(text,buffer,len);    
	free(buffer);   
}


gboolean DnDdrop (GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer *NA) {
	GdkAtom target_type;
	/* adapted from "TestDnD - main.c : Simple tutorial for GTK+ Drag-N-Drop" Copyright (C) 2005 Ryan McDougall *
	/* http://live.gnome.org/GnomeLove/DragNDropTutorial *
	/* In this version, we will accept anything the source wants to give us *
	/* context->targets recieved from source app *
	if (context-> targets) {
		target_type = GDK_POINTER_TO_ATOM (g_list_nth_data (context-> targets, 0)); /* Choose the best target type *
		gtk_drag_get_data (
						   widget,         /* "widget" should now receive 'drag-data-received' signal *
						   context,        /* represents the current state of the DnD *
						   target_type,    /* the target type we want *
						   time            /* our time stamp *
						   );
	}
	else return FALSE;     /* cancel *
	return TRUE;
}


void DnDleave (GtkWidget *widget, GdkDragContext *context, guint time, gpointer *NA) {
	GtkWidget *window=gtk_widget_get_toplevel(widget);
	gtk_window_set_title(GTK_WINDOW (window), "Drop is Gone...");
	gtk_text_view_set_editable(GTK_TEXT_VIEW(widget),TRUE);   
	DND=FALSE;
}


gboolean DnDmotion (GtkWidget *widget, GdkDragContext *context, gint x, gint y, 
					GtkSelectionData *seld, guint ttype, guint time, gpointer *NA) {
	GtkWidget *window;
	if (DND) return TRUE;   /* if we are already there, that is good enough
							 You can comment out the last line and uncomment the next one to watch 
							 the drag coordinates in the console while moving thru the text area *
	// g_print("DnDmotion() %d %d\n",x,y,); fflush(stdout); } 
	gtk_text_view_set_editable(GTK_TEXT_VIEW(widget),FALSE);   
	DND=TRUE;   
	window=gtk_widget_get_toplevel(widget);
	gtk_window_set_title(GTK_WINDOW (window), "Entered Drop Zone!");
	return TRUE;
}
*/
