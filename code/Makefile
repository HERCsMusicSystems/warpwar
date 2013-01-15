pi = ../../prolog/linux/
piosx = ../../prolog/osx/
pc = ../../prolog/code/prolog/


objects = \
	$(pi)prolog_dll_loader.o\
	$(pi)midi_stream.o $(pi)object_tracker.o $(pi)prolog_atom.o $(pi)prolog_command.o $(pi)prolog_directory.o \
	$(pi)prolog_element.o $(pi)prolog_loader.o $(pi)prolog_match.o $(pi)prolog_midi_command.o \
	$(pi)prolog_native_code.o $(pi)prolog_query.o $(pi)prolog_reader.o $(pi)prolog_resolution.o \
	$(pi)prolog_resolution_pool.o $(pi)prolog_root.o $(pi)prolog_service_class.o \
	$(pi)prolog_studio.o \
	$(pi)prolog_transport.o \
	$(pi)prolog_transport_pool.o $(pi)prolog_variable.o \
	$(pi)prolog_linux_console.o\
	$(pi)midi_command_prompt.o\
	$(pi)linux_midi.o

objectsosx = \
	$(piosx)prolog_dll_loader.o\
	$(piosx)midi_stream.o $(piosx)object_tracker.o $(piosx)prolog_atom.o $(piosx)prolog_command.o $(piosx)prolog_directory.o \
	$(piosx)prolog_element.o $(piosx)prolog_loader.o $(piosx)prolog_match.o $(piosx)prolog_midi_command.o \
	$(piosx)prolog_native_code.o $(piosx)prolog_query.o $(piosx)prolog_reader.o $(piosx)prolog_resolution.o \
	$(piosx)prolog_resolution_pool.o $(piosx)prolog_root.o $(piosx)prolog_service_class.o \
	$(piosx)prolog_studio.o \
	$(piosx)prolog_transport.o \
	$(piosx)prolog_transport_pool.o $(piosx)prolog_variable.o \
	$(piosx)prolog_linux_console.o\
	$(piosx)midi_command_prompt.o\
	$(piosx)neural.o $(piosx)encoder.o $(piosx)notes.o\
	$(piosx)mac_midi.o prolog_resources.o boarder_service.o boarder.o

prc_objects = \
	$(pc)studio.prc\
	$(pc)store.prc\
	$(pc)f1.prc\
	$(pc)help.prc\
	$(pc)record.prc\
	$(pc)neural.prc\
	$(pc)keyboard.prc\
	$(pc)notes.prc\
	$(pc)sql.prc\
	boarder.prc


~/bin/warpwar: warpwar_main.cpp
	g++ -lreadline -I$(pc) -I$(pi) \
	$(pi)prolog_resources.o $(objects) $(pi)encoder.o $(pi)neural.o \
	-Wno-write-strings warpwar_main.cpp -o ~/bin/warpwar `wx-config --cxxflags` `wx-config --libs`

warpwar: ~/bin/warper


~/bin/polygoon: gridder.cpp
	g++ -I$(pi) -I$(pc) $(pi)prolog_resources.o $(objects) $(pi)encoder.o $(pi)neural.o \
	-Wno-write-strings gridder.cpp -o ~/bin/polygoon `wx-config --cxxflags` `wx-config --libs`

polygoon: ~/bin/polygoon

~/bin/tabletop: board.cpp
	g++ -Wno-write-strings board.cpp -o ~/bin/tabletop `wx-config --cxxflags` `wx-config --libs`

tabletop: ~/bin/tabletop

../tabletop.app/Contents/MacOS/tabletop: board.cpp
	g++ -Wno-write-strings board.cpp -o ../tabletop.app/Contents/MacOS/tabletop `wx-config --cxxflags` `wx-config --libs`

tabletopmac: ../tabletop.app/Contents/MacOS/tabletop

prolog_resources.cpp: $(prc_objects) $(piosx)resource_maker.exe
	$(piosx)resource_maker.exe prolog_resources.cpp $(prc_objects)

prolog_resources.o: prolog_resources.cpp
	g++ prolog_resources.cpp -c -o prolog_resources.o

boarder.o: boarder.h boarder.cpp
	g++ -Wno-write-strings -I$(piosx) -I$(pc) boarder.cpp -c -o boarder.o `pkg-config --cflags gtk+-2.0`

boarder_service.o: boarder.h boarder_service.h boarder_service.cpp
	g++ -I$(pc) -I$(piosx) -Wno-write-strings boarder_service.cpp -c -o boarder_service.o `pkg-config --cflags gtk+-2.0`

../board: gtk_board.o $(objectsosx)
	g++ -lreadline -framework CoreMidi -framework CoreFoundation $(objectsosx) gtk_board.o -o ../board `pkg-config --libs gtk+-2.0`

gtk_board.o: gtk_board.cpp boarder.h boarder_service.h
	g++ -Wno-write-strings -I$(pc) -I$(piosx) gtk_board.cpp -c -o gtk_board.o `pkg-config --cflags gtk+-2.0`


boardmac: ../board

