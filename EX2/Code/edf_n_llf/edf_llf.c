/******
 * Author @Satya Mehta, Souvik De, Sean
 * LCM and GCD functions referred from geeksforgeeks.com
 * Some part of code referred from Prof. Sam Siewarts 
 * original scheduling point and completion time feasibility 
 * test code. 
 */ 

#include <stdio.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define U32_T unsigned int

// U=0.7333
U32_T ex0_period[] = {2, 10, 15};
U32_T ex0_wcet[] = {1, 1, 2};

// U=0.9857
U32_T ex1_period[] = {2, 5, 7};
U32_T ex1_wcet[] = {1, 1, 2};

// U=0.9967
U32_T ex2_period[] = {2, 5, 7, 13};
U32_T ex2_wcet[] = {1, 1, 1, 2};

// U=0.93
U32_T ex3_period[] = {3, 5, 15};
U32_T ex3_wcet[] = {1, 2, 3};

// U=1.0
U32_T ex4_period[] = {2, 4, 16};
U32_T ex4_wcet[] = {1, 1, 4};

//U = 1.0
U32_T ex5_period[] = {2, 5, 10};
U32_T ex5_wcet[] = {1, 2, 1};


U32_T ex6_period[] = {2, 5, 7, 13};
U32_T ex6_wcet[] = {1, 1, 1, 2};
U32_T ex6_deadline[] = {2, 3, 7, 15};
//U=1.0
U32_T ex7_period[] = {3, 5, 15};
U32_T ex7_wcet[] = {1, 2, 4};

U32_T ex8_period[] = {2, 5, 7, 13};
U32_T ex8_wcet[] = {1, 1, 1, 2};

U32_T ex9_period[] = {6, 8, 12, 24};
U32_T ex9_wcet[] = {1, 2, 4, 6};


int gcd(U32_T, U32_T);
int lcm(U32_T arr[], U32_T);



/*Function to calculate EDF*/
int edf(U32_T numServices, U32_T period[], U32_T wcet[], int lcm_period, U32_T d_line[])
{
	/*Initializing temperorary variables*/
	U32_T new_deadline[numServices];
	U32_T comp_left[numServices];
	U32_T comp_completed[numServices];
	int comp_flag=0;
	U32_T deadline;
	int early, dead_index;
	int temp=0;
	
	/*Initializing the variables*/
	for(int i=0;i<numServices; i++)
	{
		new_deadline[i] = period[i];
		comp_left[i] = wcet[i];
		comp_completed[i]=0;
		
	}
	
	/*Calculating for the LCM Period*/
	for(int i = 0; i < lcm_period; i++)
	{
		comp_flag = 0;
		early = period[numServices-1];
		
		/*calculating for the number of services*/
		for(int l = 0; l < numServices; l++)
		{
			new_deadline[l]--;	
			if(comp_left[l] > 0 && new_deadline[l]==0)
			{
				printf("Returned Statement\n");
				return 1;
			}
					
			if(comp_left[l] == 0)
			{
				new_deadline[l]=100;
			}
			if(i%period[l] == 0) //Reset the values after every deadline
			{	
				new_deadline[l] = period[l];
				comp_left[l] = wcet[l];
				comp_completed[l] = 0;
			}
								
			if(early > new_deadline[l])
			{
				early = new_deadline[l]; //Get the earliest deadline
				temp = l;
				comp_flag = 1;
			}
		}
						//comp_completed[l]++;	
		if(comp_flag == 1)
		{
			comp_left[temp]--;
			//printf("Task %d scheduled at %d\n", temp+1, i+1); //comment out to print schedule
		}
		else 
		{
			//printf("No Task executed\n"); //comment out to print shcedule
		}
	
	}
	return 0;
}

int llf(U32_T numServices, U32_T period[], U32_T wcet[], int lcm_period, U32_T d_line[])
{
	/*Initializing temperorary variables*/
	U32_T new_deadline[numServices];
	U32_T comp_left[numServices];
	U32_T comp_completed[numServices];
	U32_T laxity[numServices];
	int comp_flag=0;
	U32_T deadline;
	int early, dead_index;
	int temp=0;
	
	for(int i=0;i<numServices; i++)
	{
		new_deadline[i] = period[i];
		comp_left[i] = wcet[i];
		comp_completed[i]=0;
		
	}
	for(int i = 0; i < lcm_period; i++)
	{
		comp_flag = 0;
		early = period[numServices-1];
		
		/*Calculating for services*/
		for(int l = 0; l < numServices; l++)
		{
			new_deadline[l]--;	
			if(comp_left[l] > 0 && new_deadline[l]==0)
			{
				//printf("Returned Statement\n");
				return 1;
			}
					
			if(comp_left[l] == 0)
			{
				new_deadline[l]=lcm_period*2; //make it to a very high value so it doesn't affect the next time slot
			}
			if(i%period[l] == 0) //Reset the values after every deadlines
			{	
				new_deadline[l] = period[l];
				comp_left[l] = wcet[l];
				comp_completed[l] = 0;
			}
				
			laxity[l] = new_deadline[l] - comp_left[l];	//calculating laxity	
			if(early > laxity[l])
			{
				early = laxity[l]; 
				temp = l;
				comp_flag = 1;
			}
		}
						//comp_completed[l]++;
		if(comp_flag == 1)
		{
			comp_left[temp]--;
			//printf(\n"Task %d scheduled at %d\n", temp+1, i+1); //comment out to print schedule
		}
		else 
		{
			//printf("No Task executed\n"); //comment out to print shcedule
		}
	
	}
	return 0;
}

