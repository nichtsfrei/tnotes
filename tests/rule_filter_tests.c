#include "../src/tono_rules.h"
#include "../src/tono_rules_filter.h"
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <errno.h>
#include <string.h>
#include <time.h>
Describe(Filter);
BeforeEach(Filter) {}
AfterEach(Filter) {}
#define MONDAY 1623081612L
Ensure(Filter, returns_null_when_using_unknown_rule_id)
{
	unsigned int i;
	int (*p[])(enum TENO_RULE_ID, struct tm *, struct tm *) = {
	    &tono_rules_cmp_on_id,
	    &tono_rules_cmp_from_id,
	    &tono_rules_cmp_until_id,
	    //&tono_rules_cmp_holiday_id,
	    //&tono_rules_cmp_vacation_id,
	    &tono_rules_cmp_weekday_id,
	    &tono_rules_cmp_weekend_id};
	int size = sizeof p / sizeof *p;
	assert_true(size > 0);
	for (i = 0; i < size; i++) {
		assert_equal(p[i](TENO_RULE_NONE, NULL, NULL),
			     TENO_RULE_ERRNO_UNFITTING_CMP);
	}
	// except for generic cmp
}
Ensure(Filter, return_0_on_match)
{
	unsigned int i;
	time_t monday = MONDAY;
	time_t tuesday = MONDAY + 24 * 60 * 60;
	time_t saturday = MONDAY - 2 * 24 * 60 * 60;
	int (*p[])(enum TENO_RULE_ID, struct tm *, struct tm *) = {
	    &tono_rules_cmp_on_id,
	    &tono_rules_cmp_from_id,
	    &tono_rules_cmp_until_id,
	    //&tono_rules_cmp_holiday_id,
	    //&tono_rules_cmp_vacation_id,
	    &tono_rules_cmp_weekday_id,
	    &tono_rules_cmp_weekend_id};
	enum TENO_RULE_ID r[] = {TENO_RULE_ON,
				 TENO_RULE_FROM,
				 TENO_RULE_UNTIL,
				 // TENO_RULE_ON_HOLIDAY,
				 // TENO_RULE_ON_VACATION,
				 TENO_RULE_ON_WEEKDAY,
				 TENO_RULE_ON_WEEKEND};
	struct tm td[][2] = {
	    {*localtime(&monday), *localtime(&monday)},
	    {*localtime(&tuesday), *localtime(&monday)},
	    {*localtime(&monday), *localtime(&tuesday)},
	    //{localtime(&monday), NULL},
	    //{localtime(&monday), NULL},
	    {*localtime(&monday), *localtime(&monday)},
	    {*localtime(&saturday), *localtime(&monday)},
	};

	int size = sizeof p / sizeof *p;
	assert_true(size > 0);
	for (i = 0; i < size; i++) {
		assert_equal_with_message(
		    p[i](r[i], &td[i][0], &td[i][1]), 0, "index %d failed", i);
	}
}


TestSuite *rules_filter_tests()
{
	TestSuite *suite = create_test_suite();
	add_test_with_context(
	    suite, Filter, returns_null_when_using_unknown_rule_id);
	add_test_with_context(suite, Filter, return_0_on_match);
	return suite;
}
