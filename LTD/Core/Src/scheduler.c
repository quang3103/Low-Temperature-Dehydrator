/*
 * scheduler.c
 *
 *  Created on: Dec 24, 2020
 *      Author: MSI-Modern
 */

#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "scheduler.h"

node *head = NULL;

void schedulerInit(void) {
	head = (node *)malloc(sizeof(node));
	head->next = NULL;
}

void insertTaskFirstTime(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
	node *tmp = head;
	int totalTime = 0;

	while ((tmp->next) && (totalTime+tmp->next->delay < DELAY)) {
		totalTime += tmp->next->delay;
	    tmp = tmp->next;
	}

	node *newNode = (node *)malloc(sizeof(node));

	newNode->pTask = pFunction;
	newNode->delay = DELAY - totalTime;
	newNode->period = PERIOD;
	if (newNode->delay) newNode->runMe = 0;
	else newNode->runMe = 1;

	newNode->next = tmp->next;
	tmp->next = newNode;

	if (newNode->next) {
		newNode->next->delay -= newNode->delay;
		newNode->next->runMe = !newNode->next->delay;
	}
}

void insertTaskBack(void (* pFunction)(), uint32_t PERIOD) {
	node *tmp = head;
	int totalTime = 0;

	while ((tmp->next) && (totalTime+tmp->next->delay < PERIOD)) {
		totalTime += tmp->next->delay;
		tmp = tmp->next;
	}

	node *newNode = (node *)malloc(sizeof(node));

	newNode->pTask = pFunction;
	newNode->delay = PERIOD - totalTime;
	newNode->period = PERIOD;
	newNode->runMe = !newNode->delay;

	newNode->next = tmp->next;
	tmp->next = newNode;

	if (newNode->next) {
		newNode->next->delay -= newNode->delay;
		newNode->next->runMe = !newNode->next->delay;
	}
}

void deleteFirstNode(void) {
	node *deleteNode = head->next;
	node *savedNode = deleteNode->next;
	head->next = savedNode;
	free(deleteNode);
}

void dispatchTask(void) {
	void (*task)(void);
	uint32_t savedPeriod;
	while (head->next && head->next->runMe) {
		(*head->next->pTask)();
		task = head->next->pTask;
		savedPeriod = head->next->period;
		//savedDelay = head->next->delay;
		deleteFirstNode();
		if (savedPeriod) {
			insertTaskBack(task, savedPeriod);
		}
	}
}

void updateTask(void) {
	if (head->next) {
		head->next->delay -= 1;
		head->next->runMe = !(head->next->delay);
	}
}

void deleteAllTask(void) {
	while (head->next) deleteFirstNode();
}
