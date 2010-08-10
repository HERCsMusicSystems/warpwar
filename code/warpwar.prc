

import studio
import store
import f1

program warpwar
	[
		human earth moon hohd hohdan bizh vul
		era race location build_points
		starship systemship base
		warp_generator power_drive beams screens tubes missiles systemship_racks empty damaged
		create add_features add_feature
		disp
		;save load
	]

[[starship]]
[[systemship]]

[[create race *race]
	[var [*era 0]] [addcl [[era *race *era]]]
]
[[create base *race *location]
	[var [*bp 64]] [addcl [[build_points *race *location *bp]]]
]

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
			[show "BUILD POINTS => " *bp]
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
[[add_feature *type *name tubes *x] [var [*c *x]] [addcl [[*type *name tubes *x *c]]]]
[[add_feature *type *name missiles *x] [times *x 3 *m] [var [*c *m]] [addcl [[*type *name missiles *x *c]]]]
[[add_feature starship *name systemship_racks *x]
	[less 0 *x] [var [*c empty]] [addcl [[starship *name systemship_racks *x *c]]]
	[sub *x 1 *next] / [add_feature starship *name systemship_racks *next]
]
[[add_feature starship *name systemship_racks *x] [less_eq *x 0]]


[[disp *x] [disp race *x]]
[[disp *x] [disp starship *x]]
[[disp *x] [disp systemship *x]]

[[disp race *race]
	[era *race *era] [*era : *e]
	[show *race " era = " *e]
	[show "Locations:"]
	[build_points *race *location *bp]
	[*bp : *v]
	[show "	" *location " = " *v]
	fail
]

[[disp starship *name]
	[starship *name *race]
	[starship *name *location]
	[show *name ": " *race "'s starship on " *location]
]

[[disp systemship *name]
	[systemship *name race *race]
	[systemship *name location *location] [*location : *l]
	[show *name ": " *race "'s systemship on " *l]
]

[[disp *x]]


end := [ [auto_atoms] [preprocessor f1]

		[create race human]
		[create base human earth]
		[create base human moon]
		[create race hohd]
		[create base hohd hohdan]

		[command] ] .
