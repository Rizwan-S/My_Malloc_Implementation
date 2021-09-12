#include<stdio.h>
#include<stddef.h>
#include <string.h>

//Memory Size
# define memorySize 5000

//Global Array for memory
char memory[memorySize];

//Struct that defines blocks of memory
typedef struct block
{
    size_t size; 
    int free;  
    struct block *next; 
} block;

block* memory_list = (void*) memory;

//Function prototype
void initialize();
void split_big_space(block *big_space, size_t size);
void merge_free_blocks();
void* my_malloc(size_t size);
void* my_calloc(size_t nmemb, size_t size);
void my_free(void* ptr);

size_t my_heap_free_space (void);

int main()
{
    printf("Calloc testing with int...\n");
    int* m = (int*)my_calloc(10, sizeof(int));
    for(int i = 0; i < 10; i++)
    {
        printf("stored: %d, address: %p\n", *(m+i), (m+i));
    }
    printf("\nCalloc tested successfully!\n\n");

    printf("Checking free space...\n");
    printf("Free space: %ld\n", my_heap_free_space());

    printf("Malloc testing with float...\n");
    float* f = (float*)my_malloc(10*sizeof(float));
    for(int i =  0; i < 10; i++)
    {
        *(f+i) = i + 0.5;
        printf("stored: %f, address: %p \n", *(f + i), (f+i));
    }
    printf("\nMalloc tested successfully!\n\n");

    printf("Checking free space...\n");
    printf("Free space: %ld\n", my_heap_free_space());

    my_free(m);
    printf("Free space after freeing m pointer: %ld\n", my_heap_free_space());

    my_free(f);
    printf("Free space after freeing f pointer: %ld\n", my_heap_free_space());

    printf("Calloc testing with char...\n");
    char* ch = (char*)my_calloc(10, sizeof(char));
    for(int i = 0; i < 10; i++)
    {
        printf("stored: %d, address: %p\n", *(ch+i), (ch+i));
    }
    printf("\nCalloc tested successfully!\n\n");

    printf("Checking free space...\n");
    printf("Free space: %ld\n", my_heap_free_space());

    my_free(ch);
    printf("Free space after freeing ch pointer: %ld\n", my_heap_free_space());

    return 0;
}

//<-------------- MEMORY FUNCTIONS ------------------>
//We initialize the block of memory before starting
void initialize()
{
    long long int size_ = memorySize - sizeof(block);

    if(size_ < 0)        
        memory_list->size = 0;
    else memory_list->size = size_;
    
    memory_list->free = 1;
    memory_list->next = NULL;
    printf("Memory initialized.\n");
}

//If bigger memory is free, we divide it into two parts (one to use and other we leave it as it is)
void split_big_space(block *big_space, size_t size)
{
    block* free_space = (void*)((void*)big_space + size + sizeof(block));

    long long int size_ = (big_space->size) - size - sizeof(block);

    if(size_ < 0)
        free_space->size = 0;
    else
        free_space->size = size_;
    free_space->free = 1;
    free_space->next = big_space->next;
    
    big_space->size = size;
    big_space->free = 0;
    big_space->next = free_space;
    printf("Block split complete.\n");
}

//We use this function to combine two consecutive free blocks of memory.
void merge_free_blocks()
{
    block *curr;
    curr = memory_list;
    while(curr && curr->next)
    {
        if((curr->free) && (curr->next->free))
        {
            curr->size += (curr->next->size) + sizeof(block);
            curr->next = curr->next->next;
        }
        curr = curr->next;
    }
}

//<------------MY_MALLOC-------------->
void* my_malloc(size_t size)
{
    block *curr;
    void *result;
    if(!(memory_list->size))
    { 
        initialize();
    }
    curr = memory_list;
    while((curr->size < size || curr->free == 0) && curr->next != NULL)
    {
        printf("Block occupied! Checking next block...\n");
        curr = curr->next;
    }

    if(curr->size == size)
    {
        curr->free = 0;
        result = (void*)(++curr);
        printf("Empty block with exact memory size found...\n");
    }
    else if(curr->size > size + sizeof(struct block))
    {
        printf("Empty block with bigger memory found. Spliting the block...\n");
        split_big_space(curr, size);
        result = (void*)(++curr);
    }
    else
    {
        result = NULL;
        printf("Couldn't allocate block. Insufficient memory left...\n");
    }
    return result;
}


//<-----------------MY_CALLOC------------------>
void* my_calloc (size_t nmemb, size_t size)
{
    char* p = my_malloc(nmemb * size);
    if(!p) return NULL;
    memset(p, 0, nmemb * size);
    printf("Initializing space to all bits 0\n");
    return p;
}

//<--------------MY_HEAP_FREE_SPACE------------->
size_t my_heap_free_space (void)
{
    size_t total_memory = sizeof(memory);
    block* curr = memory_list;
    size_t occupied = 0;
    while(curr)
    {
        if(!(curr->free))
        {
            occupied += curr->size;
        }
        curr = curr->next;
    }
    if(total_memory - occupied < 0)
        return 0;
    return (total_memory - occupied);
}

void my_free(void* ptr)
{
    if(ptr && ((void*)memory <= ptr) && (ptr <= (void*)(memory + memorySize)))
    {
        block* curr = ptr;
        --curr;
        curr->free=1;
        merge_free_blocks();
        printf("Freed the memory block.\n");
    }
    else 
        printf("Please provide a valid pointer.\n");
}
