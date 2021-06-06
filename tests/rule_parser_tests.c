#include "../src/teno_rules.h"
#include "../src/teno_rules_filter.h"
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <string.h>
#include <time.h>
Describe(Rule);
BeforeEach(Rule) {}
AfterEach(Rule) {}

struct fake_stream{
    int curr;
    int len;
    char *data;
};

int fake_read(void *data){
    struct fake_stream *stream = (struct fake_stream*) data;
    int curr = stream->curr++;
    int result = curr >= stream->len ? EOF : stream->data[curr];
    return result;
}

Ensure(Rule, returns_rule_when_rule_got_found)
{
	unsigned int i;
    struct fake_stream fake_stream = {};
	char md[254];
	char header[] = "#Header\n\nsome description\n";
	char date_rules[] =
	    "ON 2021-01-01T11:55:11Z\nFROM 2022-02-02\nUNTIL 2023-03-03\n\n";
	char implicit_rules[] =
	    "ON_VACATION ON_HOLIDAY ON_WEEKDAY ON_WEEKEND\n";
	strcat(md, header);
	strcat(md, date_rules);
	strcat(md, implicit_rules);
    fake_stream.data = (char*) &md;
    fake_stream.len = strlen(md); 
	struct TenoRule *result = teno_rules_parse_content(
            &fake_stream,
            &fake_read,
	     NULL, NULL);
	assert_that(result, is_not_null);
	//assert_string_equal((char *)result->content, md);
	assert_equal(*result->rule_amount, 7);
	assert_equal(result->rule_ids[0], TENO_RULE_ON);
	assert_equal(result->rule_ids[1], TENO_RULE_FROM);
	assert_equal(result->rule_ids[2], TENO_RULE_UNTIL);
	assert_equal(result->rule_ids[3], TENO_RULE_ON_VACATION);
	assert_equal(result->rule_ids[4], TENO_RULE_ON_HOLIDAY);
	assert_equal(result->rule_ids[5], TENO_RULE_ON_WEEKDAY);
	assert_equal(result->rule_ids[6], TENO_RULE_ON_WEEKEND);
	for (i = 0; i < 3; i++) {
		assert_equal(result->rule_datetimes[i].tm_year,
			     2021 - 1900 + i);
		assert_equal(result->rule_datetimes[i].tm_mon, 0 + i);
		assert_equal(result->rule_datetimes[i].tm_mday, 1 + i);
	}
	teno_rules_free(result);
}

Ensure(Rule, returns_last_rule_when_md_ends_with_rule)
{
	unsigned int i;
    struct fake_stream fake_stream = {};
	char md[254];
	char header[] = "#Header\n\nsome description\n";
	char implicit_rules[] = "ON_VACATION ON_HOLIDAY ON_WEEKDAY ON_WEEKEND";
	strcat(md, header);
	strcat(md, implicit_rules);
    fake_stream.data = (char*) &md;
    fake_stream.len = strlen(md); 
	struct TenoRule *result = teno_rules_parse_content(
            &fake_stream,
            &fake_read,
	     NULL, NULL);
	assert_that(result, is_not_null);
	assert_equal(*result->rule_amount, 4);
	teno_rules_free(result);
}

Ensure(Rule, ignores_false_positive)
{
	unsigned int i;
    struct fake_stream fake_stream = {};
	char md[254];
	char header[] = "#Header\n\non february from dusk until dawn\n";
	char date_rules[] =
	    "on 2021-01-01T11:55:11Z\nfrom 2022-02-02\nuntil 2023-03-03\n\n";
	char implicit_rules[] =
	    "on-vacation on-holiday on-weekday on-weekend\n";
	strcat(md, header);
	strcat(md, date_rules);
	strcat(md, implicit_rules);
    fake_stream.data = (char*) &md;
    fake_stream.len = strlen(md); 
	struct TenoRule *result = teno_rules_parse_content(
            &fake_stream,
            &fake_read,
	     NULL, NULL);
	assert_that(result, is_not_null);
	assert_equal(*result->rule_amount, 7);
	teno_rules_free(result);
}

Ensure(Rule, filter_unfitting_rules)
{
	time_t monday = 1623081612L;
    struct fake_stream fake_stream = {};
	char md[254];
	char implicit_rules[] = "ON_WEEKEND\n";
	strcat(md, implicit_rules);
    fake_stream.data = (char*) &md;
    fake_stream.len = strlen(md); 
	struct TenoRule *result = teno_rules_parse_content(
            &fake_stream,
            &fake_read,
	     localtime(&monday), teno_rules_cmp);
	assert_that(result, is_null);

}

TestSuite *rules_parser_tests()
{
	TestSuite *suite = create_test_suite();
	add_test_with_context(suite, Rule, returns_rule_when_rule_got_found);
	add_test_with_context(
	    suite, Rule, returns_last_rule_when_md_ends_with_rule);
	add_test_with_context(suite, Rule, ignores_false_positive);
	add_test_with_context(suite, Rule, filter_unfitting_rules);
	return suite;
}
