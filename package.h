/*
	Copyright 2025 Eric Hernandez

	This file is part of ploc.

	ploc is free software: you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	ploc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with ploc. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PLOC_PKG_H
#define PLOC_PKG_H

#include <sqlite3.h>

struct Package {
	char name[20];
	char group[20];
	char path[255];
	int id;
};

/* Default package values */
#define DEFAULT_GROUP "none"
#define DEFAULT_PATH "/usr/local/bin/"

enum ploc_exit_codes {
	PLOC_OK, PLOC_DB_FAIL, PLOC_INIT_FAIL,
	PLOC_FS_FAIL, PLOC_CONFLICT, PLOC_MEM_FAIL,
	PLOC_ARG_SYNTAX_ERR
};

/* Create a copy of a package and install it. */
int ploc_install_package(sqlite3 *database_connection, struct Package *pkg, const char *input_path);

/* Remove a package from the file system and delete its database entry. */
int ploc_delete_package(sqlite3 *database_connection, struct Package *pkg);

/* List metadata from a loaded package. */
void ploc_list_package(struct Package *pkg);

/* Find and set the name and path given in an argument. */
void ploc_get_name_and_path(struct Package *pkg, char *arg);

#endif
