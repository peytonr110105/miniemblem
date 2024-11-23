#include <msp430.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> //probably dont need this
#include <miniemblem_items.h>
#include <miniemblem_units.h>
#include <miniemblem_mapdef_ch01.h>
#include <miniemblem_terrain.h>

#define SW1 P2IN&BIT1 // SW1 def - Pass Turn
#define SW2 P1IN&BIT1 // SW2 def - Inspect Cursor Item

//Global variables for general system funcs
//
volatile unsigned int currentGameState = 0;
unsigned int prevGameState = 0;
//
//currentGameState tracks what mode the game is in
//0 = main menu
//1 = unit editor
//2 = battle event
//3 = map overview
//4 = unit data
//5 = battle sim
//
unsigned int cursorState = 0;
unsigned int cursorCoords[2] = {0,0};
unsigned int selectedUnitID;
unsigned int selectedUnitAffiliation;
//
//cursorState used to determine what the
//cursor should be allowed to do
//0 = free (no units selected, player's turn)
//1 = limited (player unit selected, can't move farther than unit's movement range)
//2 = disabled (when inside a menu)
//
int armyPhase = 0;
//
//armyPhase tracks the phase of the game
//0 = Player
//1 = COM
//2 = NPC (most likely unused)
//

volatile int disableEXP = 0;

volatile int maxPlayerUnits;
volatile int maxEnemyUnits;
// This array holds a list of all the units for both armies
struct Unit playerUnits[10];
struct Unit enemyUnits[10];
//LED Blink events when passing turn
//
//LED1 = COM Active
//LED2 = Player Active
void LED_PASS_TURN_TO_PLAYER()
{
    //g is used for blink func timers
    unsigned int g = 0;
    unsigned int x = 0;
    for (x = 0; x <= 5; x++)
    {
        P4OUT ^= BIT7; //Blink LED1
        for (g = 0; g < 5263; g++); //DELAY: 0.1s -> 5Hz
    }
    P4OUT &= ~BIT7; //LED2 -> ON
    P1OUT |= BIT0; //LED1 -> OFF
    return;
}

void LED_PASS_TURN_TO_COM()
{
    //g is used for blink func timers
    unsigned int g = 0;
    unsigned int x = 0;
    for (x = 0; x <= 5; x++)
    {
        P1OUT ^= BIT0; //Blink LED1
        for (g = 0; g < 5263; g++); //DELAY: 0.1s -> 5Hz
    }
    P4OUT |= BIT7; //LED2 -> ON
    P1OUT &= ~BIT0; //LED1 -> OFF
    return;
}

// Initialize USCI_A0 module to UART mode
void UART_setup(void) {
    P2SEL |= BIT4 + BIT5;   // Set USCI_A0 RXD/TXD to receive/transmit data
    UCA0CTL1 |= UCSWRST;    // Set software reset during initialization
    UCA0CTL0 = 0;           // USCI_A0 control register
    UCA0CTL1 |= UCSSEL_2;   // Clock source SMCLK
    UCA0BR0 = 0x09;         // 1048576 Hz  / 115200 lower byte
    UCA0BR1 = 0x00;         // upper byte
    UCA0MCTL = 0x02;        // Modulation (UCBRS0=0x01, UCOS16=0)
    UCA0CTL1 &= ~UCSWRST;   // Clear software reset to initialize USCI state machine
}

void UART_sendCharacter(char c)
{
    while (!(UCA0IFG && UCTXIFG));   // Wait for previous character to transmit
    UCA0TXBUF = c;                // Put character into tx buffer
}

void UART_sendString(char* str)
{
    UART_sendCharacter('\r'); //Send newline
    UART_sendCharacter('\n'); //
    while (!(UCA0IFG && UCTXIFG));
    int count = strlen(str);
    int xtmp = 0;
    for (xtmp; xtmp < count;xtmp++)
    {
        UART_sendCharacter(str[xtmp]); //use sendChar as helper func
    }
    UART_sendCharacter('\r'); //Send newline
    UART_sendCharacter('\n'); //
}

