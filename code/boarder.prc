
import studio
import f1

program boarder #machine := "boarder" [
					Viewport BackgroundColour ForegroundColour Repaint SaveBoard Clean Clean? Erase Indexing NoIndexing Indexed?
					CreateRectangle CreateCircle CreatePicture CreatePictures CreateText CreateDice CreateGrid CreateDeck
					Location Size Position Scaling Rotation Sides Side Roll Shuffle Insert Release SelectDeck
					Lock Unlock Locked? Select Deselect Selected?
					mariner viking voyager mix
					diagnostics test
					
					DefaultRectangleForeground DefaultRectangleBackground
					DefaultCircleForeground DefaultCircleBackground
					DefaultPictureForeground DefaultPictureBackground
					DefaultTextForeground DefaultTextBackground
					DefaultDeckForeground DefaultDeckBackground
					DefaultGridForeground DefaultGridBackground
					
					DefaultDiceForeground DefaultDiceBackground
					DefaultTetrahedronForeground DefaultTetrahedronBackground
					DefaultCubeForeground DefaultCubeBackground
					DefaultOctahedronForeground DefaultOctahedronBackground
					DefaultDeltahedronForeground DefaultDeltahedronBackground
					DefaultDeltahedron10Foreground DefaultDeltahedron10Background
					DefaultDodecahedronForeground DefaultDodecahedronBackground
					DefaultIcosahedronForeground DefaultIcosahedronBackground
                ]

#machine DefaultRectangleForeground := "DefaultRectangleForeground"

#machine Viewport := "Viewport"
#machine BackgroundColour := "BackgroundColour"
#machine Repaint := "Repaint"
#machine SaveBoard := "SaveBoard"
#machine Clean := "Clean"
#machine Clean? := "Clean?"
#machine Erase := "Erase"
#machine SelectDeck := "SelectDeck"


#machine CreateRectangle := "CreateRectangle"
#machine CreateCircle := "CreateCircle"
#machine CreatePicture := "CreatePicture"
#machine CreateText := "CreateText"
#machine CreateDice := "CreateDice"
#machine CreateGrid := "CreateGrid"
#machine CreateDeck := "CreateDeck"

#machine diagnostics := "diagnostics"

[[CreatePictures]]
[[CreatePictures *scaling] [is_number *scaling]]
[[CreatePictures *scaling *picture : *pictures]
	[is_number *scaling]
	[CreatePicture *picture]
	[*picture Scaling *scaling]
	/ [CreatePictures *scaling : *pictures]
]
[[CreatePictures *picture : *pictures]
	[CreatePicture *picture]
	/ [CreatePictures : *pictures]
]

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

