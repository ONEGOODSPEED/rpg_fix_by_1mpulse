/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          rpgmenu.inc
 *  Type:          Core
 *  Description:   contains the RPGMenu for players
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

RPGMenu(client)
{
	decl String:item[MM_DATA_DESCRIPTION];

	new Handle:menu_main=CreateMenu(RPGMenuHandle);

	SetMenuTitle(menu_main, "%T", "main_title", client);

	Format(item, sizeof(item), "%T", "upgrade_item", client);
	AddMenuItem(menu_main,item,item,(MODULE_COUNT>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
	
	Format(item, sizeof(item), "%T", "sell_item", client);
	AddMenuItem(menu_main,item,item,(MODULE_COUNT>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);

	Format(item, sizeof(item), "%T", "stats_item", client);
	AddMenuItem(menu_main,item,item);
	
	Format(item, sizeof(item), "%T", "info_item", client);
	AddMenuItem(menu_main,item,item,(MODULE_COUNT>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
	
	Format(item, sizeof(item), "%T", "help_item", client);
	AddMenuItem(menu_main,item,item);
	
	Format(item, sizeof(item), "%T", "settings_item", client);
	AddMenuItem(menu_main,item,item);
	
	DisplayMenu(menu_main,client,MENU_TIME_FOREVER);
}

public RPGMenuHandle(Handle:menu_main,MenuAction:action,client,slot)
{
	if(action==MenuAction_Select)
	{
		switch(slot)
		{
			case 0:
				UpgradeMenu(client,0);
			case 1:
				SellMenu(client,0);
			case 2:
				StatsMenu(client);
			case 3:
				InfoMenu(client);
			case 4:
				HelpMenu(client);
			case 5:
				SettingsMenu(client);
		}
	}
}

SettingsMenu(client)
{
	decl String:display[256];
	new Handle:menu = CreateMenu(SettingsHandler);
	SetMenuTitle(menu, "%T", "settings_title", client);
	
	if(!DisableStats[client])
		Format(display, sizeof(display), "%T", "infromer_item_enable", client);
	else
		Format(display, sizeof(display), "%T", "infromer_item_disable", client);
	AddMenuItem(menu, "inform", display);
	
	if(DisableRANK[client])
		Format(display, sizeof(display), "%T", "rank_item_enable", client);
	else
		Format(display, sizeof(display), "%T", "rank_item_disable", client);
	AddMenuItem(menu, "rank", display);
	
	SetMenuExitBackButton(menu, true);
	SetMenuExitButton(menu, true);
	DisplayMenu(menu, client, MENU_TIME_FOREVER);
}

public SettingsHandler(Handle:menu, MenuAction:action, client, param2)
{
	if(action==MenuAction_Select)
	{
		decl String:info[16];
		GetMenuItem(menu, param2, info, sizeof(info));
		if(StrEqual(info, "inform"))
		{
			if(!DisableStats[client])
			{
			DisableStats[client] = true;
			TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Inform disable");
			}
			else
			{
			DisableStats[client] = false;
			TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Inform enable");
			}
		}
		else if(StrEqual(info, "rank"))
		{
			if(DisableRANK[client])
			{
				DisableRANK[client] = true;
				TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "RPGRank enable");
			}
			else
			{
				DisableRANK[client] = false;
				TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "RPGRank disable");
			}
		}
		SettingsMenu(client);
	}
	else if(action == MenuAction_Cancel)
	{
		if(param2 == MenuCancel_ExitBack)
			RPGMenu(client);
	}
	else if(action == MenuAction_End)
		CloseHandle(menu);
}

UpgradeMenu(client,firstitem)
{
    decl String:upgradeid[4],String:item[MM_DATA_DESCRIPTION];
    decl String:upgradename[MM_DATA_FULLNAME];
    new TeamLock, level;
    new Module:upgrade;
    
    new Handle:menu_upgrades=CreateMenu(UpgradeHandle);
    SetMenuExitBackButton(menu_upgrades,true);

    SetMenuTitle(menu_upgrades, "%T", "upgrades_title", client, GetCredits(client));

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        // Read moduleindex as a Module type.
        upgrade = Module:moduleindex;
        IntToString(moduleindex,upgradeid,sizeof(upgradeid));

        GetUpgradeTranslatedFullName(client,upgrade,upgradename,sizeof(upgradename));

        TeamLock=GetUpgradeConfigTeamLock(upgrade);
        level = GetPlayerUpgradeLevel(client,upgrade);

        if(ModuleMgr_IsDisabled(upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_disabled", client, upgradename);
            // Dont show the menu entry!!
            // AddMenuItem(menu_upgrades,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(GetClientTeam(client)==TeamLock || IsUpgradeInterlocked(client, upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_locked", client, upgradename);
            AddMenuItem(menu_upgrades,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(IsUpgradeMaxed(client,upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_maxed", client, upgradename);
            AddMenuItem(menu_upgrades,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else
        {
            Format(item,sizeof(item), "%T", "upgrade_purchase",client,upgradename,level+1,GetUpgradeConfigMaxLevel(upgrade),GetUpgradeConfigCost(upgrade)+(level*GetUpgradeConfigiCost(upgrade)));
            AddMenuItem(menu_upgrades,upgradeid,item);
        }
    }
    if(MODULE_COUNT>0)
        DisplayMenuAtItem(menu_upgrades,client,firstitem,MENU_TIME_FOREVER);
    else
        RPGMenu(client);
}

public UpgradeHandle(Handle:menu_upgrades,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
        decl String:upgradeid[4];
        GetMenuItem(menu_upgrades,slot,upgradeid,sizeof(upgradeid));
        new Module:upgrade = Module:StringToInt(upgradeid);
        PurchaseUpgrade(client,upgrade);
        UpgradeMenu(client,GetMenuSelectionPosition());
    }
    if(action==MenuAction_Cancel)
        if(slot==MenuCancel_ExitBack)
            RPGMenu(client);
    if(action==MenuAction_End)
        CloseHandle(menu_upgrades);
}

SellMenu(client,firstitem)
{
    decl String:upgradeid[4],String:item[MM_DATA_DESCRIPTION];
    decl String:upgradename[MM_DATA_FULLNAME];
    new TeamLock, level;
    new Module:upgrade;
    
    new Handle:menu_sell=CreateMenu(SellHandle);
    SetMenuExitBackButton(menu_sell,true);

    SetMenuTitle(menu_sell, "%T", "sell_title", client, GetCredits(client));

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        // Read moduleindex as a Module type.
        upgrade = Module:moduleindex;
        IntToString(moduleindex,upgradeid,sizeof(upgradeid));

        GetUpgradeTranslatedFullName(client,upgrade,upgradename,sizeof(upgradename));

        TeamLock=GetUpgradeConfigTeamLock(upgrade);
        level = GetPlayerUpgradeLevel(client,upgrade);

        if(ModuleMgr_IsDisabled(upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_disabled", client, upgradename);
            // Dont show the menu entry!!
            // AddMenuItem(menu_sell,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(GetClientTeam(client)==TeamLock)
        {
            Format(item,sizeof(item), "%T", "upgrade_locked", client, upgradename);
            AddMenuItem(menu_sell,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(level>0)
        {
            new cost=GetUpgradeConfigCost(upgrade)+((level-1)*GetUpgradeConfigiCost(upgrade));
            new refund=RoundToNearest(cost*g_CoreConfigCache[CoreConfig_save_percent]);
            Format(item,sizeof(item), "%T", "upgrade_sell",client,upgradename,GetPlayerUpgradeLevel(client,upgrade),GetUpgradeConfigMaxLevel(upgrade),refund);
            AddMenuItem(menu_sell,upgradeid,item);
        }
    }
    if(GetPlayerUpgradeCount(client)>0)
        DisplayMenuAtItem(menu_sell,client,firstitem,MENU_TIME_FOREVER);
    else
        RPGMenu(client);
}

public SellHandle(Handle:menu_sell,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
        decl String:upgradeid[4];
        GetMenuItem(menu_sell,slot,upgradeid,sizeof(upgradeid));
        new Module:upgrade = Module:StringToInt(upgradeid);
        SellUpgrade(client,upgrade);
        SellMenu(client,GetMenuSelectionPosition());
    }
    if(action==MenuAction_Cancel)
        if(slot==MenuCancel_ExitBack)
            RPGMenu(client);
    if(action==MenuAction_End)
        CloseHandle(menu_sell);
}


StatsMenu(client)
{
	decl String:item[MM_DATA_DESCRIPTION];

	new Handle:menu_stats=CreateMenu(StatsHandle);
	SetMenuExitBackButton(menu_stats,true);

	SetMenuTitle(menu_stats, "%T", "stats_title", client);

	Format(item, sizeof(item), "%T", "stats_reset_item", client);
	AddMenuItem(menu_stats,item,item,(g_CoreConfigCache[CoreConfig_resetstatmenu]>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
	
	Format(item, sizeof(item), "%T", "rest_item", client);
	AddMenuItem(menu_stats,item,item,(g_CoreConfigCache[CoreConfig_restmenu]>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
	
	Format(item, sizeof(item), "%T", "player_info_item", client);
	AddMenuItem(menu_stats,item,item);
	
	Format(item, sizeof(item), "%T", "fragmenu", client);
	AddMenuItem(menu_stats,item,item,(g_CoreConfigCache[CoreConfig_fragmenu]>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
	
	Format(item, sizeof(item), "%T", "stats_level_item", client, GetLevel(client));
	AddMenuItem(menu_stats,item,item,ITEMDRAW_DISABLED);
	
	Format(item, sizeof(item), "%T", "stats_xp_item", client, GetXP(client), GetReqXP(client));
	AddMenuItem(menu_stats,item,item,ITEMDRAW_DISABLED);
	
	Format(item, sizeof(item), "%T", "stats_credits_item", client, GetCredits(client));
	AddMenuItem(menu_stats,item,item,ITEMDRAW_DISABLED);
	
	DisplayMenu(menu_stats,client,MENU_TIME_FOREVER);
}

public StatsHandle(Handle:menu_stats,MenuAction:action,client,slot)
{
	if(action==MenuAction_Select)
	{
		if(slot==0)
			ResetStatsMenu(client);
		else if(slot == 1)
			RedistributionMenu(client);
		else if(slot == 2)
			PlayerInfoChooseMenu(client);
		else if(slot == 3)
			FragMenu(client);
	}
	else if(action==MenuAction_Cancel)
	{
		if(slot==MenuCancel_ExitBack)
			RPGMenu(client);
	}
	else if(action==MenuAction_End)
		CloseHandle(menu_stats);
}

FragMenu(client)
{
	if(IsValidPlayer(client))
	{
		new Handle:menu = CreateMenu(HandlerMenuFrag);
		new value5 = g_CoreConfigCache[CoreConfig_credits_frag];
		decl String:display[128];
		SetMenuTitle(menu, "%T", "course", client,value5);
		Format(display, sizeof(display), "%T", "1frag", client);
		AddMenuItem(menu, "frag1", display);
		
		Format(display, sizeof(display), "%T", "5frags", client);
		AddMenuItem(menu, "frag5", display);
		
		Format(display, sizeof(display), "%T", "10frags", client);
		AddMenuItem(menu, "frag10", display);
		
		Format(display, sizeof(display), "%T", "15frags", client);
		AddMenuItem(menu, "frag15", display);
		
		Format(display, sizeof(display), "%T", "20frags", client);
		AddMenuItem(menu, "frag20", display);
		
		Format(display, sizeof(display), "%T", "50frags", client);
		AddMenuItem(menu, "frag50", display);
		
		Format(display, sizeof(display), "%T", "100frags", client);
		AddMenuItem(menu, "frag100", display);
		
		SetMenuExitBackButton(menu, true);
		SetMenuExitButton(menu, true);
		
		DisplayMenu(menu, client, MENU_TIME_FOREVER);
	}
	else
		StatsMenu(client);
}

public HandlerMenuFrag(Handle:menu, MenuAction:action, client, param2)
{
	if(action==MenuAction_Select)
	{
		decl String:info[32];
		GetMenuItem(menu, param2, info, sizeof(info));
		if(IsValidPlayer(client))
		{
			new amountfrags;
			new getfrags = GetClientFrags(client);
			new scrd;
			if(StrEqual(info, "frag1"))
			{
				amountfrags = 1;
				scrd = g_CoreConfigCache[CoreConfig_credits_frag] * amountfrags;
				if(getfrags >= amountfrags)
				{
					SetEntProp(client, Prop_Data, "m_iFrags", getfrags -= amountfrags);
					SetCredits(client,scrd,false);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Frags changed is successfully");
					RPGMenu(client);
				}
				else
				{
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You do not have enough frags");
					RPGMenu(client);
				}
			}
			else if(StrEqual(info, "frag5"))
			{
				amountfrags = 5;
				scrd = g_CoreConfigCache[CoreConfig_credits_frag] * amountfrags;
				if(getfrags >= amountfrags)
				{
					SetEntProp(client, Prop_Data, "m_iFrags", getfrags -= amountfrags);
					SetCredits(client,scrd,false);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Frags changed is successfully");
					RPGMenu(client);
				}
				else
				{
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You do not have enough frags");
					RPGMenu(client);
				}
			}
			else if(StrEqual(info, "frag10"))
			{
				amountfrags = 10;
				scrd = g_CoreConfigCache[CoreConfig_credits_frag] * amountfrags;
				if(getfrags >= amountfrags)
				{
					SetEntProp(client, Prop_Data, "m_iFrags", getfrags -= amountfrags);
					SetCredits(client,scrd,false);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Frags changed is successfully");
					RPGMenu(client);
				}
				else
				{
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You do not have enough frags");
					RPGMenu(client);
				}
			}
			else if(StrEqual(info, "frag15"))
			{
				amountfrags = 15;
				scrd = g_CoreConfigCache[CoreConfig_credits_frag] * amountfrags;
				if(getfrags >= amountfrags)
				{
					SetEntProp(client, Prop_Data, "m_iFrags", getfrags -= amountfrags);
					SetCredits(client,scrd,false);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Frags changed is successfully");
					RPGMenu(client);
				}
				else
				{
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You do not have enough frags");
					RPGMenu(client);
				}
			}
			else if(StrEqual(info, "frag20"))
			{
				amountfrags = 20;
				scrd = g_CoreConfigCache[CoreConfig_credits_frag] * amountfrags;
				if(getfrags >= amountfrags)
				{
					SetEntProp(client, Prop_Data, "m_iFrags", getfrags -= amountfrags);
					SetCredits(client,scrd,false);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Frags changed is successfully");
					RPGMenu(client);
				}
				else
				{
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You do not have enough frags");
					RPGMenu(client);
				}
			}
			else if(StrEqual(info, "frag50"))
			{
				amountfrags = 50;
				scrd = g_CoreConfigCache[CoreConfig_credits_frag] * amountfrags;
				if(getfrags >= amountfrags)
				{
					SetEntProp(client, Prop_Data, "m_iFrags", getfrags -= amountfrags);
					SetCredits(client,scrd,false);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Frags changed is successfully");
					RPGMenu(client);
				}
				else
				{
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You do not have enough frags");
					RPGMenu(client);
				}
			}
			if(StrEqual(info, "frag100"))
			{
				amountfrags = 100;
				scrd = g_CoreConfigCache[CoreConfig_credits_frag] * amountfrags;
				if(getfrags >= amountfrags)
				{
					SetEntProp(client, Prop_Data, "m_iFrags", getfrags -= amountfrags);
					SetCredits(client,scrd,false);
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Frags changed is successfully");
					RPGMenu(client);
				}
				else
				{
					TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "You do not have enough frags");
					RPGMenu(client);
				}
			}
		}
		else
			StatsMenu(client);
	}
	else if(action == MenuAction_Cancel)
	{
		if(param2 == MenuCancel_ExitBack)
			RPGMenu(client);
	}
	else if(action==MenuAction_End)
		CloseHandle(menu);
}

PlayerInfoChooseMenu(client)
{
	new Handle:menu = CreateMenu(PlayerInfoChooseHandle);
	SetMenuTitle(menu, "%T", "Choose a player:", client);

	AddTargetsToMenu(menu, 0);	// userid
	
	SetMenuExitBackButton(menu, true);
	SetMenuExitButton(menu, true);
	
	DisplayMenu(menu, client, MENU_TIME_FOREVER);
}

public PlayerInfoChooseHandle(Handle:menu, MenuAction:action, client, param2)
{
	if(action == MenuAction_Select)
	{
		decl String:info[8];
		GetMenuItem(menu, param2, info, sizeof(info));
		new target = GetClientOfUserId(StringToInt(info));
		if(IsValidPlayer(target))
			PlayerInfo(client, target);
		else
			PlayerInfoChooseMenu(client);
	}
	else if(action==MenuAction_Cancel)
	{
		if(param2 == MenuCancel_ExitBack)
			StatsMenu(client);
	}
	else if(action == MenuAction_End)
		CloseHandle(menu);
}

PlayerInfo(client, target)
{
	decl String:display[512];
	new level = GetLevel(target);
	new credits = GetCredits(target);
	GetClientName(target, display, sizeof(display));
	Format(display, sizeof(display), "%T", "Panel: player info", client, display, level, credits, GetXP(target), GetReqXP(target));
	CreateHelpPanel(client, display, MENU_TIME_FOREVER);
}

RedistributionMenu(client)
{
	decl String:display[256];

	new Handle:menu = CreateMenu(RestoreHandle);
	SetMenuTitle(menu, "%T", "stats_restore_title", client, GetLevel(client), RoundToNearest(float(GetCredits(client))/100.0)*g_CoreConfigCache[CoreConfig_restore_percent]);

	Format(display, sizeof(display), "%T", "stats_reset_yes", client);
	AddMenuItem(menu, "yes", display);
	
	Format(display, sizeof(display), "%T", "stats_reset_no", client);
	AddMenuItem(menu, "no", display);
	
	SetMenuExitBackButton(menu, true);
	SetMenuExitButton(menu, true);
	
	DisplayMenu(menu, client, MENU_TIME_FOREVER);
}

new bool:RestoreMassive[MAXPLAYERS+1] = false;
public RestoreHandle(Handle:menu, MenuAction:action, client, param2)
{
	if(action == MenuAction_Select)
	{
		decl String:info[8];
		GetMenuItem(menu, param2, info, sizeof(info));
		if(StrEqual(info, "yes"))
		{
			new level = GetLevel(client);
			ResetPlayer(client);
			new Handle:datapack = CreateDataPack();
			WritePackCell(datapack, client);
			WritePackCell(datapack, level);
			RestrubTimer[client] = CreateTimer(2.0, Timer_Restore, datapack);
			RPGMenu(client);
		}
		else if(StrEqual(info, "no"))
			StatsMenu(client);
	}
	else if(action==MenuAction_Cancel)
	{
		if(param2 == MenuCancel_ExitBack)
			StatsMenu(client);
	}
	else if(action == MenuAction_End)
		CloseHandle(menu);
}

public Action:Timer_Restore(Handle:timer, Handle:datapack)
{
	ResetPack(datapack);
	new client = ReadPackCell(datapack);
	RestrubTimer[client] = INVALID_HANDLE;
	new level = ReadPackCell(datapack)-1;
	SetLevel(client, level, false);
	new credits = GetCredits(client);
	new Float:percent = g_CoreConfigCache[CoreConfig_restore_percent]/100.0;
	new amount = RoundToNearest(credits-(credits*percent));
	SetStaticCredits(client,amount );
}

ResetStatsMenu(client)
{
    decl String:item[MM_DATA_DESCRIPTION];

    new Handle:menu_resetstats=CreateMenu(ResetStatsHandle);

    SetMenuTitle(menu_resetstats, "%T", "stats_reset_warning", client);

    Format(item, sizeof(item), "%T", "stats_reset_yes", client);
    AddMenuItem(menu_resetstats,item,item);

    Format(item, sizeof(item), "%T", "stats_reset_no", client);
    AddMenuItem(menu_resetstats,item,item);

    DisplayMenu(menu_resetstats,client,MENU_TIME_FOREVER);
}

public ResetStatsHandle(Handle:menu_resetstats,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
        if(slot==0)
		{
			new infoLVLresetPRE = GetLevel(client);
            ResetPlayer(client);
			LogToFile(logFile, "Игрок %N обнулил свой уровень(был %i)", client, infoLVLresetPRE);
			StatsMenu(client);
		}
    }
    if(action==MenuAction_End)
        CloseHandle(menu_resetstats);
}

InfoMenu(client)
{
    decl String:upgradeid[4],String:item[MM_DATA_FULLNAME+MM_DATA_DESCRIPTION];
    decl String:upgradename[MM_DATA_FULLNAME],String:upgradedescription[MM_DATA_DESCRIPTION];
    new Module:upgrade;
    
    new Handle:menu_info=CreateMenu(InfoHandle);
    SetMenuExitBackButton(menu_info,true);

    SetMenuTitle(menu_info, "%T", "info_title", client);

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        // Read moduleindex as a Module type.
        upgrade = Module:moduleindex;
        IntToString(moduleindex,upgradeid,sizeof(upgradeid));

        GetUpgradeTranslatedFullName(client,upgrade,upgradename,sizeof(upgradename));
        GetUpgradeTranslatedDescription(client,upgrade,upgradedescription,sizeof(upgradedescription));

        new TeamLock=GetUpgradeConfigTeamLock(upgrade);

        if(!ModuleMgr_IsDisabled(upgrade)&&GetClientTeam(client)!=TeamLock)
        {
            Format(item,sizeof(item),"%s",upgradename,upgradedescription);
            AddMenuItem(menu_info,upgradeid,item);
        }
    }
    if(MODULE_COUNT>0)
        DisplayMenu(menu_info,client,MENU_TIME_FOREVER);
    else
        CloseHandle(menu_info);
}

public InfoHandle(Handle:menu_info,MenuAction:action,client,slot)
{
	if(action==MenuAction_Select)
	{
		new String:item[MM_DATA_DESCRIPTION];
		
		new String:upgradeid[4];
		GetMenuItem(menu_info, slot, upgradeid, 4);
		new Module:upgrade = Module:StringToInt(upgradeid);
		GetUpgradeTranslatedDescription(client,upgrade,item,MM_DATA_DESCRIPTION);
		TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, true, item);
		InfoMenu(client);
	}
	else if(action==MenuAction_Cancel)
        if(slot==MenuCancel_ExitBack)
            RPGMenu(client);
	else if(action==MenuAction_End)
        CloseHandle(menu_info);
}

HelpMenu(client)
{
	new Handle:menu = CreateMenu(HelpMenuHandle);
	SetMenuTitle(menu, "%T", "help_title", client);
	decl String:display[256];
	
	Format(display, sizeof(display), "%T", "What is rpg?", client);
	AddMenuItem(menu, "rpg", display);
	
	Format(display, sizeof(display), "%T", "Contact information", client);
	AddMenuItem(menu, "contacts", display);	

	SetMenuExitBackButton(menu, true);
	DisplayMenu(menu, client, MENU_TIME_FOREVER);
}

public HelpMenuHandle(Handle:menu, MenuAction:action, client, param2)
{
	if(action==MenuAction_Select)
	{
		decl String:info[512];
		GetMenuItem(menu, param2, info, sizeof(info));
		if(StrEqual(info, "rpg"))
		{
			Format(info, sizeof(info), "%T", "Panel: What is RPG?", client);
			CreateHelpPanel(client, info, MENU_TIME_FOREVER);
		}
		else if(StrEqual(info, "contacts"))
		{
			Format(info, sizeof(info), "%T", "Panel: contacts", client);
			CreateHelpPanel(client, info, MENU_TIME_FOREVER);
		}
	}
	else if(action == MenuAction_Cancel)
	{
		if(param2 == MenuCancel_ExitBack)
			RPGMenu(client);
	}
	else if(action==MenuAction_End)
		CloseHandle(menu);
}

stock CreateHelpPanel(client, const String:text[], time)
{
	new Handle:panel = CreatePanel();
	DrawPanelItem(panel, text, ITEMDRAW_RAWLINE);
	SendPanelToClient(panel, client, HandlerHelpPanel, time);
	CloseHandle(panel);
}

public HandlerHelpPanel(Handle:menu, MenuAction:action, client, param2)
{
	if(action == MenuAction_End)
		CloseHandle(menu);
}