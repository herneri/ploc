/*
	Copyright 2025 Eric Hernandez

	This file is part of ploc.

	ploc is free software: you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	ploc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with ploc. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PLOC_DB_H
#define PLOC_DB_H

#include <sqlite3.h>
#include <stdbool.h>
#include "package.h"

#define PLOC_DATABASE_PATH "/var/local/ploc.db"

enum operation_values {
	DB_INSERT, DB_REMOVE, DB_INFO
};

bool ploc_database_initialize(sqlite3 *database_connection);

void ploc_check_conflict(sqlite3 *database_connection, const int operation, struct Package *pkg);

void ploc_database_insert(sqlite3 *database_connection, struct Package *pkg);

void ploc_database_remove(sqlite3 *database_connection, struct Package *pkg);

/* Search for all packages with same name, used for -s option. */
void ploc_database_fetch_all(sqlite3 *database_connection, const char *name);

#endif
