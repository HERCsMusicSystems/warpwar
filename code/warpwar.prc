

import studio
import store
import f1

program warpwar
	[
		human sun earth moon hohd hohdan bizh vul
		barracuda piranha
		era race location build_points
		starship systemship base star route
		warp_generator power_drive beams screens tubes missiles
		systemship_racks empty damaged
		ecm armor cannons shells holds repair_bays
		create add_features add_feature
		disp disp_features
		;save load
	]


[[create race *race] [var [*era 0]] [addcl [[era *race *era]]]]
[[create base *race *location] [var [*bp 64]] [addcl [[build_points *race *location *bp]]]]
[[create star *star : *location] [addcl [[star *star *location]]]]
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


[[disp *x] [disp race *x]]
[[disp *x] [disp starship *x]]
[[disp *x] [disp systemship *x]]
[[disp *x] [disp location *x]]

[[disp race *race]
	[era *race *era] [*era : *e]
	[nl] [show *race " era = " *e]
	[show "Locations:"]
	[TRY [build_points *race *location *bp] [*bp : *v] [show "	" *location " = " *v] fail]
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

[[disp location *star]
	[nl] [show "STAR: " *star]
	[TRY [star *star *xy] [show "LOCATION: " *xy]]
	[TRY [build_points *race *star *bp] [*bp : *bpv] [show "PRESENCE: " *race " (" *bpv ")"]]
	[write "STARSHIPS: "] [TRY [starship *starship location *la] [*la : *x] [eq *x *star] [write [*starship] " "] fail] [nl]
]

[[disp_features *type *name]
	[*type *name *feature *initial *current]
	[*current : *c]
	[show "	" *feature " = " *initial " <" *c ">"]
	fail
]
[[disp_features : *]]

[[disp *x]]


end := [ [auto_atoms] [preprocessor f1]

		[create race human]
		[create base human earth]
		[create base human moon]
		[create starship barracuda moon [beams 4] [screens 4] [tubes 2] [missiles 2] [systemship_racks 4]]
		[create systemship piranha [barracuda 3] [beams 6] [screens 6]]
		[create star sun 127 128]
		[create race hohd]
		[create base hohd hohdan]

		[command] ] .
