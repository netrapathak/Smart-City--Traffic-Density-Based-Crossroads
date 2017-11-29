  /*Program to 'Control the traffic lights based on the traffic density for that
    particular lane'; based on the measurements taken by the 3 IR sensors on that
    lane
  */

  /*Variable declaration and initialization*/

  unsigned int counter=0; //Variable to count the number of iterations for each compare value(0CR0)
  unsigned int lane=1;  //To initialize to start check state from second lane
  unsigned int delay_count=234;
  unsigned int counter_count_check=0; //To count for the number of times sensor inputs are checked
  unsigned int resume_stop_check=0;
  unsigned int ir_check_mode=1;
  unsigned int sensor[4][3];
  
  /*Function that takes input from the sensors*/
  void check_lane();

  /*Fucntion that checks for the state of the lane*/
  void check_lane_status();

  /*Function that gives appropriate delay; according to the state of the lane*/
  void operate(int stateCount);

  /*To store PORTA and PORTB values*/
  char storeA, storeB,store_resumeA;

  void TIMER0_COMP()org 0x014
  {
   counter++;
  }

  void check_lane()
  {
       counter_count_check++;

       if(lane==0)
      {
       sensor[lane][0]+=PINB.B5;
       sensor[lane][1]+=PINB.B6;
       sensor[lane][2]+=PINB.B7;
      }
      else if(lane==1)
      {
       sensor[lane][0]+=PINC.B6;
       sensor[lane][1]+=PINC.B7;
       sensor[lane][2]+=PINB.B4;
      }
      else if(lane==2)
      {
       sensor[lane][0]+=PINC.B3;
       sensor[lane][1]+=PINC.B4;
       sensor[lane][2]+=PINC.B5;
      }
      else
      {
       sensor[lane][0]+=PINC.B0;
       sensor[lane][1]+=PINC.B1;
       sensor[lane][2]+=PINC.B2;
      }
 }

  void check_lane_status()
  {
   int j,state_count=0;
   for(j=0;j<3;j++)
   {
    sensor[lane][j]=sensor[lane][j]/(counter_count_check);
    if(sensor[lane][j]==1)
    state_count++;
   }
   counter_count_check=0;

    for(j=0;j<3;j++)
    {
     sensor[lane][j]=0;
    }

   if(lane==3)
   lane=0;
   else
   lane++;

   operate(state_count);
  }

  void operate(int stateCount)
  {
   int state_lane_operate = stateCount;
   switch(state_lane_operate)
   {
   case 1:
   delay_count=334;
   break;

   case 2:
   delay_count=668;
   break;

   case 3:
   delay_count=1002;
   break;

   default:
   delay_count=234;
   break;
   }
  }

  void main()
  {

   int i,j;

   DDRA=0XFF;
   PORTA=0XE1;

   DDRB=0X0F;
   PORTB=0X00;

   DDRC=0X00;
   PORTC=0X00;

   DDRD=0XF0;
   PORTD=0X00;

   storeA=0xE1;
   storeB=0x00;

   for(i=0;i<3;i++)
   {
    for(j=0;j<3;j++)
    {
     sensor[i][j]=0;
    }
   }

   TCCR0=0X0D;
   OCR0=0XE9;

   TIMSK.B1=1;
   SREG.B7=1;

   while(1)
   {

      if((PIND.B0==1) && !(resume_stop_check==2))
      {
         if(resume_stop_check==1)
         {
           TCCR0=0X0D;
           PORTA=store_resumeA;
           resume_stop_check=0;
           ir_check_mode=1;
         }
         else if(resume_stop_check==0)
         {
            TCCR0=0X00;
            PORTA=0xF0;
            PORTB=0X00;
            resume_stop_check=2;
            ir_check_mode=0;
         }
         while(PIND.B0==1);
      }

      else if(resume_stop_check==2)
      {
          while((PIND.B0==0)&(PIND.B1==0)&(PIND.B2==0)&(PIND.B3==0));
           if(PIND.B0==1)
           {
             PORTA=0XE1;
             while(PIND.B0==1);
           }

           else if(PIND.B1==1)
           {
             PORTA=0XD2;
             while(PIND.B1==1);
           }

           else if(PIND.B2==1)
           {
              PORTA=0XB4;
              while(PIND.B2==1);
           }

           else if(PIND.B3==1)
           {
              PORTA=0X78;
              while(PIND.B3==1);
           }

           resume_stop_check=1;

      }

      else if(ir_check_mode==1)
      {

        //To start checking the state, 5 seconds before changing to the next lane
        if( (counter>=(delay_count-167)) && (counter<=delay_count) )
       {
        check_lane();
       }


       /*Logic for Yellow lights- between the transit of green to red;
         yellow signal starts 3 seconds before the signal is going to be red for
         the ongoing lane
       */
       if((counter>=(delay_count-100)) && (counter<=(delay_count+5-100)) )
       {
        /*To retain the values of the green signals and pass the same to yellow;
          not considering the value in higher nibble
        */
        storeB = storeA & 0x0F;
        PORTB = storeB;
        PORTA = 0XF0 & storeA;
        store_resumeA=PORTA;
       }


       //Logic for changing the Red and Green lights for the next lane
       if(counter>=delay_count)
       {
        /*To check the state of the lane and give the time delay for green signal
          of the next lane accordingly
        */
        check_lane_status();
        PORTB=0x00;
        storeA = storeA << 1;
        storeA = storeA + 0X10;
        PORTA = storeA;


          /*After all 4 signals coovered in one circular pattern(cycle),
            start from the first lane again
          */
          if(storeA == 0X00)
          {
                   PORTA=0XE1;
                   storeA=0xE1;
          }

        store_resumeA=PORTA;
        counter=0;
       }
     }
   }
  }