#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct transition{
	int inp;int src;int dest;
}Transition;

int countStates(FILE* ptr){
	if (ptr == NULL) {
		printf("file can't be opened \n");
		return 0;
	}

	int m=0;
	while (!feof(ptr)) {
		char ch = fgetc(ptr);
		if(ch=='\n')	break;
		if(ch==' ')	continue;
		m++;
	}
	
	return m;//returns number of states in a dfa
}

void createTransTables(FILE* ptr,int m,int final[m],Transition table[2][m]){
	if (ptr == NULL) {
		printf("file can't be opened \n");
		return;
	}
	
	int cnt = 0;//temp variable to store count of first row elements( which will tell abt the final states) of a file
	int i=0;//stores input: 0 or 1
	int r=0;//stores row number
	int c=0;//stores column number
	while (!feof(ptr)) {
		char ch = fgetc(ptr);
		if(cnt<=m){//first row elements of file
			if(ch=='0' || ch=='1'){
				final[cnt] = (ch-48);//convert char to int
				cnt++;	
			}
			else if(ch=='\n'){
				cnt++;
			}
		}
		else{//apart from the first row elements of file
			if(ch=='\n'){
				c=0;
				r++;
				if(r==m){//entries for input '0' is complete
					r=0;
					i++;//change input to 1
				}
			}
			else if(ch=='1'){//if file contains 1 then it means that there is a valid transition
				table[i][r].inp = i;
				table[i][r].src = r;
				table[i][r].dest = c;
				c++;
			}
			else if(ch=='0'){
				c++;
			}			
		}
	}
}

void createCrossTable(int m,int n,Transition table1[2][m],Transition table2[2][n],int final1[m],int final2[n],int Table[2][m*n][m*n],int Final[m*n]){	
	for(int i=0;i<2;i++){
		for(int j=0;j<m;j++){
			for(int k=0;k<n;k++){
				if(final1[j]==1 && final2[k]==1){//doubt: || , &&
					Final[j*n+k] = 1;//index = rows passed*(total elements in 1 row i.e. total columns) + total elems passed 
							//in current row
				}
				int inp = table1[i][j].inp;//or,table2[i][j].inp
				int a1=table1[i][j].src;//i=table1[i][j].inp, j=table1[i][j].src
				int a2=table1[i][j].dest;
				int b1=table2[i][k].src;			
				int b2=table2[i][k].dest;
				Table[inp][a1*n+b1][a2*n+b2]=1;//index = rows passed*(total elements in 1 row i.e. total columns) + total elems
							//passed in current row	
			}
		}
	}
}

void writeFile(FILE* ptr,int N,int Final[N],int Table[2][N][N]){
	if (ptr == NULL) {
		printf("file can't be opened \n");
		return;
	}
	
	int i,j,k;char ch;
	//int r=0;int c=0;
	for(i=0;i<N-1;i++){
		ch=Final[i]+48;//int to char
		fputc(ch, ptr);
		fputc(' ', ptr);
	}
	ch=Final[i]+48;//int to char
	fputc(ch, ptr);
	fputc('\n', ptr);
	
	for(i=0;i<2;i++){
		for(j=0;j<N;j++){
			for(k=0;k<N-1;k++){
				ch = Table[i][j][k] + 48;//int to char
				fputc(ch, ptr);
				fputc(' ', ptr);
			}	
			ch = Table[i][j][k] + 48;//int to char
			fputc(ch, ptr);
			fputc('\n', ptr);
		}
	}
	
}

/*********************************************************Display functions:*********************************************************/
void displayFinalStates(int m,int final[m]){
	printf("final states:\n");
	for(int k=0;k<m;k++){
		printf("%d ",final[k]);
	}
}

void displayTransTable(int m,Transition table[2][m]){
	printf("\nDfa transition table:\n");
	for(int i=0;i<2;i++){
		for(int j=0;j<m;j++){
			printf("%d%d%d ",table[i][j].inp,table[i][j].src,table[i][j].dest);	
		}
		printf("\n");
	}
}

void displayTable(int m,int A[2][m][m]){
	printf("\ntransition table:\n");
	for(int i=0;i<2;i++){
		for(int j=0;j<m;j++){
			for(int k=0;k<m;k++){
				printf("%d ",A[i][j][k]);
			}	
			printf("\n");
		}
	}
}

/*********************************************************Main function:*********************************************************/
int main(){
	char dfa1File[20];	
	char dfa2File[20];
	char outputFile[20];
	
	printf("Enter DFA1 file name: ");
	scanf("%s",dfa1File);
	
	printf("Enter DFA2 file name: ");
	scanf("%s",dfa2File);
	
	printf("Enter output file name: ");
	scanf("%s",outputFile);
	
	/*printf("Dfa1 file name: %s\n",dfa1File);
	printf("Dfa2 file name: %s\n",dfa2File);
	printf("Output file name: %s\n",outputFile);*/		
	
	FILE* ptr;	
	//DFA1:
	ptr = fopen(dfa1File, "r");//file containing transitions of dfa1
	int m = countStates(ptr);//number of states in dfa1
	fclose(ptr);
	
	int final1[m];//final states of dfa1
	Transition table1[2][m];//transition table of dfa1
	
	ptr = fopen(dfa1File, "r");
	createTransTables(ptr,m,final1,table1);
	fclose(ptr);
	
	printf("\n/***********DFA1:************/\n");
	displayFinalStates(m,final1);
	displayTransTable(m,table1);
	
	//DFA2:
	ptr = fopen(dfa2File, "r");//file containing transitions of dfa2
	int n = countStates(ptr);//number of states in dfa2
	fclose(ptr);
	
	int final2[n];//final states of dfa2
	Transition table2[2][n];//transition table of dfa2
	
	ptr = fopen(dfa2File, "r");
	createTransTables(ptr,n,final2,table2);
	fclose(ptr);
	
	printf("\n/***********DFA2:************/\n");
	displayFinalStates(n,final2);
	displayTransTable(n,table2);
	
	//Cross-Product:
	int N = m*n;
	int Final[N];//final states of cross-product
	int Table[2][N][N];//transition table of cross-product
	memset(Final, 0, sizeof(Final[0])*N);//initialize all elements of 'Final' to 0
	memset(Table, 0, sizeof(Table[0][0][0])*2*N*N);//initialize all elements of 'Table' to 0
	
	createCrossTable(m,n,table1,table2,final1,final2,Table,Final);
	
	printf("\n/***********Cross Product:************/\n");
	displayFinalStates(N,Final);
	displayTable(N,Table);
	
	//writing into file:
	ptr = fopen(outputFile,"w");//file containing transitions of cross-product
	writeFile(ptr,N,Final,Table);
	fclose(ptr);
	
	return 0;
}

