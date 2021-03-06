#include <string.h>
#include "boarder.h"
#include "boarder_service.h"

#include <gtk/gtk.h>
#include <cairo.h>

#include "prolog_linux_console.h"

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
static void ChangeViewportName (boarder_viewport * viewport) {
	if (viewport == 0) return;
	if (viewport -> edit_mode == boarder_viewport :: move) {
		gtk_window_set_title (GTK_WINDOW (viewport -> window), viewport -> name);
		return;
	}
	char * mode;
	switch (viewport -> edit_mode) {
	case boarder_viewport :: move: mode = "Move"; break;
	case boarder_viewport :: select: mode = "Select"; break;
	case boarder_viewport :: create_rectangle: mode = "Create Rectangle"; break;
	case boarder_viewport :: create_circle: mode = "Create Circle"; break;
	case boarder_viewport :: create_text: mode = "Create Text"; break;
	case boarder_viewport :: create_tetrahedron: mode = "Create Tetrahedron"; break;
	case boarder_viewport :: create_dice: mode = "Create Dice"; break;
	case boarder_viewport :: create_cube: mode = "Create Cube"; break;
	case boarder_viewport :: create_octahedron: mode = "Create Octahedron"; break;
	case boarder_viewport :: create_deltahedron: mode = "Create Deltahedron"; break;
	case boarder_viewport :: create_deltahedron_10: mode = "Create Deltahedron 10"; break;
	case boarder_viewport :: create_dodecahedron: mode = "Create Dodecahedron"; break;
	case boarder_viewport :: create_icosahedron: mode = "Create Icosahedron"; break;
	case boarder_viewport :: create_grid: mode = "Create Grid"; break;
	case boarder_viewport :: create_deck: mode = "Create Deck"; break;
	case boarder_viewport :: edit_size: mode = "Edit Size"; break;
	case boarder_viewport :: edit_indexing: mode = "Edit Indexing"; break;
	case boarder_viewport :: edit_rotation: mode = "Edit Rotation"; break;
	case boarder_viewport :: edit_side: mode = "Edit Side"; break;
	case boarder_viewport :: edit_scaling: mode = "Edit Scaling"; break;
	case boarder_viewport :: edit_ordering: mode = "Edit Ordering"; break;
	case boarder_viewport :: shuffle: mode = "Shuffle Deck"; break;
	default: mode = "None"; break;
	}
	char command [256];
	sprintf (command, "%s [%s]", viewport -> name, mode);
	gtk_window_set_title (GTK_WINDOW (viewport -> window), command);
}
static gboolean ChangeViewportNameIdleCode (boarder_viewport * viewport) {ChangeViewportName (viewport); return FALSE;}

class viewport_action : public PrologNativeCode {
public:
	PrologDirectory * directory;
	PrologAtom * location_atom;
	PrologAtom * size_atom;
	PrologAtom * position_atom;
	PrologAtom * scaling_atom;
	PrologAtom * repaint_atom;
	PrologAtom * mode_atom;
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
		if (atom -> getAtom () == mode_atom ) {
			if (parameters -> isVar ()) {parameters -> setInteger ((int) viewport -> edit_mode); return true;}
			if (! parameters -> isPair ()) return false; parameters = parameters -> getLeft ();
			if (! parameters -> isInteger ()) return false;
			viewport -> edit_mode = (boarder_viewport :: edit_modes) parameters -> getInteger ();
			g_idle_add ((GSourceFunc) ChangeViewportNameIdleCode, viewport);
			return true;
		}
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
		location_atom = size_atom = position_atom = scaling_atom = repaint_atom = mode_atom = 0;
		this -> viewport = 0;
		if (directory) {
			location_atom = directory -> searchAtom (LOCATION);
			size_atom = directory -> searchAtom (SIZE);
			position_atom = directory -> searchAtom (POSITION);
			scaling_atom = directory -> searchAtom (SCALING);
			repaint_atom = directory -> searchAtom (REPAINT);
			mode_atom = directory -> searchAtom (MODE);
		}
	}
};

static int click_button = 0;
static bool has_selection = false;
static bool moved = false;
//static char * edit_mode = edit_mode_none;
static boarder_token * edited_token = 0;
static bool dragging = false;
static bool minimise_square_area = false;
static bool maximise_square_area = false;
static bool shift_on (void) {return minimise_square_area || maximise_square_area;}
static rect edit_area (point (0, 0), point (0, 0));

