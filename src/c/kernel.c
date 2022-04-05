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
        default:
            printString("Invalid Interrupt");
    }
}

void shell() {
  char input_buf[64];
  char path_str[128];
  char fullCommand[6][64];
  byte current_dir = FS_NODE_P_IDX_ROOT;
  int res;
  struct file_metadata metadata;
  enum fs_retcode return_code;

  while (true) {
    printString("OS@IF2230:");
    printCWD(path_str, current_dir);
    printString("$");
    readString(input_buf);
    
    res = parseCommand(input_buf, &fullCommand);
    
    if (res != -1) {
	printString(" Valid!\r\n");
    	if (strCmpN(fullCommand[0], "\r\ncat", 5)) {
	       // Utility cat
	       printString(" Berhasil!\r\n");
	       //metadata.node_name = fullCommand[1];
	       metadata.node_name = "file_luar";
	       printString(fullCommand[0]);
	       metadata.parent_index = 0xFF;
	       read(&metadata, &return_code);
	       printString(metadata.buffer);
    	} // elif untuk command lainnya
    }    
    // else 
    //   printString("Unknown command\r\n");
    printString("\n");
    //ls(current_dir);
 
  }
}

int parseCommand(char *command, char *arg) {
  bool flag = false;
  int i = 0;
  int j = 0;
  
  while ((*command) == ' ') {
    command ++;
  }
  
  while ((*command) != '\0' && !flag) {
    if (i >= 64) {
    	flag = true;
    } 
    if ((*command) == ' ') {
      *(arg + i + j) = 0;
      j = j + ((i != 0) * 64);
      i = 0;
      break;
    } else if ((*command) == '\\') {
      command ++;
      *(arg + i + j) = *command;
      if (command == 0) {
        flag = true;
      }
      i = i + !flag;
      break;
    } else {
      *(arg + i + j) = *command;
      i ++;
    }
    command ++;
  }
  *(arg + i + j) = 0;
  if (flag) {
    return -1;
  }
  return div(j, 64) + 1;
}

