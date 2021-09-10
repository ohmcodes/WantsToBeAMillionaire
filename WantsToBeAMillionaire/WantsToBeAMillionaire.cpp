// WantsToBeAMillionaire.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h> 
//access function

struct NodeQuestion {
	char nQuestion[256];
	char nChoices[4][256];
	char nAnswer;
	struct NodeQuestion* next;
	struct NodeQuestion* prev;
};

struct NodeQuestion* head;

FILE* QuestionFile;
FILE* AnswerFile;

//clearbuffer for scanf (if has input error it will only display 1 error not redundant)
void CleanBuffer() {
	int n;
	while ((n = getchar()) != EOF && n != '\n');
}

//Convert Answer to array index of Answer arrays
int AnswerToIndex(char Answer)
{
	int iAnswer;

	switch (toupper(Answer))
	{
	case 'A':
		iAnswer = 0;
		break;
	case 'B':
		iAnswer = 1;
		break;
	case 'C':
		iAnswer = 2;
		break;
	case 'D':
		iAnswer = 3;
		break;
	default:
		iAnswer = 0;
	}

	return iAnswer;
}

struct NodeQuestion* GetNewNodeQuestion(char Question[256], char Answer, char ChoicesArray[4][256], int ChoicesSize) {
	struct NodeQuestion* newNode = (struct NodeQuestion*)malloc(sizeof(struct NodeQuestion));

