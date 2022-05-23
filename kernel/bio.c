// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"
#define NBUCKET 13
struct spinlock helperlock;
struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  //struct buf head;
} bcache[NBUCKET];

void
binit(void)
{
  //struct buf *b;
  uint init_stamp = ticks;
  for(int i = 0; i < NBUCKET; i++){
    initlock(&bcache[i].lock, "bcache");
    for(int j = 0; j < NBUF; j++){
      bcache[i].buf[j].bucket = i;
      bcache[i].buf[j].timestamp = init_stamp;
    }
    // Create linked list of buffers
    /*
    bcache[i].head.prev = &bcache[i].head;
    bcache[i].head.next = &bcache[i].head;
    for(b = bcache[i].buf; b < bcache[i].buf+NBUF; b++){
      b->next = bcache.head.next;
      b->prev = &bcache.head;
      initsleeplock(&b->lock, "buffer");
      bcache[i].head.next->prev = b;
      bcache[i].head.next = b;
    }
    */
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
/*static struct buf*
bget(uint dev, uint blockno)
{
  acquire(&helperlock);
  struct buf *b;
  int id = blockno % NBUCKET;
  acquire(&bcache[id].lock);

  // Is the block already cached?
  for(int i = 0; i < NBUF; i++){
    b = &bcache[id].buf[i];
    if(b->dev == dev && b->blockno == blockno && b->used){
      b->refcnt++;
      b->timestamp = ticks;
      release(&bcache[id].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  struct buf *dst = 0, *src = 0;
  uint time = 0x7fffffff;
  for(int i = 0; i < NBUF; i++){
    if(bcache[id].buf[i].used == 0){
      dst = &bcache[id].buf[i];
      break;
    }
  }
  int pre = -1;
  for(int i = 0; i < NBUCKET; i++){
    if(i != id)
      acquire(&bcache[i].lock);
    int flag = 0;
    for(int j = 0; j < NBUF; j++){
      b = &bcache[i].buf[j];
      if(b->used == 1 && b->refcnt == 0 && b->timestamp < time){
        time = b->timestamp;
        src = b;
        flag = 1;
      }
    }
    if(flag){
      if(pre == -1)
        pre = i;
      else{
        if(pre != id)
          release(&bcache[pre].lock);
        pre = i;
      }
    }
    else if(i != id)
      release(&bcache[i].lock);
  }
  if(src == 0)
    panic("bget: no buffers");
  if(dst == 0)
    dst = src;
  src->used = 0;
  dst->used = 1;
  dst->dev = dev;
  dst->blockno = blockno;
  dst->used = 1;
  dst->valid = 0;
  dst->refcnt = 1;
  release(&bcache[id].lock);
  if(pre != id)
    release(&bcache[pre].lock);
  release(&helperlock);
  acquiresleep(&dst->lock);
  return dst;
}
*/
static struct buf*  
bget(uint dev, uint blockno)
{
  int key = blockno % NBUCKET;
  acquire(&bcache[key].lock);
  struct buf* b;
  for(b=&bcache[key].buf[0]; b<&bcache[key].buf[0]+NBUF; b++){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      b->timestamp=ticks;
      release(&bcache[key].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  uint minstamp=~0;
  struct buf* min_b=0;
  for(b=&bcache[key].buf[0] ; b<&bcache[key].buf[0]+NBUF; b++){
    if(b->timestamp<minstamp && b->refcnt==0){
      minstamp=b->timestamp;
      min_b=b;
    }
  }
  if(min_b!=0){
    min_b->dev = dev;
    min_b->blockno = blockno;
    min_b->valid = 0;
    min_b->refcnt = 1;
    min_b->timestamp=ticks;
    release(&bcache[key].lock);
    acquiresleep(&min_b->lock);
    return min_b;
  }
  panic("bget: no buffers");
}
// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);
  acquire(&bcache[b->bucket].lock);
  b->refcnt--;
  release(&bcache[b->bucket].lock);
}

void
bpin(struct buf *b) {
  acquire(&bcache[b->bucket].lock);
  b->refcnt++;
  release(&bcache[b->bucket].lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache[b->bucket].lock);
  b->refcnt--;
  release(&bcache[b->bucket].lock);
}


