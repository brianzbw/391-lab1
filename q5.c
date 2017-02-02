#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <math.h>

#define PI 3.1415926
#define f 1/298.257223563
#define ER 6378137







/*use Lambert's formula for long lines from https://en.wikipedia.org/wiki/Geographical_distance#cite_note-13*/
void calculateDis(sqlite3_context* ctx, int nargs, sqlite3_value** values){
	double lat1, lat2, long1, long2;
	
	lat1 = sqlite3_value_double(values[0]);
	long1 = sqlite3_value_double(values[1]);
	lat2 = sqlite3_value_double(values[2]);
	long2 = sqlite3_value_double(values[3]);
	
	/*convert to radian*/
	lat1 = lat1 * PI / 180;
	lat2 = lat2 * PI / 180;
	long1 = long1 * PI /180;
	long2 = long2 * PI /180;
	
	/*convert to reduced latitudes*/
	double rLat1 = atan((1 - f) * tan(lat1));
	double rLat2 = atan((1 - f) * tan(lat2));
	
	/*calculate central angle*/
	double ca = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * fabs(long1 - long2));
	
	double P = (lat1 + lat2) / 2;
	double Q = (lat2 - lat1) / 2;
	double X = (ca - sin(ca)) *  pow(sin(P), 2) * pow(cos(Q), 2) / pow(cos(ca/2), 2);
	double Y = (ca + sin(ca)) *  pow(cos(P), 2) * pow(sin(Q), 2) / pow(sin(ca/2), 2);
	
	/*calculate distance*/
	double distance = ER * (ca - (f/2) * (X -Y));
	
	sqlite3_result_double(ctx, distance);
	}
	

int main(int argc, char **argv){
	sqlite3 *db; //the database
	sqlite3_stmt *stmt; //the query
	int rc;

	if( argc!=2 ){
		fprintf(stderr, "Usage: %s <database file>\n", argv[0]);
		return(1);
	}

	rc = sqlite3_open(argv[1], &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	/* can only create the function after the db connection is established */
	sqlite3_create_function( db, "calculateDistance", 4, SQLITE_UTF8, NULL, calculateDis, NULL, NULL);

	/* the functions can now be used in regular SQL! */
	char *sql_qry = "select DISTINCT r.equipment as planes from routes r, airports a1, airports a2 where r.source_airport_ID = a1.airport_ID and r.destination_airport_ID = a2.airport_ID and calculateDistance(a1.latitude, a1.longitude, a2.latitude, a2.longitude) > 10000000;";
	rc = sqlite3_prepare_v2(db, sql_qry, -1, &stmt, 0);

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
	/*print_result(stmt_q);*/
	sqlite3_finalize(stmt); //always finalize a statement

	sqlite3_close(db);
	return 0;
}