char UART_getCharacter()
{
    while (!(UCA0IFG && UCRXIFG)); // Wait for a new character
    //Incoming character written to UCA0RXBUF
    while (!(UCA0IFG && UCTXIFG)); // Wait until TXBUF is free
    UCA0TXBUF = UCA0RXBUF; //Echo input
    P5OUT ^= BIT1;
    return UCA0RXBUF;
}

void UART_getLine(char* buf, int limit)
{
    int bufCap = 0;
    char incomingChar;
    for (bufCap; bufCap < limit;bufCap++)
    {
        incomingChar = UART_getCharacter();
        if (incomingChar != '\r')
        {
            buf[bufCap] = incomingChar;
        }
        else if (incomingChar == '\b')
        {
            buf[bufCap] = '\0'; //Cap off string
            bufCap--; //Roll back 1 character in array
            UART_sendCharacter('\0'); //Erases previous character
            UART_sendCharacter('\b'); //Moves cursor back after erasing
        }
        else
        {
            buf[bufCap] = '\0'; //Terminates string
            bufCap = 255;
            break;
        }
    }
    //If loop does not exit with special code,
    //the buffer has been exceeded.
    if (bufCap != 255)
    {
        buf[bufCap] = '\0'; //Terminate string
        UART_sendString("WARNING: Buffer Overflow detected!");
    }
}

int get_Random_Number() //Gets a random number from Timer A
{
    int rn1 = 0; //Initialize rn variables
    int rn2 = 0;
    int finalRN = 0;
    rn1 = TA0R; //Read contents of Timer A0
    rn1 = rn1 % 100; //Modulo 100 to get a random number from 0->100
    unsigned int g;
    for (g = 0; g < 5263; g++); //wait a little bit to get another number
    rn2 = TA0R;
    rn2 = rn2 % 100;
    finalRN = ((rn1 + rn2) / 2); //Average the 2 RN's to fudge numbers to be more consistent
    return finalRN;
}

int get_single_RN()
{
    int rn1 = 0; //Initialize rn variables
    rn1 = TA0R; //Read contents of Timer A0
    rn1 = rn1 % 100; //Modulo 100 to get a random number from 0->100
    return rn1;
}

