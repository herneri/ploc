/*
	Copyright 2025 Eric Hernandez

	This file is part of ploc.

	ploc is free software: you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	ploc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with ploc. If not, see <https://www.gnu.org/licenses/>.
*/

#include <sqlite3.h>
#include "database.h"
#include "package.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	sqlite3 *database_connection = NULL;
	int status = sqlite3_open(PLOC_DATABASE_PATH, &database_connection);
	struct Package pkg = {"", DEFAULT_GROUP, DEFAULT_PATH};

	enum ARG_MODES {
		FIND_ARG, GET_OPERATION
	};

	int arg_mode = FIND_ARG;

	if (status != PLOC_OK) {
		fprintf(stderr, "ploc: Failed to open database\n");
		return PLOC_DB_FAIL;
	}

	if (ploc_database_initialize(database_connection) == false) {
		return PLOC_INIT_FAIL;
	}

	for (int i = 1; i < argc; i++) {
		if (arg_mode == FIND_ARG && strnlen(argv[i], 255) < 2) {
			fprintf(stderr, "ploc: An argument must be at least 2 characters long\n");
			return PLOC_ARG_SYNTAX_ERR;
		}

		if (arg_mode == FIND_ARG && argv[i][0] == '-') {
			arg_mode = GET_OPERATION;
		} else if (arg_mode == FIND_ARG) {
			fprintf(stderr, "ploc: Option must come before operand\n");
			return PLOC_ARG_SYNTAX_ERR;
		}

		if (arg_mode == GET_OPERATION && argv[i][1] != 'a' && i + 1 >= argc) {
			fprintf(stderr, "ploc: Not enough arguments\n");
			return PLOC_ARG_SYNTAX_ERR;
		}

		switch (argv[i][1]) {
		case 'i':
			if (i + 2 >= argc) {
				fprintf(stderr, "ploc: Not enough arguments\n");
				return PLOC_ARG_SYNTAX_ERR;
			}

			if (strchr(argv[i + 2], '/') == NULL) {
				fprintf(stderr, "ploc: Destination must be an absolute path\n");
				return PLOC_ARG_SYNTAX_ERR;
			}

			ploc_get_name_and_path(&pkg, argv[i + 2]);
			ploc_install_package(database_connection, &pkg, argv[i + 1]);

			i += 2;
			break;
		case 'r':
			ploc_get_name_and_path(&pkg, argv[i + 1]);
			ploc_check_conflict(database_connection, false, &pkg);
			ploc_delete_package(database_connection, &pkg);

			i++;
			break;
		case 'l':
			strcpy(pkg.name, argv[i + 1]);
			ploc_check_conflict(database_connection, false, &pkg);
			if (strncmp(pkg.path, "NULL", 255) == 0) {
				fprintf(stderr, "ploc: %s not found\n", pkg.name);
				return PLOC_CONFLICT;
			}

			ploc_database_load_info(database_connection, &pkg);
			ploc_list_package(&pkg);
			i++;
			arg_mode = FIND_ARG;
			break;
		case 's':
			ploc_database_fetch_all(database_connection, argv[i + 1]);
			i++;
			break;
		case 'S':
			ploc_get_name_and_path(&pkg, argv[i + 1]);
			ploc_database_search_unique(database_connection, &pkg);

			i++;
			arg_mode = FIND_ARG;
			break;
		case 'a':
			ploc_database_fetch_table(database_connection);
			arg_mode = FIND_ARG;
			break;
		default:
			fprintf(stderr, "ploc: Invalid argument: %s\n", argv[i]);
			return PLOC_ARG_SYNTAX_ERR;
		}

	}

	sqlite3_close(database_connection);
	return PLOC_OK;
}
