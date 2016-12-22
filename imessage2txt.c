/*
 * imessage2txt
 *
 * Written by Paul L Daniels (Inflex), 21 December 2016
 *
 * <insert BSD Revised licence>
 * <insert MIT licence>
 *
 * Query option #1
 * SELECT datetime(message.date, 'unixepoch', '+31 years', '+10 hours'), message.is_from_me, handle.id, message.text FROM message, handle WHERE message.handle_id = handle.ROWID order by message.date;
 * 
 * Query option #2
 *  SELECT datetime(message.date, 'unixepoch', '+31 years', '+10 hours'), handle.id, case message.is_from_me when '0' then "<=-" when '1' then "-=>" end as messagedir, message.text FROM message, handle WHERE message.handle_id = handle.ROWID group by handle.ROWID order by message.date;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

char help[]="-i <sms.db> [-h] [-o]\n\
\t-i <sms.db> : specify the input database file\n\
\t-o : Obfusicate the output\n\
\t-h : this help\n\
";

struct globals {
	char *input_file;
	int obfusicate;
} glb;

/*-----------------------------------------------------------------\
  Date Code:	: 20161221-091524
  Function Name	: int
  Returns Type	: static
  	----Parameter List
	1. callback( void *NotUsed, 
	2.  int argc, 
	3.  char **argv, 
	4.  char **azColName , 
	------------------
Exit Codes	: 
Side Effects	: 
--------------------------------------------------------------------
Comments:

--------------------------------------------------------------------
Changes:

\------------------------------------------------------------------*/
static int callback( void *NotUsed, int argc, char **argv, char **azColName ) {

	char *n="", *datecode, *handle_id, *is_from_me, *message_text;

	datecode = argv[0]?argv[0]:n;
	handle_id = argv[1]?argv[1]:n;
	is_from_me = argv[2]?argv[2]:n;
	message_text = argv[3]?argv[3]:n;

	/* deliberate obfusication of output */
	if (glb.obfusicate) {
	if (strlen(handle_id) > 7) handle_id[6] = '\0';

	{
		char *p = message_text;

		if (strlen(message_text) > 20) message_text[19] = '\0';
		while (*p) { 
			*p = '.';
			p++; if (*p == '\0') break;
			p++; if (*p == '\0') break;
			p++; if (*p == '\0') break;
		}
	}
	}
	/* end of debugging / custom  output */


	fprintf(stdout,"%s, %s, %s, %s\n"
			, datecode
			, handle_id
			, (is_from_me[0]=='1')?"SENT":"RECV"
			, message_text
		   );

	return 0;

}

/*-----------------------------------------------------------------\
  Date Code:	: 20161221-091521
  Function Name	: parse_parameters
  Returns Type	: int
  	----Parameter List
	1. struct globals *g, 
	2.  int argc, 
	3.  char **argv , 
	------------------
Exit Codes	: 
Side Effects	: 
--------------------------------------------------------------------
Comments:

--------------------------------------------------------------------
Changes:

\------------------------------------------------------------------*/
int parse_parameters( struct globals *g, int argc, char **argv ) {
	int i;

	for (i=0; i < argc; i++) {

		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'h':
					fprintf(stdout,"%s %s", argv[0], help);
					exit(1);
					break;

				case 'o':
					g->obfusicate = 1;
					break;

				case 'i':
					/* set the input file*/
					i++;
					if (i < argc) {
						g->input_file = argv[i];
					} else {
						fprintf(stderr,"Insuffient parameters\n");
						exit(1);
					}
					break;

				default:
					break;
			} // switch
		} // if
	} // for

	return 0;
}


/*-----------------------------------------------------------------\
  Date Code:	: 20161221-091518
  Function Name	: main
  Returns Type	: int
  	----Parameter List
	1. int argc, 
	2.  char **argv , 
	------------------
Exit Codes	: 
Side Effects	: 
--------------------------------------------------------------------
Comments:

--------------------------------------------------------------------
Changes:

\------------------------------------------------------------------*/
int main( int argc, char **argv ) {
	struct globals *g = &glb;
	sqlite3 *db;
	char sql[1024];
	char *zErrMsg = 0;
	int rc;

	parse_parameters( g, argc, argv );

	/*
	 * If the input file is non-null, try open...
	 */
	if (g->input_file) {

		rc = sqlite3_open( g->input_file, &db );

		if ( rc ) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return (1);
		}

		snprintf(sql, sizeof(sql), "SELECT datetime(message.date, 'unixepoch', '+31 years', '+10 hours'), handle.id, message.is_from_me, message.text FROM message, handle WHERE message.handle_id = handle.ROWID order by message.date;");
		rc = sqlite3_exec( db, sql, callback, 0, &zErrMsg);
		if ( rc != SQLITE_OK ) {
			fprintf(stderr,"SQL Error: %s\n", zErrMsg);
		}

		sqlite3_close(db);

	}

	return 0;

}
