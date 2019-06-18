/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka
   http://www.ze.em-net.ne.jp/~kenken/index.html
*/

/*
	This file is shared by Megalopa and Zoea
*/

#include <xc.h>
#include "api.h"
#include "editor.h"
#include "keyinput.h"
#include "compiler.h"
#include "main.h"

struct _TBUF{
//�����N�t���̃e�L�X�g�o�b�t�@
	struct _TBUF *prev;//�O���ւ̃����N�BNULL�̏ꍇ�擪�܂��͋�
	struct _TBUF *next;//����ւ̃����N�BNULL�̏ꍇ�Ō�
	unsigned short n;//���݂̎g�p�o�C�g��
	unsigned char Buf[TBUFSIZE];//�o�b�t�@
} ;
typedef struct _TBUF _tbuf;

//_tbuf TextBuffer[TBUFMAXLINE]; //�e�L�X�g�o�b�t�@
_tbuf *TextBuffer; //���͔̂z��RAM[]�̒��Ɋm�ۂ���

_tbuf *TBufstart; //�e�L�X�g�o�b�t�@�̐擪�ʒu
_tbuf *cursorbp; //���݂̃J�[�\���ʒu�̃e�L�X�g�o�b�t�@
unsigned short cursorix; //���݂̃J�[�\���ʒu�̃e�L�X�g�o�b�t�@�擪����̈ʒu
_tbuf *disptopbp; //���ݕ\������ʍ���̃e�L�X�g�o�b�t�@
unsigned short disptopix; //���ݕ\������ʍ���̃e�L�X�g�o�b�t�@�擪����̈ʒu
int num; //���݃o�b�t�@���Ɋi�[����Ă��镶����
int cx,cy; //�J�[�\�����W
int cx2; //�㉺�ړ����̉��J�[�\��X���W
_tbuf *cursorbp1; //�͈͑I�����̃J�[�\���X�^�[�g�ʒu�̃e�L�X�g�o�b�t�@�A�͈͑I�����[�h�łȂ��ꍇNULL
unsigned short cursorix1; //�͈͑I�����̃J�[�\���X�^�[�g�ʒu�̃e�L�X�g�o�b�t�@�擪����̈ʒu
int cx1,cy1; //�͈͑I�����̃J�[�\���X�^�[�g���W
int line_no; //���݂̃J�[�\���ʒu�̍s
int line_no1; //�͈͑I�����̃J�[�\���X�^�[�g�ʒu�̍s

// �J�[�\���֘A�ʒu�̈ꎞ���p
_tbuf *cursorbp_t;
unsigned short cursorix_t;
_tbuf *disptopbp_t;
unsigned short disptopix_t;
int cx_t,cy_t,line_no_t;

//unsigned char clipboard[WIDTH_X2*EDITWIDTHY]; //�N���b�v�{�[�h�A�ő�T�C�Y�͕ҏW��ʗ̈�Ɠ���
unsigned char *clipboard; //���͔̂z��RAM[]�̒��Ɋm�ۂ���

int clipsize; //���݃N���b�v�{�[�h�Ɋi�[����Ă��镶����
int edited; //�ۑ���ɕύX���ꂽ����\���t���O

//�z��RAM[]���Ƀ��������I�m�ۂ��邽�߂̃|�C���^
char *editormallocp;

//unsigned char filebuf[FILEBUFSIZE]; //�t�@�C���A�N�Z�X�p�o�b�t�@
unsigned char *filebuf; //���͔̂z��RAM[]�̒��Ɋm�ۂ���

//unsigned char cwdpath[PATHNAMEMAX]; //���݂̃f�B���N�g���̃p�X��
unsigned char *cwdpath; //���͔̂z��RAM[]�̒��Ɋm�ۂ���

unsigned char currentfile[13],tempfile[13]; //�ҏW���̃t�@�C�����A�ꎞ�t�@�C����

//unsigned char filenames[MAXFILENUM][13]; //���[�h���̃t�@�C�����ꗗ�o�b�t�@
unsigned char (*filenames)[13]; //���͔̂z��RAM[]�̒��Ɋm�ۂ���

//unsigned char undobuf[UNDOBUFSIZE]; //�A���h�D�p�o�b�t�@
unsigned char *undobuf; //���͔̂z��RAM[]�̒��Ɋm�ۂ���
unsigned char *undobuf_top; //�A���h�D�p�o�b�t�@�̐擪���w���|�C���^
int undobuf_used; //�A���h�D�p�o�b�t�@�g�p��

const unsigned char Message1[]="Hit Any Key\n";
const unsigned char Message2[]="File System Error\n";
const unsigned char Message3[]="Retry:[Enter] / Quit:[ESC]\n";
const unsigned char ROOTDIR[]="\\";

unsigned char * editormalloc(int size){
//�z��RAM[]���ɃT�C�Ysize�̗̈���m�ۂ��A�擪�A�h���X��Ԃ�
//�m�ۂł��Ȃ��ꍇ�́A�G���[�\���������~
	unsigned char *p;
	if(editormallocp+size>RAM+RAMSIZE){
		printstr("Cannot allocate memory");
		while(1) asm("wait");
	}
	p=editormallocp;
	editormallocp+=size;
	return p;
}

void wait60thsec(unsigned short n){
	// 60����n�b�E�F�C�g�i�r�f�I��ʂ̍ŉ��s�M���o�͏I���܂ő҂j
	n+=drawcount;
	while(drawcount!=n) asm(WAIT);
}

unsigned int bpixtopos(_tbuf *bp,unsigned int ix){
// �e�L�X�g�o�b�t�@��̈ʒu����e�L�X�g�S�̂̐擪���牽�����ڂ���Ԃ�
// bp:�e�L�X�g�o�b�t�@�|�C���^
// ix:bp->Buf�̐擪����̕�����
	unsigned int pos;
	_tbuf *sbp;
	pos=0;
	sbp=TBufstart;
	while(sbp!=bp){
		pos+=sbp->n;
		sbp=sbp->next;
		if(sbp==NULL) return 0; //�G���[
	}
	return pos+ix;
}
_tbuf * postobpix(int pos,unsigned short *pix){
// �e�L�X�g�S�̂̐擪����pos�����ڂ̃e�L�X�g�o�b�t�@��̈ʒu��Ԃ�
// �߂�l�@�e�L�X�g�o�b�t�@�|�C���^
// *pix�i�߂�l�j�F�߂�l�e�L�X�g�o�b�t�@�̐擪����̈ʒu�i�|�C���^�n���j
	_tbuf *bp;
	bp=TBufstart;
	while(pos >= bp->n){
		if(bp->next==NULL) break; //�S�̍Ō���̏ꍇ
		pos-=bp->n;
		bp=bp->next;
	}
	if(pos > bp->n){
		// �I�[�o�[�����G���[�̏ꍇ�擪��Ԃ�
		*pix=0;
		return TBufstart;
	}
	*pix=pos;
	return bp;
}
_tbuf * linetobpix(int line,unsigned short *pix){
// �e�L�X�g�S�̂̐擪����line�s�ڂ̃e�L�X�g�o�b�t�@��̈ʒu��Ԃ�
// �߂�l�@�e�L�X�g�o�b�t�@�|�C���^
// *pix�i�߂�l�j�F�߂�l�e�L�X�g�o�b�t�@�̐擪����̈ʒu�i�|�C���^�n���j
	_tbuf *bp,*bp2;
	int ix,ix2;
	bp=TBufstart;
	bp2=TBufstart;
	ix=0;
	ix2=0;
	while(line>1){
		while(1){
			if(ix>=bp->n){
				if(bp->next==NULL) break;
				bp=bp->next;
				ix=0;
				continue;
			}
			if(bp->Buf[ix++] == '\n'){
				bp2=bp;
				ix2=ix;
				break;
			}
		}
		line--;
	}
	*pix=ix2;
	return bp2;
}

_tbuf * newTBuf(_tbuf *prev){
// �V�����e�L�X�g�o�b�t�@1�s�𐶐�
// prev:�}����̍s�iprev�̌��ɒǉ��j
// �߂�l�@���������o�b�t�@�ւ̃|�C���^�A�����ł��Ȃ��ꍇNULL
	_tbuf *bp,*next;

	//�o�b�t�@�̐擪����󂫂��T�[�`
	bp=TextBuffer;
	while(1){
		if(bp->prev==NULL && bp!=TBufstart) break;
		bp++;
		if(bp>=TextBuffer+TBUFMAXLINE) return NULL;//�Ō�܂ŋ󂫂Ȃ�
	}
	next=prev->next;
	//�s�}��
	bp->prev=prev;
	bp->next=next;
	prev->next=bp;
	if(next!=NULL) next->prev=bp;
	bp->n=0;
	return bp;
}

_tbuf * deleteTBuf(_tbuf *bp){
// �e�L�X�g�o�b�t�@�̍폜
// bp:�폜����s�̃|�C���^
// �߂�l�@�폜�O�̎��̃o�b�t�@�ւ̃|�C���^�A�Ȃ��ꍇNULL
	unsigned short a,b;
	_tbuf *prev,*next;
	prev=bp->prev;
	next=bp->next;
	if(prev==NULL){
		//�擪�s�̏ꍇ
		if(next==NULL) return next; //�Ō��1�s�̏ꍇ�͍폜���Ȃ�
		TBufstart=next; //���̍s��擪�s�ݒ�
	}
	else prev->next=next; //�O�����Ƀ����N�i�ŏI�s�Ȃ�NULL���R�s�[�����j
	if(next!=NULL) next->prev=prev; //��������Ύ���O�Ƀ����N
	bp->prev=NULL; //�󂫃t���O�ݒ�
	return next;
}

// �A���h�D�o�b�t�@
/*
UNDOBUFSIZE�o�C�g�̊�o�b�t�@�B�e�L�X�g�o�b�t�@�ɑ΂���ύX�������ƂɁA
�ύX���e�A�ύX�ꏊ���o�b�t�@�̐擪�ɋL�^���A�擪�ʒu��i�߂�B
�A���h�D���s���Ăяo�����ƁA�o�b�t�@�擪����ǂݏo���A�e�L�X�g�o�b�t�@�ɑ΂���
���ɖ߂��ύX���s���B
�o�b�t�@�������ς��ɂȂ�ƁA�Ō����������i�㏑���j���Ă����B

���o�b�t�@�d�l��
�@�J�n�ʒu�F�e�L�X�g�o�b�t�@�g�b�v���牽�o�C�g�ڂ��i2�o�C�g�B���ʁA��ʂ̏��j
�@�J��Ԃ����F�A������̏ꍇ�̉񐔁i2�o�C�g�B���ʁA��ʂ̏��j
�@�o�b�t�@�̑O�������납����폜�ł���悤�A�擪�ƍŌ�ɖ��߂������B�������A
�@���̖��߃R�[�h��10�ȏ�̏ꍇ�͍폜���ꂽ�������̂��̂��Ӗ�����B
1�����}��
�@UNDO_INSERT,�J�n�ʒu,UNDO_INSERT
1�����㏑��
�@UNDO_OVERWRITE,�J�n�ʒu,�������� [,����������0?9�̏ꍇ�����0��t��]
1�����폜�iDelete�j
�@UNDO_DELETE,�J�n�ʒu,�������� [,����������0?9�̏ꍇ�����0��t��]
1�����폜�iBackSpace�j
�@UNDO_BACKSPACE,�J�n�ʒu,�������� [,����������0?9�̏ꍇ�����0��t��]
�A���}���iCtrl+V�œ\��t���j
�@UNDO_CONTINS,�J�n�ʒu,�J��Ԃ���,UNDO_CONTINS
�A���폜�i�̈�I�����č폜�j
�@UNDO_CONTDEL,�J��Ԃ���,����������,�J�n�ʒu,�J��Ԃ���,UNDO_CONTDEL
*/

