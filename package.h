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

struct Package {
	char name[20];
	char group[20];
	char path[255];
	int id;
};

/* Default package values */
#define DEFAULT_GROUP "none"
#define DEFAULT_PATH "/usr/local/bin/"

/* Create a copy of a package and install it. */
int ploc_install_package(struct Package *pkg, const char *input_path);

#endif
