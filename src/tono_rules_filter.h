#ifndef TENO_RULES_FILTER_H
#define TENO_RULES_FILTER_H
#include "tono_rules.h"

int tono_rules_cmp_from_id(enum TENO_RULE_ID, struct tm *, struct tm *);
int tono_rules_cmp_until_id(enum TENO_RULE_ID, struct tm *, struct tm *);
int tono_rules_cmp_on_id(enum TENO_RULE_ID, struct tm *, struct tm *);
int tono_rules_cmp_holiday_id(enum TENO_RULE_ID, struct tm *, struct tm *);
int tono_rules_cmp_vacation_id(enum TENO_RULE_ID, struct tm *, struct tm *);
int tono_rules_cmp_weekday_id(enum TENO_RULE_ID, struct tm *, struct tm *);
int tono_rules_cmp_weekend_id(enum TENO_RULE_ID, struct tm *, struct tm *);
int tono_rules_cmp(enum TENO_RULE_ID, struct tm *, struct tm *);

#endif
