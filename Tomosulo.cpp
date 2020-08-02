// #include<bits/stdc++.h>
#include<iostream>
using namespace std;
#define MAXCYCLE 1000
#define aduration 23//add cycle time for execution
#define mduration 26//Multiply cycle time
#define lduration 4//load duration time
struct ReservationStation{int instnno;int busy;int opcode;int vj,vk,Qj,Qk;}RS[8];
struct Instructions{int opcode,source1,source2,destn,IST,EXST,EXCT,WBT;};
//index 0 to 1 for load 
//index 2 to 5 for add or subtract
//index 6 to 7 for Multiply
//index3 to 5  
//Reservation station
//initialise insttno to -1 for everything and busy to 0


//Add 0,Subtract 2 Multiply 4,Load 15,Store 14
//OPCODE
struct RegFile{int Qi,value;}R[16];//Initially set Qi of everythong to 0 and give some value
//R is value register

int issue(int cycle,int i,Instructions A[])
{
	{//if instruction is issued change busy to 0 so that it can't be issued again
		switch(A[i].opcode) 
		{
			case 15://Load 
					//find corresponding buffer empty
					for(int j=0;j<2;j++)
						if(RS[j].busy==0)//Buffer empty
						{
							RS[j].instnno=i;A[i].IST=cycle;RS[j].opcode=A[i].opcode;
							if(R[A[i].source1].Qi!=-1)//cannot load wait for value
 								{
 									RS[j].Qj=R[A[i].source1].Qi;
								}
							else//Value available and can be loaded
							{
								RS[j].vj=R[A[i].source1].value;RS[j].Qj=-1;
							}
							RS[j].busy=1;
							R[A[i].destn].Qi=j;
							return 1;
						}
					break;	
			case 14://store
					for(int j=0;j<2;j++)
						if(RS[j].busy==0)//Buffer empty
						{
							RS[j].instnno=i;A[i].IST=cycle;RS[j].opcode=A[i].opcode;
							if(R[A[i].source1].Qi!=-1)//cannot load wait for value
 								{
 									RS[j].Qj=R[A[i].source1].Qi;
								}
							else//Value available and can be loaded
							{
								RS[j].vj=R[A[i].source1].value;RS[j].Qj=-1;
							}
							RS[j].busy=1;
							if(R[A[i].destn].Qi!=-1)//Check the same for destination
								{
									RS[j].Qk=R[A[i].destn].Qi;
								}
							else
							{
								RS[j].vk=R[A[i].destn].value;
								RS[j].Qk=-1;
							}
							return 1;
						}
					break;
			case 0:case 2://Add,subtract
					for(int j=2;j<6;j++)
						if(RS[j].busy==0)//Buffer empty
						{
							RS[j].instnno=i;A[i].IST=cycle;RS[j].opcode=A[i].opcode;
							if(R[A[i].source1].Qi!=-1)//cannot load wait for value
 								{
 									RS[j].Qj=R[A[i].source1].Qi;
								}
							else
							{
								RS[j].vj=R[A[i].source1].value;RS[j].Qj=-1;
							}
							RS[j].busy=1;
							if(R[A[i].source2].Qi!=-1)
								{
									RS[j].Qk=R[A[i].source2].Qi;
								}
							else
							{
								RS[j].vk=R[A[i].source2].value;RS[j].Qk=-1;								
							}
							R[A[i].destn].Qi=j;
							return 1;
						}

					break;
			case 4://Multiply
					for(int j=6;j<8;j++)
						if(RS[j].busy==0)//Buffer empty
						{
							RS[j].instnno=i;A[i].IST=cycle;RS[j].opcode=A[i].opcode;
							if(R[A[i].source1].Qi!=-1)//cannot load wait for value
 								{
 									RS[j].Qj=R[A[i].source1].Qi;
								}
							else
							{
								RS[j].vj=R[A[i].source1].value;RS[j].Qj=-1;
							}
							RS[j].busy=1;
							if(R[A[i].source2].Qi!=-1)
								{
									RS[j].Qk=R[A[i].source2].Qi;
								}
							else
							{
								RS[j].vk=R[A[i].source2].value;RS[j].Qk=-1;								
							}
							R[A[i].destn].Qi=j;
							return 1;
						}

					break;
			default:"Invalid";//Just in case opcode is wrong
		}
		return 0;
	}
}
void writeback(Instructions A[],int pos,int cycle)
{
	if(RS[pos].opcode==14)//store result
	{
		//Update Value Since calculating clock cycle and register status,the value given is not actually stored
		RS[pos].busy=0;
		A[RS[pos].instnno].WBT=cycle;
		return;
	}
	int result;
	if(RS[pos].opcode==0)//Add result
		result=RS[pos].vj+RS[pos].vk;
	else if(RS[pos].opcode==2)//Subtract
		result=RS[pos].vj-RS[pos].vk;
	else if(RS[pos].opcode==4)//product
		result=RS[pos].vj * RS[pos].vk;
	else if(RS[pos].opcode==15)//Load
		result=RS[pos].vj;
	for(int i=0;i<16;i++)//Update register file
	{
		if(R[i].Qi==pos)//Write result
			{R[i].value=result;R[i].Qi=-1;}
	}
	for(int i=0;i<8;i++)//Update reservation station
	{
		if(RS[i].Qj==pos)
		{
			RS[i].vj=result;RS[i].Qj=-1;
		}
		if(RS[i].Qk==pos)
		{
			RS[i].vk=result;RS[i].Qk=-1;	
		}
	}
	RS[pos].busy=0;
	A[RS[pos].instnno].WBT=cycle;
}



