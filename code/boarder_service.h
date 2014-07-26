

#ifndef _BOARDER_SERVICE_
#define _BOARDER_SERVICE_

#include "prolog.h"

class boarder_service_class : public PrologServiceClass {
public:
	PrologDirectory * directory;
	virtual void init (PrologRoot * root, PrologDirectory * directory);
	virtual PrologNativeCode * getNativeCode (char * name);
	virtual ~ boarder_service_class (void);
};

#endif
