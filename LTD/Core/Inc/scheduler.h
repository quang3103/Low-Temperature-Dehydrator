/*
 * scheduler.h
 *
 *  Created on: Dec 24, 2020
 *      Author: MSI-Modern
 */

typedef struct node {
	void (*pTask)(void);
	uint32_t delay;
	uint32_t period;
	uint32_t remain;
	uint8_t runMe;
	struct node *next;
} node;

typedef struct timeNode {
	uint32_t time;
	struct timeNode *next;
} timeNode;

void schedulerInit(void);

void insertTaskFirstTime(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD);
void insertTaskBack(void (* pFunction)(), uint32_t PERIOD);

void deleteFirstNode(void);
void dispatchTask(void);
void updateTask(void);
void deleteAllTask(void);
