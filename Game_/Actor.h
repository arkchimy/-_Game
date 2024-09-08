#pragma once

struct Sprite
{
	Sprite(const char* img)
		:Image(img)
	{
		scale.x = (short)strlen(img);
		scale.y = 1;
	};
	const char* Image;
	Vec2 scale;
};
struct Bulit
{
	Bulit()
		:bActive(false), bPlayer(false), dir(0,-1), _y(0), mSpeed(1.f)
	{
		img[0] = '!';
	}
	
	void operator + (const Vec2& vec)
	{
		_y += vec.y * mSpeed * (OneFrameTime / 1000.f);
		if (_y >= 1 || _y <= -1)
		{
			_y = 0;
			Vec2 temp;
			temp = Pos + vec;

			if (0 >= temp.y || temp.y >= SceneHeight - 1)
			{
				bActive = false;
			}
			Pos = temp;
		}
		
	}
	
	void Update();
	void Render();
	void HittedActor();

	bool IsPlayer() { return bPlayer; }
	bool Fire(const Vec2& firePos,bool bPlayer);

	Vec2 Position() { return Pos; }
	bool IsActive() { return bActive; }

private:
	bool bActive;
	bool bPlayer;
	char img[1];

	float mSpeed;
	float _y;

	Vec2 dir;
	Vec2 Pos;
};

const size_t poolSize = 50;
const size_t enemypoolSize = 30;
struct BulitPool
{
	static void Fire(const Vec2& pos, bool bPlayer = true);
	static void Update();
	static void Render();
	static void initBulit();
	static Bulit pool[poolSize];
	static Bulit Enemypool[enemypoolSize];
};


struct Player
{
	Player()
		:blive(true)
	{
		const char* img = "_A_";
		mSprite = new Sprite(img);
		mPos.x = SceneWidth / 2;
		mPos.y = SceneHeight/2;
		mSpeed = 30.f;
		mHp = 1;
		x = 0;
		y = 0;
		bReady = true;
	};
	//==  총알 출동 =====
	bool operator << (Bulit& bulit)
	{
		// 플레이어의 총알이 아니면 리턴.
		if (bulit.IsPlayer())
			return false;
		//mPos가 맞을 경우가 더 적음.
		if (mPos + Vec2(1, 0) == bulit.Position() && bulit.IsActive())
		{
			bulit.HittedActor();
			mHp--;
			if (mHp <= 0)
				blive = false; // GameOver
			return true;
		}
		return false;
	}
	void PlayerInput();
	void infoinit();
	
	void Move(const Vec2& vec);
	void MoveChk(Vec2& temp); // 화면 범위 체크
	
	//== Title Scene=======================
	void StartPointMove();
	void ExitPointMove(){ mPos = Vec2(22, 34); }

	Vec2 Position(){ return mPos; }
	const char* GetSprite(){	return mSprite->Image;	}
	bool IsDead() { return mHp <= 0; }
	bool Fire();

private:
	bool bReady;
	float x, y;
	Vec2 mPos;
	Sprite* mSprite;
	float mSpeed;
	short mHp;
	bool blive;
};

struct Enemy
{
	Enemy()
		:bdir(false), bActive(false), bPlayer(false), bReady(false),
		x(0),y(0),mSpeed(0),mHp(0), currentIdx(0), mRouteMaxIdx(0),mPos(0,0)
	{
		//id를 갖게하고 id를 통해 공격 패턴에 영향을 줌
		static short idx = 0;
		id = idx++;
		sprite = new Sprite("=@=");
	}
	~Enemy()
	{
		
	}
	void operator + (const Vec2& vec)
	{
		// 범위를 벗어나면 해당 위치에 가만히 있음.
		x += vec.x * mSpeed /10.f;
		y += vec.y * mSpeed /10.f;

		// 움직임 누적수치 적용
		int rx = 0, ry = 0;
		if (1 <= x)
		{
			rx = 1;
			x = 0;
		}
		else if(-1 >= x)
		{
			rx = 1;
			x = 0;
		}
		if (1 <= y)
		{
			ry = 1;
			y = 0;
		}
		else if (-1 >= y)
		{
			ry = 1;
			y = 0;
		}

		bool x_chk, y_chk;

		Vec2 temp;
		if(bdir)
			temp = mPos + Vec2(rx, ry);
		else
			temp = mPos - Vec2(rx, ry);
		x_chk =
			0 <= (temp.x - sprite->scale.x) && (temp.x + sprite->scale.x) <= SceneWidth - 1;
		y_chk =
			0 <= (temp.y - sprite->scale.y) && (temp.y + sprite->scale.y) <= SceneHeight - 1;

		if (!x_chk) 
		{
			temp.x = mPos.x;
		}
		if (!y_chk) 
		{
			temp.y = mPos.y;
			temp.y += 1;
		}
		if (mPos != temp )
		{
			currentIdx++;
			if (currentIdx == mRouteMaxIdx) 
			{
				currentIdx = 0;
				bdir = !bdir;
				temp.y += 1;
			}
		}
		mPos = temp;

	}
	bool operator << (Bulit& bulit)
	{
		if (bulit.IsPlayer() == false)
			return false;
		if (bulit.IsActive() && bActive)
		{
			if (mPos.x - sprite->scale.x <= bulit.Position().x &&
				mPos.x + sprite->scale.x >= bulit.Position().x&&
				mPos.y == bulit.Position().y)
			{
				bulit.HittedActor();
				mHp--;
				if (mHp <= 0)
					bActive = false; // GameOver
				return true;
			}
		}
		return false;
	}
	void invisible(){ bActive = false; }
	void initInfo(const EnemyInfo& info);
	
	void Update();
	

	bool bdir;
	bool bActive ;
	bool bPlayer;
	bool bReady;

	float x, y;
	short mSpeed;
	short mHp;
	short id;

	Vec2 mPos;
	Vec2* mRoute= nullptr;

	size_t currentIdx;
	size_t mRouteMaxIdx;

	Sprite* sprite;
	
};


