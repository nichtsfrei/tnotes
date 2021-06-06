#include "teno_rules.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __USE_XOPEN
#include <time.h>
// afaik max datetime length is 2021-06-06T10:46:05+00:00
#define MAX_WORD_LEN 25
#define BUFFER_SIZE 2048

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

struct ContentArray {
	unsigned int len;
	unsigned long cap;
	char *buf;
};

struct TenoRule *teno_rules_parse_content(void *stream,
					  int (*read)(void *),
					  struct tm *today,
					  int (*cmp)(enum TENO_RULE_ID,
						     struct tm *,
						     struct tm *))
{
	void *p;
	unsigned int i, wi = 0, cmprc = 0, rc = 0;
	char c = 0;
	// we don't care for words longer than MAX_WORD_LEN
	unsigned char wb[MAX_WORD_LEN] = {0};
	struct tm *given_time_info = NULL;
	enum TENO_RULE_ID *rule_ids = NULL;
	struct tm *rule_datetimes = NULL;
	enum TENO_RULE_ID guess = TENO_RULE_NONE;
	struct TenoRule *result = NULL;
	struct ContentArray buffer = {};
	buffer.buf = calloc(BUFFER_SIZE, 1 * sizeof(*buffer.buf));
	buffer.cap = BUFFER_SIZE;

	for (i = 0, wi = 0; c != EOF; i++, wi++) {
		// we need to handle EOF separately hence we don't check eagerly
		// within loop header
		c = read(stream);
		buffer.buf[i] = c == EOF ? 0 : c;

		if (++buffer.len == buffer.cap) {
			buffer.cap = 2 * buffer.cap * sizeof(*buffer.buf);
			p = realloc(buffer.buf, buffer.cap);
			if (p == NULL) {
				fprintf(stderr,
					"fail to allocate memory (%lu) for "
					"buffer\n",
					buffer.cap);
				goto error;
			} else
				buffer.buf = p;
		}
		// we just care about keywords and dates
		if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
		    (c >= 'A' && c <= 'Z') || (c == ':') || (c == '_') ||
		    (c == '-')) {
			wb[wi % MAX_WORD_LEN] = c;
		} else {
			wb[wi] = 0;
			wi = -1;
		}
		if (wi == -1 || c == EOF) {
			switch (guess) {
			case TENO_RULE_NONE:
			handle_none:
				guess = to_rule_id((const char *)wb);
				if (given_time_info)
					free(given_time_info);
				p = calloc(1, sizeof(struct tm));
				if (p == NULL) {
					fprintf(stderr,
						"fail to allocate memory (%lu) "
						"for given_time_info\n",
						sizeof(struct tm));
					goto error;
				} else
					given_time_info = p;
				// skip to next word when we need a date or if
				// NONE
				if (guess <= TENO_RULE_ON)
					break;
			case TENO_RULE_UNTIL:
			case TENO_RULE_FROM:
			case TENO_RULE_ON:
				// could slip through TENO_RULE_NONE case on new
				// guess on same word
				if (guess <= TENO_RULE_ON &&
				    strptime((const char *)&wb,
					     "%Y-%m-%d",
					     given_time_info) == NULL)
					// not a date assume false positive
					goto handle_none;
			case TENO_RULE_ON_VACATION:
			case TENO_RULE_ON_HOLIDAY:
			case TENO_RULE_ON_WEEKDAY:
			case TENO_RULE_ON_WEEKEND:
				if ((cmprc = cmp != NULL ? cmp(guess,
							       today,
							       given_time_info)
							 : 0) ==
				    TENO_RULE_ERRNO_UNFITTING_CMP) {
					errno = TENO_RULE_ERRNO_UNFITTING_CMP;
					fprintf(stderr,
						"called unfitting cmp \n");
					goto error;
				} else if (cmprc == 0) {
					p = realloc(rule_datetimes,
						    ++rc * sizeof(struct tm));
					if (p == NULL) {
						fprintf(
						    stderr,
						    "realoc for (%lu) "
						    "tule_datetimes failed\n",
						    rc * sizeof(struct tm));
						goto error;
					} else
						rule_datetimes = p;

					p = realloc(
					    rule_ids,
					    rc * sizeof(enum TENO_RULE_ID));
					if (p == NULL) {
						fprintf(
						    stderr,
						    "realoc for (%lu) rule_ids "
						    "failed\n",
						    rc *
							sizeof(
							    enum TENO_RULE_ID));
						goto error;
					} else
						rule_ids = p;
					rule_datetimes[rc - 1] =
					    *given_time_info;
					rule_ids[rc - 1] = guess;
					free(given_time_info);
					given_time_info = NULL;
				}
				guess = TENO_RULE_NONE;
				break;
			}
		}
	}
	if (rc > 0) {
		result = calloc(1, sizeof(*result));
		result->rule_datetimes = &rule_datetimes[0];
		result->rule_ids = &rule_ids[0];
		result->rule_amount = calloc(1, sizeof(*result->rule_amount));
		*result->rule_amount = rc;
		result->content = (unsigned char *)buffer.buf;
		goto exit;
	}
error:
	if (buffer.buf)
		free(buffer.buf);
	if (rule_ids)
		free(rule_ids);
	if (rule_datetimes)
		free(rule_datetimes);
	if (given_time_info)
		free(given_time_info);
exit:
	return result;
}

void teno_rules_free(struct TenoRule *rule)
{
	free(rule->rule_datetimes);
	free(rule->rule_ids);
	free(rule->rule_amount);
}
