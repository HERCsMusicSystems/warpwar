
import studio
import f1

program boarder #machine := "boarder" [
					viewport background_colour foreground_colour repaint
					create_rectangle
					location size position scaling
					mariner viking voyager mix
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
	[create_rectangle mix]
	[diagnostics]
]

end := [
			[preprocessor f1]
			[auto_atoms]
			[test]
			[command]
		] .
