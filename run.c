/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka & Katsumi
   http://www.ze.em-net.ne.jp/~kenken/index.html
   http://hp.vector.co.jp/authors/VA016157/
*/

/*
	This file is shared by Megalopa and Zoea
*/

#include <xc.h>
#include "api.h"
#include "compiler.h"
#include "editor.h"
#include "main.h"

char* printdec(int num){
	char str[11];
	int i;
	if (num<0) {
		printchar('-');
		num=0-num;
	}
	for(i=10;0<i;i--){
		if (num==0 && i<10) break;
		str[i]='0'+rem10_32(num);
		num=div10_32(num);
	}
	for(i++;i<11;i++) {
		printchar(str[i]);
	}
}

int runbasic(char *appname,int test){
// BASIC�\�[�X�̃R���p�C���Ǝ��s
// appname ���s����BASIC�\�[�X�t�@�C��
// test 0:�R���p�C���Ǝ��s�A0�ȊO:�R���p�C���݂̂ŏI��
//
// �߂�l
//�@�@0:����I��
//�@�@-1:�t�@�C���G���[
//�@�@-2:�����N�G���[
//�@�@1�ȏ�:�R���p�C���G���[�̔����s�i�s�ԍ��ł͂Ȃ��t�@�C����̉��s�ڂ��j
	int i;
	char* buff;
	char* err;

	// Set grobal pointer
	g_gp=get_gp();
	// Set buffer positions
	buff=(char*)&(RAM[RAMSIZE-512]);
	// Set object positions
	g_object=(int*)(&RAM[0]);
	g_objpos=0;
	g_objmax=g_object+(RAMSIZE-512)/4; // Buffer area excluded.
	// Clear object area
	for(i=0;i<RAMSIZE/4;i++) g_object[i]=0x00000000;

	// Check file error
	err=init_file(buff,appname);
	if (err) {
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Can't Open ");
		printstr(appname);
		printchar('\n');
		return -1;
	}
	close_file();	

	// Initialize parameters
	g_pcg_font=0;
	g_use_graphic=0;
	g_graphic_area=0;
	clearscreen();
	setcursor(0,0,7);
	g_long_name_var_num=0;
	cmpdata_init();

	// Initialize music system
	init_music();

	printstr("BASIC "BASVER"\n");
	wait60thsec(15);

	printstr("Compiling...");

	// Compile the file
	i=compile_and_link_main_file(buff,appname);
	if (i) return i;

	// All done
	printstr("done\n");
	if(test) return 0; //�R���p�C���݂̂̏ꍇ
	wait60thsec(15);

	// Initialize the other parameters
	// Random seed
	g_rnd_seed=0x92D68CA2; //2463534242
	// Clear variables
	for(i=0;i<ALLOC_BLOCK_NUM;i++){
		g_var_mem[i]=0;
		g_var_size[i]=0;
	}
	// Clear key input buffer
	for(i=0;i<256;i++){
		ps2keystatus[i]=0;
	}
	// Reset data/read.
	reset_dataread();
	// Initialize file system
	lib_file(FUNC_FINIT,0,0,0);

	// Assign memory
	set_free_area((void*)(g_object+g_objpos),(void*)(&RAM[RAMSIZE]));

	// Warm up environment
	pre_run();

	// Execute program
	// Start program from the beginning of RAM.
	// Work area (used for A-Z values) is next to the object code area.
	start_program((void*)(&(RAM[0])),(void*)(&g_var_mem[0]));
	printstr("\nOK\n");

	// Cool down environment
	post_run();
	lib_file(FUNC_FINIT,0,0,0);

	return 0;
}
