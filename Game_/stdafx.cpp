#include "stdafx.h"
#include "Actor.h"

size_t EnemyHashTable::HashNum(const String& str)
{
	size_t len = strlen(str.str);
	size_t sum = 0;
	for (size_t i = 0; i < len; i++)
	{
		sum += str.str[i] % 10;
	}
	return sum %= 10;
}


void EnemyHashTable::insert( EnemyInfo*  input)
{
	if (exist(input->name) == false)
	{
		HashTable[HashNum(input->name)].insert(input);
		existTable[HashNum(input->name)].insert(input->name);
	}
}

bool EnemyHashTable::exist(const String& input)
{

    return existTable[HashNum(input.str)].exitChk(input);
}

bool EnemyHashTable::Search(const String& str, Enemy& out)
{
	if (exist(str.str) == false)
	{
		return false;
	}

	Node<EnemyInfo*>* info = HashTable[HashNum(str.str)].head;
	while (info != nullptr)
	{
		if (info->data->name == str.str)
		{
			out.initInfo(*info->data);
			break;
		}
		info = info->nextNode;
	}

	return true;
}
