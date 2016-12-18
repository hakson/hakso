#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DEBUG
#define BUFFER_LEN 100

typedef int bool;
#define FALSE (0)
#define TRUE (!FALSE)

typedef enum _Sex_
{
	M = 0,
	F = 1
} Sex;

typedef struct _Person_
{
	char* name;
	Sex sex;
	union
	{
		struct
		{
			struct _Person_* father;
			struct _Person_* mother;
		};
		struct _Person_* parents[2];
	};
	struct _Person_** children;
	int child_c;
} Person;

typedef struct _PeopleData_
{
	Person** people;
	int people_c;
	int unknowns;
} PeopleData;


void add(int count, char** strings, PeopleData* pd);
void drawAll(int count, char** strings, PeopleData* pd);
void draw(int count, char** strings, PeopleData* pd);
void relationship(int count, char** strings, PeopleData* pd);
void list(int count, char** strings, PeopleData* pd);
void readInput(int* count, char*** strings);
void addPerson(Person* p, PeopleData* pd);
bool mergePeople(Person * unknown, Person * p1, Sex relation, PeopleData * pd);
void removePerson(Person* p, PeopleData* pd);
Person* getPerson(const Person p, const PeopleData* pd);
Person* getPersonByName(char* name, const Sex sex, const PeopleData* pd);
void printPerson(const Person* p);
void printPeople(const PeopleData* pd);
void sortPeople(PeopleData * pd);
int sortByName(const void * a, const void * b);


