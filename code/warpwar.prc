
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
		starship systemship base star route
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
		draw_galaxy
		sort divide route_less distance choose_greater route_eligible
		fetch_bp
	]

#machine grid := "grid"
#machine orientation := "orientation"
#machine erase := "erase"
#machine hexside := "hexside"
#machine zero := "zero"
#machine draw_star := "draw_star"
#machine draw_route := "draw_route"
#machine star_colour := "star_colour"


[[divide *less *x [] *lt *lt *gt *gt]/]
[[divide *less *x [*h : *t] *lt *lta [*h : *gt] *gta] [*less *x *h] / [divide *less *x *t *lt *lta *gt *gta]]
[[divide *less *x [*h : *t] [*h : *lt] *lta *gt *gta] / [divide *less *x *t *lt *lta *gt *gta]]

[[sort *less [] *gs *gs]/]
[[sort *less [*h : *t] *gts *gtsa]
	[divide *less *h *t *lt *lta *gt *gta]/
	[sort *less *lt *gts [*h : *gs]]/
	[sort *less *gt *gs *gtsa]/
;	[eq *gtsa *gsa] [eq *gts *ls] [eq *lsa [*h : *gs]]
]

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
[[create base *race *location] / [create base *race *location 0]]
[[create base *race *location *initial] [var [*bp *initial]] [addcl [[build_points *race *location *bp]]]]
[[create star *star *economy : *location] [addcl [[star *star *location]]] [addcl [[economy *star *economy]]]]
[[create route *from *to]
	[not route *from *to]
	[not route *to *from]
	[addcl [[route *from *to]]]
]
[[create route * *]]

[[create starship *name *location : *features]
	[build_points *race *location *bp]
	[addcl [[starship *name race *race]]]
	[var [*loc *location]] [addcl [[starship *name location *loc]]]
	[dec *bp 5] [addcl [[starship *name warp_generator 5]]]
	/ [add_features starship *name *bp : *features]
]

[[create systemship *name *location : *features]
	[SELECT
		[[build_points *race *location *bp]]
		[
			[eq *location [*s *d]]
			[starship *s race *race]
			[starship *s systemship_racks *d *dock]
			[starship *s location *loc] [*loc : *bp_location]
			[*dock : *status] [eq *status empty] [*dock *name]
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
	[PROBE [build_points *race *location *bp] [*bp : *v] [show "	" *location " = " *v] fail]
]

[[disp location *star]
	[star *star : *]
	[nl] [show "STAR: " *star]
	[PROBE [star *star *xy] [show "LOCATION: " *xy]]
	[PROBE [build_points *race *star *bp] [*bp : *bpv] [show "PRESENCE: " *race " (" *bpv ")"]]
	[PROBE [economy *star *economy] [show "ECONOMY: " *economy]]
	[write "STARSHIPS: "] [PROBE [starship *starship location *la] [*la : *x] [eq *x *star] [write [*starship] " "] fail] [nl]
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

[[near [*x *y] [*x *y1]] [sum *y 1 *y1]]
[[near [*x *y] [*x *y1]] [sum *y -1 *y1]]
[[near [*x *y] [*x1 *y]] [sum *x 1 *x1]]
[[near [*x *y] [*x1 *y]] [sum *x -1 *x1]]
[[near [*x *y] [*x1 *y1]] [sum *x 1 *x1] [sum *y 1 *y1]]
[[near [*x *y] [*x1 *y1]] [sum *x -1 *x1] [sum *y -1 *y1]]
[[near *star *near] [star *star *location] [near *location *near]]
[[near *starship *near] [starship *starship location *atom] [*atom : *location] [show [*atom *location]] [near *location *near]]

[[get_star *star] [rnd *x 0 1024] [star_name *star *x]]
[[get_star *star] / [get_star *star]]

[[economy_table *x 0] [less *x 4]]
[[economy_table *x 1] [less 3 *x 12]]
[[economy_table *x 2] [less 11 *x 20]]
[[economy_table *x 3] [less 19 *x 24]]
[[economy_table *x 4] [less 23 *x 26]]
[[economy_table *x 5] [less 25 *x 28]]

[[fetch_bp] / [fetch_bp 1]]
[[fetch_bp *m]
	[PROBE
		[build_points *race *location *bp]
		[economy *location *e]
		[inc *bp *e]
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
	[star_colour 0 0 255]
	[PROBE
		[route *s1 *s2]
		[star *s1 *location1]
		[star *s2 *location2]
		;[show [*location1 *location2]]
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
]

[[route_eligible [*star1 *star2] *distance] / [route_eligible 25 [*star1 *star2] *distance]]
[[route_eligible *rnd [*star1 *star2] *distance]
	[less 1 *distance]
	[rnd *x 0 100] [less *x *rnd]
	[create route *star1 *star2]
	[show [route *star1 *star2 *rnd *distance *x]]/
]
[[route_eligible : *]]

end := [ [auto_atoms] [preprocessor f1]

		[wait *seed] [rnd_control *seed]
		[get_star *star] [show *star]

		[create race human]
		[create base human earth 64]
		[create base human moon 64]
		[create starship barracuda moon [beams 4] [screens 4] [tubes 2] [missiles 2] [systemship_racks 4]]
		[create systemship piranha [barracuda 3] [beams 6] [screens 6]]
		[create star sun 4 12 12]
		[create star moon 7 12 11]
		[create race hohd]
		[create base hohd hohdan 64]
		[create star hohdan 4 22 21]

		[FOR *x 1 36 1 [pp *x] [write ": "] [insert_star]]
		[PROBE
			[star *s1 :*]
			[isall *l [[*s1 *s2] *d] [star *s2 *] [distance *s1 *s2 *d]]
			[sort route_less *l [*r0 *r1 *r2 *r3 *r4 : *] []]
			[route_eligible 70 : *r1]
			[route_eligible 40 : *r2]
			[route_eligible 20 : *r3]
			[route_eligible 10 : *r4]
			;[PROBE [rnd *p1 0 90] [less *p1 40] [show [*p1 4 : *r1]] [create route : *r1]]
			;[PROBE [rnd *p2 0 90] [less *p2 30] [show [*p2 3 : *r2]] [create route : *r2]]
			;[PROBE [rnd *p3 0 90] [less *p3 20] [show [*p3 2 : *r3]] [create route : *r3]]
			;[PROBE [rnd *p4 0 90] [less *p4 10] [show [*p4 1 : *r4]] [create route : *r4]]
			fail
		]

		[orientation]
		[draw_galaxy]

		[command] ] .
