/*
	Copyright 2025 Eric Hernandez

	This file is part of ploc.

	ploc is free software: you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	ploc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with ploc. If not, see <https://www.gnu.org/licenses/>.
*/

#include "package.h"
#include "database.h"

#include <sqlite3.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int ploc_install_package(sqlite3 *database_connection, struct Package *pkg, const char *input_path) {
	FILE *input_file = NULL;
	FILE *output_file = NULL;
	char output_path[275];
	char buffer[255];

	strcpy(output_path, pkg->path);
	strcat(output_path, pkg->name);

	ploc_check_conflict(database_connection, true, pkg);

	input_file = fopen(input_path, "rb");
	if (input_file == NULL) {
		fprintf(stderr, "ploc: Failed to open input package at %s\n", input_path);
		return 1;
	}

	output_file = fopen(output_path, "wb");
	if (output_file == NULL) {
		fprintf(stderr, "ploc: Failed to open output package at %s\n", output_path);
		return 1;
	}

	ploc_database_insert(database_connection, pkg);

	while ((fread(buffer, 255, 1, input_file)) != 0) {
		fwrite(buffer, 255, 1, output_file);
	}

	fclose(input_file);
	fclose(output_file);

	chmod(output_path, 0775);
	return 0;
}

int ploc_delete_package(sqlite3 *database_connection, struct Package *pkg) {
	int result = -1;
	char absolute_path[255];

	ploc_database_remove(database_connection, pkg);

	strcpy(absolute_path, pkg->path);
	strcat(absolute_path, pkg->name);

	result = unlink(absolute_path);
	if (result != 0) {
		fprintf(stderr, "ploc: Failed to delete package at %s\n", absolute_path);
		return PLOC_FS_FAIL;
	}

	return PLOC_OK;
}

void ploc_list_package(struct Package *pkg) {
	printf("Name:\t%s\n", pkg->name);
	printf("Group:\t%s\n", pkg->group);
	printf("Path:\t%s\n", pkg->path);
	return;
}
