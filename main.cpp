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
#include <string>

// ゲーム画面サイズの設定
//
//! スクリーン幅
const int kScreenWidth = 400;
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


//! ブロックの配置
int place[4][2] = { { 0,1 },{ 0,1 },{ 0,1 },{ 0,1 } };
//! 全体のスピード
double tempSpeed = 100;

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
	loadTexture(0, "color.raw");	//playerbox
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
* @struct TrafficLines
* @brief 車線クラス
*/
struct TrafficLines
{
	//! ブロックの中心X座標
	int centerX;
	//! ブロックの中心Y座標
	int centerY;
	//! ブロックの横幅/2
	int width_2;
	//! ブロックの高さ/2
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
	* @brief 描画メンバ関数
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor3d(1.0, 1.0, 1.0);
		
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
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	void update(double dt)
	{
	}
};

/**
* @struct DottedLines
* @brief 破線クラス
*/
struct DottedLines
{
	//! ブロックの中心X座標
	double centerX;
	//! ブロックの中心Y座標
	double centerY;
	//! ブロックの横幅/2
	double width_2;
	//! ブロックの高さ/2
	double height_2;
	//! 速さ
	double speed;

	DottedLines()
	{
		centerX = 0;
		centerY = 0;
		width_2 = 5;
		height_2 = 35;
		speed = tempSpeed;
	}
	void init()
	{

	}
	/**
	* @fn void DottedLines::draw()
	* @brief 描画メンバ関数
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor3d(1.0, 1.0, 1.0);

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
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
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
* @brief 壁クラス
*/
struct Wall
{
	//! ブロックの中心X座標
	double centerX;
	//! ブロックの中心Y座標
	double centerY;
	//! ブロックの横幅/2
	double width_2;
	//! ブロックの高さ/2
	double height_2;
	//! 速さ
	double speed;
	
	Wall()
	{
		centerX = 0;
		centerY = 0;
		width_2 = 35;
		height_2 = 5;
		speed = tempSpeed;
	}
	void init()
	{

	}
	/**
	* @fn void Wall::draw()
	* @brief 描画メンバ関数
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor3d(1.0, 0.0, 0.0);

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
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	double Uniform(void) {
		static int x = 10;
		int a = 1103515245, b = 12345, c = 2147483647;
		x = (a*x + b)&c;

		return ((double)x + 1.0) / ((double)c + 2.0);
	}
	void update(double dt,int i)
	{
		centerY -= dt * speed;
		// Wallのループ
		if (centerY+height_2 <= 0)
		{
			if (i == 0 || i == 2 || i == 4) {
				int intRand = (int)(rand()*(double)3 / (1.0 + RAND_MAX));;
				
				printf("%d", intRand);
				if (intRand == 0) {
					place[i / 2][0] = 1;
					place[i / 2][1] = 2;
				}
				else if (intRand == 1) {
					place[i / 2][0] = 2;
					place[i / 2][1] = 0;
				}
				else if (intRand == 2) {
					place[i / 2][0] = 0;
					place[i / 2][1] = 1;
				}
			}
			centerX = place[i/2][i%2] * 70 + (130);
			centerY = 600 + 25;
		}
	}
};

/**
* @struct Car
* @brief 車クラス
*/
struct Car
{
	//! ブロックの中心X座標
	double centerX;
	//! ブロックの中心Y座標
	double centerY;
	//! ブロックの横幅/2
	double width_2;
	//! ブロックの高さ/2
	double height_2;
	// 横移動
	double slide_speed;
	

	Car()
	{
		centerX = 200;
		centerY = 35 + 210;
		width_2 = 10;
		height_2 = 20;
		slide_speed = 5;
	}
	void init()
	{

	}
	/**
	* @fn void Wall::draw()
	* @brief 描画メンバ関数
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor3d(0.0, 0.0, 1.0);
		//外側左車線
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
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	void update(double dt)
	{
		
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

/**
* @struct DrawInt
* @brief 数値描画クラス
*/
struct DrawInt
{
	//! 文字の開始X軸位置
	int firstX;
	//! 文字の開始Y軸位置
	int firstY;
	//! 描画文字
	char str[50];
	//! score用数値
	int score;
	//! 更新するかしないか
	bool needLoad;
	//! 文字の色
	double color[3] = { 1.0, 1.0, 1.0 };

