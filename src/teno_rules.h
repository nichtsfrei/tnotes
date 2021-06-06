#ifndef TENO_RULES_H
#define TENO_RULES_H
#ifndef NULL
// for the issue that ccls isn't wrapped for nixos
#define NULL ((void *)0)
#endif

enum TENO_RULE_ID{
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



struct TenoRule{
    const unsigned char *content;
    const struct tm *rule_datetimes;
    const enum TENO_RULE_ID *rule_ids;
    // just use pointer to keep it simple while creating a couple of TenoRule 
    unsigned int *rule_amount;
};

struct TenoRule *teno_rules_parse_content(const unsigned char *content, unsigned int content_length);

#endif
