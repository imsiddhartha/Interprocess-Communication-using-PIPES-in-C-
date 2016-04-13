#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include<string.h>
#include<iostream>

using namespace std;

struct command 
{
	const char *cmd[1000];
	 int flags;
};
int main(int argc,char* argv[])
{
	int countPipe=0,len;
	char *str=argv[1];
	int i=0,k;
	char *str1;
	
	while(str[i]!='\0')							//counting number of pipies in the cmd line arg
	{
		if(str[i]=='|')
			countPipe++;
		i++;
	}
	
	struct command c[countPipe+1];char *p[countPipe+1];		//num of cmds is countPipe+1
	
	for(int i=0;i<=countPipe;i++)
	{	
		p[i]=strtok(str, "|");

		while( p[i] != NULL ) 
		   {
		   	i++;
			p[i] = strtok(NULL, "|");
		   }
	}

	for(int kk=0;kk<=countPipe;kk++)
	{
		len=strlen(p[kk]);	
		p[len]='\0';
		str1=strtok(p[kk], " ");					//cmd[0] pe sirf command ja rhi h!
		c[kk].cmd[0]=str1;

		k=1;
	
		while( k<len-2) 
		{	
			p[kk] = strtok(NULL, " ");				//To seperate flags,cmds and path etc..
			
			if(p[kk]=='\0')						//No futher flags etc.. then break
				break;
			if(p[kk][0]=='-')						//for retreiving flags 
			{
				c[kk].cmd[k]=p[kk];
			}
			else if(p[kk][0]=='/')					// for retreiving directories
			{
			c[kk].cmd[k]=p[kk];
			}
			else
			{
				c[kk].cmd[k]=p[kk];				//different cmds that does not have flags and paths.EG: grep etc..
			}
		k++;
		}
		c[kk].flags=k-1;							//number of flags,paths, in the command
		c[kk].cmd[k]='\0';						//Null at the end of array
	}
	
	int pfd[2];
	
	if(pipe(pfd) < 0)
	{
		printf("While openning pipe\n");
		_exit(-1);
	}		
	int childpid;int status;
	childpid =fork();
	if(childpid==0)
	{	
		close(0);								//close stdin
		dup2(pfd[0],0);							//make copy of stdin
		close(pfd[1]);							//close write end of pipe
		
		int res=execvp (c[1].cmd[0], (char * const *)c[1].cmd);		
		
		if(res<0)
		{
			perror("execvp");
			_exit(-1);
		}
		_exit(0);
	}
	else
	{
		/*cout<<"Parent :"<<endl;*/	
		close(1);								//close stdout
		dup2(pfd[1], 1);							//make copy of stdout
		close(pfd[0]);							//close read end of pipe
		int res=execvp (c[0].cmd[0], (char * const *)c[0].cmd);		
		if(res<0)
		{
			perror("execvp");
			_exit(-1);
		}
		
		close(pfd[1]);
	    	close(1);
		//wait(&status);							//wait for child child to terminate first
		waitpid(childpid,&status,0);
	}	
	
return 0;
}
