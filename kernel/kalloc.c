// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void
kinit()
{
  //initialize all the CPU locks
  for (int cpu = 0; cpu < NCPU; cpu++){
    initlock(&kmem[cpu].lock, "kmem");
  }
  
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  push_off();
  int cpu = cpuid();
  pop_off();

  acquire(&kmem[cpu].lock);

  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);

  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    //code from kfree modified to work for only the CPU calling kinint
    memset(p, 1, PGSIZE);

    struct run *r;
    r = (struct run*)p;
    r->next = kmem[cpu].freelist;
    kmem[cpu].freelist = r;
  }

  release(&kmem[cpu].lock);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);
  r = (struct run*)pa;

  push_off();
  int cpu = cpuid();
  pop_off();

  acquire(&kmem[cpu].lock);
  r->next = kmem[cpu].freelist;
  kmem[cpu].freelist = r;
  release(&kmem[cpu].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  push_off();
  int cpu = cpuid();
  pop_off();
  acquire(&kmem[cpu].lock);

  r = kmem[cpu].freelist;
  if (r)
    kmem[cpu].freelist = r->next;
  else
  {
    for (int i = 0; i < NCPU; i++)
    {
      //if we are checking ourselfs we skip this iteration else we
      //get a lock on the CPU we're checking
      if (i == cpu)
        continue;
      acquire(&kmem[i].lock);
      //get the CPU's next page
      r = kmem[i].freelist;
      //if the page is free, we 'steal' it
      if (r)
      {	
        kmem[i].freelist = r->next;
        //we also release the lock on the CPU
        release(&kmem[i].lock);
        break;
      }else{
        //if we didnt get a blank page we release and keep going through the for loop
        release(&kmem[i].lock);
      }
    }
  }
  //release CPU lock
  release(&kmem[cpu].lock);
 //fill the page
  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  return (void *)r;
}