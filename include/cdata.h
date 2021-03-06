/*
 * cdata.h: arquivo de inclus�o de uso apenas na gera��o da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida
 *
 * Vers�o de 25/04/2017
 *
 */
#ifndef __cdata__
#define __cdata__

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4
#include <ucontext.h>
/* N�O ALTERAR ESSA struct */
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Cria��o; 1: Apto; 2: Execu��o; 3: Bloqueado e 4: T�rmino
	int 		ticket;		// prioridade do processo (nome errado)
	ucontext_t 	context;	// contexto de execu��o da thread (SP, PC, GPRs e recursos) 
} TCB_t; 

typedef struct join {
	int waitedTid;    //thread that is waiting
	int waitingTid;	  //thread which is being waited
} JOIN;

#endif
