#pragma once

struct TitleInfo
{
	void FlipPush(size_t height, const char* str)
	{
		for (size_t i = 0; i < SceneWidth; i++)
		{
			flips[height][i] = str[i];
		}
		flips[height][SceneWidth - 1] = 0;
	}
	char flips[SceneHeight][SceneWidth];
};

enum EStage
{
	TITLE = 0,
	GAME,
	LOAD,
	CLEAR,
	GAMEOVER,
	EXIT,
};


void InitGame();
void LoadGame();
void KeyInput();

void gameLogic();
bool Skip();

void Render();
// switch 분기 render 
void RenderLoad();
void RenderTitle();
void RenderGameOver();
void RenderClear();
void RenderGame();


void FrameRender();

void InitBuffer();
void UpdateBuffer(const Vec2& pos, const char* sprite);

void resetStageList();
void LoadScene(char* name, TitleInfo& out);

size_t LoadFile(char** buffer, char* filename,
	const char* flag);
//파일에서 읽은 데이터를 구분자로 구별하여 List로 반환
void Split(char * buffer,size_t bufferSize,const char sep, List<String>& out);
bool EnemyInfoInit();

bool ReadFile(char** out, size_t& size, FILE* const file);

size_t FileTell(FILE* const file);

