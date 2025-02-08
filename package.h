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
	char *name;
	char *group;
	char *path;
	int id;
};

/* Default package values */
const char *default_group = "none";
const char *default_path = "/usr/local/bin/";

#endif
