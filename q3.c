#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
/*Bowen ZHu
part of the code used from eclass:
https://sites.ualberta.ca/~denilson/files/cmput391/sample_code/select.c
*/

int main(int argc,  char **argv){
	sqlite3 *db; //the database
	sqlite3_stmt *stmt; //the update statement

	int rc;

	if( argc!=2 ){
		fprintf(stderr, "Usage: %s <database file> <select query>\n", argv[0]);
		return(1);
	}

	rc = sqlite3_open(argv[1], &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	char *sql_stmt = "select a1.name, count(DISTINCT r.airline_ID)\
			from airports a1, airports a2, routes r\
			where a1.country = 'Canada' and \
			(r.destination_airport_ID = a1.airport_ID and a2.airport_ID = r.source_airport_ID) \
			or (r.source_airport_ID = a1.airport_ID and a2.airport_ID = r.destination_airport_ID ) \
			GROUP BY a1.airport_ID \
			ORDER BY count(DISTINCT r.airline_ID) DESC LIMIT 10;";/*limit function using from http://stackoverflow.com/questions/2728999/how-to-get-top-5-records-in-sqlite */

	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);

	if (rc != SQLITE_OK) {  
		fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}    

	while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		int col;
		for(col=0; col<sqlite3_column_count(stmt)-1; col++) {
			printf("%s|", sqlite3_column_text(stmt, col));
		}
		printf("%s", sqlite3_column_text(stmt, col));
		printf("\n");
	}

	sqlite3_finalize(stmt); //always finalize a statement
}


