#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef WIN32
#include <windows.h>
#include <GL/glut.h>
#else
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#endif
#include <time.h>

// �Q�[����ʃT�C�Y�̐ݒ�
//
//! �X�N���[����
const int kScreenWidth = 400;
//! �X�N���[������
const int kScreenHeight = 600;

//! �e�N�X�`����
const int kNumTextures = 10;
//! �e�N�X�`���摜�̉���
const int kTextureWidth = 256;
//! �e�N�X�`���摜�̍���
const int kTextureHeight = 256;
//! �e�N�X�`���}�b�v�ۑ��p�z��
GLubyte textureImage[kNumTextures][kTextureHeight][kTextureWidth][4]; //�Ō��"4"�́CR, G, B, A��4�����ɑΉ�
//! �e�N�X�`�����ʎq
GLuint textureID[kNumTextures];

/**
* @fn bool loadTexture(int index, const char* filename)
* @brief �e�N�X�`���摜�t�@�C���̃��[�h
* @param [in] index �e�N�X�`���C���f�N�X
* @param [in] filename �t�@�C����
* @retval true ���[�h����
* @retval false ���[�h���s
*/
bool loadTexture(int index, const char* filename)
{
	// �e�N�X�`���}�b�v���t�@�C������ǂݍ���
	FILE* ftex = fopen(filename, "rb");
	if (ftex == nullptr) // �t�@�C�����[�h���s
	{
		return false;
	}
	for (int h = 0; h < kTextureHeight; ++h)
	{
		for (int w = 0; w < kTextureWidth; ++w)
		{
			// R, G, B & �A���t�@�������t�@�C������ǂݏo��
			for (int i = 0; i < 4; ++i)
			{
				fread(&textureImage[index][h][w][i], sizeof(unsigned char), 1, ftex);
			}
		}
	}
	fclose(ftex);

	// �e�N�X�`���I�u�W�F�N�g�̍쐬
	glBindTexture(GL_TEXTURE_2D, textureID[index]);
	// �e�N�X�`���}�b�v�̃f�[�^�i�[�`���̎w��
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// �e�N�X�`���̊��蓖�� 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kTextureWidth, kTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage[index]);
	// �e�N�X�`���̌J��Ԃ����@�̎w�� 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// �e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// �F�̒����i���̐ݒ�j
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textureID[index]);
	return true;
}

/**
* @fn void setupTextures()
* @brief �S�e�N�X�`���摜�̏����ݒ�
*/
void setupTextures()
{
	// �e�N�X�`���摜�𐶐�
	glGenTextures(2, textureID); //�e�N�X�`���I�u�W�F�N�g�̖��O�t��

	// TODO: �K�v�ȃe�N�X�`���t�@�C���������Ń��[�h
	// e.g.
	loadTexture(0, "color.raw");	//playerbox
	loadTexture(1, "char.raw");
}

//! �O��A�b�v�f�[�g���̎���
static uint64_t prevTime;
/**
* @fn uint64_t getTimestampUS()
* @brief �������}�C�N���b�P�ʂŎ擾
* @retval ���ݎ����i�}�C�N���b�j
*/
uint64_t getTimestampUS()
{
#ifdef WIN32
	LARGE_INTEGER freq, t;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t);
	return t.QuadPart * 1e6 / freq.QuadPart;
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t absoluteUs =
		static_cast<int64_t>(ts.tv_sec) * 1000000
		+ static_cast<int64_t>(ts.tv_nsec) / 1000;
	return absoluteUs;
#endif
}

/**
* @struct TrafficLines
* @brief �Ԑ��N���X
*/
struct TrafficLines
{
	//! �u���b�N�̒��SX���W
	int centerX;
	//! �u���b�N�̒��SY���W
	int centerY;
	//! �u���b�N�̉���/2
	int width_2;
	//! �u���b�N�̍���/2
	int height_2;

	TrafficLines()
	{
		centerX = 0;
		centerY = 0;
		width_2 = 5;
		height_2 = 300;
	}
	void init()
	{

	}
	/**
	* @fn void TrafficLines::draw()
	* @brief �`�惁���o�֐�
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor3d(1.0, 1.0, 1.0);
		//�O�����Ԑ�
		centerX = 95;
		centerY = 300;
		glBegin(GL_POLYGON);
		glVertex2i(centerX - width_2, centerY - height_2);
		glVertex2i(centerX - width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY - height_2);
		glEnd();
		//�O���E�Ԑ�
		centerX = 305;
		centerY = 300;
		glBegin(GL_POLYGON);
		glVertex2i(centerX - width_2, centerY - height_2);
		glVertex2i(centerX - width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY - height_2);
		glEnd();
		glDisable(GL_BLEND);
	}
	/**
	* @fn void TrafficLines::update(double dt)
	* @brief ��ԍX�V�����o�֐�
	* @param [in] dt ���ԍ����i�b�j
	*/
	void update(double dt)
	{
	}
};

