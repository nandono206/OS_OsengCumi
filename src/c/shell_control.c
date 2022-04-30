#include "header/std_type.h"
#include "header/std_lib.h"
#include "header/shell.h"
#include "header/filesystem_utils.h"
#include "header/mkdir.h"
#include "header/ls.h"
#include "header/cd.h"
#include "header/cat.h"
#include "header/mv.h"
#include "header/cp.h"


void printCWD(char *path_str, byte curr_dir){
    byte directories[1024];
    char folderList[32];
    char outputString[256];
    int i, indexes, counter;
    

    printString("~/");
    counter = 0;
    //jika bukan  di root
    if (curr_dir != 0xFF) 
    {
        readSector(directories, FS_NODE_SECTOR_NUMBER);
        readSector(directories + 512, FS_MAP_SECTOR_NUMBER+1);
        
        i = 0;
        while (curr_dir != 0xFF)
        {
            folderList[i] = curr_dir;
            curr_dir = directories[curr_dir * 16];
            i++;
        }
        i--;
        while (i >= 0)
        {
            indexes = 0;
            while (directories[folderList[i] * 16 + 2 + indexes] != 0x00)
            {
                outputString[counter] = directories[folderList[i] * 16 + 2 + indexes];
                indexes++;
                counter++;
            }
            if (i > 0)
            {
                outputString[counter] = '/';
                counter++;
            }
            i--;
        }
        outputString[counter] = 0x00;
        printString(outputString);
    }
}


void splitCommand(char *input, char *com, char *args1, char *args2, char divider)
{
    int i = 0;
    char *j = input;
    int comLength = 0;
    int args1Length = 0;
    int args2Length = 0;

    while (*j != 0x00)
    {
        if (*j == divider)
        {
            if (i == 0)
            {
                i = 1;
            } else if (i == 1) {
            	i = 2;
            }
        }
        else
        {
            if (i == 1)
            {
            	args1[args1Length] = *j;
                args1Length++;
            } else if (i == 2) {
            	com[comLength] = *j;
                comLength++;
            }
            else
            {
                args2[args2Length] = *j;
                args2Length++;
            }
        }
        j++;
    }
    com[comLength] = 0x0;
    args1[args1Length] = 0x0;
    args2[args2Length] = 0x0;
}


void shell() {
  char input_buf[64];
  char command[64];
  char args1[64];
  char args2[64];
  char path_str[128];
  char fullCommand[10][64];
  byte current_dir = FS_NODE_P_IDX_ROOT;
  int res;
  struct file_metadata metadata;
  struct file_metadata meta;
  enum fs_retcode return_code;

  while (true) {
    printString("OS@IF2230:");
    printCWD(path_str, current_dir);
    printString("$");
    readString(input_buf);
    printString("\n");
    
    splitCommand(input_buf, args2, args1, command, ' ');
    // printString(command);
    // printString(args1);
    // printString(args2);
    // printString(command);
    // printString("\n");
    // printString(args);
    
    /*if (res != -1) {
	printString(" Valid!\r\n");
    	if (strCmpN(fullCommand[0], "\r\n", 5)) {
	       // Utility cd
	       printString(" Berhasil!\r\n");
	       cat("file_luar", current_dir);
	       */
	       //metadata.node_name = fullCommand[1];
	    
   // elif untuk command lainnya
      if (strcmp(command, "\r\ncat")){
        cat(args1, current_dir);
      }
      else if (strcmp(command, "\r\nls")) {
        lsCommand(current_dir);
        printString("\n");
        // printString("inside command\r\n");
      }
      else if (strcmp(command, "\r\ncd")){
        // printString("inside cd command\r\n");
        cd(current_dir, args1, &current_dir);
      }
      else if (strcmp(command, "\r\nmkdir")){
        // printString("inside cd command\r\n");
        mkdir(args1, current_dir);
      } else if (strcmp(command, "\r\ncp")) {
      	cp(args1, args2, current_dir);
      } else if (strcmp(command, "\r\nmv")) {
      	mv(args2, args1, current_dir);
      } else if (strcmp(command, "\r\ntest")) {
        meta.node_name    = "shell";
        meta.parent_index = 0xFF;
        executeProgram(&meta, 0x2000);
    }

      else {
        printString("Unknown command\r\n");
      }
    }    
    // else 
    //   printString("Unknown command\r\n");
    printString("\r\n");
    //ls(current_dir);
 
  
}