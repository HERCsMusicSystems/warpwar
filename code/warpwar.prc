
import star_names
import studio
import store
import f1
import help

program warpwar #machine := "galaxy"
	[
		human sun earth moon hohd hohdan bizh vul
		barracuda piranha
		era race location build_points economy
		starship systemship presence base star route
		warp_generator power_drive beams screens tubes missiles
		systemship_racks empty damaged
		ecm armor cannons shells holds repair_bays
		create add_features add_feature
		disp disp_features
		near near_eq
		get_star insert_star economy_table
		;save load
		addvarcl save clear
		; era build_points starship systemship
		grid orientation erase hexside zero draw_star star_colour draw_route
		draw_galaxy draw_base draw_bases draw_presence draw_presences
		route_less distance choose_greater route_eligible
		fetch_bp
		possible_move move
		generate_galaxy
		locate_ship
	]

#machine grid := "grid"
#machine orientation := "orientation"
#machine erase := "erase"
#machine hexside := "hexside"
#machine zero := "zero"
#machine draw_star := "draw_star"
#machine draw_route := "draw_route"
#machine star_colour := "star_colour"
#machine draw_base := "draw_base"
#machine draw_presence := "draw_presence"


[[route_less [[**] *d1] [[**] *d2]] [less *d1 *d2]]
[[choose_greater *x *y *x] [less *y *x]/]
[[choose_greater *x *y *y]/]
[[distance [*x1 *y1] [*x2 *y2] *d]/
	[sub *x2 *x1 *dx] [sub *y2 *y1 *dy]
	[SELECT
		[[less_eq 0 *dx]
			[SELECT
				[[less_eq 0 *dy] [choose_greater *dx *dy *d]]
				[[sub *dx *dy *d]]
			]
		]
		[[SELECT
			[[less *dy 0] [abs *dx *adx] [abs *dy *ady] [choose_greater *adx *ady *d]]
			[[sub *dy *dx *d]]
		]]
	]/
]
[[distance *s1 *s2 *d]/
	[star *s1 *l1]
	[star *s2 *l2]
	[distance *l1 *l2 *d]/
]

[[addvarcl *clause *x] [var [*v *x]] [APPEND *clause [*v] *clv] [addcl [*clv]]]

[[save *file_name]
	[file_writer *tc *file_name]
	[save *tc era] [*tc "\n"]
	[save *tc build_points] [*tc "\n"]
	[save *tc base] [*tc "\n"]
	[save *tc star] [*tc "\n"]
	[save *tc economy] [*tc "\n"]
	[save *tc route] [*tc "\n"]
	[save *tc starship] [*tc "\n"]
	[save *tc systemship] [*tc "\n"]
	[*tc "[exit]\n"]
	[*tc]
]

[[save *tc *relation]
	[PROBE
		[cl [[*relation : *tail]]] [APPEND *front [*v] *tail]
		[SELECT
			[[has_machine *v] / [*v : *value] [*tc [[addvarcl [*relation : *front] *value]] "\n"]]
			[[*tc [[addcl [[*relation : *tail]]]] "\n"]]
		]
		fail
	]
]

[[clear]
	[delallcl era]
	[delallcl build_points]
	[delallcl star]
	[delallcl economy]
	[delallcl route]
	[delallcl starship]
	[delallcl systemship]
]

[[create race *race] [var [*era 0]] [addcl [[era *race *era]]]]
[[create presence *race *location] / [create presence *race *location 0]]
[[create presence *race *location *initial] [var [*bp *initial]] [addcl [[build_points *race *location *bp]]]]
[[create base *race *location *multiplier]
	[build_points *race *location *bp]
	[times *multiplier 15 *cost]
	[*bp : *bpc] [less_eq *cost *bpc]
	[dec *bp *cost]
	[addcl [[base *race *location *multiplier]]]
]
[[create star *star *economy : *location] [addcl [[star *star *location]]] [addcl [[economy *star *economy]]]]
[[create route *from *to]
	[not route *from *to]
	[not route *to *from]
	[addcl [[route *from *to]]]
]
[[create route * *]]

[[create starship *name *location : *features]
	[base *race *location : *]
	[build_points *race *location *bp]
	[addcl [[starship *name race *race]]]
	[var [*loc *location]] [addcl [[starship *name location *loc]]]
	[dec *bp 5] [addcl [[starship *name warp_generator 5]]]
	/ [add_features starship *name *bp : *features]
]

