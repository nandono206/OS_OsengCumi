// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"
#include "header/std_lib.h"
#include "header/filesystem.h"
#include "header/std_type.h"

int main() {
  char buf[512];
  clearScreen();
  fillMap();
  makeInterrupt21();
  printString("Halo dunia!\r\n");
  shell();
  while(true) {
    readString(buf);
    printString(buf);
  }
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
    switch (AX) {
        case 0x0:
            printString(BX);
            break;
        case 0x1:
            readString(BX);
            break;
        case 0x2:
            readSector(BX, CX);
            break;
        case 0x3:
            writeSector(BX, CX);
            break;
        case 0x4:
            read(BX, CX);
            break;
        case 0x5:
            write(BX, CX);
            break;
        case 0x6:
            executeProgram(BX, CX);
            break;
        default:
            printString("Invalid Interrupt");
    }
}

void executeProgram(struct file_metadata *metadata, int segment)
{
    enum fs_retcode fs_ret;
    byte buf[8192];

    metadata->buffer = buf;
    read(metadata, &fs_ret);
    if (fs_ret == FS_SUCCESS)
    {
        int i = 0;
        for (i = 0; i < 8192; i++)
        {
            if (i < metadata->filesize)
                putInMemory(segment, i, metadata->buffer[i]);
            else
                putInMemory(segment, i, 0x00);
        }
        launchProgram(segment);
    }
    else
        printString("exec: file not found\r\n");
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

void printString(char *string) {
  int i = 0;
  while (string[i] != '\0') {
    interrupt(0x10, 0xe00 + string[i], 0, 0, 0);
    if (string[i] == '\n') {
        interrupt(0x10, 0xe00+0xd, 0x000F, 0, 0);
    }
    i++;
  }
  // interrupt(0x10, 0xe00 + '\n', 0, 0, 0);
  // interrupt(0x10, 0xe00 + '\r', 0, 0, 0);
}


void readString(char *string){
  int i = 2;
  int ax = 0xe << 8;
  string[0] = '\r'; // enter
  string[1] = '\n'; // newline
  while(true) {
    char input = interrupt(0x16, 0x0, 0, 0, 0);
    if(input == '\r'){ // input adalah enter
      string[i] = '\0'; // endstring
      string[i + 1] = '\r';
      string[i + 2] = '\n';
      break;
    } else if (input == '\b') { // input adalah backspace
        if (i > 2) {
          interrupt(0x10, 0xe00 + '\b', 0, 0, 0);
          interrupt(0x10, 0xe00 + '\0', 0, 0, 0);
          interrupt(0x10, 0xe00 + '\b', 0, 0, 0);
          string[i] = '\0';
          i--;
        } else {
          i = 2;
	  string[0] = '\r';
	  string[1] = '\n';
	}
    } else {
        string[i] = input;
	ax += input;
	interrupt(0x10, ax, 0x11, 0, 0);
	ax -= input;
	i++;
    }
  }
}


void clearScreen() {
   int AX, BX, DX;
   AX = (0x07 << 8) | 0x00;
   BX = (0x07 << 8) | 0;
   DX = (24 << 8) | 79;
   interrupt(0x10, AX, BX, 0, DX);
   AX = (0x02 << 8) | 0x00;
   interrupt(0x10, AX, 0, 0, 0);
}

void readSector(byte *buffer, int sector_number) {
    int sector_read_count = 0x01;
    int cylinder, sector;
    int head, drive;

    cylinder = div(sector_number, 36) << 8; // CH
    sector   = mod(sector_number, 18) + 1;  // CL

    head  = mod(div(sector_number, 18), 2) << 8; // DH
    drive = 0x00;                                // DL

    interrupt(
        0x13,                       // Interrupt number
        0x0200 | sector_read_count, // AX
        buffer,                     // BX
        cylinder | sector,          // CX
        head | drive                // DX
    );
}

void writeSector(byte *buffer, int sector_number) {
    int sector_read_count = 0x01;
    int cylinder, sector;
    int head, drive;

    cylinder = div(sector_number, 36) << 8; // CH
    sector   = mod(sector_number, 18) + 1;  // CL

    head  = mod(div(sector_number, 18), 2) << 8; // DH
    drive = 0x00;                                // DL

    interrupt(
        0x13,                       // Interrupt number
        0x0301 | sector_read_count, // AX
        buffer,                     // BX
        cylinder | sector,          // CX
        head | drive                // DX
    );
}

void fillMap() {
  struct map_filesystem map_fs_buffer;
  int i;

  // Load filesystem map
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);

  /* 
    Edit filesystem map disini 
                             */
  for (i=0; i<16; i++){
    if (i>=0 && i < 16){
      map_fs_buffer.is_filled[i] = true;
    }
    else if (i>255 && i<512){
      map_fs_buffer.is_filled[i] = true;
    }
    
    
  }
  

  // Update filesystem map
  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER); 
}