static bool yes_no (GtkWidget * viewport, char * text) {
	GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW (viewport), GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", text);
	gtk_window_set_title (GTK_WINDOW (dialog), "INFO");
	bool ret = gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES;
	gtk_widget_destroy (dialog);
	return ret;
}

static gboolean viewport_key_on_event (GtkWidget * widget, GdkEventKey * event, boarder_viewport * viewport) {
	//printf ("key on [%s %i]\n", gdk_keyval_name (event -> keyval), (int) event -> keyval);
	if (board == 0) return FALSE;
	int key = (int) event -> keyval;
	switch (key) {
	case 49:
		viewport -> edit_mode = boarder_viewport :: move;
		ChangeViewportName (viewport);
		break;
	case 50:
		viewport -> edit_mode = boarder_viewport :: select;
		board -> clear_selection ();
		has_selection = false;
		ChangeViewportName (viewport);
		board -> repaint ();
		break;
	case 59: case 39:
		if (viewport -> edit_mode == boarder_viewport :: select) {
			GtkWidget * colour_dialog = gtk_color_selection_dialog_new (key == 59 ? "Foreground Colour" : "Background Colour");
			if (gtk_dialog_run (GTK_DIALOG (colour_dialog)) == GTK_RESPONSE_OK) {
				printf ("foreground [%i %i %i %i]\n", 1, 2, 3, 4);
				GtkColorSelection * selection = GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (colour_dialog) -> colorsel);
				GdkColor colour;
				gtk_color_selection_get_current_color (selection, & colour);
				board -> apply_colour_to_selection (colour . red >> 8, colour . green >> 8, colour . blue >> 8, key == 59);
				board -> repaint ();
			}
			gtk_widget_destroy (colour_dialog);
		}
		break;
	case 51: viewport -> edit_mode = boarder_viewport :: create_rectangle; ChangeViewportName (viewport); break;
	case 52: viewport -> edit_mode = boarder_viewport :: create_circle; ChangeViewportName (viewport); break;
	case 53: viewport -> edit_mode = boarder_viewport :: create_grid; ChangeViewportName (viewport); break;
	case 54: viewport -> edit_mode = boarder_viewport :: create_deck; ChangeViewportName (viewport); break;
	case 55: viewport -> edit_mode = boarder_viewport :: create_text; ChangeViewportName (viewport); break;
	case 121: viewport -> edit_mode = boarder_viewport :: create_tetrahedron; ChangeViewportName (viewport); break;
	case 117: viewport -> edit_mode = boarder_viewport :: create_dice; ChangeViewportName (viewport); break;
	case 105: viewport -> edit_mode = boarder_viewport :: create_cube; ChangeViewportName (viewport); break;
	case 111: viewport -> edit_mode = boarder_viewport :: create_octahedron; ChangeViewportName (viewport); break;
	case 112: viewport -> edit_mode = boarder_viewport :: create_deltahedron; ChangeViewportName (viewport); break;
	case 91: viewport -> edit_mode = boarder_viewport :: create_deltahedron_10; ChangeViewportName (viewport); break;
	case 93: viewport -> edit_mode = boarder_viewport :: create_dodecahedron; ChangeViewportName (viewport); break;
	case 92: viewport -> edit_mode = boarder_viewport :: create_icosahedron; ChangeViewportName (viewport); break;
	case 'z': viewport -> edit_mode = boarder_viewport :: edit_size; ChangeViewportName (viewport); break;
	case 'x': viewport -> edit_mode = boarder_viewport :: edit_indexing; ChangeViewportName (viewport); break;
	case 'c': viewport -> edit_mode = boarder_viewport :: edit_rotation; ChangeViewportName (viewport); break;
	case 'v': viewport -> edit_mode = boarder_viewport :: edit_side; ChangeViewportName (viewport); break;
	case 'b': viewport -> edit_mode = boarder_viewport :: edit_scaling; ChangeViewportName (viewport); break;
	case 'n': viewport -> edit_mode = boarder_viewport :: edit_ordering; ChangeViewportName (viewport); break;
	case 'm': viewport -> edit_mode = boarder_viewport :: shuffle; ChangeViewportName (viewport); break;
	case 32:
		if (viewport -> edit_mode == boarder_viewport :: shuffle) {
			if (yes_no (widget, "Shuffle Deck?")) {
				GtkWidget * info = gtk_message_dialog_new (GTK_WINDOW (widget), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
					board -> shuffle_deck_from_selection () ? "Shuffled successfully." : "Nothing shuffled.");
				gtk_window_set_title (GTK_WINDOW (info), "INFO");
				gtk_dialog_run (GTK_DIALOG (info));
				gtk_widget_destroy (info);
			}
			break;
		}
		if (viewport -> edit_mode == boarder_viewport :: edit_indexing) {board -> reindex_selection (); board -> repaint ();}
		if (viewport -> edit_mode > boarder_viewport :: select) break;
		if (board -> release_token_from_selection (false) != 0) {board -> clear_selection (); board -> repaint (); break;}
		if (board -> randomise_selected_dices ()) {board -> repaint (); break;}
		break;
	case 65293: if (board -> release_token_from_selection (true) != 0) {board -> clear_selection (); board -> repaint ();} break;
	case 65505: maximise_square_area = true; break;
	case 65506: minimise_square_area = true; break;
	case 65361: case 65362: case 65363: case 65364:
		if (viewport -> edit_mode <= boarder_viewport :: select) {
			boarder_clean = false;
			int step = minimise_square_area ? 8 : maximise_square_area ? 32 : 1;
			point delta (key == 65363 ? step : key == 65361 ? -step : 0, key == 65362 ? -step : key == 65364 ? step : 0);
			if (has_selection) {
				board -> move_selection (delta);
				board -> repaint ();
			} else {
				viewport -> board_position -= delta;
				gtk_widget_queue_draw (viewport -> window);
			}
		} else if (viewport -> edit_mode == boarder_viewport :: edit_size) {
			boarder_clean = false;
			int step = 1;
			point delta (key == 65363 ? step : key == 65361 ? -step : 0, key == 65362 ? -step : key == 65364 ? step : 0);
			board -> resize_selection (delta, minimise_square_area, maximise_square_area);
			board -> repaint ();
		} else if (viewport -> edit_mode == boarder_viewport :: edit_indexing) {
			boarder_clean = false;
			int step = 1;
			point delta (key == 65363 ? step : key == 65361 ? -step : 0, key == 65362 ? -step : key == 65364 ? step : 0);
			board -> reindex_selection (minimise_square_area | maximise_square_area ? rect (delta, point (0, 0)) : rect (point (0, 0), delta));
			board -> repaint ();
		} else if (viewport -> edit_mode == boarder_viewport :: edit_rotation) {
			boarder_clean = false;
			double step = (key == 65362 || key == 65363) ? 1.0 : -1.0;
			board -> rotate_selection (step, minimise_square_area || maximise_square_area);
			board -> repaint ();
		} else if (viewport -> edit_mode == boarder_viewport :: edit_side) {
			boarder_clean = false;
			int step = (key == 65362 || key == 65363) ? 1 : -1;
			board -> reside_selection (step, minimise_square_area || maximise_square_area);
			board -> repaint ();
		} else if (viewport -> edit_mode == boarder_viewport :: edit_scaling) {
			boarder_clean = false;
			double step = (key == 65362 || key == 65363) ? 2.0 : 0.5;
			board -> rescale_selection (step);
			board -> repaint ();
		} else if (viewport -> edit_mode == boarder_viewport :: edit_ordering) {
			boarder_clean = false;
			int step = (key == 65362 || key == 65363) ? 1 : -1;
			if (key == 65362 || key == 65363) {
				if (minimise_square_area || maximise_square_area) board -> reorder_selection_to_very_front ();
				else board -> reorder_selection_to_front ();
			} else {
				if (minimise_square_area || maximise_square_area) board -> reorder_selection_to_very_back ();
				else board -> reorder_selection_to_back ();
			}
			board -> repaint ();
		}
		break;
	case 65535:
		if (yes_no (widget, "Delete?")) {board -> erase_selection (); board -> repaint ();}
		break;
	default: break; //printf ("KEY = %i\n", key); break;
	}
	return FALSE;
}

