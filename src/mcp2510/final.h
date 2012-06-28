#ifndef _EL_CONSTANTS_H_
#define _EL_CONSTANTS_H_


//状态们

//Car status
#define EL_CAR_STOP (0)
#define EL_CAR_STARTING (1)
#define EL_CAR_RUNNING (2)

//Inner light status
#define EL_LIGHT_ON (3)
#define EL_LIGHT_OFF (4)
#define EL_LIGHT_ALWAYS_ON (5)

//Door status
#define EL_DOOR_OPEN (6)
#define EL_DOOR_CLOSE (7)
#define EL_DOOR_LOCKED (8)


//地址表示

//MCU's id
#define EL_MCU (0)
//Light's id
#define EL_LIGHT (1)
//Main door's id
#define EL_MAIN_DOOR (2)
//Sub door's id
#define EL_SUB_DOOR (3)


//位表示？
#define EL_BIT_TO (0)	//[0]: to whom?
#define EL_BIT_FROM (1)	//[1]: from whom?
#define EL_BIT_OP (2)	//[2]: operation
#define EL_BIT_PARAM (3)//[3]: parameter 
//[4]: 
//[5]: 
//[6]: 
//[7]: 



#define TRUE (1)
#define FALSE (0)


//操作

//from light
#define EL_OP_LIGHT_SET_ALWAYS_ON (0)

//from MCU
#define EL_OP_MCU_SET_LIGHT_ONOFF (0)


//data数组的长度为8！
void send(int dev, char* data);

typedef void (*sighandler_t)(int);

#endif