void clearScreen()
{
    UART_sendString('\033[2J');
    unsigned int g;
    for (g = 0; g < 5263; g++); //wait a little bit to make the clear look nice
}

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // Configure pins
    //
    // Notes on hardware commands & behavior:
    // MSP430 switches use low (0) to signal a press
    //
    // Pin commands
    // ^= -> XOR readout from pin (toggle)
    // &= ~PINx -> Set 0
    // |= -> Set 1
    //
    P1DIR |= BIT0; //Pin 1.0 to output (LED1 CONTROL)
    P4DIR |= BIT7; //Pin 4.7 to output (LED2 CONTROL)
    P2DIR &= ~BIT1; //Pin 2.1 -> SW1 input
    P1DIR &= ~BIT1; //Pin 1.1 -> SW2 input
    P2REN |= BIT1; //Enable Pull-up resistor at P2.1
    P2OUT |= BIT1; //Input/Output for P2.1
    P1OUT |= BIT1; //I/O for P1.1
    P1REN |= BIT1; //Pull-up resistor at P1.1
    //Init LEDs
    P1OUT |= BIT0; //LED1 -> ON
    P4OUT &= ~BIT7; //LED2 -> OFF
    //
    //Configure Timer
    // Timer A acts as our random number generator.
    //
    TA0CCTL0 = TASSEL_2 + MC_1 + ID_3; //SMCLK, Up-mode
    TA0CCR0 = 10000;
    //Player input is usually one character at a time
    char playerInput;
    //temp string for system output
    char sysOutStr[127];
    unsigned long int g = 0;
    //i is used for main func timers (debouncing)
    unsigned long int i = 0;
    while (1)
    {
        if (currentGameState == 0) //Main Menu
        {
            UART_sendString("---===Welcome to MiniEmblem===---");
            UART_sendString("Press 'D' to play the demo, or 'B' to use the Battle Simulator");
            playerInput = UART_getCharacter();
            if (playerInput == 'D')
            {
                clearScreen();
                UART_sendString("Implement me!");
            }
            else if (playerInput == 'B')
            {
                clearScreen();
                UART_sendString("Launching Battle Simulator...");
                currentGameState = 1; //Go into Unit Setup menu
                for (g = 0; g < 5263; g++); //DELAY: 0.1s -> 5Hz, use delay to make it so LEDS dont go crazy while this is a stub
            }
            else
            {
                UART_sendString("Invalid command.");
            }
        }
        else if (currentGameState == 1) //Unit Editor
        {
            clearScreen();
            UART_sendString("~~~MiniEmblem Unit Editor~~~");
            UART_sendString("Would you like to configure units manually, or use a pre-defined roster?");
            UART_sendString("Enter 0 for manual config, 1 for pre-defined: ");
            playerInput = UART_getCharacter();
            if (playerInput == '0')
            {
                struct Unit plrUnit1,plrUnit2,plrUnit3,plrUnit4,plrUnit5;
                manualUnitEditor(plrUnit1);
                manualUnitEditor(plrUnit2);
                manualUnitEditor(plrUnit3);
                manualUnitEditor(plrUnit4);
                manualUnitEditor(plrUnit5);
                playerUnits[0] = plrUnit1;
                playerUnits[1] = plrUnit2;
                playerUnits[2] = plrUnit3;
                playerUnits[3] = plrUnit4;
                playerUnits[4] = plrUnit5;
                struct Unit comUnit1,comUnit2,comUnit3,comUnit4,comUnit5;
                manualUnitEditor(comUnit1);
                manualUnitEditor(comUnit2);
                manualUnitEditor(comUnit3);
                manualUnitEditor(comUnit4);
                manualUnitEditor(comUnit5);
                enemyUnits[0] = comUnit1;
                enemyUnits[1] = comUnit2;
                enemyUnits[2] = comUnit3;
                enemyUnits[3] = comUnit4;
                enemyUnits[4] = comUnit5;
            }
            else
            {
                maxPlayerUnits = 5;
                maxEnemyUnits = 5;
                struct Unit plrUnit1,plrUnit2,plrUnit3,plrUnit4,plrUnit5;
                loadPreset1(plrUnit1,0,0);
                loadPreset2(plrUnit2,0,1);
                loadPreset3(plrUnit3,0,2);
                loadPreset4(plrUnit4,0,3);
                loadPreset5(plrUnit5,0,4);
                playerUnits[0] = plrUnit1;
                playerUnits[1] = plrUnit2;
                playerUnits[2] = plrUnit3;
                playerUnits[3] = plrUnit4;
                playerUnits[4] = plrUnit5;
                struct Unit comUnit1,comUnit2,comUnit3,comUnit4,comUnit5;
                loadPreset1(comUnit1,1,0);
                loadPreset2(comUnit2,1,1);
                loadPreset3(comUnit3,1,2);
                loadPreset4(comUnit4,1,3);
                loadPreset5(comUnit5,1,4);
                enemyUnits[0] = comUnit1;
                enemyUnits[1] = comUnit2;
                enemyUnits[2] = comUnit3;
                enemyUnits[3] = comUnit4;
                enemyUnits[4] = comUnit5;
            }
            UART_sendString("Configuration complete. Press any key to continue to the simulator.");
            UART_getCharacter();
            currentGameState = 5;
        }
        else if (currentGameState == 5) //Battle Simulator
        {
            clearScreen();
            UART_sendString("Player units:");
            int x;
            for (x = 0; x < maxPlayerUnits;x++)
            {
                strcpy(sysOutStr,playerUnits[x].unitName); //Copy unit name into outstr
                if (playerUnits[x].unitState != 1) //Display unit ID next to alive characters
                {
                    char tmpstr2[12];
                    strcat(sysOutStr," (");
                    snprintf(tmpstr2,12,"%d)",x);
                    //Tack on unit ID
                    strcat(sysOutStr,playerInput);
                    if (playerUnits[x].unitState == 2)
                    {
                        strcat(sysOutStr," (Already acted)");
                    }
                }
                else //Display (DEAD) next to dead characters
                {
                    strcat(sysOutStr," (DEAD)");
                }
                UART_sendString(sysOutStr);
            }
            UART_sendString("Enemy units:");
            for (x = 0; x < maxEnemyUnits;x++)
            {
                strcpy(sysOutStr,enemyUnits[x].unitName); //Copy unit name into outstr
                if (enemyUnits[x].unitState != 1) //Display unit ID next to alive characters
                {
                    char tmpstr2[12];
                    strcat(sysOutStr," (");
                    snprintf(tmpstr2,12,"%d)",x);
                    //Tack on unit ID
                    strcat(sysOutStr,playerInput);
                    if (enemyUnits[x].unitState == 2)
                    {
                        strcat(sysOutStr," (Already acted)");
                    }
                }
                else //Display (DEAD) next to dead characters
                {
                    strcat(sysOutStr," (DEAD)");
                }
                UART_sendString(sysOutStr);
            }
            if (armyPhase == 0)
            {
                UART_sendString("PLAYER, enter unit ID to command: ");
                playerInput = UART_getCharacter();
                selectedUnitID = atoi(playerInput);
                //Stop player from making an invalid selection (out-of-bounds or unit is not actionable)
                while (selectedUnitID > maxPlayerUnits || selectedUnitID < 0 || playerUnits[selectedUnitID].unitState != 0)
                {
                    UART_sendString("Invalid selection! Select another unit to command.");
                    playerInput = UART_getCharacter();
                    selectedUnitID = atoi(playerInput);
                }
                strcpy(sysOutStr,"You are commanding: ");
                strcat(sysOutStr,playerUnits[selectedUnitID].unitName);
                UART_sendString(sysOutStr);
                UART_sendString("Enter enemy unit ID to attack, enter 'c' to cancel, or press Switch 2 to view unit page.");
                playerInput = UART_getCharacter();
                if (playerInput != 'c')
                {
                  int tempEnemyID = atoi(playerInput);
                  while (enemyUnits[tempEnemyID].unitState == 1 || tempEnemyID < 0 || tempEnemyID > maxEnemyUnits)
                  {
                      UART_sendString("Invalid enemy selection! Select another enemy.");
                      playerInput = UART_getCharacter();
                      tempEnemyID = atoi(playerInput);
                  }
                  runBattleEvent(playerUnits[selectedUnitID],enemyUnits[tempEnemyID]); //Run battle event, write results to tempEnemyID
                }
            }
        }
        else if (currentGameState == 2)
        {
            if (armyPhase == 1) //stub code, make COM immediately pass turn to player
            {
                LED_PASS_TURN_TO_PLAYER();
                armyPhase = 0;
                UART_sendString("~~~PLAYER PHASE~~~");
                unsigned int g;
                for (g = 0; g < 5263; g++); //DELAY: 0.1s -> 5Hz, use delay to make it so LEDS dont go crazy while this is a stub
            }
        }
        else if (currentGameState == 4)
        {
            playerInput = UART_getCharacter();
            int selectedItem = atoi(playerInput);
            getItemName(playerUnits[selectedUnitID].unitItems[selectedItem],sysOutStr);
            if (playerInput > 0 && playerInput < 4 && strcmp(sysOutStr,"ERR") != 0) //Only equip/use items if item is valid
            {
                if(playerUnits[selectedUnitID].unitItems[selectedItem] == 9) //Vulnerary logic
                {
                    int g;
                    playerUnits[selectedUnitID].unitItems[selectedItem] = 255; //Remove vulnerary from inventory
                    playerUnits[selectedUnitID].unitStats[8] = playerUnits[selectedUnitID].unitStats[0]; //Set HP to full
                    for (g = 1;g < 5;g++) //Move items in unit inventory to next empty slot
                    {
                        if (playerUnits[selectedUnitID].unitItems[selectedItem - 1] == 255)
                        {
                            playerUnits[selectedUnitID].unitItems[selectedItem - 1] = playerUnits[selectedUnitID].unitItems[selectedItem]; //move item up
                            playerUnits[selectedUnitID].unitItems[selectedItem] = 255; //set current slot to empty to make logic hold for rest of loop
                        }
                    }
                    showPlayerUnitPage(playerUnits[selectedUnitID]);
                    playerUnits[selectedUnitID].unitState = 2; //End unit's turn for using vulnerary
                    UART_sendLine("Vulnerary used. Unit's HP was fully restored.");
                }
                else //Equip item
                {
                    int tempItem = playerUnits[selectedUnitID].unitItems[0];
                    playerUnits[selectedUnitID].unitItems[0] = playerUnits[selectedUnitID].unitItems[selectedItem];
                    playerUnits[selectedUnitID].unitItems[selectedItem] = tempItem;
                }
            }
        }
    }
}

