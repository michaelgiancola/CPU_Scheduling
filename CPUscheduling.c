/*Author: Michael Giancola
 *Date: 27/10/2019
 *Description: This file contains a program that will apply CPU Scheduling Algorithms to a sample file that 
 contains ready queues. This program will print out the ready queue once a scheduling alg has manipulated it.
 **/

#include <stdio.h>
#include <stdlib.h>

static FILE* output_fp;		//the output file pointer is static so that all functions within this file can access it

typedef struct process{		//this struct represents a process
	int pNum;		//is the process number (name)
	int burstNum;		//the CPU burst time assocaited with a process
	int rrOccurence;	//the number of occurances this process has in a Round Robin ready queue
	int turnAroundTime;
} Proc;

//these are function prototypes for FCFS, SJF and RR Scheduling Algorithms
void fcfs(int qIndex, int processCount, Proc qArray[]);
void sjf(int qIndex, int processCount, Proc qArray[]);
void rr(int qIndex, int processCount, Proc qArray[], int tq);

//change input file name
int main(){

	FILE *fp;							//input file pointer
	char* filename = "cpu_scheduling_input_file.txt";		//name of input file
	int queueNum;							//variable to identify which queue is which
	int qCount = 0;
	int pCount = 0;

	fp = fopen(filename, "r");					//opens the input file with read permissions
	if(fp == NULL){							//check to see if input file was unable to open
		printf("File %s could not be opened", filename);
		fprintf(output_fp,"File %s could not be opened", filename);	//print to output file as well pointed at by output_fp
		return 1;
	}

	output_fp = fopen("cpu_scheduling_output_file.txt", "w");	//opens/creates output file with write permissions
	if(output_fp == NULL){						//check to see if output file was unable to open
		printf("Output file could not be opened");
		fprintf(output_fp,"Output file could not be opened");
		return 1;
	}

	//this loop gets the amount of queues in the file
	char c;
	while(fscanf(fp, "%c", &c) != EOF){
		if(c == 't'){
			fgetc(fp);
		}
		if(c == 'q'){
			qCount++;
		}
	}	

	fclose(fp);//close file so that when I reopen, I can read from the beginning again

	fp = fopen(filename, "r");
        if(fp == NULL){
                printf("File %s could not be opened", filename);
		fprintf(output_fp,"File %s could not be opened", filename);
                return 1;
        }

	//this loop gets me the number of processes in each queue by searching for the p character in each queue
	int *processCount = malloc(sizeof(int)*qCount);
	int i = 0;
	fgetc(fp);
	while(fscanf(fp, "%c", &c) != EOF){
		if(c == 'p'){
			pCount++;
			processCount[i] = pCount;
		}
        	if(c == 't'){
                        fgetc(fp);
                }
                if(c == 'q'){
			pCount = 0;
                        i++;
                }
        }

	fclose(fp);//close file so that when I reopen, I can read from the beginning again
	
	Proc ** qArray = malloc(sizeof(Proc *)*qCount);		//creates a dynamic 2-D array of structures (rows are queues and columns are processes)

	int j;
	for(j=0; j<qCount; j++){				//loop through the 2D array and create a jagged array since # of processes can change
		qArray[j] = malloc(sizeof(Proc)*processCount[j]);
	}
	
	int *tqs = malloc(sizeof(int)*qCount);			//dynamic array for the time quantums for a variable amount of queues
	
	
	fp = fopen(filename, "r");
        if(fp == NULL){
                printf("File %s could not be opened", filename);
		fprintf(output_fp,"File %s could not be opened", filename);
                return 1;
        }

	int queueCounter = 0;

       	while(1){						//this loop populates the 2D dynamic array with processes
      		fgetc(fp); 					//reads character q is input file	

		fscanf(fp, "%d", &queueNum); 			//read queue number in input file and stores in variable

		fgetc(fp); 					//read space character
		fgetc(fp); 					//read character t in input file
		fgetc(fp); 					//read character q in input file
		fgetc(fp); 					//read space character

		fscanf(fp, "%d", &tqs[queueCounter]); 		//read time quantum number in input file
		
		int processNum;
		int burst;
		char check;
		int processCounter = 0;
			
		while(1){					//this subloop physically adds the process objects inside the 2D array in the queues
			check = fgetc(fp);			//reads a new line character or space character

			if(check == EOF){ 			//when EOF character is found all ready queues have been created so break out of loop
				break;
			}

			check = fgetc(fp);			//set read in character to check 

		       	if(check =='q'){ 			//when a q is found alone, it must be the start of the next queue 
                                break;
                        }

			if(check == EOF){ 			//when EOF character is found all ready queues have been created so break out of loop
                                break;
                        }

			fscanf(fp, "%d", &processNum);		//read process number and store it in a processNum variable
			fscanf(fp, "%d", &burst);		//read CPU burst time associated with process and store it in a burst variable
			Proc obj;				//create a process (Proc) object
			obj.pNum = processNum;			//set process number to object
			obj.burstNum = burst;			//set burst time to object and then store the object in the 2D array within its queue array
			qArray[queueCounter][processCounter] = obj;
			processCounter++;
		}
			queueCounter++;
			if(check == EOF){
				break;
			}
	}

	for(queueCounter = 0; queueCounter < qCount; queueCounter++){			//loops through queues and calls each scheduling algorithim
		fcfs(queueCounter, processCount[queueCounter], qArray[queueCounter]);	//prints all info associated with the FCFS alogrithm
		sjf(queueCounter, processCount[queueCounter], qArray[queueCounter]);	//prints all info associated with the SJF algorithm
		rr(queueCounter, processCount[queueCounter], qArray[queueCounter], tqs[queueCounter]);	//prints info associated with the RR algorithm
	}
		
	fclose(fp);			//closes the input file
	fclose(output_fp);		//closes the output file

	free(processCount);		//frees dynamically allocated memory for the process count array
	
	for(j=0; j<qCount; j++){	//loops through and frees the dynamic arrays which represented each queue
               free(qArray[j]);
        }

	free(qArray);			//frees the dynamically allocated 2D array

	return 0;
}

