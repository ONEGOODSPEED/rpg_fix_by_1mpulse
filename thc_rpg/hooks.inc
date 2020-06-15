/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          hooks.inc
 *  Type:          Core
 *  Description:   hooks any player commands needed for the project
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

HookCmds()
{
    RegConsoleCmd("say",SayCommand);
    RegConsoleCmd("say_team",SayCommand);
}

public Action:SayCommand(client,argc)
{
	if(IsValidPlayer(client))
	{
		decl String:text[192];
		GetCmdArgString(text, sizeof(text));
		BreakString(text, text, sizeof(text));
		
		decl String:sArgs[192], String:buffer[32][32];
		GetCmdArgString(sArgs, sizeof(sArgs));
		StripQuotes(sArgs);
		new count = ExplodeString(g_CoreConfigCache[CoreConfig_cmd], ",", buffer, 32, 32);
		for(new i = 0; i < count; ++i)
		{
			if(StrEqual(buffer[i], sArgs, false))
			{
				RPGMenu(client);
			}
		}
		if(StrContains(text, "rpgrank", false) != -1)
		{
			BreakString(text[8], text, MAX_NAME_LENGTH);
			new target = FindTarget(client, text, false, false);
			if(IsValidPlayer(target))
				GetPlayerRank(client, target);
			else
				GetPlayerRank(client);
		}
		else if(StrEqual(text, "rpgtop10", false) || StrEqual(text, "rpgtop", false))
			GetPlayerTop10(client);
		else if(StrEqual(text, "rpgstats", false))
			StatsMenu(client);
		else if(StrEqual(text, "frags", false)&& g_CoreConfigCache[CoreConfig_fragmenu]>0)
			FragMenu(client);
	}

	return Plugin_Continue;
}