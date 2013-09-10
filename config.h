#ifndef INI_FILE_H_
#define INI_FILE_H_

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include "stlc_list.h"

#define LEFT_BRACE '['
#define RIGHT_BRACE ']'
#define COMMENT ';'
#define SIGN_OF_EQUAL "=="

/** @brief 默认情况下系统存在3个类别(0:白名单，1:黑名单，2:搜有引擎敏感词汇) */
#define BASE_URL_FIRST_GROUP 3

#define LINE_LEN 256

/** @brief max length of each value */
#define VALUE_LEN 256

typedef struct keys {
	struct stlc_list_head node;
	char value[VALUE_LEN];
	char key[VALUE_LEN];
	int id;
}keys_t;

typedef struct sections{
	struct stlc_list_head node;
	char section[VALUE_LEN];
	keys_t *keys;
}sections_t;

typedef struct group_range {
	int begin;
	int end;
	int group_num;
}group_range_t;

extern group_range_t *g_group;
/** @brief the end of each line */
#define NEW_LINE(c) ('\n' == c || '\r' == c)? 1 : 0


/** @brief delete the end-of-line indicator */
#define DELE_NEW_LINE_INDICATOR(buffer, len) 	\
									if (NEW_LINE(buffer[len - 1])) { \
										if (NEW_LINE(buffer[len - 2])) \
											buffer[len - 2] = '\0';   \
										else  						\
											buffer[len - 1] = '\0';\
									}	

#define STRING_COMPARE(x, equ, y) (strcmp(x, y) equ 0)

int read_ini_string(const char *section, const char *key,char *value, 
					int size, char *g_file_content);
int load_file_content(const char *, char **);
int read_ini_file(char *, sections_t *);

#define CONFIG_FILE "../category.ini"
#define DIRECTORY "../category/"
#define PHASH_FILE "../phash.txt"
#define GROUP_FILE "../group.txt"

#endif //end of INI_FILE_H_

