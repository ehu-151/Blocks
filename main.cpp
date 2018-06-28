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
const int kScreenWidth = 300;
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
	// テクスチャマップのデータ格納形式の指定
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// テクスチャの割り当て 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kTextureWidth, kTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage[index]);
	// テクスチャの繰り返し方法の指定 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// 色の調整（環境の設定）
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textureID[index]);
	return true;
}

/**
* @fn void setupTextures()
* @brief 全テクスチャ画像の初期設定
*/
void setupTextures()
{
	// テクスチャ画像を生成
	glGenTextures(2, textureID); //テクスチャオブジェクトの名前付け

	// TODO: 必要なテクスチャファイルをここでロード
	// e.g.
	loadTexture(0, "block.bmp");	//playerbox
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

//! キーボードステートインスタンス
KeyboardState keyboardState;

/**
* @fn void initializeGame()
* @brief ゲーム環境の初期化
*/
void initializeGame()
{
	//initする
}

/**
* @fn bool isGameCleared()
* @brief ゲームクリアチェック
* @retval true クリア
* @retval false ゲーム進行中
*/
bool isGameCleared()
{
	//クリア条件
	return false;
}

/**
* @fn bool isGameOvered()
* @brief ゲームオーバーチェック
* @retval true ゲームオーバー
* @retval false ゲーム続行中
*/

bool isMiss()
{
	return false;
}
bool isGameOvered()
{
	return false;
}
// チェック処理

/**
* @fn void update(double dt)
* @brief ゲーム状態更新
* @param [in] dt 経過時間（秒）
*/
void update(double dt)
{
	//update処理
}

/**
* @fn void onDisplay()
* @brief 描画イベントハンドラ
*/
void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// TODO: .draw
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

void onKeyboardUP(unsigned char key, int x, int y) {
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

}

/**
* @fn void onPassiveMotion(int x, int y)
* @brief マウス移動イベントハンドラ
* @param [in] x マウスカーソルX座標
* @param [in] y マウスカーソルY座標
*/
void onPassiveMotion(int x, int y)
{

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