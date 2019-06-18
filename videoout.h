//#define WIDTH_X 30 // ������������
#define WIDTH_X1 30 // ������������1
#define WIDTH_X2 40 // ������������2(6�h�b�g�t�H���g���p��)
#define WIDTH_Y 27 // �c����������
//#define ATTROFFSET (WIDTH_X*WIDTH_Y) // VRAM��̃J���[�p���b�g�i�[�ʒu
#define ATTROFFSET1 (WIDTH_X1*WIDTH_Y) // VRAM��̃J���[�p���b�g�i�[�ʒu1
#define ATTROFFSET2 (WIDTH_X2*WIDTH_Y) // VRAM��̃J���[�p���b�g�i�[�ʒu2

#define G_X_RES 256 // �������𑜓x
#define G_Y_RES 224 // �c�����𑜓x
#define G_H_WORD (G_X_RES/4) // 1�s����̃��[�h��(16bit�P��)


// ���̓{�^���̃|�[�g�A�r�b�g��`
#define KEYPORT PORTB
#define KEYUP 0x0400
#define KEYDOWN 0x0080
#define KEYLEFT 0x0100
#define KEYRIGHT 0x0200
#define KEYSTART 0x0800
#define KEYFIRE 0x4000

extern volatile char drawing;		//�@�\�����Ԓ���-1
extern volatile unsigned short drawcount;		//�@1��ʕ\���I�����Ƃ�1�����B�A�v������0�ɂ���B
							// �Œ�1��͉�ʕ\���������Ƃ̃`�F�b�N�ƁA�A�v���̏���������ʊ��ԕK�v���̊m�F�ɗ��p�B
extern unsigned char TVRAM[]; //�e�L�X�g�r�f�I������
extern unsigned short *gVRAM; //�O���t�B�b�NVRAM�J�n�ʒu�̃|�C���^

extern const unsigned char FontData[],FontData2[]; //�t�H���g�p�^�[����`
extern unsigned char *cursor;
extern unsigned char cursorcolor;
extern unsigned char *fontp;
extern unsigned char twidth; //�e�L�X�g1�s�������i30 or 40�j

void start_composite(void); //�J���[�R���|�W�b�g�o�͊J�n
void stop_composite(void); //�J���[�R���|�W�b�g�o�͒�~
void init_composite(void); //�J���[�R���|�W�b�g�o�͏�����
void clearscreen(void); //�e�L�X�g��ʃN���A
void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g); //�e�L�X�g�p���b�g�ݒ�
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g); //�o�b�N�O�����h�J���[�ݒ�
void set_graphmode(unsigned char m); //�O���t�B�b�N���[�h�ύX
void init_graphic(unsigned short *gvram); //�O���t�B�b�N�@�\���p����
void set_width(unsigned char m); //30�������[�h(8�h�b�g�t�H���g)��40�������[�h(6�h�b�g�t�H���g)�̐؂�ւ�

void vramscroll(void);
	//1�s�X�N���[��
void vramscrolldown(void);
	//1�s�t�X�N���[��
void setcursor(unsigned char x,unsigned char y,unsigned char c);
	//�J�[�\���ʒu�ƃJ���[��ݒ�
void setcursorcolor(unsigned char c);
	//�J�[�\���ʒu���̂܂܂ŃJ���[�ԍ���c�ɐݒ�
void printchar(unsigned char n);
	//�J�[�\���ʒu�Ƀe�L�X�g�R�[�hn��1�����\�����A�J�[�\����1�����i�߂�
void printstr(unsigned char *s);
	//�J�[�\���ʒu�ɕ�����s��\��
void printnum(unsigned int n);
	//�J�[�\���ʒu�ɕ����Ȃ�����n��10�i���\��
void printnum2(unsigned int n,unsigned char e);
	//�J�[�\���ʒu�ɕ����Ȃ�����n��e����10�i���\���i�O�̋󂫌������̓X�y�[�X�Ŗ��߂�j
void cls(void);
	//�e�L�X�g��ʏ������A�J�[�\����擪�Ɉړ�
void startPCG(unsigned char *p,int a);
	// RAM�t�H���g�iPCG�j�̗��p�J�n�Ap���t�H���g�i�[�ꏊ�Aa��0�ȊO�ŃV�X�e���t�H���g���R�s�[
void stopPCG(void);
	// RAM�t�H���g�iPCG�j�̗��p��~

void g_clearscreen(void);
//�O���t�B�b�N��ʃN���A
void g_set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g);
//�O���t�B�b�N�p�J���[�p���b�g�ݒ�

void g_pset(int x,int y,unsigned int c);
// (x,y)�̈ʒu�ɃJ���[c�œ_��`��

void g_putbmpmn(int x,int y,char m,char n,const unsigned char bmp[]);
// ��m*�cn�h�b�g�̃L�����N�^�[�����Wx,y�ɕ\��
// unsigned char bmp[m*n]�z��ɁA�P���ɃJ���[�ԍ�����ׂ�
// �J���[�ԍ���0�̕����͓����F�Ƃ��Ĉ���

void g_clrbmpmn(int x,int y,char m,char n);
// �cm*��n�h�b�g�̃L�����N�^�[����
// �J���[0�œh��Ԃ�

void g_gline(int x1,int y1,int x2,int y2,unsigned int c);
// (x1,y1)-(x2,y2)�ɃJ���[c�Ő�����`��

void g_hline(int x1,int x2,int y,unsigned int c);
// (x1,y)-(x2,y)�ւ̐������C���������`��

void g_circle(int x0,int y0,unsigned int r,unsigned int c);
// (x0,y0)�𒆐S�ɁA���ar�A�J���[c�̉~��`��

void g_boxfill(int x1,int y1,int x2,int y2,unsigned int c);
// (x1,y1),(x2,y2)��Ίp���Ƃ���J���[c�œh��ꂽ�����`��`��

void g_circlefill(int x0,int y0,unsigned int r,unsigned int c);
// (x0,y0)�𒆐S�ɁA���ar�A�J���[c�œh��ꂽ�~��`��

void g_putfont(int x,int y,unsigned int c,int bc,unsigned char n);
//8*8�h�b�g�̃A���t�@�x�b�g�t�H���g�\��
//���W�ix,y)�A�J���[�ԍ�c
//bc:�o�b�N�O�����h�J���[�A�����̏ꍇ����
//n:�����ԍ�

void g_printstr(int x,int y,unsigned int c,int bc,unsigned char *s);
//���W(x,y)����J���[�ԍ�c�ŕ�����s��\���Abc:�o�b�N�O�����h�J���[

void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n);
//���W(x,y)�ɃJ���[�ԍ�c�Ő��ln��\���Abc:�o�b�N�O�����h�J���[

void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e);
//���W(x,y)�ɃJ���[�ԍ�c�Ő��ln��\���Abc:�o�b�N�O�����h�J���[�Ae���ŕ\��

unsigned int g_color(int x,int y);
//���W(x,y)��VRAM��̌��݂̃p���b�g�ԍ���Ԃ��A��ʊO��0��Ԃ�
