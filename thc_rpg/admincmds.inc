/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          admincmds.inc
 *  Type:          Core
 *  Description:   hooks all admin commands
 *                 contains admin functions and the admin menu
 *
 *  Copyright (C) 2009-2011  ArsiRC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

#pragma semicolon 1

#undef REQUIRE_PLUGIN
#include <adminmenu>

new Handle:hAdminMenu = INVALID_HANDLE;

HookAdminCmds()
{
    // Hook all admin cmds
	RegConsoleCmd("thc_rpg_xp", AdminCmd_XP,"THC RPG Admin Command", FCVAR_PLUGIN);
	RegConsoleCmd("thc_rpg_level", AdminCmd_Level,"THC RPG Admin Command", FCVAR_PLUGIN);
	RegConsoleCmd("thc_rpg_credits", AdminCmd_Credits,"THC RPG Admin Command", FCVAR_PLUGIN);
	RegConsoleCmd("thc_rpg_resetplayer", AdminCmd_ResetPlayer,"THC RPG Admin Command", FCVAR_PLUGIN);
	RegConsoleCmd("thc_rpg_resetallplayers", AdminCmd_ResetAllPlayers,"THC RPG Admin Command", FCVAR_PLUGIN);
	RegConsoleCmd("thc_rpg_resetdb", AdminCmd_ResetDB,"THC RPG Admin Command", FCVAR_PLUGIN);
	RegConsoleCmd("thc_rpg_delete", AdminCmd_DeletePlayer,"THC RPG Admin Command", FCVAR_PLUGIN);
}

