#include "teno_rules.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define __USE_XOPEN
#include <time.h>
// afaik max datetime length is 2021-06-06T10:46:05+00:00
#define MAX_WORD_LEN 25

enum TENO_RULE_ID to_rule_id(const char *word)
{
	enum TENO_RULE_ID result = TENO_RULE_NONE;
	if (strncmp(word, "ON_VACATION", 11) == 0 ||
	    strncmp(word, "on-vacation", 11) == 0) {
		result = TENO_RULE_ON_VACATION;
	} else if (strncmp(word, "ON_HOLIDAY", 10) == 0 ||
		   strncmp(word, "on-holiday", 10) == 0) {
		result = TENO_RULE_ON_HOLIDAY;
	} else if (strncmp(word, "ON_WEEKDAY", 10) == 0 ||
		   strncmp(word, "on-weekday", 10) == 0) {
		result = TENO_RULE_ON_WEEKDAY;
	} else if (strncmp(word, "ON_WEEKEND", 10) == 0 ||
		   strncmp(word, "on-weekend", 10) == 0) {
		result = TENO_RULE_ON_WEEKEND;
	} else if (strncmp(word, "ON", 2) == 0 || strncmp(word, "on", 2) == 0) {
		result = TENO_RULE_ON;
	} else if (strncmp(word, "FROM", 4) == 0 ||
		   strncmp(word, "from", 4) == 0) {
		result = TENO_RULE_FROM;
	} else if (strncmp(word, "UNTIL", 4) == 0 ||
		   strncmp(word, "until", 4) == 0) {
		result = TENO_RULE_UNTIL;
	}
	return result;
}

struct TenoRule *teno_rules_parse_content(const unsigned char *content,
					  unsigned int content_length)
{
	time_t rawtime;
	unsigned int i, wi = 0, rc = 0;
	unsigned char c;
	// we don't care for words longer than MAX_WORD_LEN
	unsigned char wb[MAX_WORD_LEN] = { 0 };
	struct tm *given_time_info = NULL;
	enum TENO_RULE_ID *rule_ids = NULL;
	struct tm *rule_datetimes = NULL;
	enum TENO_RULE_ID guess = TENO_RULE_NONE;
	struct TenoRule *result = NULL;

	for (i = 0; i < content_length; i++, wi++) {
		c = content[i];
		// we just care about keywords and dates
		if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
		    (c >= 'A' && c <= 'Z') || (c == ':') || (c == '_') ||
		    (c == '-')) {
			wb[wi % MAX_WORD_LEN] = c;
		} else {
			wb[wi] = 0;
			wi = -1;
		}
		if (wi == -1 || i == content_length - 1) {
			switch (guess) {
			case TENO_RULE_NONE:
				guess = to_rule_id((const char *)wb);
				// skip to next word when we need a date or if NONE
				if (guess <= TENO_RULE_ON)
					break;
			case TENO_RULE_UNTIL:
			case TENO_RULE_FROM:
			case TENO_RULE_ON:
				// could slip through TENO_RULE_NONE case on new guess on same word
				if (guess <= TENO_RULE_ON) {
					given_time_info =
						calloc(1, sizeof(struct tm));
					if (strptime((const char *)&wb,
						     "%Y-%m-%d",
						     given_time_info) == NULL) {
						// not a date assume false positive
						free(given_time_info);
						given_time_info = NULL;
						guess = to_rule_id(
							(const char *)wb);
						break;
					}
				}
			case TENO_RULE_ON_VACATION:
			case TENO_RULE_ON_HOLIDAY:
			case TENO_RULE_ON_WEEKDAY:
			case TENO_RULE_ON_WEEKEND:
				// probably better to use stack instead of heap
				if (given_time_info == NULL) {
					given_time_info =
						calloc(1, sizeof(struct tm));
				}
				rule_datetimes =
					realloc(rule_datetimes,
						++rc * sizeof(struct tm));
				rule_ids =
					realloc(rule_ids,
						rc * sizeof(enum TENO_RULE_ID));
				rule_datetimes[rc - 1] = *given_time_info;
				rule_ids[rc - 1] = guess;
				free(given_time_info);
				given_time_info = NULL;
				guess = TENO_RULE_NONE;
				break;
			}
		}
	}
exit:
	if (rc > 0) {
		result = calloc(1, sizeof(*result));
		result->content = content;
		result->rule_datetimes = &rule_datetimes[0];
		result->rule_ids = &rule_ids[0];
		result->rule_amount = calloc(1, sizeof(*result->rule_amount));
		*result->rule_amount = rc;
	}
	return result;
error:
	if (rule_ids)
		free(rule_ids);
	if (rule_datetimes)
		free(rule_datetimes);
	return NULL;
}

