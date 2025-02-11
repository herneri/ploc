/*
	Copyright 2025 Eric Hernandez

	This file is part of ploc.

	ploc is free software: you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	ploc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with ploc. If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sqlite3.h>
#include "database.h"
#include "package.h"

bool ploc_database_initialize(sqlite3 *database_connection) {
	const char *sql_statement = "\
					CREATE TABLE IF NOT EXISTS package(\
						id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
						name VARCHAR(20) NOT NULL,\
						pgroup VARCHAR(20) DEFAULT 'none',\
						path VARCHAR(255) DEFAULT '' NOT NULL\
					);\
					\
					CREATE TABLE IF NOT EXISTS path(\
						id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
						name VARCHAR(255) NOT NULL\
					);";
	char *error_message = NULL;
	int status = sqlite3_exec(database_connection, sql_statement, NULL, NULL, &error_message);

	if (status != 0) {
		fprintf(stderr, "ploc: Failed to initialize database: %s\n", error_message);

		if (error_message != NULL) {
			free(error_message);
		}

		return false;
	}

	return true;
}

void ploc_check_conflict(sqlite3 *database_connection, const int operation, struct Package *pkg) {
	sqlite3_stmt *prepared_statement = NULL;
	char *sql_statement = NULL;
	int result = 0;

	if (operation == DB_INSERT) {
		sql_statement = "SELECT name, path FROM package WHERE name = ? AND path = ?";
	} else {
		sql_statement = "SELECT name FROM package WHERE name = ?";
	}

	sqlite3_prepare_v2(database_connection, sql_statement, 140, &prepared_statement, NULL);

	sqlite3_bind_text(prepared_statement, 1, pkg->name, -1, NULL);
	if (operation == DB_INSERT) {
		sqlite3_bind_text(prepared_statement, 2, pkg->path, -1, NULL);
	}

	result = sqlite3_step(prepared_statement);

	if (result == SQLITE_ROW) {
		switch (operation) {
		case DB_INSERT:
			fprintf(stderr, "ploc: A package already exists with the name \"%s\" at \"%s\".\nRun installation again with -f to force it.\n", pkg->name, pkg->path);
			exit(PLOC_CONFLICT);
			break;
		}
	}

	sqlite3_finalize(prepared_statement);
	return;
}

void ploc_database_insert(sqlite3 *database_connection, struct Package *pkg) {
	char *sql_statement = "INSERT INTO package(name, pgroup, path) VALUES(?, ?, ?)";
	sqlite3_stmt *prepared_statement = NULL;

	sqlite3_prepare_v2(database_connection, sql_statement, 295, &prepared_statement, NULL);
	sqlite3_bind_text(prepared_statement, 1, pkg->name, -1, NULL);
	sqlite3_bind_text(prepared_statement, 2, pkg->group, -1, NULL);
	sqlite3_bind_text(prepared_statement, 3, pkg->path, -1, NULL);

	sqlite3_step(prepared_statement);
	sqlite3_finalize(prepared_statement);

	return;
}
