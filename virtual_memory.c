#include <stdio.h>
#include <stdbool.h>
#define PAGE_SIZE 1024
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define FRAME_COUNT 256
#define MEMORY_SIZE (FRAME_SIZE * FRAME_COUNT)
typedef struct {
bool valid;
int frame_number;
} PageTableEntry;
typedef struct {
bool valid;
bool dirty;
int page_number;
} FrameTableEntry;
PageTableEntry page_table[PAGE_TABLE_SIZE];
FrameTableEntry frame_table[FRAME_COUNT];
char memory[MEMORY_SIZE];

void initialize() {
int i;
for (i = 0; i < PAGE_TABLE_SIZE; i++) {
page_table[i].valid = false;
page_table[i].frame_number = -1;
}
for (i = 0; i < FRAME_COUNT; i++) {
frame_table[i].valid = false;
frame_table[i].dirty = false;
frame_table[i].page_number = -1;
}
}
void page_fault(int page_number) {
int victim_frame, victim_page;
// Find a victim frame using a random replacement algorithm
do {
victim_frame = rand() % FRAME_COUNT;
victim_page = frame_table[victim_frame].page_number;
} while (frame_table[victim_frame].valid && page_table[victim_page].valid);
// If the victim page is valid and dirty, write it back to disk
if (frame_table[victim_frame].valid && frame_table[victim_frame].dirty) {
printf("Page %d is being written back to disk.\n", victim_page);
// Write the page back to disk
frame_table[victim_frame].dirty = false;
}
// Load the requested page into the victim frame
printf("Page %d is being loaded into frame %d.\n", page_number, victim_frame);
frame_table[victim_frame].valid = true;
frame_table[victim_frame].page_number = page_number;
page_table[page_number].valid = true;
page_table[page_number].frame_number = victim_frame;
}
char read_byte(int virtual_address) {
int page_number = virtual_address / PAGE_SIZE;
int offset = virtual_address % PAGE_SIZE;
if (!page_table[page_number].valid) {
printf("Page fault for virtual address %d.\n", virtual_address);
page_fault(page_number);
}
int frame_number = page_table[page_number].frame_number;
int physical_address = (frame_number * FRAME_SIZE) + offset;
return memory[physical_address];}
void write_byte(int virtual_address, char value) {
int page_number = virtual_address / PAGE_SIZE;
int offset = virtual_address % PAGE_SIZE;
if (!page_table[page_number].valid) {
printf("Page fault for virtual address %d.\n", virtual_address);
page_fault(page_number); }
int frame_number = page_table[page_number].frame_number;
int physical_address = (frame_number * FRAME_SIZE) + offset;

memory[physical_address] = value;
frame_table[frame_number].dirty = true;}
int main() {
initialize();
// Access virtual memory
read_byte(0); // Page fault
write_byte(1024, 'A');
read_byte(1024);
write_byte(2048, 'B');
read_byte(2048);
read_byte(1024);

return 0;
}