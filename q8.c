#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

/*Bowen ZHu
part of the code used from eclass:
https://sites.ualberta.ca/~denilson/files/cmput391/sample_code/select.c
*/

void print_result(sqlite3_stmt *stmt);

int main(int argc, char **argv){
	sqlite3 *db; //the database
	sqlite3_stmt    *stmt_q; 
	int rc;

	rc = sqlite3_open("lab1.sql", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	char *sql_qry = "WITH RECURSIVE reached(destination_airport_ID) AS(select destination_airport_ID from routes where source_airport = ? UNION select r.destination_airport_ID from reached re, routes r where re.destination_airport_ID = r.source_airport_ID) select source_airport_ID from routes except select * from reached;";

	rc = sqlite3_prepare_v2(db, sql_qry, -1, &stmt_q, 0);
	if (rc != SQLITE_OK) {  
		fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}

	/* iterate through different ids */
	char input_id[4];
	printf("enter id: ");
	scanf("%s",input_id);

	sqlite3_bind_text(stmt_q, 1, input_id, -1, SQLITE_STATIC); //http://www.adp-gmbh.ch/sqlite/bind_insert.html

	print_result(stmt_q);
	// always reset the compiled statement and clear the bindings
	sqlite3_reset(stmt_q);
	sqlite3_clear_bindings(stmt_q);



	sqlite3_finalize(stmt_q); //always finalize a statement

	sqlite3_close(db);
	return 0;
}

void print_result(sqlite3_stmt *stmt){
	int rc;

	while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		int col;
		for(col=0; col<sqlite3_column_count(stmt)-1; col++) {
			printf("%s|", sqlite3_column_text(stmt, col));
		}
		printf("%s", sqlite3_column_text(stmt, col));
		printf("\n");
	}


}
