
import studio
import f1

program boarder #machine := "boarder" [
					Viewport BackgroundColour ForegroundColour Repaint SaveBoard Clean Clean? Erase Indexing NoIndexing Indexed?
					CreateRectangle CreateCircle CreatePicture CreatePictures CreateText CreateDice CreateGrid CreateDeck
					Location Size Position Scaling Rotation Sides Side Roll Shuffle Insert Release ReleaseRandom SelectDeck
					Lock Unlock Locked? Select Deselect Selected?
					mariner viking voyager mix
					diagnostics test sv
					
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
#machine DefaultRectangleBackground := "DefaultRectangleBackground"
#machine DefaultCircleForeground := "DefaultCircleForeground"
#machine DefaultCircleBackground := "DefaultCircleBackground"
#machine DefaultPictureForeground := "DefaultPictureForeground"
#machine DefaultPictureBackground := "DefaultPictureBackground"
#machine DefaultTextForeground := "DefaultTextForeground"
#machine DefaultTextBackground := "DefaultTextBackground"
#machine DefaultDeckForeground := "DefaultDeckForeground"
#machine DefaultDeckBackground := "DefaultDeckBackground"
#machine DefaultGridForeground := "DefaultGridForeground"
#machine DefaultGridBackground := "DefaultGridBackground"
#machine DefaultDiceForeground := "DefaultDiceForeground"
#machine DefaultDiceBackground := "DefaultDiceBackground"
#machine DefaultTetrahedronForeground := "DefaultTetrahedronForeground"
#machine DefaultTetrahedronBackground := "DefaultTetrahedronBackground"
#machine DefaultCubeForeground := "DefaultCubeForeground"
#machine DefaultCubeBackground := "DefaultCubeBackground"
#machine DefaultOctahedronForeground := "DefaultOctahedronForeground"
#machine DefaultOctahedronBackground := "DefaultOctahedronBackground"
#machine DefaultDeltahedronForeground := "DefaultDeltahedronForeground"
#machine DefaultDeltahedronBackground := "DefaultDeltahedronBackground"
#machine DefaultDeltahedron10Foreground := "DefaultDeltahedron10Foreground"
#machine DefaultDeltahedron10Background := "DefaultDeltahedron10Background"
#machine DefaultDodecahedronForeground := "DefaultDodecahedronForeground"
#machine DefaultDodecahedronBackground := "DefaultDodecahedronBackground"
#machine DefaultIcosahedronForeground := "DefaultIcosahedronForeground"
#machine DefaultIcosahedronBackground := "DefaultIcosahedronBackground"

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

[[sv] [SaveBoard "sonda.txt"]]

end := [
			[preprocessor f1]
			[auto_atoms]
			[TRY [batch "sonda.txt"]]
			[command]
			[SELECT
				[[Clean?] [show "Nothing to save."]]
				[[show [SaveBoard "sonda.txt"]] [show "Board saved."]]
			]
		] .

