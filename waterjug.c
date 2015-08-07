#include<stdio.h>
#include<stdlib.h>
int total1,total2,goal,rec_count,*record;					//Global variables

typedef struct configuration							//This structure holds the state information, i.e. information
{										//about the amount of water held by both jugs, a pointer to the
  int jug1,jug2;								//state in the queue that is next (just behind it) and a pointer
  struct configuration* infrontOf,*derivedFrom;					//to the state from which it is derived (its parent).
}configuration;

typedef struct state_queue							//This structure is a queue that maintains the states that are 
{										//being processed in a Breadth-First-Search manner.
  configuration* head,*tail;
}state_queue;

int find_gcd(int a,int b)							//Euclid's Algorithm to find the gcd of two numbers
{
  int A,B,R;
  A = a;
  B = b;
  while(B > 0)
  {
    R = A % B;
    A = B;
    B = R;
  }
  return A;
}   

void recordState(int* jug_info_record)						//Record the current jug_info.
{
  record[2*rec_count - 2] = jug_info_record[0];
  record[2*rec_count - 1] = jug_info_record[1];
}										//Extra Heuristic information i.e. if the state produced has been
 										//produced before, do not continue with that state. Use the next 											//state.
int presentInRecord(int* check_jug_info_present)				
{										//Search for the given state in the record of previous states.
  int i;
  for(i = 0;i <= (2*(rec_count - 2));i = i+2)
  {
    if(record[i] == check_jug_info_present[0])
    {
      if(record[i + 1] == check_jug_info_present[1])
        return 1;
    }
  }
  return 0;
}

void print_record()								//Print the entire record of all states encountered.
{
  printf("\n\nRECORD (MAINTAINED TO ENSURE THAT STATES DO NOT REPEAT IN THE QUEUE)::\n\n");
  int i;
  for(i = 0;i <= (2*(rec_count - 2));i = i + 2)
    printf("JUG1:\t%d\tJUG2\t%d\n",record[i],record[i + 1]);
}  

void print(int* jug_info)							//Print the state (current jug_info).
{
  printf("||\tJUG 1:\t%d\t||\t\t||\tJUG 2:\t%d\t||\n\n",jug_info[0],jug_info[1]);
}

int check_goal(int* jug_info,int i,configuration* parentOfGoalState)		//Check if the current state is the goal state.
{
  if(jug_info[0] == goal || jug_info[1] == goal)
  {
    print(jug_info);
    print_record();
    free(record);
    printf("\n\nSo the steps of the shortest solution are:\n\n");
    configuration* p = parentOfGoalState;
    printf("\tJUG 1\t\t\t\tJUG 2\t\t\n\t%d Litres\t\t\t%d Litres\n\n",total1,total2);
    printf("||\t%d\t||\t\t||\t%d\t||\n\n",jug_info[0],jug_info[1]);
    int j = 1;
    while(p->derivedFrom)
    {
      printf("||\t%d\t||\t\t||\t%d\t||\n\n",p->jug1,p->jug2);
      p = p->derivedFrom;
      j++;
    }
    printf("||\t%d\t||\t\t||\t%d\t||\n\n",p->jug1,p->jug2);
    free(p);
    printf("No. of steps required=\t%d\nNo. of recursive calls=\t%d\n",++j,i);
    return 1;
  }
  return 0;
}

void init_list(state_queue* nodeList)						//Initialize the queue.
{
  nodeList->head = nodeList->tail = NULL;
}

int emptyList(state_queue* nodeList)						//Check if the queue is empty.
{
  if(nodeList->head == NULL && nodeList->tail == NULL)
    return 1;
  return 0;
}

void addStateToList(int* jug_info,state_queue* nodeList,configuration* myParent)	
{ 										//Enqueue the current state to the queue (add it).
  configuration* state = (configuration*)malloc(sizeof(configuration));
  state->jug1 = jug_info[0];
  state->jug2 = jug_info[1];
  state->infrontOf = NULL;							
  state->derivedFrom = myParent;						//Set the 'derivedFrom' pointer to the parent of the state.
  record = (int*)realloc(record,sizeof(int)*rec_count*2);			//Increase memory allocated to record to accomodate a new state.
  recordState(jug_info);							//Record the state.
  if(emptyList(nodeList))
    nodeList->tail = nodeList->head = state;
  else 
  {  
    nodeList->tail->infrontOf = state;
    nodeList->tail = state;
  }
  rec_count++;
} 

configuration* removeStateFromList(state_queue* nodeList)			//Dequeue operation (removes the state at the head of the queue).
{
  configuration* currentState = nodeList->head;
  if(nodeList->tail == nodeList->head)						//If the queue contains only one state.
    init_list(nodeList);
  else
    nodeList->head = currentState->infrontOf;
  return currentState;
}

