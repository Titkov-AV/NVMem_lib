/*
	Author: Platonov_VV Titkov_AV
	Date: 12.03.2020
	Theme: NVMem lib 
*/

#ifndef NVMEM_H_
#define NVMEM_H_
//-------------------------------

//-----------Prototypes----------//
//nvmem_t *init_mem();					//Функция инициализации области ОЗУ под структуру для хранения в ЭНП
void get_mem();						//Функция получения данных из ЭНП

//-------------------------------//
//тип регистрируемой переменной
typedef enum {
	tfloatvar=0,
	tintvar=1,
	tfloatarr=2,
	tintarr=3
} tvar;

//структура описания item
typedef struct {
	char name_var[50];		//Имя переменной
	tvar kind;				//Тип
	int maxarraycnt;		//Максимальное количество элементов массива
	void *var;				//указатель на значение
} descr_item_t;

//структура данных для хранения в ЭНП
typedef struct {
	char copyright[50];
	char version[50];
	int changed_struct;
	int descript_size;
	int descript_maxcnt;
	descr_item_t *begin_descript;	//указатель на начало расположения дискрипторов
	void *begin_vars;				//начало области с переменными
	void *end_vars;					//конец выделенной области по calloc
	void *current_end_vars;			//конец  области уже занятой под переменные
	int lastitem;					//индекс последнего зарегистрированного элемента
} nvmem_header_t;

extern int init(int descr_size, int var_size);
extern void *register_var(tvar akind, int maxcnt, char *aname);
//extern int *register_var(tvar akind, int maxcnt, char *aname);
extern void *get_var(char *aname);
//-------------------------------
#endif /* NVMEM_H_ */
