/*
   This file is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

/*
	This file is shared by Megalopa and Zoea
*/

#include "compiler.h"

/*
	static const int reserved_var_names[];
	This dimension contains var name integers of reserved var names.
	To make following structure, execute "reservednames.js" in Windows.
*/

static const int reserved_var_names[]={
<<<<<<< HEAD
	0x000106b8, /*ABS*/
	0x0001f67c, /*ACOS*/
	0x0002414c, /*ARGS*/
	0x0001090c, /*ASC*/
	0x0002469f, /*ASIN*/
	0x00024a8f, /*ATAN*/
	0x002f7c1e, /*ATAN2*/
	0x0047c31c, /*BREAK*/
	0x00035869, /*CALL*/
	0x00575afe, /*CDATA*/
	0x00036c3d, /*CEIL*/
	0x000111af, /*CHR*/
	0x0cb1b682, /*CIRCLE*/
	0x005d1ea3, /*CLEAR*/
	0x00011240, /*CLS*/
	0x005f66cb, /*COLOR*/
	0x000112ac, /*COS*/
	0x0003a041, /*COSH*/
	0x00616415, /*CREAD*/
	0x0de593fb, /*CURSOR*/
	0x00040fbe, /*DATA*/
	0x00011644, /*DEC*/
	0x0fe19c42, /*DELETE*/
	0x000116de, /*DIM*/
	0x000100a8, /*DO*/
	0x0004fd8e, /*ELSE*/
	0x1434a177, /*ELSEIF*/
	0x00011c99, /*END*/
	0x0091c927, /*ENDIF*/
	0x00053854, /*EXEC*/
	0x00011e0d, /*EXP*/
	0x000579c8, /*FABS*/
	0x16e3d4be, /*FCLOSE*/
	0x00058fcf, /*FEOF*/
	0x00059895, /*FGET*/
	0x00a67500, /*FGETC*/
	0x00a7e061, /*FIELD*/
	0x0005a3a2, /*FILE*/
	0x177f0ca5, /*FINPUT*/
	0x0005b1df, /*FLEN*/
	0x00aa3445, /*FLOAT*/
	0x00aa363b, /*FLOOR*/
	0x0005b84d, /*FMOD*/
	0x00ac5c9f, /*FOPEN*/
	0x000121db, /*FOR*/
	0x18352839, /*FPRINT*/
	0x0005c865, /*FPUT*/
	0x00ad2e40, /*FPUTC*/
	0x00aefdec, /*FSEEK*/
	0x00063b90, /*GCLS*/
	0x1a808bcb, /*GCOLOR*/
	0x1ab733b3, /*GETDIR*/
	0x00c60f03, /*GOSUB*/
	0x0006796c, /*GOTO*/
	0x1bcfcc39, /*GPRINT*/
	0x00012a99, /*HEX*/
	0x00010153, /*IF*/
	0x00f8701a, /*INKEY*/
	0x00f88ba5, /*INPUT*/
	0x000130e9, /*INT*/
	0x00092084, /*KEYS*/
	0x013be43d, /*LABEL*/
	0x00013ecf, /*LEN*/
	0x00013ed5, /*LET*/
	0x0009e96a, /*LINE*/
	0x00014030, /*LOG*/
	0x0145f324, /*LOG10*/
	0x000a07f9, /*LOOP*/
	0x000abca3, /*MODF*/
	0x016418d4, /*MUSIC*/
	0x000b4321, /*NEXT*/
	0x000148f8, /*NEW*/
	0x00014a5d, /*NOT*/
	0x38a658d7, /*OPTION*/
	0x000152c0, /*PCG*/
	0x000cacec, /*PEEK*/
	0x3b1c6aea, /*PEEK16*/
	0x3b1c6b2e, /*PEEK32*/
	0x00010252, /*PI*/
	0x01ac8479, /*POINT*/
	0x000ce05e, /*POKE*/
	0x3c20dc0a, /*POKE16*/
	0x3c20dc4e, /*POKE32*/
	0x00015480, /*POW*/
	0x01aea739, /*PRINT*/
	0x000cf3d5, /*PSET*/
	0x3cb45fa4, /*PUBLIC*/
	0x3cc0fe21, /*PUTBMP*/
	0x000e18d5, /*READ*/
	0x00015d2e, /*REM*/
	0x425c9703, /*RETURN*/
	0x00015e69, /*RND*/
	0x45c26d49, /*SCROLL*/
	0x45f6e3b3, /*SETDIR*/
	0x00016287, /*SGN*/
	0x000162cf, /*SIN*/
	0x000ee52d, /*SINH*/
	0x01f9a429, /*SOUND*/
	0x000f0e49, /*SQRT*/
	0x47f711de, /*SYSTEM*/
	0x000166bf, /*TAN*/
	0x000f72ed, /*TANH*/
	0x02182fee, /*TVRAM*/
	0x022c2a2d, /*UNTIL*/
	0x4e8887d0, /*USEPCG*/
	0x4e88a5f3, /*USEVAR*/
	0x000170dd, /*VAL*/
	0x000170e3, /*VAR*/
	0x00119505, /*WAIT*/
	0x0011a9e9, /*WEND*/
	0x025aef62, /*WHILE*/
	0x025b8d75, /*WIDTH*/
=======
	0x0001070c, /*ABS*/
	0x0002099d, /*ACOS*/
	0x000258ac, /*ARGS*/
	0x00010971, /*ASC*/
	0x00025e4a, /*ASIN*/
	0x0002627b, /*ATAN*/
	0x00343bc9, /*ATAN2*/
	0x004f5c95, /*BREAK*/
	0x00038a2f, /*CALL*/
	0x006110c1, /*CDATA*/
	0x00039f24, /*CEIL*/
	0x0001129b, /*CHR*/
	0x0e7f3303, /*CIRCLE*/
	0x0067525f, /*CLEAR*/
	0x00011330, /*CLS*/
	0x0069cb6b, /*COLOR*/
	0x0001139f, /*COS*/
	0x0003d60c, /*COSH*/
	0x006bf57f, /*CREAD*/
	0x0fd6b45b, /*CURSOR*/
	0x00045129, /*DATA*/
	0x00011776, /*DEC*/
	0x122a52c0, /*DELETE*/
	0x00011814, /*DIM*/
	0x000100ac, /*DO*/
	0x000551b8, /*ELSE*/
	0x171a03a1, /*ELSEIF*/
	0x00011e1d, /*END*/
	0x00a20bbe, /*ENDIF*/
	0x00058fdc, /*EXEC*/
	0x00011f9b, /*EXP*/
	0x0005da5b, /*FABS*/
	0x1a358bdd, /*FCLOSE*/
	0x0005f193, /*FEOF*/
	0x0005fae1, /*FGET*/
	0x00b94291, /*FGETC*/
	0x00bacd24, /*FIELD*/
	0x00060687, /*FILE*/
	0x1ae2b2ef, /*FINPUT*/
	0x00061598, /*FLEN*/
	0x00bd52ae, /*FLOAT*/
	0x00bd54b2, /*FLOOR*/
	0x00061c59, /*FMOD*/
	0x00bfaa2c, /*FOPEN*/
	0x000123a9, /*FOR*/
	0x1badd288, /*FPRINT*/
	0x00062d52, /*FPUT*/
	0x00c08ce6, /*FPUTC*/
	0x00c286ca, /*FSEEK*/
	0x0006ac5c, /*GCLS*/
	0x1e59e7b7, /*GCOLOR*/
	0x1e96cb44, /*GETDIR*/
	0x00dc556c, /*GOSUB*/
	0x0006edac, /*GOTO*/
	0x1fcfed5d, /*GPRINT*/
	0x00012cef, /*HEX*/
	0x00083d61, /*IDLE*/
	0x0001015c, /*IF*/
	0x01149470, /*INKEY*/
	0x0114b178, /*INPUT*/
	0x00013391, /*INT*/
	0x0009d063, /*KEYS*/
	0x016022dc, /*LABEL*/
	0x00014249, /*LEN*/
	0x0001424f, /*LET*/
	0x000aa9ff, /*LINE*/
	0x000143b4, /*LOG*/
	0x016b0db9, /*LOG10*/
	0x000aca45, /*LOOP*/
	0x000b8e81, /*MODF*/
	0x018c8c85, /*MUSIC*/
	0x000c21d6, /*NEXT*/
	0x00014d04, /*NEW*/
	0x00014e73, /*NOT*/
	0x40e24fde, /*OPTION*/
	0x0001575c, /*PCG*/
	0x000daac8, /*PEEK*/
	0x43be47b3, /*PEEK16*/
	0x43be47f9, /*PEEK32*/
	0x00010262, /*PI*/
	0x01dd7f7a, /*POINT*/
	0x000de11a, /*POKE*/
	0x44e0c435, /*POKE16*/
	0x44e0c47b, /*POKE32*/
	0x00015928, /*POW*/
	0x01dfd111, /*PRINT*/
	0x000df5af, /*PSET*/
	0x45858d00, /*PUBLIC*/
	0x459341b1, /*PUTBMP*/
	0x000f35e7, /*READ*/
	0x0001625e, /*REM*/
	0x4c0e4e9d, /*RETURN*/
	0x000163a2, /*RND*/
	0x4ff58ae0, /*SCROLL*/
	0x50300d40, /*SETDIR*/
	0x00016802, /*SGN*/
	0x0001684c, /*SIN*/
	0x0010130d, /*SINH*/
	0x02338a69, /*SOUND*/
	0x00103e75, /*SQRT*/
	0x526b8f2e, /*SYSTEM*/
	0x00016c7d, /*TAN*/
	0x0010ae22, /*TANH*/
	0x024b5425, /*TIMER*/
	0x02557a82, /*TVRAM*/
	0x026bf064, /*UNTIL*/
	0x59f94768, /*USEPCG*/
	0x59f9673f, /*USEVAR*/
	0x0001772d, /*VAL*/
	0x00017733, /*VAR*/
	0x0012ff0c, /*WAIT*/
	0x00131519, /*WEND*/
	0x02a044ad, /*WHILE*/
	0x02a0f0f8, /*WIDTH*/
>>>>>>> remotes/origin/timer
	// Additional names follow
	ADDITIONAL_RESERVED_VAR_NAMES
};

