#include "blk.h"
#define DIRECTORYPATH  "/home/pi/kimdohwan/blackBox/"
#define LIMITMEMORY 0.3
#define RECORDTIME 60000
#define VIDEOCOUNT 120

const char *MMOUNT = "/proc/mounts";

MOUNTP *dfopen();
MOUNTP *dfget(MOUNTP *MP);
int dfclose(MOUNTP *MP);
int rmdirs(const char *path, int is_error_stop);
void scanNremove();
int main()
{
	MOUNTP *MP;
	//1. microSD필수디바이스확인
	if ((MP=dfopen()) == NULL)
	{
		perror("error! 메모리카드를확인해주세요!\n");
		return 1;
    }
	dfget(MP);
	dfclose(MP);
	
	//2. make directory
	time_t UTCtime;
	struct tm *tm;
	char buf[BUFSIZ];
	char dir_name[BUFSIZ];
	
	char command[BUFSIZ];
	int system_chk;
	
	int video_cnt=1;
	//video_cnt<=VIDEOCOUNT
	while(1)
	{
		time(&UTCtime);
		tm=localtime(&UTCtime);
		
		sprintf(dir_name,"%s",DIRECTORYPATH);
		strftime(buf,sizeof(buf),"%Y%m%d_%H",tm); //사용자정의문자열지정
		strcat(dir_name,buf);
		printf("dir_name : %s\n",dir_name);
		
		
		//make directory name
		if(mkdir(dir_name, 0776)  == -1 && errno != EEXIST)
		{
			printf("error! make directory\n");
			return -1;
		}
		printf("make directory!\n");
		
		//buf memset
		memset(buf, '\0', sizeof(buf));

		//3. 메모리여유공간체크 모자르면  remove directory
		while(1)
		{
			if ((MP=dfopen()) == NULL)
			{
				perror("error! MP open\n");
				return 1;
			}
			dfget(MP);
			if(MP->size.avail <= MP->size.blocks*LIMITMEMORY)
			{
					printf("Not enough memory!\n");
					scanNremove();
					dfclose(MP);
			}
			else
			{
				printf("memory -> avail : %lu	blocks :%lu\n", MP->size.avail, MP->size.blocks);
				dfclose(MP);
				break;
			}
		}
		
		
		//make videofile name
		time(&UTCtime);
		tm=localtime(&UTCtime);
		strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", tm); //사용자정의문자열지정
		//system("raspivid -o /home/pi/kimdohwan/blackBox/20180531_11/20180531_112810.h264 -t 60000");
		printf("====================================\n");
		sprintf(command, "raspivid -o %s/%s.h264 -w 640 -h 480 -t %d", dir_name, buf, RECORDTIME);
		printf("command : %s\n", command);
		//sleep(3);
		
		
		system_chk=system(command);
		if(system_chk != 0)
		{
			printf("failed recording!\n");
			return 1;
		}
		
		printf("%s.h264 촬영완료\n", buf);
		//버프초기화
		memset(command, '\0', sizeof(command));
		memset(buf, '\0', sizeof(buf));
		memset(dir_name, '\0', sizeof(dir_name));
		
		printf("video count : %d\n-----------------------------------------\n", video_cnt);		
		video_cnt++;
	}
	
     return 0;
}

void scanNremove(void)
{
	char* src = DIRECTORYPATH;
	char rmdir_name[BUFSIZ];
	struct dirent **namelist;
	int count;
	int idx;
	
	if((count = scandir(src, &namelist, NULL, alphasort)) == -1)
	{
			fprintf(stderr, "%s Directory Scan Error: %s\n", src, strerror(errno));
			exit(EXIT_FAILURE);
	}

	for(idx = 0; idx < count; idx++) 
	{
			//"." , ".."는삭제대상에서제외
			if(strcmp(namelist[idx]->d_name, ".") != 0 && strcmp(namelist[idx]->d_name, "..")!=0) 
			{
					strcpy(rmdir_name, src);
					strcat(rmdir_name, namelist[idx]->d_name);
					printf("rmdir_name : %s\n", rmdir_name);
					printf("%s\n", namelist[idx]->d_name);
					//디렉토리하위의모든파일및디렉토리를삭제
					rmdirs(rmdir_name, 1);
					printf("remove first directory\n");
					break;
				}
		}

		// 건별 데이터 메모리 해제
		for(idx = 0; idx < count; idx++)
		{
			free(namelist[idx]);
		}

		// namelist에 대한 메모리 해제
		free(namelist);
}

