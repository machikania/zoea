/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka
   http://www.ze.em-net.ne.jp/~kenken/index.html
*/

#define TBUFMAXLINE 201 //�e�L�X�g�o�b�t�@��

#define TBUFSIZE 200 //�e�L�X�g�o�b�t�@1�̃T�C�Y
#define TBUFMAXSIZE (TBUFSIZE*(TBUFMAXLINE-1)) //�ő�o�b�t�@�e�ʁi�o�b�t�@1�s���󂯂�j
//#define EDITWIDTHX 30 //�G�f�B�^��ʉ���
#define EDITWIDTHY 26 //�G�f�B�^��ʏc��
#define COLOR_NORMALTEXT 7 //�ʏ�e�L�X�g�F
#define COLOR_ERRORTEXT 4 //�G���[���b�Z�[�W�e�L�X�g�F
#define COLOR_AREASELECTTEXT 4 //�͈͑I���e�L�X�g�F
#define COLOR_BOTTOMLINE 5 //��ʍŉ��s�̐F
#define COLOR_DIR 6 //�f�B���N�g�����\���̐F
#define FILEBUFSIZE 256 //�t�@�C���A�N�Z�X�p�o�b�t�@�T�C�Y
#define MAXFILENUM 200 //���p�\�t�@�C���ő吔
#define PATHNAMEMAX 128 //���[�L���O�f�B���N�g���p�X���̍ő�l
#define UNDOBUFSIZE 2048 //�A���h�D�p�o�b�t�@�T�C�Y

#define ERR_FILETOOBIG -1
#define ERR_CANTFILEOPEN -2
#define ERR_CANTWRITEFILE -3

#define TEMPFILENAME "~TEMP.BAS" //���s���\�[�X�ۑ��t�@�C����
#define WORKDIRFILE "~WORKDIR.TMP" //���s���p�X�ۑ��t�@�C����

#define UNDO_INSERT 1
#define UNDO_OVERWRITE 2
#define UNDO_DELETE 3
#define UNDO_BACKSPACE 4
#define UNDO_CONTINS 5
#define UNDO_CONTDEL 6

void texteditor(void); //�e�L�X�g�G�f�B�^�{��
int runbasic(char *s,int test); //�R���p�C�����Ď��s
extern unsigned char tempfile[13];
void wait60thsec(unsigned short n);
