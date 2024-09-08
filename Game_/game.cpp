#include "stdafx.h"
#include "game.h"
#include "Actor.h"
#include "MyConsole.h"

#include <conio.h>
#pragma comment(lib, "winmm.lib")

//====================== Actor ======================
Player gPlayer;
Enemy enemies[Enemy_CNT];
bool gAllDie = false;

// ���� �� Flip�ϴ� ���·� BackBuffer�� ���� ��.
static char buffer[SceneHeight][SceneWidth];

// enemy�� �̵�����, status ������ node�� �� �ؽ����̺�
static EnemyHashTable enemyhashtable;

List<String> g_StageList;//Stage01, Stage02,txt���� ����Ʈ��.
List<String> g_EnemyInfoList;//g_StageList���� ���� Stage�� ��ġ, ��ġ ����
List<String> g_EenmyStatusList;// Enemy�� �󼼽����̴���

// ================= ���� ��� ======================
const String StagesFile = "./Datas/Stages/";
const String EnemiesFile = "./Datas/Enemies/";
const String ScenesFile = "./Datas/Scenes/";

//=================== ������ ���� ========================
static DWORD startTime = timeGetTime();
static DWORD lastTime = startTime; //�� ������ �ð�
static DWORD frame = 0;
static DWORD logicCnt = 0, renderCnt = 0, SleepCnt = 0;
static bool bSkip = false;

static DWORD SleepTime = 0;
//=================== Scene ���� ====================
bool bLoop = true; // while Ż��

EStage gStageNum = TITLE;
TitleInfo gtitleScene;
TitleInfo gclearScene;
TitleInfo ggameOverScene;

//=====================================================
void InitGame()
{
	// StageInfo Load
	char* buffer;
	String* StagesFileRoute = StagesFile + "StageInfo.txt";
	size_t bufferSize = LoadFile(&buffer, StagesFileRoute->str, "r");
	delete(StagesFileRoute);
	Split(buffer, bufferSize, '\n', g_StageList);
	free(buffer);


	String* filename = ScenesFile + "Title.txt";
	LoadScene(filename->str, gtitleScene);
	free(filename);

	filename = ScenesFile + "GameClear.txt";
	LoadScene(filename->str, gclearScene);
	free(filename);

	filename = ScenesFile + "GameOver.txt";
	LoadScene(filename->str, ggameOverScene);
	free(filename);

	gPlayer.infoinit();
	gPlayer.StartPointMove();
}

void LoadGame()
{
	static bool bLoadChk = false;//�������� ������ üũ
	
	if (g_StageList.empty() && bLoadChk == false)
	{
		//�������� ���� ����
		gAllDie = false;
		gStageNum = CLEAR;

		char* buffer;
		String* StagesFileRoute = StagesFile + "StageInfo.txt";
		size_t bufferSize = LoadFile(&buffer, StagesFileRoute->str, "r");
		delete(StagesFileRoute);
		Split(buffer, bufferSize, '\n', g_StageList);
		free(buffer);
		return;
	}
	//���� �����ִ� �Ѿ� ��Ȱ��ȭ
	BulitPool::initBulit();

	DWORD beforeTime;

	

	//õõ�� �ε��Ǵ� ������ �������.
	if (gStageNum == LOAD && bLoadChk == false && g_StageList.empty() == false)
	{
		bLoadChk = true;
		for (int i = 0; i < Enemy_CNT; i++)
		{
			enemies[i].invisible();
		}


		beforeTime = timeGetTime();

		String node;
		size_t bufferSize;
		char* buffer;

		g_StageList.pop(node);

		String* EnemyInfoRoute = StagesFile + node.str;
		bufferSize = LoadFile(&buffer, EnemyInfoRoute->str, "r");

		delete(EnemyInfoRoute);

		Split(buffer, bufferSize, ',', g_EnemyInfoList);
		free(buffer);
	}
	//�������� �� ������ ���� �� �������� ����
	// ���� ���������� true ��ȯ
	if (EnemyInfoInit() && bLoadChk)
	{
		gStageNum = GAME;
		gPlayer.StartPointMove();
		bLoadChk = false;
	}

}

