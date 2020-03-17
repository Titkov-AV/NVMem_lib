/*
	Author: Titkov_AV
	Date: 12.03.2020
	Theme: NVMem_test
*/
#include <stdio.h>
#include "../inc/main.h"

//--------Prototypes---------//

//void save_context();
//int *ploop1, *ploop3;//, *ploop1, *ploop3;
//float *ploop1, *ploop3;
float *reading1, *ploop1;
int *reading3, *ploop3;

//---------------------------//
int main () {

	printf ("START\n");
	int i;
	init(2000, 100);
	printf ("INIT!\n");

	ploop1 = register_var(1, 20, "test1");
	printf ("test1 registered\n");
	ploop3 = register_var(2, 20, "test3");
	printf ("test3 registered\n");

	(*ploop1) = 46.12;
	(*ploop3) = 424;

	for (i=0; i<=5; i++)
	{
		(*(ploop3+i)) = 232.5*i;
	}
	reading1 = get_var("test1");
	printf ("From RAM %.3f\r\n", (*reading1));
	reading3 = get_var("test3");
	printf ("From RAM %d\r\n", (*(reading3+4)));
	save_context_f("test1");
//	float *ploop2 = get_var("test2");
//	printf ("From RAM %lf\r\n", (*ploop2));
	printf ("END\r\n");

	//block until user types something
	getchar();
	return 0;
};
