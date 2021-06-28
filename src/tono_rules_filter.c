#include "tono_rules_filter.h"
#include <time.h>
#define GUARD_RULE_ID(n, m)                                                    \
	if ((n) != (m))                                                        \
	return TENO_RULE_ERRNO_UNFITTING_CMP
int tono_rules_cmp_on_id(enum TENO_RULE_ID id,
			 struct tm *date,
			 struct tm *input_date)
{
	GUARD_RULE_ID(id, TENO_RULE_ON);
	int result = 365;
	if (date == NULL || input_date == NULL)
		goto exit;
	result = date->tm_yday - input_date->tm_yday;
exit:
	return result;
}

int tono_rules_cmp_from_id(enum TENO_RULE_ID id,
			   struct tm *date,
			   struct tm *input_date)
{
	GUARD_RULE_ID(id, TENO_RULE_FROM);
	int result = -365;
	if (date == NULL || input_date == NULL)
		goto exit;
	result = date->tm_yday - input_date->tm_yday;
	result = result >= 0 ? 0 : result;
exit:
	return result;
}

int tono_rules_cmp_until_id(enum TENO_RULE_ID id,
			    struct tm *date,
			    struct tm *input_date)
{
	GUARD_RULE_ID(id, TENO_RULE_UNTIL);
	int result = 365;
	if (date == NULL || input_date == NULL)
		goto exit;
	result = date->tm_yday - input_date->tm_yday;
	result = result <= 0 ? 0 : result;
exit:
	return result;
}

int tono_rules_cmp_weekday_id(enum TENO_RULE_ID id,
			      struct tm *date,
			      struct tm *input_date)
{
	int result = 7;
	GUARD_RULE_ID(id, TENO_RULE_ON_WEEKDAY);
	if (date == NULL)
		goto exit;
	result = (date->tm_wday > 0 && date->tm_wday < 6) ? 0 : 1;
exit:
	return result;
}

int tono_rules_cmp_weekend_id(enum TENO_RULE_ID id,
			      struct tm *date,
			      struct tm *input_date)
{
	int result = 7;
	GUARD_RULE_ID(id, TENO_RULE_ON_WEEKEND);
	if (date == NULL)
		goto exit;
	result = (date->tm_wday == 0 || date->tm_wday == 6) ? 0 : date->tm_wday;
exit:
	return result;
}

int tono_rules_cmp(enum TENO_RULE_ID id, struct tm *date, struct tm *input)
{
	int result;
	switch (id) {
	case TENO_RULE_ON:
		result = tono_rules_cmp_on_id(id, date, input);
		break;
	case TENO_RULE_FROM:
		result = tono_rules_cmp_from_id(id, date, input);
		break;
	case TENO_RULE_UNTIL:
		result = tono_rules_cmp_until_id(id, date, input);
		break;
	case TENO_RULE_ON_WEEKDAY:
		result = tono_rules_cmp_weekday_id(id, date, input);
		break;
	case TENO_RULE_ON_WEEKEND:
		result = tono_rules_cmp_weekend_id(id, date, input);
		break;
	default:
		result = -1;
	}
	return result;
}
