//Item ID's (small list since 1 map)
//0 = Rapier
//1 = Iron Sword
//2 = Steel Sword
//3 = Iron Lance
//4 = Steel Lance
//5 = Silver Lance
//6 = Iron Axe
//7 = Steel Axe
//8 = Heal (Staff)
//9 = Vulnerary (might = HP healed)
int itemMight[10] = {5,5,7,10,14,8,11,10,10};
//If item accuracy = 255, it can never miss
int itemAccuracy[10] = {100,90,80,85,70,80,70,55,255,255};
//Triangle: 0 = Swords, 1 = Lances, 2 = Axes, 3 = Magic, 5 = Item
int itemTriangle[10] = {0,0,0,1,1,1,2,2,2,5,5};
//Range: 1 = 1-range, 2 = 2-range, 3 = 1-2 range
//lol fuck bows
int itemRange[10] = {1,1,1,1,1,1,1,1,1,1};

void getItemName(int itemID, char* itemName)
{
    if (itemID == 0)
    {
        strcpy(itemName,"Rapier");
    }
    else if (itemID == 1)
    {
        strcpy(itemName,"Iron Sword");
    }
    else if (itemID == 2)
    {
        strcpy(itemName,"Steel Sword");
    }
    else if (itemID == 3)
    {
        strcpy(itemName,"Iron Lance");
    }
    else if (itemID == 4)
    {
        strcpy(itemName,"Steel Lance");
    }
    else if (itemID == 5)
    {
        strcpy(itemName,"Silver Lance");
    }
    else if (itemID == 6)
    {
        strcpy(itemName,"Iron Axe");
    }
    else if (itemID == 7)
    {
        strcpy(itemName,"Steel Axe");
    }
    else if (itemID == 8)
    {
        strcpy(itemName,"Heal");
    }
    else if (itemID == 9)
    {
        strcpy(itemName,"Vulnerary");
    }
    else
    {
        strcpy(itemName,"ERR");
    }
    return;
}
