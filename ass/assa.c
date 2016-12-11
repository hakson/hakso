#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BUFFER_LEN 100

typedef int bool;
#define true 1
#define false 0

typedef enum _Sex_
{
	M = 0,
	F = 1
} Sex;

typedef struct _Person_
{
	char* name;
	Sex sex;
	int father;
	int mother;
	int* children;
	int child_c;
} Person;

typedef struct _PeopleData_
{
	Person* people;
	int people_c;
	int unknowns;
} PeopleData;


void add(int count, char** strings, PeopleData* pd);
void drawAll(int count, char** strings, PeopleData* pd);
void draw(int count, char** strings, PeopleData* pd);
void relationship(int count, char** strings, PeopleData* pd);
void list(int count, char** strings, PeopleData* pd);
void readInput(int* count, char*** strings);
int addPerson(Person p, PeopleData* pd);
int getPerson(Person p, PeopleData* pd);
int getPersonByName(char* name, Sex sex, PeopleData* pd);
void printPerson(Person* p, PeopleData* pd);
void printPeople(PeopleData* pd);


int main(int argc, char* argv)
{
	PeopleData pd;
	pd.people = NULL;
	pd.people_c = 0;
	pd.unknowns = 0;

	//todo
	//parsing of parameters

	// main program loop
	while (true)
	{
		int count = 0;
		char** strings = NULL;
		readInput(&count, &strings);

		// execute correct command
		if (count < 1)
		{
			continue;
		}

		if (strcmp(strings[0], "add") == 0)
		{
			add(count, strings, &pd);
		}
		else if (strcmp(strings[0], "draw-all") == 0)
		{
			drawAll(count, strings, &pd);
		}
		else if (strcmp(strings[0], "quit"))
		{
			exit(0);
		}

		// free and reset
		int i;
		for (i = 0; i < count; i++)
		{
			free(strings[i]);
		}
		free(strings);

		count = 0;
	}

	return 0;
}

void add(int count, char** strings, PeopleData* pd)
{
	if (count < 6)
	{
		return;
	}

	Person p1 = { .name = NULL,.sex = M,.father = -1,.mother = -1,.children = NULL };
	Person p2 = { .name = NULL,.sex = M,.father = -1,.mother = -1,.children = NULL };

	int names_len[2] = { 0,0 };
	char *names[2] = { NULL,NULL };
	int names_start[2] = { -1,-1 };
	int names_end[2] = { -1,-1 };

	// find out position of the name and if male or female
	int i;
	for (i = 1; i < count; i++)
	{
		if (strcmp(strings[i], "[m]") == 0)
		{
			p1.sex = M;
			names_start[0] = 1;
			names_end[0] = i;
			break;
		}
		if (strcmp(strings[i], "[f]") == 0)
		{
			p1.sex = F;
			names_start[0] = 1;
			names_end[0] = i;
			break;
		}
	}

	for (i = names_end[0] + 2; i < count; i++)
	{
		if (strcmp(strings[i], "[m]") == 0)
		{
			p2.sex = M;
			names_start[1] = names_end[0] + 2;
			names_end[1] = i;
			break;
		}
		if (strcmp(strings[i], "[f]") == 0)
		{
			p2.sex = F;
			names_start[1] = names_end[0] + 2;
			names_end[1] = i;
			break;
		}
	}

	if (names_start[0] == -1 || names_start[1] == -1, names_end[0] == -1 || names_end[1] == -1) // return if names not found
	{
		return;
	}

	// copy names to people
	for (i = 0; i < 2; i++)
	{
		int j;
		for (j = names_start[i]; j < names_end[i]; j++)
		{
			names_len[i] += (int)strlen(strings[j]) + 1;
		}
		names[i] = malloc(sizeof(char) * names_len[i]);

		int l = 0;
		for (j = names_start[i]; j < names_end[i]; j++, l++)
		{
			int k;
			for (k = 0; k < (int)strlen(strings[j]); k++, l++)
			{
				names[i][l] = strings[j][k];
			}
			names[i][l] = ' ';
		}
		l--;
		names[i][l] = (char)0;
	}

	p1.name = names[0];
	p2.name = names[1];

	// get people if the exist and create them if they dont
	int p1p = getPerson(p1, pd);
	if (p1p == -1)
	{
		p1p = addPerson(p1, pd);
		printf("added: ");
		printPerson(&pd->people[pd->people_c - 1], pd);
	}
	else
	{
		free(p1.name);
		p1 = pd->people[p1p];
		printf("not added\n");
	}

	int p2p = getPerson(p2, pd);
	if (p2p == -1)
	{
		p2p = addPerson(p2, pd);
		printf("added: ");
		printPerson(&pd->people[pd->people_c - 1], pd);
	}
	else
	{
		free(p2.name);
		p2 = pd->people[p2p];
		printf("not added\n");
	}

	// relationship
	char* relationship = strings[names_end[0] + 1];
	if (strcmp(relationship, "father") == 0)
	{
		if (pd->people[p2p].father != -1)
		{
			return;
		}
		if (pd->people[p1p].sex != M)
		{
			return;
		}
		pd->people[p2p].father = p1p;
		pd->people[p1p].children = realloc(pd->people[p1p].children, sizeof(int) * (pd->people[p1p].child_c + 1));
		pd->people[p1p].child_c++;
		pd->people[p1p].children[pd->people[p1p].child_c - 1] = p2p;
	}
	else if (strcmp(relationship, "mother") == 0)
	{
		if (pd->people[p2p].mother != -1)
		{
			return;
		}
		if (pd->people[p1p].sex != F)
		{
			return;
		}
		pd->people[p2p].mother = p1p;
		pd->people[p1p].children = realloc(pd->people[p1p].children, sizeof(int) * (pd->people[p1p].child_c + 1));
		pd->people[p1p].child_c++;
		pd->people[p1p].children[pd->people[p1p].child_c - 1] = p2p;
	}
	else if (strcmp(relationship, "fgf") == 0)
	{
		if (pd->people[p2p].father == -1) // if there is no father we need one with questionmark
		{
			Person f = { .name = NULL,.sex = M,.father = -1,.mother = -1,.children = NULL };
			f.name = realloc(f.name, sizeof(char*) * 2);
			f.name[0] = '?';
			f.name[1] = (char)NULL;
			f.sex = M;
			f.father = p1p;
			f.children = realloc(f.children, sizeof(int));
			f.children[0] = p2p;
			f.child_c++;
			// child of f
			pd->people[p2p].father = addPerson(f, pd);
			// parent of f
			pd->people[p1p].child_c++;
			//free(p1p->children);
			//Person** cp = malloc(sizeof(Person*) * p1p->child_c);
			pd->people[p1p].children = realloc(pd->people[p1p].children, sizeof(int) * pd->people[p1p].child_c);
			pd->people[p1p].children[pd->people[p1p].child_c - 1] = pd->people[p2p].father;
		}

		if (pd->people[pd->people[p2p].father].father == -1)
		{
			// set the grandfather
			pd->people[pd->people[p2p].father].father = p1p;
		}
	}

	printf("\n--\n");

	printPeople(pd);

	printf("\n--\n");
}

