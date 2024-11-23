//Define maximum unit level as 20
int maxLevel = 20;

//Class mov in tiles
int classMov[9] = {5,7,8,7,4,5,5,5,5};

void getUnitClass(char* classStr,int unitClass)
{
    if (unitClass == 0)
    {
        strcpy(classStr,"Lord");
    }
    else if (unitClass == 1)
    {
        strcpy(classStr,"Cavalier");
    }
    else if (unitClass == 2)
    {
        strcpy(classStr,"Paladin");
    }
    else if (unitClass == 3)
    {
        strcpy(classStr,"Pegasus Kn.");
    }
    else if (unitClass == 4)
    {
        strcpy(classStr,"Sw. Knight");
    }
    else if (unitClass == 5)
    {
        strcpy(classStr,"Archer");
    }
    else if (unitClass == 6)
    {
        strcpy(classStr,"Curate");
    }
    else if (unitClass == 7)
    {
        strcpy(classStr,"Fighter");
    }
    else if (unitClass == 8)
    {
        strcpy(classStr,"Pirate");
    }
    else
    {
        strcpy(classStr,"ERR");
    }
    return;
}

struct Unit
{
    char unitName[10];
    //Stats are allocated as such:
    //0: Max HP
    //1: STR
    //2: SPD
    //3: LCK
    //4: SKL
    //5: DEF
    //6: RES
    //7: MAG
    //8: Current Health
    //9: Current Level
    //10: EXP (get 100 to level-up)
    int unitStats[11];
    int unitItems[5];
    //Growths follow same alloc pattern as base stats
    int unitGrowths[8];
    //Unit Affiliation defines who controls this unit
    //0 - player
    //1 - enemy
    //2 - npc
    int unitAffiliation;
    //probably won't be used
    int unitClass;
    //0 = x-coord, 1 = y-coord
    int unitCoords[2];
    //Unit state, 0 = alive, 1 = dead, 2 = turn used
    int unitState;
    int unitID;
};

//Unit presets go here, call func and give it a Unit struct

//Preset 1 = FE1 Marth
void loadUnitPreset1(struct Unit tempunit,int affiliation, int unitID)
{
    strcpy(tempunit.unitName,"Mars");
    tempunit.unitStats[9] = 1; //Join LVL
    tempunit.unitStats[0] = 18; //Max HP
    tempunit.unitStats[1] = 5; //STR
    tempunit.unitStats[2] = 7; //SPD
    tempunit.unitStats[3] = 7; //LCK
    tempunit.unitStats[4] = 3; //SKL
    tempunit.unitStats[5] = 7; //DEF
    tempunit.unitStats[6] = 0; //RES
    tempunit.unitStats[7] = 0; //MAG
    tempunit.unitStats[8] = 18; //Current HP
    tempunit.unitStats[10] = 0; //EXP, set to 0 for player units
    tempunit.unitGrowths[0] = 90; //HP
    tempunit.unitGrowths[1] = 50; //STR
    tempunit.unitGrowths[2] = 50; //SPD
    tempunit.unitGrowths[3] = 70; //LCK
    tempunit.unitGrowths[4] = 40; //SKL
    tempunit.unitGrowths[5] = 20; //DEF
    tempunit.unitGrowths[6] = 0; //RES
    tempunit.unitGrowths[7] = 0; //MAG
    tempunit.unitItems[0] = 0; //Rapier
    tempunit.unitAffiliation = affiliation;
    tempunit.unitClass = 0;
    tempunit.unitCoords[0] = 0;
    tempunit.unitCoords[1] = 0;
    tempunit.unitState = 0;
    tempunit.unitID = unitID;
}

