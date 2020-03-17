/*
	Author: Platonov_VV Titkov_AV
	Date: 12.03.2020
	Theme: NVMem function
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../inc/NVMem.h"

const char *copy = "elesy";
const char *ver = "0.0.1";
//-----------Prototypes----------//
int init(int descr_size, int var_size);
void *register_var(tvar akind, int maxcnt, char *aname);
//int *register_var(tvar akind, int maxcnt, char *aname);
void *get_var(char *aname);

static int load_header(char *filename);
static int save_header(char *filename, nvmem_header_t *p);
static int reload_struct(rl_reas reason);
static int copy_header(nvmem_header_t *p);
int save_context_f(char *aname);
//-------------------------------//

nvmem_header_t *pnvvars;			//указатель на начало всей области

//descr_size - размер области описания структуры
//var_size - размер области под переменные
int init(int descr_size, int var_size)
{		
	//читаем файлы или области памяти, возможно необходимо востановление
	int file_ok;
#ifdef PC
	if ((file_ok=load_header("copy1.dat"))==1){
		if ((file_ok=load_header("copy2.dat"))==1){
			goto def;	//файл отсутствует, начальная инициализация
		}
		else{
			reload_struct(from_c2);			//присутствует только copy2
			return 0;
		}
	}
	else{
		if ((file_ok=load_header("copy2"))==1){
			reload_struct(from_c1);			//присутствует только copy1
			return 0;
		}
		else {
			reload_struct(from_last);		//присутствует оба
			return 0;
		}
		return 0;
	}
#endif

#ifdef MC

#endif
def:
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
	strcpy(pnvvars->copyright, copy);
	strcpy(pnvvars->version, ver);

	pnvvars->struct_cnt = 0;				//счетчик записанных данных
	pnvvars->descript_size = descr_size;	//размер выделенной области под descrip
	pnvvars->vars_size = var_size;			//размер выделенной области под vars
	//максимальное количество дескрипторов, которое войдет в выделенную область
	pnvvars->descript_maxcnt = (descr_size - sizeof(nvmem_header_t))/sizeof(descr_item_t);

	//указатель на начало расположения дискрипторов, располагается сразу за header-ом
	pnvvars->begin_descript = (void *)pnvvars + sizeof(nvmem_header_t);

	//указатели на начало, конец, и текущее положение переменных
	pnvvars->begin_vars = (void *)(pnvvars->begin_descript) + pnvvars->descript_maxcnt*sizeof(descr_item_t);
	pnvvars->end_vars = (void *)(pnvvars->begin_vars) + var_size;
	pnvvars->current_end_vars = pnvvars->begin_vars;

	//количество зарегистрированный переменных
	pnvvars->lastitem=-1;				//-1 items отсутствуют

	//создаем файл 1 и 2, храним 2 копии


//	load_header("testing.dat");
	save_header("copy1.dat", pnvvars);
	pnvvars->struct_cnt++;
	save_header("copy2.dat", pnvvars);

/*
	FILE * fp3;
	fp3 = fopen ("file.txt", "w+");
	fprintf(fp3, "%s %s %s %d", "We", "are", "in", 2012);
	fclose(fp3);

 	FILE *fp1;
	fp1=fopen("1.txt", "w+");
	printf(fp1, "%s", "1");
	fclose(fp1);

	FILE *fp1, *fp2;
	if ((fp1=fopen("1", "w+b"))==NULL){
			printf ("Cannot open file.\n");
			exit(1);
		}
		else{
			fprintf("1st file", (*pnvvars));
		}
	if ((fp2=fopen("2", "w+b"))==NULL){
		printf ("Cannot open file.\n");
		exit(1);
	}
	else{
		fprintf(fp2, pnvvars);
	}*/
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
	//не работает!
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
	return 0;		//имя не найдено, возвратить указатель на начало памяти
};

