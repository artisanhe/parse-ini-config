#include <stdio.h>

#include "config.h"

int main(int argc, char **argv)
{
	sections_t section_head = {0};
	read_ini_file(CONFIG_FILE, &section_head);
	travel_keys_under_section("category", &section_head);
	travel_keys_under_section("test", &section_head);
	return 0;
}
