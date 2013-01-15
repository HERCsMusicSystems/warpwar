#include <string.h>
#include "boarder.h"
#include "boarder_service.h"

#include <gtk/gtk.h>
#include <cairo.h>

#include "prolog_linux_console.h"

boarder * board = 0;

class viewport_action : public PrologNativeCode {
public:
	boarder_viewport * viewport;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (viewport == 0) return false;
		if (parameters -> isEarth ()) {
			viewport -> atom -> unProtect ();
			viewport -> atom -> setMachine (NULL);
			viewport -> atom -> unProtect ();
			gtk_widget_destroy (viewport -> window);
			board -> remove_viewport (viewport);
			delete this;
			return true;
		}
		double locations [4];
		int location_counter = 0;
		while (parameters -> isPair ()) {
			PrologElement * left = parameters -> getLeft ();
			if (left -> isInteger () && location_counter < 4) locations [location_counter++] = left -> getInteger ();
			parameters = parameters -> getRight ();
		}
		if (location_counter == 4) {viewport -> setWindowLocation (rect (locations)); return true;}
		if (location_counter == 2) {viewport -> setBoardPosition (point (locations)); return true;}
		return false;
	}
	viewport_action (void) {this -> viewport = 0;}
};

extern PrologLinuxConsole * console;
static gboolean viewport_delete_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	char command [256];
	sprintf (command, "[show [%s]]\n", viewport -> atom -> name ());
	console -> insert (command);
	return TRUE;
}
static gboolean viewport_draw_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	if (viewport == 0) return FALSE;
	if (viewport -> board == 0) return FALSE;
	cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
	board -> draw (cr, viewport);
	cairo_destroy (cr);
	return FALSE;
}
static gboolean viewport_configure_event (GtkWidget * widget, GdkEvent * event, boarder_viewport * viewport) {
	viewport -> location . size = point (widget -> allocation . width, widget -> allocation . height);
	return FALSE;
}

class viewport : public PrologNativeCode {
public:
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

		viewport_action * machine = new viewport_action ();
		if (! atom -> getAtom () -> setMachine (machine)) {delete machine; return false;}

		PROLOG_STRING viewport_name;
		if (name == 0) prolog_string_copy (viewport_name, "VIEWPORT");
		else prolog_string_copy (viewport_name, name -> getText ());
		machine -> viewport = board -> insert_viewport (atom -> getAtom (), viewport_name, rect (locations));

		GtkWidget * window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window), viewport_name);
		g_signal_connect (window, "delete-event", G_CALLBACK (viewport_delete_event), machine -> viewport);
		GtkWidget * drawing_area = gtk_drawing_area_new ();
		gtk_container_add (GTK_CONTAINER (window), drawing_area);
		g_signal_connect (G_OBJECT (drawing_area), "expose-event", G_CALLBACK (viewport_draw_event), machine -> viewport);
		g_signal_connect (G_OBJECT (drawing_area), "configure-event", G_CALLBACK (viewport_configure_event), machine -> viewport);
		gtk_widget_show_all (window);

		machine -> viewport -> window = window;
		return true;
	}
};

class token_actions : public PrologNativeCode {
public:
	PrologDirectory * directory;
	boarder_token * token;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		printf ("DIRECTORY [%s]\n", directory -> name ());
		return true;
	}
	token_actions (PrologDirectory * directory) {this -> directory = directory; token = 0;}
};

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
		rectangle_token * token = new rectangle_token ();
		board -> insert_token (token);
		return true;
	}
	create_rectangle (PrologDirectory * directory) {this -> directory = directory;}
};

class background_colour : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * red = parameters -> getLeft (); parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * green = parameters -> getLeft (); parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * blue = parameters -> getLeft (); parameters = parameters -> getRight ();
		if (! red -> isInteger () || ! green -> isInteger () || ! blue -> isInteger ()) return false;
		if (parameters -> isPair ()) {
			PrologElement * alpha = parameters -> getLeft ();
			if (! alpha -> isInteger ()) return false;
			board -> background_colour = colour (red -> getInteger (), green -> getInteger (), blue -> getInteger (), alpha -> getInteger ());
		} else board -> background_colour = colour (red -> getInteger (), green -> getInteger (), blue -> getInteger ());
		return true;
	}
};

class repaint : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		boarder_viewport * viewport = board -> viewports;
		while (viewport != 0) {
			gtk_widget_queue_draw (viewport -> window);
			viewport = viewport -> next;
		}
		return true;
	}
};

class diagnostics : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (board == 0) return false;
		boarder_viewport * viewport = board -> viewports;
		printf ("DIAGNOSTICS:\n");
		while (viewport != 0) {
			printf ("	viewport [%s %s] <%i %i>\n", viewport -> atom -> name (), viewport -> name, viewport -> board_position . x, viewport -> board_position . y);
			viewport = viewport -> next;
		}
		return true;
	}
};

void boarder_service_class :: init (PrologRoot * root) {
	board = new boarder ();
	this -> root = root;
}

PrologNativeCode * boarder_service_class :: getNativeCode (char * name) {
	PrologDirectory * dir = root -> searchDirectory ("boarder");
	if (strcmp (name, "viewport") == 0) return new viewport ();
	if (strcmp (name, "background_colour") == 0) return new background_colour ();
	if (strcmp (name, "repaint") == 0) return new repaint ();
	if (strcmp (name, "create_rectangle") == 0) return new create_rectangle (dir);
	if (strcmp (name, "diagnostics") == 0) return new diagnostics ();
	return NULL;
}

boarder_service_class :: ~ boarder_service_class (void) {
	if (board != 0) delete board; board = 0;
}
