pi = ../../prolog/linux/
piosx = ../../prolog/osx/
pc = ../../prolog/code/prolog/


objects = \
	$(pi)prolog_dll_loader.o\
	$(pi)midi_stream.o $(pi)object_tracker.o $(pi)prolog_atom.o $(pi)prolog_command.o $(pi)prolog_directory.o \
	$(pi)prolog_element.o $(pi)prolog_loader.o $(pi)prolog_match.o \
	$(pi)prolog_native_code.o $(pi)prolog_query.o $(pi)prolog_reader.o $(pi)prolog_resolution.o \
	$(pi)prolog_root.o $(pi)prolog_service_class.o \
	$(pi)prolog_studio.o \
	$(pi)prolog_transport.o \
	$(pi)prolog_variable.o \
	$(pi)prolog_linux_console.o\
	$(pi)prolog_xml.o $(pi)tinyxml.o $(pi)prolog_neural.o $(pi)prolog_distribution.o $(pi)encoder.o\
	$(pi)prolog_http.o $(pi)prolog_control.o $(pi)joystick_linux.o $(pi)prolog_conductor.o $(pi)prolog_midi.o\
	boarder_service.o boarder.o graphics2d.o

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
	$(piosx)mac_midi.o boarder_service.o boarder.o

prc_objects = \
	$(pc)studio.prc\
	$(pc)conductor.prc\
	$(pc)midi.prc\
	$(pc)http.prc\
	$(pc)store.prc\
	$(pc)f1.prc\
	$(pc)help.prc\
	$(pc)record.prc\
	$(pc)neural.prc\
	$(pc)keyboard.prc\
	$(pc)sql.prc\
	$(pc)test.prc\
	$(pc)xml.prc\
	$(pc)distribution.prc\
	$(pc)control.prc\
	../code/boarder.prc

prc_resources = $(pi)studio_prc.o $(pi)conductor_prc.o $(pi)midi_prc.o $(pi)http_prc.o $(pi)store_prc.o $(pi)f1_prc.o $(pi)help_prc.o $(pi)record_prc.o\
	$(pi)neural_prc.o $(pi)keyboard_prc.o $(pi)sql_prc.o $(pi)test_prc.o $(pi)xml_prc.o $(pi)distribution_prc.o $(pi)control_prc.o boarder_prc.o

~/bin/warpwar: warpwar_main.cpp
	g++ -lreadline -I$(pc) -I$(pi) \
	$(objects) $(pi)encoder.o $(pi)neural.o \
	-Wno-write-strings warpwar_main.cpp -o ~/bin/warpwar `wx-config --cxxflags` `wx-config --libs`

warpwar: ~/bin/warper


~/bin/polygoon: gridder.cpp
	g++ -I$(pi) -I$(pc) $(objects) $(pi)encoder.o $(pi)neural.o \
	-Wno-write-strings gridder.cpp -o ~/bin/polygoon `wx-config --cxxflags` `wx-config --libs`

polygoon: ~/bin/polygoon

~/bin/tabletop: board.cpp
	g++ -Wno-write-strings board.cpp -o ~/bin/tabletop `wx-config --cxxflags` `wx-config --libs`

tabletop: ~/bin/tabletop

../tabletop.app/Contents/MacOS/tabletop: board.cpp
	g++ -Wno-write-strings board.cpp -o ../tabletop.app/Contents/MacOS/tabletop `wx-config --cxxflags` `wx-config --libs`

tabletopmac: ../tabletop.app/Contents/MacOS/tabletop

boarder.o: ../code/boarder.h ../code/boarder.cpp
	g++ -Wno-write-strings -I$(piosx) -I$(pc) ../code/boarder.cpp -c -o boarder.o `pkg-config --cflags gtk+-2.0`

dice.o: ../code/boarder.h ../code/dice.cpp
	g++ -Wno-write-strings -I$(piosx) -I$(pc) ../code/dice.cpp -c -o dice.o `pkg-config --cflags gtk+-2.0`

grid.o: ../code/boarder.h ../code/grid.cpp
	g++ -Wno-write-strings -I$(piosx) -I$(pc) ../code/grid.cpp -c -o grid.o `pkg-config --cflags gtk+-2.0`

boarder_service.o: ../code/boarder.h ../code/boarder_service.h ../code/boarder_service.cpp
	g++ -I$(pc) -I$(piosx) -Wno-write-strings ../code/boarder_service.cpp -c -o boarder_service.o `pkg-config --cflags gtk+-2.0`

../board: gtk_board.o $(objectsosx)
	g++ -lreadline -framework CoreMidi -framework CoreFoundation $(objectsosx) gtk_board.o -o ../board `pkg-config --libs gtk+-2.0`

gtk_board.o: ../code/gtk_board.cpp ../code/boarder.h ../code/boarder_service.h
	g++ -Wno-write-strings -I. -I$(pc) -I$(piosx) ../code/gtk_board.cpp -c -o gtk_board.o `pkg-config --cflags gtk+-2.0`

graphics2d.o: ../../prolog/code/prolog/graphics2d.h ../../prolog/code/prolog/graphics2d.cpp
	g++ -Wno-write-strings ../../prolog/code/prolog/graphics2d.cpp -c -o graphics2d.o


boarder_prc.o: ../code/boarder.prc
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386\
		--redefine-sym _binary____code_boarder_prc_start=resource_boarder\
		../code/boarder.prc boarder_prc.o
	objdump -t boarder_prc.o

boardlinux: ~/bin/boarder

~/bin/boarder: gtk_board.o $(objects)
	g++ gtk_board.o $(objects) -o ~/bin/boarder -lpthread -ldl -lreadline `pkg-config --libs gtk+-2.0`


01gurps: ~/bin/01gurps

01gurps_objects = gtk_board.o dice.o grid.o

~/bin/01gurps: $(01gurps_objects) $(objects) $(prc_resources)
	g++ $(01gurps_objects) $(objects) $(prc_resources) -o ~/bin/01gurps -lpthread -ldl -lreadline -lX11 `pkg-config --libs gtk+-2.0`

