/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          impulse2.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to move faster when shooting
 *  Author:        ArsiRC
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

/**
 * This module's identifier.
 */
new Module:g_moduleimpulse2;

new g_impulse2UpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
impulse2_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "impulse2");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "impulse2");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to move faster when shooting");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;
    
    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 6;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_moduleimpulse2 = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleimpulse2, "Event_OnEventsRegister", "impulse2_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleimpulse2, "impulse2_OnConfigReload", "configs/thc_rpg/impulse2.txt");
    #endif
}

/**
 * Register all events here.
 */
public impulse2_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleimpulse2, "Event_OnMapStart",                   "impulse2_OnMapStart");
	#if defined PROJECT_GAME_CSS
		EventMgr_RegisterEvent(g_moduleimpulse2, "Event_PlayerHurt",              "impulse2_PlayerHurt");
	#endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public impulse2_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public impulse2_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleimpulse2, LogType_Normal, "impulse2ConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleimpulse2, CM_CONFIGINDEX_FIRST, "impulse2Module_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:impulse2Module_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "impulse2", false))
    {
        g_impulse2UpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_impulse2UpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_impulse2UpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_impulse2UpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_impulse2UpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_impulse2UpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_impulse2UpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");

        if(g_impulse2UpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleimpulse2);

        ModuleMgr_WriteCell(g_moduleimpulse2, ModuleData_TeamLock, g_impulse2UpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleimpulse2, ModuleData_MaxLevel, g_impulse2UpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleimpulse2, ModuleData_Cost, g_impulse2UpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleimpulse2, ModuleData_iCost, g_impulse2UpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleimpulse2);
}
#endif

/**
 * The map has started.
 */
public impulse2_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleimpulse2, CM_CONFIGINDEX_FIRST, "impulse2Module_ConfigCache");
    #endif
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public impulse2_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
	if(AreValidPlayers(victim, attacker))
	{
		new level = GetPlayerUpgradeLevel(victim, g_moduleimpulse2);
		if(level > 0 && GetRandomFloat(0.0, 1.0) <= 0.03*level)
		{
			new max_ammo = 0;
			new Weapon = GetEntPropEnt(victim, Prop_Data, "m_hActiveWeapon");
			decl String:weapon_name[64];
			GetEntPropString(Weapon, Prop_Data, "m_iClassname", weapon_name, sizeof(weapon_name));
			if(StrEqual(weapon_name, "weapon_p90"))
				max_ammo=50;
			if(StrEqual(weapon_name, "weapon_deagle")||StrEqual(weapon_name, "weapon_xm1014")||StrEqual(weapon_name, "weapon_sawedoff"))         
				max_ammo=7;                    
			if(StrEqual(weapon_name, "weapon_ak47")||StrEqual(weapon_name, "weapon_m4a1")||StrEqual(weapon_name, "weapon_sg552")||StrEqual(weapon_name, "weapon_sg556")||StrEqual(weapon_name, "weapon_aug")||StrEqual(weapon_name, "weapon_sg550")||StrEqual(weapon_name, "weapon_elite")||StrEqual(weapon_name, "weapon_mac10")||StrEqual(weapon_name, "weapon_tmp")||StrEqual(weapon_name, "weapon_mp5navy")||StrEqual(weapon_name, "weapon_mp7")||StrEqual(weapon_name, "weapon_mp9"))      
				max_ammo=30;
			if(StrEqual(weapon_name, "weapon_galil")||StrEqual(weapon_name, "weapon_galilar"))
				max_ammo=35;
			if(StrEqual(weapon_name, "weapon_scout")||StrEqual(weapon_name, "weapon_awp")||StrEqual(weapon_name, "weapon_ssg08"))
				max_ammo=10;
			if(StrEqual(weapon_name, "weapon_g3sg1")||StrEqual(weapon_name, "weapon_glock")||StrEqual(weapon_name, "weapon_fiveseven")||StrEqual(weapon_name, "weapon_scar20")||StrEqual(weapon_name, "weapon_m4a1_silencer"))
				max_ammo=20;
			if(StrEqual(weapon_name, "weapon_famas")||StrEqual(weapon_name, "weapon_ump45"))
				max_ammo=25;
			if(StrEqual(weapon_name, "weapon_usp")||StrEqual(weapon_name, "usp_silencer")||StrEqual(weapon_name, "weapon_usp")||StrEqual(weapon_name, "weapon_cz75a"))
				max_ammo=12; 
			if(StrEqual(weapon_name, "weapon_p228")||StrEqual(weapon_name, "weapon_hkp2000")||StrEqual(weapon_name, "weapon_p250"))
				max_ammo=13;
			if(StrEqual(weapon_name, "weapon_m3")||StrEqual(weapon_name, "weapon_nova"))
				max_ammo=8;
			if(StrEqual(weapon_name, "weapon_m249"))
				max_ammo=100;
			if(StrEqual(weapon_name, "weapon_negev"))
				max_ammo=150;
			if(StrEqual(weapon_name, "weapon_mag7"))
				max_ammo=5;
			if(StrEqual(weapon_name, "weapon_tec9"))
				max_ammo=32;
			if(StrEqual(weapon_name, "weapon_bizon"))
				max_ammo=64;
			if(IsValidEdict(Weapon))
				SetEntData(Weapon, g_iWeaponClipOffset, max_ammo, 4, true);
		}
	}
}