	DrawInt()
	{
		firstX = 0;
		firstY = 0;
		score = 0;
		needLoad = true;
	}
	// Colorを変える
	void setColor(double r, double g, double b) {
		color[0] = r;
		color[1] = g;
		color[2] = b;
	}

	void initString(char* mozi)
	{
		sprintf(str, "%s", mozi);
	}
	/**
	* @fn void Wall::draw()
	* @brief 描画メンバ関数
	*/
	void draw()
	{
		for (int i = 0; i < sizeof(str); i++)
		{
			glColor3d(color[0], color[1], color[2]);
			glRasterPos2i(firstX + (i * 10), firstY);
			int chara = (unsigned char)str[i];
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, chara);
		}
	}
	/**
	* @fn void DrawInt::update(double dt)
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	void update(double dt)
	{
		if (needLoad)
		{
			score = score + 1;
			sprintf(str, "%d", score);
		}
	}
};

/**
* @struct SlowWall
* @brief スロー壁クラス
*/
struct SlowWall
{
	//! ブロックの初めの位置
	double centerX;
	//! ブロックの初めの位置
	double centerY;
	//! ブロックの横幅/2
	double width_2;
	//! ブロックの下に伸びる高さ
	double height;
	//! 透明度
	double al;

	SlowWall()
	{
		centerX = 200;
		centerY = 600;
		width_2 = 105;
		height = 0;
		al = 1.0;
	}
	void init()
	{

	}
	/**
	* @fn void SlowWall::draw()
	* @brief 描画メンバ関数
	*/
	void draw()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4d(0.2, 0.2, 0.2, al);
		//外側左車線
		glBegin(GL_POLYGON);
		glVertex2i(centerX - width_2, centerY);
		glVertex2i(centerX - width_2, centerY - height);
		glVertex2i(centerX + width_2, centerY - height);
		glVertex2i(centerX + width_2, centerY);
		glEnd();
		glDisable(GL_BLEND);
	}
	/**
	* @fn void SlowWall::update(double dt)
	* @brief 状態更新メンバ関数
	* @param [in] dt 時間差分（秒）
	*/
	void update(double dt)
	{
		height += 1;
	}
};



//! キーボードステートインスタンス
KeyboardState keyboardState;
//! 車線の数
const int kNumTrafficLine = 2;
//! 車線インスタンス
TrafficLines trafficLines[kNumTrafficLine];
//! 破線の数
const int kNumDottedLine = 10;
//! 破線インスタンス
DottedLines dottedLines[kNumDottedLine];
//! 壁の最大数
const int kNumWall = 6;
//! 壁のインスタンス
Wall wall[kNumWall];
// 車のインスタンス
Car car;
// 数値描画インスタンス
DrawInt drawInt[2];
DrawInt drawString[2];
//! スロー時の壁のインスタンス
SlowWall slowWall;


/**
* @fn void initializeGame()
* @brief ゲーム環境の初期化
*/
void initializeGame()
{
	//initする
	for (int h = 0; h <5; ++h)
	{
		for (int w = 0; w < 2; ++w)
		{
			dottedLines[h * 2 + w].centerX = w * 70 + (160+ dottedLines[h * 2 + w].width_2);
			dottedLines[h * 2 + w].centerY = h * 140 + (0 + dottedLines[h * 2 + w].height_2);
		}
	}
	
	//wall
	for (int h = 0; h < 3; h++)
	{
		for (int w = 0; w < 2; w++)
		{
			wall[h * 2 + w].centerX = place[h][w] * 70 + (130);
			wall[h * 2 + w].centerY = h * 210 + (210);
		}
	}
	// trafficLines
	trafficLines[0].centerX = 95;
	trafficLines[0].centerY = 300;
	trafficLines[1].centerX = 305;
	trafficLines[1].centerY = 300;

	//drawInt
	// Scoreと表示
	drawString[0].firstX = 20;
	drawString[0].firstY = 500;
	drawString[0].initString("Score");
	// Scoreの数値の表示
	drawInt[0].firstX = 20;
	drawInt[0].firstY = 470;
	drawInt[0].initString("0");
	// Speedと表示
	drawString[1].firstX = 20;
	drawString[1].firstY = 400;
	drawString[1].initString("Speed");
	// Speedの数値の表示
	drawInt[1].firstX = 20;
	drawInt[1].firstY = 370;
	std::string str;
	char speedStr[30];
	snprintf(speedStr, 12, "%.0lf", wall[0].speed);
	drawInt[1].initString(speedStr);

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
	if (car.centerY + car.height_2 <= 0) {
		return true;
	}

	return false;
}
// チェック処理
bool checkWall(Car& car, Wall& wall) {
	//車と壁の衝突チェック
	// 車の上辺の衝突条件
	if (car.centerY+car.height_2>=wall.centerY-wall.height_2 &&car.centerY + car.height_2 <= wall.centerY + wall.height_2)
	{
		// 車の下辺が壁の枠の中に入ったら衝突
		
			//車の左頂点が衝突しているか  || 車の右頂点が衝突しているか
			if ((car.centerX - car.width_2) >= (wall.centerX - wall.width_2) && (car.centerX - car.width_2) <= (wall.centerX + wall.width_2) ||
				(car.centerX + car.width_2) >= (wall.centerX - wall.width_2) && (car.centerX + car.width_2) <= (wall.centerX + wall.width_2))
			{
				car.centerY = wall.centerY - wall.height_2 - car.height_2;
				return true;

			}
		
		
	}
	return false;
}

// 車線(路側帯)が右からの衝突判定、衝突したらtrue
bool checkTrafficLineByRight(Car& car, TrafficLines& line) {
	// 横からぶつかったらtrue
	if (car.centerX + car.width_2 >= line.centerX - line.width_2 && car.centerX - car.width_2 < line.centerX + line.width_2)
	{
		return true;
	}
	return false;
}
// 車線(路側帯)が左からの衝突判定、衝突したらtrue
bool checkTrafficLineByLeft(Car& car, TrafficLines& line) {
	// 横からぶつかったらtrue
	if (car.centerX + car.width_2 > line.centerX - line.width_2  && car.centerX - car.width_2 <= line.centerX + line.width_2)
	{
		return true;
	}
	return false;
}

// 車が壁に右からぶつかる時、衝突したらtrue
bool checkWallByRight(Car& car, Wall& wall) {
	// 壁が隣にあるとき
	if (car.centerY + car.height_2 > wall.centerY - wall.height_2 && car.centerY - car.height_2 < wall.centerY + wall.height_2) {
		// 横からぶつかった時
		if (car.centerX + car.width_2 >= wall.centerX - wall.width_2 && car.centerX - car.width_2 < wall.centerX + wall.width_2) {
			return true;
		}
		
	}
	return false;
}
// 車が壁に左からぶつかる時、衝突したらtrue
bool checkWallByLeft(Car& car, Wall& wall) {
	// 壁が隣にあるとき
	if (car.centerY + car.height_2 > wall.centerY - wall.height_2 && car.centerY - car.height_2 < wall.centerY + wall.height_2) {
		// 横からぶつかった時
		if (car.centerX + car.width_2 > wall.centerX - wall.width_2 && car.centerX - car.width_2 <= wall.centerX + wall.width_2) {
			return true;
		}

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
	// GameOverかどうか
	if (isGameOvered())
	{
		printf("Game Over...\n");
		// Scoreの更新を止める
		drawInt[0].needLoad = false;
		drawInt[0].setColor(1.0, 0, 0);
	}

	//update処理
	//破線
	for (int i = 0; i < kNumDottedLine; i++)
	{
		dottedLines[i].update(dt);
	}
	
	//wall
	for (int i = 0; i < kNumWall; i++)
	{
	
		
		wall[i].update(dt,i);
		checkWall(car,wall[i]);
	}
	
	// キーボード操作
	// aの時
	if (keyboardState.is_a==true)
	{
		// Wallの衝突判定
		bool isCollision = false;
		for (int i = 0; i < kNumWall; i++)
		{
			if (checkWallByLeft(car, wall[i])) {
				//true:衝突
				isCollision = true;
				break;
			}
			else {
				//false:衝突していない
			}
		}
		// TrafficLineの衝突判定
		for (int i = 0; i < kNumTrafficLine; i++)
		{	
			if (checkTrafficLineByLeft(car, trafficLines[i]))
			{
				//trueなので衝突、isCollisionをtrueにする
				isCollision = true;
				break;
			}
		}
		if (isCollision == false)
		{
			car.centerX -= car.slide_speed;
		}
	}
	// dの時
	else if (keyboardState.is_d == true)
	{
		// Wallの衝突判定
		bool isCollision = false;
		for (int i = 0; i < kNumWall; i++)
		{
			if (checkWallByRight(car, wall[i])) {
				//true:衝突
				isCollision = true;
				break;
			}
			else {
				//false:衝突していない
			}
		}
		// TrafficLineの衝突判定
		for (int i = 0; i < kNumTrafficLine; i++)
		{
			if (checkTrafficLineByRight(car, trafficLines[i]))
			{
				//trueなので衝突、isCollisionをtrueにする
				isCollision = true;
				break;
			}
		}

		if (isCollision == false)
		{
			car.centerX += car.slide_speed;
		}
	}
	

	// スコア100ごとにSpeedUp:上限250
	if (drawInt[0].score % 100 == 0 && drawInt[0].score != 0 && wall[0].speed < 250 && wall[0].speed!=50)
	{
		//破線
		for (int i = 0; i < kNumDottedLine; i++)
		{
			dottedLines[i].speed += 10;
		}
		//wall
		for (int i = 0; i < kNumWall; i++)
		{
			wall[i].speed += 10;
		}
		tempSpeed += 10;
		
		//壁に埋まってしまうのでなし
		//car.slide_speed += 5;
	}
	// スペースを押すとスロー
	if (keyboardState.is_space == true)
	{
		if (50 != wall[0].speed)
		{
			tempSpeed = wall[0].speed;
			// スローにする
			//破線
			for (int i = 0; i < kNumDottedLine; i++)
			{
				dottedLines[i].speed = 50;
			}
			//wall
			for (int i = 0; i < kNumWall; i++)
			{
				wall[i].speed = 50;
			}
		}
		// スローを使うと、画面が狭まる
		slowWall.update(dt);
	}
	else
	{
		//もとに戻す
		//破線
		for (int i = 0; i < kNumDottedLine; i++)
		{
			dottedLines[i].speed = tempSpeed;
		}
		//wall
		for (int i = 0; i < kNumWall; i++)
		{
			wall[i].speed = tempSpeed;
		}
	}
	drawInt[0].update(dt);
	char speedStr[30];
	snprintf(speedStr, 12, "%.0lf", tempSpeed);
	drawInt[1].initString(speedStr);

	glutSwapBuffers();
	glFlush();

	
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
	trafficLines[0].draw();
	trafficLines[1].draw();
	for (int i = 0; i < kNumDottedLine; i++)
	{
		dottedLines[i].draw();
	}
	for (int i = 0; i < kNumWall; i++)
	{
		wall[i].draw();
	}
	car.draw();
	drawString[0].draw();
	drawInt[0].draw();	
	drawString[1].draw();
	drawInt[1].draw();
	//SlowWall
	slowWall.draw();

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