int main(int argc, char* argv)
{
	PeopleData pd;
	pd.people = NULL;
	pd.people_c = 0;
	pd.unknowns = 0;

	//todo
	//parsing of parameters

	// main program loop
	while (TRUE)
	{
		int i;
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
		else if (strcmp(strings[0], "list") == 0)
		{
			list(count, strings, &pd);
		}
		else if (strcmp(strings[0], "quit") == 0)
		{
			for (i = 0; i < count; i++)
			{
				free(strings[i]);
			}
			free(strings);

			break;
		}

		// free and reset
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

	Person* p1 = calloc(1, sizeof(Person));
	Person* p2 = calloc(1, sizeof(Person));

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
			p1->sex = M;
			names_start[0] = 1;
			names_end[0] = i;
			break;
		}
		if (strcmp(strings[i], "[f]") == 0)
		{
			p1->sex = F;
			names_start[0] = 1;
			names_end[0] = i;
			break;
		}
	}

	for (i = names_end[0] + 2; i < count; i++)
	{
		if (strcmp(strings[i], "[m]") == 0)
		{
			p2->sex = M;
			names_start[1] = names_end[0] + 2;
			names_end[1] = i;
			break;
		}
		if (strcmp(strings[i], "[f]") == 0)
		{
			p2->sex = F;
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

	p1->name = names[0];
	p2->name = names[1];

	// relationship
	Person* temp;
	char* relationship = strings[names_end[0] + 1];
	if (strcmp(relationship, "father") == 0 || strcmp(relationship, "mother") == 0)
	{
		Sex parent = strcmp(relationship, "father") != 0;
		if (parent != p1->sex)
		{
			// todo sex doesnt fit
			return;
		}

		if (getPerson(*p2, pd) == NULL)
		{
			addPerson(p2, pd);
			if (getPerson(*p1, pd) == NULL) // both unknown
			{
				addPerson(p1, pd);
			}
			else // parent known
			{
				temp = p1;
				p1 = getPerson(*p1, pd);
				free(temp->name);
				free(temp->children);
				free(temp);
			}
			p1->child_c++;
			p1->children = realloc(p1->children, sizeof(Person*) * p1->child_c);
			p1->children[p1->child_c - 1] = p2;
			p2->parents[parent] = p1;
		}
		else // child known
		{
			temp = p2;
			p2 = getPerson(*p2, pd);
			free(temp->name);
			free(temp->children);
			free(temp);

			if (getPerson(*p1, pd) == NULL) // parent unknown
			{
				if (p2->parents[parent] == NULL)
				{
					addPerson(p1, pd);
					p1->child_c++;
					p1->children = realloc(p1->children, sizeof(Person*) * p1->child_c);
					p1->children[p1->child_c - 1] = p2;
					p2->parents[parent] = p1;
				}
				else
				{
					assert(p2->parents[parent]->name != NULL);
					if (p2->parents[parent]->name[0] == '?')
					{
						free(p2->parents[parent]->name);
						p2->parents[parent]->name = p1->name;
						free(p1->children); // probably not necesary
						free(p1);
						p1 = p2->parents[parent];
					}
					else
					{
						// todo imposible relationship
						return;
					}
				}
			}
			else // both are known
			{
				temp = p1;
				p1 = getPerson(*p1, pd);
				free(temp->name);
				free(temp->children);
				free(temp);

				if (p2->parents[parent] == NULL)
				{
					p2->parents[parent] = p1;
					p1->child_c++;
					p1->children = realloc(p1->children, sizeof(Person*) * p1->child_c);
					p1->children[p1->child_c - 1] = p2;
				}
				else
				{
					assert(p2->parents[parent]->name != NULL);
					if (p2->parents[parent]->name[0] == '?')
					{
						// Complex merger yikes
						mergePeople(p2->parents[parent], p1, parent, pd);
					}
					else
					{
						//  impossible relationship
						return;
					}
				}
			}
		}
	}
	else if (strcmp(relationship, "fgf") == 0 ||
		strcmp(relationship, "fgm") == 0 ||
		strcmp(relationship, "mgf") == 0 ||
		strcmp(relationship, "mgm") == 0)
	{
		Sex parent1 = relationship[0] != 'f';
		Sex parent2 = relationship[2] != 'f';
		if (parent2 != p1->sex)
		{
			// sex doesnt fit
			return;
		}

		if (getPerson(*p2, pd) == NULL)
		{
			if (getPerson(*p1, pd) == NULL) // both unknown
			{
				addPerson(p1, pd);
			}
			else // great parent known
			{
				temp = p1;
				p1 = getPerson(*p1, pd);
				free(temp->name);
				free(temp->children);
				free(temp);
			}
			addPerson(p2, pd);
			Person* unknown_person = malloc(sizeof(Person));
			unknown_person->name = malloc(sizeof(char) * 2);
			unknown_person->name[0] = '?';
			unknown_person->name[1] = (char)0;
			unknown_person->sex = parent1;
			unknown_person->child_c = 1;
			unknown_person->children = malloc(sizeof(Person*));
			unknown_person->children[0] = p2;
			unknown_person->parents[parent2] = p1;
			unknown_person->parents[!parent2] = NULL;
			addPerson(unknown_person, pd);
			p1->child_c++;
			p1->children = realloc(p1->children, sizeof(Person*) * p1->child_c);
			p1->children[p1->child_c - 1] = unknown_person;
			p2->parents[parent1] = unknown_person;
		}
		else // child known
		{
			if (getPerson(*p1, pd) == NULL) // great parent unknown
			{

			}
			else // both known
			{
				if (p2->parents[parent1] == NULL)
				{
					Person*	unknown_person = malloc(sizeof(Person));
					unknown_person->name = malloc(sizeof(char) * 2);
					unknown_person->name[0] = '?';
					unknown_person->name[1] = (char)0;
					unknown_person->sex = parent1;
					unknown_person->child_c = 1;
					unknown_person->children = malloc(sizeof(Person*));
					unknown_person->children[0] = p2;
					unknown_person->parents[parent2] = p1;
					unknown_person->parents[!parent2] = NULL;
					addPerson(unknown_person, pd);
					p1->child_c++;
					p1->children = realloc(p1->children, sizeof(Person*) * p1->child_c);
					p1->children[p1->child_c - 1] = unknown_person;
					p2->parents[parent1] = unknown_person;
				}
				else
				{
					if (p2->parents[parent2] == NULL)
					{
						p2->parents[parent2] = p1;
						p1->child_c++;
						p1->children = realloc(p1->children, sizeof(Person*) * p1->child_c);
						p1->children[p1->child_c - 1] = p2->parents[parent2];
					}
					else
					{
						// error message
					}
				}
			}
		}
	}
#ifdef DEBUG
	printf("\n--\t--\n\n");
	printPeople(pd);
	printf("\n\n--\t--\n\n");
#endif // DEBUG
}

void drawAll(int count, char** strings, PeopleData* pd)
{
	if (count != 2)
	{
		// todo wrong parametercount
		return;
	}

	sortPeople(pd);

	char* filename = malloc((strlen(strings[1]) + 5) * sizeof(char));
	strcpy(filename, strings[1]);
	strcpy(filename + strlen(strings[1]), ".dot");

	FILE* f = fopen(filename, "w+");

	fputs("digraph FamilyTree\n", f);
	fputs("{\n", f);

	int i;
	for (i = 0; i < pd->people_c; i++)
	{
		// write only the name if end of a tree
		// not necesary but like that on all the examples
		if (pd->people[i]->father == NULL && pd->people[i]->mother == NULL)
		{
			fputs("  \"", f);
			fputs(pd->people[i]->name, f);
			if (pd->people[i]->sex == M)
			{
				fputs(" [m]", f);
			}
			else
			{
				fputs(" [f]", f);
			}
			fputs("\";\n", f);
			continue;
		}

		// write the relationships if they have children
		int j;
		for ( j = 1; j > -1; j--)
		{
			if (pd->people[i]->parents[j] == NULL)
			{
				continue;
			}

			fputs("  \"", f);
			fputs(pd->people[i]->name, f);
			if (pd->people[i]->sex == M)
			{
				fputs(" [m]", f);
			}
			else
			{
				fputs(" [f]", f);
			}

			fputs("\" -> \"", f);

			fputs(pd->people[i]->parents[j]->name, f);
			if (pd->people[i]->parents[j]->sex == M)
			{
				fputs(" [m]", f);
			}
			else
			{
				fputs(" [f]", f);
			}

			fputs("\";\n", f);
		}
	}

	fputs("}\n", f);

	fclose(f);

	free(filename);

#ifdef DEBUG
	printf("drawn\n");
#endif // DEBUG

}

void draw(int count, char** strings, PeopleData* pd)
{

}

void relationship(int count, char** strings, PeopleData* pd)
{

}

void list(int count, char** strings, PeopleData* pd)
{
	if (count != 1)
	{
		// todo error message
		return;
	}

	if (pd->people_c == 0)
	{
		// todo error message
		return;
	}

	sortPeople(pd);

	int i;
	for (i = 0; i < pd->people_c; i++)
	{
		printf(pd->people[i]->name);
		printf(" ");
		if (pd->people[i]->sex == M)
		{
			printf("[m]");
		}
		else
		{
			printf("[f]");
		}
		printf("\n");
	}
}

void readInput(int* count, char*** strings)
{
	char in_str[BUFFER_LEN];
	printf("esp> ");
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

void addPerson(Person* p, PeopleData* pd)
{
	if (strcmp(p->name, "?") == 0)
	{
		char number[5];
		pd->unknowns++;
		sprintf(number, "%d", pd->unknowns + 1);
		if (pd->unknowns < 10)
		{
			p->name = realloc(p->name, sizeof(char*) * 3);
			number[1] = (char)0;
		}
		else if (pd->unknowns >= 10)
		{
			p->name = realloc(p->name, sizeof(char*) * 4);
			number[2] = (char)0;
		}
		else if (pd->unknowns >= 100)
		{
			p->name = realloc(p->name, sizeof(char*) * 5);
			number[3] = (char)0;
		}
		else if (pd->unknowns >= 1000)
		{
			p->name = realloc(p->name, sizeof(char*) * 6);
			number[4] = (char)0;
		}
		else
		{
			// hmm
		}

		int i;
		for (i = 0; i < (int)strlen(number); i++)
		{
			p->name[i + 1] = number[i];
		}
		p->name[i + 1] = (char)0;
	}

	pd->people_c++;
	pd->people = realloc(pd->people, sizeof(Person*) * pd->people_c);
	pd->people[pd->people_c - 1] = p;
}

bool mergePeople(Person* unknown, Person* p1, Sex relation, PeopleData* pd)
{
	bool possible = (unknown->father == NULL || p1->father == NULL) ||
		(unknown->mother == NULL || p1->mother == NULL);
	if (!possible)
	{
		// impossible relationship - merger not possible
		return FALSE;
	}

	free(unknown->name);
	int i;
	for (i = 0; i < unknown->child_c; i++)
	{
		unknown->children[i]->parents[relation] = p1;
	}
	p1->children = realloc(p1->children, sizeof(Person*) * (p1->child_c + unknown->child_c));
	for (i = 0; i < unknown->child_c; i++)
	{
		p1->children[i + p1->child_c] = unknown->children[i];
	}
	for (i = 0; i < 2; i++)
	{
		if (unknown->parents[i] != NULL)
		{
			p1->parents[i] = unknown->parents[i];
			int j;
			for (j = 0; j < p1->parents[i]->child_c; j++)
			{
				if (p1->parents[j]->children[j] == unknown)
				{
					p1->parents[j]->children[j] = p1;
				}
			}
		}
	}
	p1->child_c += unknown->child_c;
	free(unknown->children);
	removePerson(unknown, pd);
	free(unknown);

	return TRUE;
}

void removePerson(Person* p, PeopleData* pd)
{
	int index = -1;
	int i;
	for (i = 0; i < pd->people_c; i++)
	{
		if (pd->people[i] == p)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		return;
	}

	pd->people[index] = pd->people[pd->people_c - 1];

	pd->people_c--;
	pd->people = realloc(pd->people, sizeof(Person*) * pd->people_c);
}

Person* getPerson(const Person p, const PeopleData* pd)
{
	int i;
	for (i = 0; i < pd->people_c; i++)
	{
		if (strcmp(pd->people[i]->name, p.name) == 0)
		{
			if (pd->people[i]->sex == p.sex)
			{
				return pd->people[i];
			}
		}
	}
	return NULL;
}

Person* getPersonByName(char* name, const Sex sex, const PeopleData* pd)
{
	Person p;
	p.name = name;
	p.sex = sex;
	return getPerson(p, pd);
}

void printPerson(const Person* p)
{
	if (p->name == NULL)
	{
		return;
	}

	printf("%s, ", p->name);

	if (p->sex == M)
	{
		printf("[m], ");
	}
	else
	{
		printf("[f], ");
	}

	if (p->father != NULL)
	{
		printf("father: %s, ", p->father->name);
	}

	if (p->mother != NULL)
	{
		printf("mother: %s, ", p->mother->name);
	}

	if (p->child_c != 0)
	{
		printf("children: %d\n", p->child_c);
		int i;
		for (i = 0; i < p->child_c; i++)
		{
			printf("\t%s", p->children[i]->name);
		}
	}

	printf("\n");
}

void printPeople(const PeopleData* pd)
{
	int i;
	for (i = 0; i < pd->people_c; i++)
	{
		printPerson(pd->people[i]);
	}
}

void sortPeople(PeopleData* pd)
{
	if (pd->people_c > 1)
	{
		qsort(pd->people, pd->people_c, sizeof(Person**), sortByName);
	}
}

int sortByName(const void * a, const void * b)
{
	Person* p1 = (*((Person**)a));
	Person* p2 = (*((Person**)b));

	int c = strcmp(p1->name, p2->name);
	if (c == 0)
	{
		if (p2->sex == F)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	return c;
}