//Battle events, the real meat of the program
//and what will save my grade
//Returns 0 if foe is still alive
//Returns 1 if foe dies
//Returns 2 if player dies
int runBattleEvent(struct Unit playerUnit,struct Unit enemyUnit)
{
    int damage;
    char tempString[127];
    UART_sendString("The battle begins...");
    strcpy(tempString,playerUnit.unitName);
    strcat(tempString," attacks!");
    UART_sendString(tempString);
    int foeAvo = enemyUnit.unitStats[2] + getTerrainBonus(enemyUnit);
    int plrHit = getAccuracy(playerUnit) - foeAvo;
    if (getRandomNumber() <= plrHit)
    {
        UART_sendString("The attack struck!");
        snprintf(tempString,127,"%d damage was dealt!",damage);
        enemyUnit.unitStats[8] -= damage;
        if (enemyUnit.unitStats[8] <= 0)
        {
            enemyUnit.unitStats[8] = 0;
            enemyUnit.unitState = 2;
            UART_sendString("Enemy was defeated!");
            if (enemyUnit.unitAffiliation == 1 && disableEXP != 1) //Award XP to player unit
            {
                int exp = 34 + (playerUnit.unitStats[9] - enemyUnit.unitStats[9]); //Reduce EXP gain if player level > foe level
                awardEXP(playerUnit,exp);
                UART_sendString("Press any key to continue...");
                UART_getCharacter();
                return 1;
            }
        }
    }
    else
    {
        UART_sendString("The attack missed!");
    }
    if (itemRange[playerUnit.unitItems[0]] == itemRange[enemyUnit.unitItems[0]] && enemyUnit.unitState != 1) //Enemy counter-attacks if ranges match and he's not dead
    {
        UART_sendString("The defender counter-attacks!");
        foeAvo = playerUnit.unitStats[2] + getTerrainBonus(playerUnit);
        plrHit = getAccuracy(enemyUnit) - foeAvo;
        if (getRandomNumber() <= plrHit)
        {
            UART_sendString("The attack struck!");
            snprintf(tempString,127,"%d damage was dealt!",damage);
            playerUnit.unitStats[8] -= damage;
            if (playerUnit.unitStats[8] <= 0)
            {
                playerUnit.unitStats[8] = 0;
                playerUnit.unitState = 2;
                UART_sendString("Player was defeated!");
                UART_sendString("Press any key to continue...");
                UART_getCharacter();
                return 2;
            }
        }
    }
    UART_sendString("Press any key to continue...");
    UART_getCharacter();
    return 0;
}