//Preset 2 = FE1 Jagen
void loadUnitPreset2(struct Unit tempunit,int affiliation, int unitID)
{
    strcpy(tempunit.unitName,"Jeigan");
    tempunit.unitStats[9] = 1; //Join LVL
    tempunit.unitStats[0] = 20; //Max HP
    tempunit.unitStats[1] = 7; //STR
    tempunit.unitStats[2] = 8; //SPD
    tempunit.unitStats[3] = 1; //LCK
    tempunit.unitStats[4] = 10; //SKL
    tempunit.unitStats[5] = 9; //DEF
    tempunit.unitStats[6] = 0; //RES
    tempunit.unitStats[7] = 0; //MAG
    tempunit.unitStats[8] = 20; //Current HP
    tempunit.unitStats[10] = 0; //EXP, set to 0 for player units
    tempunit.unitGrowths[0] = 10; //HP
    tempunit.unitGrowths[1] = 10; //STR
    tempunit.unitGrowths[2] = 10; //SPD
    tempunit.unitGrowths[3] = 10; //LCK
    tempunit.unitGrowths[4] = 10; //SKL
    tempunit.unitGrowths[5] = 10; //DEF
    tempunit.unitGrowths[6] = 0; //RES
    tempunit.unitGrowths[7] = 0; //MAG
    tempunit.unitItems[0] = 5; //Silver Lance
    tempunit.unitItems[1] = 1; //Iron Sword
    tempunit.unitAffiliation = affiliation;
    tempunit.unitClass = 2;
    tempunit.unitCoords[0] = 0;
    tempunit.unitCoords[1] = 0;
    tempunit.unitState = 0;
    tempunit.unitID = unitID;
}

//Preset 3 = FE1 Cain/Abel
void loadUnitPreset3(struct Unit tempunit,int affiliation, int unitID)
{
    strcpy(tempunit.unitName,"Cabel");
    tempunit.unitStats[9] = 3; //Join LVL
    tempunit.unitStats[0] = 18; //Max HP
    tempunit.unitStats[1] = 7; //STR
    tempunit.unitStats[2] = 6; //SPD
    tempunit.unitStats[3] = 3; //LCK
    tempunit.unitStats[4] = 4; //SKL
    tempunit.unitStats[5] = 6; //DEF
    tempunit.unitStats[6] = 0; //RES
    tempunit.unitStats[7] = 0; //MAG
    tempunit.unitStats[8] = 18; //Current HP
    tempunit.unitStats[10] = 0; //EXP, set to 0 for player units
    tempunit.unitGrowths[0] = 70; //HP
    tempunit.unitGrowths[1] = 50; //STR
    tempunit.unitGrowths[2] = 50; //SPD
    tempunit.unitGrowths[3] = 40; //LCK
    tempunit.unitGrowths[4] = 50; //SKL
    tempunit.unitGrowths[5] = 20; //DEF
    tempunit.unitGrowths[6] = 0; //RES
    tempunit.unitGrowths[7] = 0; //MAG
    tempunit.unitItems[0] = 3; //Iron Lance
    tempunit.unitItems[1] = 1; //Iron Sword
    tempunit.unitAffiliation = affiliation;
    tempunit.unitClass = 2;
    tempunit.unitCoords[0] = 0;
    tempunit.unitCoords[1] = 0;
    tempunit.unitState = 0;
    tempunit.unitID = unitID;
}

//Preset 4 = FE1 Draug
void loadUnitPreset4(struct Unit tempunit,int affiliation, int unitID)
{
    strcpy(tempunit.unitName,"Doga");
    tempunit.unitStats[9] = 1; //Join LVL
    tempunit.unitStats[0] = 18; //Max HP
    tempunit.unitStats[1] = 7; //STR
    tempunit.unitStats[2] = 3; //SPD
    tempunit.unitStats[3] = 1; //LCK
    tempunit.unitStats[4] = 3; //SKL
    tempunit.unitStats[5] = 11; //DEF
    tempunit.unitStats[6] = 0; //RES
    tempunit.unitStats[7] = 0; //MAG
    tempunit.unitStats[8] = 18; //Current HP
    tempunit.unitStats[10] = 0; //EXP, set to 0 for player units
    tempunit.unitGrowths[0] = 60; //HP
    tempunit.unitGrowths[1] = 20; //STR
    tempunit.unitGrowths[2] = 40; //SPD
    tempunit.unitGrowths[3] = 20; //LCK
    tempunit.unitGrowths[4] = 40; //SKL
    tempunit.unitGrowths[5] = 10; //DEF
    tempunit.unitGrowths[6] = 0; //RES
    tempunit.unitGrowths[7] = 0; //MAG
    tempunit.unitItems[0] = 1; //Iron Sword
    tempunit.unitItems[1] = 9; //Vulnerary
    tempunit.unitAffiliation = affiliation;
    tempunit.unitClass = 4;
    tempunit.unitCoords[0] = 0;
    tempunit.unitCoords[1] = 0;
    tempunit.unitState = 0;
    tempunit.unitID = unitID;
}