void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
  
  // Tambahkan tipe data yang dibutuhkan
  struct node_filesystem   node_fs_buffer; // 1024 byte
  struct node_entry        node_buffer;
  struct sector_filesystem sector_fs_buffer; // 512 byte
  struct sector_entry      sector_entry_buffer;
  
  bool found = false;
  int i, j = 0;
  int S;

  // Masukkan filesystem dari storage ke memori buffer
  // Asumsikan semua buffer filesystem diatas telah terinisiasi dengan baik

  // Pembacaan storage ke buffer
  readSector(sector_fs_buffer.sector_list, FS_SECTOR_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[0]),  FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // 1. Cari node dengan nama dan lokasi yang sama pada filesystem.
  //    Jika ditemukan node yang cocok, lanjutkan ke langkah ke-2.
  //    Jika tidak ditemukan kecocokan, tuliskan retcode FS_R_NODE_NOT_FOUND
  //    dan keluar.  

  while (!found && i < 64) {
    memcpy(&node_buffer, &(node_fs_buffer.nodes[i]), sizeof(struct node_entry));
    if (node_buffer.parent_node_index == metadata->parent_index && strcmp(node_buffer.name, metadata->node_name)) {
    	S = node_buffer.sector_entry_index;
    	found = true;
    }
    
  }

  if (!found) {
    *return_code = FS_R_NODE_NOT_FOUND;
    return;
  }

  // 2. Cek tipe node yang ditemukan
  //    Jika tipe node adalah file, lakukan proses pembacaan.
  //    Jika tipe node adalah folder, tuliskan retcode FS_R_TYPE_IS_FOLDER
  //    dan keluar.

  if (S == FS_NODE_S_IDX_FOLDER) {
    *return_code = FS_R_TYPE_IS_FOLDER;
    return;
  } else {
  	
      // Pembacaan
      // 1. memcpy() entry sector sesuai dengan byte S
      // 2. Lakukan iterasi proses berikut, i = 0..15
      // 3. Baca byte entry sector untuk mendapatkan sector number partisi file
      // 4. Jika byte bernilai 0, selesaikan iterasi
      // 5. Jika byte valid, lakukan readSector() 
      //    dan masukkan kedalam buffer yang disediakan pada metadata
      // 6. Lompat ke iterasi selanjutnya hingga iterasi selesai
      // 7. Tulis retcode FS_SUCCESS dan ganti filesize 
            //pada akhir proses pembacaan.
      memcpy(&sector_entry_buffer, &(sector_fs_buffer.sector_list[S]), sizeof(struct node_entry));
     S = sector_entry_buffer.sector_numbers[j];
      while (j < 15 && S != 0) {
        readSector(&metadata->buffer[j*512], S);
        j++;
        S = sector_entry_buffer.sector_numbers[j];
      }
      (*metadata).filesize = j*512;
      *return_code = FS_SUCCESS;
  } 
}

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem   node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct map_filesystem    map_fs_buffer;
  // Tambahkan tipe data yang dibutuhkan
  struct sector_entry sector_entry_buffer;
  bool nodeEmpty = false;
  bool sectorEmpty = false;
  int emptySectorCounter = 0;
  int empty_space = 0;
  int i = 0, j = 0;
  int emptyMapCount = 0;
  
  int nodeIdx, sectorIdx;

  // Masukkan filesystem dari storage ke memori
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  readSector(sector_fs_buffer.sector_list, FS_SECTOR_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);  

  // 1. Cari node dengan nama dan lokasi parent yang sama pada node.
  //    Jika tidak ditemukan kecocokan, lakukan proses ke-2.
  //    Jika ditemukan node yang cocok, tuliskan retcode 
  //    FS_W_FILE_ALREADY_EXIST dan keluar. 
  for (i = 0; i < 64; i++) {
    if (node_fs_buffer.nodes[i].parent_node_index == metadata->parent_index && strcmp(node_fs_buffer.nodes[i].name, metadata->node_name)) {
      *return_code = FS_W_FILE_ALREADY_EXIST;
      printString("Caution: File already exist.");
      return;
    }
  }

  // 2. Cari entri kosong pada filesystem node dan simpan indeks.
  //    Jika ada entry kosong, simpan indeks untuk penulisan.
  //    Jika tidak ada entry kosong, tuliskan FS_W_MAXIMUM_NODE_ENTRY
  //    dan keluar.
  /* for (nodeLine = 0; nodeLine < 64; nodeLine++) {
    if (node_fs_buffer.nodes[nodeLine] == 0x00) {
      emptyFound = true;
      break;
    }
  } */
  /* if (!emptyFound) {
  for (i = 0; i < 64; i++) {
    if (strlen(node_fs_buffer.nodes[i].name) == 0) {
      nodeEmpty = true;
      nodeIdx = i;
      break;
    }
  }
  if (!nodeEmpty) {
    printString("Caution: Maximum node entry.");
    *return_code = FS_W_MAXIMUM_NODE_ENTRY;
    return;
  } */
  // for (i = 0; i < 64; i++) {
  //       if (strlen(node_fs_buffer.nodes[i].name) == 0) {
  //           nodeEmpty = true;
  //           nodeIdx = i;
  //           break;
  //       }
  //   }

  //   if (!nodeEmpty) {
  //       printString("Caution: Maximum node entry.");
  //       *return_code = FS_W_MAXIMUM_NODE_ENTRY;
  //       return;
  //   }
    i = 0;
    while (i < 64 && !nodeEmpty)
    {
        if (strlen(node_fs_buffer.nodes[i].name) == 0)
        {
            nodeIdx = i;
            nodeEmpty = true;
        }
        i++;
    }

    if (!nodeEmpty)
    {
        printString("Caution: Maximum node entry.");
        *return_code = FS_W_MAXIMUM_NODE_ENTRY;
        return;
    }



  // 3. Cek dan pastikan entry node pada indeks P adalah folder.
  //    Jika pada indeks tersebut adalah file atau entri kosong,
  //    Tuliskan retcode FS_W_INVALID_FOLDER dan keluar.
    if (metadata->parent_index != FS_NODE_P_IDX_ROOT &&
        !(node_fs_buffer.nodes[metadata->parent_index].sector_entry_index == FS_NODE_S_IDX_FOLDER))
    {
        printString("Caution: Invalid folder.");
        *return_code = FS_W_INVALID_FOLDER;
        return;
    }

   

  // 4. Dengan informasi metadata filesize, hitung sektor-sektor 
  //    yang masih kosong pada filesystem map. Setiap byte map mewakili 
  //    satu sektor sehingga setiap byte mewakili 512 bytes pada storage.
  //    Jika empty space tidak memenuhi, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika ukuran filesize melebihi 8192 bytes, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika tersedia empty space, lanjutkan langkah ke-5.
  for (i = 0; i < 512; i++){
        if (!map_fs_buffer.is_filled[i])
        {
            emptySectorCounter++;
        }
    }

    empty_space = emptySectorCounter * 512;

    if (empty_space < metadata->filesize || metadata->filesize > 8192){
        printString("Caution: Not enough storage.");
        *return_code = FS_W_NOT_ENOUGH_STORAGE;
        return;
    }

  // 5. Cek pada filesystem sector apakah terdapat entry yang masih kosong.
  //    Jika ada entry kosong dan akan menulis file, simpan indeks untuk 
  //    penulisan.
  //    Jika tidak ada entry kosong dan akan menulis file, tuliskan
  //    FS_W_MAXIMUM_SECTOR_ENTRY dan keluar.
  //    Selain kondisi diatas, lanjutkan ke proses penulisan.
  i = 0;
    while (i < 32 && !sectorEmpty)
    {
        if (sector_fs_buffer.sector_list[i].sector_numbers[0] == 0)
        {
            sectorIdx = i;
            sectorEmpty = true;
        }
        i++;
    }

    if (!sectorEmpty)
    {
        printString("Caution: Maximum sector entry.");
        *return_code = FS_W_MAXIMUM_SECTOR_ENTRY;
        return;
    }

  // Penulisan
  // 1. Tuliskan metadata nama dan byte P ke node pada memori buffer
  // 2. Jika menulis folder, tuliskan byte S dengan nilai 
  //    FS_NODE_S_IDX_FOLDER dan lompat ke langkah ke-8
  // 3. Jika menulis file, tuliskan juga byte S sesuai indeks sector
  // 4. Persiapkan variabel j = 0 untuk iterator entry sector yang kosong
  // 5. Persiapkan variabel buffer untuk entry sector kosong
  // 6. Lakukan iterasi berikut dengan kondisi perulangan 
        //(penulisan belum selesai && i = 0..255)
  //    1. Cek apakah map[i] telah terisi atau tidak
  //    2. Jika terisi, lanjutkan ke iterasi selanjutnya / continue
  //    3. Tandai map[i] terisi
  //    4. Ubah byte ke-j buffer entri sector dengan i
  //    5. Tambah nilai j dengan 1
  //    6. Lakukan writeSector() dengan file pointer buffer pada metadata 
  //       dan sektor tujuan i
  //    7. Jika ukuran file yang telah tertulis lebih besar atau sama dengan
  //       filesize pada metadata, penulisan selesai
  strcpy(node_fs_buffer.nodes[nodeIdx].name, metadata->node_name);
  node_fs_buffer.nodes[nodeIdx].parent_node_index = metadata->parent_index;

  if (metadata->filesize != 0) {
    node_fs_buffer.nodes[nodeIdx].sector_entry_index = sectorIdx;
  } 
  else {
    node_fs_buffer.nodes[nodeIdx].sector_entry_index = FS_NODE_S_IDX_FOLDER;
  }

  i = 0;
    while (i < 255)
    {
        if (!map_fs_buffer.is_filled[i])
        {
            map_fs_buffer.is_filled[i] = true;
            sector_entry_buffer.sector_numbers[j] = i;
            j++;
            writeSector(metadata->buffer, i);
            metadata->filesize -= 512;
            metadata->buffer += 512;
            if (metadata->filesize <= 0)
            {
                break;
            }
        }
        i++;
    }

  // 7. Lakukan update dengan memcpy() buffer entri sector dengan 
  //    buffer filesystem sector
  // 8. Lakukan penulisan seluruh filesystem (map, node, sector) ke storage
  //    menggunakan writeSector() pada sektor yang sesuai
  // 9. Kembalikan retcode FS_SUCCESS
  memcpy(sector_fs_buffer.sector_list[sectorIdx].sector_numbers, sector_entry_buffer.sector_numbers, 16);

  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  writeSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  *return_code = FS_SUCCESS;
}

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