void awardEXP(struct Unit playerUnit,int exp)
{
    char tempStr[127];
    char tempStr2[15];
    playerUnit.unitStats[10] += exp;
    if (playerUnit.unitStats[10] >= 100)
    {
        playerUnit.unitStats[10] -= 100;
        UART_sendString("--==Level up!==--");
        //Level-up shit goes here
        //Can't really stick this in a loop since console needs output :(
        if (get_Single_RN() <= playerUnit.unitGrowths[0])
        {
            UART_sendString("Health increased!");
            playerUnit.unitStats[0] += 1;
        }
        if (get_Single_RN() <= playerUnit.unitGrowths[1])
        {
            UART_sendString("Strength increased!");
            playerUnit.unitStats[1] += 1;
        }
        if (get_Single_RN() <= playerUnit.unitGrowths[2])
        {
            UART_sendString("Speed increased!");
            playerUnit.unitStats[2] += 1;
        }
        if (get_Single_RN() <= playerUnit.unitGrowths[3])
        {
            UART_sendString("Luck increased!");
            playerUnit.unitStats[3] += 1;
        }
        if (get_Single_RN() <= playerUnit.unitGrowths[4])
        {
            UART_sendString("Skill increased!");
            playerUnit.unitStats[4] += 1;
        }
        if (get_Single_RN() <= playerUnit.unitGrowths[5])
        {
            UART_sendString("Defense increased!");
            playerUnit.unitStats[5] += 1;
        }
        if (get_Single_RN() <= playerUnit.unitGrowths[6])
        {
            UART_sendString("Magic increased!");
            playerUnit.unitStats[6] += 1;
        }
        if (get_Single_RN() <= playerUnit.unitGrowths[7])
        {
            UART_sendString("Resistance increased!");
            playerUnit.unitStats[7] += 1;
        }
    }
    stcpy(tempStr,playerUnit.unitName);
    strcat(tempStr," now has ");
    snprintf(tempStr2,15,"%d EXP.",playerUnit.unitStats[10]);
    strcat(tempStr,tempStr2);
    UART_sendString(tempStr);
    return;
}