void execute(Instructions A[],int cycle)
{
	int pos=-1,tinstnno=10000;int tinstnno2=10000,pos2=-1;//choose number greater than n
 	for(int i=0;i<2;i++)//Load or store
 	{
 	if(RS[i].Qj==-1 && RS[i].Qk==-1 && RS[i].busy==1 && A[RS[i].instnno].IST!=cycle && A[RS[i].instnno].EXST==-1)
 		{
 			if(tinstnno>=RS[i].instnno)	
 				{tinstnno=RS[i].instnno;
 				 pos=i;
 				}
 		}
 	if(cycle>=A[RS[i].instnno].EXCT && RS[i].busy==1 && A[RS[i].instnno].EXCT!=-1)
 		{
 			//Find which to be moved to cdb queue
 			if(RS[i].instnno<tinstnno2)
 			{
 				tinstnno2=RS[i].instnno;
 				pos2=i;
 			}
 			//Move tueue or do so 		
 	}
 	}
 	if(pos!=-1)
 	{
 			if(A[RS[pos].instnno].EXST==-1)//Update start cycle time if not updated before
 				A[RS[pos].instnno].EXST=cycle;
 			if(A[RS[pos].instnno].EXCT==-1)//Update end cycle
 				A[RS[pos].instnno].EXCT=A[RS[pos].instnno].EXST+ lduration-1; 				

 	}
 	for(int i=2;i<8;i++)//Add,Subt,Mul
 	{
 		if(RS[i].Qj==-1 && RS[i].Qk==-1 && RS[i].busy==1 && A[RS[i].instnno].EXST==-1 && A[RS[i].instnno].IST!=cycle)
 		{	
 			if(A[RS[i].instnno].EXST==-1)//Update start cycle time if not updated before
 				A[RS[i].instnno].EXST=cycle;
 			if(RS[i].opcode==0 || RS[i].opcode==2)//Duration of ADD or SUB 2 CC
 				{if(A[RS[i].instnno].EXCT==-1)//
 					A[RS[i].instnno].EXCT=A[RS[i].instnno].EXST+ aduration-1;}
 			else if(RS[i].opcode==4)//Duration of MUL 2 CC
 				{if(A[RS[i].instnno].EXCT==-1)
 					A[RS[i].instnno].EXCT=A[RS[i].instnno].EXST+ mduration-1;}
 		}
 		if(cycle>=A[RS[i].instnno].EXCT && RS[i].busy==1 && A[RS[i].instnno].EXCT!=-1)
 		{
 			//Find which to be moved to cdb queue
 			if(RS[i].instnno<tinstnno2)
 			{
 				tinstnno2=RS[i].instnno;
 				pos2=i;
 			}
 		}
 	}
 	if(pos2!=-1 && A[RS[pos2].instnno].EXCT!=cycle)
 		writeback(A,pos2,cycle);//Update the result in the next cycle
}