void ls(byte curr_dir){
  byte all[1024];
  int i;

  readSector(all, FS_MAP_SECTOR_NUMBER);
  readSector(all + 512, FS_NODE_SECTOR_NUMBER+1);

  for(i = 0; i<64; i++){
    if (curr_dir == all[i * 16]){
      printString(all + i * 16 + 2);
      printString("\r\n");
    }
  }
  printString("\r\n");


}

void cd(byte parentIdx, char *targetName, byte *newParentIdx)
{
    enum fs_retcode return_code;
    struct node_filesystem node_fs_buffer;
    bool found;
    int i;

    readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    if (strcmp(targetName, ".."))
    {
        if (parentIdx == FS_NODE_P_IDX_ROOT)
        {
            printString("cd: root doesn't have parent directory\r\n");
            return_code = FS_SUCCESS;
            return;
        }
        *newParentIdx = node_fs_buffer.nodes[parentIdx].parent_node_index;
        return;
    }
    if (strcmp(targetName, "/"))
    {
        *newParentIdx = FS_NODE_P_IDX_ROOT;
        return_code = FS_SUCCESS;
        return;
    }
    i = 0;
    found = false;
    while (i < 64 && !found)
    {
        if (node_fs_buffer.nodes[i].parent_node_index == parentIdx && strcmp(targetName, node_fs_buffer.nodes[i].name))
        {
            *newParentIdx = i;
            return_code = FS_SUCCESS;
            found = true;
        }
        i++;
    }
    if (!found)
    {
        printString("cd: no such file or directory: ");
        printString(targetName);
        printString("\r\n");
        return_code = FS_R_NODE_NOT_FOUND;
    }
}