/*
	check_var_name();
	This function reads the current position of source code and check if
	it contains valid var name, the function returns 0 or plus value.
	If not, it returns -1;
*/

int check_var_name(){
	char b1;
	int j;
	int i=0;
	int prevpos=g_srcpos;
	next_position();
	b1=g_source[g_srcpos];
	// When changing here, see also get_label()
	if ((b1<'A' || 'Z'<b1) && b1!='_') return -1;
	do {
		// First character must be A-Z or _
		// From second, A-Z, _, and 0-9 can be used.
		i*=37;
		if (b1=='_') b1='Z'+1;
		if ('0'<=b1 && b1<='9') {
			i+=b1-'0';
		} else if (g_srcpos==prevpos) {
			// First character must be A-Z or _.
			// Subtract 9, resulting 1-27 but not 10-36.
			// This subtraction is required to maintain
			// final number being <0x80000000.
			i+=b1-'A'+1;
		} else {
			i+=b1-'A'+10;
		}
		g_srcpos++;
		b1=g_source[g_srcpos];
	} while ('0'<= b1 && b1<='9' || 'A'<=b1 && b1<='Z' || b1=='_');
	// Length of the label must be between 2 and 6.
	if (g_srcpos-prevpos<2) {
		// One letter var name, A-Z
		return i-1;
	}
	if (6<g_srcpos-prevpos) {
		// Too long. This is not var name.
		g_srcpos=prevpos;
		return -1;
	}
	i+=65536;
	// Check if this is reserved var name.
	for(j=0;j<sizeof reserved_var_names/sizeof reserved_var_names[0];j++){
		if (reserved_var_names[j]==i) {
			// This var name is reserved as used for function or statement.
			g_srcpos=prevpos;
			return -1;
		}
	}
	// Reserved var names table was checked. This must be a long var name.
	return i;
}