void KeyInput()
{
	static Vec2 key;

	frame++;
	startTime += OneFrameTime;

	key.x = 0;
	key.y = 0;
	switch (gStageNum)
	{
	case TITLE:
		if (GetAsyncKeyState(VK_UP))
			gPlayer.StartPointMove();
		else if (GetAsyncKeyState(VK_DOWN))
			gPlayer.ExitPointMove();

		if (GetAsyncKeyState(0x41) || GetAsyncKeyState(0x64))
		{
			if (gPlayer.Position() == TitleStartPos)
			{
				gStageNum = LOAD;
			}
			else
			{
				gStageNum = EXIT;
			}
		}
		break;
	case GAME:
		gPlayer.PlayerInput();
		if (GetAsyncKeyState(VK_SPACE))
		{
			//Pause
		}
		if (GetAsyncKeyState(0x41) || GetAsyncKeyState(0x64))
		{
			//Ǯ�� 
			if(gPlayer.Fire())
			{
				BulitPool::Fire(gPlayer.Position() + Vec2(1, 0));
			}
		}
		break;
	case LOAD:
		break;
	case GAMEOVER:
	case CLEAR:
		if (GetAsyncKeyState(VK_SPACE))
		{
			gStageNum = TITLE;
			gPlayer.StartPointMove();
		}
		break;
	}

}



void gameLogic()
{
	bool aliveEnemyChk = false;
	logicCnt++;
	if (gPlayer.IsDead())
	{
		gPlayer.infoinit();
		gStageNum = GAMEOVER;
		resetStageList();
	}

	switch (gStageNum)
	{
	case TITLE:
		break;
	case GAME:
		aliveEnemyChk = false;
		for (int i = 0; i < Enemy_CNT; i++)
		{
			enemies[i].Update();
			if (enemies[i].bActive)
				aliveEnemyChk = true;
		}
		BulitPool::Update();
		if (gStageNum == GAME && !aliveEnemyChk)
		{
			gStageNum = LOAD;
			gAllDie = true;
		}
		else
		{
			gAllDie = false;
		}
		break;

	case LOAD:
		LoadGame();
		break;

	case GAMEOVER:
		break;

	case CLEAR:
		break;

	case EXIT:
		bLoop = false;
	}



	DWORD currentTime = timeGetTime();
	if (startTime > currentTime)
	{
		Sleep(startTime - currentTime);
		SleepCnt++;
	}
	else if (currentTime - startTime >= 20)
	{
		bSkip = true;
	}
	else
	{
		bSkip = false;
	}


}

bool Skip()
{
	return bSkip;
}

void Render()
{
	renderCnt++;

	InitBuffer();
	switch (gStageNum)
	{
	case TITLE:
		RenderTitle();
		break;
	case GAME:
		RenderGame();
		break;
	case LOAD:
		RenderLoad();
		break;
	case GAMEOVER:
		RenderGameOver();
		break;
	case CLEAR:
		RenderClear();
		break;
	}

	FrameRender();
	for (int i = 0; i < SceneHeight; i++)
	{
		cs_Move(0, i);
		printf(buffer[i]);
	}

}

void RenderLoad()
{
	static DWORD flash = 0;

	if (flash <= 25)
		UpdateBuffer({ SceneWidth / 2 - 5,SceneHeight / 2 - 3 }, " Loading");
	else if (flash == 50)
		flash = 0;

	flash++;
}

void RenderTitle()
{
	static DWORD flash = 0;

	for (int i = 0; i < SceneHeight; i++)
	{
		UpdateBuffer({ 0,i }, gtitleScene.flips[i]);
	}
	if (flash <= 25)
	{
		UpdateBuffer({ 2,27 }, "                                                            ");
	}
	else if (flash == 50)
		flash = 0;
	flash++;

	UpdateBuffer(gPlayer.Position(), "= > ");
}