//сохранение контекста по имени в файл
int save_context_f(char *aname){

/*	 var_type;
	 max_arr;
	void *pdata;	*/					//указатель на данные переменной

	//получаем указатель описание сохраняемой переменной
	int i;
	descr_item_t *item_discr;
	char *read_name;
	for (i=-1; i<pnvvars->lastitem; i++){
		item_discr = (void*)pnvvars->begin_descript + sizeof(descr_item_t)*(i+1);
		read_name = (item_discr->name_var);
		if (strcmp(aname, read_name) == 0)
			break;
		else continue;
	}
	tvar var_type = item_discr->kind;
	int max_arr = item_discr->maxarraycnt;
	void *pdata = item_discr->var;

//	FILE *fopen()
	return 0;
}

static int save_header(char *filename, nvmem_header_t *p){

	FILE *fp;
	char *c;
	int size = sizeof(nvmem_header_t);

	if ((fp = fopen(filename, "wb"))==NULL)
	{
		printf("Error occured while opening file");
		return -1;
	}
	//устанавливаем указатель на начало структуры
	c = (char*)p;
	//посимвольно записываем в файл структуру
	for(int i=0; i<size; i++)
	{
		putc(*c++, fp);
	}
	fclose(fp);

	return 0;
};

static int load_header(char *filename){

	FILE *fp;
	char *c;
	int i;

	int size = sizeof(nvmem_header_t);
	// выделяем память для считываемой структуры
	nvmem_header_t * ptr = (nvmem_header_t*) malloc(size);

	if ((fp = fopen(filename, "rb")) == NULL)
	    {
	        printf("Error occured while opening file");
	        return 1;
	    }

	// устанавливаем указатель на начало блока выделенной памяти
	c = (char *)ptr;

	// считываем посимвольно из файла
	while ((i = getc(fp))!=EOF)
	{
		*c = i;
		c++;
	}

	fclose(fp);
/*	// вывод на консоль загруженной структуры
	printf("%s %5d \n", ptr->copyright, ptr->lastitem);
	free(ptr);*/
	return 0;
};

nvmem_header_t *ld_phead(char *filename){

	FILE *fp;
	char *c;
	int i;

	int size = sizeof(nvmem_header_t);
	// выделяем память для считываемой структуры
	nvmem_header_t * ptr = (nvmem_header_t*) malloc(size);

	if ((fp = fopen(filename, "rb")) == NULL)
	    {
	        printf("Error occured while opening file");
	        return NULL;
	    }

	// устанавливаем указатель на начало блока выделенной памяти
	c = (char *)ptr;

	// считываем посимвольно из файла
	while ((i = getc(fp))!=EOF)
	{
		*c = i;
		c++;
	}

	fclose(fp);
	return ptr;
};

static int reload_struct(rl_reas reason){

	nvmem_header_t *p1, *p2;

	switch(reason){
		case from_c1:
			p1 = ld_phead("copy1.dat");
			//выделяем память
			pnvvars = (nvmem_header_t*)calloc(1, p1->descript_size + p1->vars_size);
			if(!pnvvars) {
				printf ("Allocation failure.");
				exit (1);
			}
			pnvvars = p1;
			//copy_header(p1);

			//создаем copy2
			break;
		case from_c2:
			break;
		case from_last:
			break;
	}
	return 0;
};

static int copy_header(nvmem_header_t *p){

	for (int i=0; i<NAME_LONG; i++){
		pnvvars->copyright[i] = p->copyright[i];
	}
	for (int i=0; i<NAME_LONG; i++){
		pnvvars->version[i] = p->version[i];
		}
	pnvvars->struct_cnt = p->struct_cnt;
	pnvvars->descript_size = p->descript_size;
	pnvvars->vars_size = p->vars_size;
	pnvvars->descript_maxcnt = p->descript_maxcnt;
	pnvvars->begin_descript = p->begin_descript;
	pnvvars->begin_vars = p->begin_vars;
	pnvvars->end_vars = p->end_vars;
	pnvvars->current_end_vars = p->current_end_vars;
	pnvvars->lastitem = p->lastitem;

	return 0;
};










