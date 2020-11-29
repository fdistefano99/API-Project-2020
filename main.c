#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define LINESIZE 1026			//max length of a text line including the terminator character
#define INITDIM 1024			//size of the first vectors of text lines or commands allocated
#define HASHDIM 20			//max size of the super-vectors

//structure of a block of the command history
typedef struct{
	int oldmax, newmax, ind1, ind2, oldlen, newlen, prevdel, *succdel;
	char type, **oldtext, **newtext;
}command;

void resize(int type);
void addCommand(int ind1, int ind2, int oldmax, int newmax, char type);
void restoreText(char ** state, int length,int ind1, int ind2);
void getPos(int ind, int pos[]);
void change(int ind1, int ind2);
void print(int ind1, int ind2);
void delete(int ind1, int ind2);
void moveInHistory(int mov, int todo);

char **hashText[HASHDIM];			//declaration of the super-vector of text lines
command **hashHistory[HASHDIM];			//declaration of the super-vector of command history
command *actualCommand=NULL;
int actualmax=-1, actualexp[2], actualdim[2], commcounter=-1, lastdel=-1, *futdel=NULL, virtualmove=-1, oldcommcounter=-1, noaction=1;
const double sub=log2(INITDIM);
char buffer[LINESIZE];


//the main receives a command from the prompt, splits the command to capture the necessary information and calls the right method
int main(int argc, char *argv[]){
	futdel=(int*)malloc(sizeof(int));
	*futdel=-1;
	actualexp[0]=0, actualexp[1]=0, actualdim[0]=INITDIM, actualdim[1]=INITDIM;
	hashText[0]=(char**)malloc(INITDIM*sizeof(char*));
	hashHistory[0]=(command**)malloc(INITDIM*sizeof(command*));
	char lastchar;
	int len=0, ind1=0, ind2=0;
	while ((strncmp((fgets(buffer, LINESIZE, stdin)), "q", 1))){
		len=strlen(buffer);
		lastchar=buffer[len-2];
		buffer[len-2]='\0';
		switch(lastchar){
			case 'c':
				ind1=atoi(strtok(buffer, ","));
				ind2=atoi(strtok(NULL, ","));
				change(ind1-1, ind2-1);
				break;
			case 'd':
				ind1=atoi(strtok(buffer, ","));
				ind2=atoi(strtok(NULL, ","));
				delete(ind1-1, ind2-1);
				break;
			case 'p':
				ind1=atoi(strtok(buffer, ","));
				ind2=atoi(strtok(NULL, ","));
				print(ind1-1, ind2-1);
				break;
			case 'u':
				ind1=atoi(buffer);
				moveInHistory(-ind1, 0);
				break;
			case 'r':
				ind1=atoi(buffer);
				moveInHistory(ind1, 0);
				break;
		}
	}
	return 0;
}

/*this method allocates another vector of text lines or commands in the next block of the related super-vector, the size of the new vector allocated is twice that 
of the previous vector*/
void resize(int type){
	actualexp[type]++;
	if(type==0){
		hashText[actualexp[type]]=(char**)malloc(actualdim[type]*sizeof(char*));
	}
	else{
		hashHistory[actualexp[type]]=(command**)malloc(actualdim[type]*sizeof(command*));
	}
	actualdim[type]=2*actualdim[type];
}

//given the number of a text line, this method returns the position in the super-vector(pos[0]) and in the related vector of text lines(pos[1])
void getPos(int ind, int pos[]){
	if(ind<INITDIM){
		pos[0]=0;
		pos[1]=ind;
	}
	else{
		pos[0]=ceil(log2(ind+1))-sub;
		pos[1]=ind+1-(int)pow(2, pos[0]+sub-1);
	}
}

//this method is used to add a command in the command history, if the current vector of text lines is full it calls the resize method
void addCommand(int ind1, int ind2, int oldmax, int newmax,char type){
	if(commcounter>actualdim[1]-1){
		resize(1);
		addCommand(ind1, ind2, oldmax, newmax,type);
	}
	else{
		int pos[2];
		command *p=NULL;
		getPos(commcounter, pos);
		hashHistory[pos[0]][pos[1]]=(command*)malloc(sizeof(command));
		p=hashHistory[pos[0]][pos[1]];
		p->ind1=ind1;
		p->ind2=ind2;
		p->oldmax=oldmax;
		p->newmax=newmax;
		p->type=type;
		p->prevdel=lastdel;
		p->succdel=futdel;
		oldmax=p->oldmax;
		newmax=p->newmax;
		switch(type){
			case 'c':
				if(ind1>oldmax){
					p->oldtext=NULL;
					p->oldlen=0;
				}
				else if(ind1<=oldmax && ind2>oldmax){
					p->oldtext=(char**)malloc((oldmax-ind1+1)*sizeof(char*));
					p->oldlen=oldmax-ind1+1;
				}
				else{
					p->oldtext=(char**)malloc((ind2-ind1+1)*sizeof(char*));
					p->oldlen=ind2-ind1+1;
				}
				p->newtext=(char**)malloc((ind2-ind1+1)*sizeof(char*));
				p->newlen=ind2-ind1+1;
				break;
			case 'd':
				p->oldtext=(char**)malloc((oldmax+1)*sizeof(char*));
				p->oldlen=oldmax+1;
				p->newtext=(char**)malloc((newmax+1)*sizeof(char*));
				p->newlen=newmax+1;
				if(lastdel==-1 || lastdel!=commcounter-1){
					*futdel=commcounter;
					futdel=(int*)malloc(sizeof(int));
				}
				break;
			case 'n':
				p->oldtext=NULL;
				p->oldlen=0;
				p->newtext=NULL;
				p->newlen=0;
				break;
			case 'f':
				p->oldtext=(char**)malloc((oldmax-newmax)*sizeof(char*));
				p->oldlen=oldmax-newmax;
				p->newtext=NULL;
				p->newlen=0;
				break;
		}
		*futdel=-1;
		actualCommand=p;
	}
}