	//Insert Question
	strcpy_s(newNode->nQuestion, Question);
	//Insert Choices
	for (int i = 0; i < ChoicesSize; i++)
	{
		//debug
		//printf("Node Ins %s\n", ChoicesArray[i]);
		strcpy_s(newNode->nChoices[i], ChoicesArray[i]);
	}
	//Insert Answer
	newNode->nAnswer = Answer;

	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

void InsertNodeQuestion(char Question[256], char Answer, char ChoicesArray[4][256], int ChoicesSize) {
	struct NodeQuestion* temp = head;

	struct NodeQuestion* newNode = GetNewNodeQuestion(Question, Answer, ChoicesArray, ChoicesSize);
	if (head == NULL) {
		head = newNode;
		return;
	}
	while (temp->next != NULL)
	{
		temp = temp->next; // Go To last Node
	}
	temp->next = newNode;
	newNode->prev = temp;
}

void InsertQuestion()
{
	char YesNo = 'y';
	char Question[256];
	char ChoicesArray[4][256];
	char ChoicesSize = 0;
	char Answer;
	char Letters[4]= { 'A', 'B', 'C', 'D' };
	int err = 0;

	do
	{
		//check if the condtion still no and doesnt have errors
		if (YesNo == 'y' && err == 0)
		{
			printf("Enter new Question: ");
			scanf_s(" %[^\n]s", Question, 256);

			//Allow user to skipp adding data
			if (strcmp(Question,"cancel")==0)
			{
				goto AddmoreQ;
			}

			//declare size of array coz it will disappear when you pass it to a function
			ChoicesSize = sizeof(Letters) / sizeof(Letters[0]);

			printf("Enter desired answer for A,B,C and D \n");

			//loop for inserting choices
			for (int i = 0; i < ChoicesSize; i++)
			{
				printf("Choice %c: ", Letters[i]);
				scanf_s(" %[^\n]s", &ChoicesArray[i], 256);

				//Allow user to skipp adding data
				if (strcmp(ChoicesArray[i], "cancel") == 0)
				{
					goto AddmoreQ;
				}
			}

			//debug
			/*
			for (int i = 0; i < ChoicesSize; i++)
			{
				printf("Choice %s: \n", ChoicesArray[i]);
			}
			*/

			do
			{
				printf("What is the answer? Choose from (A,B,C,D) upper or lower doesn't matter. ");
				scanf_s(" %c", &Answer, 1);

				if (!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'))
				{
					printf("Invalid input please try again.\n");
				}

				CleanBuffer();

			} while(!(toupper(Answer) >= 'A' && toupper(Answer) <= 'D'));

			//debug
			//printf("Answer in int: %i\n", iAnswer);

			//Insert data to node
			InsertNodeQuestion(Question, Answer, ChoicesArray, ChoicesSize);
		}
		
		//catch cancel function
		AddmoreQ:

		printf("Add more question? (y|n): ");
		scanf_s(" %c", &YesNo, 1);

		//validate if the input has error
		if (YesNo != 'y' && YesNo != 'n')
		{
			err = 1;
			printf("Invalid input please try again.\n");
		}
		else
		{
			err = 0;
		}

		CleanBuffer();
	} while (YesNo != 'n');
}

void Print(char header[256]) {
	struct NodeQuestion* temp = head;
	char Letters[4] = { 'A', 'B', 'C', 'D' };

	if (header != NULL)
	{
		printf("%s \n", header);
	}

	while (temp != NULL) {
		printf("Question: %s \n", temp->nQuestion);

		for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
		{
			printf("Choice %c: %s\n", Letters[i], temp->nChoices[i]);
		}

		printf("Answer: %c\n", temp->nAnswer);

		temp = temp->next;
	}
	
	printf("\n");
}

const char* getfield(char* line, int num)
{
	char* token = NULL;
	char* next_token = NULL;

		//initiate line
	for (token = strtok_s(line, ";", &next_token);
		//check if not null
		token && *token;
		//remove newline when detects
		token = strtok_s(NULL, ";\n", &next_token))
	{
		if (!--num)
			return token;
	}
}

void InitCSV()
{
	FILE* fp;
	fopen_s(&fp, "Questions.csv", "r");
	char line[1024];
	int i = 0;
	char Question[256];
	char ChoicesArray[4][256];
	char Answer;
	if (fp)
	{
		while (fgets(line, 1024, fp))
		{
			int k = 0;
			if(i > 0)
			{
				/*
				for (int j = 1; j < 7; j++)
				{
					char* tmp = _strdup(line);
					if (j == 6)
					{
						printf("%s", getfield(tmp, j));
					}
					else
					{
						printf("%s,", getfield(tmp, j));
					}
					
					// NOTE strtok clobbers tmp
					free(tmp);
				}
				*/

				//construct choices array
				for (int j = 1; j < 7; j++)
				{
					char* tmp = _strdup(line);
					//printf("%i\n", j);
					if (j == 1)
					{
						strcpy_s(Question, getfield(tmp, j));
						//printf("%s\n", Question);
						//printf("%s", getfield(tmp, j));
					}
					else if (j == 2)
					{
						Answer = getfield(tmp, j)[0];
						//printf(",%c\n", Answer);
					}
					else
					{
						//printf(",%s\n", getfield(tmp, j));
						//printf("INDEX %i\n", k);
						strcpy_s(ChoicesArray[k], getfield(tmp, j));
						//printf(",%s\n", ChoicesArray[k]);
						k++;
					}

					//printf("%s", getfield(tmp, i));
					free(tmp);
				}

				//debug
				/*
				printf("%s\n", Question);
				printf("%c\n", Answer);
				printf("%s\n", ChoicesArray[0]);
				printf("%s\n", ChoicesArray[1]);
				printf("%s\n", ChoicesArray[2]);
				printf("%s\n", ChoicesArray[3]);
				*/
				

				//Insert data to node
				InsertNodeQuestion(Question, Answer, ChoicesArray, 4);

				//printf("\n");
				//printf("------------------------------\n");
			}
			i++;
		}
		fclose(fp);
	}
}

void CreateCSV() {
	FILE* fp;
	struct NodeQuestion* temp = head;

	fopen_s(&fp, "Questions.csv", "w+");

	fprintf(fp, "Question,Answer,cA,cB,cC,cD");

	while (temp != NULL) {
		fprintf(fp, "\n%s", temp->nQuestion);
		fprintf(fp, ";%s", temp->nAnswer);

		for (int i = 0; i < sizeof(temp->nChoices) / sizeof(temp->nChoices[0]); i++)
		{
			fprintf(fp, ";%s", temp->nChoices[i]);
		}
		temp = temp->next;
	}

	fclose(fp);
}

void EditQuestion()
{

}

void DeleteQuestion()
{

}

int main()
{
	InitCSV();

	//InsertQuestion();
	Print(NULL);

	//char test[4][6][256] = { { "test123", "B", "123", "qwe", "asd", "zxc" }, { "test321", "B", "123", "qwe", "asd", "zxc" }, { "test456", "B", "123", "qwe", "asd", "zxc" } };
	//system("cls");
	//CreateCSV();
}
