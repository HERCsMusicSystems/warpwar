#include "operating_system.h"
#include <gtk/gtk.h>

#include "prolog.h"
#include "prolog_studio.h"

#include "boarder.h"
#include "boarder_service.h"

#include <string.h>

#ifdef WINDOWS_OPERATING_SYSTEM
#include <process.h>
#define RUNNER_RETURN void
#define RUNNER_PARAMETER void *
#define RETURN
#endif

#ifdef LINUX_OPERATING_SYSTEM
#include <pthread.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/times.h>
#include <fcntl.h>
#define RUNNER_RETURN static void *
#define RUNNER_PARAMETER void *
#define RETURN return 0;
#endif

/*
#ifdef LINUX_OPERATING_SYSTEM
#ifdef MAC_OPERATING_SYSTEM
#include "mac_midi.h"
mac_midi_service midi_service ("STUDIO");
#else
#include "linux_midi.h"
linux_midi_service midi_service;
#endif
#endif

#ifdef WINDOWS_OPERATING_SYSTEM
#include "windows_midi.h"
windows_midi_service midi_service;
#endif
*/

/*
prolog_midi_reader * midi_reader = NULL;

volatile bool running = false;
RUNNER_RETURN transport_runner (RUNNER_PARAMETER root) {
	running = true;
	while (running) {
		((PrologRoot *) root) -> moveTransport ();
#ifdef WINDOWS_OPERATING_SYSTEM
		Sleep (20);
#endif
#ifdef LINUX_OPERATING_SYSTEM
		usleep (20000);
#endif
	}
	running = true;
	RETURN
}
*/

#ifdef LINUX_OPERATING_SYSTEM
typedef void * (* runner_procedure) (RUNNER_PARAMETER);
void beginthread (runner_procedure runner, int value, PrologRoot * root) {
	pthread_t threader;
	pthread_attr_t attr;
	pthread_attr_init (& attr);
	pthread_attr_setstacksize (& attr, 120 * 1024);
	pthread_attr_setdetachstate (& attr, PTHREAD_CREATE_DETACHED);
	pthread_create (& threader, & attr, runner, root);
	pthread_attr_destroy (& attr);
}
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
#define beginthread(procedure, value, pointer) _beginthread (procedure, value, pointer)
#endif