void production_rules(state_queue* nodeList)					//These are the set of production rules.
{
  int stateRepeatFlag = 1;
  static int i = 1;								//Used to count the number of recursive calls.
  if(emptyList(nodeList))							//If the queue is empty then no solution can be found.
  {
    printf("No solution found!!\n\n");
    exit(1);
  }
  configuration* parentState = removeStateFromList(nodeList);
  int* jug_info = (int*)malloc(sizeof(int)*2);
  jug_info[0] = parentState->jug1;
  jug_info[1] = parentState->jug2;
  printf("\nState: JUG1: %d\tJUG2: %d\tcan give:\n\n",jug_info[0],jug_info[1]);
  if(jug_info[0] < total1)							//1. Fill jug1 completely.
  {
    jug_info[0] = total1;
    if(!presentInRecord(jug_info))						//We add the state to the queue only if it has not been 
    {										//encountered before(i.e. not present in the record already).
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;						//Set the jug_info back to its initial value for that state.
    jug_info[1] = parentState->jug2;
  }
  if(jug_info[1] < total2)							//2. Fill jug2 completely.
  {
    jug_info[1] = total2;
    if(!presentInRecord(jug_info))
    {
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;
    jug_info[1] = parentState->jug2;
  }
  if(jug_info[0] > 0) 								//3. Empty jug1 completely.
  {
    jug_info[0] = 0;
    if(!presentInRecord(jug_info))
    {
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;
    jug_info[1] = parentState->jug2;
  }
  if(jug_info[1] > 0)								//4. Empty jug2 completely.
  {
    jug_info[1] = 0;
    if(!presentInRecord(jug_info))
    {
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;
    jug_info[1] = parentState->jug2;
  }
  if((jug_info[0] + jug_info[1] > total1) && jug_info[1] > 0)			//5. Pour water from jug2 to jug1 until jug1 is full.
  {										
    jug_info[1] = jug_info[1] - (total1 - jug_info[0]);
    jug_info[0] = total1;
    if(!presentInRecord(jug_info))						
    {										  
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;
    jug_info[1] = parentState->jug2;
  }
  if((jug_info[0] + jug_info[1] > total2) && jug_info[0] > 0)			//6. Pour water from jug1 to jug2 until jug2 is full.
  {						
    jug_info[0] = jug_info[0] - (total2 - jug_info[1]);
    jug_info[1] = total2;
    if(!presentInRecord(jug_info))
    {
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;
    jug_info[1] = parentState->jug2;
  }
  if((jug_info[0] + jug_info[1] <= total1) && jug_info[1] > 0)			//7. Pour all the water from jug2 into jug1.
  {						
    jug_info[0] = jug_info[0] + jug_info[1];
    jug_info[1] = 0;
    if(!presentInRecord(jug_info))      
    {
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;
    jug_info[1] = parentState->jug2;
  }
  if((jug_info[0] + jug_info[1] <= total2) && jug_info[0] > 0)			//8. Pour all the water from jug1 into jug2.
  {						
    jug_info[1] = jug_info[0] + jug_info[1];
    jug_info[0] = 0;
    if(!presentInRecord(jug_info))
    {
      stateRepeatFlag = 0;
      if(check_goal(jug_info,i,parentState))
        return;
      addStateToList(jug_info,nodeList,parentState);
      print(jug_info);
    }
    jug_info[0] = parentState->jug1;
    jug_info[1] = parentState->jug2;
  }
  if(stateRepeatFlag == 1)							//The produced states have all been produced before.
    printf("NO NEW STATES\n");
  free(jug_info);
  i++;
  production_rules(nodeList);							//Recursively call production_rules().
}

int main()
{
  state_queue nodeList;
  int gcd, jug_info[2] = {0};							//Initially we have JUG1 = 0 and JUG2 = 0.
  printf("\t\t\t\tThis is a program for the Waterjug Problem\n\n");
  printf("The Production Rules have been supplied and the Control Strategy is Breadth-First-Search with Additional Heuristic information \n\n");
  printf("Enter the full capacities of the two waterjugs. They must not be equal!:\n");
  scanf("%d%d",&total1,&total2);
  if(total1 == total2)
    exit(1);
  printf("\nEnter the amount of water you wish to measure using the %d litre jug and the %d litre jug:\n",total1,total2);
  scanf("%d",&goal);  
  gcd = find_gcd(total1,total2);						//The goal (amount to be measured) must be a multiple of the 
  if(goal % gcd != 0 || (goal > total1 && goal > total2))			//GCD of the capacities of the two jugs.
  {
    printf("The solution does not exist!\nTerminating...\n\n");
    exit(1);
  }
  rec_count = 1;								//rec_count counts the number of states recorded.
  init_list(&nodeList);
  addStateToList(jug_info,&nodeList,NULL);					//The starting state has no parent so it is derived from NULL.
  production_rules(&nodeList);
}
