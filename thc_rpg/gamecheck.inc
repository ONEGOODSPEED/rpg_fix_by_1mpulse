#define GAMECSS GameCSS()
#define GAMECSGO GameCSGO()

stock tValveGame;

stock ValveGameEnum:NCRPG_GetGame() 
{
    return ValveGameEnum:tValveGame;
}

stock DetermineGameMode() 
{
    new String:gameDir[64];
    GetGameFolderName(gameDir, sizeof(gameDir));

    if (StrContains(gameDir, "cstrike", false) == 0) 
    {
        tValveGame = _:Game_CS;
    } 
    else if(StrContains(gameDir, "csgo", false) == 0)
    {
        tValveGame = _:Game_CSGO;
    }
    else
    {
        SetFailState("This game does not support [THC RPG]");
    }
}

stock bool:GameCSS() 
{
    return NCRPG_GetGame() == Game_CS;
}

stock bool:GameCSGO() 
{
    return NCRPG_GetGame() == Game_CSGO;
}