int main(void)
{ 
	
	printf("********************EDF Test***************************\n\n\n");
    int i, j;
	U32_T numServices;
   
    printf("Ex-0 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D): ",
		   ((1.0/2.0) + (1.0/10.0) + (2.0/15.0)));
	numServices = sizeof(ex0_period)/sizeof(U32_T);
	i = lcm(ex0_period, numServices);
	printf("LCM of periods of ex0_period:- %d ", i);
	j = edf(numServices, ex0_period, ex0_wcet, i, ex0_period);
	if(j)
	{
		printf("INFEASIBLE ");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	printf("Ex-1 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (2.0/7.0)));
	numServices = sizeof(ex1_period)/sizeof(U32_T);
	i = lcm(ex1_period, numServices);
	printf("LCM of periods of ex1_period:- %d ", i);
	j = edf(numServices, ex1_period, ex1_wcet, i, ex1_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	
	printf("Ex-2 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (1.0/7.0) + (2.0/13.0)));
	numServices = sizeof(ex2_period)/sizeof(U32_T);
	i = lcm(ex2_period, numServices);
	printf("LCM of periods of ex0_period:- %d ", i);
	j = edf(numServices, ex2_period, ex2_wcet, i, ex2_period); 
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	printf("Ex-3 U=%4.2f (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D): ",
		   ((1.0/3.0) + (2.0/5.0) + (3.0/15.0)));
	numServices = sizeof(ex3_period)/sizeof(U32_T);
	i = lcm(ex3_period, numServices);
	printf("LCM of periods of ex3_period:- %d ", i);
	j = edf(numServices, ex3_period, ex3_wcet, i, ex3_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	printf("Ex-4 U=%4.2f (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D): ",
		   ((1.0/2.0) + (1.0/4.0) + (4.0/16.0)));
	numServices = sizeof(ex4_period)/sizeof(U32_T);
	i = lcm(ex4_period, numServices);
	printf("LCM of periods of ex4_period:- %d ", i);
	j = edf(numServices, ex4_period, ex4_wcet, i, ex4_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
    
    printf("Ex-5 U=%4.2f (C1=1, C2=2, C3=1; T1=2, T2=5, T3=10; T=D): ",
		   ((1.0/2.0) + (2.0/5.0) + (1.0/10.0)));
	numServices = sizeof(ex5_period)/sizeof(U32_T);
	i = lcm(ex5_period, numServices);
	printf("LCM of periods of ex5_period:- %d ", i);
	j = edf(numServices, ex5_period, ex5_wcet, i, ex5_period);
	
    if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
        
    printf("Ex-6 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (1.0/7.0) + (2.0/13.0)));
	numServices = sizeof(ex6_period)/sizeof(U32_T);
    i = lcm(ex6_period, numServices);
    printf("LCM of periods of ex6_period:- %d ", i);
	j = edf(numServices, ex6_period, ex6_wcet, i, ex6_deadline);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
        
	printf("Ex-7 U=%4.2f (C1=1, C2=2, C3=4; T1=3, T2=5, T3=15; T=D): ",
		   ((1.0/3.0) + (2.0/5.0) + (4.0/15.0)));
	numServices = sizeof(ex7_period)/sizeof(U32_T);
	i = lcm(ex7_period, numServices);
	printf("LCM of periods of ex7_period:- %d ", i);
	j = edf(numServices, ex7_period, ex7_wcet, i, ex7_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
    
        
    printf("Ex-8 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (1.0/7.0) + (2.0/13.0)));
	numServices = sizeof(ex8_period)/sizeof(U32_T);
	i = lcm(ex8_period, numServices);
	printf("LCM of periods of ex8_period:- %d ", i);
	j = edf(numServices, ex8_period, ex8_wcet, i, ex8_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
   
    printf("Ex-9 U=%4.2f (C1=1, C2=2, C3=4, C4=6; T1=6, T2=8, T3=12, T4=24; T=D): ",
		   ((1.0/6.0) + (2.0/8.0) + (4.0/12.0) + (6.0/24.0)));
	numServices = sizeof(ex9_period)/sizeof(U32_T);
	i = lcm(ex9_period, numServices);
	printf("LCM of periods of ex9_period:- %d ", i);
	j = edf(numServices, ex9_period, ex9_wcet, i, ex9_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	
	printf("\n\n\n*********LLF Test*****************\n\n\n");
	
	printf("Ex-0 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D): ",
		   ((1.0/2.0) + (1.0/10.0) + (2.0/15.0)));
	numServices = sizeof(ex0_period)/sizeof(U32_T);
	i = lcm(ex0_period, numServices);
	printf("LCM of periods of ex0_period:- %d ", i);
	j = llf(numServices, ex0_period, ex0_wcet, i, ex0_period);
	if(j)
	{
		printf("INFEASIBLE ");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	printf("Ex-1 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (2.0/7.0)));
	numServices = sizeof(ex1_period)/sizeof(U32_T);
	i = lcm(ex1_period, numServices);
	printf("LCM of periods of ex1_period:- %d ", i);
	j = llf(numServices, ex1_period, ex1_wcet, i, ex1_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	
	printf("Ex-2 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (1.0/7.0) + (2.0/13.0)));
	numServices = sizeof(ex2_period)/sizeof(U32_T);
	i = lcm(ex2_period, numServices);
	printf("LCM of periods of ex0_period:- %d ", i);
	j = llf(numServices, ex2_period, ex2_wcet, i, ex2_period); 
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	printf("Ex-3 U=%4.2f (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D): ",
		   ((1.0/3.0) + (2.0/5.0) + (3.0/15.0)));
	numServices = sizeof(ex3_period)/sizeof(U32_T);
	i = lcm(ex3_period, numServices);
	printf("LCM of periods of ex3_period:- %d ", i);
	j = llf(numServices, ex3_period, ex3_wcet, i, ex3_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	printf("Ex-4 U=%4.2f (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D): ",
		   ((1.0/2.0) + (1.0/4.0) + (4.0/16.0)));
	numServices = sizeof(ex4_period)/sizeof(U32_T);
	i = lcm(ex4_period, numServices);
	printf("LCM of periods of ex4_period:- %d ", i);
	j = llf(numServices, ex4_period, ex4_wcet, i, ex4_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
    
    printf("Ex-5 U=%4.2f (C1=1, C2=2, C3=1; T1=2, T2=5, T3=10; T=D): ",
		   ((1.0/2.0) + (2.0/5.0) + (1.0/10.0)));
	numServices = sizeof(ex5_period)/sizeof(U32_T);
	i = lcm(ex5_period, numServices);
	printf("LCM of periods of ex5_period:- %d ", i);
	j = llf(numServices, ex5_period, ex5_wcet, i, ex5_period);
	
    if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
        
    printf("Ex-6 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (1.0/7.0) + (2.0/13.0)));
	numServices = sizeof(ex6_period)/sizeof(U32_T);
    i = lcm(ex6_period, numServices);
    printf("LCM of periods of ex6_period:- %d ", i);
	j = llf(numServices, ex6_period, ex6_wcet, i, ex6_deadline);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
        
	printf("Ex-7 U=%4.2f (C1=1, C2=2, C3=4; T1=3, T2=5, T3=15; T=D): ",
		   ((1.0/3.0) + (2.0/5.0) + (4.0/15.0)));
	numServices = sizeof(ex7_period)/sizeof(U32_T);
	i = lcm(ex7_period, numServices);
	printf("LCM of periods of ex7_period:- %d ", i);
	j = llf(numServices, ex7_period, ex7_wcet, i, ex7_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
    
        
    printf("Ex-8 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
		   ((1.0/2.0) + (1.0/5.0) + (1.0/7.0) + (2.0/13.0)));
	numServices = sizeof(ex8_period)/sizeof(U32_T);
	i = lcm(ex8_period, numServices);
	printf("LCM of periods of ex8_period:- %d ", i);
	j = llf(numServices, ex8_period, ex8_wcet, i, ex8_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}
	
	printf("Ex-9 U=%4.2f (C1=1, C2=2, C3=4, C4=6; T1=6, T2=8, T3=12, T4=24; T=D): ",
		   ((1.0/6.0) + (2.0/8.0) + (4.0/12.0) + (6.0/24.0)));
	numServices = sizeof(ex9_period)/sizeof(U32_T);
	i = lcm(ex9_period, numServices);
	printf("LCM of periods of ex9_period:- %d ", i);
	j = llf(numServices, ex9_period, ex9_wcet, i, ex9_period);
	if(j)
	{
		printf("INFEASIBLE\n");
	}
	else
	{
		printf("FEASIBLE\n");
	}	
 }

int gcd(U32_T a, U32_T b) 
{ 
    if (b == 0) 
    return a; 
    return gcd(b, a % b); 
} 

int lcm(U32_T arr[], U32_T n) 
{ 
 
    int ans = arr[0]; 
    for (int i = 1; i < n; i++) 
        ans = (((arr[i] * ans)) / (gcd(arr[i], ans))); 
    return ans; 
} 
