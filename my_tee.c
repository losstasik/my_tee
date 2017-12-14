#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<unistd.h>
#define	MAXBUFF		5120

int child_act (char *file, int readfd);

int main(int argc, char* argv[]) {
    //char file[30];
    //int fd;
   
    pid_t childpid;
    int fd[10][2], status;
    char buff[MAXBUFF];
    int num_of_files = argc - 1;

    //fork new child proc for each file
    for (int i=1; i<argc; i++) {

        //create new pipe
        if (pipe(fd[i-1]) == -1) {
            perror("Failed to create the pipe"); 
            exit(EXIT_FAILURE);
        }

        //fork new child
        if ((childpid = fork()) == -1) {
            perror("Failed to fork"); 
            exit(EXIT_FAILURE);
        }
        else if (childpid == 0) { /* this is child*/
            
            close(fd[i-1][1]); /*  child closes write pipe desciptor */

            child_act(argv[i], fd[i-1][0]);

            close(fd[i-1][0]); /*  child closes read pipe desciptor */

            _exit(EXIT_SUCCESS); 
        }
        else { /* this is parent*/
            close(fd[i-1][0]); /*  parent closes read pipe desciptor */
        }
    }

    //read input from stdin
   // while (fgets(buff, MAXBUFF, stdin) != NULL) {
     //   printf("%s - %d", buff, (int) sizeof buff);
        //fput write(fd[1], buff, )

       // write(fd[1], buff, )
	//} 
    
    
    
    int n;
    /* read data from stdin */
    while ((n = read(STDIN_FILENO, buff, MAXBUFF)) > 0) {
        /* write to stdout */
		if (write(STDOUT_FILENO, buff, n) != n) {
			perror("parent: data write to STDOUT error");
			exit(EXIT_FAILURE);
		}
        
        /* write to pipe(s) */
        for (int i=0; i<num_of_files; i++) {
            if (write(fd[i][1], buff, n) != n) {
                perror("parent: data write to pipe eror");
                exit(EXIT_FAILURE);
            }
        }
    }
    
     /* close write pipe(s) */
     for (int i=0; i<num_of_files; i++) {
        close(fd[i][1]); 
     }
     
     
     /* wait for childs  */
     for (int i = 1; i < argc; i++) {
        wait(&status);
     }

    exit(EXIT_SUCCESS);
}

int  child_act (char *filename, int readfd ) {
    //printf("filename is %s\n", filename);
    FILE * pFile;
    char buff[MAXBUFF];
    int n;

    if ((pFile = fopen(filename, "w")) == NULL) {
        perror("cant open file");
        return -1;
    }
    else {
        while ((n = read(readfd, buff, MAXBUFF)) > 0) {
            fwrite(buff, sizeof(char), n, pFile);
		}
        fclose(pFile);
    }

    return 0;
}