MOUNTP *dfopen()
{
    MOUNTP *MP;

    // /proc/mounts 파일을 연다.
    MP = (MOUNTP *)malloc(sizeof(MOUNTP));
    if(!(MP->fp = fopen(MMOUNT, "r")))
    {
        return NULL;
    }
    else
        return MP;
}

MOUNTP *dfget(MOUNTP *MP)
{
    char buf[256];
    char *bname;
    char null[16];
    struct statfs lstatfs;
    struct stat lstat; 
    int is_root = 0;
	
    // /proc/mounts로 부터 마운트된 파티션의 정보를 얻어온다.
    while(fgets(buf, 255, MP->fp))
    {
        is_root = 0;
        sscanf(buf, "%s%s%s",MP->devname, MP->mountdir, MP->fstype);
         if (strcmp(MP->mountdir,"/") == 0) is_root=1;
        if (stat(MP->devname, &lstat) == 0 || is_root)
        {
            if (strstr(buf, MP->mountdir) && S_ISBLK(lstat.st_mode) || is_root)
            {
                // 파일시스템의 총 할당된 크기와 사용량을 구한다.        
                statfs(MP->mountdir, &lstatfs);
                MP->size.blocks = lstatfs.f_blocks * (lstatfs.f_bsize/1024); 
                MP->size.avail  = lstatfs.f_bavail * (lstatfs.f_bsize/1024); 
                return MP;
            }
        }
    }
    rewind(MP->fp);
    return NULL;
}

int dfclose(MOUNTP *MP)
{
    fclose(MP->fp);
}

int rmdirs(const char *path, int is_error_stop)
{
    DIR *  dir_ptr      = NULL;
    struct dirent *file = NULL;
    struct stat   buf;
    char   filename[1024];

    /* 목록을 읽을 디렉토리명으로 DIR *를 return 받습니다. */
    if((dir_ptr = opendir(path)) == NULL) {
		/* path가 디렉토리가 아니라면 삭제하고 종료합니다. */
		return unlink(path);
    }

    /* 디렉토리의 처음부터 파일 또는 디렉토리명을 순서대로 한개씩 읽습니다. */
    while((file = readdir(dir_ptr)) != NULL) {
        // readdir 읽혀진 파일명 중에 현재 디렉토리를 나타네는 . 도 포함되어 있으므로 
        // 무한 반복에 빠지지 않으려면 파일명이 . 이면 skip 해야 함
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
             continue;
        }

        sprintf(filename, "%s/%s", path, file->d_name);

        /* 파일의 속성(파일의 유형, 크기, 생성/변경 시간 등을 얻기 위하여 */
        if(lstat(filename, &buf) == -1) {
            continue;
        }

        if(S_ISDIR(buf.st_mode)) { // 검색된 이름의 속성이 디렉토리이면
            /* 검색된 파일이 directory이면 재귀호출로 하위 디렉토리를 다시 검색 */
            if(rmdirs(filename, is_error_stop) == -1 && is_error_stop) {
                return -1;
            }
        } else if(S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)) { // 일반파일 또는 symbolic link 이면
            if(unlink(filename) == -1 && is_error_stop) {
                return -1;
            }
        }
    }

    /* open된 directory 정보를 close 합니다. */
    closedir(dir_ptr);
    
    return rmdir(path);
}
