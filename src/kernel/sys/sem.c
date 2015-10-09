#include <nanvix/pm.h>
#include <sys/sem.h>

struct semaforo table[MAX_TAM];

int sys_semget(unsigned key){
	
	//procurar key na tabela
	for(int i = 0; i < MAX_TAM; i++){
		if(table[i].key == key && table[i].used == 1){
			table[i].count++;
			return i;
		}
	}
	
	// caso nao encontre a key, criar um novo 
	// semaforo na primeira posicao vazia da tabela
	for(int i = 0; i < MAX_TAM; i++){
		if(table[i].used ==  0){
			table[i].key = key;
			table[i].val = 0;
			table[i].count = 1;
			table[i].chain = NULL;
			table[i].used = 1;
			for(int j = 0; j < PROC_MAX; j++){
				if(table[j].pids[i] == -1){
					table[j].pids[i] = curr_proc->pid;
					break;
				}
			}
			return i;
		}
	}
	
	// incluir id do processo corrente no vetor de processos do semáforo	
	
	
	return -1;
}

/*
 * Destruir semáforo
 */ 
void destruct(int id){
	table[id].used = 0;
	table[id].key = 0;
	
	
}

int sys_semctl(int id, int cmd, int val){
	if(id >= 0 && id < MAX_TAM){
		if(cmd == GETVAL){  			
			return table[id].val;
		}
		else if(cmd == SETVAL && val >= 0){
			table[id].val = val;
			return 0;
		}
		else if(cmd == IPC_RMID){  	
			if(table[id].count == 1){
				destruct(id);
				// deletar id do processo corrente no vetor de processos do semáforo	
				for(int i = 0; i < PROC_MAX; i++){
					if(table[id].pids[i] == curr_proc->pid){
						table[id].pids[i] = -1;
						break;
					}
				}		
			}
			else{
				table[id].count--;
			}
			return 0;
		}
	}
	return -1;
}

int sys_semop(int id, int op){
	// testar se id é válido
	if(id >= 0 && id < MAX_TAM){
		// chamada para Down
		if(op < 0){		
			// dormir os processos enquanto 
			while(table[id].val == 0){		
				sleep(&table[id].chain, PRIO_SEM);
			}		
			table[id].val--;
			return 0;
		}
		// chamada para UP
		else if(op > 0){
			wakeup(&table[id].chain);
			table[id].val++;	
			return 0;
		}
	}
	return -1;
}