/**
* @struct DottedLines
* @brief �j���N���X
*/
struct DottedLines
{
	//! �u���b�N�̒��SX���W
	double centerX;
	//! �u���b�N�̒��SY���W
	double centerY;
	//! �u���b�N�̉���/2
	double width_2;
	//! �u���b�N�̍���/2
	double height_2;
	//! ����
	double speed;

	DottedLines()
	{
		centerX = 0;
		centerY = 0;
		width_2 = 5;
		height_2 = 35;
		speed = 100;
	}
	void init()
	{

	}
	/**
	* @fn void DottedLines::draw()
	* @brief �`�惁���o�֐�
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor3d(1.0, 1.0, 1.0);
		//�O�����Ԑ�
		glBegin(GL_POLYGON);
		glVertex2i(centerX - width_2, centerY - height_2);
		glVertex2i(centerX - width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY - height_2);
		glEnd();
		glDisable(GL_BLEND);
	}
	/**
	* @fn void DottedLines::update(double dt)
	* @brief ��ԍX�V�����o�֐�
	* @param [in] dt ���ԍ����i�b�j
	*/
	void update(double dt)
	{
		centerY -= dt * speed;
		if (centerY <= -35)
		{
			centerY = 600 + 65;
		}
	}
};

/**
* @struct Wall
* @brief �ǃN���X
*/
struct Wall
{
	//! �u���b�N�̒��SX���W
	double centerX;
	//! �u���b�N�̒��SY���W
	double centerY;
	//! �u���b�N�̉���/2
	double width_2;
	//! �u���b�N�̍���/2
	double height_2;
	//! ����
	double speed;
	//! �u���b�N�̔z�u
	//int place[4][2]= { { 0,1 },{ 0,1 },{ 0,1 },{ 0,1 } };

	Wall()
	{
		centerX = 0;
		centerY = 0;
		width_2 = 35;
		height_2 = 5;
		speed = 100;
	}
	void init()
	{

	}
	/**
	* @fn void Wall::draw()
	* @brief �`�惁���o�֐�
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor3d(1.0, 0.0, 0.0);
		//�O�����Ԑ�
		glBegin(GL_POLYGON);
		glVertex2i(centerX - width_2, centerY - height_2);
		glVertex2i(centerX - width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY - height_2);
		glEnd();
		glDisable(GL_BLEND);
	}
	/**
	* @fn void Wall::update(double dt)
	* @brief ��ԍX�V�����o�֐�
	* @param [in] dt ���ԍ����i�b�j
	*/
	void update(double dt)
	{
		centerY -= dt * speed;
		if (centerY <= -5)
		{
			centerY = 600 + 25;
		}
	}
};

/**
* @struct KeyboardState
* @brief �L�[�{�[�h�X�e�[�g�N���X
*/
struct KeyboardState
{
	//! "a"�������ꂽ��
	bool is_a;
	//! "d"�������ꂽ��
	bool is_d;
	//! "w"�������ꂽ��
	bool is_w;
	//! "s"�������ꂽ��
	bool is_s;
	//! " "�������ꂽ��
	bool is_space;

	/**
	* @fn KeyboardState:KeyBoardState()
	* @brief �f�t�H���g�R���X�g���N�^
	*/
	KeyboardState()
	{
		is_a = false;
		is_d = false;
		is_w = false;
		is_s = false;
		is_space = false;
	}
	/**
	* @fn void KeyboardState::init()
	* @brief �����������o�֐�
	*/
	void init()
	{
		is_a = false;
		is_d = false;
		is_w = false;
		is_s = false;
		is_space = false;
	}
	/**
	* @fn void KeyboardState::draw()
	* @brief �`�惁���o�֐�
	*/
	void draw()
	{
	}
	/**
	* @fn void KeyboardState::update(double dt)
	* @brief ��ԍX�V�����o�֐�
	* @param [in] dt ���ԍ����i�b�j
	*/
	void update(double dt)
	{
	}
};

//! �L�[�{�[�h�X�e�[�g�C���X�^���X
KeyboardState keyboardState;
//! �Ԑ��C���X�^���X
TrafficLines trafficLines;
//! �j���̐�
const int kNumDottedLine = 10;
//! �j���C���X�^���X
DottedLines dottedLines[kNumDottedLine];
//! �ǂ̍ő吔
const int kNumWall = 6;
//! �ǂ̃C���X�^���X
Wall wall[kNumWall];

/**
* @fn void initializeGame()
* @brief �Q�[�����̏�����
*/
void initializeGame()
{
	//init����
	for (int h = 0; h <5; ++h)
	{
		for (int w = 0; w < 2; ++w)
		{
			dottedLines[h * 2 + w].centerX = w * 70 + (160+ dottedLines[h * 2 + w].width_2);
			dottedLines[h * 2 + w].centerY = h * 140 + (0 + dottedLines[h * 2 + w].height_2);
		}
	}
	int place[3][2] = { { 0,1 },{ 1,2 },{ 0,1 } };
	//wall
	for (int h = 0; h < 3; h++)
	{
		for (int w = 0; w < 2; w++)
		{
			wall[h * 2 + w].centerX = place[h][w] * 70 + (130);
			wall[h * 2 + w].centerY = h * 210 + (210);
		}
	}

}