void pushundomem(unsigned char c){
// �A���h�D�p�������̐擪��1�o�C�g�𒙂߂�
// �󂫂��Ȃ��Ȃ����ꍇ�A�Ō����1���ߕ��𖳌���
	unsigned char *p;
	int n;

	if(undobuf_used>=UNDOBUFSIZE){
	//�󂫂��Ȃ��ꍇ�A�Ō���̃u���b�N�̃o�C�g�������o�b�t�@���p�ςݗʂ��猸�炷
		p=undobuf_top-undobuf_used; //�Ō��
		if(p<undobuf) p+=UNDOBUFSIZE;
		switch(*p){
			case UNDO_INSERT: //1�����}��
				undobuf_used-=4;
				break;
			case UNDO_OVERWRITE: //1�����㏑��
			case UNDO_DELETE: //1�����폜
			case UNDO_BACKSPACE: //1�����폜�iBS�j
				undobuf_used-=4;
				p+=3;
				if(p>=undobuf+UNDOBUFSIZE) p-=UNDOBUFSIZE;
				if(*p<10) undobuf_used--; //�R�[�h0?9�̏ꍇ����0���t������Ă���
				break;
			case UNDO_CONTINS: //�A���}��
				undobuf_used-=6;
				break;
			case UNDO_CONTDEL: //�A���폜
				//�J��Ԃ����̓ǂݏo��
				p++;
				if(p>=undobuf+UNDOBUFSIZE) p-=UNDOBUFSIZE;
				n=*p++;
				if(p>=undobuf+UNDOBUFSIZE) p-=UNDOBUFSIZE;
				n+=*p<<8;
				undobuf_used-=n+8;
				break;
		}
	}
	//�A���h�D�o�b�t�@�擪��1�o�C�g�}�����A�擪�ʒu��1�i�߂�
	*undobuf_top++=c;
	if(undobuf_top>=undobuf+UNDOBUFSIZE) undobuf_top-=UNDOBUFSIZE;
	undobuf_used++;
}
void pushundomem2(unsigned short w){
// �A���h�D�o�b�t�@��2�o�C�g���߂�A���ʁA��ʂ̏�
	pushundomem((unsigned char)w);
	pushundomem(w>>8);
}
unsigned char popundomem(){
// �A���h�D�o�b�t�@����1�o�C�g�ǂݏo���A�擪��1�߂�
// �߂�l�F�ǂݏo�����R�[�h
	undobuf_top--;
	if(undobuf_top<undobuf) undobuf_top+=UNDOBUFSIZE;
	undobuf_used--;
	return *undobuf_top;
}
unsigned short popundomem2(){
// �A���h�D�o�b�t�@����2�o�C�g�ǂݏo��
// �߂�l�F�ǂݏo����2�o�C�g�R�[�h
	unsigned short w;
	w=popundomem()<<8;
	w+=popundomem();
	return w;
}
void setundobuf(int com,_tbuf *bp,unsigned short ix,unsigned char c,unsigned short n){
//�A���h�D�o�b�t�@�Ƀf�[�^���Z�b�g����
//com:�R�}���h�@1:1�����폜�A2:1�����㏑���A3:1�����}���A4:�A���폜�A5:�A���}���J�n
//bp,ix:�o�b�t�@��̎��s�ꏊ�i�J�[�\���ʒu�j
//c:�����i�㏑���A�}���̏ꍇ�̂ݎg�p�j
//n:�A�����i�A���̏ꍇ�̂ݎg�p�j
	unsigned short pos;

	pos=bpixtopos(bp,ix); //�e�L�X�g�o�b�t�@�擪���牽�o�C�g�ڂ������߂�
	switch(com){
		case UNDO_INSERT: //1�����}��
			pushundomem(com);
			pushundomem2(pos);
			pushundomem(com);
			break;
		case UNDO_OVERWRITE: //1�����㏑��
		case UNDO_DELETE: //1�����폜�iDelete�j
		case UNDO_BACKSPACE: //1�����폜�iBackSpace�j
			pushundomem(com);
			pushundomem2(pos);
			pushundomem(c);
			if(c<10) pushundomem(0); //10�����̃R�[�h�̏ꍇ0��t��
			break;
		case UNDO_CONTINS: //�A���}��
			pushundomem(com);
			pushundomem2(pos);
			pushundomem2(n);
			pushundomem(com);
			break;
		case UNDO_CONTDEL: //�A���폜
			pushundomem(com);
			pushundomem2(n);
			break;
	}
}

int insertchar(_tbuf *bp,unsigned int ix,unsigned char c,int undo){
//�e�L�X�g�o�b�t�@bp�̐擪����ix�o�C�g�̈ʒu��c��}��
//undo 0:�ʏ�i�A���h�D�o�b�t�@�Ɋi�[����j�A1:�A���}�����A2:�A���h�D��
//�߂�l�@�����F0�A�s���܂��͗e�ʃI�[�o�[�F-1�A�󂫂�����͂��Ȃ̂Ɏ��s�F1
	unsigned char *p;

	if(ix > bp->n) return -1; //�s���w��
	if(num >= TBUFMAXSIZE) return -1; //�o�b�t�@�e�ʃI�[�o�[
	if(bp->n < TBUFSIZE){
		//���C����������1�o�C�g�}���\//
		for(p=bp->Buf + bp->n ; p > bp->Buf+ix ; p--) *p=*(p-1);
		*p=c;
		if(!undo) setundobuf(UNDO_INSERT,bp,ix,0,0); //�A���h�D�o�b�t�@�ݒ�
		bp->n++;
		num++; //�o�b�t�@�g�p��
//		if(bp->n >= TBUFSIZE && bp->next==NULL) newTBuf(bp); //�o�b�t�@�������ς��ɂȂ�����V���Ƀo�b�t�@����
		return 0;
	}
	//���C�������ӂ��ꍇ
	if(bp->next==NULL || bp->next->n >=TBUFSIZE){
		// �ŏI�s�܂��͎��̃��C���o�b�t�@�������ς����������s�}��
		if(newTBuf(bp)==NULL){
			// ���C���o�b�t�@�}���s��
			return 1;
		}
	}
	if(ix==TBUFSIZE){
		insertchar(bp->next,0,c,undo);
		return 0;
	}
	p=bp->Buf + TBUFSIZE-1;
	insertchar(bp->next,0,*p,1); //���̍s�̐擪��1�����}���i�K���󂫂���j
	for( ; p > bp->Buf+ix ; p--) *p=*(p-1);
	*p=c;
	if(!undo) setundobuf(UNDO_INSERT,bp,ix,0,0); //�A���h�D�o�b�t�@�ݒ�
	return 0;
}

int overwritechar(_tbuf *bp,unsigned int ix,unsigned char c,int undo){
//�e�L�X�g�o�b�t�@bp�̐擪����ix�o�C�g�̈ʒu��c�ŏ㏑��
//undo 0:�ʏ�i�A���h�D�o�b�t�@�Ɋi�[����j�A1:�A�����A2:�A���h�D��
//�߂�l�@�����F0�A�s���܂��͗e�ʃI�[�o�[�F-1�A�󂫂�����͂��Ȃ̂Ɏ��s�F1

	//���݂̃o�b�t�@�ʒu�̕������I�[�܂��͉��s�̏ꍇ�A�}�����[�h
	if(ix > bp->n) return -1; //�s���w��
	while(ix >= bp->n){
		if(bp->next==NULL){
			//�e�L�X�g�S�̍Ō���̏ꍇ�͑}��
			return insertchar(bp,ix,c,undo);
		}
		bp=bp->next;
		ix=0;
	}
	if(bp->Buf[ix]=='\n') return insertchar(bp,ix,c,undo);
	if(!undo) setundobuf(UNDO_OVERWRITE,bp,ix,bp->Buf[ix],0); //�A���h�D�o�b�t�@�ݒ�
	bp->Buf[ix]=c;
	return 0;
}

void deletechar(_tbuf *bp,unsigned int ix,int undo){
//�e�L�X�g�o�b�t�@bp�̐擪����ix�o�C�g�̈ʒu��1�o�C�g�폜
//undo -1:�ʏ�BackSpace�i�A���h�D�o�b�t�@�Ɋi�[����j
//      0:�ʏ�DELETE�i�A���h�D�o�b�t�@�Ɋi�[����j�A1:�A�����A2:�A���h�D��
	unsigned char *p;

	if(ix > bp->n) return; //�s���w��
	if(ix !=bp->n){
		//�o�b�t�@�̍Ō�̕��������łȂ��ꍇ

		//�A���h�D�o�b�t�@�ݒ�
		if(undo==1) pushundomem(bp->Buf[ix]); //�A���폜��
		else if(undo==-1) setundobuf(UNDO_BACKSPACE,bp,ix,bp->Buf[ix],0); //1�����폜(backspace)
		else if(undo==0) setundobuf(UNDO_DELETE,bp,ix,bp->Buf[ix],0); //1�����폜

		for(p=bp->Buf+ix ; p< bp->Buf + bp->n-1 ; p++) *p=*(p+1);
		bp->n--;
		num--; //�o�b�t�@�g�p��
		return;
	}
	//�s�o�b�t�@�̌��݂̍Ō�̏ꍇ�i�폜���镶�����Ȃ��ꍇ�j
	if(bp->next==NULL) return; //�S�̂̍Ō�̏ꍇ�A�������Ȃ�
	deletechar(bp->next,0,undo); //���̍s�̐擪�������폜
}
int gabagecollect1(void){
//�f�Љ����ꂽ�e�L�X�g�o�b�t�@�̌��Ԃ𖄂߂�K�x�[�W�R���N�V����
//�J�[�\���̑O�ƌ�낻�ꂼ��T�����čŏ���1�o�C�g���̂ݎ��{
//�߂�l 1�o�C�g�ł��ړ������ꍇ�F1�A�Ȃ������ꍇ�F0

	_tbuf *bp;
	int f=0;
	unsigned char *p,*p2;

	//�J�[�\�����o�b�t�@�̐擪�ɂ���ꍇ�A�O�̃o�b�t�@�̍Ō���ɕύX
	//�i�������O�ɋ󂫂��Ȃ��ꍇ�Ɛ擪�o�b�t�@�̏ꍇ�������j
	while(cursorix==0 && cursorbp->prev!=NULL && cursorbp->prev->n <TBUFSIZE){
		cursorbp=cursorbp->prev;
		cursorix=cursorbp->n;
	}
	//��ʍ���ʒu���o�b�t�@�̐擪�ɂ���ꍇ�A�O�̃o�b�t�@�̍Ō���ɕύX
	//�i�������擪�o�b�t�@�̏ꍇ�������j
	while(disptopix==0 && disptopbp->prev!=NULL){
		disptopbp=disptopbp->prev;
		disptopix=disptopbp->n;
	}
	//�J�[�\���̂���o�b�t�@�ȊO�̋�o�b�t�@��S�č폜
	bp=TBufstart;
	while(bp!=NULL){
		if(bp->n == 0 && bp!=cursorbp){
			if(bp==disptopbp) disptopbp=bp->next; //��ʍ���ʒu����o�b�t�@�擪�̏ꍇ�A���ɂ��炷
			bp=deleteTBuf(bp); //�󂫃o�b�t�@�폜
		}
		else bp=bp->next;
	}

	//�J�[�\���ʒu���O�̖��܂��Ă��Ȃ��o�b�t�@��擪����T�[�`
	bp=TBufstart;
	while(bp->n >= TBUFSIZE){
		if(bp==cursorbp) break;
		bp=bp->next;
	}
	if(bp!=cursorbp){
		//�ŏ��Ɍ������󂫏ꏊ�Ɏ��̃o�b�t�@����1�o�C�g�ړ�
		bp->Buf[bp->n++] = bp->next->Buf[0];
		bp=bp->next;
		p=bp->Buf;
		p2=p+bp->n-1;
		for( ; p<p2 ; p++) *p=*(p+1);
		bp->n--;
		f=1;
		if(bp == disptopbp) disptopix--;
		if(bp == cursorbp) cursorix--;
//		else if(bp->n == 0) deleteTBuf(bp);
	}
	if(cursorbp->next ==NULL) return f; //�J�[�\���ʒu���ŏI�o�b�t�@�Ȃ�I��
	//�J�[�\���ʒu�̎��̃o�b�t�@���疄�܂��Ă��Ȃ��o�b�t�@���T�[�`
	bp=cursorbp;
	do{
		bp=bp->next;
		if(bp->next ==NULL) return f; //�ŏI�o�b�t�@�ɓ��B�Ȃ�I��
	} while(bp->n >=TBUFSIZE);

	//�ŏ��Ɍ������󂫏ꏊ�Ɏ��̃o�b�t�@����1�o�C�g�ړ�
	bp->Buf[bp->n++] = bp->next->Buf[0];
	bp=bp->next;
	p=bp->Buf;
	p2=p+bp->n-1;
	for( ; p<p2 ; p++) *p=*(p+1);
	bp->n--;
	f=1;
	if(bp->n == 0) deleteTBuf(bp);
	return f;
}
void gabagecollect2(void){
// �ω����Ȃ��Ȃ�܂�1�o�C�g���̃K�x�[�W�R���N�V�������Ăяo��
	while(gabagecollect1()) ;
}
void inittextbuf(void){
// �e�L�X�g�o�b�t�@�̏�����
	_tbuf *bp;
	for(bp=TextBuffer;bp<TextBuffer+TBUFMAXLINE;bp++) bp->prev=NULL; //���g�p�o�b�t�@��
	TBufstart=TextBuffer; //�����N�̐擪�ݒ�
	TBufstart->next=NULL;
	TBufstart->n=0;
	num=0; //�o�b�t�@�g�p��
	edited=0; //�ҏW�ς݃t���O�N���A
	undobuf_top=undobuf;
	undobuf_used=0;
}
void redraw(){
//��ʂ̍ĕ`��
	unsigned char *vp;
	_tbuf *bp,*bp1,*bp2;
	int ix,ix1,ix2;
	int x,y;
	unsigned char ch,cl;

	vp=TVRAM;
	bp=disptopbp;
	ix=disptopix;
	cl=COLOR_NORMALTEXT;
	if(cursorbp1==NULL){
		//�͈͑I�����[�h�łȂ��ꍇ
		bp1=NULL;
		bp2=NULL;
	}
	else{
		//�͈͑I�����[�h�̏ꍇ�A�J�n�ʒu�ƏI���̑O�㔻�f����
		//bp1,ix1���J�n�ʒu�Abp2,ix2���I���ʒu�ɐݒ�
		if(cy<cy1 || (cy==cy1 && cx<cx1)){
			bp1=cursorbp;
			ix1=cursorix;
			bp2=cursorbp1;
			ix2=cursorix1;
		}
		else{
			bp1=cursorbp1;
			ix1=cursorix1;
			bp2=cursorbp;
			ix2=cursorix;
		}
	}
	for(y=0;y<EDITWIDTHY;y++){
		if(bp==NULL) break;
		for(x=0;x<twidth;x++){
			//����������ʒu�܂ŃT�[�`
			while(ix>=bp->n){
				if(bp==bp1 && ix==ix1) cl=COLOR_AREASELECTTEXT;
				if(bp==bp2 && ix==ix2) cl=COLOR_NORMALTEXT;
				bp=bp->next;
				ix=0;
				if(bp==NULL) break;
			}
			if(bp==NULL) break; //�o�b�t�@�ŏI
			if(bp==bp1 && ix==ix1) cl=COLOR_AREASELECTTEXT;
			if(bp==bp2 && ix==ix2) cl=COLOR_NORMALTEXT;
			ch=bp->Buf[ix++];
			if(ch=='\n') break;
			if(twidth==30) *(vp+ATTROFFSET1)=cl;
			else *(vp+ATTROFFSET2)=cl;
			*vp++=ch;
		}
		//���s����уo�b�t�@�ŏI�ȍ~�̉E���\������
		if(twidth==30){
			for(;x<WIDTH_X1;x++){
				*(vp+ATTROFFSET1)=0;
				*vp++=0;
			}
		}
		else{
			for(;x<WIDTH_X2;x++){
				*(vp+ATTROFFSET2)=0;
				*vp++=0;
			}
		}
	}
	//�o�b�t�@�ŏI�ȍ~�̉����\������
	for(;y<EDITWIDTHY;y++){
		if(twidth==30){
			for(x=0;x<WIDTH_X1;x++){
				*(vp+ATTROFFSET1)=0;
				*vp++=0;
			}
		}
		else{
			for(x=0;x<WIDTH_X2;x++){
				*(vp+ATTROFFSET2)=0;
				*vp++=0;
			}
		}
	}
}

void cursor_left(void){
//�J�[�\����1�O�Ɉړ�
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cy ��ʏ�̃J�[�\���ʒu
//cx2 cx�Ɠ���
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu

	_tbuf *bp;
	int ix;
	int i;
	int x;

	//�o�b�t�@��̃J�[�\���ʒu��1�O�Ɉړ�
	if(cursorix!=0) cursorix--;
	else while(1) {
		//1�O�̃o�b�t�@�̍Ō���Ɉړ��A��������o�b�t�@�͔�΂�
		if(cursorbp->prev==NULL) return; //�e�L�X�g�S�̐擪�Ȃ̂ňړ����Ȃ�
		cursorbp=cursorbp->prev;
		if(cursorbp->n >0){
			cursorix=cursorbp->n-1;//�o�b�t�@�Ō��
			break;
		}
	}

	//�J�[�\������щ�ʍ���ʒu�̍X�V
	if(cx>0){
		//���[�łȂ���΃J�[�\����P����1���Ɉړ����ďI��
		cx--;
		cx2=cx;
		return;
	}
	if(cy>0){
		//���[������[�ł͂Ȃ��ꍇ
		if(cursorbp->Buf[cursorix]!='\n'){
			// �ړ��悪���s�R�[�h�łȂ��ꍇ�A�J�[�\����1��̍s�̉E�[�Ɉړ�
			cx=twidth-1;
			cx2=cx;
			cy--;
			return;
		}
		//��ʍ���ʒu����Ō����X���W���T�[�`
		bp=disptopbp;
		ix=disptopix;
		x=0;
		while(ix!=cursorix || bp!=cursorbp){
			if(bp->n==0){
				//��o�b�t�@�̏ꍇ����
				bp=bp->next;
				ix=0;
				continue;
			}
			if(bp->Buf[ix++]=='\n' || x>=twidth-1) x=0;
			else x++;
			if(ix >= bp->n){
				bp=bp->next;
				ix=0;
			}
		}
		cx=x;
		cx2=cx;
		cy--;
		line_no--;
		return;
	}

	//���[����[�̏ꍇ
	if(cursorbp->Buf[cursorix]!='\n'){
		// �ړ��悪���s�R�[�h�łȂ��ꍇ�A�J�[�\���͉E�[�Ɉړ�
		// ��ʍ���ʒu�͉�ʉ������O�Ɉړ�
		cx=twidth-1;
		cx2=cx;
	}
	else{
		//�ړ��悪���s�R�[�h�̏ꍇ
		//�s���i���s�̎��̕����܂��̓o�b�t�@�擪�j�ƌ��݈ʒu�̕���������
		//��ʉ����Ŋ������]�肪�J�[�\��X���W
		bp=cursorbp;
		ix=cursorix;
		i=0;
		while(1){
			if(ix==0){
				if(bp->prev==NULL) break;
				bp=bp->prev;
				ix=bp->n;
				continue;
			}
			ix--;
			if(bp->Buf[ix]=='\n') break;
			i++;
		}
		cx=i % twidth;
		cx2=cx;
		line_no--;
	}
	//��ʍ���ʒu�͌��݈ʒu����X���W���������Ƃ���
	bp=cursorbp;
	ix=cursorix;
	x=cx;
	while(x>0){
		if(ix==0){
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		x--;
	}
	disptopbp=bp;
	disptopix=ix;
}
void cursor_right(void){
//�J�[�\����1���Ɉړ�
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cy ��ʏ�̃J�[�\���ʒu
//cx2 cx�Ɠ���
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu

	_tbuf *bp;
	int ix;
	int i;
	int x;
	unsigned char c;

	if(cursorix >= cursorbp->n){
		//�o�b�t�@�Ō���̏ꍇ�A���̐擪�Ɉړ�
		bp=cursorbp;
		while(1) {
			//��o�b�t�@�͔�΂�
			if(bp->next==NULL) return; //�e�L�X�g�S�̍Ō���Ȃ̂ňړ����Ȃ�
			bp=bp->next;
			if(bp->n >0) break;
		}
		cursorbp=bp;
		cursorix=0;//�o�b�t�@�擪
	}
	c=cursorbp->Buf[cursorix++]; //�o�b�t�@��̃J�[�\���ʒu�̃R�[�h��ǂ��1���Ɉړ�
	if(c!='\n' && cx<twidth-1){
		//�J�[�\���ʒu�����s�ł��E�[�ł��Ȃ��ꍇ�P����1�E�Ɉړ����ďI��
		cx++;
		cx2=cx;
		return;
	}
	cx=0; //�J�[�\�������[�Ɉړ�
	cx2=cx;
	if(c=='\n') line_no++;
	if(cy<EDITWIDTHY-1){
		//���[�łȂ���΃J�[�\�������s�Ɉړ����ďI��
		cy++;
		return;
	}
	//���[�̏ꍇ
	//��ʍ���ʒu���X�V
	//���s�R�[�h�܂��͉�ʉ���������܂ŃT�[�`
	bp=disptopbp;
	ix=disptopix;
	x=0;
	while(x<twidth){
		if(ix >= bp->n){
			bp=bp->next;
			ix=0;
			continue;
		}
		if(bp->Buf[ix++]=='\n') break;
		x++;
	}
	disptopbp=bp;
	disptopix=ix;
}
void cursor_up(void){
//�J�[�\����1��Ɉړ�
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cy ��ʏ�̃J�[�\���ʒu
//cx2 �ړ��O��cx�Ɠ���
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu

	_tbuf *bp;
	int ix;
	int i;
	int x;
	unsigned char c;

	//��ʕ����O�ɖ߂����Ƃ��낪�o�b�t�@��J�[�\���̈ړ���
	//�r���ŉ��s�R�[�h������Εʂ̎�i�Ō���
	bp=cursorbp;
	ix=cursorix;
	i=cx2-cx;
	while(i<twidth){
		if(ix==0){
			if(bp->prev==NULL) return; //�o�b�t�@�擪�܂ŃT�[�`������ړ��Ȃ�
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		if(bp->Buf[ix]=='\n') break;
		i++;
	}
	cursorbp=bp;
	cursorix=ix;
	//��ʕ��̊Ԃɉ��s�R�[�h���Ȃ������ꍇ
	if(i==twidth){
		cx=cx2;
		//��ʏ�[�łȂ���΃J�[�\����1��Ɉړ����ďI��
		if(cy>0){
			cy--;
			return;
		}
		//��ʏ�[�̏ꍇ�A�J�[�\���ʒu����X���W���߂����Ƃ��낪��ʍ���ʒu
		x=cx;
		while(x>0){
			if(ix==0){
				bp=bp->prev;
				ix=bp->n;
				continue;
			}
			ix--;
			x--;
		}
		disptopbp=bp;
		disptopix=ix;
		return;
	}
	//���s�����������ꍇ
	//�s���i���s�̎��̕����܂��̓o�b�t�@�擪�j�ƌ��݈ʒu�̕���������
	//��ʉ����Ŋ������]������߂�
	line_no--;
	i=0;
	while(1){
		if(ix==0){
			if(bp->prev==NULL) break;
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		if(bp->Buf[ix]=='\n') break;
		i++;
	}
	x=i % twidth; //���s�u���b�N�̍ŏI�s�̉E�[
	bp=cursorbp;
	ix=cursorix;
	//�o�b�t�@��̃J�[�\���ʒu�͉��s�u���b�N�̍ŏI�s�E�[����J�[�\��X���W���߂�
	//�ŏI�s�E�[�̂ق����������ꍇ�A���̏ꏊ���o�b�t�@��̃J�[�\���ʒu�Ƃ���
	while(x>cx2){
		if(ix==0){
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		x--;
	}
	cursorbp=bp;
	cursorix=ix;
	cx=x; //cx2�܂��͉��s�u���b�N�ŏI�s�E�[
	if(cy>0){
		//��ʏ�[�łȂ���΃J�[�\����1��Ɉړ����ďI��
		cy--;
		return;
	}
	//��ʏ�[�̏ꍇ
	//��ʍ���ʒu�͌��݈ʒu����X���W���������Ƃ���
	while(x>0){
		if(ix==0){
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		x--;
	}
	disptopbp=bp;
	disptopix=ix;
}
void cursor_down(void){
//�J�[�\����1���Ɉړ�
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cy ��ʏ�̃J�[�\���ʒu
//cx2 �ړ��O��cx�Ɠ���
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu

	_tbuf *bp;
	int ix;
	int x;
	unsigned char c;

	//���s�̐擪�T�[�`
	//�J�[�\���ʒu�����ʉE�[�܂ł̊Ԃɉ��s�R�[�h������Ύ��̕������擪
	bp=cursorbp;
	ix=cursorix;
	x=cx;
	while(x<twidth){
		if(ix>=bp->n){
			if(bp->next==NULL) return; //�o�b�t�@�Ō�܂ŃT�[�`������ړ��Ȃ�
			bp=bp->next;
			ix=0;
			continue;
		}
		c=bp->Buf[ix];
		ix++;
		x++;
		if(c=='\n'){
			line_no++;
			break;
		}
	}
	//���s�擪����cx2�����������ɃT�[�`
	x=0;
	while(x<cx2){
		if(ix>=bp->n){
			if(bp->next==NULL) break; //�o�b�t�@�Ō�̏ꍇ�����Ɉړ�
			bp=bp->next;
			ix=0;
			continue;
		}
		if(bp->Buf[ix]=='\n') break; //���s�R�[�h�̏ꍇ�����Ɉړ�
		ix++;
		x++;
	}
	cursorbp=bp;
	cursorix=ix;
	cx=x;
	//��ʉ��[�łȂ���΃J�[�\����1���Ɉړ����ďI��
	if(cy<EDITWIDTHY-1){
		cy++;
		return;
	}
	//���[�̏ꍇ
	//��ʍ���ʒu���X�V
	//���s�R�[�h�܂��͉�ʉ���������܂ŃT�[�`
	bp=disptopbp;
	ix=disptopix;
	x=0;
	while(x<twidth){
		if(ix >= bp->n){
			bp=bp->next;
			ix=0;
			continue;
		}
		if(bp->Buf[ix++]=='\n') break;
		x++;
	}
	disptopbp=bp;
	disptopix=ix;
}
void cursor_home(void){
//�J�[�\�����s�擪�Ɉړ�
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cx2 0
//cy �ύX�Ȃ�
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu�i�ύX�Ȃ��j

	//�J�[�\��X���W���O�Ɉړ�
	while(cx>0){
		if(cursorix==0){
			//��o�b�t�@�͔�΂�
			cursorbp=cursorbp->prev;
			cursorix=cursorbp->n;
			continue;
		}
		cursorix--;
		cx--;
	}
	cx2=0;
}
void cursor_end(void){
//�J�[�\�����s���Ɉړ�
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cx2 �s��
//cy �ύX�Ȃ�
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu�i�ύX�Ȃ��j

	//�J�[�\��X���W����ʕ������Ɉړ�
	//���s�R�[�h�܂��̓o�b�t�@�ŏI������΂����Ɉړ�
	while(cx<twidth-1){
		if(cursorix>=cursorbp->n){
			//��o�b�t�@�͔�΂�
			if(cursorbp->next==NULL) break;
			cursorbp=cursorbp->next;
			cursorix=0;
			continue;
		}
		if(cursorbp->Buf[cursorix]=='\n') break;
		cursorix++;
		cx++;
	}
	cx2=cx;
}
void cursor_pageup(void){
//PageUp�L�[
//�ŏ�s���ŉ��s�ɂȂ�܂ŃX�N���[��
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cx2
//cy
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu

	_tbuf *bp;
	int ix;
	int i;
	int cy_old;

	cy_old=cy;
	while(cy>0) cursor_up(); // cy==0�ɂȂ�܂ŃJ�[�\������Ɉړ�
	for(i=0;i<EDITWIDTHY-1;i++){
		//��ʍs��-1�s���J�[�\������Ɉړ�
		bp=disptopbp;
		ix=disptopix;
		cursor_up();
		if(bp==disptopbp && ix==disptopix) break; //�ŏ�s�ňړ��ł��Ȃ������ꍇ������
	}
	//����Y���W�܂ŃJ�[�\�������Ɉړ��A1�s�������Ȃ������ꍇ�͍ŏ�s�ɗ��܂�
	if(i>0) while(cy<cy_old) cursor_down();
}
void cursor_pagedown(void){
//PageDown�L�[
//�ŉ��s���ŏ�s�ɂȂ�܂ŃX�N���[��
//�o�́F���L�ϐ����ړ���̒l�ɕύX
//cursorbp,cursorix �o�b�t�@��̃J�[�\���ʒu
//cx,cx2
//cy
//disptopbp,disptopix ��ʍ���̃o�b�t�@��̈ʒu

	_tbuf *bp;
	int ix;
	int i;
	int y;
	int cy_old;

	cy_old=cy;
	while(cy<EDITWIDTHY-1){
		// cy==EDITWIDTH-1�ɂȂ�܂ŃJ�[�\�������Ɉړ�
		y=cy;
		cursor_down();
		if(y==cy) break;// �o�b�t�@�ŉ��s�ňړ��ł��Ȃ������ꍇ������
	}
	for(i=0;i<EDITWIDTHY-1;i++){
		//��ʍs��-1�s���J�[�\�������Ɉړ�
		bp=disptopbp;
		ix=disptopix;
		cursor_down();
		if(bp==disptopbp && ix==disptopix) break; //�ŉ��s�ňړ��ł��Ȃ������ꍇ������
	}
	//���[���炳��Ɉړ������s�����A�J�[�\������Ɉړ��A1�s�������Ȃ������ꍇ�͍ŉ��s�ɗ��܂�
	if(i>0) while(cy>cy_old) cursor_up();
}
void cursor_top(void){
//�J�[�\�����e�L�X�g�o�b�t�@�̐擪�Ɉړ�
	cursorbp=TBufstart;
	cursorix=0;
	cursorbp1=NULL; //�͈͑I�����[�h����
	disptopbp=cursorbp;
	disptopix=cursorix;
	cx=0;
	cx2=0;
	cy=0;
	line_no=1;
}

int countarea(void){
//�e�L�X�g�o�b�t�@�̎w��͈͂̕��������J�E���g
//�͈͂�(cursorbp,cursorix)��(cursorbp1,cursorix1)�Ŏw��
//��둤�̈�O�̕����܂ł��J�E���g
	_tbuf *bp1,*bp2;
	int ix1,ix2;
	int n;

	//�͈͑I�����[�h�̏ꍇ�A�J�n�ʒu�ƏI���̑O�㔻�f����
	//bp1,ix1���J�n�ʒu�Abp2,ix2���I���ʒu�ɐݒ�
	if(cy<cy1 || (cy==cy1 && cx<cx1)){
		bp1=cursorbp;
		ix1=cursorix;
		bp2=cursorbp1;
		ix2=cursorix1;
	}
	else{
		bp1=cursorbp1;
		ix1=cursorix1;
		bp2=cursorbp;
		ix2=cursorix;
	}
	n=0;
	while(1){
		if(bp1==bp2 && ix1==ix2) return n;
		if(ix1 < bp1->n){
			n++;
			ix1++;
		}
		else{
			bp1=bp1->next;
			ix1=0;
		}
	}
}
void deletearea_len(_tbuf *bp,unsigned int ix,int n,int undo){
//�e�L�X�g�o�b�t�@�̎w��ʒu���畡�������폜
//bp,ix:�폜�J�n�ʒu
//n:�폜���镶����
//undo:0:�ʏ�A2:�A���h�D��
	unsigned char *p;
	int i;

	//�I��͈͂��ŏ��̃o�b�t�@�̍Ō�܂ł���ꍇ
	if(n>=(bp->n - ix)){
		if(!undo){
			p=bp->Buf+ix;
			for(i=ix;i < bp->n;i++) pushundomem(*p++); //�A���h�D�o�b�t�@�Ɋi�[
		}
		n -= bp->n - ix; //�폜��������
		num-=bp->n - ix; //�o�b�t�@�g�p�ʂ�����
		bp->n=ix; //ix�ȍ~���폜
		bp=bp->next;
		if(bp==NULL) return;
		ix=0;
	}
	//���̃o�b�t�@�ȍ~�A�I��͈͂̏I���ʒu���܂܂�Ȃ��o�b�t�@�͍폜
	while(n>=bp->n){
		if(!undo){
			p=bp->Buf;
			for(i=0;i < bp->n;i++) pushundomem(*p++); //�A���h�D�o�b�t�@�Ɋi�[
		}
		n-=bp->n; //�폜��������
		num-=bp->n; //�o�b�t�@�g�p�ʂ�����
		bp=deleteTBuf(bp); //�o�b�t�@�폜���Ď��̃o�b�t�@�ɐi��
		if(bp==NULL) return;
	}
	//�I��͈͂̏I���ʒu���܂ޏꍇ�A1�������폜
	if(!undo) undo=1;
	while(n>0){
		deletechar(bp,ix,undo); //�o�b�t�@����1�����폜�inum�͊֐�����1�������j
		n--;
	}
}
void deletearea(void){
//�e�L�X�g�o�b�t�@�̎w��͈͂��폜
//�͈͂�(cursorbp,cursorix)��(cursorbp1,cursorix1)�Ŏw��
//��둤�̈�O�̕����܂ł��폜
//�폜��̃J�[�\���ʒu�͑I��͈͂̐擪�ɂ��A�͈͑I�����[�h��������

	_tbuf *bp;
	int ix;
	int n;

	n=countarea(); //�I��͈͂̕������J�E���g
	if(n==0) return;

	//�͈͑I���̊J�n�ʒu�ƏI���ʒu�̑O��𔻒f���ăJ�[�\�����J�n�ʒu�ɐݒ�
	if(cy>cy1 || (cy==cy1 && cx>cx1)){
		cursorbp=cursorbp1;
		cursorix=cursorix1;
		cx=cx1;
		cy=cy1;
		line_no=line_no1;
	}
	cx2=cx;
	cursorbp1=NULL; //�͈͑I�����[�h����

	//bp,ix���J�n�ʒu�ɐݒ�
	bp=cursorbp;
	ix=cursorix;

	setundobuf(UNDO_CONTDEL,bp,ix,0,n); //�A���h�D�o�b�t�@�ݒ�i�A���폜�J�n�j
	deletearea_len(bp,ix,n,0); //n�������폜
	//�A���h�D�o�b�t�@�ɘA���폜�I���ݒ�
	pushundomem2(bpixtopos(bp,ix));
	pushundomem2(n);
	pushundomem(UNDO_CONTDEL);
}
void clipcopy(void){
// �I��͈͂��N���b�v�{�[�h�ɃR�s�[
	_tbuf *bp1,*bp2;
	int ix1,ix2;
	char *ps,*pd;

	//�͈͑I�����[�h�̏ꍇ�A�J�n�ʒu�ƏI���̑O�㔻�f����
	//bp1,ix1���J�n�ʒu�Abp2,ix2���I���ʒu�ɐݒ�
	if(cy<cy1 || (cy==cy1 && cx<cx1)){
		bp1=cursorbp;
		ix1=cursorix;
		bp2=cursorbp1;
		ix2=cursorix1;
	}
	else{
		bp1=cursorbp1;
		ix1=cursorix1;
		bp2=cursorbp;
		ix2=cursorix;
	}
	ps=bp1->Buf+ix1;
	pd=clipboard;
	clipsize=0;
	while(bp1!=bp2 || ix1!=ix2){
		if(ix1 < bp1->n){
			*pd++=*ps++;
			clipsize++;
			ix1++;
		}
		else{
			bp1=bp1->next;
			ps=bp1->Buf;
			ix1=0;
		}
	}
}
void clippaste(void){
// �N���b�v�{�[�h����\��t��
	int n,i;
	unsigned char *p;

	if(clipsize==0 || num+clipsize>TBUFMAXSIZE) return;
	setundobuf(UNDO_CONTINS,cursorbp,cursorix,0,clipsize); //�A���h�D�o�b�t�@�ݒ�
	p=clipboard;
	for(n=clipsize;n>0;n--){
		i=insertchar(cursorbp,cursorix,*p,1);
		if(i>0){
			//�o�b�t�@�󂫂�����̂ɑ}�����s�̏ꍇ
			gabagecollect2(); //�S�̃K�x�[�W�R���N�V����
			i=insertchar(cursorbp,cursorix,*p,1);//�e�L�X�g�o�b�t�@�ɂP�����}��
		}
		if(i!=0) break;//�}�����s
		cursor_right();//��ʏ�A�o�b�t�@��̃J�[�\���ʒu��1���Ɉړ�
		p++;
	}
}
void movecursor(int pos){
// �J�[�\�������݂̈ʒu����C�ӂ̈ʒu�Ɉړ�
// pos�F�ړ��������e�L�X�g�o�b�t�@�擪����̃o�C�g�ʒu
	int pos2,d;
	pos2=bpixtopos(cursorbp,cursorix);
	d=pos-pos2;
	if(d==0) return;
	if(d>0){
		while(d>0){
			cursor_right();
			d--;
		}
	}
	else{
		while(d<0){
			cursor_left();
			d++;
		}
	}
}
void undoexec(){
//�A���h�D���s
	unsigned char c,c1;
	_tbuf *bp;
	unsigned short n,ix;
	int pos;

	if(undobuf_used==0) return; //�A���h�D�o�b�t�@��
	cursorbp1=NULL; //�͈͑I������
	c=popundomem(); //�A���h�D�o�b�t�@�擪�̖��ߓǂݏo��
	switch(c){
		case UNDO_INSERT: //1�����}��
			//�J�[�\���ړ���1�����폜
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			deletechar(bp,ix,2);
			popundomem(); //dummy read
			break;
		case UNDO_CONTINS: //�A���}��
			//�J�[�\���ړ����A�������폜
			n=popundomem2();
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			deletearea_len(bp,ix,n,2);
			popundomem(); //dummy read
			break;
		case UNDO_CONTDEL: //�A���폜
			//�J�[�\���ړ����A�������A�R�[�h��ǂݏo���đ}��
			n=popundomem2();
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			while(n>0){
				insertchar(bp,ix,popundomem(),2);
				n--;
			}
			popundomem2(); //dummy read
			popundomem(); //dummy read
			break;
		case 0: //0�̏ꍇ�A����1�o�C�g�����ۂ̗L���ȃR�[�h
			c=popundomem();
		default: //1�����폜�iDEL�ABS�j�A1�����㏑��
			//�J�[�\���ړ���1�����}���i�܂��͏㏑���j
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			c1=popundomem();
			if(c1==UNDO_OVERWRITE){
				overwritechar(bp,ix,c,2);
			}
			else{
				insertchar(bp,ix,c,2);
				if(c1==UNDO_BACKSPACE) cursor_right();
			}
			break;
	}
}

void set_areamode(){
//�͈͑I�����[�h�J�n���̃J�[�\���J�n�ʒu�O���[�o���ϐ��ݒ�
	cursorbp1=cursorbp;
	cursorix1=cursorix;
	cx1=cx;
	cy1=cy;
	line_no1=line_no;
}
void save_cursor(void){
//�J�[�\���֘A�O���[�o���ϐ����ꎞ���
	cursorbp_t=cursorbp;
	cursorix_t=cursorix;
	disptopbp_t=disptopbp;
	disptopix_t=disptopix;
	cx_t=cx;
	cy_t=cy;
	line_no_t=line_no;
}
void restore_cursor(void){
//�J�[�\���֘A�O���[�o���ϐ����ꎞ���ꏊ����߂�
	cursorbp=cursorbp_t;
	cursorix=cursorix_t;
	disptopbp=disptopbp_t;
	disptopix=disptopix_t;
	cx=cx_t;
	cy=cy_t;
	line_no=line_no_t;
}

int filesystemretry(){
// SD�t�@�C���V�X�e���̍ď������m�F�Ǝ��{
// SD�t�@�C���ւ̕ۑ���ǂݍ��ݎ��Ƀt�@�C���G���[�����������ꍇ�ɌĂяo��
// �߂�l�@0�F�����������A-1�F�������邱�ƂȂ�Escape�Ŕ�����
	unsigned short vk;
	while(1){
		setcursorcolor(COLOR_NORMALTEXT);
		printstr((unsigned char *)Message3); //Retry / Quit
		while(1){
			inputchar(); //1�������͑҂�
			vk=vkey & 0xff;
			if(vk==VK_RETURN || vk==VK_SEPARATOR) break;
			if(vk==VK_ESCAPE) return -1;
		}
		//�t�@�C���V�X�e��������
		if(FSInit()!=FALSE) return 0; //����
		//�G���[�̏ꍇ
		setcursorcolor(COLOR_ERRORTEXT);
		printstr((unsigned char *)Message2);//File System Error
	}
}

int sdfilecopy(char *sourcefile,char *distfile){
// SD�J�[�h��̃t�@�C�����R�s�[
// soucefile:�R�s�[���t�@�C����
// distfile:�R�s�[��t�@�C����
// �߂�l�F����I�� 0�A�G���[�I�����G���[�ԍ�
	FSFILE *sfp,*dfp;
	int n,er,c;
	er=0;
	sfp=FSfopen(sourcefile,"r");
	if(sfp==NULL) return ERR_CANTFILEOPEN;
	dfp=FSfopen(distfile,"w");
	if(dfp==NULL){
		FSfclose(sfp);
		return ERR_CANTFILEOPEN;
	}
	c=0;
	while(1){
		if(c==0){
			printchar('.');
			c=100;
		}
		c--;
		n=FSfread(filebuf,1,FILEBUFSIZE,sfp);
		if(n==0) break;
		if(FSfwrite(filebuf,1,n,dfp)!=n){
			er=ERR_CANTWRITEFILE;
			break;
		}
	}
	FSfclose(sfp);
	FSfclose(dfp);
	return er;
}
int savetextfile(char *filename){
// �e�L�X�g�o�b�t�@���e�L�X�g�t�@�C���ɏ�������
// �������ݐ�����0�A���s�ŃG���[�R�[�h�i�����j��Ԃ�
	FSFILE *fp;
	_tbuf *bp;
	int ix,n,i,er;
	unsigned char *ps,*pd;
	er=0;//�G���[�R�[�h
	i=-1;
	fp=FSfopen(filename,"w");
	if(fp==NULL) return ERR_CANTFILEOPEN;
	bp=TBufstart;
	ix=0;
	ps=bp->Buf;
	do{
		pd=filebuf;
		n=0;
		while(n<FILEBUFSIZE-1){
		//���s�R�[�h��2�o�C�g�ɂȂ邱�Ƃ��l�����ăo�b�t�@�T�C�Y-1�܂łƂ���
			while(ix>=bp->n){
				bp=bp->next;
				if(bp==NULL){
					break;
				}
				ix=0;
				ps=bp->Buf;
			}
			if(bp==NULL) break;
			if(*ps=='\n'){
				*pd++='\r'; //���s�R�[�h0A��0D 0A�ɂ���
				n++;
			}
			*pd++=*ps++;
			ix++;
			n++;
		}
		if(n>0){
			i=FSfwrite(filebuf,1,n,fp);
			if(i!=n) er=ERR_CANTWRITEFILE;
		}
	} while(bp!=NULL && er==0);
	FSfclose(fp);
	return er;
}
int loadtextfile(char *filename){
// �e�L�X�g�t�@�C�����e�L�X�g�o�b�t�@�ɓǂݍ���
// �ǂݍ��ݐ�����0�A���s�ŃG���[�R�[�h�i�����j��Ԃ�
	FSFILE *fp;
	_tbuf *bp;
	int ix,n,i,er;
	unsigned char *ps,*pd;
	er=0;//�G���[�R�[�h
	fp=FSfopen(filename,"r");
	if(fp==NULL) return ERR_CANTFILEOPEN;
	inittextbuf();
	bp=TextBuffer;
	ix=0;
	pd=bp->Buf;
	do{
		n=FSfread(filebuf,1,FILEBUFSIZE,fp);
		ps=filebuf;
		for(i=0;i<n;i++){
			if(ix>=TBUFSIZE){
				bp->n=TBUFSIZE;
				bp=newTBuf(bp);
				if(bp==NULL){
					er=ERR_FILETOOBIG;
					break;
				}
				ix=0;
				pd=bp->Buf;
			}
			if(*ps=='\r') ps++; //���s�R�[�h0D 0A��0A�ɂ���i�P����0D�����j
			else{
				*pd++=*ps++;
				ix++;
				num++;//�o�b�t�@��������
				if(num>TBUFMAXSIZE){
					er=ERR_FILETOOBIG;
					break;
				}
			}
		}
	} while(n==FILEBUFSIZE && er==0);
	if(bp!=NULL) bp->n=ix;//�Ō�̃o�b�t�@�̕�����
	FSfclose(fp);
	if(er){
		//�G���[�����̏ꍇ�o�b�t�@�A�J�[�\���ʒu������
		inittextbuf();
		cursor_top();
	}
	return er;
}
int overwritecheck(char *fn){
// �t�@�C���̏㏑���m�F
// �t�@�C���̑��݂��`�F�b�N���A���݂���ꍇ�L�[�{�[�h����㏑���m�F����
// fn:�t�@�C�����ւ̃|�C���^
// �߂�l�@0�F���݂��Ȃ��܂��͏㏑���A-1�F�㏑�����Ȃ�
	SearchRec sr;
	unsigned short vk;
	if(FindFirst(fn,ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_ARCHIVE,&sr)) return 0; //�t�@�C�������݂��Ȃ�
	setcursorcolor(COLOR_ERRORTEXT);
	printstr(fn);
	printstr(": File Exists\n");
	setcursorcolor(COLOR_NORMALTEXT);
	printstr("Ovewrite:[Enter] / Stop:[ESC]\n");
	while(1){
		inputchar(); //1�������͑҂�
		vk=vkey & 0xff;
		if(vk==VK_RETURN || vk==VK_SEPARATOR) return 0;
		if(vk==VK_ESCAPE) return -1;
	}
}
void printfilename(unsigned char x,unsigned char y,int f,int num_dir){
// x,y�̈ʒu�Ƀt�@�C�����܂��̓f�B���N�g������\��

	if(f==-2){
		setcursor(x,y,COLOR_ERRORTEXT);
		printchar('<');
		printstr("New FILE");
		printchar('>');
	}
	else if(f==-1){
		setcursor(x,y,COLOR_ERRORTEXT);
		printchar('<');
		printstr("New Dir");
		printchar('>');
	}
	else if(f<num_dir){
		setcursor(x,y,COLOR_DIR);
		printchar('[');
		printstr(filenames[f]);
		printchar(']');
	}
	else{
		setcursor(x,y,COLOR_NORMALTEXT);
		printstr(filenames[f]);
	}
}
int select_dir_file(int filenum,int num_dir, unsigned char* msg){
// filenames[]�z��ɓǂݍ��܂ꂽ�t�@�C���܂��̓f�B���N�g������ʕ\�����L�[�{�[�h�őI������
// filenum:�t�@�C���{�f�B���N�g����
// num_dir:�f�B���N�g�����ifilenames[]�͐擪����num_dir-1�܂ł��f�B���N�g���j
// msg:��ʏ㕔�ɕ\�����郁�b�Z�[�W
// �߂�l
//�@filenames[]�̑I�����ꂽ�t�@�C���܂��̓f�B���N�g���ԍ�
//�@-1�F�V�K�f�B���N�g���쐬�Atempfile[]�Ƀf�B���N�g����
//�@-2�F�V�K�t�@�C���쐬�Atempfile[]�Ƀt�@�C����
//�@-3�FESC�L�[�������ꂽ
	int top,f;
	unsigned char *ps,*pd;
	int x,y;
	unsigned char vk;
	//�t�@�C���ꗗ����ʂɕ\��
	cls();
	setcursor(0,0,COLOR_NORMALTEXT);
	printstr(msg);
	printstr(": ");
	setcursorcolor(4);
	printstr("Select&[Enter] / [ESC]\n");
	for(f=-2;f<filenum;f++){
		x=(f&1)*15+1;
		y=(f+2)/2+1;
		if(y>=WIDTH_Y-1) break;
		printfilename(x,y,f,num_dir);
	}
	top=-2;//��ʈ�Ԑ擪�̃t�@�C���ԍ�
	f=-2;//���ݑI�𒆂̃t�@�C���ԍ�
	while(1){
		setcursor((f&1)*15,(f-top)/2+1,5);
		printchar(0x1c);// Right Arrow
		cursor--;
		while(1){
			inputchar();
			vk=vkey & 0xff;
			if(vk) break;
		}
		printchar(' ');
		setcursor(0,WIDTH_Y-1,COLOR_NORMALTEXT);
		for(x=0;x<twidth-1;x++) printchar(' '); //�ŉ��s�̃X�e�[�^�X�\��������
		switch(vk){
			case VK_UP:
			case VK_NUMPAD8:
				//����L�[
				if(f>=0){
					f-=2;
					if(f<top){
						//��ʍŏ㕔�̏ꍇ�A���ɃX�N���[�����čŏ㕔�Ƀt�@�C����2�\��
						if(twidth==WIDTH_X1){
							setcursor(WIDTH_X1-1,WIDTH_Y-2,COLOR_NORMALTEXT);
							while(cursor>=TVRAM+WIDTH_X1*2){
								*cursor=*(cursor-WIDTH_X1);
								*(cursor+ATTROFFSET1)=*(cursor+ATTROFFSET1-WIDTH_X1);
								cursor--;
							}
							while(cursor>=TVRAM+WIDTH_X1) *cursor--=' ';
						}
						else{
							setcursor(WIDTH_X2-1,WIDTH_Y-2,COLOR_NORMALTEXT);
							while(cursor>=TVRAM+WIDTH_X2*2){
								*cursor=*(cursor-WIDTH_X2);
								*(cursor+ATTROFFSET2)=*(cursor+ATTROFFSET2-WIDTH_X2);
								cursor--;
							}
							while(cursor>=TVRAM+WIDTH_X2) *cursor--=' ';
						}
						top-=2;
						printfilename(1,1,top,num_dir);
						printfilename(16,1,top+1,num_dir);
					}
				}
				break;
			case VK_DOWN:
			case VK_NUMPAD2:
				//�����L�[
				if(((f+2)&0xfffe)<filenum){
					f+=2;
					if(f>=filenum) f--;
					if(f-top>=(WIDTH_Y-2)*2){
						//��ʍŉ����̏ꍇ�A��ɃX�N���[�����čŉ����Ƀt�@�C����1��or2�\��
						setcursor(0,1,COLOR_NORMALTEXT);
						if(twidth==WIDTH_X1){
							while(cursor<TVRAM+WIDTH_X1*(WIDTH_Y-2)){
								*cursor=*(cursor+WIDTH_X1);
								*(cursor+ATTROFFSET1)=*(cursor+ATTROFFSET1+WIDTH_X1);
								cursor++;
							}
							while(cursor<TVRAM+WIDTH_X1*(WIDTH_Y-1)) *cursor++=' ';
						}
						else{
							while(cursor<TVRAM+WIDTH_X2*(WIDTH_Y-2)){
								*cursor=*(cursor+WIDTH_X2);
								*(cursor+ATTROFFSET2)=*(cursor+ATTROFFSET2+WIDTH_X2);
								cursor++;
							}
							while(cursor<TVRAM+WIDTH_X2*(WIDTH_Y-1)) *cursor++=' ';
						}
						top+=2;
						printfilename(1,WIDTH_Y-2,f&0xfffe,num_dir);
						if((f|1)<filenum){
							printfilename(16,WIDTH_Y-2,f|1,num_dir);
						}
					}
				}
				break;
			case VK_LEFT:
			case VK_NUMPAD4:
				//�����L�[
				if(f&1) f--;
				break;
			case VK_RIGHT:
			case VK_NUMPAD6:
				//�E���L�[
				if((f&1)==0 && f+1<filenum) f++;
				break;
			case VK_RETURN: //Enter�L�[
			case VK_SEPARATOR: //�e���L�[��Enter
				if(f==-2){
					//�V�K�t�@�C��
					setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
					printstr("Input File Name: ");
					setcursorcolor(COLOR_NORMALTEXT);
					//�t�@�C��������
					*tempfile=0;
					if(lineinput(tempfile,8+1+3)<0) break; //ESC�L�[
					if(*tempfile==0) break; //�t�@�C�������͂Ȃ�
				}
				else if(f==-1){
					//�V�K�f�B���N�g��
					setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
					printstr("Input Dir Name: ");
					setcursorcolor(COLOR_NORMALTEXT);
					//�f�B���N�g��������
					*tempfile=0;
					if(lineinput(tempfile,8+1+3)<0) break; //ESC�L�[
					if(FSmkdir(tempfile)){
						setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
						printstr("Cannot Make Directory        ");
						break;
					}
				}
				else{
					//�t�@�C�����܂��̓f�B���N�g������tempfile�ɃR�s�[
					ps=filenames[f];
					pd=tempfile;
					while(*ps) *pd++=*ps++;
					*pd=0;
				}
				return f;
			case VK_ESCAPE:
				//ESC�L�[
				return -3;
		}
	}
}
int getfilelist(int *p_num_dir){
// �J�����g�f�B���N�g���ł̃f�B���N�g���A.BAS�A.TXT�A.INI�t�@�C���ꗗ��ǂݍ���
// *p_num_dir:�f�B���N�g������Ԃ�
// filenames[]:�t�@�C��������уf�B���N�g�����ꗗ
// �߂�l�@�t�@�C���{�f�B���N�g����

	unsigned char *ps,*pd;
	int filenum;
	SearchRec sr;
	filenum=0;
	//�f�B���N�g���̃T�[�`
	if(FindFirst("*",ATTR_DIRECTORY,&sr)==0){
		do{
			//filenames[]�Ƀf�B���N�g�����̈ꗗ��ǂݍ���
			ps=sr.filename;
			pd=filenames[filenum];
			while(*ps!=0) *pd++=*ps++;
			*pd=0;
			filenum++;
		}
		while(!FindNext(&sr) && filenum<MAXFILENUM);
	}
	*p_num_dir=filenum;
	if(filenum>=MAXFILENUM) return filenum;
	//�g���q BAS�t�@�C���̃T�[�`
	if(FindFirst("*.BAS",ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_ARCHIVE,&sr)==0){
		do{
			//filenames[]�Ƀt�@�C�����̈ꗗ��ǂݍ���
			ps=sr.filename;
			pd=filenames[filenum];
			while(*ps!=0) *pd++=*ps++;
			*pd=0;
			filenum++;
		}
		while(!FindNext(&sr) && filenum<MAXFILENUM);
	}
	if(filenum>=MAXFILENUM) return filenum;
	//�g���q TXT�t�@�C���̃T�[�`
	if(FindFirst("*.TXT",ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_ARCHIVE,&sr)==0){
		do{
			//filenames[]�Ƀt�@�C�����̈ꗗ��ǂݍ���
			ps=sr.filename;
			pd=filenames[filenum];
			while(*ps!=0) *pd++=*ps++;
			*pd=0;
			filenum++;
		}
		while(!FindNext(&sr) && filenum<MAXFILENUM);
	}
	if(filenum>=MAXFILENUM) return filenum;
	//�g���q INI�t�@�C���̃T�[�`
	if(FindFirst("*.INI",ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_ARCHIVE,&sr)==0){
		do{
			//filenames[]�Ƀt�@�C�����̈ꗗ��ǂݍ���
			ps=sr.filename;
			pd=filenames[filenum];
			while(*ps!=0) *pd++=*ps++;
			*pd=0;
			filenum++;
		}
		while(!FindNext(&sr) && filenum<MAXFILENUM);
	}
	return filenum;
}
void save_as(int ow){
// ���݂̃e�L�X�g�o�b�t�@�̓��e��SD�J�[�h�ɕۑ�
// ow�@0:���O��t���ĕۑ��@�@1:�㏑���ۑ�
// �t�@�C�����̓O���[�o���ϐ�currentfile[]
// �t�@�C�����̓L�[�{�[�h����ύX�\
// ���������ꍇcurrentfile���X�V

	int er;
	int filenum,num_dir,f;
	unsigned char *ps,*pd;

	cls();
	setcursor(0,0,COLOR_NORMALTEXT);
	printstr("Save To SD Card\n");
	if(currentfile[0]==0) ow=0; //�t�@�C�������ݒ肳��Ă��Ȃ��ꍇ���O��t���ĕۑ�

	//currentfile����tempfile�ɃR�s�[
	ps=currentfile;
	pd=tempfile;
	while(*ps!=0) *pd++=*ps++;
	*pd=0;

	//�J�����g�f�B���N�g����ϐ�cwdpath�ɃR�s�[
	while(1){
		if(FSgetcwd(cwdpath,PATHNAMEMAX)) break;
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Cannot Get Current Dir\n");
		if(filesystemretry()) return; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
	}
	//���݂̃f�B���N�g���̃p�X��\��
	setcursorcolor(COLOR_NORMALTEXT);
	printstr("Current Directory is\n");
	printstr(cwdpath);
	printchar('\n');
	while(1){
		if(ow==0){
			printstr("Input File Name + [Enter]\n");
			printstr("[ESC] Select File/Dir or Quit\n");
			//�t�@�C��������
			if(lineinput(tempfile,8+1+3)<0){
				//ESC�L�[�������ꂽ�ꍇ�A�t�@�C���I���A�f�B���N�g���ύX��ʂ܂��͏I��
				while(1){
					filenum=getfilelist(&num_dir); //�f�B���N�g���A�t�@�C�����ꗗ��ǂݍ���
					f=select_dir_file(filenum,num_dir,"Save"); //�t�@�C���̑I��
					cls();
					if(f==-3){
						//�I��
						FSchdir(cwdpath);//�J�����g�f�B���N�g�������ɖ߂�
						return;
					}
					else if(f==-2){
						//�V�K�t�@�C��
						if(overwritecheck(tempfile)==0) break;//�㏑���`�F�b�N
					}
					else if(f<num_dir){
						//�V�K�f�B���N�g���܂��̓f�B���N�g���ύX
						FSchdir(tempfile);//�f�B���N�g���ύX���čēx�t�@�C���ꗗ��
					}
					else break;
				}
			}
			else{
				if(*tempfile==0) continue; //NULL������̏ꍇ
				if(overwritecheck(tempfile)) continue;
			}
		}
		printstr("Writing...\n");
		er=savetextfile(tempfile); //�t�@�C���ۑ��Aer:�G���[�R�[�h
		if(er==0){
			printstr("OK");
			FSremove(TEMPFILENAME); //���s���ɐ�������ꎞ�t�@�C�����폜
			//tempfile����currentfile�ɃR�s�[���ďI��
			ps=tempfile;
			pd=currentfile;
			while(*ps!=0) *pd++=*ps++;
			*pd=0;
			FSgetcwd(cwdpath,PATHNAMEMAX); //�J�����g�p�X���X�V
			edited=0; //�ҏW�ς݃t���O�N���A
			wait60thsec(60);//1�b�҂�
			return;
		}
		setcursorcolor(COLOR_ERRORTEXT);
		if(er==ERR_CANTFILEOPEN) printstr("Bad File Name or File Error\n");
		else printstr("Cannot Write\n");
		if(filesystemretry()) return; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
	}
}

void newtext(void){
// �V�K�e�L�X�g�쐬
	unsigned char vk;
	if(edited && num){
		//�ŏI�ۑ���ɕҏW�ς݂̏ꍇ�A�ۑ��̊m�F
		cls();
		setcursorcolor(COLOR_NORMALTEXT);
		printstr("Save Editing File?\n");
		printstr("Save:[Enter] / Not Save:[ESC]\n");
		while(1){
			inputchar(); //1�����L�[���͑҂�
			vk=vkey & 0xff;
			if(vk==VK_RETURN || vk==VK_SEPARATOR){
				save_as(0); //���O��t���ĕۑ�
				break;
			}
			else if(vk==VK_ESCAPE) break;
		}
	}
	inittextbuf(); //�e�L�X�g�o�b�t�@������
	cursor_top(); //�J�[�\�����e�L�X�g�o�b�t�@�̐擪�ɐݒ�
	currentfile[0]=0; //��ƒ��t�@�C�����N���A
}

void msra(void){
// Make Self-Running Application �i���Ȏ��s�A�v���P�[�V�����̍쐬�j
// �ŏ��Ƀ\�[�X�t�@�C���𖼑O��t���ĕۑ�
// ����BASIC�V�X�e����HEX�t�@�C�����\�[�X�t�@�C�����̊g���q��HEX�ɂ������O�ŃR�s�[

	int er;
	unsigned char *ps,*pd;
	cls();
	setcursor(0,0,COLOR_NORMALTEXT);
	printstr("Make Self-Running Application\n\n");
	printstr("(Work on Root Directory)\n");

	//�J�����g�f�B���N�g����ϐ�cwdpath�ɃR�s�[
	while(1){
		if(FSgetcwd(cwdpath,PATHNAMEMAX)) break;
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Cannot Get Current Dir\n");
		if(filesystemretry()) return; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
	}
	while(1){
		//�J�����g�f�B���N�g�������[�g�ɕύX
		if(FSchdir((char *)ROOTDIR)==0) break;
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Cannot Change To Root Dir\n");
		if(filesystemretry()) return; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
	}
	//currentfile����tempfile�ɃR�s�[
	ps=currentfile;
	pd=tempfile;
	while(*ps!=0) *pd++=*ps++;
	*pd=0;

	while(1){
		setcursorcolor(COLOR_NORMALTEXT);
		printstr("Input File Name (xxx.BAS)\n");
		if(lineinput(tempfile,8+1+3)<0){
			//ESC�L�[�������ꂽ
			FSchdir(cwdpath); //�J�����g�f�B���N�g�������ɖ߂�
			return;
		}
		ps=tempfile;
		while(*ps!='.' && *ps!=0) ps++;
		if(ps+4>=tempfile+13 ||
			*ps!='.' ||
			(*(ps+1)!='b' && *(ps+1)!='B') ||
			(*(ps+2)!='a' && *(ps+2)!='A') ||
			(*(ps+3)!='s' && *(ps+3)!='S') ||
			*(ps+4)!=0){
				setcursorcolor(COLOR_ERRORTEXT);
				printstr("File Name Must Be xxx.BAS\n");
				continue;
		}
		if(overwritecheck(tempfile)) continue;
		printstr("Writing BASIC File\n");
		er=savetextfile(tempfile); //�t�@�C���ۑ��Aer:�G���[�R�[�h
		if(er==0) break;
		setcursorcolor(COLOR_ERRORTEXT);
		if(er==ERR_CANTFILEOPEN) printstr("Bad File Name or File Error\n");
		else printstr("Cannot Write\n");

		//�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
		if(filesystemretry()){
			if(FSchdir(cwdpath)){
				cwdpath[0]='\\';
				cwdpath[1]=0;
			}
			return;
		}
	}
	printstr("OK\n\n");
	FSremove(TEMPFILENAME); //���s���ɐ�������ꎞ�t�@�C�����폜
	//tempfile����currentfile�ɃR�s�[���ďI��
	ps=tempfile;
	pd=currentfile;
	while(*ps!=0) *pd++=*ps++;
	*pd=0;
	edited=0; //�ҏW�ς݃t���O�N���A
	// �g���q��HEX�ɂ���BASIC�V�X�e���t�@�C�����R�s�[
	*(ps-3)='H';
	*(ps-2)='E';
	*(ps-1)='X';
	if(overwritecheck(tempfile)) return;
	printstr("Copying\n");
	printstr(HEXFILE);
	printstr(" To ");
	printstr(tempfile);
	printstr("\nWait For A While");
	er=sdfilecopy(HEXFILE,tempfile);
	if(FSchdir(cwdpath)){
		cwdpath[0]='\\';
		cwdpath[1]=0;
	}
	if(er==0){
		printstr("\nDone");
		wait60thsec(120);//2�b�҂�
		return;
	}
	setcursorcolor(COLOR_ERRORTEXT);
	if(er==ERR_CANTFILEOPEN){
		printstr(HEXFILE);
		printstr(" Not Found\n");
	}
	else if(er==ERR_CANTWRITEFILE){
		printstr("Write Error\n");
	}
	setcursorcolor(COLOR_NORMALTEXT);
	printstr((unsigned char *)Message1);// Hit Any Key
	inputchar(); //1�������͑҂�
	return;
}
int fileload(void){
// SD�J�[�h����t�@�C����I�����ēǂݍ���
// currenfile[]�Ƀt�@�C�������L��
// �Ώۃt�@�C���g���q BAS�����TXT
// �߂�l�@0�F�ǂݍ��݂��s�����@-1�F�ǂݍ��݂Ȃ�
	int filenum,f,er;
	unsigned char *ps,*pd;
	unsigned char vk;
	int num_dir;//�f�B���N�g����

	//�t�@�C���̈ꗗ��SD�J�[�h����ǂݏo��
	cls();
	if(edited && num){
		//�ŏI�ۑ���ɕҏW�ς݂̏ꍇ�A�ۑ��̊m�F
		setcursorcolor(COLOR_NORMALTEXT);
		printstr("Save Program Before Load?\n");
		printstr("Save:[Enter] / Not Save:[ESC]\n");
		while(1){
			inputchar(); //1�����L�[���͑҂�
			vk=vkey & 0xff;
			if(vk==VK_RETURN || vk==VK_SEPARATOR){
				save_as(0); //���O��t���ĕۑ�
				break;
			}
			else if(vk==VK_ESCAPE) break;
		}
	}
	//�J�����g�f�B���N�g����ϐ�cwdpath�ɃR�s�[
	while(1){
		if(FSgetcwd(cwdpath,PATHNAMEMAX)) break;
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Cannot Get Current Dir\n");
		if(filesystemretry()) return -1; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
	}
	while(1){
		filenum=getfilelist(&num_dir); //�f�B���N�g���A�t�@�C�����ꗗ��ǂݍ���
		if(filenum==0){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr(".BAS or .TXT File Not Found\n");
			printstr((unsigned char *)Message1);// Hit Any Key
			inputchar(); //1�������͑҂�
			FSchdir(cwdpath);//�J�����g�f�B���N�g�������ɖ߂�
			return -1;
		}
		//�t�@�C���̑I��
		f=select_dir_file(filenum,num_dir,"Load");
		if(f==-3){
			//�ǂݍ��܂��ɏI��
			FSchdir(cwdpath);//�J�����g�f�B���N�g�������ɖ߂�
			return -1;
		}
		else if(f==-2){
			//�V�K�t�@�C���܂��̓t�@�C��������͂��ēǂݍ���
			er=loadtextfile(tempfile); //�e�L�X�g�o�b�t�@�Ƀt�@�C���ǂݍ���
			if(er==ERR_CANTFILEOPEN){
				//�t�@�C�������݂��Ȃ��ꍇ�A�V�K�e�L�X�g
				edited=0;
				newtext();
			}
			else if(er==ERR_FILETOOBIG){
				//�t�@�C���T�C�Y�G���[�̏ꍇ�A�I����ʂɖ߂�
				setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
				printstr("File Too Big                 ");
				wait60thsec(60);//1�b�҂�
				continue;
			}
			//currenfile[]�Ƀt�@�C�������R�s�[
			ps=tempfile;
			pd=currentfile;
			while(*ps) *pd++=*ps++;
			*pd=0;
			FSgetcwd(cwdpath,PATHNAMEMAX);//cwdpath���J�����g�f�B���N�g���̃p�X�ɕύX
			return 0;
		}
		else if(f<num_dir){
			//�V�K�f�B���N�g���܂��̓f�B���N�g���ύX���āA�ēx�t�@�C���ꗗ��ʂ�
			FSchdir(tempfile);
		}
		else{
			er=loadtextfile(filenames[f]); //�e�L�X�g�o�b�t�@�Ƀt�@�C���ǂݍ���
			if(er==0){
				//cwdpath[]�Acurrenfile[]�Ƀp�X�A�t�@�C�������R�s�[���ďI��
				FSgetcwd(cwdpath,PATHNAMEMAX);
				ps=filenames[f];
				pd=currentfile;
				while(*ps!=0) *pd++=*ps++;
				*pd=0;
				return 0;
			}
			setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
			if(er==ERR_CANTFILEOPEN) printstr("Cannot Open File             ");
			else if(er=ERR_FILETOOBIG) printstr("File Too Big                 ");
			wait60thsec(60);//1�b�҂�
		}
	}
}
void changewidth(void){
// 30�������[�h��40�������[�h�̐؂�ւ�
	if(twidth==WIDTH_X1) set_width(1);
	else set_width(0);
	cursor_top(); //�J�[�\�����e�L�X�g�o�b�t�@�̐擪�ɐݒ�
	redraw(); //�ĕ`��
}
void run(int test){
//KM-BASIC�R���p�C�������s
// test 0:�R���p�C���Ǝ��s�A0�ȊO:�R���p�C���݂̂ŏI��
	int er,er2;
	FSFILE *fp;
	unsigned int disptoppos,cursorpos;
	unsigned char widthmode;
	int i,edited1;
	_tbuf *bp;
	unsigned short ix;
	unsigned char *p;

	cls();
	setcursor(0,0,COLOR_NORMALTEXT);
	while(1){
		//�J�����g�f�B���N�g�������[�g�ɕύX
		if(FSchdir((char *)ROOTDIR)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To Root Dir\n");
			if(filesystemretry()) return; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
			continue;
		}
		//���[�g�f�B���N�g���̃p�X���ۑ��t�@�C���Ɏ��s���p�X��ۑ�
		fp=FSfopen(WORKDIRFILE,"w");
		if(fp==NULL){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Open Work Dir File\n");
			if(filesystemretry()){
				//�t�@�C���V�X�e���ď������A������߂��ꍇ�̓J�����g�f�B���N�g����߂�return����
				FSchdir(cwdpath);
				return;
			}
			continue;
		}
		for(p=cwdpath;*p;p++) ;
		er=FSfwrite(cwdpath,1,p-cwdpath+1,fp);
		FSfclose(fp);
		if(er!=p-cwdpath+1){
			FSremove(WORKDIRFILE);
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Write Work Dir File\n");
			if(filesystemretry()){
				//�t�@�C���V�X�e���ď������A������߂��ꍇ�̓J�����g�f�B���N�g����߂�return����
				FSchdir(cwdpath);
				return;
			}
			continue;
		}
		break;
	}
	while(1){
		//�J�����g�f�B���N�g�������ɖ߂�
		if(FSchdir(cwdpath)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To CWD\n");
			if(filesystemretry()) return; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
			continue;
		}
		//���s�p���n���t�@�C���ɕۑ�
		if(savetextfile(TEMPFILENAME)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Write To SD Card\n");
			if(filesystemretry()) return; //�t�@�C���V�X�e���ď������A������߂��ꍇ��return����
			continue;
		}
		break;
	}

	//�J�[�\���ʒu�A��ʕ\���ʒu�A��ʃ��[�h�̕ۑ�
	disptoppos=bpixtopos(disptopbp,disptopix);
	cursorpos=bpixtopos(cursorbp,cursorix);
	widthmode=twidth;
	edited1=edited; //�ҏW�ς݃t���O�̈ꎞ�ޔ�
	set_width(0);//30�������[�h�ɐݒ�

	// Enable Break key
	g_disable_break=0;
	//KM-BASIC���s
	er2=runbasic(TEMPFILENAME,test);

	stopPCG();//�V�X�e���t�H���g�ɖ߂�
	setcursorcolor(COLOR_NORMALTEXT);
	printchar('\n');
	printstr((unsigned char *)Message1);// Hit Any Key
	do ps2readkey(); //�L�[�o�b�t�@����ɂȂ�܂œǂݏo��
	while(vkey!=0);
	ps2mode(); //�L�[�{�[�h�L����
	inputchar(); //1�������͑҂�
	stop_music(); //���y�Đ���~
	init_composite(); //�p���b�g�������̂��߉�ʏ�����
	//��ʃ��[�h��߂�
	if(widthmode==WIDTH_X1) set_width(0);
	else set_width(1);

	FSgetcwd(cwdpath,PATHNAMEMAX);//�J�����g�f�B���N�g���p�X�ϐ���߂�
	while(1){
		//�J�����g�f�B���N�g�������[�g�ɕύX
		if(FSchdir((char *)ROOTDIR)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To Root Dir\n");
			filesystemretry(); //�t�@�C���V�X�e���ď�����
			continue;
		}
		//���[�g�f�B���N�g���̃p�X���ۑ��t�@�C������p�X����ǂݏo��
		fp=FSfopen(WORKDIRFILE,"r");
		if(fp==NULL){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Open Work Dir File\n");
			filesystemretry(); //�t�@�C���V�X�e���ď�����
			continue;
		}
		er=FSfread(cwdpath,1,PATHNAMEMAX,fp);
		FSfclose(fp);
		if(er<=0){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Read Work Dir File\n");
			filesystemretry(); //�t�@�C���V�X�e���ď�����
			continue;
		}
		FSremove(WORKDIRFILE); //�p�X���ۑ��t�@�C���폜
		break;
	}
	while(1){
		//�J�����g�f�B���N�g�������ɖ߂�
		if(FSchdir(cwdpath)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To CWD\n");
			filesystemretry(); //�t�@�C���V�X�e���ď�����
			continue;
		}
		//���s�p���n���t�@�C�����猳�ɖ߂�
		if(loadtextfile(TEMPFILENAME)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Load From SD Card\n");
			filesystemretry(); //�t�@�C���V�X�e���ď�����
			continue;
		}
		break;
	}
	if(er2<=0){
		//����I���܂��̓t�@�C���G���[�܂��̓����N�G���[�̏ꍇ
		//�J�[�\�������̈ʒu�ɐݒ�
		disptopbp=postobpix(disptoppos,&disptopix);
		cursorbp=postobpix(cursorpos,&cursorix);
	}
	else{
		//�R���p�C���G���[�̏ꍇ
		//�J�[�\�����G���[�s�ŉ�ʃg�b�v�Ɉړ�
		disptopbp=linetobpix(er2,&disptopix);
		cursorbp=disptopbp;
		cursorix=disptopix;
		cx=0;
		cx2=0;
		cy=0;
		line_no=er2;
		//�����ɂȂ�悤�X�N���[��
		for(i=0;i<EDITWIDTHY/2;i++){
			//��ʍs�������J�[�\������Ɉړ�
			bp=disptopbp;
			ix=disptopix;
			cursor_up();
			if(bp==disptopbp && ix==disptopix) break; //�ŏ�s�ňړ��ł��Ȃ������ꍇ������
		}
		for(;i>0;i--) cursor_down(); //����Y���W�܂ŃJ�[�\�������Ɉړ�
	}
	cursorbp1=NULL; //�͈͑I�����[�h����
	clipsize=0; //�N���b�v�{�[�h�N���A
	edited=edited1;
	FSremove(TEMPFILENAME);
}
void displaybottomline(void){
//�G�f�B�^�[��ʍŉ��s�̕\��
	unsigned char *p;
	unsigned char c;
	int t;
	p=cursor; //�J�[�\���ʒu�̑ޔ�
	c=cursorcolor;
	setcursor(0,WIDTH_Y-1,COLOR_BOTTOMLINE);
	if(shiftkeys() & CHK_SHIFT){
		printstr("NEW |MSRA|WDTH|TEST|");
		setcursorcolor(COLOR_ERRORTEXT);
		t=TBUFMAXSIZE-num;
		if(t==0) t=1;
		while(t<10000){
			printchar(' ');
			t*=10;
		}
		printstr("LEFT:");
		printnum(TBUFMAXSIZE-num);
	}
	else{
		printstr("LOAD|SAVE|    |RUN |");
		setcursorcolor(COLOR_ERRORTEXT);
		t=line_no;
		if(t==0) t=1;
		while(t<10000){
			printchar(' ');
			t*=10;
		}
		printstr("LINE:");
		printnum(line_no);
	}
	cursor=p; //�J�[�\���ʒu�߂�
	cursorcolor=c;
}
void normal_code_process(unsigned char k){
// �ʏ핶�����͏���
// k:���͂��ꂽ�����R�[�h
	int i;

	edited=1; //�ҏW�ς݃t���O
	if(insertmode || k=='\n' || cursorbp1!=NULL){ //�}�����[�h
		if(cursorbp1!=NULL) deletearea();//�I��͈͂��폜
		i=insertchar(cursorbp,cursorix,k,0);//�e�L�X�g�o�b�t�@�ɂP�����}��
		if(i>0){
			//�o�b�t�@�󂫂�����̂ɑ}�����s�̏ꍇ
			gabagecollect2(); //�S�̃K�x�[�W�R���N�V����
			i=insertchar(cursorbp,cursorix,k,0);//�e�L�X�g�o�b�t�@�ɂP�����}��
		}
		if(i==0) cursor_right();//��ʏ�A�o�b�t�@��̃J�[�\���ʒu��1���Ɉړ�
	}
	else{ //�㏑�����[�h
		i=overwritechar(cursorbp,cursorix,k,0);//�e�L�X�g�o�b�t�@�ɂP�����㏑��
		if(i>0){
			//�o�b�t�@�󂫂�����̂ɏ㏑���i�}���j���s�̏ꍇ
			//�i�s����o�b�t�@�Ō���ł͑}���j
			gabagecollect2(); //�S�̃K�x�[�W�R���N�V����
			i=overwritechar(cursorbp,cursorix,k,0);//�e�L�X�g�o�b�t�@�ɂP�����㏑��
		}
		if(i==0) cursor_right();//��ʏ�A�o�b�t�@��̃J�[�\���ʒu��1���Ɉړ�
	}
}
void control_code_process(unsigned char k,unsigned char sh){
// ���䕶�����͏���
// k:���䕶���̉��z�L�[�R�[�h
// sh:�V�t�g�֘A�L�[���

	save_cursor(); //�J�[�\���֘A�ϐ��ޔ��i�J�[�\���ړ��ł��Ȃ������ꍇ�߂����߁j
	switch(k){
		case VK_LEFT:
		case VK_NUMPAD4:
			 //�V�t�g�L�[�������Ă��Ȃ���Δ͈͑I�����[�h�����iNumLock�{�V�t�g�{�e���L�[�ł������j
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD4) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //�͈͑I�����[�h�łȂ���Δ͈͑I�����[�h�J�n
			if(sh & CHK_CTRL){
				//CTRL�{������Home
				cursor_home();
				break;
			}
			cursor_left();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//�͈͑I�����[�h�ŉ�ʃX�N���[�����������ꍇ
				if(cy1<EDITWIDTHY-1) cy1++; //�͈̓X�^�[�g�ʒu���X�N���[��
				else restore_cursor(); //�J�[�\���ʒu��߂��i��ʔ͈͊O�͈̔͑I���֎~�j
			}
			break;
		case VK_RIGHT:
		case VK_NUMPAD6:
			 //�V�t�g�L�[�������Ă��Ȃ���Δ͈͑I�����[�h�����iNumLock�{�V�t�g�{�e���L�[�ł������j
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD6) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //�͈͑I�����[�h�łȂ���Δ͈͑I�����[�h�J�n
			if(sh & CHK_CTRL){
				//CTRL�{�E����End
				cursor_end();
				break;
			}
			cursor_right();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//�͈͑I�����[�h�ŉ�ʃX�N���[�����������ꍇ
				if(cy1>0) cy1--; //�͈̓X�^�[�g�ʒu���X�N���[��
				else restore_cursor(); //�J�[�\���ʒu��߂��i��ʔ͈͊O�͈̔͑I���֎~�j
			}
			break;
		case VK_UP:
		case VK_NUMPAD8:
			 //�V�t�g�L�[�������Ă��Ȃ���Δ͈͑I�����[�h�����iNumLock�{�V�t�g�{�e���L�[�ł������j
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD8) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //�͈͑I�����[�h�łȂ���Δ͈͑I�����[�h�J�n
			cursor_up();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//�͈͑I�����[�h�ŉ�ʃX�N���[�����������ꍇ
				if(cy1<EDITWIDTHY-1) cy1++; //�͈̓X�^�[�g�ʒu���X�N���[��
				else restore_cursor(); //�J�[�\���ʒu��߂��i��ʔ͈͊O�͈̔͑I���֎~�j
			}
			break;
		case VK_DOWN:
		case VK_NUMPAD2:
			 //�V�t�g�L�[�������Ă��Ȃ���Δ͈͑I�����[�h�����iNumLock�{�V�t�g�{�e���L�[�ł������j
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD2) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //�͈͑I�����[�h�łȂ���Δ͈͑I�����[�h�J�n
			cursor_down();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//�͈͑I�����[�h�ŉ�ʃX�N���[�����������ꍇ
				if(cy1>0) cy1--; //�͈̓X�^�[�g�ʒu���X�N���[��
				else restore_cursor(); //�J�[�\���ʒu��߂��i��ʔ͈͊O�͈̔͑I���֎~�j
			}
			break;
		case VK_HOME:
		case VK_NUMPAD7:
			 //�V�t�g�L�[�������Ă��Ȃ���Δ͈͑I�����[�h�����iNumLock�{�V�t�g�{�e���L�[�ł������j
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD7) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //�͈͑I�����[�h�łȂ���Δ͈͑I�����[�h�J�n
			cursor_home();
			break;
		case VK_END:
		case VK_NUMPAD1:
			 //�V�t�g�L�[�������Ă��Ȃ���Δ͈͑I�����[�h�����iNumLock�{�V�t�g�{�e���L�[�ł������j
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD1) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //�͈͑I�����[�h�łȂ���Δ͈͑I�����[�h�J�n
			cursor_end();
			break;
		case VK_PRIOR: // PageUp�L�[
		case VK_NUMPAD9:
			 //�V�t�g�{PageUp�͖����iNumLock�{�V�t�g�{�u9�v�����j
			if((sh & CHK_SHIFT) && ((k!=VK_NUMPAD9) || ((sh & CHK_NUMLK)==0))) break;
			cursorbp1=NULL; //�͈͑I�����[�h����
			cursor_pageup();
			break;
		case VK_NEXT: // PageDown�L�[
		case VK_NUMPAD3:
			 //�V�t�g�{PageDown�͖����iNumLock�{�V�t�g�{�u3�v�����j
			if((sh & CHK_SHIFT) && ((k!=VK_NUMPAD3) || ((sh & CHK_NUMLK)==0))) break;
			cursorbp1=NULL; //�͈͑I�����[�h����
			cursor_pagedown();
			break;
		case VK_DELETE: //Delete�L�[
		case VK_DECIMAL: //�e���L�[�́u.�v
			edited=1; //�ҏW�ς݃t���O
			if(cursorbp1!=NULL) deletearea();//�I��͈͂��폜
			else deletechar(cursorbp,cursorix,0);
			break;
		case VK_BACK: //BackSpace�L�[
			edited=1; //�ҏW�ς݃t���O
			if(cursorbp1!=NULL){
				deletearea();//�I��͈͂��폜
				break;
			}
			if(cursorix==0 && cursorbp->prev==NULL) break; //�o�b�t�@�擪�ł͖���
			cursor_left();
			deletechar(cursorbp,cursorix,-1);
			break;
		case VK_INSERT:
		case VK_NUMPAD0:
			insertmode^=1; //�}�����[�h�A�㏑�����[�h��؂�ւ�
			break;
		case 'C':
			//CTRL+C�A�N���b�v�{�[�h�ɃR�s�[
			if(cursorbp1!=NULL && (sh & CHK_CTRL)) clipcopy();
			break;
		case 'X':
			//CTRL+X�A�N���b�v�{�[�h�ɐ؂���
			if(cursorbp1!=NULL && (sh & CHK_CTRL)){
				clipcopy();
				deletearea(); //�I��͈͂̍폜
				edited=1; //�ҏW�ς݃t���O
			}
			break;
		case 'V':
			//CTRL+V�A�N���b�v�{�[�h����\��t��
			if((sh & CHK_CTRL)==0) break;
			if(clipsize==0) break;
			edited=1; //�ҏW�ς݃t���O
			if(cursorbp1!=NULL){
				//�͈͑I�����Ă��鎞�͍폜���Ă���\��t��
				if(num-countarea()+clipsize<=TBUFMAXSIZE){ //�o�b�t�@�󂫗e�ʃ`�F�b�N
					deletearea();//�I��͈͂��폜
					clippaste();//�N���b�v�{�[�h�\��t��
				}
			}
			else{
				if(num+clipsize<=TBUFMAXSIZE){ //�o�b�t�@�󂫗e�ʃ`�F�b�N
					clippaste();//�N���b�v�{�[�h�\��t��
				}
			}
			break;
		case 'S':
			//CTRL+S�ASD�J�[�h�ɕۑ�
			if(num==0) break;
			if(sh & CHK_CTRL) save_as(1); //�㏑���ۑ�
			break;
		case 'O':
			//CTRL+O�A�t�@�C���ǂݍ���
			if(sh & CHK_CTRL){
				if(fileload()==0){ //�t�@�C����I�����ēǂݍ���
					//�ǂݍ��݂��s�����ꍇ�A�J�[�\���ʒu��擪��
					cursor_top();
				}
			}
			break;
		case 'N':
			//CTRL+N�A�V�K�쐬
			if(sh & CHK_CTRL) newtext();
			break;
		case VK_F1: //F1�L�[
			if(sh & CHK_SHIFT) newtext();//SHIFT+F1�L�[�@�V�K�쐬
			else{
				//�t�@�C���ǂݍ���
				if(fileload()==0){ //�t�@�C����I�����ēǂݍ���
					//�ǂݍ��݂��s�����ꍇ�A�J�[�\���ʒu��擪��
					cursor_top();
				}
			}
			break;
		case VK_F2: //F2�L�[
			if(num==0) break;
			if(sh & CHK_SHIFT) msra(); //create direct running file
			else save_as(0); //�t�@�C������t���ĕۑ�
			break;
		case VK_F3: //F3�L�[
			if(sh & CHK_SHIFT) changewidth(); //30�������[�h�A40�������[�h�؂�ւ�
			break;
		case VK_F4: //F4�L�[
			if(num==0) break;
			if(sh & CHK_SHIFT) run(1); //�R���p�C���e�X�g
			else run(0); //�R���p�C�������s
			break;
		case 'Z':
			//CTRL+Z�A�A���h�D
			if(sh & CHK_CTRL) undoexec();
			break;
	}
}
void texteditor(void){
//�e�L�X�g�G�f�B�^�[�{��
	unsigned char k1,k2,sh;
	FSFILE *fp;

	editormallocp=RAM;
	TextBuffer=(_tbuf *)editormalloc(sizeof(_tbuf)*TBUFMAXLINE);
	clipboard=editormalloc(WIDTH_X2*EDITWIDTHY);
	filebuf=editormalloc(FILEBUFSIZE);
	cwdpath=editormalloc(PATHNAMEMAX);
	filenames=(unsigned char (*)[])editormalloc(MAXFILENUM*13);
	undobuf=editormalloc(UNDOBUFSIZE);

//	TextBuffer=(_tbuf *)RAM;
//	clipboard=(unsigned char *)TextBuffer+sizeof(_tbuf)*TBUFMAXLINE;
//	filebuf=clipboard+WIDTH_X2*EDITWIDTHY;
//	cwdpath=filebuf+FILEBUFSIZE;
//	filenames=(unsigned char (*)[])(cwdpath+PATHNAMEMAX);
//	undobuf=(unsigned char *)filenames+MAXFILENUM*13;

	inittextbuf(); //�e�L�X�g�o�b�t�@������
	currentfile[0]=0; //��ƒ��t�@�C�����N���A
	cwdpath[0]='\\'; //�J�����g�f�B���N�g�������[�g�ɐݒ�
	cwdpath[1]=0;

	//���s�������ꎞ�t�@�C�����������ꍇ�͓ǂݍ���
	fp=FSfopen(WORKDIRFILE,"r");
	if(fp!=NULL){
		FSfread(cwdpath,1,PATHNAMEMAX,fp);
		FSfclose(fp);
		FSchdir(cwdpath);
		if(loadtextfile(TEMPFILENAME)==0){
			edited=1;
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Temporary File Loaded\n");
			printstr("Current Directory is\n");
			setcursorcolor(COLOR_DIR);
			printstr(cwdpath);
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("\nSave To SD Card If Necessary\n");
			setcursorcolor(COLOR_NORMALTEXT);
			printstr((unsigned char *)Message1); //Hit Any Key
			inputchar(); //1�������͑҂�
		}
		else{
			cwdpath[0]='\\'; //�J�����g�f�B���N�g�������[�g�ɐݒ�
			cwdpath[1]=0;
		}
	}
	cursor_top(); //�J�[�\�����e�L�X�g�o�b�t�@�̐擪�Ɉړ�
	insertmode=1; //0:�㏑���A1:�}��
	clipsize=0; //�N���b�v�{�[�h�N���A
	blinktimer=0; //�J�[�\���_�Ń^�C�}�[�N���A

	while(1){
		redraw();//��ʍĕ`��
		setcursor(cx,cy,COLOR_NORMALTEXT);
		getcursorchar(); //�J�[�\���ʒu�̕�����ޔ��i�J�[�\���_�ŗp�j
		while(1){
			//�L�[���͑҂����[�v
			wait60thsec(1);  //60����1�b�E�F�C�g
			blinkcursorchar(); //�J�[�\���_�ł�����
			k1=ps2readkey(); //�L�[�o�b�t�@����ǂݍ��݁Ak1:�ʏ핶�����͂̏ꍇASCII�R�[�h
			displaybottomline(); //��ʍŉ��s�Ƀt�@���N�V�����L�[�@�\�\��
			if(vkey) break;  //�L�[�������ꂽ�ꍇ���[�v���甲����
			if(cursorbp1==NULL) gabagecollect1(); //1�o�C�g�K�x�[�W�R���N�V�����i�͈͑I�����͂��Ȃ��j
		}
		resetcursorchar(); //�J�[�\�������̕����\���ɖ߂�
		k2=(unsigned char)vkey; //k2:���z�L�[�R�[�h
		sh=vkey>>8;             //sh:�V�t�g�֘A�L�[���
		if(k2==VK_RETURN || k2==VK_SEPARATOR) k1='\n'; //Enter�����͒P���ɉ��s��������͂Ƃ���
		if(k1) normal_code_process(k1); //�ʏ핶�������͂��ꂽ�ꍇ
		else control_code_process(k2,sh); //���䕶�������͂��ꂽ�ꍇ
		if(cursorbp1!=NULL && cx==cx1 && cy==cy1) cursorbp1=NULL;//�I��͈͂̊J�n�ƏI�����d�Ȃ�����͈͑I�����[�h����
 	}
}
