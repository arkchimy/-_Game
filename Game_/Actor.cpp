#include "stdafx.h"
#include "Actor.h"
#include "game.h"


Bulit BulitPool::pool[poolSize];
Bulit BulitPool::Enemypool[enemypoolSize];

extern Player gPlayer;
extern Enemy enemies[Enemy_CNT];
extern EStage gStageNum;

void Bulit::Update()
{
	if (bActive )
	{
		*this + dir;
		// [Player | Enemy] << Bulit (Hit 판정)
		if (gPlayer << *this)
			return;
		for (int i = 0; i < Enemy_CNT; i++)
		{
			if (enemies[i] << *this)
				break;
		}
	}
}

void Bulit::Render()
{
	if (bActive == false)
		return;
	UpdateBuffer(Pos, img);
}

void Bulit::HittedActor()
{
	bActive = false;
}

bool Bulit::Fire(const Vec2& firePos, bool bPlayer)
{
	if (bActive == false)
	{
		bActive = true;
		Pos = firePos;
		this->bPlayer = bPlayer;
		_y = 0;
		if (bPlayer)
		{
			
			dir = Vec2(0, -1);
			mSpeed = 30.f;
			img[0] = '!';
		}
		else 
		{
			dir = Vec2(0, 1);
			mSpeed = 10.f;
			img[0] = '*';
		}

		return true;
	}
	return false;
}

void BulitPool::Fire(const Vec2& pos, bool bPlayer)
{
	switch (bPlayer) 
	{
		case true:
			for (size_t i = 0; i < poolSize; i++)
			{
				// 발사 성공 시 true
				if (pool[i].Fire(pos, bPlayer))
				{
					break;
				}
			}
			break;
		case false:
			for (size_t i = 0; i < enemypoolSize; i++)
			{
				// 발사 성공 시 true
				if (Enemypool[i].Fire(pos, bPlayer))
				{
					break;
				}
			}
	}

	
}

void BulitPool::Update()
{
	for (size_t i = 0; i < poolSize; i++)
	{
		pool[i].Update();
	}
	for (size_t i = 0; i < enemypoolSize; i++)
	{
		Enemypool[i].Update();
	}
}

void BulitPool::Render()
{
	for (size_t i = 0; i < poolSize; i++)
	{
		pool[i].Render();
	}
	for (size_t i = 0; i < enemypoolSize; i++)
	{
		Enemypool[i].Render();
	}
}

void BulitPool::initBulit()
{
	for (size_t i = 0; i < poolSize; i++)
	{
		pool[i].HittedActor();
	}
	for (size_t i = 0; i < enemypoolSize; i++)
	{
		Enemypool[i].HittedActor();
	}
}

void Player::PlayerInput()
{
	static Vec2 key;
	key.x = 0;
	key.y = 0;
	if (GetAsyncKeyState(VK_LEFT))
	{
		key.x = -1;
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		key.x = 1;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		key.y = -1;
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		key.y = +1;
	}
	gPlayer.Move(key);
}

void Player::infoinit()
{
	//부활
	mPos.x = SceneWidth / 2;
	mPos.y = SceneHeight / 2;
	mSpeed = 30.f;
	mHp = 1;
	x = 0;
	y = 0;
}

void Player::Move(const Vec2& vec)
{
	// 속도에 맞춰 이동.
	x += vec.x * mSpeed * (OneFrameTime / 1000.f);//0.02sec
	y += vec.y * mSpeed * (OneFrameTime / 1000.f);// Y축이 더 김.

	int rx = 0, ry = 0;
	if (x < -1)
	{
		rx = -1;
		x = 0;
	}
	else if (1 <= x)
	{
		rx = 1;
		x = 0;
	}
	if (y < -1)
	{
		ry = -1;
		y = 0;
	}
	else if (1 <= y)
	{
		ry = 1;
		y = 0;
	}

	Vec2 temp = mPos + Vec2(rx, ry);
	MoveChk(temp);

	mPos = temp;
}

void Player::MoveChk(Vec2& temp)
{
	bool x_chk, y_chk;
	x_chk =
		0 <= (temp.x - mSprite->scale.x) && (temp.x + mSprite->scale.x) < SceneWidth - 1;
	y_chk =
		0 <= (temp.y - mSprite->scale.y) && (temp.y + mSprite->scale.y) < SceneHeight - 1;

	if (!x_chk)
		temp.x = mPos.x;
	if (!y_chk)
		temp.y = mPos.y;
}

void Player::StartPointMove()
{
	switch (gStageNum)
	{
	case TITLE:
		mPos = TitleStartPos;
		break;
	case GAME:
		mPos = GameStartPos;
		break;
	}
}

bool Player::Fire()
{
	static DWORD fireTime = timeGetTime();
	DWORD currentTime = timeGetTime();
	if (currentTime > fireTime + 200)
	{
		bReady = true;
	}
	if (bReady)
	{
		bReady = false;
		fireTime = timeGetTime();
		return true;
	}
	return false;

}



void Enemy::initInfo(const EnemyInfo& info)
{
	mSpeed = info.speed;
	mRoute = info.goal;
	sprite->Image = info.img;
	mHp = info.hp;
	mRouteMaxIdx = info.goalMaxIdx;
	bdir = info.bdir;
	bActive = true;

	x = 0;
	y = 0;
	currentIdx = 0;
	bReady = false;
}
void Enemy::Update()
{
	if (bActive)
	{
		if (mRoute == nullptr)
		{
			__debugbreak();
		}
		*this + mRoute[currentIdx];
		if ((id + currentIdx) % 10 == 0)
		{
			bReady = true;
		}
		else if (bReady)
		{
			bReady = false;
			BulitPool::Fire(mPos + Vec2{ 1,0 }, bPlayer);
		}
		if (mPos.y == 50)
		{
			gStageNum = GAMEOVER;
			resetStageList();
		}
	}

}
