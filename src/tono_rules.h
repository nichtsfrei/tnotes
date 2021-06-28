#ifndef TENO_RULES_H
#define TENO_RULES_H

#define TENO_RULE_ERRNO_UNFITTING_CMP 2342

enum TENO_RULE_ID {
	TENO_RULE_NONE,
	TENO_RULE_UNTIL,
	TENO_RULE_FROM,
	TENO_RULE_ON,
	// special rules which do not need a defined datetime string
	TENO_RULE_ON_VACATION,
	TENO_RULE_ON_HOLIDAY,
	// based on weekend, weekdays
	TENO_RULE_ON_WEEKDAY,
	TENO_RULE_ON_WEEKEND,
};

struct TenoRule {
	const unsigned char *content;
	// just use pointer to keep it simple while creating a couple of
	// TenoRule
	unsigned int *rule_amount;
	struct tm *rule_datetimes;
	enum TENO_RULE_ID *rule_ids;
};

// int tono_rules_cmp_fake(enum TENO_RULE_ID id, struct tm *date){

struct TenoRule *
tono_rules_parse_content(void *stream,
			 int (*read)(void *),
			 struct tm *today,
			 int (*cmp)(enum TENO_RULE_ID, struct tm *, struct tm *)

);

void tono_rules_free(struct TenoRule *rule);

#endif
