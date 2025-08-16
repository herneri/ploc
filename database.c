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
#include <string.h>
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

void ploc_handle_conflict(struct Package *pkg, sqlite3_stmt *prepared_statement, char **first_path) {
	char **package_instances = NULL;
	char *temp_package_path = *first_path;
	const char *memory_error_message = "ploc: Multiple packages of same name detected, but not enough memory to store them.\n";

	int instance_count = 0;
	int index_input = 0;
	int result = 0;

	package_instances = (char **) malloc(sizeof(char *) * 2);
	if (package_instances == NULL) {
		fprintf(stderr, "%s", memory_error_message);
		exit(PLOC_MEM_FAIL);
	}

	package_instances[0] = temp_package_path;
	instance_count++;

	temp_package_path = (char *) malloc(sizeof(char) * 255);
	if (temp_package_path == NULL) {
		fprintf(stderr, "%s", memory_error_message);
		exit(PLOC_MEM_FAIL);
	}

	package_instances[1] = temp_package_path;
	strcpy(package_instances[1], (char *) sqlite3_column_text(prepared_statement, 0));
	instance_count++;

	while ((result = sqlite3_step(prepared_statement)) == SQLITE_ROW) {
		char **temp = package_instances;
		package_instances = (char **) realloc(package_instances, (instance_count + 1));

		if (package_instances == NULL) {
			fprintf(stderr, "%s", memory_error_message);
			exit(PLOC_MEM_FAIL);
		}

		temp_package_path = (char *) malloc(255);
		if (temp_package_path == NULL) {
			fprintf(stderr, "%s", memory_error_message);
			exit(PLOC_MEM_FAIL);
		}

		temp_package_path[254] = '\0';

		package_instances[instance_count] = temp_package_path;
		strcpy(package_instances[instance_count], (char *) sqlite3_column_text(prepared_statement, 0));
		instance_count++;
	}

	printf("ploc: %d packages with the name %s found\n", instance_count, pkg->name);

	for (int i = 0; i < instance_count; i++) {
		printf("%d => %s \n", i, package_instances[i]);
	}

	printf("\nChoose an index to carry out this operation on:\n");

	do {
		printf("(must be 0 through %d) ", instance_count - 1);
		scanf("%d", &index_input);
	} while (index_input < 0 || index_input >= instance_count);

	strcpy(pkg->path, package_instances[index_input]);

	for (int i = 0; i < instance_count; i++) {
		free(package_instances[i]);
	}

	free(package_instances);
	return;
}

void ploc_check_conflict(sqlite3 *database_connection, const bool is_insertion, struct Package *pkg) {
	sqlite3_stmt *prepared_statement = NULL;
	char *sql_statement = NULL;
	char *temp_package_path = NULL;
	int result = 0;

	if (is_insertion == true) {
		sql_statement = "SELECT name, path FROM package WHERE name = ? AND path = ?";
	} else {
		sql_statement = "SELECT path FROM package WHERE name = ?";
	}

	sqlite3_prepare_v2(database_connection, sql_statement, 140, &prepared_statement, NULL);

	sqlite3_bind_text(prepared_statement, 1, pkg->name, -1, NULL);
	if (is_insertion == true) {
		sqlite3_bind_text(prepared_statement, 2, pkg->path, -1, NULL);
	}

	result = sqlite3_step(prepared_statement);

	if (result != SQLITE_ROW) {
		sqlite3_finalize(prepared_statement);
		return;
	}

	if (is_insertion == true) {
		fprintf(stderr, "ploc: A package already exists with the name \"%s\" at \"%s\".\nRun installation again with -f to force it.\n", pkg->name, pkg->path);
		sqlite3_finalize(prepared_statement);
		exit(PLOC_CONFLICT);
	}

	temp_package_path = (char *) malloc(sizeof(char) * 255);
	if (temp_package_path == NULL) {
		fprintf(stderr, "ploc: Not enough memory to check for package conflict.\n");
		exit(PLOC_MEM_FAIL);
	}

	temp_package_path[254] = '\0';
	strcpy(temp_package_path, (char *) sqlite3_column_text(prepared_statement, 0));

	result = sqlite3_step(prepared_statement);
	if (result == SQLITE_DONE) {
		free(temp_package_path);
		sqlite3_finalize(prepared_statement);
		return;
	}

	ploc_handle_conflict(pkg, prepared_statement, &temp_package_path);
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

void ploc_database_remove(sqlite3 *database_connection, struct Package *pkg) {
	char *sql_statement = "DELETE FROM package WHERE path = ? AND name = ?";
	sqlite3_stmt *prepared_statement = NULL;

	sqlite3_prepare_v2(database_connection, sql_statement, 295, &prepared_statement, NULL);
	sqlite3_bind_text(prepared_statement, 1, pkg->path, -1, NULL);
	sqlite3_bind_text(prepared_statement, 2, pkg->name, -1, NULL);

	sqlite3_step(prepared_statement);
	sqlite3_finalize(prepared_statement);

	return;
}

void ploc_database_fetch_all(sqlite3 *database_connection, const char *name) {
	char *sql_statement = "SELECT path FROM package WHERE name = ?";
	sqlite3_stmt *prepared_statement = NULL;

	sqlite3_prepare_v2(database_connection, sql_statement, 295, &prepared_statement, NULL);
	sqlite3_bind_text(prepared_statement, 1, name, -1, NULL);

	while (sqlite3_step(prepared_statement) == SQLITE_ROW) {
		printf("%s\n", sqlite3_column_text(prepared_statement, 0));
	}

	sqlite3_finalize(prepared_statement);
	return;
}

void ploc_database_search_unique(sqlite3 *database_connection, struct Package *pkg) {
	char *sql_statement = "SELECT path, name FROM package WHERE path = ? AND name = ?";
	sqlite3_stmt *prepared_statement = NULL;

	sqlite3_prepare_v2(database_connection, sql_statement, 295, &prepared_statement, NULL);
	sqlite3_bind_text(prepared_statement, 1, pkg->path, -1, NULL);
	sqlite3_bind_text(prepared_statement, 2, pkg->name, -1, NULL);

	if (sqlite3_step(prepared_statement) == SQLITE_ROW) {
		printf("%s%s [installed]\n", pkg->path, pkg->name);
	} else {
		fprintf(stderr, "ploc: Package at %s%s isn't recorded by ploc.\n", pkg->path, pkg->name);
	}

	sqlite3_finalize(prepared_statement);
	return;
}

void ploc_database_load_info(sqlite3 *database_connection, struct Package *pkg) {
	char *sql_statement = "SELECT pgroup, path FROM package WHERE name = ? AND path = ?";
	sqlite3_stmt *prepared_statement = NULL;

	sqlite3_prepare_v2(database_connection, sql_statement, 295, &prepared_statement, NULL);
	sqlite3_bind_text(prepared_statement, 1, pkg->name, -1, NULL);
	sqlite3_bind_text(prepared_statement, 2, pkg->path, -1, NULL);

	if (sqlite3_step(prepared_statement) != SQLITE_ROW) {
		fprintf(stderr, "ploc: Package at %s%s isn't recorded by ploc.\n", pkg->path, pkg->name);
		exit(PLOC_CONFLICT);
	}

	strcpy(pkg->group, sqlite3_column_text(prepared_statement, 0));
	sqlite3_finalize(prepared_statement);
	return;
}
