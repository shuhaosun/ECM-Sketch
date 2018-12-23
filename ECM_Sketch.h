#ifndef _CMSKETCH_H
#define _CMSKETCH_H

#include <algorithm>
#include <cstring>
#include <string.h>
#include "params.h"
#include "BOBHash32.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

typedef struct Node
{
	int exponent;
	int end;
	int start;
}Bucket;

typedef struct Node1
{
	struct Node bucket[100];
	int number;
}Counter;

class ECM_Sketch
{	
private:
	BOBHash32 * bobhash32[MAX_HASH_NUM];
	int index[MAX_HASH_NUM];
	Counter *counter[MAX_HASH_NUM];
	int w, d;
	int MAX_CNT;
	int counter_index_size;
	uint64_t hash_value;

public:
	int mem_acc;
	ECM_Sketch(int _w, int _d)
	{
		mem_acc = 0;
		counter_index_size = 20;
		w = _w;
		d = _d;
		
		for(int i = 0; i < d; i++)	
		{
			counter[i] = new Counter[w];
			memset(counter[i], -1, sizeof(Counter) * w);
		}

		MAX_CNT = (1 << COUNTER_SIZE) - 1;

		for(int i = 0; i < d; i++)
		{
			bobhash32[i] = new BOBHash32(i);
		}
	}
	void Expire_bucket(int i, int j, int t)
	{
		int z = counter[i][j].number - 1;
		if (z != -2)
		{
			for (int q = z; q >= 0; q--)
			{
				if (counter[i][j].bucket[q].end <= (t - WINDOW_SIZE))
				{
					counter[i][j].bucket[q].exponent = -1;
					counter[i][j].bucket[q].start = -1;
					counter[i][j].bucket[q].end = -1;
					counter[i][j].number--;
				}
				else
				{
					break;
				}
			}
		}
	}
	void Insert_bucket(int i, int j, int t)
	{
		int z = counter[i][j].number;
		int p = -1;
		int value = 0;
		int first = 0;
		if (counter[i][j].number == -1)
		{
			counter[i][j].bucket[0].exponent = 0;
			counter[i][j].bucket[0].start = t;
			counter[i][j].bucket[0].end = t;
			counter[i][j].number = 1;
		}
		else
		{
			while (p < z)
			{
				if ((counter[i][j].bucket[p + 2].exponent == value) && (p == -1))    // p+2 = 1
				{
					counter[i][j].bucket[p + 2].exponent++;
					counter[i][j].bucket[p + 2].end = counter[i][j].bucket[p + 1].end;
					counter[i][j].bucket[p + 1].start = counter[i][j].bucket[p + 2].end;
					counter[i][j].bucket[p + 1].end = t;
					p = p + 2;
					value = counter[i][j].bucket[p].exponent;
				}
				else if ((counter[i][j].bucket[p + 2].exponent == value) && (p != -1))
				{
					counter[i][j].bucket[p + 2].exponent++;
					counter[i][j].bucket[p + 2].end = counter[i][j].bucket[p + 1].end;
					counter[i][j].bucket[p + 1].start = counter[i][j].bucket[p + 2].end;
					for (int q = p + 1; q > first; q--)
					{
						counter[i][j].bucket[q].exponent = counter[i][j].bucket[q - 1].exponent;
						counter[i][j].bucket[q].start = counter[i][j].bucket[q - 1].start;
						counter[i][j].bucket[q].end = counter[i][j].bucket[q - 1].end;
					}
					first++;
					counter[i][j].number--;
					p = p + 2;
					value = counter[i][j].bucket[p].exponent;
				}
				else if ((counter[i][j].bucket[p + 2].exponent != value) && (p != -1))
				{
					break;
				}
				else
				{
					for (int q = z; q > 0; q--)
					{
						counter[i][j].bucket[q].exponent = counter[i][j].bucket[q - 1].exponent;
						counter[i][j].bucket[q].start = counter[i][j].bucket[q - 1].start;
						counter[i][j].bucket[q].end = counter[i][j].bucket[q - 1].end;
					}
					counter[i][j].number++;
					counter[i][j].bucket[0].exponent = 0;
					counter[i][j].bucket[0].start = counter[i][j].bucket[1].end;
					counter[i][j].bucket[0].end = t;
					break;
				}
			}
			int qq = 0;
			if (first != 0)
			{
				for (int q = first; q < first + counter[i][j].number; ++q)
				{
					counter[i][j].bucket[qq].exponent = counter[i][j].bucket[q].exponent;
					counter[i][j].bucket[qq].start = counter[i][j].bucket[q].start;
					counter[i][j].bucket[qq].end = counter[i][j].bucket[q].end;
					qq++;
				}
				for (int q = qq; q < qq + first; ++q)
				{
					counter[i][j].bucket[q].exponent = -1;
					counter[i][j].bucket[q].start = -1;
					counter[i][j].bucket[q].end = -1;
				}
			}
		}
	}
	void Insert(const char * str, int t)
	{
		for(int i = 0; i < d; i++)
		{
			index[i] = (bobhash32[i]->run(str, strlen(str))) % w;

			Expire_bucket(i, index[i], t);
			Insert_bucket(i, index[i], t);

			mem_acc ++;
		}
	}
	int Bucket_sum(int i, int j, int t)
	{
		int z = counter[i][j].number;
		if (z == -1)
		{
			return 0;
		}
		else
		{
			int exp;
			int count_bucket = 1;
			int count = 0;
			int q;
			for (q = 0; q < z - 1; ++q)
			{
				exp = counter[i][j].bucket[q].exponent;
				for (int k = 0; k < exp; ++k)
				{
					count_bucket = 2 * count_bucket;
				}
				count = count + count_bucket;
				count_bucket = 1;
			}
			for (int k = 0; k < counter[i][j].bucket[q].exponent; ++k)
			{
				count_bucket = 2 * count_bucket;
			}
			
			count = count + count_bucket / 2;

			return count;
		}
	}
	int Query(const char *str, int t)
	{
		int min_value = MAX_CNT;
		int temp;
		for(int i = 0; i < d; i++)
		{
			index[i] = (bobhash32[i]->run(str, strlen(str))) % w;
			Expire_bucket(i, index[i], t);
			temp = Bucket_sum(i, index[i], t);
			min_value = temp < min_value ? temp : min_value;
		}
		return min_value;
	}
	~ECM_Sketch()
	{
		for(int i = 0; i < d; i++)	
		{
			delete []counter[i];
		}


		for(int i = 0; i < d; i++)
		{
			delete bobhash32[i];
		}
	}
};
#endif//_CMSKETCH_H
