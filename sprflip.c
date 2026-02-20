/* sprflip -- (Sprite Flip)
 * The program re-express text representing sprite data
 * mirrored horizontally and vertically.
 * This will help expedite creating sprites that rotate or
 * that need to appear in left-handed and right handed version.
 * Also, this is useful just to create the assembly language
 * data bitmaps based on only a text picture of the data.
 *
 * Output provides bitmaps for Player 0 and Player 1.
 *
 * Sprite data input is expressed as
 * - (0) or (.) period for empty space/transparent
 * - (1) for Player 0 bit, -
 * - (2) for Player 1 bit, and
 * - (3) for overlap.
 *
 * At this time Player data is only 8 bits per line.
 * Any characters other than the characters described
 * will be discarded.
 * More than 8 usable bytes will be discarded.
 *
 * Up to 256 lines of data may be provided.
 * The program will ignore further data.
 * But, why would you do this?
 *
 * A short line of data will be padded with zeros to the right.
 *
 * An outright empty line (no convertible characters) will be ignored.
 *
 * (In the future, this could be expanded to allow for
 * 10 bits to include the missiles, and/or 16 bits or 20 bits
 * to include all four players and missiles offset from each
 * other.)
 *
 * Given input data:   . 1 . 2 3 . . 2
 *
 * The output will resemble:
 * ; 0 1 2 3 4 5 6 7   PM0               PM1
 * ; . 1 . 2 3 . . 2   0 1 0 0 1 0 0 0   0 0 0 1 1 0 0 1
 * ;
 * ; _PM0
 * ;   .by %01001000
 * ;
 * ; _PM1
 * ;   .by %00011001
 *
 * The block above will be output for each orientation.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



		
/* Output '1' or '0' based on the pixels belonging
 * to the Player. (the pixels are listed in "onebits").
 */
 
int dumpPMbits(char * data,    /* String of pixels */
			   int    order,   /* Order to output pixels/bits (0)/left to right, (1)/Right to left */
			   char * onebits, /* List of pixels that should be '1' bits */
			   int    padme )  /* Blank padding between pixels? 0/No, 1/Yes. */
{
	int loop = 0;

	if (!data || !onebits || (order < 0) || (order > 1) )
	{
		fprintf(stderr, "sprflip: dumpPMbits() bad args\n");
		return -1;
	}

	if ( order )  /* If 1 then reverse order */
		data += 7 ; /* start at the end */

	while (loop < 8)
	{
		if (strchr(onebits,*data))
			fputc('1',stdout);
		else
			fputc('0',stdout);

		if (padme)
			fputc(' ',stdout);

		if ( order )
			data--;
		else
			data++;

		loop++;
	}

	return 0;
}



/* A comment line has the text map of the pixels followed by the
 * PM0 and PM1 bitmaps on the same line.
 */
 
int commentLine(char * l,
				int    order )
{
	fprintf(stdout,"; ");

	if ( !order ) /* Left to Right */
		fprintf(stdout, "%c %c %c %c %c %c %c %c   ",
				*(l),*(l+1),*(l+2),*(l+3),*(l+4),*(l+5),*(l+6),*(l+7) );
	else
		fprintf(stdout, "%c %c %c %c %c %c %c %c   ",
				*(l+7),*(l+6),*(l+5),*(l+4),*(l+3),*(l+2),*(l+1),*(l) );
				
	dumpPMbits(l, order, "13", 1); /* '1' is padding between bits */

	fprintf(stdout,"  ");          /* more space between columns */

	dumpPMbits(l, order, "23", 1); /* '1' is padding between bits */

	fputc('\n', stdout);
	
	return 0;
}
			


/* Iterate through the pixel rows and output the bits for 
 * a player's pixels (which are the pixels in the onpixels list.)
 */
 
