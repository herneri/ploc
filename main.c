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

	sqlite3_close(database_connection);
	return PLOC_OK;
}
