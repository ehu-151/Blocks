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

// ゲーム画面サイズの設定
//
//! スクリーン幅
const int kScreenWidth = 600;
//! スクリーン高さ
const int kScreenHeight = 600;

//! テクスチャ数
const int kNumTextures = 10;
//! テクスチャ画像の横幅
const int kTextureWidth = 256;
//! テクスチャ画像の高さ
const int kTextureHeight = 256;
//! テクスチャマップ保存用配列
GLubyte textureImage[kNumTextures][kTextureHeight][kTextureWidth][4]; //最後の"4"は，R, G, B, Aの4成分に対応
//! テクスチャ識別子
GLuint textureID[kNumTextures];

/**
* @fn bool loadTexture(int index, const char* filename)
* @brief テクスチャ画像ファイルのロード
* @param [in] index テクスチャインデクス
* @param [in] filename ファイル名
* @retval true ロード成功
* @retval false ロード失敗
*/
bool loadTexture(int index, const char* filename)
{
	// テクスチャマップをファイルから読み込み
	FILE* ftex = fopen(filename, "rb");
	if (ftex == nullptr) // ファイルロード失敗
	{
		return false;
	}
	for (int h = 0; h < kTextureHeight; ++h)
	{
		for (int w = 0; w < kTextureWidth; ++w)
		{
			// R, G, B & アルファ成分をファイルから読み出し
			for (int i = 0; i < 4; ++i)
			{
				fread(&textureImage[index][h][w][i], sizeof(unsigned char), 1, ftex);
			}
		}
	}
	fclose(ftex);

	// テクスチャオブジェクトの作成
	glBindTexture(GL_TEXTURE_2D, textureID[index]);
	// テクスチャの割り当て 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kTextureWidth, kTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage[index]);
	// テクスチャマップのデータ格納形式の指定
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// テクスチャの繰り返し方法の指定 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// 色の調整（環境の設定）
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

/**
* @fn void setupTextures()
* @brief 全テクスチャ画像の初期設定
*/
void setupTextures()
{
	// テクスチャ画像を生成
	glGenTextures(kNumTextures, textureID); //テクスチャオブジェクトの名前付け

	// TODO: 必要なテクスチャファイルをここでロード
	// e.g.
	//loadTexture(0, "color.raw");	//playerbox
	loadTexture(1, "char.raw");
}

//! 前回アップデート時の時間
static uint64_t prevTime;
/**
* @fn uint64_t getTimestampUS()
* @brief 時刻をマイクロ秒単位で取得
* @retval 現在時刻（マイクロ秒）
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
* @brief プレイヤーが操作するブロック
*/
struct PlayerBox
{
	//! ブロックの中心X座標
	int centerX;
	//! ブロックの中心Y座標
	int centerY;
	//! ブロックの横幅/2
	int width_2;
	//! ブロックの高さ/2
	int height_2;
	//! ブロック残機(ストック数)
	int stock;

	/**
	* @fn PlayerBox::PlayerBox()
	* @brief デフォルトコンストラクタ
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
	* @brief 初期化メンバ関数
	*/
	void init()
	{
	}
	/**
	* @fn void PlayerBox::draw()
	* @brief 描画メンバ関数
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
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	void update(double dt)
	{
		
	}
};

/**
* @struct Block
* @brief ブロック
*/
struct Block
{
	//! ブロック中心X座標
	int centerX;
	//! ブロック中心Y座標
	int centerY;
	//! ブロック横幅/2
	int width_2;
	//! ブロック高さ/2
	int height_2;
	//! 可視性
	double visibility;

	/**
	* @fn Block::Block()
	* @brief デフォルトコンストラクタ
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
	* @brief 初期化メンバ関数
	*/
	void init()
	{
	}
	/**
	* @fn void Block::draw()
	* @brief 描画メンバ関数
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
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
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
* @brief ボールクラス
*/
struct Ball
{
	//! ボール中心X座標
	double centerX;
	//! ボール中心Y座標
	double centerY;
	//! ボール進行X方向(-1 or 1)
	double dirX;
	//! ボール進行Y方向(-1 or 1)
	double dirY;
	//! ボール半径
	double radius;
	//! ボール移動速度
	double speed;
	/**
	* @fn Ball::Ball()
	* @brief デフォルトコンストラクタ
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
	* @brief 初期化メンバ関数
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
	* @brief 描画メンバ関数
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
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	void update(double dt)
	{
		centerX += dt * speed * dirX;
		centerY += dt * speed * dirY;
	}
};

/**
* @struct KeyboardState
* @brief キーボードステートクラス
*/
struct KeyboardState
{
	//! "a"が押されたか
	bool is_a;
	//! "d"が押されたか
	bool is_d;
	//! "w"が押されたか
	bool is_w;
	//! "s"が押されたか
	bool is_s;
	//! " "が押されたか
	bool is_space;

	/**
	* @fn KeyboardState:KeyBoardState()
	* @brief デフォルトコンストラクタ
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
	* @brief 初期化メンバ関数
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
	* @brief 描画メンバ関数
	*/
	void draw()
	{
	}
	/**
	* @fn void KeyboardState::update(double dt)
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	void update(double dt)
	{
	}
};

//! プレイヤーブロックインスタンス
PlayerBox player;
//! ボールインスタンス
Ball ball;
//! ブロックの総数
const int kNumBlocks = 25;
//! ブロックインスタンス
Block blocks[kNumBlocks];
//! キーボードステートインスタンス
KeyboardState keyboardState;

/**
* @fn void initializeGame()
* @brief ゲーム環境の初期化
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
* @brief ゲームクリアチェック
* @retval true クリア
* @retval false ゲーム進行中
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
* @brief ゲームオーバーチェック
* @retval true ゲームオーバー
* @retval false ゲーム続行中
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
* @brief 壁との衝突判定
* @param [in,out] ball ボール
* @retval true 衝突あり
* @retval false 衝突なし
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
* @brief プレイヤーブロックとの衝突判定
* @param [in,out] ball ボール
* @param [in] player プレイヤーブロック
* @retval true 衝突あり
* @retval false 衝突なし
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
* @brief ブロックとの衝突判定
* @param [in,out] ball ボール
* @param [in,out] block 判定対象ブロック
* @retval true 衝突あり
* @retval false 衝突なし
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
* @brief ゲーム状態更新
* @param [in] dt 経過時間（秒）
*/
void update(double dt)
{
	ball.update(dt);
	player.update(dt);
	for (int i = 0; i < kNumBlocks; ++i)
	{
		blocks[i].update(dt);
	}

	//ミス処理
	if (isMiss()){
		ball.init();//ボールの初期化(位置、スピード)
		player.stock -= 1;//ストックを減らす
	}
	//ゲームオバー処理
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

	//ボールの位置更新
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
* @brief 描画イベントハンドラ
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
* @brief キードードイベントハンドラ
* @param [in] key キーコード
* @param [in] x マウスカーソルX座標
* @param [in] y マウスカーソルY座標
*/
void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	// "a"が押された時
	case 'a':
		keyboardState.is_a = true;
		break;
		// "d"が押された時
	case 'd':
		keyboardState.is_d = true;
		break;
		// "w"が押された時
	case 'w':
		keyboardState.is_w = true;
		break;
		// "s"が押された時
	case 's':
		keyboardState.is_s = true;
		break;
		// " "が押された時
	case ' ':
		keyboardState.is_space = true;
		break;
		// "q","Q"が押された時
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
		// "a"が離された時
	case 'a':
		keyboardState.is_a = false;
		break;
		// "d"が離された時
	case 'd':
		keyboardState.is_d = false;
		break;
		// "w"が離された時
	case 'w':
		keyboardState.is_w = false;
		break;
		// "s"が離された時
	case 's':
		keyboardState.is_s = false;
		break;
		// " "が離された時
	case ' ':
		keyboardState.is_space = false;
		break;
	default:
		break;
	
	}
}

/**
* @fn void onMouseMotion(int x, int y)
* @brief マウスドラッグイベントハンドラ
* @param [in] x マウスカーソルX座標
* @param [in] y マウスカーソルY座標
*/
void onMouseMotion(int x, int y)
{
	//player.centerX = x;
	//player.centerY = kScreenHeight - y;
}

/**
* @fn void onPassiveMotion(int x, int y)
* @brief マウス移動イベントハンドラ
* @param [in] x マウスカーソルX座標
* @param [in] y マウスカーソルY座標
*/
void onPassiveMotion(int x, int y)
{
	//player.centerX = x;
	//player.centerY = kScreenHeight - y;
}

/**
* @fn void onIdle()
* @brief アイドル状態イベントハンドラ
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
	// GLUTフレームワークの初期化（おまじない）
	glutInit(&argc, argv);
	// 表示ウィンドウのサイズを設定
	glutInitWindowSize(kScreenWidth, kScreenHeight);
	// 表示モードの設定（おまじない）
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	// ウィンドウの表示
	glutCreateWindow("blocks");
	// イベントハンドラの設定
	glutDisplayFunc(onDisplay);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);// 押したとき
	glutKeyboardUpFunc(onKeyboardUP);//離したとき
	glutMotionFunc(onMouseMotion);
	glutPassiveMotionFunc(onPassiveMotion);
	// ゲーム世界のサイズを設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, kScreenWidth, 0, kScreenHeight);
	// テクスチャデータのロード
	setupTextures();
	// ゲーム環境の初期化
	initializeGame();
	// ゲーム開始時刻の記録
	prevTime = getTimestampUS();
	// ゲームループ開始
	glutMainLoop();
	return 0;
}