int playerBitmaps(char   data[][9],
				  int    start,
				  int    end,
				  int    order,
				  char * onpixels)
{
	int    loop = start;
	char * line      = NULL;
	
	while ( loop != end )
	{
		line = data[loop];

		fprintf(stdout,"\t.by %%"); /* tab indent, and .by data type declaration and % for binary */

		dumpPMbits(line, order, onpixels, 0); /* NO padding between bits */

		fputc('\n',stdout);
		
		if ( start < end )
			loop++;
		else
			loop--;
	}
	
	return 0;
}


/* Dump data lines in order from top to bottom */

int dumpLines( char data[][9],
			   int  start,
			   int  end,
			   int  order ) /* (0) Left To Right  v  (1) Right To Left */
{
	int    loop = start;
	char * line = NULL;
	
	/* First Section... Picture of coded pixels and bits in comments. */
	
	fprintf(stdout,"\n; 0 1 2 3 4 5 6 7   PM0               PM1\n"); 	/* Comment Title */

	while (loop != end )
	{
		line = data[loop];

		commentLine(line,order);

		if ( start < end )
			loop++;
		else
			loop--;
	}

	/* Next Section.  First PM Object's bits. */

	fprintf(stdout,"\nPM0_something\n");
	
	playerBitmaps(data, start, end, order,"13");


	/* Last Section.  Second PM Object's bits. */

	fprintf(stdout,"\nPM1_something\n");

	playerBitmaps(data, start, end, order,"23");

	fputc('\n',stdout);

	return 0;
}




int main( int argc, char ** argv )
{
	char line[257];     /* The stdio input buffer. Please don't break me */
	char spr[256][9];   /* Please don't break me either */
	int  numLines = 0 ; /* Number of lines of data read from file.*/
	int  len      = 0 ; /* length of the current line being read.*/
	char * c;
	char * s;
	char * directs = NULL;
	char * defdir = "n";
	char   opt;


	while ( (opt = getopt(argc, argv, "d:") ) != -1) 
	{
		switch (opt) 
		{
			case 'd':
				directs = optarg;
				break;
				   
			default: /* '?' */
				fprintf(stderr, 
					"Usage: %s [-d nhv]\n\nn = Normal (default)\nh = Horizontal flip\nv = Vertical flip\n\n",
					argv[0] );
				exit(EXIT_FAILURE);
		}
	}
	
	if ( ! directs )
		directs = defdir;
	
	while ( (c = (char *)fgets( line, 256, stdin ) ) && (numLines < 256) )
	{
		line[256] = '\0';  /* Force end of string just in case.  I'm lazy. */

		if ( line[0] == '#' ) /* Ignore commented line */
			continue;

		s = c;

		while ( *s )  /* strip/discard invalid chars */
		{
			if ( strchr( ".0123", *s ) )
				*c++ = *s++;
			else
				s++;
		}

		*c = '\0';                    /* Force end of string here. */

		if ( !strlen(line) ) /* no data to process. skip it */
		{
			fprintf(stderr, "sprflip: No usable data read at line %d. Skipping...\n",numLines+1);
			continue;
		}

		strncat(line, ".......", 8); /* Force padding in line to at least length 8 */

		line[8] = '\0';  /* force end of string */

		strcpy( spr[numLines++], line ); /* Copy line to sprite data array. */ 
	}

	if ( !numLines )
	{
		fprintf(stderr, "sprflip: No Lines Read\n" );
		exit(0);
	}

	if ( strchr(directs,'n') ) 
		dumpLines(spr,0,numLines,0); /* data, start, end++, left to right pixel order) */

	if ( strchr( directs, 'h') && !(strchr( directs, 'v') ) )
		dumpLines(spr,0,numLines,1); /* data, start, end++, right to left pixel order) */

	if ( !strchr( directs, 'h') && (strchr( directs, 'v') ) )
		dumpLines(spr,numLines-1,-1,0); /* data, start, end--, left to right pixel order) */
	
	if ( strchr( directs, 'h') && (strchr( directs, 'v') ) )
		dumpLines(spr,numLines-1,-1,1); /* data, start, end--, right to left pixel order) */

	return 0;
}



