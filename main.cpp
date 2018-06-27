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
const int kScreenWidth = 600;
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
	// �e�N�X�`���̊��蓖�� 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kTextureWidth, kTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage[index]);
	// �e�N�X�`���}�b�v�̃f�[�^�i�[�`���̎w��
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// �e�N�X�`���̌J��Ԃ����@�̎w�� 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// �e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// �F�̒����i���̐ݒ�j
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

/**
* @fn void setupTextures()
* @brief �S�e�N�X�`���摜�̏����ݒ�
*/
void setupTextures()
{
	// �e�N�X�`���摜�𐶐�
	glGenTextures(kNumTextures, textureID); //�e�N�X�`���I�u�W�F�N�g�̖��O�t��

	// TODO: �K�v�ȃe�N�X�`���t�@�C���������Ń��[�h
	// e.g.
	//loadTexture(0, "color.raw");	//playerbox
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
* @struct PlayerBox
* @brief �v���C���[�����삷��u���b�N
*/
struct PlayerBox
{
	//! �u���b�N�̒��SX���W
	int centerX;
	//! �u���b�N�̒��SY���W
	int centerY;
	//! �u���b�N�̉���/2
	int width_2;
	//! �u���b�N�̍���/2
	int height_2;
	//! �u���b�N�c�@(�X�g�b�N��)
	int stock;

	/**
	* @fn PlayerBox::PlayerBox()
	* @brief �f�t�H���g�R���X�g���N�^
	*/
	PlayerBox()
	{
		width_2 = 50;
		height_2 = 10;
		centerX = 50;
		centerY = 50;
		stock = 3;
	}
	/**
	* @fn void PlayerBox::init()
	* @brief �����������o�֐�
	*/
	void init()
	{
	}
	/**
	* @fn void PlayerBox::draw()
	* @brief �`�惁���o�֐�
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, textureID[0]);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_POLYGON);
		glTexCoord2f(0, 1);  glVertex2i(centerX - width_2, centerY - height_2);
		glTexCoord2f(0, 0);  glVertex2i(centerX - width_2, centerY + height_2);
		glTexCoord2f(1, 0);  glVertex2i(centerX + width_2, centerY + height_2);
		glTexCoord2f(1, 1);  glVertex2i(centerX + width_2, centerY - height_2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	/**
	* @fn void PlayerBox::update(double dt)
	* @brief ��ԍX�V�����o�֐�
	* @param [in] dt ���ԍ����i�b�j
	*/
	void update(double dt)
	{
		
	}
};

/**
* @struct Block
* @brief �u���b�N
*/
struct Block
{
	//! �u���b�N���SX���W
	int centerX;
	//! �u���b�N���SY���W
	int centerY;
	//! �u���b�N����/2
	int width_2;
	//! �u���b�N����/2
	int height_2;
	//! ����
	double visibility;

	/**
	* @fn Block::Block()
	* @brief �f�t�H���g�R���X�g���N�^
	*/
	Block()
	{
		visibility = 1.0;
		width_2 = 50;
		height_2 = 50;
		centerX = 50;
		centerY = 50;
	}
	/**
	* @fn void Block::init()
	* @brief �����������o�֐�
	*/
	void init()
	{
	}
	/**
	* @fn void Block::draw()
	* @brief �`�惁���o�֐�
	*/
	void draw()
	{
		if (visibility <= 0)
		{
			return;
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4d(1.0, 1.0, 1.0, visibility);
		glBegin(GL_POLYGON);
		glVertex2i(centerX - width_2, centerY - height_2);
		glVertex2i(centerX - width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY + height_2);
		glVertex2i(centerX + width_2, centerY - height_2);
		glEnd();
		glDisable(GL_BLEND);
	}
	/**
	* @fn void Block::update(double dt)
	* @brief ��ԍX�V�����o�֐�
	* @param [in] dt ���ԍ����i�b�j
	*/
	void update(double dt)
	{
		if (visibility > 0 && visibility < 1.0)
		{
			visibility -= dt * 2.0;
		}
	}
};

/**
* @struct Ball
* @brief �{�[���N���X
*/
struct Ball
{
	//! �{�[�����SX���W
	double centerX;
	//! �{�[�����SY���W
	double centerY;
	//! �{�[���i�sX����(-1 or 1)
	double dirX;
	//! �{�[���i�sY����(-1 or 1)
	double dirY;
	//! �{�[�����a
	double radius;
	//! �{�[���ړ����x
	double speed;
	/**
	* @fn Ball::Ball()
	* @brief �f�t�H���g�R���X�g���N�^
	*/
	Ball()
	{
		centerX = 0;
		centerY = 100;
		dirX = 1.0;
		dirY = 1.0;
		radius = 10.0;
		speed = 200;
	}
	/**
	* @fn void Ball::init()
	* @brief �����������o�֐�
	*/
	void init()
	{
		centerX = 0;
		centerY = 100;
		dirX = 1.0;
		dirY = 1.0;
		radius = 10.0;
		speed = 200;
	}
	/**
	* @fn void Ball:draw()
	* @brief �`�惁���o�֐�
	*/
	void draw()
	{
		glBegin(GL_POLYGON);
		glVertex2d(centerX - radius, centerY - radius);
		glVertex2d(centerX - radius, centerY + radius);
		glVertex2d(centerX + radius, centerY + radius);
		glVertex2d(centerX + radius, centerY - radius);
		glEnd();
	}
	/**
	* @fn void Ball::update(double dt)
	* @brief ��ԍX�V�����o�֐�
	* @param [in] dt ���ԍ����i�b�j
	*/
	void update(double dt)
	{
		centerX += dt * speed * dirX;
		centerY += dt * speed * dirY;
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

//! �v���C���[�u���b�N�C���X�^���X
PlayerBox player;
//! �{�[���C���X�^���X
Ball ball;
//! �u���b�N�̑���
const int kNumBlocks = 25;
//! �u���b�N�C���X�^���X
Block blocks[kNumBlocks];
//! �L�[�{�[�h�X�e�[�g�C���X�^���X
KeyboardState keyboardState;

/**
* @fn void initializeGame()
* @brief �Q�[�����̏�����
*/
void initializeGame()
{
	for (int h = 0; h < 5; ++h)
	{
		for (int w = 0; w < 5; ++w)
		{
			blocks[h * 5 + w].centerX = w * 100 + 100;
			blocks[h * 5 + w].centerY = h * 40 + 350;
			blocks[h * 5 + w].width_2 = 45;
			blocks[h * 5 + w].height_2 = 18;
			blocks[h * 5 + w].init();
		}
	}
	player.init();
	ball.init();
}

/**
* @fn bool isGameCleared()
* @brief �Q�[���N���A�`�F�b�N
* @retval true �N���A
* @retval false �Q�[���i�s��
*/
bool isGameCleared()
{
	for (int i = 0; i < kNumBlocks; ++i)
	{
		if (blocks[i].visibility >= 1.0)
		{
			return false;
		}
	}
	return true;
}

/**
* @fn bool isGameOvered()
* @brief �Q�[���I�[�o�[�`�F�b�N
* @retval true �Q�[���I�[�o�[
* @retval false �Q�[�����s��
*/

bool isMiss()
{
	return ball.centerY < ball.radius;
}
bool isGameOvered()
{
	return player.stock == 0;
}

/**
* @fn bool checkWall(Ball& ball)
* @brief �ǂƂ̏Փ˔���
* @param [in,out] ball �{�[��
* @retval true �Փ˂���
* @retval false �Փ˂Ȃ�
*/
bool checkWall(Ball& ball)
{
	if (ball.centerX + ball.radius > kScreenWidth)
	{
		ball.centerX = kScreenWidth - ball.radius;
		ball.dirX = -1;
		return true;
	}
	if (ball.centerX - ball.radius < 0)
	{
		ball.centerX = ball.radius;
		ball.dirX = +1;
		return true;
	}
	if (ball.centerY + ball.radius > kScreenHeight)
	{
		ball.centerY = kScreenHeight - ball.radius;
		ball.dirY = -1;
		return true;
	}
	if (ball.centerY - ball.radius < 0)
	{
		ball.centerY = ball.radius;
		ball.dirY = +1;
		return true;
	}
	return false;
}

/**
* @fn bool checkPlayer(Ball& ball, PlayerBox player)
* @brief �v���C���[�u���b�N�Ƃ̏Փ˔���
* @param [in,out] ball �{�[��
* @param [in] player �v���C���[�u���b�N
* @retval true �Փ˂���
* @retval false �Փ˂Ȃ�
*/
bool checkPlayer(Ball& ball, PlayerBox player)
{
	double xp = +(ball.centerX + ball.radius) - (player.centerX - player.width_2);
	double xn = -(ball.centerX - ball.radius) + (player.centerX + player.width_2);
	double yp = +(ball.centerY + ball.radius) - (player.centerY - player.height_2);
	double yn = -(ball.centerY - ball.radius) + (player.centerY + player.height_2);
	if (xp < 0 || xn < 0 || yp < 0 || yn <= 0)
	{
		return false;
	}
	if (ball.dirX > 0 && xp < xn && xp < yp && xp < yn)
	{
		ball.centerX = player.centerX - player.width_2 - ball.radius;
		ball.dirX = -1;
		return true;
	}
	else if (ball.dirX < 0 && xn < yp && xn < yn)
	{
		ball.centerX = player.centerX + player.width_2 + ball.radius;
		ball.dirX = +1;
		return true;
	}
	else if (ball.dirY > 0 && yp < yn)
	{
		ball.centerY = player.centerY - player.height_2 - ball.radius;
		ball.dirY = -1;
		return true;
	}
	else if (ball.dirY < 0)
	{
		ball.centerY = player.centerY + player.height_2 + ball.radius;
		ball.dirY = +1;
		return true;
	}
	return false;
}

/**
* @fn bool checkBlock(Ball& ball, Block& block)
* @brief �u���b�N�Ƃ̏Փ˔���
* @param [in,out] ball �{�[��
* @param [in,out] block ����Ώۃu���b�N
* @retval true �Փ˂���
* @retval false �Փ˂Ȃ�
*/
bool checkBlock(Ball& ball, Block& block)
{
	if (block.visibility < 1.0)
	{
		return false;
	}
	double xp = +(ball.centerX + ball.radius) - (block.centerX - block.width_2);
	double xn = -(ball.centerX - ball.radius) + (block.centerX + block.width_2);
	double yp = +(ball.centerY + ball.radius) - (block.centerY - block.height_2);
	double yn = -(ball.centerY - ball.radius) + (block.centerY + block.height_2);
	if (xp < 0 || xn < 0 || yp < 0 || yn <= 0)
	{
		return false;
	}
	if (ball.dirX > 0 && xp < xn && xp < yp && xp < yn)
	{
		ball.centerX = block.centerX - block.width_2 - ball.radius;
		ball.dirX = -1;
		block.visibility = 0.5;
		return true;
	}
	else if (ball.dirX < 0 && xn < yp && xn < yn)
	{
		ball.centerX = block.centerX + block.width_2 + ball.radius;
		ball.dirX = +1;
		block.visibility = 0.5;
		return true;
	}
	else if (ball.dirY > 0 && yp < yn)
	{
		ball.centerY = block.centerY - block.height_2 - ball.radius;
		ball.dirY = -1;
		block.visibility = 0.5;
		return true;
	}
	else if (ball.dirY < 0)
	{
		ball.centerY = block.centerY + block.height_2 + ball.radius;
		ball.dirY = +1;
		block.visibility = 0.5;
		return true;
	}
	return false;
}

/**
* @fn void update(double dt)
* @brief �Q�[����ԍX�V
* @param [in] dt �o�ߎ��ԁi�b�j
*/
void update(double dt)
{
	ball.update(dt);
	player.update(dt);
	for (int i = 0; i < kNumBlocks; ++i)
	{
		blocks[i].update(dt);
	}

	//�~�X����
	if (isMiss()){
		ball.init();//�{�[���̏�����(�ʒu�A�X�s�[�h)
		player.stock -= 1;//�X�g�b�N�����炷
	}
	//�Q�[���I�o�[����
	if (isGameOvered())
	{
		printf("Game Over...\n");
		exit(0);
	}

	checkPlayer(ball, player);
	for (int i = 0; i < kNumBlocks; ++i)
	{
		if (checkBlock(ball, blocks[i]))
		{
			break;
		}
	}
	checkWall(ball);

	if (isGameCleared())
	{
		printf("Game Clear!\n");
		exit(0);
	}

	//�{�[���̈ʒu�X�V
	if (keyboardState.is_a)
	{
		player.centerX -= dt * 300;
	}
	if (keyboardState.is_d)
	{
		player.centerX += dt * 300;
	}
	if (keyboardState.is_w)
	{
		player.centerY += dt * 300;
	}
	if (keyboardState.is_s)
	{
		player.centerY -= dt * 300;
	}
	if (keyboardState.is_space)
	{
		ball.speed = 100;
	}
	else
	{
		ball.speed = 200;
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
	for (int i = 0; i < kNumBlocks; ++i)
	{
		blocks[i].draw();
	}
	ball.draw();
	player.draw();
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

void onKeyboardUP(unsigned char key, int x, int y){
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
	//player.centerX = x;
	//player.centerY = kScreenHeight - y;
}

/**
* @fn void onPassiveMotion(int x, int y)
* @brief �}�E�X�ړ��C�x���g�n���h��
* @param [in] x �}�E�X�J�[�\��X���W
* @param [in] y �}�E�X�J�[�\��Y���W
*/
void onPassiveMotion(int x, int y)
{
	//player.centerX = x;
	//player.centerY = kScreenHeight - y;
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