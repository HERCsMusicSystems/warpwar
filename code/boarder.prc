
import studio

program boarder #machine := "boarder" [
					viewport background_colour repaint
					create_rectangle
					mariner viking voyager
					diagnostics test
                ]


#machine viewport := "viewport"
#machine background_colour := "background_colour"
#machine repaint := "repaint"

#machine create_rectangle := "create_rectangle"

#machine diagnostics := "diagnostics"

[[test]
	[viewport "SONDA" mariner]
	[viewport "SONDA" viking]
	[viewport "SONDA" voyager]
	[diagnostics]
]

end := [
			[auto_atoms]
			[command]
		] .