public Action:AdminCmd_XP(client,argc)
{
	if(!CheckAdminAccess(client,sAdminFlags))
	{
		CReplyToCommand(client, "No Access", client);
		return Plugin_Handled;
	}
	
    if(argc>=3)
    {
        decl String:action[8],String:target[64],String:amount[8];
        decl String:adminname[64],String:name[64];
        new newamt, clients[MAXPLAYERS];
        GetCmdArg(1,action,sizeof(action));
        GetCmdArg(2,target,sizeof(target));
        GetCmdArg(3,amount,sizeof(amount));
        GetClientName(client,adminname,sizeof(adminname));
        new count=FindMatchingPlayers(target,clients);
        for(new x=0;x<count;x++)
        {
            if(IsValidPlayer(clients[x]))
            {
                GetClientName(clients[x],name,sizeof(name));
                if(StrEqual(action,"set")||StrEqual(action,"="))
                    newamt=StringToInt(amount)-GetXP(clients[x]);
                else if(StrEqual(action,"add")||StrEqual(action,"+"))
                    newamt=StringToInt(amount);
                else if(StrEqual(action,"take")||StrEqual(action,"-"))
                    newamt=-(StringToInt(amount));
				new oldxp = GetXP(clients[x]);
                SetXP(clients[x],newamt,true);
                //TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set", adminname, newamt);
				TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin set XP",name,newamt);
				LogToFile(logFile, "Player %s set %s xp %i --> %i",adminname, name, oldxp, GetXP(clients[x]));
            }
        }
        if(count==0)
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin XP syntax");
    
    return Plugin_Handled;
}

public Action:AdminCmd_Level(client,argc)
{
	if(!CheckAdminAccess(client,sAdminFlags))
	{
		CReplyToCommand(client, "No Access", client);
		return Plugin_Handled;
	}
	
    if(argc>=3)
    {
        decl String:action[8],String:target[64];
        decl String:level[8],String:adminname[64],String:name[64];
        new newlevel, clients[MaxClients];
        GetCmdArg(1,action,sizeof(action));
        GetCmdArg(2,target,sizeof(target));
        GetCmdArg(3,level,sizeof(level));
        GetClientName(client,adminname,sizeof(adminname));
        new count=FindMatchingPlayers(target,clients);
        for(new x=0;x<count;x++)
        {
            if(IsValidPlayer(clients[x]))
            {
                GetClientName(clients[x],name,sizeof(name));
                if(StrEqual(action,"set")||StrEqual(action,"="))
                    newlevel=StringToInt(level)-GetLevel(clients[x]);
                else if(StrEqual(action,"add")||StrEqual(action,"+"))
                    newlevel=StringToInt(level);
                else if(StrEqual(action,"take")||StrEqual(action,"-"))
                    newlevel=-(StringToInt(level));
				new oldlevel = GetLevel(clients[x]);
                SetLevel(clients[x],newlevel,true);
               // TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",adminname,newlevel);
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin set level",name,newlevel);
				LogToFile(logFile, "Player %s set %s level %i --> %i", adminname, name, oldlevel, GetLevel(clients[x]));
            }
        }
        if(count==0)
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin level syntax");
    
    return Plugin_Handled;
}

public Action:AdminCmd_Credits(client,argc)
{
	if(!CheckAdminAccess(client,sAdminFlags))
	{
		CReplyToCommand(client, "No Access", client);
		return Plugin_Handled;
	}
	
    if(argc>=3)
    {
        decl String:action[8],String:target[64];
        decl String:amount[8],String:adminname[64],String:name[64];
        new newamt, clients[MaxClients];
        GetCmdArg(1,action,sizeof(action));
        GetCmdArg(2,target,sizeof(target));
        GetCmdArg(3,amount,sizeof(amount));
        GetClientName(client,adminname,sizeof(adminname));
        new count=FindMatchingPlayers(target,clients);
        for(new x=0;x<count;x++)
        {
            if(IsValidPlayer(clients[x]))
            {
                GetClientName(clients[x],name,sizeof(name));
                if(StrEqual(action,"set")||StrEqual(action,"="))
                    newamt=StringToInt(amount)-GetCredits(clients[x]);
                else if(StrEqual(action,"add")||StrEqual(action,"+"))
                    newamt=StringToInt(amount);
                else if(StrEqual(action,"take")||StrEqual(action,"-"))
                    newamt=-(StringToInt(amount));
				new oldcredits = GetCredits(clients[x]);
                SetCredits(clients[x],newamt,true);
                //TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",adminname,newamt);
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin set credits",name,newamt);
				LogToFile(logFile, "Player %s set %s credits %i --> %i", adminname, name, oldcredits, GetCredits(clients[x]));
            }
        }
        if(count==0)
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin credits syntax");
    
    return Plugin_Handled;
}


public Action:AdminCmd_ResetPlayer(client,argc)
{
	if(!CheckAdminAccess(client,sAdminFlags))
	{
		CReplyToCommand(client, "No Access", client);
		return Plugin_Handled;
	}
	
    if(argc>=1)
    {
        decl String:givenplayer[64], String:authstring[64], String:playername[64];
        GetCmdArg(1,givenplayer,sizeof(givenplayer));

        if(StrContains(givenplayer,"STEAM",false)>-1)
        {
            for(new i=1;i<=MaxClients;i++)
            {
                if(IsValidPlayer(i))
                {
                    GetClientAuthString(i,authstring,sizeof(authstring));
                    if(StrEqual(givenplayer,authstring,false))
                        ResetPlayer(i);
                }
            }
        }
        else
        {
            for(new i=1;i<=MaxClients;i++)
            {
                if(IsValidPlayer(i))
                {
                    GetClientName(i,playername,sizeof(playername));
                    if(StrEqual(givenplayer,playername,false))
                        ResetPlayer(i);
                }
            }
        }
    }

    return Plugin_Handled;
}

public Action:AdminCmd_ResetAllPlayers(client,argc)
{
	if(!CheckAdminAccess(client,sAdminFlags))
	{
		CReplyToCommand(client, "No Access", client);
		return Plugin_Handled;
	}
	
    for(new i=1;i<=MaxClients;i++)
        if(IsValidPlayer(i))
            ResetPlayer(i);

    return Plugin_Handled;
}

public Action:AdminCmd_DeletePlayer(client, args)
{
	if(!CheckAdminAccess(client,sAdminFlags))
	{
		CReplyToCommand(client, "No Access", client);
		return Plugin_Handled;
	}
	
	if(args < 1)
	{
		TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin delete player syntax");
		return Plugin_Handled;
	}
	
	decl String:buffer[32], String:buffer2[1024];
	GetCmdArgString(buffer, sizeof(buffer));
	for(new i = 1; i <= MaxClients; i++)
	{
		if(!IsValidPlayer(i))
			continue;
		
		if(!GetClientAuthString(i, buffer2, sizeof(buffer2)))	
			continue;
		
		if(StrEqual(buffer, buffer2, false))
		{
			ResetPlayer(i);
			return Plugin_Handled;
		}
	}
	
	Format(buffer2, sizeof(buffer2), "DELETE FROM thc_rpg WHERE ID = '%s'", buffer);
	if(SQL_FastQuery(g_hSQL, buffer2))
		TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Player successfully removed");
	else
	{
		SQL_GetError(g_hSQL, buffer2, sizeof(buffer2));
		TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Unable to remove a player, reason:", buffer2);
	}
	
	return Plugin_Handled;
}

public Action:AdminCmd_ResetDB(client,argc)
{
	if(!CheckAdminAccess(client,sAdminFlags))
	{
		CReplyToCommand(client, "No Access", client);
		return Plugin_Handled;
	}
	
    // delete table from db
    ManageSQLTable(2);
    // remove all vector data
    ClearPlayersVector();
    // create table in db
    ManageSQLTable(1);
    // init all connected players
    for(new i=1;i<=MaxClients;i++)
        if(IsValidPlayer(i))
        {
            // reset global upgrade variables
            ResetUpgradeVariables(i);
            SetHurtXP(i,0);
            InitPlayer(i);
            LoadPlayerData(i);
        }

    return Plugin_Handled;
}

AdminMenuFunc(client)
{
	new Handle:menu = CreateMenu(AdmMenuHandle);
	decl String:display[256];
	Format(display, sizeof(display), "%T", "admin_main_title",client);
	SetMenuTitle(menu, display);
	
	Format(display, sizeof(display), "%T", "admin_edit", client);
	AddMenuItem(menu, "adm_rpg", display);
		
	Format(display, sizeof(display), "%T", "admin_vip_item", client);
	AddMenuItem(menu, "adm_vip", display);	

	SetMenuExitBackButton(menu, true);
	DisplayMenu(menu, client, MENU_TIME_FOREVER);
}

public AdmMenuHandle(Handle:menu, MenuAction:action, client, param2)
{
	if(action==MenuAction_Select)
	{
		decl String:info[512];
		GetMenuItem(menu, param2, info, sizeof(info));
		if(StrEqual(info, "adm_rpg"))
		{
			ShowPlayerSelectionMenu(client);
		}
		else if(StrEqual(info, "adm_vip"))
		{
			 AdminVipMenu(client);
		}
	}
	else if(action==MenuAction_End)
		CloseHandle(menu);
}

/******************************************************************************************
*                           PLAYER SELECTION MENU HANDLERS                               *
******************************************************************************************/

ShowPlayerSelectionMenu(client)
{
	if(CheckAdminAccess(client,sAdminFlags))
	{	
		new Handle:menu = CreateMenu(Handle_PlayerSelectionMenu);
		SetMenuTitle(menu, "Выберите Игрока:");
		decl String:userid[15], String:name[64];
		for (new i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i))
			{
				IntToString(GetClientUserId(i), userid, 15);
				GetClientName(i, name, 64);
				if(StrContains(name, "GOTV", false) < 0)
				{
					AddMenuItem(menu, userid, name);
				}
			}
		}
		SetMenuExitBackButton(menu, true);
		DisplayMenu(menu, client, 0);
	}
}

public Handle_PlayerSelectionMenu(Handle:menu, MenuAction:action, client, option)
{
    switch(action)
    {
        case MenuAction_Select:
        {
			decl String:userid[15];
			GetMenuItem(menu, option, userid, 15);
			new u = StringToInt(userid);
			new target = GetClientOfUserId(u);
			ShowPlayerMenu(client, target);
        }
            
        case MenuAction_Cancel:
            if(option == MenuCancel_ExitBack)
                RedisplayAdminMenu(hAdminMenu, client);
        
        case MenuAction_End:
            CloseHandle(menu);
    }
}

/******************************************************************************************
*                                    RPG MENU HANDLERS                                   *
******************************************************************************************/

void:ShowPlayerMenu(client, target)
{
    decl String:item[128],String:starget[8];

    IntToString(target, starget, sizeof(starget));

    new Handle:hPlayerMenu = CreateMenu(Handle_PlayerMenu);

    SetMenuTitle(hPlayerMenu, "%T", "admin_choose_option_title", client);
    SetMenuExitButton(hPlayerMenu, true);
    SetMenuExitBackButton(hPlayerMenu, true);
    
    // Now add item to it
    Format(item, sizeof(item), "%T", "admin_xp_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_level_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_credits_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_reset_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    // Send the menu to our admin
    DisplayMenu(hPlayerMenu, client, 30);
}

public Handle_PlayerMenu(Handle:PlayerMenu, MenuAction:action, client, param)
{
    decl String:info[5];
    GetMenuItem(PlayerMenu, param, info, sizeof(info));
    new target=StringToInt(info);

    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
                    ShowXPMenu(client, target);
                case 1:
                    ShowLevelMenu(client, target);
                case 2:
                    ShowCreditsMenu(client, target);
                case 3:
                    ResetPlayer(target);
            }
        }
            
        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerSelectionMenu(client);
        
        case MenuAction_End:
            CloseHandle(PlayerMenu);
    }
}