static gboolean viewport_key_off_event (GtkWidget * widget, GdkEventKey * event, boarder_viewport * viewport) {
	int key = (int) event -> keyval;
	switch (key) {
	case 65505: maximise_square_area = false; break;
	case 65506: minimise_square_area = false; break;
	default: break;
	}
	//printf ("key off [%s %i]\n", gdk_keyval_name (event -> keyval), key);
	return FALSE;
}

static gboolean viewport_draw_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	if (viewport == 0) return FALSE;
	if (viewport -> board == 0) return FALSE;
	cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
	board -> draw (cr, viewport);
	if (viewport -> edit_mode == boarder_viewport :: select && dragging) {
		rect location ((edit_area . position - viewport -> board_position) * viewport -> scaling, edit_area . size * viewport -> scaling);
		cairo_rectangle (cr, RECT (location));
		cairo_stroke (cr);
	}
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
		root -> pair (root -> atom ("01gurps", "Position"),
		root -> pair (root -> integer ((int) edit_area . position . x),
		root -> pair (root -> integer ((int) edit_area . position . y),
		root -> earth ()))));
	PrologElement * creation_query;
	if (extended) {
		if (order == 6) creation_query = root -> pair (root -> atom ("01gurps", "CreateDice"),
					root -> pair (root -> var (0),
					root -> earth ()));
		else creation_query = root -> pair (root -> atom ("01gurps", "CreateDice"),
			root -> pair (root -> var (0),
			root -> pair (root -> integer (order),
			root -> pair (root -> integer (0),
			root -> pair (root -> integer (10),
			root -> earth ())))));
	} else creation_query = root -> pair (root -> atom ("01gurps", "CreateDice"),
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

static void CreateFigureCommand (char * figure, bool zero_size = true);

static void CreateTextCommand (point display_location);

/*
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
*/
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
/*
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
*/

static point BoardPoint (boarder_viewport * viewport, GdkEventButton * event) {
	return viewport -> board_position + point (event -> x, event -> y) /
		(viewport -> scaling != 0.0 ? viewport -> scaling : 1.0);
}
static gint window_button_up_event (GtkWidget * widget, GdkEventButton * event, boarder_viewport * viewport) {
	//printf ("button up [%i]\n", event -> button);
	edited_token = 0;
	dragging = false;
	if (board == 0) return TRUE;
	point board_point = BoardPoint (viewport, event);
	edit_area . size = board_point - edit_area . position;
	if (minimise_square_area) edit_area . minimise ();
	if (maximise_square_area) edit_area . maximise ();
	boarder_token * token = 0;
	switch (viewport -> edit_mode) {
	case boarder_viewport :: move:
		token = board -> hit_test (rect (board_point, point (0, 0)));
		while (token != 0 && ! token -> can_insert ()) token = token -> hit_test_next (rect (board_point, point (0, 0)));
		if (token != 0) {board -> transfer_selection_to_deck (token); board -> repaint ();}
		break;
	case boarder_viewport :: select:
		token = board -> hit_test (edit_area);
		has_selection |= token != 0;
		while (token != 0) {token -> selected = true; token = token -> hit_test_next (edit_area);}
		board -> repaint ();
		break;
	case boarder_viewport :: create_text: CreateTextCommand (viewport -> location . position + point (event -> x, event -> y));
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport);
		board -> repaint (); break;
	default: break;
	}
	return TRUE;
}

