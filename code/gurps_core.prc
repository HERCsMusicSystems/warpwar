import studio

program gursp_core [
						d4 d6 d8 d10 d100 d12 d20 DD
						CRITICAL_SUCCESS SUCCESS FAILURE CRITICAL_FAILURE CRITICAL SUCCESS FAILURE TIE
						roll roll_result quick_contest_result
						t
					]

auto := [[timestamp *seed : *] [rnd_control *seed] [show "SEED = " *seed]]

[[DD *x *dice *offset] [rnd 0 *y *dice] [sum *y *offset *x] [show "D" *dice " => " *x]]
[[DD *x *dice *offset *multiplier] / [DD *x *dice *offset *multiplier 0]]
[[DD *x *dice *offset *multiplier *shift] [<= *multiplier 1]
	[DD *y *dice *offset] [sum *y *shift *x]
]
[[DD *x *dice *offset *multiplier *shift] [< 1 *multiplier]
	[DD *a *dice *offset] [-- *multiplier *next]
	[DD *b *dice *offset *next *shift] [sum *a *b *x]
]

[[d4 *x : *command] / [DD *x 4 1 : *command] [show "Total = " *x]]
[[d6 *x : *command] / [DD *x 6 1 : *command] [show "Total = " *x]]
[[d8 *x : *command] / [DD *x 8 1 : *command] [show "Total = " *x]]
[[d10 *x : *command] / [DD *x 10 1 : *command] [show "Total = " *x]]
[[d100 *x : *command] / [DD *x 100 1 : *command] [show "Total = " *x]]
[[d12 *x : *command] / [DD *x 12 1 : *command] [show "Total = " *x]]
[[d20 *x : *command] / [DD *x 20 1 : *command] [show "Total = " *x]]

[[roll *effective_skill *status *margin]
	[d6 *result 3]
	[roll_result *result *effective_skill : *res]
	[show *res]
]

[[roll_result 3 *effective_skill CRITICAL_SUCCESS *margin] [sum 3 *margin *effective_skill]]
[[roll_result 4 *effective_skill CRITICAL_SUCCESS *margin] [sum 4 *margin *effective_skill]]
[[roll_result 5 *effective_skill CRITICAL_SUCCESS *margin] [>= *effective_skill 15] [sum 5 *margin *effective_skill]]
[[roll_result 6 *effective_skill CRITICAL_SUCCESS *margin] [>= *effective_skill 16] [sum 6 *margin *effective_skill]]
[[roll_result 18 *effective_skill CRITICAL_FAILURE *margin] [sum 18 *margin *effective_skill]]
[[roll_result 17 *effective_skill CRITICAL_FAILURE *margin] [<= *effective_skill 15] [sum 17 *margin *effective_skill]]
[[roll_result 17 *effective_skill FAILURE *margin] [> *effective_skill 15] [sum 17 *margin *effective_skill]]
[[roll_result *result *effective_skill CRITICAL_FAILURE *margin] [sum *result *margin *effective_skill] [<= *margin -10]]
[[roll_result *result *effective_skill SUCCESS *margin] [<= *result *effective_skill] [sum *result *margin *effective_skill]]
[[roll_result *result *effective_skill FAILURE *margin] [> *result *effective_skill] [sum *result *margin *effective_skill]]

[[quick_contest_result *a [CRITICAL_SUCCESS *mm] *b [CRITICAL_SUCCESS *mm] TIE     0  [*a CRITICAL_SUCCESS] [*b CRITICAL_SUCCESS]] /]
[[quick_contest_result *a [CRITICAL_SUCCESS *am] *b [CRITICAL_SUCCESS *bm] SUCCESS *m [*a CRITICAL_SUCCESS] [*b CRITICAL_SUCCESS]] [> *am *bm] / [sum *bm *m *am]]
[[quick_contest_result *a [CRITICAL_SUCCESS *am] *b [CRITICAL_SUCCESS *bm] SUCCESS *m [*b CRITICAL_SUCCESS] [*a CRITICAL_SUCCESS]] [< *am *bm] / [sum *am *m *bm]]
[[quick_contest_result *a [CRITICAL_SUCCESS *am] *b [*status          *bm] SUCCESS *m [*a CRITICAL_SUCCESS] [*b *status         ]]             / [sum *bm *m *am]]
[[quick_contest_result *a [*status          *am] *b [CRITICAL_SUCCESS *bm] SUCCESS *m [*b CRITICAL_SUCCESS] [*a *status         ]]             / [sum *am *m *bm]]
[[quick_contest_result *a [SUCCESS *mm] *b [SUCCESS *mm] TIE     0  [*a SUCCESS] [*b SUCCESS]] /]
[[quick_contest_result *a [SUCCESS *am] *b [SUCCESS *bm] SUCCESS *m [*a SUCCESS] [*b SUCCESS]] [> *am *bm] / [sum *bm *m *am]]
[[quick_contest_result *a [SUCCESS *am] *b [SUCCESS *bm] SUCCESS *m [*b SUCCESS] [*a SUCCESS]] [< *am *bm] / [sum *am *m *bm]]
[[quick_contest_result *a [SUCCESS *am] *b [*status *bm] SUCCESS *m [*a SUCCESS] [*b *status]]             / [sum *bm *m *am]]
[[quick_contest_result *a [*status *am] *b [SUCCESS *bm] SUCCESS *m [*b SUCCESS] [*a *status]]             / [sum *am *m *bm]]
[[quick_contest_result *a [FAILURE *mm] *b [FAILURE *mm] TIE     0  [*a FAILURE] [*b FAILURE]] /]
[[quick_contest_result *a [FAILURE *am] *b [FAILURE *bm] SUCCESS *m [*a FAILURE] [*b FAILURE]] [> *am *bm] / [sum *bm *m *am]]
[[quick_contest_result *a [FAILURE *am] *b [FAILURE *bm] SUCCESS *m [*b FAILURE] [*a FAILURE]] [< *am *bm] / [sum *am *m *bm]]
[[quick_contest_result *a [FAILURE *am] *b [*status *bm] SUCCESS *m [*a FAILURE] [*b *status]]             / [sum *bm *m *am]]
[[quick_contest_result *a [*status *am] *b [FAILURE *bm] SUCCESS *m [*b FAILURE] [*a *status]]             / [sum *am *m *bm]]
[[quick_contest_result *a [CRITICAL_FAILURE *mm] *b [CRITICAL_FAILURE *mm] TIE     0  [*a CRITICAL_FAILURE] [*b CRITICAL_FAILURE]] /]
[[quick_contest_result *a [CRITICAL_FAILURE *am] *b [CRITICAL_FAILURE *bm] SUCCESS *m [*a CRITICAL_FAILURE] [*b CRITICAL_FAILURE]] [> *am *bm] / [sum *bm *m *am]]
[[quick_contest_result *a [CRITICAL_FAILURE *am] *b [CRITICAL_FAILURE *bm] SUCCESS *m [*b CRITICAL_FAILURE] [*a CRITICAL_FAILURE]] [< *am *bm] / [sum *am *m *bm]]

[[t *x *y] [roll_result *x *y : *res] [show *res]]

end .


