//int cat(

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
  struct sector_filesystem sector_fs_buffer2;
  
  bool found = false;
  bool nameMatch;
  int i = 0;
  int S, j;
  int counter = 0;
  byte temp;

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
    for (i = 0; i < 64; i ++) {
      if (i >= 32) {
        readSector(&(node_fs_buffer.nodes[i]),  FS_NODE_SECTOR_NUMBER + 1);
      } else {
        readSector(&(node_fs_buffer.nodes[i]),  FS_NODE_SECTOR_NUMBER);
      }
      if (node_fs_buffer.nodes[i].parent_node_index == (*metadata).parent_index) {
        if (node_fs_buffer.nodes[i].name[0] != 0x0) {
          nameMatch = true;
          j = 0;
          while (nameMatch && j < 14) {
            if ((*metadata).node_name[j] != node_fs_buffer.nodes[i].name[j]) {
              nameMatch = false;
            } else if (node_fs_buffer.nodes[i].name[j] == '\0' && (*metadata).node_name[j] == '\0') {
              break;
            }
          }
          if (nameMatch) {
            S = node_fs_buffer.nodes[i].sector_entry_index;
            found = true;
            break;
          }
        }
      }
    }
  }

  if (!found) {
    *return_code = FS_R_NODE_NOT_FOUND;
  }

  // 2. Cek tipe node yang ditemukan
  //    Jika tipe node adalah file, lakukan proses pembacaan.
  //    Jika tipe node adalah folder, tuliskan retcode FS_R_TYPE_IS_FOLDER
  //    dan keluar.

  if (S == 0xFF) {
    *return_code = FS_R_TYPE_IS_FOLDER;
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
      memcpy(&node_buffer, &(node_fs_buffer.nodes[S]), sizeof(struct node_entry));
      memcpy(
            &sector_entry_buffer,
            &(sector_fs_buffer.sector_list[S]),
            sizeof(struct sector_entry)
      );
     
      while (counter < 16) {
        if (sector_entry_buffer.sector_numbers[counter] == '0') {
          break;
        }
        readSector(&temp, sector_entry_buffer.sector_numbers[counter]);
        (*metadata).buffer[counter] = temp;
        counter ++;
      }
      (*metadata).filesize = counter --;
      *return_code = FS_SUCCESS;
  }
}

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem   node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct map_filesystem    map_fs_buffer;
  // Tambahkan tipe data yang dibutuhkan
  bool found = false;
  bool nameMatch;
  int i = 0;
  int S, j;
  int counter = 0;
  byte temp;
  int nodeLine = 0;

  // Masukkan filesystem dari storage ke memori
  readSector(map_fs_buffer.is_filled, FS_MAP_SECTOR_NUMBER);
  readSector(node_fs_buffer.nodes, FS_NODE_SECTOR_NUMBER);
  readSector(sector_fs_buffer.sector_list, FS_SECTOR_SECTOR_NUMBER);

  // 1. Cari node dengan nama dan lokasi parent yang sama pada node.
  //    Jika tidak ditemukan kecocokan, lakukan proses ke-2.
  //    Jika ditemukan node yang cocok, tuliskan retcode 
  //    FS_W_FILE_ALREADY_EXIST dan keluar. 
  while (!found && i < 64) {
    for (i = 0; i < 64; i ++) {
      if (i >= 32) {
        readSector(&(node_fs_buffer.nodes[i]),  FS_NODE_SECTOR_NUMBER + 1);
      } else {
        readSector(&(node_fs_buffer.nodes[i]),  FS_NODE_SECTOR_NUMBER);
      }
      if (node_fs_buffer.nodes[i].parent_node_index == (*metadata).parent_index) {
        if (node_fs_buffer.nodes[i].name[0] != 0x0) {
          nameMatch = true;
          j = 0;
          while (nameMatch && j < 14) {
            if ((*metadata).node_name[j] != node_fs_buffer.nodes[i].name[j]) {
              nameMatch = false;
            } else if (node_fs_buffer.nodes[i].name[j] == '\0' && (*metadata).node_name[j] == '\0') {
              break;
            }
          }
          if (nameMatch) {
            S = node_fs_buffer.nodes[i].sector_entry_index;
            found = true;
            break;
          }
        }
      }
    }
  }

  if (found) {
    *return_code = FS_W_FILE_ALREADY_EXIST;
    return;
  }

  // 2. Cari entri kosong pada filesystem node dan simpan indeks.
  //    Jika ada entry kosong, simpan indeks untuk penulisan.
  //    Jika tidak ada entry kosong, tuliskan FS_W_MAXIMUM_NODE_ENTRY
  //    dan keluar.
  for (nodeLine = 0; nodeLine < 64; nodeLine++) {
    if (node_fs_buffer.nodes[nodeLine] == 0x00) {
      emptyFound = true;
      break;
    }
  }
  if (!emptyFound) {
    *return_code = FS_W_MAXIMUM_NODE_ENTRY;
    return;
  }

  // 3. Cek dan pastikan entry node pada indeks P adalah folder.
  //    Jika pada indeks tersebut adalah file atau entri kosong,
  //    Tuliskan retcode FS_W_INVALID_FOLDER dan keluar.

  // 4. Dengan informasi metadata filesize, hitung sektor-sektor 
  //    yang masih kosong pada filesystem map. Setiap byte map mewakili 
  //    satu sektor sehingga setiap byte mewakili 512 bytes pada storage.
  //    Jika empty space tidak memenuhi, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika ukuran filesize melebihi 8192 bytes, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika tersedia empty space, lanjutkan langkah ke-5.

  // 5. Cek pada filesystem sector apakah terdapat entry yang masih kosong.
  //    Jika ada entry kosong dan akan menulis file, simpan indeks untuk 
  //    penulisan.
  //    Jika tidak ada entry kosong dan akan menulis file, tuliskan
  //    FS_W_MAXIMUM_SECTOR_ENTRY dan keluar.
  //    Selain kondisi diatas, lanjutkan ke proses penulisan.

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
  
  // 7. Lakukan update dengan memcpy() buffer entri sector dengan 
  //    buffer filesystem sector
  // 8. Lakukan penulisan seluruh filesystem (map, node, sector) ke storage
  //    menggunakan writeSector() pada sektor yang sesuai
  // 9. Kembalikan retcode FS_SUCCESS
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


}

