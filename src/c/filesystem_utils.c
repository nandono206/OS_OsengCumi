#include "header/filesystem_utils.h"


void readSector(byte *buffer, int sector_number)
{
  int sector_read_count = 0x01;
  int cylinder, sector;
  int head, drive;
  cylinder = div(sector_number, 36) << 8;     // CH
  sector = mod(sector_number, 18) + 1;        // CL
  head = mod(div(sector_number, 18), 2) << 8; // DH
  drive = 0x00;                               // DL
  interrupt(
      0x13,                       // Interrupt number
      0x0200 | sector_read_count, // AX
      buffer,                     // BX
      cylinder | sector,          // CX
      head | drive                // DX
  );
}

void writeSector(byte *buffer, int sector_number)
{
  int sector_read_count = 0x01;
  int cylinder, sector;
  int head, drive;
  cylinder = div(sector_number, 36) << 8;     // CH
  sector = mod(sector_number, 18) + 1;        // CL
  head = mod(div(sector_number, 18), 2) << 8; // DH
  drive = 0x00;                               // DL
  interrupt(
      0x13,                       // Interrupt number
      0x0300 | sector_read_count, // AX
      buffer,                     // BX
      cylinder | sector,          // CX
      head | drive                // DX
  );
}

void fillMap()
{
  struct map_filesystem map_fs_buffer;
  int i;
  // Load filesystem map
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  /*
  Edit filesystem map disini
  */
  // kernel part
  for (i = 0; i < 16; i++)
  {
    map_fs_buffer.is_filled[i] = true;
  }
  // unreachable part
  for (i = 256; i < 512; i++)
  {
    map_fs_buffer.is_filled[i] = true;
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



int findNodeIdx(char* name, byte parent, struct node_filesystem *buffer){
  int i = 0;
  while (i < 64){
    if (buffer->nodes[i].parent_node_index == parent && strcmp(buffer->nodes[i].name, name)) {
      return i;
    }
    i++;
  }
  
  if (i == 64) {
    return -1;
  }

  return i;
}

void initializeNode(){
  struct node_filesystem node_fs_buffer;
  int i;
  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  i = 0;
  while (i < 64) {
    node_fs_buffer.nodes[i].name[0] = '\0';
    i++;
  }

  writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
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