//Preset 5 = FE1 Wrys
void loadUnitPreset5(struct Unit tempunit,int affiliation, int unitID)
{
    strcpy(tempunit.unitName,"Wrys");
    tempunit.unitStats[9] = 1; //Join LVL
    tempunit.unitStats[0] = 16; //Max HP
    tempunit.unitStats[1] = 1; //STR
    tempunit.unitStats[2] = 7; //SPD
    tempunit.unitStats[3] = 1; //LCK
    tempunit.unitStats[4] = 5; //SKL
    tempunit.unitStats[5] = 3; //DEF
    tempunit.unitStats[6] = 0; //RES
    tempunit.unitStats[7] = 0; //MAG
    tempunit.unitStats[8] = 16; //Current HP
    tempunit.unitStats[10] = 0; //EXP, set to 0 for player units
    tempunit.unitGrowths[0] = 20; //HP
    tempunit.unitGrowths[1] = 0; //STR
    tempunit.unitGrowths[2] = 10; //SPD
    tempunit.unitGrowths[3] = 10; //LCK
    tempunit.unitGrowths[4] = 0; //SKL
    tempunit.unitGrowths[5] = 0; //DEF
    tempunit.unitGrowths[6] = 0; //RES
    tempunit.unitGrowths[7] = 0; //MAG
    tempunit.unitItems[0] = 5; //Silver Lance
    tempunit.unitItems[1] = 1; //Iron Sword
    tempunit.unitAffiliation = affiliation;
    tempunit.unitClass = 2;
    tempunit.unitCoords[0] = 0;
    tempunit.unitCoords[1] = 0;
    tempunit.unitState = 0;
    tempunit.unitID = unitID;
}

//Enemy Preset 1 = FE1 Pirates
void loadEnemyPreset1(struct Unit tempunit,int affiliation, int unitID)
{
    strcpy(tempunit.unitName,"Pirate");
    tempunit.unitStats[9] = 1; //Join LVL
    tempunit.unitStats[0] = 18; //Max HP
    tempunit.unitStats[1] = 5; //STR
    tempunit.unitStats[2] = 6; //SPD
    tempunit.unitStats[3] = 0; //LCK
    tempunit.unitStats[4] = 1; //SKL
    tempunit.unitStats[5] = 4; //DEF
    tempunit.unitStats[6] = 0; //RES
    tempunit.unitStats[7] = 0; //MAG
    tempunit.unitStats[8] = 18; //Current HP
    tempunit.unitStats[10] = 0; //EXP, set to 0 for player units
    tempunit.unitGrowths[0] = 20; //HP
    tempunit.unitGrowths[1] = 0; //STR
    tempunit.unitGrowths[2] = 10; //SPD
    tempunit.unitGrowths[3] = 10; //LCK
    tempunit.unitGrowths[4] = 0; //SKL
    tempunit.unitGrowths[5] = 0; //DEF
    tempunit.unitGrowths[6] = 0; //RES
    tempunit.unitGrowths[7] = 0; //MAG
    tempunit.unitItems[0] = 6; //Iron Axe
    tempunit.unitAffiliation = affiliation;
    tempunit.unitClass = 8;
    tempunit.unitCoords[0] = 0;
    tempunit.unitCoords[1] = 0;
    tempunit.unitState = 0;
    tempunit.unitID = unitID;
}
