
import studio
import f1

program boarder #machine := "boarder" [
					Viewport BackgroundColour ForegroundColour Repaint SaveBoard Clean Clean? Erase
					CreateRectangle CreateCircle CreatePicture CreateText CreateDice
					Location Size Position Scaling Rotation Sides Side Roll
					Lock Unlock Locked? Select Deselect Selected?
					mariner viking voyager mix
					diagnostics test
                ]


#machine Viewport := "Viewport"
#machine BackgroundColour := "BackgroundColour"
#machine Repaint := "Repaint"
#machine SaveBoard := "SaveBoard"
#machine Clean := "Clean"
#machine Clean? := "Clean?"
#machine Erase := "Erase"


#machine CreateRectangle := "CreateRectangle"
#machine CreateCircle := "CreateCircle"
#machine CreatePicture := "CreatePicture"
#machine CreateText := "CreateText"
#machine CreateDice := "CreateDice"

#machine diagnostics := "diagnostics"

[[test]
	[CreateRectangle mix]
	[Viewport "SONDA" mariner]
	[Viewport "SONDA" viking]
	[Viewport "SONDA" voyager]
	[diagnostics]
]

end := [
			[preprocessor f1]
			[auto_atoms]
			[TRY [batch "sonda.txt"]]
			[command]
			[SELECT
				[[Clean?] [show "Nothing to save."]]
				[[SaveBoard "sonda.txt"] [show "Board saved."]]
			]
		] .