//this method is used to restore a previous state of the entire text
void restoreText(char **state, int length, int ind1, int ind2){
	if(state!=NULL){
		int pos[2], i=0, j=0;
		getPos(ind1, pos);
		length--;
		if(pos[0]==0){
			j=INITDIM-1;
		}
		else{
			j=(int)pow(2, pos[0]+sub-1)-1;
		}
		for(i=0; i<=length; i++){
			if(pos[1]>j){
				getPos(ind1+i, pos);
				j=(int)pow(2, pos[0]+sub-1)-1;
			}
			hashText[pos[0]][pos[1]]=state[i];
			pos[1]++;	
		}
	}
}

/*this method is used to find the right state to restore, to find this state the method looks for the delete command closest to the state to restore and uses the 
state of the entire text saved by the delete command in the command history to restore the target state much faster*/
void moveInHistory(int mov, int todo){
	if(todo==0){
		virtualmove=virtualmove+mov;
		if(virtualmove<-1){
			virtualmove=-1;
		}
		else if(commcounter==oldcommcounter && virtualmove>commcounter){
			virtualmove=commcounter;
		}
		else if(commcounter!=oldcommcounter && virtualmove>oldcommcounter){
			virtualmove=oldcommcounter;
		}
	}
	else{
		int pos[2], moveinit=0, moveend=0, prevdist=INT_MAX, succdist=INT_MAX, commdist=0, begindist=0, mindist=0, i=0, j=0;
		char direction;
		command *p=NULL;
		moveend=virtualmove;
		if(moveend!=-1){
			getPos(moveend, pos);
			p=hashHistory[pos[0]][pos[1]];
			lastdel=p->prevdel;
			futdel=p->succdel;
			if(p->type=='d'){
				actualmax=p->newmax;
				restoreText(p->newtext, p->newlen, 0, actualmax);
			}
			else{
				if(lastdel!=-1){prevdist=moveend-lastdel+1;}
				if(*futdel!=-1){succdist=*futdel-moveend;}
				commdist=abs(moveend-commcounter);
				begindist=moveend+1;
				mindist=prevdist;
				moveinit=lastdel;
				direction='u';
				if(mindist>=succdist){
					mindist=succdist;
					moveinit=*futdel;
					direction='d';
				}
				if(mindist>=commdist){
					mindist=commdist;
					moveinit=commcounter;
					direction='d';
					if(moveend>moveinit){
						direction='u';
						moveinit++;
					}
				}
				if(mindist>=begindist){
					moveinit=0;
					direction='u';
				}
				getPos(moveinit, pos);
				if(direction=='u'){
					if(pos[0]==0){
						j=INITDIM-1;
					}
					else{
						j=(int)pow(2, pos[0]+sub-1)-1;
					}
					for(i=moveinit; i<=moveend; i++){
						if(pos[1]>j){
							getPos(i, pos);
							j=(int)pow(2, pos[0]+sub-1)-1;
						}
						p=hashHistory[pos[0]][pos[1]];
						restoreText(p->newtext, p->newlen, p->ind1, p->ind2);
						pos[1]++;
					}
					actualmax=p->newmax;
				}
				else{
					for(i=moveinit; i>moveend; i--){
						if(pos[1]<=0){
							getPos(i, pos);
						}
						p=hashHistory[pos[0]][pos[1]];
						restoreText(p->oldtext, p->oldlen, p->ind1, p->ind2);
						pos[1]--;
					}
					actualmax=p->oldmax;
				}
			}
			actualCommand=p;
		}
		else{
			lastdel=-1;
			futdel=hashHistory[0][0]->succdel;
			actualmax=-1;
		}
		commcounter=virtualmove;
	}
}

