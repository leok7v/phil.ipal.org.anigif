/***********************************************************************\
| Copyright © 1997, by Philip David Howard - all rights reserved	|
|									|
| author	Philip D. Howard					|
| email		phil@ipal.org                                           |
\***********************************************************************/

/***********************************************************************\
| program	gifscan							|
|									|
| purpose	Scan a GIF file to see what is in it.			|
|									|
| usage		gifscan < file.gif					|
|									|
| note		This program is "patent free" in that it contains no	|
|		code to implement any aspect of the LZW compression	|
|		algorithm whose patent is held by Unisys.		|
|									|
| note		GIF is a service mark, property of CompuServe, Inc.	|
|									|
| note		The GIF standard is being deprecated in favor of the	|
|		new PNG (Portable Network Graphics) standard.		|
\***********************************************************************/

#include <stdio.h>

/***********************************************************************\
| getbyte								|
| macro to read a byte and quit with an error message if EOF		|
\***********************************************************************/
#define getbyte(v,m) if(((v)=getc((infp)))==EOF)return "." m;

#define getbytepr(v,m) if(((v)=getc((infp)))==EOF)return "." m;printf(" %02x",(v));

#define getword(v,m,n) if((l=getc((infp)))==EOF)return "." m;printf(" %02x",l);if((h=getc((infp)))==EOF)return "." n;printf(" %02x",h);(v)=l+(h<<8);


/***********************************************************************\
| prch									|
| function to print a character only as printable characters		|
\***********************************************************************/
int
prch(
	int		ch
)
{
    switch ( ch ) {
    case '\a': fputs( "\\a" , stdout ); break;
    case '\b': fputs( "\\b" , stdout ); break;
    case   27: fputs( "\\e" , stdout ); break;
    case '\f': fputs( "\\f" , stdout ); break;
    case '\n': fputs( "\\n" , stdout ); break;
    case '\r': fputs( "\\r" , stdout ); break;
    case '\t': fputs( "\\t" , stdout ); break;
      default:
	if ( ch < 32 ) {
	    putc( '^' , stdout );
	    putc( ch + 0x40 , stdout );
	    return 2;
	}
	else if ( ch >= 127 ) {
	    putc( '\\' , stdout );
	    putc( "01234567"[ ( ch >> 6 ) & 7 ] , stdout );
	    putc( "01234567"[ ( ch >> 3 ) & 7 ] , stdout );
	    putc( "01234567"[   ch        & 7 ] , stdout );
	    return 4;
	}
	else if ( ch == '^' || ch == '\\' ) {
	    putc( ch , stdout );
	    putc( ch , stdout );
	    return 2;
	}
	else putc( ch , stdout );
    }
    return 1;
}