int main()
{
	for(int i=0;i<8;i++)
	{
	RS[i].busy=0;RS[i].instnno=-1;RS[i].vj=RS[i].vk=RS[i].Qj=RS[i].Qk=RS[i].opcode=-1;
	}
	for(int i=0;i<16;i++)//Initialise values for the register
	{
	R[i].value=i;R[i].Qi=-1;
	}
	int n;
	cout<<"Enter number of instructions";
	cin>>n;string s,s1,s2,s3,s4;
	Instructions A[n];//Has the required parameters with the details of cc
	for(int i=0;i<n;i++)
	{	cout<<"Opcode,Destn,source1,source2 as 16 bit binary number";
		//cin>>s;//Read the binary string
		// s1=s.substr(0,4);
		// s2=s.substr(4,4);
		// s3=s.substr(8,4);
		// s4=s.substr(12,4);
		// A[i].opcode=stoi(s1,0,2);//Convert the binary string to integers with opcode,destn,source1,source2.
		// A[i].destn=stoi(s2,0,2);
		// A[i].source1=stoi(s3,0,2);
		// A[i].source2=stoi(s4,0,2);
		cin>>A[i].opcode>>A[i].destn>>A[i].source1>>A[i].source2;
		A[i].IST=A[i].EXST=A[i].EXCT=A[i].WBT=-1;
	}
	for(int c=1,x=0;c<MAXCYCLE;c++)
	{
		cout<<"\n\tCYCLE"<<c;
		if(x<n)
			if(issue(c,x,A)==1)
				{cout<<"Issue is 1";x+=1;}
		execute(A,c);
		//Execute
		//Display of reservation station
		cout<<"\n\tRESERVATION STATION";
		cout<<"\nInstn Busy opcode vj vk Qj Qk \n";
		for(int i=0;i<8;i++)
			cout<<"\n"<<RS[i].instnno<<" "<<RS[i].busy<<" "<<RS[i].opcode<<" "<<RS[i].vj<<" "<<RS[i].vk<<" "<<RS[i].Qj<<" "<<RS[i].Qk;
		cout<<"\n\tREGISTER FILE\t";
		//Display of register files
		for(int i=0;i<16;i++)
			cout<<R[i].Qi<<":"<<R[i].value<<"  ";
		int flag=0;
		for(int i=0;i<n;i++)
		{
			if(A[i].WBT==-1)
				flag=1;
		}
		if(flag==0)//Breaking if WB is updated for all instructions
			{break;}
	cout<<"\n TIME CHART\n";
	cout<<"\ninst IST EXST EXCT  WBT\n";
	for(int i=0;i<n;i++)
	{
		cout<<i<<"    "<<A[i].IST<<"    "<<A[i].EXST<<"    "<<A[i].EXCT<<"     "<<A[i].WBT<<"\n";
	}
	}
	//TIME CALCULATION
	cout<<"\n\n TIME CHART\n";
	cout<<"\ninst IST EXST EXCT  WBT\n";
	for(int i=0;i<n;i++)
	{
		cout<<i<<"    "<<A[i].IST<<"    "<<A[i].EXST<<"    "<<A[i].EXCT<<"     "<<A[i].WBT<<"\n";
	}
}