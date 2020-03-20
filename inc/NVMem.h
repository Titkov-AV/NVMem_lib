/*
	Author: Platonov_VV Titkov_AV
	Date: 12.03.2020
	Theme: NVMem lib 
*/

#ifndef NVMEM_H_
#define NVMEM_H_
//-------------------------------

#define PC 1

#define NAME_LONG 50
//-----------Prototypes----------//


//-------------------------------//
//тип регистрируемой переменной
typedef enum {
	tfloatvar=0,
	tintvar=1,
	tfloatarr=2,
	tintarr=3
} tvar;

//файл востановления системы
typedef enum{
	from_c1 = 1,		//файл copy1 отсутсвует
	from_c2,			//файл copy2 отсутсвует
	from_last			//последний сохраненный
} rl_reas;

//структура описания item
typedef struct {
	char name_var[NAME_LONG];		//Имя переменной
	tvar kind;				//Тип
	int maxarraycnt;		//Максимальное количество элементов массива
	void *var;				//указатель на значение
} descr_item_t;

//структура данных для хранения в ЭНП
typedef struct {
	char copyright[NAME_LONG];
	char version[NAME_LONG];
	int struct_cnt;					//счетчик для хранения данных
	int file_mark;					//признак файла, 0 - copy1, 1 - copy2
	int descript_size;
	int vars_size;
	int descript_maxcnt;
	int lastitem;					//индекс последнего зарегистрированного элемента
	descr_item_t *begin_descript;	//указатель на начало расположения дискрипторов
	void *begin_vars;				//начало области с переменными
	void *end_vars;					//конец выделенной области по calloc
	void *current_end_vars;			//конец  области уже занятой под переменные
} nvmem_header_t;

extern int init(int descr_size, int var_size);
extern void *register_var(tvar akind, int maxcnt, char *aname);
//extern int *register_var(tvar akind, int maxcnt, char *aname);
extern void *get_var(char *aname);
extern int save_context_f();
//-------------------------------
#endif /* NVMEM_H_ */
