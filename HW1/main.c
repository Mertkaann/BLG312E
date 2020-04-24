#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int level = 1;
    int child_list[5] ;
	int child_num = 0;
    int child = fork();
    if(child != 0){
    	child_list[child_num] = child;
    	child_num++;
    }else{
        child_num=0;
    }

    if (child > 0) {
        child = fork();
        if(child != 0){
        child_list[child_num] = child;
        child_num++;
        }else{
        child_num=0;
        }
        if (child > 0) {
        	
            child = fork();
            if(child != 0){
        		child_list[child_num] = child;
        		child_num++;
       	 	}else{
        		child_num=0;
        	}
            int status;
            int corpse = wait(&status);
        }
        int status;
        int corpse = wait(&status);
    }
    if (child == 0) {
    	level++;
        child = fork();
        if(child != 0){
        		child_list[child_num] = child;
        		child_num++;
       	 	}else{
        		child_num=0;
        	}
        if (child == 0) {
        	level++;
            child = fork();
            if(child != 0){
        		child_list[child_num] = child;
        		child_num++;
       	 	}else{
        		child_num=0;
        	}
            if(child == 0)
       			 level++;
            int status;
            int corpse = wait(&status);
        }
        else if (child > 0) {
                child = fork();
                if(child != 0){
        		child_list[child_num] = child;
        		child_num++;
       	 	}else{
        		child_num=0;
        	}
                int status;
       			int corpse = wait(&status);
       			if(child == 0)
       			 level++;
            }
        int status;
        int corpse = wait(&status);
    }

    printf("<PID:%d, Children_PID:{",getpid());
    for(int i=0;i<child_num;i++)
    	printf("%d, ",child_list[i]);
    printf("}, level:%d>\n",level);
    return 0;
}