static gint window_button_down_event (GtkWidget * widget, GdkEventButton * event, boarder_viewport * viewport) {
	//printf ("button down [%i]\n", (int) event -> button);
	dragging = true;
	if (board == 0) return TRUE;
	edit_area = rect (BoardPoint (viewport, event), point (0, 0));
	boarder_token * token = 0;
	switch (viewport -> edit_mode) {
	case boarder_viewport :: move:
		token = board -> hit_test (edit_area);
		if (token != 0) {
			if (! token -> selected) board -> clear_selection ();
			token -> selected = has_selection = true;
		} else board -> clear_selection (has_selection = false);
		board -> repaint ();
		break;
	case boarder_viewport :: create_rectangle: CreateFigureCommand ("CreateRectangle"); boarder_clean = false; break;
	case boarder_viewport :: create_circle: CreateFigureCommand ("CreateCircle"); boarder_clean = false; break;
	case boarder_viewport :: create_tetrahedron: CreateDiceCommand (4);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_cube: CreateDiceCommand (6);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_dice: CreateDiceCommand (6, true);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_octahedron: CreateDiceCommand (8);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_deltahedron: CreateDiceCommand (10);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_deltahedron_10: CreateDiceCommand (10, true);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_dodecahedron: CreateDiceCommand (12);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_icosahedron: CreateDiceCommand (20);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport); break;
	case boarder_viewport :: create_grid: CreateFigureCommand ("CreateGrid", false);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport);
		board -> repaint (); break;
	case boarder_viewport :: create_deck: CreateFigureCommand ("CreateDeck", false);
		viewport -> edit_mode = boarder_viewport :: move; boarder_clean = false; ChangeViewportName (viewport);
		board -> repaint (); break;
	default: break;
	}
	return TRUE;
}