void mkdir(char *name, byte cwd) {
    struct file_metadata metadata;
    enum fs_retcode retcode;

    metadata.filesize = 0;
    metadata.node_name = name;
    metadata.parent_index = cwd;

    write(&metadata, &retcode);

    if (retcode != FS_SUCCESS) {
        printString("Unable to make a new directory. \r\n");
    }
}

void cat(char* name, byte curr_dir){
    enum fs_retcode return_code;
    struct file_metadata metadata;
    struct node_entry node;
    struct node_filesystem node_fs;
    byte buffer[8192];

    clear(buffer, 8192);

    metadata.parent_index = curr_dir;
    metadata.buffer = buffer;
    metadata.node_name = name;

    read(&metadata, &return_code);
    
    if (return_code == FS_R_NODE_NOT_FOUND) {
    	printString("File not found\n"); 	
    } else if (return_code == FS_R_TYPE_IS_FOLDER) {
    	printString("Not a file\n");
    }
    printString((char *)metadata.buffer);
    printString("\n");
}

void lsCommand(byte curr_dir){
    struct node_filesystem node_fs_buffer;
    char* name[64];
    int i;
    int length;
    
    readSector(&(node_fs_buffer.nodes[0]),  FS_NODE_SECTOR_NUMBER);
	  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    for(i = 0; i < 64; i ++){
        if(node_fs_buffer.nodes[i].parent_node_index == curr_dir && strlen(node_fs_buffer.nodes[i].name) != 0){
            printString("\r\n");
            printString(node_fs_buffer.nodes[i].name);
        }
    }
}

