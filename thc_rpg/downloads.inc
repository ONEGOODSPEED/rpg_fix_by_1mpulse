LoadDownloads()
{
	if (g_CoreConfigCache[CoreConfig_downloads] == 1)
	{
		new sound = 0;
		decl String:file[255];
		BuildPath(Path_SM, file, 255, "configs/thc_rpg/downloads.txt");
		new Handle:fileh = OpenFile(file, "r");
		if (fileh != INVALID_HANDLE)
		{
			decl String:buffer[256];
			buffer[0] = '\0';
			while(ReadFileLine(fileh, buffer, sizeof(buffer)))
			{
				if (strlen(buffer) > 0)
				{
					TrimString(buffer);
					decl String:bufferx[256];
					Format(bufferx, sizeof(bufferx), "%s", buffer);
					if (StrContains(buffer, ".wav", false) != -1 || StrContains(buffer, ".mp3", false) != -1)
					{
						sound = 1;
						Format(bufferx, sizeof(bufferx), "sound/%s", buffer);
					}
					if (StrContains(buffer, "//") == -1 && !StrEqual(buffer, "") && FileExists(bufferx))
					{
						AddFileToDownloadsTable(bufferx);
						if (sound)
						{
							PrecacheSound(buffer);
							sound = 0;
						}
						else
						{
							PrecacheModel(buffer);
						}
					}
				}
			}
		}
	}
}