static gint window_button_motion_event (GtkWidget * window, GdkEventButton * event, boarder_viewport * viewport) {
	if (board == 0) return TRUE;
	if (! dragging) return TRUE;
	point p = BoardPoint (viewport, event);
	point delta = p - edit_area . position - edit_area . size;
	edit_area . size = p - edit_area . position;
	if (minimise_square_area) edit_area . minimise ();
	if (maximise_square_area) edit_area . maximise ();
	switch (viewport -> edit_mode) {
	case boarder_viewport :: move:
		boarder_clean = false;
		if (has_selection) {
			board -> move_selection (delta);
			board -> repaint ();
		} else {
			viewport -> board_position -= edit_area . size;
			gtk_widget_queue_draw (viewport -> window);
		}
		break;
	case boarder_viewport :: select: board -> repaint (); break;
	case boarder_viewport :: create_rectangle:
	case boarder_viewport :: create_circle:
		if (edited_token != 0) {
			edited_token -> set_location (edit_area);
			boarder_clean = false;
			board -> repaint ();
		}
		break;
	default: break;
	}
	return TRUE;
}

void DnDreceive (GtkWidget *widget, GdkDragContext *context, gint x, gint y, 
				 GtkSelectionData *data, guint ttype, guint time, boarder_viewport * viewport) {
	gchar * ptr = (char *) data -> data;
	if (board == 0) return;
	if (board -> not_ready_for_drop) return;
	board -> not_ready_for_drop = true;
	PrologRoot * root = board -> root;
	if (root == 0) return;
	char command [4096];
	PrologElement * query = root -> earth ();
	while (strncmp (ptr, "file:///", 8) == 0) {
		ptr += 7;
		#ifdef WIN32
		ptr++;
		#endif
		char * cp = command;
		while (* ptr >= ' ') {
			* cp++ = * ptr++;
		}
		* cp = '\0';
		query = root -> pair (root -> text (command), query);
		while (* ptr > '\0' && * ptr <= ' ') ptr++;
	}
	double scaling = viewport -> scaling != 0.0 ? 1.0 / viewport -> scaling : 1.0;
	query = root -> pair (root -> integer ((int) viewport -> board_position . y + (int) ((double) y * scaling)), query);
	query = root -> pair (root -> integer ((int) viewport -> board_position . x + (int) ((double) x * scaling)), query);
	query = root -> pair (root -> atom ("01gurps", "DragAndDrop"), query);
	query = root -> pair (root -> earth (), root -> pair (query, root -> earth ()));
	root -> resolution (query);
	delete query;
}

