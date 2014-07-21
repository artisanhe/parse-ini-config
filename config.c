#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "config.h"
#include "list.h"


void print_error(char *err_msg)
{
	printf("%s\n", err_msg);
	exit(0);
}

/*
 * @brief delete_line_break_indicator 
 * @buffer[in,out] 
 * @NOTE:
 * detete the last one or two character which is '\n' or 'r'
 */
int delete_line_break_indicator(char *buffer)
{
	int len = strlen(buffer), i, j = len - 1;
	for (i = j; i >= 0; i--) {
		if (!isspace(buffer[i])) {
			if (i == j) {
				break;
			}
			else { 
				buffer[i + 1] = '\0';
				break;
			}
		}
	}

	DELE_NEW_LINE_INDICATOR(buffer, len);
#if 0
	if (NEW_LINE(buffer[len - 1])) {
		if (NEW_LINE(buffer[len - 2]))
			buffer[len - 2] = '\0';
		else 
			buffer[len - 1] = '\0';
	}
#endif
	return 0;
}

/*
 * @read_file_content
 * @file[in,out] the direcotry of category.ini 
 * @section_head[in,out]
 * @Note
:
 *	parse the file which typs is .ini
 */
int read_file_content(const char *file, sections_t *section_head)
{
	FILE *fp = NULL;
	char * ret_buf, contents[LINE_LEN] = {0}, *saveptr = NULL, *pcon = contents;
	sections_t *section_node = NULL;
	keys_t *keys;

	assert(file !=NULL);
	
	if ((fp = fopen(file,"r")) == NULL) {
		fprintf(stderr, "Open [%s] %s\n", file ,strerror(errno));
		exit(1);
	}

	while (fgets(pcon, LINE_LEN, fp)){
		/** @brief skip all the space character that contains
			('\r', '\n', '\f', '\t', '\v') 
		*/
		while (isspace(*pcon)) {
			pcon++;
		}
		
		/** @brief find the section '[]' */
		if (*pcon == LEFT_BRACE) {
			if ((ret_buf = strchr(pcon, RIGHT_BRACE))) {
				if ((section_node = (sections_t *)malloc(sizeof(sections_t)))
								== NULL) {
					perror("MALLOC:");
					exit(0);
				}
				memset(section_node, 0, sizeof(sections_t));
				memcpy(section_node->section, pcon + 1, ret_buf - pcon - 1);
				list_add_tail(&section_node->node, &section_head->node);
			}else {
				print_error("syntex error , there are need ']'");
			}
		}else if (*pcon == COMMENT) {
			/** @brief this is a comment line and ignore it */
			continue;
		}else {
		
			/** @brief find all the keys and value which under the section found */
			if (section_node == NULL) {
				print_error("syntex error, there are no '[]'");
			}
			
			int i = 0;
			if (section_node->keys == NULL) {
				if ((section_node->keys = (keys_t *)malloc(sizeof(keys_t))) 
							== NULL){
					perror("MALLOC:");
					exit(0);
				}
				memset(section_node->keys, 0, sizeof(keys_t));
			    INIT_LIST_HEAD(&section_node->keys->node);
			}
			if ((keys = (keys_t *)malloc(sizeof(keys_t)))
						== NULL) {
				perror("MALLOC:");
				exit(0);
			}	
			memset(keys, 0, sizeof(keys_t));
			
			while (ret_buf = strtok_r(pcon, SIGN_OF_EQUAL, &saveptr)) {
				if (i == 0) {
					delete_line_break_indicator(ret_buf);
					memcpy(keys->key, ret_buf, strlen(ret_buf)); }else if (i == 1) {
					delete_line_break_indicator(ret_buf);
					memcpy(keys->value, ret_buf, strlen(ret_buf));
				}
				pcon = NULL;
				i++;
			}

			if (i == 0) {
				free(keys);
			}else 
				list_add_tail(&keys->node, &section_node->keys->node);
		}

		memset(contents, 0, LINE_LEN);
		pcon = contents;
	}

	fclose(fp);
	return 1;
}
/*
 * @travel_keys_under_section
 * @section_name[in] 
 * @key[in]
 * @section_head[in,out] the struct which stores the head of section list
 * @return 
 *		0 don't find the key other find the key and return the key id
 */
int find_key_under_section(char *section_name , char *key,
								sections_t * section_head)
{
	assert(section_name != NULL);
	assert(key != NULL);
	assert(section_head != NULL);

	/** @brief group_id is begin at "3", because 
		(black_name is 0, white_name is 1, search_keyword is 2) */
	int num = BASE_URL_FIRST_GROUP;
	struct list_head *pos = NULL, *pos_child = NULL;
	sections_t * tmp_node = NULL;
	keys_t *tmp_key = NULL;
	list_for_each(pos, &section_head->node) {
		tmp_node = (sections_t *)container_of(pos, sections_t, node);
		if (STRING_COMPARE(section_name, ==, tmp_node->section)) {
			list_for_each(pos_child, &tmp_node->keys->node) {
				tmp_key = (keys_t *)container_of(pos_child, 
													keys_t, node);
				if (STRING_COMPARE(key, ==, tmp_key->key)) {
					return num;
				}

				num++;
			}
		}
	}

	return 0;
}

/*
 * @get_value_by_key
 * @section_name[in]
 * @key[in]
 * @section_head[in,out] the struct which stores the head of section list
 */
int get_value_by_key(char *section_name, char *key, sections_t * section_head)
{
	assert(section_name != NULL);
	assert(key != NULL);
	assert(section_head != NULL);
	
	struct list_head *pos = NULL, *pos_child = NULL;
	sections_t * tmp_node = NULL;
	keys_t *tmp_key = NULL;
	list_for_each(pos, &section_head->node) {
		tmp_node = (sections_t *)container_of(pos, sections_t, node);
		if (STRING_COMPARE(section_name, ==, tmp_node->section)) {
			list_for_each(pos_child, &tmp_node->keys->node) {
				tmp_key = (keys_t *)container_of(pos_child, 
													keys_t, node);
				if (STRING_COMPARE(key, ==, tmp_key->key)) {
					printf("Find the value:[%s]\n", tmp_key->value);
				}
			}
		}
	}

	return 1;
}

/*
 * @travel_keys_under_section
 * @section_name[in] 
 * @section_head[in,out] the struct which stores the head of section list
 */
int travel_keys_under_section(char *section_name, sections_t *section_head)
{
	assert(section_name != NULL);
	assert(section_head != NULL);
	
	struct list_head *pos = NULL, *pos_child = NULL;
	sections_t * tmp_node = NULL;
	keys_t *tmp_key = NULL;
	list_for_each(pos, &section_head->node) {
		tmp_node = (sections_t *)container_of(pos, sections_t, node);
		if (STRING_COMPARE(section_name, ==, tmp_node->section)) {
			printf("The section is:[%s] and the keys are:\n", 
				tmp_node->section);
			list_for_each(pos_child, &tmp_node->keys->node) {
				tmp_key = (keys_t *)container_of(pos_child, 
													keys_t, node);
				printf("%s\n", tmp_key->key);
			}
		}
	}

	return 0;
}

/*
 * @read_ini_file
 * @filename[in] the name of *.ini file
 * @section_head[in,out] 
 */
int read_ini_file(char * filename, sections_t *section_head)
{
	char buffer[VALUE_LEN] = {0}, *file_contents = NULL;
	/** @brief init the head of list */
    INIT_LIST_HEAD(&section_head->node);


	/** @brief parse the whole .ini file and 
		store information in section_head list */
	read_file_content(filename, section_head);
	
	return 0;
}