int getDamageDealt(struct Unit playerUnit,struct Unit enemyUnit)
{
    int plrPow = playerUnit.unitStats[1] + itemMight[playerUnit.unitItems[0]];
    int damage = plrPow - enemyUnit.unitStats[5];
    if (damage < 0)
    {
        return 0;
    }
    return damage;
}

int getTerrainBonus(struct Unit playerUnit)
{
    return terrainAvoidBonus[mapLayout[playerUnit.unitCoords[0]][playerUnit.unitCoords[1]]];
}

int getTerrainDefBonus(struct Unit playerUnit)
{
    return terrainDefBonus[mapLayout[playerUnit.unitCoords[0]][playerUnit.unitCoords[1]]];
}

void manualUnitEditor(struct Unit tempunit)
{
    char playerStringIn[10];
    UART_sendString("~~~MiniEmblem Unit Editor~~~");
    UART_sendString("Enter unit name (Max of 10 characters): ");
    UART_getLine(playerStringIn,10);
    strcpy(tempunit.unitName,playerStringIn);
    UART_sendString("Enter unit's base level (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[9] = atoi(playerStringIn);
    UART_sendString("Enter unit's max HP (Max: 60): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[0] = atoi(playerStringIn);
    UART_sendString("Enter unit's Strength (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[1] = atoi(playerStringIn);
    UART_sendString("Enter unit's Speed (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[2] = atoi(playerStringIn);
    UART_sendString("Enter unit's Luck (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[3] = atoi(playerStringIn);
    UART_sendString("Enter unit's Skill (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[4] = atoi(playerStringIn);
    UART_sendString("Enter unit's Defence (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[5] = atoi(playerStringIn);
    UART_sendString("Enter unit's Resistance (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[6] = atoi(playerStringIn);
    UART_sendString("Enter unit's Magic (Max: 20): ");
    UART_getLine(playerStringIn,2);
    tempunit.unitStats[7] = atoi(playerStringIn);
    UART_sendString("Set unit's weapon: ");
    UART_getLine(playerStringIn,2);
    tempunit.unitItems[0] = atoi(playerStringIn);
    UART_sendString("Set unit affiliation (0 - player, 1 - enemy): ");
    UART_getLine(playerStringIn,1);
    tempunit.unitAffiliation = atoi(playerStringIn);
    return;
}

void showPlayerUnitPage(struct Unit tempunit)
{
    clearScreen();
    char sysOut[127];
    char tmpSysOut[15];
    //Unit page header
    strcpy(sysOut,tempunit.unitName);
    strcat(sysOut,"'s Unit Page:");
    UART_sendLine(sysOut);
    //MAX HP
    strcpy(sysOut,"Current HP / MAX: ");
    snprintf(tmpSysOut,15,"%d / %d",tempunit.unitStats[8],tempunit.unitStats[0]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //STR
    strcpy(sysOut,"Strength: ");
    snprintf(tmpSysOut,15,"%d",tempunit.unitStats[1]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //SPD
    strcpy(sysOut,"Speed: ");
    snprintf(tmpSysOut,15,"%d",tempunit.unitStats[2]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //LCK
    strcpy(sysOut,"Luck: ");
    snprintf(tmpSysOut,15,"%d",tempunit.unitStats[3]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //SKL
    strcpy(sysOut,"Skill: ");
    snprintf(tmpSysOut,15,"%d",tempunit.unitStats[4]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //DEF
    strcpy(sysOut,"Defence: ");
    snprintf(tmpSysOut,15,"%d",tempunit.unitStats[5]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //RES
    strcpy(sysOut,"Resistance: ");
    snprintf(tmpSysOut,15,"%d",tempunit.unitStats[6]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //MAG
    strcpy(sysOut,"Magic: ");
    snprintf(tmpSysOut,15,"%d",tempunit.unitStats[7]);
    strcat(sysOut,tmpSysOut);
    UART_sendLine(sysOut);
    //Held items
    int x = 0;
    UART_sendLine("Held items (0-4): ");
    for (x;x < 5;x++)
    {
        getItemName(tempunit.unitItems[x],sysOut);
        if (strcmp(sysOut,"ERR") != 0)
        {
            UART_sendLine(sysOut);
        }
    }
    UART_sendLine("Enter 0-4 to equip / use that slot's item, or press SW2 to exit");
}

// Switch Interrupts
#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR (void)
{
    int i = 1;
    for (i;i < 2000;i++); //Debouncing
    //
    //
    //Main Menu commands

    if ((SW1) == 0 && armyPhase == 0) //If player phase and SW1 pressed, pass turn to COM
    {
        LED_PASS_TURN_TO_COM();
        armyPhase = 1;
        UART_sendString("~~~ENEMY PHASE~~~");
        unsigned int g;
        for (g = 0; g < 5263; g++); //DELAY: 0.1s -> 5Hz, use delay to make it so LEDS dont go crazy while this is a stub
    }
    else if ((SW1) == 0 && (SW2) == 0)
    {
    }
    else if ((SW1) == 1 && (SW2) == 0 && (currentGameState == 5 || currentGameState == 2)) //Unit Page
    {
        if (selectedUnitAffiliation == 0)
        {
            showPlayerUnitPage(playerUnits[selectedUnitID]);
        }
        else
        {
            showEnemyUnitPage(enemyUnits[selectedUnitID]);
        }
    }
    else if ((SW1) == 1 && (SW2) == 0 && currentGameState == 4) //Press SW2 again to exit unit page
    {
        clearScreen();
        currentGameState = prevGameState;
    }
    else //return to default
    {
    }
    P1IFG &= ~BIT0; //clear interrupt flags
    P1IFG &= ~BIT1;
}