#ifdef INTERNAL_RESOURCES
//#include "neural.h"
//#include "notes.h"
//#ifdef LINUX_OPERATING_SYSTEM
//#include "prolog_mysql.h"
//#endif
#ifdef WINDOWS_OPERATING_SYSTEM
#include "resource.h"
class resource_loader_class : public PrologResourceLoader {
public:
	char * load (char * name) {
		HRSRC resource = NULL;
		if (strcmp (name, "studio") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STUDIO_PRC), RT_RCDATA);
		if (strcmp (name, "conductor") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (CONDUCTOR_PRC), RT_RCDATA);
		if (strcmp (name, "midi") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (MIDI_PRC), RT_RCDATA);
		if (strcmp (name, "http") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HTTP_PRC), RT_RCDATA);
		if (strcmp (name, "store") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STORE_PRC), RT_RCDATA);
		if (strcmp (name, "f1") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (F1_PRC), RT_RCDATA);
		if (strcmp (name, "help") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HELP_PRC), RT_RCDATA);
		if (strcmp (name, "record") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (RECORD_PRC), RT_RCDATA);
		if (strcmp (name, "neural") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (NEURAL_PRC), RT_RCDATA);
		if (strcmp (name, "keyboard") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (KEYBOARD_PRC), RT_RCDATA);
		if (strcmp (name, "sql") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (SQL_PRC), RT_RCDATA);
		if (strcmp (name, "test") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (TEST_PRC), RT_RCDATA);
		if (strcmp (name, "studio.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STUDIO_PRC), RT_RCDATA);
		if (strcmp (name, "conductor.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (CONDUCTOR_PRC), RT_RCDATA);
		if (strcmp (name, "midi.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (MIDI_PRC), RT_RCDATA);
		if (strcmp (name, "http.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HTTP_PRC), RT_RCDATA);
		if (strcmp (name, "store.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (STORE_PRC), RT_RCDATA);
		if (strcmp (name, "f1.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (F1_PRC), RT_RCDATA);
		if (strcmp (name, "help.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (HELP_PRC), RT_RCDATA);
		if (strcmp (name, "record.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (RECORD_PRC), RT_RCDATA);
		if (strcmp (name, "neural.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (NEURAL_PRC), RT_RCDATA);
		if (strcmp (name, "keyboard.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (KEYBOARD_PRC), RT_RCDATA);
		if (strcmp (name, "sql.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (SQL_PRC), RT_RCDATA);
		if (strcmp (name, "test.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (TEST_PRC), RT_RCDATA);
		if (! resource) return NULL;
		HGLOBAL loader = LoadResource (NULL, resource);
		if (! loader) return NULL;
		return (char *) LockResource (loader);
	}
} resource_loader;
#endif

#ifdef LINUX_OPERATING_SYSTEM
extern char resource_0 [];
extern char resource_1 [];
extern char resource_2 [];
extern char resource_3 [];
extern char resource_4 [];
extern char resource_5 [];
extern char resource_6 [];
extern char resource_7 [];
extern char resource_8 [];
extern char resource_9 [];
extern char resource_10 [];
extern char resource_11 [];
extern char resource_12 [];
class resource_loader_class : public PrologResourceLoader {
public:
	char * load (char * name) {
		char * ret = NULL;
		if (strcmp (name, "studio") == 0) ret = resource_0;
		if (strcmp (name, "conductor") == 0) ret = resource_1;
		if (strcmp (name, "midi") == 0) ret = resource_2;
		if (strcmp (name, "http") == 0) ret = resource_3;
		if (strcmp (name, "store") == 0) ret = resource_4;
		if (strcmp (name, "f1") == 0) ret = resource_5;
		if (strcmp (name, "help") == 0) ret = resource_6;
		if (strcmp (name, "record") == 0) ret = resource_7;
		if (strcmp (name, "neural") == 0) ret = resource_8;
		if (strcmp (name, "keyboard") == 0) ret = resource_9;
		if (strcmp (name, "sql") == 0) ret = resource_10;
		if (strcmp (name, "test") == 0) ret = resource_11;
		if (strcmp (name, "boarder") == 0) ret = resource_12;
		if (strcmp (name, "studio.prc") == 0) ret = resource_0;
		if (strcmp (name, "conductor.prc") == 0) ret = resource_1;
		if (strcmp (name, "midi.prc") == 0) ret = resource_2;
		if (strcmp (name, "http.prc") == 0) ret = resource_3;
		if (strcmp (name, "store.prc") == 0) ret = resource_4;
		if (strcmp (name, "f1.prc") == 0) ret = resource_5;
		if (strcmp (name, "help.prc") == 0) ret = resource_6;
		if (strcmp (name, "record.prc") == 0) ret = resource_7;
		if (strcmp (name, "neural.prc") == 0) ret = resource_8;
		if (strcmp (name, "keyboard.prc") == 0) ret = resource_9;
		if (strcmp (name, "sql.prc") == 0) ret = resource_10;
		if (strcmp (name, "test.prc") == 0) ret = resource_11;
		if (strcmp (name, "boarder.prc") == 0) ret = resource_12;
		return ret;
	}
} resource_loader;
#endif

class service_class_loader_class : public PrologServiceClassLoader {
public:
	PrologServiceClass * load (char * name) {
		//if (strcmp (name, "neural") == 0) return new neural_service ();
		//if (strcmp (name, "notes") == 0) return new NotesServiceClass ();
		if (strcmp (name, "boarder") == 0) return new boarder_service_class ();
		//#ifdef LINUX_OPERATING_SYSTEM
		//#ifndef MAC_OPERATING_SYSTEM
		//		if (strcmp (name, "prolog_mysql") == 0) return new MySQLServiceClass ();
		//#endif
		//#endif
		return NULL;
	}
} service_class_loader;

#endif

#ifdef LINUX_OPERATING_SYSTEM
#include "prolog_linux_console.h"
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
#include "prolog_windows_console.h"
#endif

PrologRoot * root = 0;
PrologCommand * console = 0;

RUNNER_RETURN prc_runner (RUNNER_PARAMETER parameter) {
	PrologRoot * root = (PrologRoot *) parameter;
	root -> resolution ("boarder.prc");
//	midi_service . setOutputPort (-1);
//	midi_service . setInputPort (-1);
	gtk_main_quit ();
	RETURN
}

static gboolean delete_event (GtkWidget * widget, GdkEvent * event, gpointer data) {
	//console -> close ();
	//gtk_main_quit ();
	console -> insert ("[exit]\n");
	return FALSE;
}

static gboolean menu_response (GtkWidget * widget, GdkEvent * event, gpointer data) {
}

#include <cairo.h>

static int call_position = 0;

static gboolean re_draw_with_cairo (GtkWidget * widget, GdkEvent * event , gpointer data) {
	cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
	cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
	cairo_set_line_width (cr, 1.0);
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
	cairo_move_to (cr, 20.5, 20.5);
	cairo_line_to (cr, 0.5 + (call_position++), 100.5);
	cairo_stroke (cr);
	cairo_move_to (cr, 50.5, 20.5);
	cairo_line_to (cr, 50.5, 200.5);
	cairo_stroke (cr);
	cairo_set_source_rgb (cr, 0, 0, 1.0);
	cairo_move_to (cr, 51.5, 30.5);
	cairo_line_to (cr, 51.5, 100.5);
	cairo_stroke (cr);
	//cairo_rectangle (cr, 100, 50, 60, 70);
	//cairo_stroke (cr);
	cairo_destroy (cr);
//	printf ("CAIRO\n");
	return FALSE;
}

static GdkPixmap * pixmap = NULL;
static gboolean gdk_configure_event (GtkWidget * widget, GdkEventConfigure * event) {
	if (pixmap != NULL) g_object_unref (pixmap);
	pixmap = gdk_pixmap_new (widget -> window, widget -> allocation . width, widget -> allocation . height, -1);
	gdk_draw_rectangle (pixmap, widget -> style -> white_gc, TRUE, 0, 0, widget -> allocation . width, widget -> allocation . height);
	return TRUE;
}
static gboolean re_draw_with_gdk (GtkWidget * widget, GdkEventExpose * event, gpointer data) {
	gdk_draw_line (pixmap, widget -> style -> fg_gc [gtk_widget_get_state (widget)], 20, 20, call_position++, 100);
	gdk_draw_drawable (widget -> window, widget -> style -> fg_gc [gtk_widget_get_state (widget)], pixmap, event -> area . x, event -> area . y, event -> area . x, event -> area . y, event -> area . width, event -> area . height);
	return FALSE;
}

void create_main_windows (void) {
	GtkWidget * window;
	GtkWidget * menu_bar;
	GtkWidget * group_menu;
	GtkWidget * group_submenu;
	GtkWidget * sonda_menu_item;
	GtkWidget * exit_menu_item;
	GtkWidget * mariner_menu_item;
	GtkWidget * vbox;
	GtkWidget * drawing_area;
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 300, 200);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_title (GTK_WINDOW (window), "SONDA");
	g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), vbox);
	menu_bar = gtk_menu_bar_new ();
	exit_menu_item = gtk_menu_item_new_with_label ("EXIT");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), exit_menu_item);
	group_menu = gtk_menu_item_new_with_label ("GROUP");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), group_menu);
	group_submenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (group_menu), group_submenu);
	sonda_menu_item = gtk_radio_menu_item_new_with_label (NULL, "SONDA");
	gtk_menu_shell_append (GTK_MENU_SHELL (group_submenu), sonda_menu_item);
	mariner_menu_item = gtk_radio_menu_item_new_with_label_from_widget (GTK_RADIO_MENU_ITEM (sonda_menu_item), "MARINER");
	gtk_menu_shell_append (GTK_MENU_SHELL (group_submenu), mariner_menu_item);
	gtk_box_pack_start (GTK_BOX (vbox), menu_bar, FALSE, FALSE, 0);
	
	drawing_area = gtk_drawing_area_new ();
	gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT (drawing_area), "configure_event", G_CALLBACK (gdk_configure_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "expose_event", G_CALLBACK (re_draw_with_cairo), NULL);
	
	
	gtk_widget_show_all (window);
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_show (window);
}

extern int token_counter;

#ifdef LINUX_OPERATING_SYSTEM
#include <X11/Xlib.h>
#endif

int main (int args, char * argv []) {
//	srand (time (0));
	gtk_init (& args, & argv);
	PrologRoot * root = new PrologRoot ();
	root -> get_search_directories_from_environment ("PRC_MODULE_SEARCH_PATHS");
#ifdef INTERNAL_RESOURCES
	root -> setResourceLoader (& resource_loader);
	root -> setServiceClassLoader (& service_class_loader);
#endif
	root -> set_uap32_captions ();
#ifdef LINUX_OPERATING_SYSTEM
	console = new PrologLinuxConsole ();
#endif
#ifdef WINDOWS_OPERATING_SYSTEM
	console = new PrologWindowsConsole ();
#endif
//	console -> open ();
	root -> insertCommander (console);
	beginthread (prc_runner, 0, root);
	
	//create_main_windows ();
	gtk_main ();
	printf ("GTK MAIN LOOP STOPPED\n");
	
//	console -> stop ();
/*
	if (running) {
		running = false;
		while (! running) {
#ifdef WINDOWS_OPERATING_SYSTEM
			Sleep (20);
#endif
#ifdef LINUX_OPERATING_SYSTEM
			usleep (20000);
#endif
		}
		running = false;
	}

	root -> removeThreads ();
	if (root -> getCommander () != NULL) delete root -> getCommander ();
//	if (midi_reader != NULL) delete midi_reader;
*/
	if (root -> getCommander () != 0) delete root -> getCommander ();
	delete root;
	//	if (object_left ())
	drop_object_counter ();
#ifdef WINDOWS_OPERATING_SYSTEM
	FreeConsole ();
#endif
	printf (" token_counter [%i]\n", token_counter);
	return 0;
}


int main_bak (int argc, char * * argv) {
	
	gtk_main ();
	return 0;
}