[[create systemship *name *location : *features]
	[SELECT
		[[base *race *location : *] [build_points *race *location *bp]]
		[
			[eq *location [*s *d]]
			[starship *s race *race]
			[starship *s systemship_racks *d *dock]
			[starship *s location *loc] [*loc : *bp_location]
			[*dock : *status] [eq *status empty] [*dock *name]
			[base *race *bp_location : *]
			[build_points *race *bp_location *bp]
		]
	]
	[addcl [[systemship *name race *race]]]
	[var [*local *location]] [addcl [[systemship *name location *local]]]
	/ [add_features systemship *name *bp : *features]
]

[[add_features * * *]/]
[[add_features *type *name *bp [*feature *x] : *features]
	[add_feature *type *name *feature *x]
	[dec *bp *x] / [add_features *type *name *bp : *features]
]

[[add_feature *type *name power_drive *x] [var [*c *x]] [addcl [[*type *name power_drive *x *c]]]]
[[add_feature *type *name beams *x] [var [*c *x]] [addcl [[*type *name beams *x *c]]]]
[[add_feature *type *name screens *x] [var [*c *x]] [addcl [[*type *name screens *x *c]]]]
[[add_feature *type *name ecm *x] [var [*c *x]] [addcl [[*type *name ecm *x *c]]]]
[[add_feature *type *name tubes *x] [var [*c *x]] [addcl [[*type *name tubes *x *c]]]]
[[add_feature *type *name missiles *x] [times *x 3 *m] [var [*c *m]] [addcl [[*type *name missiles *x *c]]]]
[[add_feature *type *name armor *x] [times *x 2 *a] [var [*c *a]] [addcl [[*type *name armor *x *a]]]]
[[add_feature *type *name cannons *x] [var [*c *x]] [addcl [[*type *name cannons *x *c]]]]
[[add_feature *type *name shells *x] [times *x 6 *s] [var [*c *s]] [addcl [[*type *name shells *s *a]]]]
[[add_feature starship *name systemship_racks *x]
	[less 0 *x] [var [*c empty]] [addcl [[starship *name systemship_racks *x *c]]]
	[sub *x 1 *next] / [add_feature starship *name systemship_racks *next]
]
[[add_feature starship *name systemship_racks *x] [less_eq *x 0]]
[[add_feature starship *name holds *x]
	[less 0 *x] [var [*c 0]] [addcl [[starship *name holds *x *c]]]
	[sub *x 1 *next] / [add_feature starship *name holds *next]
]
[[add_feature starship *name holds *x] [less_eq *x 0]]
[[add_feature starship *name repair_bays 5] [var [*c 5]] [addcl [[starship *name repair_bays 5 *c]]]]


[[disp] [disp * *] fail]
[[disp race] [PROBE [disp race *] fail] /]
[[disp starship] [PROBE [disp starship *] fail] /]
[[disp systemship] [PROBE [disp systemship *] fail] /]
[[disp location] [PROBE [disp location *] fail] /]
[[disp *x] [disp race *x]]
[[disp *x] [disp starship *x]]
[[disp *x] [disp systemship *x]]
[[disp *x] [disp location *x]]

[[disp race *race]
	[era *race *era] [*era : *e]
	[nl] [show *race " era = " *e]
	[show "Locations:"]
	[PROBE
		[build_points *race *location *bp] [*bp : *v]
		[SELECT
			[[is_var *location] /
				[show "	TOTAL BP: " *v]
				[base *race *location *m]
				[economy *location *economy]
				[show "	" *location " BASE:" *m " ECONOMY:" *economy]
			]
			[
				[economy *location *economy]
				[ONE [SELECT
					[[base *race *location *m] [show "	" *location " BP:" *v " BASE:" *m " ECONOMY:" *economy]]
					[[show "	" *location " BP:" *v " ECONOMY:" *economy]]
				]]
			]
		]
;		[economy *location *economy]
		fail
	]
	[show "Starships:"]
	[PROBE [starship *s race *race] [starship *s location *l] [*l : *loc] [show "	" *s " on " *loc] fail]
	[show "Systemships:"]
	[PROBE [systemship *s race *race] [systemship *s location *l] [*l : *loc] [show "	" *s " on " *loc] fail]
]