/**
* @fn bool isGameCleared()
* @brief �Q�[���N���A�`�F�b�N
* @retval true �N���A
* @retval false �Q�[���i�s��
*/
bool isGameCleared()
{
	//�N���A����
	return false;
}

/**
* @fn bool isGameOvered()
* @brief �Q�[���I�[�o�[�`�F�b�N
* @retval true �Q�[���I�[�o�[
* @retval false �Q�[�����s��
*/

bool isMiss()
{
	return false;
}
bool isGameOvered()
{
	return false;
}
// �`�F�b�N����

/**
* @fn void update(double dt)
* @brief �Q�[����ԍX�V
* @param [in] dt �o�ߎ��ԁi�b�j
*/
void update(double dt)
{
	//update����
	//�j��
	for (int i = 0; i < kNumDottedLine; i++)
	{
		dottedLines[i].update(dt);
	}
	for (int i = 0; i < kNumWall; i++)
	{
		wall[i].update(dt);
	}
}

/**
* @fn void onDisplay()
* @brief �`��C�x���g�n���h��
*/
void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// TODO: .draw
	trafficLines.draw();
	for (int i = 0; i < kNumDottedLine; i++)
	{
		dottedLines[i].draw();
	}
	for (int i = 0; i < kNumWall; i++)
	{
		wall[i].draw();
	}

	glutSwapBuffers();
	glFlush();
}

/**
* @fn void onKeyboard(unsigned char key, int x, int y)
* @brief �L�[�h�[�h�C�x���g�n���h��
* @param [in] key �L�[�R�[�h
* @param [in] x �}�E�X�J�[�\��X���W
* @param [in] y �}�E�X�J�[�\��Y���W
*/
void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		// "a"�������ꂽ��
	case 'a':
		keyboardState.is_a = true;
		break;
		// "d"�������ꂽ��
	case 'd':
		keyboardState.is_d = true;
		break;
		// "w"�������ꂽ��
	case 'w':
		keyboardState.is_w = true;
		break;
		// "s"�������ꂽ��
	case 's':
		keyboardState.is_s = true;
		break;
		// " "�������ꂽ��
	case ' ':
		keyboardState.is_space = true;
		break;
		// "q","Q"�������ꂽ��
	case 'q':
	case 'Q':
		exit(0);
		break;
	default:
		break;
	}
}

void onKeyboardUP(unsigned char key, int x, int y) {
	switch (key)
	{
		// "a"�������ꂽ��
	case 'a':
		keyboardState.is_a = false;
		break;
		// "d"�������ꂽ��
	case 'd':
		keyboardState.is_d = false;
		break;
		// "w"�������ꂽ��
	case 'w':
		keyboardState.is_w = false;
		break;
		// "s"�������ꂽ��
	case 's':
		keyboardState.is_s = false;
		break;
		// " "�������ꂽ��
	case ' ':
		keyboardState.is_space = false;
		break;
	default:
		break;

	}
}

/**
* @fn void onMouseMotion(int x, int y)
* @brief �}�E�X�h���b�O�C�x���g�n���h��
* @param [in] x �}�E�X�J�[�\��X���W
* @param [in] y �}�E�X�J�[�\��Y���W
*/
void onMouseMotion(int x, int y)
{

}

/**
* @fn void onPassiveMotion(int x, int y)
* @brief �}�E�X�ړ��C�x���g�n���h��
* @param [in] x �}�E�X�J�[�\��X���W
* @param [in] y �}�E�X�J�[�\��Y���W
*/
void onPassiveMotion(int x, int y)
{

}

/**
* @fn void onIdle()
* @brief �A�C�h����ԃC�x���g�n���h��
*/
void onIdle()
{
	uint64_t time = getTimestampUS();
	double dt = (time - prevTime) / 1.0e6;
	if (time - prevTime > 1.0e6 / 30.0)
	{
		update(dt);
		onDisplay();
		prevTime = time;
	}
}

int main(int argc, char* argv[])
{
	// GLUT�t���[�����[�N�̏������i���܂��Ȃ��j
	glutInit(&argc, argv);
	// �\���E�B���h�E�̃T�C�Y��ݒ�
	glutInitWindowSize(kScreenWidth, kScreenHeight);
	// �\�����[�h�̐ݒ�i���܂��Ȃ��j
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	// �E�B���h�E�̕\��
	glutCreateWindow("blocks");
	// �C�x���g�n���h���̐ݒ�
	glutDisplayFunc(onDisplay);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);// �������Ƃ�
	glutKeyboardUpFunc(onKeyboardUP);//�������Ƃ�
	glutMotionFunc(onMouseMotion);
	glutPassiveMotionFunc(onPassiveMotion);
	// �Q�[�����E�̃T�C�Y��ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, kScreenWidth, 0, kScreenHeight);
	// �e�N�X�`���f�[�^�̃��[�h
	setupTextures();
	// �Q�[�����̏�����
	initializeGame();
	// �Q�[���J�n�����̋L�^
	prevTime = getTimestampUS();
	// �Q�[�����[�v�J�n
	glutMainLoop();
	return 0;
}