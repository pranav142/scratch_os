#include "FAT.h"

void printFAT12BPB(const FAT12_BPB *bpb) {
  printf("Bytes per Sector: %u\n", bpb->bytesPerSector);
  printf("Sectors per Cluster: %u\n", bpb->sectorsPerCluster);
  printf("Reserved Sectors: %u\n", bpb->reservedSectors);
  printf("Number of FATs: %u\n", bpb->numFATs);
  printf("Max Root Directory Entries: %u\n", bpb->maxRootDirEntries);
  printf("Total Sectors: %u\n", bpb->totalSectors);
  printf("Media Descriptor: %X\n", bpb->mediaDescriptor);
  printf("Sectors per FAT: %u\n", bpb->sectorsPerFAT);
  printf("Sectors per Track: %u\n", bpb->sectorsPerTrack);
  printf("Number of Heads: %u\n", bpb->numHeads);
  printf("Hidden Sectors: %u\n", bpb->hiddenSectors);
  printf("Large Total Sectors: %u\n", bpb->largeTotalSectors);
  printf("Physical Drive Number: %u\n", bpb->physicalDriveNumber);
  printf("Reserved: %u\n", bpb->reserved);
  printf("Extended Boot Signature: %X\n", bpb->extendedBootSignature);
  printf("Volume Serial Number: %u\n", bpb->volumeSerialNumber);
  printf("Volume Label: %s\n", bpb->volumeLabel);
  printf("Filesystem Type: %s\n", bpb->filesystemType);
}
