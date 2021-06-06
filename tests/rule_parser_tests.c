#include "../src/teno_rules.h"
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <string.h>
#include <time.h>
Describe(Rule);
BeforeEach(Rule)
{
}
AfterEach(Rule)
{
}

Ensure(Rule, returns_rule_when_rule_got_found)
{
    unsigned int i;
    char md[254];
    char header[] = "#Header\n\nsome description\n";
    char date_rules[] = "ON 2021-01-01T11:55:11Z\nFROM 2022-02-02\nUNTIL 2023-03-03\n\n";
    char implicit_rules[] = "ON_VACATION ON_HOLIDAY ON_WEEKDAY ON_WEEKEND\n";
    strcat(md, header);
    strcat(md, date_rules);
    strcat(md, implicit_rules);
    struct TenoRule *result = teno_rules_parse_content((const unsigned char*) md, strlen((const char*) md));
	assert_that(result, is_not_null);
    assert_string_equal((char *) result->content, md);
    assert_equal(result->rule_ids[0], TENO_RULE_ON);
    assert_equal(result->rule_ids[1], TENO_RULE_FROM);
    assert_equal(result->rule_ids[2], TENO_RULE_UNTIL);
    assert_equal(result->rule_ids[3], TENO_RULE_ON_VACATION);
    assert_equal(result->rule_ids[4], TENO_RULE_ON_HOLIDAY);
    assert_equal(result->rule_ids[5], TENO_RULE_ON_WEEKDAY);
    assert_equal(result->rule_ids[6], TENO_RULE_ON_WEEKEND);
    for (i = 0; i < 3; i++){
        assert_equal(result->rule_datetimes[i].tm_year, 2021 - 1900 + i);
        assert_equal(result->rule_datetimes[i].tm_mon, 0 + i);
        assert_equal(result->rule_datetimes[i].tm_mday, 1+ i);
    }
}

Ensure(Rule, returns_last_rule_when_md_ends_with_rule)
{
    unsigned int i;
    char md[254];
    char header[] = "#Header\n\nsome description\n";
    char implicit_rules[] = "ON_VACATION ON_HOLIDAY ON_WEEKDAY ON_WEEKEND";
    strcat(md, header);
    strcat(md, implicit_rules);
    struct TenoRule *result = teno_rules_parse_content((const unsigned char*) md, strlen((const char*) md));
	assert_that(result, is_not_null);
    assert_equal(*result->rule_amount, 4);
}

TestSuite *rules_parser_tests()
{
	TestSuite *suite = create_test_suite();
	add_test_with_context(suite, Rule, returns_rule_when_rule_got_found);
	add_test_with_context(suite, Rule, returns_last_rule_when_md_ends_with_rule);
	return suite;
}