/******************************************************************************************
*                                      XP MENU HANDLERS                                  *
******************************************************************************************/

ShowXPMenu(client, target)
{
    decl String:starget[8];

    IntToString(target, starget, sizeof(starget));

    new Handle:hXPMenu = CreateMenu(Handle_XPMenu);

    SetMenuTitle(hXPMenu, "%T", "admin_xp_title", client);
    SetMenuExitButton(hXPMenu, true);
    
    // Now add item to it
    AddMenuItem(hXPMenu, starget, "-100000 XP");
    AddMenuItem(hXPMenu, starget, "-50000 XP");
    AddMenuItem(hXPMenu, starget, "-10000 XP");
    AddMenuItem(hXPMenu, starget, "+10000 XP");
    AddMenuItem(hXPMenu, starget, "+50000 XP");
    AddMenuItem(hXPMenu, starget, "+100000 XP");

    // Send the menu to our admin
    DisplayMenu(hXPMenu, client, 30);
}

public Handle_XPMenu(Handle:XPMenu, MenuAction:action, client, param)
{
    decl String:info[5];
    GetMenuItem(XPMenu, param, info, sizeof(info));
    new target=StringToInt(info);
    new oldXP=GetXP(target);
	
    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
				{
					SetXP(target,-100000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set",client,GetXP(target));
					LogToFile(logFile, "Admin %N set player %N xp %i --> %i", client, target, oldXP, GetXP(target));
				}
                case 1:
				{
                   	SetXP(target,-50000,true);
                   	TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set",client,GetXP(target));
                   	LogToFile(logFile, "Admin %N set player %N xp %i --> %i", client, target, oldXP, GetXP(target));
				}
                case 2:
				{
                   	SetXP(target,-10000,true);
                   	TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set",client,GetXP(target));
                   	LogToFile(logFile, "Admin %N set player %N xp %i --> %i", client, target, oldXP, GetXP(target));
				}
                case 3:
				{
                   	SetXP(target,10000,true);
                   	TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set",client,GetXP(target));
                   	LogToFile(logFile, "Admin %N set player %N xp %i --> %i", client, target, oldXP, GetXP(target));
				}
                case 4:
				{
                   	SetXP(target,50000,true);
                   	TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set",client,GetXP(target));
                   	LogToFile(logFile, "Admin %N set player %N xp %i --> %i", client, target, oldXP, GetXP(target));
				}
                case 5:
				{
                   	SetXP(target,100000,true);
                   	TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set",client,GetXP(target));
                   	LogToFile(logFile, "Admin %N set player %N xp %i --> %i", client, target, oldXP, GetXP(target));
				}
            }
            
            // Now display the menu again
            ShowXPMenu(client, target);
        }
            
        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);

        case MenuAction_End:
            CloseHandle(XPMenu);
    }
}