void fcfs(int qIndex, int processCount, Proc qArray[]){			//FCFS scheduling algorithm implementation
	Proc *tempArray2 = malloc(sizeof(Proc)*processCount);	
	int i;
	int waitTime = 0;
	double avgTime;
	int waitSum = 0;

	for(i = 0; i < processCount; i++){				//sets the passed through original array to the tempArray2 created in this function
		tempArray2[i] = qArray[i];
	}

	printf("\nReady Queue %d Applying FCFS Scheduling:\n\nOrder of selection by CPU:\n", qIndex+1);
	fprintf(output_fp, "\nReady Queue %d Applying FCFS Scheduling:\n\nOrder of selection by CPU:\n", qIndex+1);	//print to output file as well

	for(i = 0; i < processCount; i++){							//print the order of selection by CPU
		printf("p%d ", tempArray2[i].pNum);
		fprintf(output_fp,"p%d ", tempArray2[i].pNum);
	}
	
	printf("\n");
	fprintf(output_fp,"\n");
	printf("\nIndividual waiting times for each process:");
	fprintf(output_fp,"\nIndividual waiting times for each process:");

	for(i = 0; i < processCount; i++){							//calculate/print the waiting times of each process in queue
		if (i == 0){
                        printf("\np%d = %d", tempArray2[i].pNum, waitTime);
			fprintf(output_fp,"\np%d = %d", tempArray2[i].pNum, waitTime);
                        waitTime = tempArray2[i].burstNum;					//calculate waiting time for first process only
                }

                else{										//calculates waiting time for the rest of processes
			waitSum += waitTime;
                        printf("\np%d = %d", tempArray2[i].pNum, waitTime);
			fprintf(output_fp,"\np%d = %d", tempArray2[i].pNum, waitTime);
                        waitTime = waitTime + tempArray2[i].burstNum;				//incremement waittime by new process burst
                }
	}	
	
	avgTime = (double)waitSum/((double)processCount); 					//calculate avg wait time by adding wait times/# of processs
	printf("\n\nAverage waiting time = %.1f\n", avgTime);
	fprintf(output_fp,"\n\nAverage waiting time = %.1f\n", avgTime);			//print avg wait time with one decimal place

	free(tempArray2);	//free dynamic array called tempArray2
}

void sjf(int qIndex, int processCount, Proc qArray[]){			//SJF Scheduling Alg implementation
	
	Proc *tempArray = malloc(sizeof(Proc)*processCount);
	int i;
	int j = processCount;
	int end = processCount;
	Proc temp;
	int waitTime = 0;
        double avgTime;
        int waitSum = 0;

	for(i = 0; i < processCount; i++){				//move contents from argument array into new created array
                tempArray[i] = qArray[i];
        }
						
	for(j - 1; j >= 0; j--){	//my implementation of bubble sort algorithm to put the processes in SJF ready queue format based on burst time
		for(i = 0; i < end-1; i++){
			if(tempArray[i].burstNum > tempArray[i+1].burstNum){
				temp = tempArray[i+1];
				tempArray[i+1] = tempArray[i];
				tempArray[i] = temp;	
			}
		}
		end--;
	}

        printf("\nReady Queue %d Applying SJF Scheduling:\n\nOrder of selection by CPU:\n", qIndex+1);
	fprintf(output_fp,"\nReady Queue %d Applying SJF Scheduling:\n\nOrder of selection by CPU:\n", qIndex+1);

        for(i = 0; i < processCount; i++){
                printf("p%d ", tempArray[i].pNum);
		fprintf(output_fp,"p%d ", tempArray[i].pNum);
        }

       	printf("\n");
	fprintf(output_fp,"\n");

        printf("\nIndividual waiting times for each process:");
	fprintf(output_fp,("\nIndividual waiting times for each process:"));

        for(i = 0; i < processCount; i++){						//just like above, calculates wait times for processes the same way
                 if (i == 0){
                        printf("\np%d = %d", tempArray[i].pNum, waitTime);
			fprintf(output_fp,"\np%d = %d", tempArray[i].pNum, waitTime);
                        waitTime = tempArray[i].burstNum;
                }

                else{
			waitSum += waitTime;
                        printf("\np%d = %d", tempArray[i].pNum, waitTime);
			fprintf(output_fp,"\np%d = %d", tempArray[i].pNum, waitTime);
                        waitTime = waitTime + tempArray[i].burstNum;
                }
        }

        avgTime = (double)waitSum/((double)processCount);				//avg wait time calculation
        printf("\n\nAverage waiting time = %.1f\n", avgTime);
	fprintf(output_fp,"\n\nAverage waiting time = %.1f\n", avgTime);
}