gboolean DnDdrop (GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer *NA) {
	GdkAtom target_type;
	if (board != 0) board -> not_ready_for_drop = false;
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
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (viewport_key_on_event), viewport);
	g_signal_connect (G_OBJECT (window), "key-release-event", G_CALLBACK (viewport_key_off_event), viewport);
	gtk_window_move (GTK_WINDOW (window), (int) viewport -> location . position . x, (int) viewport -> location . position . y);
	gtk_window_resize (GTK_WINDOW (window), (int) viewport -> location . size . x, (int) viewport -> location . size . y);
		
	const GtkTargetEntry targets [3] = {{"text/plain",0,0}, {"text/uri-list", 0, 0}, {"application/x-rootwindow-drop", 0, 0}};
	gtk_drag_dest_set (drawing_area, GTK_DEST_DEFAULT_ALL, targets, 3, GDK_ACTION_COPY);
	g_signal_connect (drawing_area, "drag-drop", G_CALLBACK (DnDdrop), viewport);
	g_signal_connect (drawing_area, "drag-motion", G_CALLBACK (DnDmotion), viewport);
	g_signal_connect (drawing_area, "drag-data-received", G_CALLBACK (DnDreceive), viewport);
	g_signal_connect (drawing_area, "drag-leave", G_CALLBACK (DnDleave), viewport);

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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
	PrologAtom * sides_atom, * text_atom;
	boarder_token * token;
	static char * name (void) {return token_action_code;}
	char * codeName (void) {return token_action_code;}
	bool isTypeOf (char * code_name) {return name () == code_name ? true : PrologNativeCode :: isTypeOf (code_name);}
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (token == 0) return false;
		if (parameters -> isEarth ()) {
			token -> atom -> setMachine (0);
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
			if (! parameters -> isPair ()) return false; PrologElement * x = parameters -> getLeft ();
			if (x -> isAtom ()) {
				PrologNativeCode * machine = x -> getAtom () -> getMachine ();
				if (machine == 0) return false;
				if (! machine -> isTypeOf (token_actions :: name ())) return false;
				parameters = parameters -> getRight ();
				if (! parameters -> isPair ()) return false; x = parameters -> getLeft (); if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
				if (! parameters -> isPair ()) return false; PrologElement * y = parameters -> getLeft (); if (! y -> isInteger ()) return false;
				if (token -> moveOnGrid (((token_actions *) machine) -> token, point (x -> getInteger (), y -> getInteger ()))) return true;
				return ((token_actions *) machine) -> token -> moveOnGrid (token, point (x -> getInteger (), y -> getInteger ()));
			}
			if (! x -> isInteger ()) return false; parameters = parameters -> getRight ();
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
			int ret = token -> randomise_side ();
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
			if (! deck_machine -> isTypeOf (token_actions :: name ())) return false;
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
		if (atom -> getAtom () == sides_atom) {
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) {parameters -> setInteger (token -> get_sides ()); return true;}
			if (! parameters -> isInteger ()) return false;
			return token -> set_sides (parameters -> getInteger ());
		}
		if (atom -> getAtom () == text_atom) {
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) {parameters -> setText (token -> get_text ()); return true;}
			if (! parameters -> isText ()) return false;
			return token -> set_text (parameters -> getText ());
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
		sides_atom = text_atom = 0;
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
			sides_atom = directory -> searchAtom (SIDES);
			text_atom = directory -> searchAtom (TEXT_ATOM);
		}
	}
};

class TokenTypeClass : public PrologNativeCode {public: virtual char * codeName (void) {return token_actions :: name ();}};

bool show_entry_dialog (char * area, point display_location) {
	strcpy (area, "");
	GtkWidget * dialog = gtk_dialog_new ();
	gtk_dialog_add_button (GTK_DIALOG (dialog), "OK", 0);
	gtk_dialog_add_button (GTK_DIALOG (dialog), "CANCEL", 1);
	GtkWidget * entry = gtk_entry_new ();
	GtkWidget * content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	gtk_container_add (GTK_CONTAINER (content), entry);
	gtk_window_set_title (GTK_WINDOW (dialog), "Enter Text");
	gtk_window_move (GTK_WINDOW (dialog), (int) display_location . x, (int) display_location . y);
	gtk_widget_show_all (dialog);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) != 0) {gtk_widget_destroy (dialog); return false;}
	area_cat (area, 0, (char *) gtk_entry_get_text (GTK_ENTRY (entry)));
	gtk_widget_destroy (dialog);
	return true;
}

static void CreateTextCommand (point display_location) {
	if (board == 0) return;
	PrologRoot * root = board -> root;
	if (root == 0) return;
	AREA area;
	if (! show_entry_dialog (area, display_location)) return;
	PrologElement * location_query = root -> pair (root -> var (0),
		root -> pair (root -> atom ("01gurps", "Position"),
		root -> pair (root -> integer ((int) edit_area . position . x),
		root -> pair (root -> integer ((int) edit_area . position . y),
		root -> earth ()))));
	PrologElement * creation_query = root -> pair (root -> atom ("01gurps", "CreateText"),
		root -> pair (root -> var (0),
		root -> pair (root -> text (area),
		root -> earth ())));
	PrologElement * query = root -> pair (root -> earth (),
		root -> pair (creation_query, root -> pair (location_query, root -> earth ())));
	root -> resolution (query);
	delete query;
}