[[disp location *star]
	[star *star : *]
	[nl] [show "STAR: " *star]
	[PROBE [star *star *xy] [show "LOCATION: " *xy]]
	[PROBE [build_points *race *star *bp] [*bp : *bpv] [show "PRESENCE: " *race " (" *bpv ")"]]
	[PROBE [base *race *star *m] [show "BASE: " *race " (" *m ")"]]
	[PROBE [economy *star *economy] [show "ECONOMY: " *economy]]
	[write "STARSHIPS: "] [PROBE [starship *starship location *la] [*la : *x] [eq *x *star] [write [*starship] " "] fail] [nl]
	[write "SYSTEMSHIPS: "] [PROBE [systemship *systemship location *lsa] [*lsa : *sx] [eq *sx *star] [write [*systemship] " "] fail] [nl]
]

[[disp starship *name]
	[starship *name race *race]
	[starship *name location *location] [*location : *l]
	[nl] [show *name ": " *race "'s starship on " *l]
	[disp_features starship *name]
]

[[disp systemship *name]
	[systemship *name race *race]
	[systemship *name location *location] [*location : *l]
	[nl] [show *name ": " *race "'s systemship on " *l]
	[disp_features systemship *name]
]

[[disp_features *type *name]
	[*type *name *feature *initial *current]
	[*current : *c]
	[show "	" *feature " = " *initial " <" *c ">"]
	fail
]
[[disp_features : *]]

[[disp *]]

[[near_eq *x *x]]
[[near_eq *x *y] [near *x *y]]
[[near_eq *star *near] [star *star *location] [near_eq *location *near]]

[[near [*x *y] [*x *y1]] [sum *y 1 *y1]]
[[near [*x *y] [*x *y1]] [sum *y -1 *y1]]
[[near [*x *y] [*x1 *y]] [sum *x 1 *x1]]
[[near [*x *y] [*x1 *y]] [sum *x -1 *x1]]
[[near [*x *y] [*x1 *y1]] [sum *x 1 *x1] [sum *y 1 *y1]]
[[near [*x *y] [*x1 *y1]] [sum *x -1 *x1] [sum *y -1 *y1]]
;[[near *star *near] [star *star *location] [near *location *near]]
;[[near *starship *near] [starship *starship location *atom] [*atom : *location] [show [*atom *location]] [near *location *near]]

[[possible_move *s1 *s2] [route *s1 *s2]]
[[possible_move *s1 *s2] [route *s2 *s1]]
[[possible_move *s1 *s2] [star *s1 *l1] [star *s2 *l2] [near *l1 *l2]]
[[possible_move *l1 *l2] [near *l1 *l2] [not star * *l1] [not star * *l2]]
[[possible_move *star *location] [star *star *l] [near *l *location]]
[[possible_move *location *star] [star *star *l] [near *l *location]]

[[move *from *from [*from]]]
[[move *from *to [*from : *tail]]
	[move *somewhere *to *tail]
	[possible_move *somewhere *from]
	[not ONLIST *from *tail]
	[show [*from : *tail]]
]

[[get_star *star] [rnd *x 0 1024] [star_name *star *x]]
[[get_star *star] / [get_star *star]]

[[economy_table *x 0] [less *x 4]]
[[economy_table *x 1] [less 3 *x 12]]
[[economy_table *x 2] [less 11 *x 20]]
[[economy_table *x 3] [less 19 *x 24]]
[[economy_table *x 4] [less 23 *x 26]]
[[economy_table *x 5] [less 25 *x 28]]

[[fetch_bp] /
	[PROBE
		[era *race : *]
		[fetch_bp *race]
		fail
	]
]

[[fetch_bp *race]
	[build_points *race *location *bp]
	[is_var *location] /
	[PROBE
		[economy *location *e]
		[ONE [SELECT
			[[base *race *location *m]]
			[[starship *s race *race] [starship *s location *loc] [*loc : *l] [eq *l *location] [eq *m 1]]
		]]
		[times *e *m *em]
		[show [*race *location *e *em]]
		[inc *bp *em]
		fail
	]
]

[[fetch_bp *race] /
	[PROBE
		[build_points *race *location *bp]
		[economy *location *e]
		[ONE [SELECT
			[[base *race *location *m] [times *e *m *em]]
			[[div *e 2 *em]]
		]]
		[show [*race *location *e *em]]
		[inc *bp *em]
		fail
	]
]


