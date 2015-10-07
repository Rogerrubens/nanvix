struct semaforo {
		unsigned key;
		int val;
		int count;
		struct process * chain;
		pid_t pids[];
		int used;
}

struct semaforo table[1000];

public int semget(unsigned key){
	
	//procurar key na tabela
	for(int i = 0; i < 1000; i++){
		if(table[i].key == key){
			table[i].count++;
			return i;
		}
	}
	
	// caso nao encontre a key, criar um novo 
	// semaforo na primeira posicao vazia da tabela
	for(int i = 0; i < 1000; i++){
		if(table[i].used ==  0){
			table[i].key = key;
			table[i].val = 0;
			table[i].count = 1;
			table[i].chain = NULL;
			table[i].used = 1;
			return i;
		}
	}
	return -1;
}

public int semctl(int id, int cmd, int val){
	if(id >= 0 && id < 1000){
		if(cmd == 0){  			
			return table[id].val;
		}
		else if(cmd == 1 && val >= 0){
			table[id].val = val;
			return 0;
		}
		else if(cmd == 3){  	
			if(table[id].count == 1){
				table[id].used = 0;
				table[id].key = 0;
			}
			else{
				
				table[id].count--;
			}
			return 0;
		}
	}
	return -1;
}

public int semop(int id, int op){
	// testar se id é válido
	if(id >= 0 && id < 1000){
		// chamada para Down
		if(op < 0){		
			// dormir os processos enquanto 
			while(table[id].val == 0){		
				sleep(table[id].chain, PRIO_SEM);
			}		
			table[id].val--;
			return 0;
		}
		// chamada para UP
		else if(op > 0){
			wakeup(table[id].chain);
			table[id].val++;	
			return 0;
		}
	}
	return -1;
}
