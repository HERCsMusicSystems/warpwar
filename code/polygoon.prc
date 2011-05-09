import studio

program polygoon #machine := "polygoon" [grid dip insert_tank]

#machine grid := "grid"
#machine dip := "dip"
#machine insert_tank := "insert_tank"

end := [
	[insert_tank 20 -20]
	[insert_tank 150 50]
	[insert_tank -200 99]
	[command]
	] .
