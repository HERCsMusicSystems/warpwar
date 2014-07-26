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
#include "prolog_neural.h"
//#ifdef LINUX_OPERATING_SYSTEM
//#include "prolog_mysql.h"
//#endif
#include "prolog_xml.h"
#include "prolog_distribution.h"
#ifdef WINDOWS_OPERATING_SYSTEM
#include "01gurps_resource.h"
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
		if (strcmp (name, "xml") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (XML_PRC), RT_RCDATA);
		if (strcmp (name, "distribution") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (DISTRIBUTION_PRC), RT_RCDATA);
		if (strcmp (name, "boarder") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (BOARDER_PRC), RT_RCDATA);
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
		if (strcmp (name, "xml.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (XML_PRC), RT_RCDATA);
		if (strcmp (name, "distribution.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (DISTRIBUTION_PRC), RT_RCDATA);
		if (strcmp (name, "boarder.prc") == 0) resource = FindResource (NULL, MAKEINTRESOURCE (BOARDER_PRC), RT_RCDATA);
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
extern char resource_13 [];
extern char resource_14 [];
extern char resource_15 [];
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
		if (strcmp (name, "xml") == 0) ret = resource_12;
		if (strcmp (name, "distribution") == 0) ret = resource_13;
		if (strcmp (name, "control") == 0) ret = resource_14;
		if (strcmp (name, "boarder") == 0) ret = resource_15;
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
		if (strcmp (name, "xml.prc") == 0) ret = resource_12;
		if (strcmp (name, "distribution.prc") == 0) ret = resource_13;
		if (strcmp (name, "control.prc") == 0) ret = resource_14;
		if (strcmp (name, "boarder.prc") == 0) ret = resource_15;
		return ret;
	}
} resource_loader;
#endif

class service_class_loader_class : public PrologServiceClassLoader {
public:
	PrologServiceClass * load (char * name) {
		if (strcmp (name, "prolog.neural") == 0) return new PrologNeuralServiceClass ();
		if (strcmp (name, "prolog.xml") == 0) return new PrologXMLServiceClass ();
		if (strcmp (name, "prolog.distribution") == 0) return new PrologDistributionServiceClass ();
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
	gtk_main_quit ();
	RETURN
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
	root -> insertCommander (console);
	beginthread (prc_runner, 0, root);
	
	gtk_main ();
	printf ("GTK MAIN LOOP STOPPED\n");
	
	if (root -> getCommander () != 0) delete root -> getCommander ();
	delete root;
	drop_object_counter ();
	printf (" token_counter [%i]\n", token_counter);
#ifdef WINDOWS_OPERATING_SYSTEM
	FreeConsole ();
#endif
	return 0;
}
