#pragma once
#include <iostream>
#include <windows.h>
#include <stdlib.h>

#define SceneHeight  50
#define SceneWidth  100
#define Enemy_CNT 50


struct String
{
	String()
	{
		str[0] = 0;
	}
	String(const char* data)
	{
		size_t data_len = strlen(data);//null미포

		for (size_t i = 0; i < data_len; i++)
		{
			str[i] = data[i];
		}
		str[data_len] = 0;

	}
	bool operator ==(const String& other)
	{
		size_t a = strlen(str);
		size_t b = strlen(other.str);
		if (a != b)
			return false;

		for (size_t i = 0; i < a; i++)
		{
			if (str[i] != other.str[i])
				return false;
		}
		return true;
	}
	String* operator + (const char* input) const
	{
		size_t size = strlen(str);
		size_t othersize = strlen(input);

		if (size + othersize >= SceneWidth)
			__debugbreak();

		String* out = new String();

		for (size_t i = 0; i < size; i++)
		{
			out->str[i] = str[i];
		}
		for (size_t i = 0; i < othersize; i++)
		{
			out->str[size + i] = input[i];
		}
		out->str[size + othersize] = 0;
		return out;
	}
	String* operator + (DWORD input) const
	{
		size_t size = strlen(str);

		char ptr[4 + 1];
		_itoa_s(input, ptr, 10);
		ptr[4] = 0;
		if (size + 4 >= SceneWidth)
			__debugbreak();

		String* out = new String();
		for (size_t i = 0; i < size; i++)
		{
			out->str[i] = str[i];
		}
		for (size_t i = 0; i < 4; i++)
		{
			out->str[size + i] = ptr[i];
		}
		out->str[size + 4] = 0;

		return out;//임시객체 반환중 => 고쳐야함
	}

	char str[SceneWidth];
};
struct Vec2
{
	Vec2(int X = 0, int Y = 0)
		:x(X), y(Y)
	{

	}
	Vec2(const String a, const String b)
	{
		// 음수처리는 Enemy01.txt,3,2,70,0,[F | B], 에서 
		//  'F' 'B' 추가 정보로 판단하고 음수는 들어올 수 없음.
		size_t len = strlen(a.str);
		int sum = 0;


		for (size_t i = 0; i < len; i++)
		{
			if (0x30 <= a.str[i] && a.str[i] <= 0x39)
				sum += (a.str[i] - 0x30) * (int)pow(10, len - i - 1);
		}
		x = sum;
		len = strlen(b.str);
		sum = 0;
		for (size_t i = 0; i < len; i++)
		{
			sum += (b.str[i] - '0') * (int)pow(10, len - i - 1);
		}
		y = sum;
	}
	Vec2 operator + (const Vec2& other)
	{
		return Vec2(x + other.x, y + other.y);
	}
	Vec2 operator - (const Vec2& other)
	{
		return Vec2(x - other.x, y - other.y);
	}
	bool operator == (const Vec2& other)
	{
		return x == other.x && y == other.y;
	}
	bool operator != (const Vec2& other)
	{
		return !(x == other.x && y == other.y);
	}
	short x;
	short y;

};

const DWORD OneFrameTime = 20;// 한 프레임의 시간
const Vec2 TitleStartPos = Vec2(22, 31);
const Vec2 GameStartPos = Vec2(32, 40);

extern bool bLoop;

template <typename T>
struct Node
{
	Node() = default;
	Node(const T input) :data(input), nextNode(nullptr) {}

	T data;
	Node<T>* nextNode;
};

template <typename T>
struct List
{
	List() :head(nullptr), tail(nullptr) {}

	~List()
	{

		while (head != nullptr)  
		{
			Node<T>* temp = head->nextNode;
			delete head;
			head = temp;
		}
		
	}
	bool empty()
	{
		return head == nullptr;
	}
	bool exitChk(const T& data)
	{
		Node<T>* temp = head;
		while (temp != nullptr)
		{
			if (temp->data == data)
				return true;
			temp = temp->nextNode;
		}
		return false;
	}
	
	void insert(const T& input)
	{

		if (empty())
		{
			head = new Node<T>(input);
			tail = head;
			return;
		}
		tail->nextNode = new Node<T>(input);
		tail = tail->nextNode;
	}
	bool pop(T& out)
	{
		if (empty())
			return false;
		
		Node<T>* temp = head->nextNode;
		out = head->data;
		delete head;
		head = temp;

		return true;
	}

	Node<T>* head;
	Node<T>* tail;
};

struct EnemyInfo
{
	EnemyInfo()
		:goal(nullptr), img(nullptr),hp(0),speed(0), goalMaxIdx(0)
	{

	}
	~EnemyInfo()
	{
		free(goal);
	}
	void goalVector(Vec2 target)
	{
		size_t size = abs(target.x) + abs(target.y);
		goalMaxIdx = size;

		if (size == 0)
			return;
		goal = (Vec2*)malloc(sizeof(Vec2) * size);

		for (size_t i = 0; i < size; i++)
		{
			if (target.x > 0)
			{
				target.x--;
				goal[i].x = -1;
				goal[i].y = 0;
			}
			else if (target.y > 0)
			{
				target.y--;

				goal[i].x = 0;
				goal[i].y = -1;
			}

		}
	}
	String name;
	Vec2* goal;
	short hp;
	short speed;
	char* img;
	size_t goalMaxIdx;
	bool bdir;

};
struct EnemyHashTable
{
	~EnemyHashTable()
	{
		while (HashTable->empty() == false)
		{
			EnemyInfo* temp;
			HashTable->pop(temp);
			delete(temp);
		}
	}
	size_t HashNum(const String& str);

	void insert(EnemyInfo* input);
	bool exist(const String& str);
	bool Search(const String& str, struct Enemy& out);

	List<EnemyInfo*> HashTable[10];
	List<String> existTable[10];
};