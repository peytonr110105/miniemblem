//Terrain lookup table + funcs
//Terrain list:
//0 - Plains: 1 mov/0 avo/0 def
//1 - Forest 2 mov/20 avo/1 def
//2 - Mountain 255(fliers only)/40 avo/2 def
//3 - Wall: 255(fliers only)/0 avo/0 def
//4 - Throne: 1 mov/30 avo/3 def
//5 - Cliff: 255(fliers only)/0 avo/0 def
//6 - Ocean 254(fliers & pirates only)/0 avo/0 def
//7 - Bridge 1 mov/0 avo/0 def
//8 - Channel 4 mov/10 avo/0 def
//9 - House 1 mov/10 avo/0 def
int terrainMovCost[10] = {1,2,255,255,1,255,254,1,4,1};
int terrainAvoidBonus[10] = {0,20,40,0,30,0,0,0,10,10};
int terrainDefBonus[10] = {0,1,2,0,3,0,0,0,0,0};

void getTerrainName(int terrainID, char* terrainName)
{
    if (terrainID == 0)
    {
        strcpy(terrainName,"Plains");
    }
    else if (terrainID == 1)
    {
        strcpy(terrainName,"Forest");
    }
    else if (terrainID == 2)
    {
        strcpy(terrainName,"Mountain");
    }
    else if (terrainID == 3)
    {
        strcpy(terrainName,"Wall");
    }
    else if (terrainID == 4)
    {
        strcpy(terrainName,"Throne");
    }
    else if (terrainID == 5)
    {
        strcpy(terrainName,"Cliff");
    }
    else if (terrainID == 6)
    {
        strcpy(terrainName,"Ocean");
    }
    else if (terrainID == 7)
    {
        strcpy(terrainName,"Bridge");
    }
    else if (terrainID == 8)
    {
        strcpy(terrainName,"Channel");
    }
    else if (terrainID == 9)
    {
        strcpy(terrainName,"House");
    }
    else
    {
        strcpy(terrainName,"Plains");
    }
    return;
}
