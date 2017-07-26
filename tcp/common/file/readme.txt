定义文件列表
使用json来做


static int search_for_dir(char buf[], int len, int socket, char *pDirName)
{
	DIR *dir;
	struct dirent *ent;
	int off = 0;
	char timebuf[FTPBUFSIZE] = {0};
	struct stat st;
	struct passwd *pwd = NULL;
	struct group *grp = NULL;
	struct tm *ptm = NULL;
	int timelen = 0;
	char *filename = NULL;
	if (((dir = opendir(pDirName))) == NULL)
	{
		nslog(NS_ERROR, "opendir is NULL\n");
		return FALSE;
	}
	FTPSendReq(socket, g_ftpCmd_Req[0]);//"150 Begin transfer"
	buf[0] = '\0';
	while ((ent = readdir(dir)) != NULL)
	{
		filename = ent->d_name;
		char mode[] = "----------";

		memset(timebuf, 0, sizeof(timebuf));
		if (strcmp(filename, ".") == 0)
			continue;
		if (stat(filename, &st) < 0)
		{
			nslog(NS_ERROR, "stat:%s %s Dir:%s", strerror(errno), filename, pDirName);
			continue;
		}
		if (getpwuid(st.st_uid) == NULL || getgrgid(st.st_gid) == NULL)
		{
			continue;
		}
		if (S_ISDIR(st.st_mode))
			mode[0] = 'd';
		if (st.st_mode & S_IRUSR)
			mode[1] = 'r';
		if (st.st_mode & S_IWUSR)
			mode[2] = 'w';
		if (st.st_mode & S_IXUSR)
			mode[3] = 'x';
		if (st.st_mode & S_IRGRP)
			mode[4] = 'r';
		if (st.st_mode & S_IWGRP)
			mode[5] = 'w';
		if (st.st_mode & S_IXGRP)
			mode[6] = 'x';
		if (st.st_mode & S_IROTH)
			mode[7] = 'r';
		if (st.st_mode & S_IWOTH)
			mode[8] = 'w';
		if (st.st_mode & S_IXOTH)
			mode[9] = 'x';
		mode[10] = '\0';
		off += snprintf(buf + off, len - off, "%s ", mode);

		/* hard link number, this field is nonsense for ftp */
		off += snprintf(buf + off, len - off, "%d ", 1);

		/* user */
		if ((pwd = getpwuid(st.st_uid)) == NULL)
		{
			closedir(dir);
			return FALSE;
		}
		off += snprintf(buf + off, len - off, "%s ", pwd->pw_name);

		/* group */
		if ((grp = getgrgid(st.st_gid)) == NULL)
		{
			closedir(dir);
			return FALSE;
		}
		off += snprintf(buf + off, len - off, "%s ", grp->gr_name);

		/* size */
		off += snprintf(buf + off, len - off, "%*lld ", 10, (long long int)st.st_size);

		/* mtime */
		ptm = localtime(&st.st_mtime);
		if (ptm && (timelen = strftime(timebuf, sizeof(timebuf), "%b %d %H:%S", ptm)) > 0) {
			timebuf[timelen] = '\0';
			off += snprintf(buf + off, len - off, "%s ", timebuf);
		}
		else
		{
			closedir(dir);
			return FALSE;
		}

		off += snprintf(buf + off, len - off, "%s\r\n", filename);

	}
	if(dir)
	{
		closedir(dir);
	}
	return off;
}