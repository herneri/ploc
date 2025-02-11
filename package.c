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

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sqlite3.h>

int ploc_install_package(sqlite3 *database_connection, struct Package *pkg, const char *input_path) {
	FILE *input_file = NULL;
	FILE *output_file = NULL;
	char *output_path = strcat(pkg->path, pkg->name);
	char buffer[255];

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
