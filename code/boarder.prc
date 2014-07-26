
import studio

program boarder #machine := "boarder" [
					Viewport BackgroundColour ForegroundColour Repaint SaveBoard Clean Clean? Erase Indexing NoIndexing Indexed?
					CreateRectangle CreateCircle CreatePicture CreatePictures CreateText CreateDice CreateGrid CreateDeck TokenType
					Location Size Position Mode Scaling Rotation Sides Text Side Roll Shuffle Insert Release ReleaseRandom SelectDeck
					Lock Unlock Locked? Select Deselect Selected? Selection
					DragAndDrop HitTest IsDeck?
					rep

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
#machine HitTest := "HitTest"
#machine Selection := "Selection"
#machine IsDeck? := "IsDeck?"


#machine CreateRectangle := "CreateRectangle"
#machine CreateCircle := "CreateCircle"
#machine CreatePicture := "CreatePicture"
#machine CreateText := "CreateText"
#machine CreateDice := "CreateDice"
#machine CreateGrid := "CreateGrid"
#machine CreateDeck := "CreateDeck"

#machine TokenType := "TokenType"

#machine diagnostics := "diagnostics"

[[rep : *command] *command [Repaint]]

[[DragAndDrop *x *y] [Repaint]]
[[DragAndDrop *x *y *file : *command]
	[relativise_path *file *relative]
	[show "action => " [*x *y *file *relative]]
	[CreatePicture *picture *relative] [*picture Position *x *y]
	[add *x 8 *x1] [add *y 8 *y1]
	/ [DragAndDrop *x1 *y1 : *command]
]

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
			[auto_atoms]
			[TRY [batch "sonda.txt"]]
			[command]
			[SELECT
				[[Clean?] [show "Nothing to save."]]
				[[show [SaveBoard "sonda.txt"]] [show "Board saved."]]
			]
		] .