static void CreateFigureCommand (char * figure, bool zero_size) {
	if (board == 0) return;
	PrologRoot * root = board -> root;
	if (root == 0) return;
	PrologElement * location_query = 0;
	if (zero_size) {
		location_query = root -> pair (root -> var (0),
			root -> pair (root -> atom ("01gurps", "Location"),
			root -> pair (root -> integer ((int) edit_area . position . x),
			root -> pair (root -> integer ((int) edit_area . position . y),
			root -> pair (root -> integer (0),
			root -> pair (root -> integer (0),
			root -> earth ()))))));
	} else {
		location_query = root -> pair (root -> var (0),
			root -> pair (root -> atom ("01gurps", "Position"),
			root -> pair (root -> integer ((int) edit_area . position . x),
			root -> pair (root -> integer ((int) edit_area . position . y),
			root -> earth ()))));
	}
	PrologElement * creation_query = root -> pair (root -> atom ("01gurps", figure),
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
			if (machine -> isTypeOf (token_actions :: name ())) {
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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
		if (atom -> getAtom () -> getMachine () != 0) return false;
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
		if (! machine -> isTypeOf (token_actions :: name ())) return false;
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
			printf ("	viewport [%s %s] <%i %i>\n", viewport -> atom -> name (),
				viewport -> name, (int) viewport -> board_position . x, (int) viewport -> board_position . y);
			viewport = viewport -> next;
		}
		return true;
	}
};

void boarder_service_class :: init (PrologRoot * root, PrologDirectory * directory) {
	board = new boarder (root);
	this -> directory = directory;
}

PrologNativeCode * boarder_service_class :: getNativeCode (char * name) {
	if (strcmp (name, TOKEN_TYPE) == 0) return new TokenTypeClass ();
	if (strcmp (name, VIEWPORT) == 0) return new viewport (directory);
	if (strcmp (name, BACKGROUND_COLOUR) == 0) return new default_colour (& board -> background_colour);
	if (strcmp (name, REPAINT) == 0) return new repaint ();
	if (strcmp (name, SAVE_BOARD) == 0) return new save_board ();
	if (strcmp (name, CLEAN) == 0) return new clean ();
	if (strcmp (name, IS_CLEAN) == 0) return new is_clean ();
	if (strcmp (name, ERASE) == 0) return new erase ();
	if (strcmp (name, SELECT_DECK) == 0) return new select_deck ();
	if (strcmp (name, CREATE_RECTANGLE) == 0) return new create_rectangle (directory);
	if (strcmp (name, CREATE_CIRCLE) == 0) return new create_circle (directory);
	if (strcmp (name, CREATE_PICTURE) == 0) return new create_picture (directory);
	if (strcmp (name, CREATE_TEXT) == 0) return new create_text_token (directory);
	if (strcmp (name, CREATE_DICE) == 0) return new create_dice (directory);
	if (strcmp (name, CREATE_GRID) == 0) return new create_grid (directory);
	if (strcmp (name, CREATE_DECK) == 0) return new create_deck (directory);
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
	return 0;
}

boarder_service_class :: ~ boarder_service_class (void) {
	if (board != 0) delete board; board = 0;
}

#ifdef WINDOWS_OPERATING_SYSTEM
extern "C" {
	__declspec (dllexport) char * get_module_code (void) {
		HMODULE hm = GetModuleHandle ("01gurps.dll");
		HRSRC resource = FindResource (hm, MAKEINTRESOURCE (401), RT_RCDATA);
		if (resource == 0) return 0;
		HGLOBAL loader = LoadResource (hm, resource);
		if (loader == 0) return 0;
		return (char *) LockResource (loader);
	}
	__declspec (dllexport) PrologServiceClass * create_service_class (void) {return new boarder_service_class ();}
}
#endif

#ifdef LINUX_OPERATING_SYSTEM
extern "C" {
	extern char * get_module_code (void);
	extern PrologServiceClass * create_service_class (void);
}
extern char resource_01gurps;
char * get_module_code (void) {return & resource_01gurps;}
PrologServiceClass * create_service_class (void) {return new boarder_service_class ();}
#endif

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