/******************************************************************************************
*                                   LEVEL MENU HANDLERS                                  *
******************************************************************************************/

ShowLevelMenu(client, target)
{
	decl String:starget[12];

	IntToString(target, starget, sizeof(starget));

	new Handle:hLevelMenu = CreateMenu(Handle_LevelMenu);

	SetMenuTitle(hLevelMenu, "%T", "admin_level_title", client);
	SetMenuExitButton(hLevelMenu, true);

    // Now add items to it
	AddMenuItem(hLevelMenu, starget, "-50 Levels");
	AddMenuItem(hLevelMenu, starget, "-10 Levels");
	AddMenuItem(hLevelMenu, starget, "-5 Level");
	AddMenuItem(hLevelMenu, starget, "-1 Level");
	AddMenuItem(hLevelMenu, starget, "+1 Level");
	AddMenuItem(hLevelMenu, starget, "+5 Level");
	AddMenuItem(hLevelMenu, starget, "+10 Levels");
	AddMenuItem(hLevelMenu, starget, "+50 Levels");

	// Send the menu to our admin
	DisplayMenu(hLevelMenu, client, 30);
}

public Handle_LevelMenu(Handle:LevelMenu, MenuAction:action, client, param)
{
    decl String:info[5];
    GetMenuItem(LevelMenu, param, info, sizeof(info));
    new target=StringToInt(info);
	new oldlevel=GetLevel(target);
    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
				{
                    SetLevel(target,-50,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
                case 1:
				{
                    SetLevel(target,-10,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
                case 2:
				{
                    SetLevel(target,-5,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
				case 3:
				{
                    SetLevel(target,-1,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
				case 4:
				{
                    SetLevel(target,1,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
                case 5:
			    {
                    SetLevel(target,5,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
                case 6:
				{
                    SetLevel(target,10,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
                case 7:
				{
                    SetLevel(target,50,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetLevel(target));
					LogToFile(logFile, "Admin %N set player %N level %i --> %i", client, target, oldlevel, GetLevel(target));
				}
            }
            
            // Now display the menu again
            ShowLevelMenu(client, target);
        }
            
        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);
                
        case MenuAction_End:
            CloseHandle(LevelMenu);
    }
}

/******************************************************************************************
*                                 CREDITS MENU HANDLERS                                  *
******************************************************************************************/

ShowCreditsMenu(client, target)
{
	decl String:starget[8];

	IntToString(target, starget, sizeof(starget));

	new Handle:hCreditsMenu = CreateMenu(Handle_CreditsMenu);

	SetMenuTitle(hCreditsMenu, "%T", "admin_credits_title", client);
	SetMenuExitButton(hCreditsMenu, true);

    // Now add players to it
	AddMenuItem(hCreditsMenu, starget, "-500000 Credits");
	AddMenuItem(hCreditsMenu, starget, "-100000 Credits");
	AddMenuItem(hCreditsMenu, starget, "-50000 Credits");
	AddMenuItem(hCreditsMenu, starget, "-10000 Credits");
	AddMenuItem(hCreditsMenu, starget, "+10000 Credits");
	AddMenuItem(hCreditsMenu, starget, "+50000 Credits");
	AddMenuItem(hCreditsMenu, starget, "+100000 Credits");
	AddMenuItem(hCreditsMenu, starget, "+500000 Credits");
	// Send the menu to our admin
	DisplayMenu(hCreditsMenu, client, 30);
}

public Handle_CreditsMenu(Handle:CreditsMenu, MenuAction:action, client, param)
{
    decl String:info[12];
    GetMenuItem(CreditsMenu, param, info, sizeof(info));
    new target=StringToInt(info);
	new oldcredits=GetCredits(target);
    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
				{
                    SetCredits(target,-500000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
                case 1:
				{
                    SetCredits(target,-100000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
                case 2:
				{
                    SetCredits(target,-50000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
				case 3:
				{
                    SetCredits(target,-10000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
				case 4:
				{
                    SetCredits(target,10000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
                case 5:
			    {
                    SetCredits(target,50000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
                case 6:
				{
                    SetCredits(target,100000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
                case 7:
				{
                    SetCredits(target,500000,true);
					TransMgr_PrintText(target, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",client,GetCredits(target));
					LogToFile(logFile, "Admin %N set player %N credits %i --> %i", client, target, oldcredits, GetCredits(target));
				}
            }

            // Now display the menu again
            ShowCreditsMenu(client, target);
        }

        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);
                
        case MenuAction_End:
            CloseHandle(CreditsMenu);
    }
}