void cp(char *filename, char *tocopyname, byte currdir)
{
    byte buffer[8192];
    struct node_filesystem node_fs;
    struct node_entry node;
    struct file_metadata metadata;
    enum fs_retcode retcode;
    int i;

    clear(buffer, 8192);
    metadata.parent_index = currdir;
    metadata.buffer = buffer;
    metadata.node_name = filename;

    read(&metadata, &retcode);

    switch (retcode)
    {
    case FS_R_NODE_NOT_FOUND:
        printString("Error: File \"");
        printString(filename);
        printString("\" not found.\r\n");
        
        return;
    case FS_R_TYPE_IS_FOLDER:
        printString("Error: Can't copy directory.\r\n");
        return;

    }

    metadata.node_name = tocopyname;

    write(&metadata, &retcode);



    switch (retcode)
    {
    case FS_W_FILE_ALREADY_EXIST:
        printString("Error: File already exist \r\n");
        break;
    case FS_W_NOT_ENOUGH_STORAGE:
        printString("Error: Not enough space.\r\n");;
        break;
    case FS_W_MAXIMUM_NODE_ENTRY:
        printString("Error: Node reached maximum capacity.\r\n");
        break;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
        printString("Error: Sector reached maximum capacity.\r\n");
        break;
    case FS_W_INVALID_FOLDER:
        printString("Error: Invalid folder.\r\n");
        break;
    }
}

void mv(char *dir, char* file, byte cwd) {
    struct node_filesystem node_fs_buffer;
    byte new_dir;
    bool found = false;
    int node_idx = 0;

    readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    while (!found && node_idx < 64) {
        if (strcmp(dir, node_fs_buffer.nodes[node_idx].name) && node_fs_buffer.nodes[node_idx].parent_node_index == cwd) {
            new_dir = node_idx;
            found = true;
        }
        node_idx ++;
    }

    if (!found) {
        printString("Directory not found\n");
        return;
    }

    found = false;
    node_idx = 0;
    while (!found && node_idx < 64) {
        if (strcmp(file, node_fs_buffer.nodes[node_idx].name) && node_fs_buffer.nodes[node_idx].parent_node_index == cwd) {
            node_fs_buffer.nodes[node_idx].parent_node_index = new_dir;
            found = true;
        }
        node_idx ++;
    }
    if (!found) {
        printString("File not found\n");
        return;
    }

    writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}
