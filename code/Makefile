pi = ../../hercs-studio-gm/linux/
pc = ../../hercs-studio-gm/code/prolog


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

~/bin/warpwar: warpwar_main.cpp
	g++ -I../../hercs-studio-gm/code/prolog -I../../hercs-studio-gm/linux \
	$(pi)prolog_resources.o $(objects) $(pi)encoder.o $(pi)neural.o \
	-Wno-write-strings warpwar_main.cpp -o ~/bin/warpwar `wx-config --cxxflags` `wx-config --libs`

warpwar: ~/bin/warpwar


~/bin/polygoon: gridder.cpp
	g++ -I$(pi) -I$(pc) $(pi)prolog_resources.o $(objects) $(pi)encoder.o $(pi)neural.o \
	-Wno-write-strings gridder.cpp -o ~/bin/polygoon `wx-config --cxxflags` `wx-config --libs`

polygoon: ~/bin/polygoon

~/bin/tabletop: board.cpp
	g++ -Wno-write-strings board.cpp -o ~/bin/tabletop `wx-config --cxxflags` `wx-config --libs`

tabletop: ~/bin/tabletop