void RenderGameOver()
{
	static DWORD flash = 0;

	for (int i = 0; i < SceneHeight; i++)
	{
		UpdateBuffer({ 0,i }, ggameOverScene.flips[i]);
	}
	if (flash <= 25)
	{
		UpdateBuffer({ 2,27 }, "                                                            ");
	}
	else if (flash == 50)
		flash = 0;
	flash++;
}

void RenderClear()
{
	static DWORD flash = 0;

	for (int i = 0; i < SceneHeight; i++)
	{
		UpdateBuffer({ 0,i }, gclearScene.flips[i]);
	}
	if (flash <= 25)
	{
		UpdateBuffer({ 2,27 }, "                                                            ");
	}
	else if (flash == 50)
		flash = 0;
	flash++;
}

void RenderGame()
{
	for (int i = 0; i < Enemy_CNT; i++)
	{
		if (enemies[i].bActive)
			UpdateBuffer(enemies[i].mPos, enemies[i].sprite->Image);
	}
	UpdateBuffer(gPlayer.Position(), gPlayer.GetSprite());
	BulitPool::Render();
}

void FrameRender()
{
	static DWORD FramePerSecond = 0;
	static DWORD Loginc_Cnt = 0;
	static DWORD Render_Cnt = 0;
	static DWORD Sleep_Cnt = 0;

	String* FrameInfo;
	String temp;
	FrameInfo = temp + "FramePerSecond :";
	temp = FrameInfo->str;
	delete(FrameInfo);

	FrameInfo = temp + FramePerSecond;
	temp = FrameInfo->str;
	delete(FrameInfo);

	FrameInfo = temp + "   Loginc_Cnt :";
	temp = FrameInfo->str;
	delete(FrameInfo);

	FrameInfo = temp + Loginc_Cnt;
	temp = FrameInfo->str;
	delete(FrameInfo);

	FrameInfo = temp + "   Render_Cnt :";
	temp = FrameInfo->str;
	delete(FrameInfo);

	FrameInfo = temp + Render_Cnt;
	temp = FrameInfo->str;
	delete(FrameInfo);

	FrameInfo = temp + "   Sleep_Cnt :";
	temp = FrameInfo->str;
	delete(FrameInfo);

	FrameInfo = temp + Sleep_Cnt;
	temp = FrameInfo->str;
	delete(FrameInfo);

	UpdateBuffer({ 0,0 }, temp.str);


	if (timeGetTime() - lastTime >= 1000)
	{
		FramePerSecond = frame;
		Loginc_Cnt = logicCnt;
		Render_Cnt = renderCnt;
		Sleep_Cnt = SleepCnt;

		lastTime += 1000;

		frame = 0;
		logicCnt = 0;
		renderCnt = 0;
		SleepCnt = 0;
	}
}


void resetStageList()
{
	while (g_StageList.empty() == false)
	{
		String temp;
		g_StageList.pop(temp);
	}

	char* buffer;
	String* StagesFileRoute = StagesFile + "StageInfo.txt";
	size_t bufferSize = LoadFile(&buffer, StagesFileRoute->str, "r");
	delete(StagesFileRoute);
	Split(buffer, bufferSize, '\n', g_StageList);
	free(buffer);
}

void LoadScene(char* name, TitleInfo& out)
{
	char* buffer;
	// ���ο��� �����Ҵ�.
	size_t bufferSize = LoadFile(&buffer, name, "r");

	List<String> titleStr;
	Split(buffer, bufferSize, '\n', titleStr);
	free(buffer);

	size_t idx = 0;
	while (!titleStr.empty())
	{
		String flip;
		titleStr.pop(flip);
		out.FlipPush(idx++, flip.str);
	}
}

void UpdateBuffer(const Vec2& pos, const char* sprite)
{
	int len = strlen(sprite);

	for (int i = 0; i < len; i++)
	{
		buffer[pos.y][pos.x + i] = sprite[i];
	}

}



size_t LoadFile(char** buffer, char* filename, const char* flag)
{
	FILE* file;
	fopen_s(&file, filename, flag);

	size_t FileLen = 0;

	if (ReadFile(buffer, FileLen, file) == false)
	{
		//�޸𸮰��� ����
		__debugbreak();
	}

	fflush(file);
	fclose(file);
	return FileLen;
}



