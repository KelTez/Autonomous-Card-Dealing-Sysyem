//#include "NXTServo-lib-UW.c"

/*
    UPDATED CODE IN 2021, TO MAKE IT A BIT MORE READABLE. NO FUNCTIONAL CHANGES DONE, OLD COMMENTS REMAIN.
    THIS IS A BLAKJACK DEALING ROBOT
*/

/*
    Function used to shuffle cards
*/
void shuffle() { 
    motor[motorA] = -45; 
    wait1Msec(100);
    motor[motorC] = -45;

    while(nNxtButtonPressed!=3); 

    motor[motorA], motor[motorC] = 0; 
}

/*
    Function used to deal the cards
*/
void deal(int numCards) { 
    motor[motorB] = -45; 
    wait1Msec(numCards * 500); // takes 0.5 seconds to deal one card motor[motorB] = 0; 
}

/*
    Function used to rotate the robot
*/
 void rotate() { 
    setServoSpeed(S3, 1, 10, -17, 12); 
    motor[motorA] = 15;
    /*
        See note at the bottom as to why motor[motorA] is initialized wait1Msec(500); 
        To get past already detected player while (SensorValue[S1] > 100); 
        motor[motorA] = 0; setServoSpeed(S3, 2, 0, -12, 16); 
    */
}

/*
    Rotate and deal 2 cards to available players
*/
void rotate_and_deal() { 
    rotate(); 
    deal(2); 
}

/*
    This function finds a person and either deals a card or passes. 
*/
void hit_or_pass() { 
    time1[T1] = 0;

    while (time1[t1] < 36000 && SensorValue[s2] == 0)
    {
    	if (sensorValue[s4] > 70) //Used to detect if someone claps via the sound sensor. A clap == deal card/hit.
    	{
    		deal(1);
    		wait1Msec (2000);
    	}
    }
}

/*
    Counting mechanism.
    Players click a button on the robot to increase or decrease the amount of rounds of blackjack they wish to play. 
    Pressing button 3 stops this.
    Also used to pick a winner by selecting the 'numbered' player that won.
*/
int countt() { 
    int count = 0;
    while (nNxtButtonPressed != 3) {
        if (nNxtButtonPressed == 1) { 
            count ++; 

            while (nNxtButtonPressed == 1);

            displayString(1, "Number: "); 
            displayString(2, "%d", count); 

            while (nNxtButtonPressed == -1);

        } else if(nNxtButtonPressed == 2) {
    		count --;

    		while(nNxtButtonPressed == 2);

    		displayString(1, "Number: ");
    		displayString(2,  "%d", count);

    		while (nNxtButtonPressed == -1);
    	}
    }
    eraseDisplay();
    return count;
}

void reset (int &reset_value) { 
    NxtDisplayString (0, "Hit orange: END"); 
    NxtDisplayString (1, "Hit touch: RESET"); 

    while (nNxtButtonPressed != 3 && SensorValue[S2] != 1);

    if (SensorValue[S2] == 1){
        reset_value = 1;
    }
    else if (nNxtButtonPressed == 3){
        reset_value = 0;
    }
  }

 /*
    Main
 */ 

task main() {

    int reset_value = 1;

    SensorType[S1] = sensorSONAR;
    SensorType[S2] = sensorTouch;
    SensorType[S3] = sensorI2CCustom9V; //color sensor
    SensorType[S4] = sensorSoundDB;

    while (reset_value == 1 || reset_value == 2) {
        
        int max_score = 5, n = 0, no_players = 0, winner = 0, rounds = 1, c = 0;

        // Robot C did not allow initialization in the form {0}, so we assume max player count of 10;
        //FUTURE NOTE; makes more sense to to initialize a constant MAX_PLAYERS value instead
        int person[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 

        if (reset_value == 1) {
                nxtDisplayString(1, "Right to increase");
                nxtDisplayString(2, "Left to decrease");
                wait10Msec(200);
                eraseDisplay();
                nxtDisplayString(0, "Number of rounds");
                rounds = countt();
        }

        for (int i = 1; i <= rounds; i++) {
            shuffle();

            while (nNxtButtonPressed == 3);

            while (nNxtButtonPressed !=3);

            nMotorEncoder[motorA] = 0;

            /*
                The robot will rotate and when it detects a player (via rotate_and_deal()) it will deal a card and increase player count.
                Rotates a full 360 degrees.
            */
            while (nMotorEncoder[motorA] < 780){ // 780 rotates 360 as measured by motorEncoder[motorA]
                rotate_and_deal();
                no_players++
                wait10Msec(200);  //Waits for the player to pick up a card
            }

            for (int n = 0; n < no_players; n++) { 
                hit_or_pass(); 
                rotate (); 
            }

            displayString (0, "Input a winner");

            while (nNxtButtonPressed == 3);

            winner = countt();

            person[winner]++;

        }
    
       reset_value = 1;

       for (int i = 1; i <= no_players; i++) { //Checks for max number of wins
            
                if (person[i] > max_score)
                {
                    max_score = person[i];
                    c = i;
                }
        }

        int counter = 0;
        
        for (int i = 1; i <= no_players; i++) // checks to see if there is a tie
        	if (person[i] == max_score)
        		counter ++;


      	if (counter >= 2) {
            reset_value = 2;
            nxtDisplayString (0, "Tie");
            nxtDisplayString (1, "Will play on more round");
            nxtDisplayString (2, "Hit button to continue");
            while (nNxtButtonPressed == 3);
            while(nNxtButtonPressed == -1);
            eraseDisplay();
        } else {
            nxtDisplayString (0, "Winner is person" "%d", c);
            nxtDisplayString (1, "Max score = %d", max_score);
            nxtDisplayString (2, "Hit button to continue");
            while (nNxtButtonPressed == 3);
            while(nNxtButtonPressed == -1);
            eraseDisplay ();
        }
            
    while (nNxtButtonPressed == 3);
        
    if (reset_value != 2)
        reset(reset_value);
    }
}