/*
	int get_var_number();
	This function returns variable number that can be used as the index of $s8
*/

int get_var_number(){
	int i,j,spos;
	int* record;
	// This must be a short or long var name.
	spos=g_srcpos;
	i=check_var_name();
	if (i<0) return -1;
	// If it is a short name, immediately return.
	if (i<26) return i;
	// Check if CLASS::STATIC
	if (g_source[g_srcpos]==':' && g_source[g_srcpos+1]==':') {
		// This is CLASS::STATIC
		g_srcpos++;
		g_srcpos++;
		j=check_var_name();
		if (j<26) return -1;
		cmpdata_reset();
		while(record=cmpdata_find(CMPDATA_STATIC)){
			if (record[1]!=i) continue;
			if (record[2]!=j) continue;
			// Found CLASS::STATIC
			i=record[0]&0x0000FFFF;
			j=0;
			break;
		}	
		if (j) {
			// Not found. Maybe a static method
			g_srcpos=spos;
			return -1;
		}
	} else {
		// Search long var names registered by USEVAR statement.
		// If found, returns the value that can be used as the index of $s8
		i=search_var_name(i);
		if (i<0) return -1;
	}
	// This var name is defined by USEVAR statement.
	return i+ALLOC_LNV_BLOCK;
	
}

/*
	int search_var_name(int nameint);
	This function searchs registered long var name in compile data table.
	If not found, this function returns -1.
	If found, it retunrs var number, beginning 0.
*/

int search_var_name(int nameint){
	int* cmpdata;
	cmpdata_reset();
	while(cmpdata=cmpdata_find(CMPDATA_USEVAR)){
		if (cmpdata[1]==nameint) return cmpdata[0]&0x0000ffff;
	}
	return -1;
}

/*
	char* register_var_name(int nameint);
	This function is called when compiler detects "USEVAR" statement.
	It registers the long file name in compile data table.
*/

char* register_var_name(int nameint){
	// Check if registered before. If did, cause error.
	if (search_var_name(nameint)!=-1) return ERR_INVALID_VAR_NAME;
	// Number of long var name is restricted
	if (ALLOC_LNV_NUM<=g_long_name_var_num) return ERR_INVALID_VAR_NAME;
	// Register var name as a compile data
	g_temp=nameint;
	return cmpdata_insert(CMPDATA_USEVAR,g_long_name_var_num++,&g_temp,1);
}