/*this method adds new text lines to the text or changes text lines that already exist saving the old text lines in the command history, if the current vector of 
text lines is full it calls the resize method*/
void change(int ind1, int ind2){
	if(ind2>actualdim[0]-1){
		resize(0);
		change(ind1, ind2);
	}
	else{
		if(virtualmove!=commcounter){moveInHistory(-1, 1);}
		commcounter++;
		oldcommcounter=commcounter;
		virtualmove=commcounter;
		noaction=0;
		int oldmax=actualmax;
		char *str=NULL;
		int pos[2], i=0, j=0, o=0, n=0;
		if(ind2>actualmax){
			actualmax=ind2;
		}
		addCommand(ind1, ind2, oldmax, actualmax, 'c');
		getPos(ind1, pos);
		if(pos[0]==0){
			j=INITDIM-1;
		}
		else{
			j=(int)pow(2, pos[0]+sub-1)-1;
		}
		for(i=ind1; i<=ind2; i++){
			if(pos[1]>j){
				getPos(i, pos);
				j=(int)pow(2, pos[0]+sub-1)-1;
			}
			if(i<=oldmax){
				actualCommand->oldtext[o]=hashText[pos[0]][pos[1]];
				o++;
			}
			str=fgets(buffer, LINESIZE, stdin);
			hashText[pos[0]][pos[1]]=(char*)malloc((strlen(str)+1)*sizeof(char));
			strcpy(hashText[pos[0]][pos[1]], str);
			actualCommand->newtext[n]=hashText[pos[0]][pos[1]];
			n++;
			pos[1]++;
		}
		str=fgets(buffer, LINESIZE, stdin);
	}
}

//this method simply prints the text lines between the two addresses
void print(int ind1, int ind2){
	if(virtualmove!=commcounter){
		if(noaction==0){oldcommcounter=commcounter;}
		moveInHistory(-1, 1);
		noaction=1;
	}
	int pos[2], i=0, j=0;
	getPos(ind1, pos);
	if(pos[0]==0){
		j=INITDIM-1;
	}
	else{
		j=(int)pow(2, pos[0]+sub-1)-1;
	}
	for(i=ind1; i<=ind2; i++){
		if(pos[1]>j){
			getPos(i, pos);
			j=(int)pow(2, pos[0]+sub-1)-1;
		}
		if(i!=-1 && i<=actualmax){
			fputs(hashText[pos[0]][pos[1]], stdout);
		}
		else{
			fputs(".\n", stdout);
		}
		pos[1]++;
	}
}

//this method deletes the text lines between the two addresses and saves the state of the entire text in the command history
void delete(int ind1, int ind2){
	if(virtualmove!=commcounter){moveInHistory(-1, 1);}
	commcounter++;
	oldcommcounter=commcounter;
	virtualmove=commcounter;
	noaction=0;
	if(ind1<=actualmax && !(ind1==-1 && ind2==-1)){
		int pos1[2], pos2[2], i=0, j=0, t=0, width=0, oldmax=0;
		if(ind1==-1){ind1=0;}
		if(ind2>=actualmax){
			getPos(ind1, pos1);
			if(pos1[0]==0){
				j=INITDIM-1;
			}
			else{
				j=(int)pow(2, pos1[0]+sub-1)-1;
			}
			addCommand(ind1, actualmax, actualmax, ind1-1, 'f');
			for(i=ind1; i<=actualmax; i++){
				if(pos1[1]>j){
					getPos(i, pos1);
					j=(int)pow(2, pos1[0]+sub-1)-1;
				}
				actualCommand->oldtext[t]=hashText[pos1[0]][pos1[1]];
				t++;
				pos1[1]++;
			}
			actualmax=ind1-1;
		}
		else{
			oldmax=actualmax;
			width=ind2-ind1+1;
			actualmax=actualmax-width;
			addCommand(0, ind2, oldmax, actualmax, 'd');
			lastdel=commcounter;
			pos1[0]=0, pos1[1]=0;
			j=INITDIM-1;
			getPos(ind1+width, pos2);
			if(pos2[0]==0){
				t=INITDIM-1;
			}
			else{
				t=(int)pow(2, pos2[0]+sub-1)-1;
			}
			for(i=0; i<=oldmax; i++){
				if(pos1[1]>j){
					getPos(i, pos1);
					j=(int)pow(2, pos1[0]+sub-1)-1;
				}
				else if(pos2[1]>t){
					getPos(i+width, pos2);
					t=(int)pow(2, pos2[0]+sub-1)-1;
				}
				actualCommand->oldtext[i]=hashText[pos1[0]][pos1[1]];
				if(i<=actualmax){
					if(i>=ind1){
						hashText[pos1[0]][pos1[1]]=hashText[pos2[0]][pos2[1]];
						pos2[1]++;
					}
					actualCommand->newtext[i]=hashText[pos1[0]][pos1[1]];
				}
				pos1[1]++;
			}
		}
	}
	else{
		addCommand(-1, -1, -1, -1, 'n');
	}
}
