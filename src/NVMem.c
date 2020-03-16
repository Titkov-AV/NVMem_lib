/*
	Author: Platonov_VV Titkov_AV
	Date: 12.03.2020
	Theme: NVMem function
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../inc/NVMem.h"


//-----------Prototypes----------//
int init(int descr_size, int var_size);
void *register_var(tvar akind, int maxcnt, char *aname);
//int *register_var(tvar akind, int maxcnt, char *aname);
void *get_var(char *aname);
//-------------------------------//

nvmem_header_t *pnvvars;			//указатель на начало всей области

//descr_size - размер области описания структуры
//var_size - размер области под переменные
int init(int descr_size, int var_size)
{		
	int i;
	if(sizeof(nvmem_header_t)>descr_size)
		return -1;
	if(sizeof(var_size)<4)
		return -2;

	//выделяем память
	pnvvars = (nvmem_header_t*)calloc(1,descr_size + var_size);
	if(!pnvvars) {
		printf ("Allocation failure.");
	//	exit (1);
	}

	//заполняем header
	for (i=0; i<=50; i++){
		pnvvars->copyright[i] = 5*i;
	}
	pnvvars->changed_struct = 1;			//флаг инициализации
	pnvvars->version[0] = 0;
	pnvvars->descript_size = descr_size;	//размер выделенной области под descrip

	//максимальное количество дескрипторов, которое войдет в выделенную область
	pnvvars->descript_maxcnt = (descr_size - sizeof(nvmem_header_t))/sizeof(descr_item_t);

	//указатель на начало расположения дискрипторов, располагается сразу за header-ом
	pnvvars->begin_descript = (void *)pnvvars + sizeof(nvmem_header_t);

	//указатели на начало, конец, и текущее положение переменных
	pnvvars->begin_vars = (void *)(pnvvars->begin_descript) + pnvvars->descript_maxcnt*sizeof(descr_item_t);
	pnvvars->end_vars = (void *)(pnvvars->begin_vars) + var_size;
	pnvvars->current_end_vars = pnvvars->begin_vars;

	pnvvars->lastitem=-1;				//-1 items отсутствуют
	return 0;
}

//регистрирует переменную, возвращает указатель на область её расположения либо код ошибки (код ошибки <0)
//akind - тип
//maxcnt - максимальное количество элементов в массиве item
//*aname - желаемое имя item
//int *register_var(tvar akind, int maxcnt, char *aname)
void *register_var(tvar akind, int maxcnt, char *aname)
{
	//добавить
		///проверка на оставшееся место под переменную
	////////////////////////////
	int varsize;
	//вычисляем размер для хранимых данных
/*	switch(akind){
		case tfloatvar:
			varsize = sizeof(float);
		break;
		case tintvar:
			varsize = sizeof(int);
		break;
		case tfloatarr:
			varsize = sizeof(float)*maxcnt;
		break;
		case tintarr:
			varsize = sizeof(int)*maxcnt;
		break;
		default: return -2; //ошибка типа данных
	}*/
	
	//вычисляем размер для хранимых данных
	if (akind == 0)
		varsize = sizeof(float);
	else if (akind == 1)
		varsize = sizeof(int);
	else if (akind == 2)
		varsize = sizeof(float)*maxcnt;
	else if (akind == 3)
		varsize = sizeof(int)*maxcnt;
	else
		return -2;

	pnvvars->lastitem++;
	if(pnvvars->lastitem > pnvvars->descript_maxcnt){
		pnvvars->lastitem--;
		return -1;
	}

	//указатель на создаваемый item
	descr_item_t *pitem = (void*)(pnvvars->begin_descript) + pnvvars->lastitem*sizeof(descr_item_t);
	pitem->kind=akind;
	strcpy(pitem->name_var, aname);
	pitem->maxarraycnt = maxcnt;
	pitem->var = pnvvars->current_end_vars;
	pnvvars->current_end_vars += varsize;
	return pitem->var;
}

//возвращает указатель на зарегистрированную переменную либо код ошибки (код ошибки <0)
//*aname - запрашиваемое имя item
void *get_var(char *aname)
{
	int i;
	descr_item_t *item_adr;
	char *read_name;
	for (i=-1; i<pnvvars->lastitem; i++){
		item_adr = (void*)pnvvars->begin_descript + sizeof(descr_item_t)*(i+1);
		read_name = (item_adr->name_var);
		if (strcmp(aname, read_name) == 0)
			return item_adr->var;	
		else continue;
	}
	return -1;		//имя не найдено
};