void drawAll(int count, char** strings, PeopleData* pd)
{
	printf("draw\n");
}

void draw(int count, char** strings, PeopleData* pd)
{

}

void relationship(int count, char** strings, PeopleData* pd)
{

}

void list(int count, char** strings, PeopleData* pd)
{

}

void readInput(int* count, char*** strings)
{
	char in_str[BUFFER_LEN];
	printf("esp>");
	fgets(in_str, BUFFER_LEN, stdin);

	char * pch = NULL;
	pch = strtok(in_str, " \n");
	if (pch != NULL)
	{
		int i = 0;
		do
		{
			(*strings) = (char**)realloc((*strings), (i + 1) * sizeof(char*));
			(*strings)[i] = (char*)malloc((strlen(pch) + 1) * sizeof(char));

			int j;
			for (j = 0; j < (int)(strlen(pch) + 1); j++)
			{
				(*strings)[i][j] = pch[j];
			}

			pch = strtok(NULL, " \n");
			i++;
		} while (pch != NULL);
		*count = i;
	}
}

int addPerson(Person p, PeopleData* pd)
{
	if (strcmp(p.name, "?") == 0)
	{
		p.name = realloc(p.name, sizeof(char*) * 2);
		char number[5];
		sprintf(number, "%d", pd->unknowns);
		if (pd->unknowns < 10)
		{
			p.name = realloc(p.name, sizeof(char*) * 3);
			number[1] = (char)NULL;
		}
		else if (pd->unknowns >= 10)
		{
			p.name = realloc(p.name, sizeof(char*) * 4);
			number[2] = (char)NULL;
		}
		else if (pd->unknowns >= 100)
		{
			p.name = realloc(p.name, sizeof(char*) * 5);
			number[3] = (char)NULL;
		}
		else if (pd->unknowns >= 1000)
		{
			p.name = realloc(p.name, sizeof(char*) * 6);
			number[4] = (char)NULL;
		}
		else
		{
			// hmm
		}
		int i;
		for (i = 0; i < (int)strlen(number); i++)
		{
			p.name[i + 1] = number[i];
		}
		p.name[i + 1] = (char)NULL;
	}

	pd->people_c++;
	pd->people = realloc(pd->people, sizeof(Person) * pd->people_c);
	pd->people[pd->people_c - 1] = p;
	return pd->people_c - 1;
}

int getPerson(Person p, PeopleData* pd)
{
	int i;
	for (i = 0; i < pd->people_c; i++)
	{
		if (strcmp(pd->people[i].name, p.name) == 0)
		{
			if (pd->people[i].sex == p.sex)
			{
				return i;
			}
		}
	}
	return -1;
}

int getPersonByName(char* name, Sex sex, PeopleData* pd)
{
	Person p;
	p.name = name;
	p.sex = sex;
	return getPerson(p, pd);
}

void printPerson(Person* p, PeopleData* pd)
{
	printf("%s, ", p->name);

	if (p->sex == M)
	{
		printf("[m], ");
	}
	else
	{
		printf("[f], ");
	}

	if (p->father != -1)
	{
		printf("father: %s, ", pd->people[p->father].name);
	}

	if (p->mother != -1)
	{
		printf("mother: %s, ", pd->people[p->mother].name);
	}

	if (p->child_c != 0)
	{
		printf("children: %d\n", p->child_c);
		int i;
		for (i = 0; i < p->child_c; i++)
		{
			printf("\t%s", pd->people[p->children[i]].name);
		}
	}

	printf("\n");
}

void printPeople(PeopleData* pd)
{
	int i;
	for (i = 0; i < pd->people_c; i++)
	{
		printPerson(&pd->people[i], pd);
	}
}