void rr(int qIndex, int processCount, Proc qArray[], int tq){
	
	Proc *tempArray1 = malloc(sizeof(Proc)*processCount);	//create a dynamic array which will be manipulated so that the original is not tampered with
	int i;							
	int burst;
	int newCount = 0;
	int occurCount;

	for(i = 0; i < processCount; i++){	//array is passed by reference so here I am storing the contents in the original array into my tempArray
                tempArray1[i] = qArray[i];
        }

	for(i = 0; i < processCount; i++){	//this loop is for counting the amount of extra process entries that must be added to the rr queue
		burst = tempArray1[i].burstNum;
		occurCount = 0;
		while(1){
			if(burst > tq){
				newCount++;
				occurCount++;			
				burst -= tq;
			}	
			else{
				break;
			}
		}
		tempArray1[i].rrOccurence = (occurCount+1);
	}
	
	newCount += processCount;	//add the amount of original processes in the ready queue to the new amount which incorporates a time quantum

	Proc *newArray = realloc(tempArray1, sizeof(Proc)*(newCount));	//realloc the array so that there is more room in the rr ready queue
	int count = 0;

	for(i = 0; i < newCount; i++){	//populate the new realloc array with the rr algorithim implemented (each process gets at most q time units at once)
                burst = newArray[i].burstNum;
		if(burst > tq){
    			newArray[i].pNum = tempArray1[i].pNum;
			newArray[i].burstNum = tq;			
               		burst -= tq;					//subtracts tq from burst time to see how many times the process occurs in queue
			newArray[processCount + count].pNum = newArray[i].pNum;
			newArray[processCount + count].burstNum = burst;
			count++;
               	}
        }

	printf("\nReady Queue %d Applying RR Scheduling:\n\nOrder of selection by CPU:\n", qIndex+1);
	fprintf(output_fp,"\nReady Queue %d Applying RR Scheduling:\n\nOrder of selection by CPU:\n", qIndex+1);

       
	for(i = 0; i < newCount; i++){	//prints the order of selection 
                printf("p%d ", newArray[i].pNum);
		fprintf(output_fp,"p%d ", newArray[i].pNum);
       	}
	
       	printf("\n");
	fprintf(output_fp,"\n");

       	printf("\nTurnaround times for each process:\n");
	fprintf(output_fp,"\nTurnaround times for each process:\n");

	int turnAroundTime = 0;
	int counter = 0;		//counts how many times the same process is found in the queue
	int j;
					
	for (i = 0; i < processCount; i++){						//loops through each process
		for(j = 0; j < newCount; j++){						//loops through each occurence in the rr queue
			if(newArray[j].pNum == newArray[i].pNum){			//if we are at the process of interest execute the following
				counter++;						//incremement found counter meaning a process was found in the queue
				turnAroundTime += newArray[j].burstNum;
				while(counter <  newArray[i].rrOccurence){		//loop until all occurences of process are found
					if(newArray[j+1].pNum == newArray[i].pNum){
						counter++;
					}
					turnAroundTime += newArray[j+1].burstNum;
					j++;
				}	
				break;
			}
		}
		counter = 0;
		newArray[i].turnAroundTime = turnAroundTime;
		turnAroundTime = 0;
	}

	j = processCount;
	int end = processCount;
	Proc temp;

	//use bubble sort to but the processes in order of the least number of occurences to the most so that the output is correctly formatted
	for(j - 1; j >= 0; j--){
                for(i = 0; i < end-1; i++){
                        if(newArray[i].rrOccurence > newArray[i+1].rrOccurence){
                                temp = newArray[i+1];
                                newArray[i+1] = newArray[i];
                                newArray[i] = temp;
                        }
                }
                end--;
        }

	for(i = 0; i < processCount; i++){	//prints out all the turn around times in order from the queue
		printf("p%d = %d\n", newArray[i].pNum, newArray[i].turnAroundTime);
		fprintf(output_fp,"p%d = %d\n", newArray[i].pNum, newArray[i].turnAroundTime);
	}

	free(newArray);	//free dynamic array created in this function
}

