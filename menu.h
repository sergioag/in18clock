#ifndef IN18CLOCK_MENU_H
#define IN18CLOCK_MENU_H

#define NO_PARENT	-1
#define	NO_CHILD	-1
#define NO_LOAD		-1

typedef struct menu_struct
{
	int 	parent;
	int 	firstChild;
	int 	lastChild;
	int 	value;
	int 	minValue;
	int 	maxValue;
	int 	eepromOffset;
	int 	blinkPattern;
	void	(*displayHandler)();
	boolean (*onEditHandler)();
	void	(*onIncrementHandler)();
	void	(*onDecrementHandler)();
	void	(*onSaveHandler)();
	void	(*onShowHandler)();
};

void menuSetup(menu_struct *new_menu, int numElements);
int menuGetCurrentPosition();
void menuUpdate();
void menuSetValue(int index, int value);
int menuGetValue(int index);
void menuSetBlinkPattern(int index, int blinkPattern);
int menuSave(int index);

#endif //IN18CLOCK_MENU_H