const char *
gifscan (
	int		argc
,
	char * *	argv
)
{
    static const char * cnames[ 3 ] = { "red" , "green" , "blue" };

    unsigned char buffer[ 256 ];

    FILE * infp;

    unsigned long pos;	/* current byte position in GIF file */

    int arat;	/* aspect ratio						*/
    int bgci;	/* background color index				*/
    int blkid;	/* block id						*/
    int bsize;	/* block size of data block				*/
    int ccw;	/* character cell width					*/
    int cch;	/* character cell height				*/
    int cres;	/* color resolution					*/
    int delay;	/* delay in 100ms units between images			*/
    int disp;	/* image disposal method				*/
    int eof;	/* end of file flag					*/
    int extid;	/* extension id						*/
    int gcsf;	/* global color sort flag				*/
    int gctp;	/* global color table present				*/
    int gcts;	/* global color table size				*/
    int h;	/* hi byte						*/
    int i;	/* loop index						*/
    int ih;	/* image height						*/
    int ilp;	/* image left position					*/
    int intf;	/* interlace flag					*/
    int itp;	/* image top position					*/
    int iw;	/* image width						*/
    int j;	/* loop index						*/
    int l;	/* lo byte						*/
    int lcsf;	/* local color sort flag				*/
    int lctp;	/* local color table present				*/
    int lcts;	/* local color table size				*/
    int lsh;	/* logical screen height				*/
    int lsw;	/* logical screen width					*/
    int mcs;	/* minimum code size					*/
    int resv;	/* reserved value					*/
    int tbci;	/* text background color index				*/
    int tfci;	/* text foreground color index				*/
    int tgh;	/* text grid height					*/
    int tgw;	/* text grid width					*/
    int tglp;	/* text grid left position				*/
    int tgtp;	/* text grid top position				*/
    int trcf;	/* transparent color flag				*/
    int trci;	/* transparent color index				*/
    int uinf;	/* user input flag					*/
    int vers;	/* version number, 87 or 89				*/

    trcf = 0;
    trci = 0;

    eof = 0;
    pos = 0;

    infp = stdin;


    /********\
    | header |
    \********/
    printf("%06lu:",pos);
    for ( i = 0 ; i < 6 ; ++ i ) {
	l = getc( infp );
	if ( l == EOF ) { eof = 1; break; }
	printf(" %02x",l);
	buffer[ i ] = l;
    }
    pos += 6;
    printf("\n");
    if ( eof ) {
	printf("EOF in GIF header, got %d byte%s\n",i,i==1?"":"s");
	return "";
    }
    if ( buffer[0] != 'G' || buffer[1] != 'I' || buffer[2] != 'F' ) {
	printf("Did not find \"GIF\" at beginning of file\n");
	return "";
    }
    if ( buffer[3] == '8' && buffer[4] == '7' && buffer[5] == 'a' ) {
	printf("GIF version 87a\n\n");
	vers = 87;
    }
    else if ( buffer[3] == '8' && buffer[4] == '9' && buffer[5] == 'a' ) {
	printf("GIF version 89a\n\n");
	vers = 89;
    }
    else {
	printf("Unrecognized GIF version %c%c%c\n",
	       buffer[3],buffer[4],buffer[5]);
    }

    /***************************\
    | logical screen descriptor |
    \***************************/

    /* screen width */
    printf("%06lu:",pos);
    getword(lsw,"in LSD at width lo byte","in LSD at width hi byte");
    pos += 2;
    printf("\nscreen width = %d\n\n",lsw);

    /* screen height */
    printf("%06lu:",pos);
    getword(lsh,"in LSD at height lo byte","in LSD at height hi byte");
    pos += 2;
    printf("\nscreen height = %d\n\n",lsh);

    /* packed fields */
    printf("%06lu:",pos);
    getbytepr(l,"in LSD at packed fields");
    ++ pos;
    gctp = ( l >> 7 ) & 1;
    cres = ( l >> 4 ) & 7;
    gcsf = ( l >> 3 ) & 1;
    gcts =   l        & 7;
    printf("\n%d....... global color table is %ssent\n",
	   (l>>7)&1,gctp?"pre":"ab");
    printf(".%d%d%d.... color resolution bits = %d+1 (%d values)\n",
	   (l>>6)&1,(l>>5)&1,(l>>4)&1,cres,1<<(cres+1));
    printf("....%d... global color table is%s sorted\n",
	   (l>>3)&1,gcsf?" ":" not");
    printf(".....%d%d%d global color table size = 3*2^(%d+1)"
	   " (%d entries, %d bytes)\n\n",
	   (l>>2)&1,(l>>1)&1,l&1,gcts,1<<(gcts+1),3<<(gcts+1));

    /* background color */
    printf("%06lu:",pos);
    getbytepr(bgci,"in LSD at background color index");
    ++ pos;
    printf("\nbackground color index = %d\n",bgci);
    if ( bgci > ( 1 << ( gcts + 1 ) ) ) {
	printf("    exceeds size of global color table (%d)\n",1<<(gcts+1));
    }
    printf("\n");

    /* aspect ratio */
    printf("%06lu:",pos);
    getbytepr(arat,"in LSD at aspect ratio");
    ++ pos;
    if ( arat ) {
	double farat;
	farat = (double) arat;
	farat += 15.0;
	farat /= 64.0;
	printf("\naspect ratio factor = %d (ratio is %10.6f)\n\n",arat,farat);
    }
    else {
	printf("\naspect ratio factor = 0 (no ratio information)\n\n");
    }

    /********************\
    | global color table |
    \********************/
    if ( gctp ) {
	printf("global color table detail:  (* indicates background color)\n");
	h = 1 << ( gcts + 1 );
	for ( i = 0 ; i < h ; ++ i ) {
	    if ( i % 4 == 0 ) {
		if ( i ) printf("\n");
		printf("%06lu:",pos);
	    }
	    else {
		printf(" ");
	    }
	    for ( j = 0 ; j < 3 ; ++ j ) {
		if ( ( l = getc( infp ) ) == EOF ) {
		    printf("EOF\nunexpected EOF in LSD global color table,"
			   " index %d %s\n",i,cnames[j]);
		    return "";
		}
		printf(" %02x",l);
	    }
	    printf("%c(%03d)",i==bgci?'*':' ',i);
	    pos += 3;
	}
	printf("\n\n");
    }

    /**********************\
    | various other blocks |
    \**********************/
    for (;;) {
	int data;
	int out;

	data = 0;
	out = 0;
	
	printf("%06lu:",pos);
	getbytepr(blkid,"between data blocks (terminator missing)");
	++ pos;

	switch ( blkid ) {

	  case 0x21: /* extension introducer */
	    printf("\nextension introducer:\n\n");

	    /**************************\
	    | various extension blocks |
	    \**************************/
	    printf("%06lu:",pos);
	    getbytepr(extid,"at extension ID");
	    ++ pos;
	    getbytepr(bsize,"at extension block size");
	    ++ pos;

	    switch ( extid ) {

	      case 0x01: /* plain text extension */
		printf("\nplain text extension, block size = %d ",bsize);
		if ( bsize < 12 ) {
		    printf("(too small, should be %d, skipping block)\n\n",12);
		    break;
		}
		else if ( bsize > 12 ) {
		    printf("(too large, should be %d, will skip excess)\n\n",12);
		}
		else {
		    printf("(valid size)\n\n");
		}

		/* text grid left pos */
		getword(tglp,"in PTE at TGLP lo byte","in PTE at TGLP hi byte");
		pos += 2;
		bsize -= 2;
		printf("\ntext grid left position = %d\n\n",tglp);

		getword(tgtp,"in PTE at TGTP lo byte","in PTE at TGTP hi byte");
		pos += 2;
		bsize -= 2;
		printf("\ntext grid top position = %d\n\n",tgtp);

		getword(tgw,"in PTE at TGW lo byte","in PTE at TGW hi byte");
		pos += 2;
		bsize -= 2;
		printf("\ntext grid width = %d\n\n",tgw);

		getword(tgh,"in PTE at TGH lo byte","in PTE at TGH hi byte");
		pos += 2;
		bsize -= 2;
		printf("\ntext grid height = %d\n\n",tgh);

		getbyte(ccw,"in PTE at CCW");
		++ pos;
		-- bsize;
		printf("\ncharacter cell width = %d\n\n",ccw);

		getbyte(cch,"in PTE at CCH");
		++ pos;
		-- bsize;
		printf("\ncharacter cell height = %d\n\n",cch);

		getbyte(tfci,"in PTE at TFCI");
		++ pos;
		-- bsize;
		printf("\ntext foreground color index = %d\n\n",tfci);

		getbyte(tbci,"in PTE at TBCI");
		++ pos;
		-- bsize;
		printf("\ntext background color index = %d\n\n",tbci);

		data = 1;
		break;

	      case 0xf9: /* graphic control extension */
		printf("\ngraphics control extension, block size = %d ",bsize);
		if ( bsize < 4 ) {
		    printf("(too small, should be %d, skipping block)\n\n",4);
		    break;
		}
		else if ( bsize > 4 ) {
		    printf("(too large, should be %d, will skip excess)\n\n",4);
		}
		else {
		    printf("(valid size)\n\n");
		}

		/* packed fields */
		printf("%06lu:",pos);
		getbytepr(l,"in GCE at packed fields");
		++ pos;
		-- bsize;

		resv = ( 1 >> 5 ) & 7;
		disp = ( l >> 2 ) & 7;
		uinf = ( l >> 1 ) & 1;
		trcf =   l        & 1;
		printf("\n%d%d%d..... reserved value = %d\n",
		       (l>>7)&1,(l>>6)&1,(l>>5)&1,resv);
		printf("...%d%d%d.. disposal method = %d (",
		       (l>>4)&1,(l>>3)&1,(l>>2)&1,disp);
		switch ( disp ) {
		  case 0:
		    printf("no disposal specified, no action required)\n");
		    break;
		  case 1:
		    printf("do not dispose, leave graphic in place)\n");
		    break;
		  case 2:
		    printf("restore to background color)\n");
		    break;
		  case 3:
		    printf("restore to previous contents)\n");
		    break;
		  default:
		    printf("undefined disposal method)\n");
		    break;
		}
		printf("......%d. user input flag = %s\n",
		       (l>>1)&1,uinf?"yes":"no");
		printf(".......%d transparent color flag = %ssent\n\n",
		       l&1,trcf?"pre":"ab");

		/* delay time */
		printf("%06lu:",pos);
		getword(delay,"in GCE at delay lo byte","in GCE at delay hi byte");
		pos += 2;
		bsize -= 2;
		printf("\ndelay time = %d (%d.%02d secs)\n\n",delay,
		       delay/100,delay%100);

		/* transparent color index */
		printf("%06lu:",pos);
		getbytepr(trci,"in GCE at transparent color index");
		++ pos;
		-- bsize;
		printf("\ntransparent color index = %d\n\n",trci);

		data = 1;
		break;

	      case 0xfe: /* comment extension */
		for (;;) {
		    printf("\ncomment extension, block size = %d\n",bsize);
		    if ( bsize == 0 ) break;
		    h = 0;
		    while ( bsize -- ) {
			if ( h % 16 == 0 ) printf("%06lu: ",pos);
			getbyte(l,"in comment data");
			prch( l );
			++ pos;
			++ h;
			if ( h % 16 == 0 ) printf("\n");
		    }
		    getbyte(bsize,"in comment size");
		    ++ pos;
		}
		printf("\n");
		data = 0;
		break;

	      case 0xff: /* application extension */
		printf("\napplication extension block, size= %d ",bsize);
		if ( bsize < 11 ) {
		    printf("(too small, should be %d, skipping block)\n\n",11);
		    break;
		}
		else if ( bsize > 11 ) {
		    printf("(too large, should be %d, will skip excess\n\n",11);
		}
		else {
		    printf("(valid size)\n\n");
		}

		printf("%06lu:",pos);
		for ( i = 0 ; i < 8 ; ++ i ) {
		    getbytepr(l,"in AEB at application identifier");
		    buffer[ i ] = l;
		}
		printf("\napplication identifier = \"");
		for ( i = 0 ; i < 8 ; ++ i ) {
		    prch( buffer[ i ] );
		}
		printf("\"\n\n");
		pos += 8;
		bsize -= 8;

		printf("%06lu:",pos);
		for ( i = 0 ; i < 3 ; ++ i ) {
		    getbytepr(l,"in AEB at application authentication code");
		    buffer[ i ] = l;
		}
		printf("\napplication authentication code = \"");
		for ( i = 0 ; i < 3 ; ++ i ) {
		    prch( buffer[ i ] );
		}
		printf("\"\n\n");
		pos += 3;
		bsize -= 3;

		data = 1;
		break;

	      default: /* unrecognized */
		printf("\nunrecognized extension type: 0x%02x"
		       "  (unable to proceed with scan)\n",
		       extid);
		out = 1;
		break;

	    }

	    
	    /****************************************************\
	    | skip to end of extension block if anything remains |
	    \****************************************************/
	    if ( bsize ) {
		printf("\nskipping %d bytes to end of extension block\n",
		       bsize);
		h = 0;
		while ( bsize -- ) {
		    if ( h % 32 == 0 ) printf("%06lu:",pos);
		    getbytepr(l,"while skipping to end of extension block");
		    ++ pos;
		    ++ h;
		}
	    }

	    /*******************************************************\
	    | skip additional data blocks until terminator is found |
	    \*******************************************************/
	    if ( data ) for (;;) {
		printf("%06lu:",pos);
		getbytepr(bsize,"at data block size");
		++ pos;
		if ( bsize == 0 ) {
		    printf("\nempty data block terminator\n\n");
		    break;
		}
		printf("\ndata block size = %d\n",bsize);
		h = 0;
		while ( bsize -- ) {
		    if ( h % 16 == 0 ) printf("%06lu:",pos);
		    getbytepr(l,"in data block");
		    ++ pos;
		    ++ h;
		    if ( h % 16 == 0 ) printf("\n");
		}
		printf("\n\n");
	    }

	    break;

	  case 0x2c: /* image separator */
	    printf("\nimage separator:\n\n");

	    /* image left position */
	    printf("%06lu:",pos);
	    getword(ilp,"in ID at ILP lo byte","in ID at ILP hi byte");
	    printf("\nimage left position = %d\n\n",ilp);
	    pos += 2;

	    /* image top position */
	    printf("%06lu:",pos);
	    getword(itp,"in ID at ITP lo byte","in ID at ITP hi byte");
	    printf("\nimage top position = %d\n\n",itp);
	    pos += 2;

	    /* image width */
	    printf("%06lu:",pos);
	    getword(iw,"in ID at IW lo byte","in ID at IW hi byte");
	    printf("\nimage width = %d\n\n",iw);
	    pos += 2;

	    /* image height */
	    printf("%06lu:",pos);
	    getword(ih,"in ID at IH lo byte","in ID at IH hi byte");
	    printf("\nimage height = %d\n",ih);
	    pos += 2;

	    /* check image against screen */
	    if ( ilp + iw > lsw ) {
		printf("this image is off the right of the screen\n");
	    }
	    if ( itp + ih > lsh ) {
		printf("this image is off the bottom of the screen\n");
	    }
	    printf("\n");

	    /* packed fields */
	    printf("%06lu:",pos);
	    getbytepr(l,"in ID at packed fields");
	    ++ pos;
	    lctp = ( l >> 7 ) & 1;
	    intf = ( l >> 6 ) & 1;
	    lcsf = ( l >> 5 ) & 1;
	    resv = ( l >> 3 ) & 3;
	    lcts =   l        & 7;
	    printf("\n%d....... local color table is %ssent\n",
		   (l>>7)&1,lctp?"pre":"ab");
	    printf(".%d...... image is%s interlaced\n",
		   (l>>6)&1,intf?" ":" not");
	    printf("..%d..... local color table is%s sorted\n",
		   (l>>5)&1,lcsf?" ":" not");
	    printf(".....%d%d%d local color table size = 3*2^(%d+1)"
		   " (%d entries, %d bytes)\n\n",
		   (l>>2)&1,(l>>1)&1,l&1,lcts,1<<(lcts+1),3<<(lcts+1));

	    /*******************\
	    | local color table |
	    \*******************/
	    if ( lctp ) {
		if ( trcf && trci == bgci ) {
		    printf("local color table detail:  (# indicates BG and TRANS color)\n");
		}
		else {
		    printf("local color table detail:  (* indicates background color)\n");
		    if ( trcf ) {
			printf("                           (+ indicates transparent color)\n");
		    }
		}
		h = 1 << ( lcts + 1 );
		for ( i = 0 ; i < h ; ++ i ) {
		    if ( i % 4 == 0 ) {
			if ( i ) printf("\n");
			printf("%06lu:",pos);
		    }
		    else {
			printf(" ");
		    }
		    for ( j = 0 ; j < 3 ; ++ j ) {
			if ( ( l = getc( infp ) ) == EOF ) {
			    printf("EOF\nunexpected EOF in ID local color table,"
				   " index %d %s\n",i,cnames[j]);
			    return "";
			}
			printf(" %02x",l);
		    }
		    if ( trcf && trci == bgci ) putc(i==trci?'#':' ',stdout);
		    else putc(i==trci?'+':(i==bgci?'*':' '),stdout);
		    printf("(%03d)",i);
		    pos += 3;
		}
		printf("\n\n");
	    }
	    trcf = 0; /* invalidate the translate for another image */

	    /* image data min code size */
	    printf("%06lu:",pos);
	    getbytepr(mcs,"after ID (and LCT) at LZW min code size");
	    ++ pos;
	    printf("\nLZW minimum code size = %d",mcs);

	    for (;;) {
		printf("\n\n%06lu:",pos);
		getbytepr(bsize,"at data block size");
		++ pos;
		printf("\nimage data block size = %d",bsize);
		if ( bsize == 0 ) break;
		h = 0;
		while ( bsize -- ) {
		    if ( h % 20 == 0 ) printf("\n%06lu:",pos);
		    getbytepr(l,"in data block");
		    ++ pos;
		    ++ h;
		}
	    }
	    printf("\n\n");

	    break;

	  case 0x3b: /* trailer */
	    printf("\ntrailer - end of GIF file\n");

	    /* we want an EOF here */
	    h = 0;
	    while ( ( l = getc( infp ) ) != EOF ) {
		if ( h % 320 == 0 ) printf("\nGIF ended, Expecting EOF:\n");
		if ( h % 20 == 0 ) printf("%06lu:",pos);
		printf(" %02x",l);
		++ h;
		if ( h % 20 == 0 ) printf("\n");
	    }

	    out = 1;
	    break;

	  default: /* unrecognized */
	    printf("\nunrecognized block type: 0x%02x"
		   "  (unable to proceed with scan)\n",
		   blkid);
	    out = 1;
	    break;

	}
	if ( out ) break;
    }

    return 0;
}

/***********************************************************************\
| main									|
\***********************************************************************/
int
main (
	int		argc
,
	char * *	argv
)

{
    const char * msg;

    msg = gifscan( argc , argv );

    if ( ! msg ) return 0;

    if ( ! msg[0] ) return 1;

    if ( msg[0] == '.' ) {
	printf( " EOF\nunexpected end of file encountered %s\n" , msg+1 );
    }
    else {
	printf( "%s\n" , msg );
    }

    return 1;
}
