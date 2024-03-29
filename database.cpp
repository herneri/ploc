/*
    Copyright 2024 Eric Hernandez

    This file is part of ploc.

    ploc is free software: you can redistribute it and/or modify it under the terms of the GNU General
    Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    ploc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with ploc. If not, see <https://www.gnu.org/licenses/>.
*/

#include "sqlite_src/sqlite3.h"
#include "id_linked_list.cpp"

using namespace std;

class Database {
	private:
		static sqlite3 *db;
		const static char *name;

		static string sql_statement;
		static char *error_message;
		static char *target;
		
		static int status;
		static int query_type;

	public:
		enum operation_values {
			DB_REMOVE, DB_REPLACE, DB_INSERT
		};

		/* The database connection opened here must be closed
		   wherever this function is used */
		static void connect() {
			status = sqlite3_open(name, &db);
			if (status < 0) {
				cerr << "ploc: Failed to open database" << endl;
			}

			return;
		}

		static void disconnect() {
			sqlite3_close(db);
			return;
		}

		static void initialize() {
			sql_statement = "CREATE TABLE IF NOT EXISTS package(\
							 	id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
								name VARCHAR(20) NOT NULL,\
								pgroup VARCHAR(20) DEFAULT 'none',\
								path VARCHAR(255) DEFAULT '' NOT NULL\
							 );\
							 \
							 CREATE TABLE path(\
							 	id INT PRIMARY KEY AUTO_INCREMENT NOT NULL,\
							 	name VARCHAR(255) NOT NULL\
							 );";

			status = sqlite3_exec(db, sql_statement.c_str(), NULL, NULL, &error_message);
			if (status < 0) {
				cout << error_message << endl;
			}

			return;
		}

		static void set_target(char *target) {
			Database::target = target;
			return;
		}

		static void general_search() {
			sqlite3_stmt *prepared_statement;

			enum columns {
				PATH, NAME
			};

			sql_statement = "SELECT path, name FROM package WHERE name = '" + (string) target + "';";
			status = sqlite3_prepare(db, sql_statement.c_str(), -1, &prepared_statement, NULL);
			if (status < 0) {
				cout << error_message << endl;
				return;
			}

			while (status = sqlite3_step(prepared_statement) == SQLITE_ROW) {
				cout << (char *) sqlite3_column_text(prepared_statement, PATH) << (char *) sqlite3_column_text(prepared_statement, NAME) << endl;
			}

			sqlite3_finalize(prepared_statement);
			return;
		}

		static int instance_search() {
			struct id_list::id_node *ids = NULL;
			sqlite3_stmt *prepared_statement;
			string package_name;
			int selected_id;

			enum column {
				PATH, ID
			};

			sql_statement = "SELECT path, id FROM package WHERE name = '" + (string) target + "';";
			status = sqlite3_prepare(db, sql_statement.c_str(), -1, &prepared_statement, NULL);
			if (status < 0) {
				cout << error_message << endl;
				return 1;
			}

			while (status = sqlite3_step(prepared_statement) == SQLITE_ROW) {
				package_name = (char *) sqlite3_column_text(prepared_statement, PATH);
				package_name = package_name + target;
				ids = id_list::add_tail(ids, sqlite3_column_int(prepared_statement, ID), (char *) package_name.c_str());
			}

			sqlite3_finalize(prepared_statement);
			id_list::print_list(ids);
			cout << "Multiple packages named '" + (string) target + "' found" << endl;
			cout << "Select id of one: ";
			cin >> selected_id;

			status = id_list::search_for_id(ids, selected_id);
			if (status != 0) {
				return -1;
			}

			return selected_id;
		}

		/* Provides insertion and deletion in the database */
		/* Updating is done in a dedicated function */
		static int modify(Package pkg, char operation) {
			if (operation == DB_INSERT) {
				sql_statement = "INSERT INTO package(name, path) VALUES('" + (string) pkg.name + "', '" + (string) pkg.path + "');";
			} else if (operation == DB_REMOVE) {
				sql_statement = "DELETE FROM package WHERE id = " + to_string(pkg.id) + ";";
			}

			status = sqlite3_exec(db, sql_statement.c_str(), NULL, NULL, &error_message);
			if (status < 0) {
				cout << "ploc: " << error_message << endl;
				return 1;
			}

			return 0;
		}
};

sqlite3 *Database::db;
const char *Database::name = "ploc.db";

string Database::sql_statement;
char *Database::error_message;
char *Database::target;

int Database::status;
int Database::query_type;
