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
		fprintf(stderr, "Usage: %s <database file> \n", argv[0]);
		return(1);
	}

	rc = sqlite3_open(argv[1], &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	char *sql_stmt = "WITH RECURSIVE reached(destination_airport_ID) AS(select destination_airport_ID from routes where source_airport_ID = 49 UNION select r.destination_airport_ID from reached re, routes r where re.destination_airport_ID = r.source_airport_ID) select source_airport_ID from routes except select * from reached;";

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