[[insert_star] / [insert_star 2 24 2 24]]
[[insert_star *low_x *high_x *low_y *high_y]
	[get_star *name]
	[not star *name : *]
	[rnd *x *low_x *high_x] [rnd *y *low_y *high_y]
	[not res
		[star *other_star *other_location]
		[near_eq *other_location [*x *y]]		
		[show [*other_star *other_location [*x *y]]]
	]
	[rnd *econ 0 28]
	[economy_table *econ *economy]
	[create star *name *economy *x *y]
	[show "Inserting " *name " at " [*x *y]]
]

[[draw_galaxy]
	[erase] [grid]
	[star_colour 0 255 0] [draw_presences]
	[star_colour 255 255 0] [draw_bases]
	[star_colour 0 0 255]
	[PROBE
		[route *s1 *s2]
		[star *s1 *location1]
		[star *s2 *location2]
		[draw_route *location1 *location2]
		fail
	]
	[star_colour 255 0 0]
	[PROBE
		[star *star *location] [economy *star *e]
		[add *star ":" *e *star_name]
		[draw_star *star_name : *location]
		fail
	]
	[show "GALAXY DRAWED."]
]

[[draw_bases]
	[PROBE [base *race *star : *] [star *star *location] [draw_base *race : *location] fail]
]

[[draw_presences]
	[isall *starships [*race : *location] [starship *starship race *race] [starship *starship location *l] [*l : *lx]
		[SELECT
			[[is_atom *lx] [star *lx *location]]
			[[eq *lx [*x *y]] [is_integer *x] [eq *lx *location]]
		]
	]
	[isall *systemships [*race : *location] [systemship *systemship race *race] [systemship *systemship location *l] [*l : *lx]
		[SELECT
			[[is_atom *lx] [star *lx *location]]
			[[eq *lx [*x *y]] [is_integer *x] [eq *lx *location]]
		]
	]
;	[show "starships => " *starships]
;	[show "systemships => " *systemships]
	[APPEND *starships *systemships *all]
;	[show "all => " *all]
	[NODUP *all *no_dup]
;	[show "to print: " *no_dup]
	[draw_presences *no_dup]
]
[[draw_presences []]]
[[draw_presences [*p : *t]]
	[draw_presence : *p]
	/ [draw_presences *t]
]

[[locate_ship *ship *location]
	[starship *ship location *l]
	[*l *location]
]
[[locate_ship *ship [*starship *dock]]
	[systemship *ship location *l]
	[starship *starship systemship_racks *dock *h]
	[*h : *hold] [eq *hold empty] /
	[*h *ship]
	[*l [*starship *dock]]
]
[[locate_ship *ship *location]
	[systemship *ship location *l]
	[*l : *bay]
	[eq *bay [*starship *dock]]
	[starship *starship systemship_racks *dock *h]/
	[*l *location]
	[*h empty]
]
[[locate_ship *ship *location]
	[systemship *ship location *l]
	[*l *location]
]

[[route_eligible [*star1 *star2] *distance] / [route_eligible 25 [*star1 *star2] *distance]]
[[route_eligible *rnd [*star1 *star2] *distance]
	[less 1 *distance]
	[rnd *x 0 100] [less *x *rnd]
	[create route *star1 *star2]
	[show [route *star1 *star2 *rnd *distance *x]]/
]
[[route_eligible : *]]

[[generate_galaxy]
	[FOR *x 1 36 1 [pp *x] [write ": "] [insert_star]]
	[PROBE
		[star *s1 :*]
		[isall *l [[*s1 *s2] *d] [star *s2 *] [distance *s1 *s2 *d]]
		[sort route_less *l [*r0 *r1 *r2 *r3 *r4 : *] []]
		[route_eligible 70 : *r1]
		[route_eligible 40 : *r2]
		[route_eligible 20 : *r3]
		[route_eligible 10 : *r4]
		fail
	]
]

end := [ [auto_atoms] [preprocessor f1]

		[wait *seed] [rnd_control *seed]
		[get_star *star] [show *star]

		[create race human]
		[create presence human earth 64]
		[create presence human moon 64]
		[create base human moon 1]
		[create starship barracuda moon [beams 4] [screens 4] [tubes 2] [missiles 2] [systemship_racks 4]]
		[create systemship piranha [barracuda 3] [beams 6] [screens 6]]
		[create star sun 4 12 12]
		[create star moon 7 12 11]
		[create race hohd]
		[create presence hohd hohdan 64]
		[create base hohd hohdan 1]
		[create star hohdan 4 22 21]

		[generate_galaxy]
		[draw_galaxy]

		[command] ] .


