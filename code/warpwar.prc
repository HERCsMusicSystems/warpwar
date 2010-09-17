
import star_names
import studio
import store
import f1
import help

program warpwar
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
		near
		get_star insert_star economy_table
		;save load
		addvarcl save clear
		; era build_points starship systemship
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
[[create base *race *location] [var [*bp 64]] [addcl [[build_points *race *location *bp]]]]
[[create star *star *economy : *location] [addcl [[star *star *location]]] [addcl [[economy *star *economy]]]]
[[create route *from *to] [addcl [[route *from *to]]]]

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

[[disp : *]]

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

[[insert_star]
	[get_star *name]
	[rnd *x 10 30] [rnd *y 10 30]
	[not res
		[star *other_star *other_location]
		[near *other_location [*x *y]]		
		[show [*other_star *other_location [*x *y]]]
	]
	[rnd *econ 0 28]
	[economy_table *econ *economy]
	[create star *name *economy *x *y]
	[show "Inserting " *name " at " [*x *y]]
]

end := [ [auto_atoms] [preprocessor f1]

		[wait *seed] [rnd_control *seed]
		[get_star *star] [show *star]

		[create race human]
		[create base human earth]
		[create base human moon]
		[create starship barracuda moon [beams 4] [screens 4] [tubes 2] [missiles 2] [systemship_racks 4]]
		[create systemship piranha [barracuda 3] [beams 6] [screens 6]]
		[create star sun 4 127 128]
		[create star moon 7 127 129]
		[create race hohd]
		[create base hohd hohdan]
		[create star hohdan 4 22 21]

		[command] ] .
