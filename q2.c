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

	char *sql_stmt = "select Airline_ID from routes except select DISTINCT r1.Airline_ID from routes r1, routes r2 where r1.Airline_ID = r2.Airline_ID and r1.Destination_airport_ID = r2.Source_airport_ID and r1.Source_airport_ID = r2.Destination_airport_ID;";

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