void Split(char *  buffer, size_t bufferSize ,const char sep, List<String>& out)
{
	char* p1 = buffer;
	char* p2 = buffer;

	while (p2 != buffer + bufferSize - 1)
	{
		if (*p2 == sep)
		{
			//Text������ ��� �ٹٲ��� 0D 0A�� 1����Ʈ�� ����.
			size_t len = p2 - p1;
			*p2 = 0;//�ι���.
			char* name = (char*)malloc(len);
			int idx = 0;
			while (p1 <= p2)
			{
				if (*p1 == '\n')
				{
					p1++;
					continue;
				}
				name[idx++] = *(p1++);
				
			}
			out.insert(name);
			p2 = p1;

			
		}
		else
			p2++;
	}
	if (p1 != p2)
	{

		
		if (*p2 == sep)
		{
			//Text������ ��� �ٹٲ��� 0D 0A�� 1����Ʈ�� ����.
			size_t len = p2 - p1;
			*p2 = 0;//�ι���.
			char* name = (char*)malloc(len);
			int idx = 0;
			while (p1 <= p2)
			{
				name[idx++] = *(p1++);
			}
			out.insert(name);
			p2 = p1;


		}
	}
}


bool EnemyInfoInit()
{

	static size_t idx;
	if (g_EnemyInfoList.empty())
	{
		idx = 0;
		return true;
	}

	String temp;
	g_EnemyInfoList.pop(temp);

	EnemyInfo* newNode = new EnemyInfo();
		
	newNode->name = temp;

	String x, y;
	g_EnemyInfoList.pop(x);//
	g_EnemyInfoList.pop(y);
	Vec2 initpos(x,y);
	enemies[idx].mPos = initpos;
		
	g_EnemyInfoList.pop(x);
	g_EnemyInfoList.pop(y);

	String bdir;
	g_EnemyInfoList.pop(bdir);
	if (bdir.str[0] == 'F')
		newNode->bdir = true;
	else
		newNode->bdir = false;

	// Enemy0X.txt�а� newNode ���� �����ϱ�.
	if (enemyhashtable.exist(temp) == false)
	{
		Vec2 golePos(x, y);
		newNode->goalVector(golePos);
			
		//Enemy0X.txt Status��������
		char* buffer;
		size_t bufferSize;

		FILE* enemyTxt;
		String* Enemies = EnemiesFile + temp.str;
		fopen_s(&enemyTxt, Enemies->str, "r");
		delete(Enemies);
			
		ReadFile(&buffer, bufferSize, enemyTxt);
		fclose(enemyTxt);

		Split(buffer, bufferSize, '\n', g_EenmyStatusList);

		String img;
		g_EenmyStatusList.pop(img);
		newNode->img = (char*)malloc(strlen(img.str) + 1);
		for (size_t i = 0; i < strlen(img.str); i++)
		{
			newNode->img[i] = img.str[i];
		}
		newNode->img[strlen(img.str)] = 0;

		String hp;//hp 1�� ���� �ڸ�
		g_EenmyStatusList.pop(hp);
		newNode->hp = (short)hp.str[0] - '0';

		String speed;//speed ���� �ڸ�
		g_EenmyStatusList.pop(speed);
		newNode->speed = (short)speed.str[0] - '0';

		enemyhashtable.insert(newNode);


	}
		
	enemyhashtable.Search(temp, enemies[idx]);
	idx++;
	
	return false;

}


void InitBuffer()
{
	for (int i = 0; i < SceneHeight; i++)
	{
		memset(buffer[i], ' ', SceneWidth);
		buffer[i][SceneWidth - 1] = '\0';
	}
}

bool ReadFile(char** out, size_t& size, FILE* const file)
{

	size = FileTell(file);

	*out = (char*)malloc(size);
	if (*out == nullptr)
		return false;
	size = fread(*out, 1, size, file);
	return true;
}

size_t FileTell(FILE* const file) 
{
	fseek(file, 0, SEEK_END);
	size_t Filelen = ftell(file);
	fseek(file, 0, SEEK_SET);

	return Filelen;
}

