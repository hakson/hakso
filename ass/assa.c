#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LEN 100

typedef int bool;
#define true 1
#define false 0

typedef enum _Gen_
{
	M = 0,
	F = 1
} Gen;

typedef struct _Person_
{
	char* name;
	Gen gen;
	struct _Person_* parent;
	struct _Person_** children;
} Person;

void add(int count, char** strings);
void drawAll(int count, char** strings);
void draw(int count, char** strings);
void relationship(int count, char** strings);
void list(int count, char** strings);
void readInput(int* count, char*** strings);

int main(int argc, char* argv)
{
	while (true)
	{
		int count = 0;
		char** strings = NULL;
		readInput(&count, &strings);

		//che
		if (count < 1)
		{
			continue;
		}

		if (strcmp(strings[0], "add") == 0)
		{
			add(count, strings);
		}
		else if (strcmp(strings[0], "draw-all") == 0)
		{
			drawAll(count, strings);
		}
		else if (strcmp(strings[0], "quit"))
		{
			exit(0);
		}

		// free and reset
		int i = 0;
		for (i = 0; i < count; i++)
		{
			free(strings[i]);
		}
		free(strings);

		count = 0;
	}

	return 0;
}

void add(int count, char** strings)
{
	if (count != 6)
	{
		return;
	}

	printf("add\n");
}

void drawAll(int count, char** strings)
{
	printf("draw\n");
}

void draw(int count, char** strings)
{

}

void relationship(int count, char** strings)
{

}

void list(int count, char** strings)
{

}

void readInput(int* count, char*** strings)
{
	char in_str[BUFFER_LEN];
	printf("esp>");
	//sscanf(in_str, "%s");
	fgets(in_str, BUFFER_LEN, stdin);

	char * pch = NULL;
	pch = strtok(in_str, " ");
	if (pch != NULL)
	{
		(*strings) = (char**)malloc(sizeof(char*));

		int i = 0;
		do
		{
			(*strings)[i] = (char*)malloc((strlen(pch) + 1) * sizeof(char));
			int j;
			for (j = 0; j < (int)(strlen(pch) + 1); j++)
			{
				(*strings)[i][j] = pch[j];
			}
			printf("%s\n", pch);

			i++;

			pch = strtok(NULL, " \n");
			(*strings) = (char**)realloc((*strings), (i + 1) * sizeof(char*));
		} while (pch != NULL);
		*count = i;
	}

	//parsing
}