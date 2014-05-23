
#include "threads/scheduler.h"



 
 
 /*
    ;; rsp + 196   -> ss                <-- rsi (savedregs) points here
    ;; rsp + 188   -> rsp
    ;; rsp + 180   -> rflags
    ;; rsp + 172   -> cs
    ;; rsp + 164   -> rip          
    ;; rsp + 156   -> return pointer    
    ;; rsp + 148   -> interrupt number
    ;; rsp + 140   -> rax
    ;; rsp + 132   -> rbx
    ;; rsp + 124   -> rcx
    ;; rsp + 116   -> rdx
    ;; rsp + 108   -> rsi
    ;; rsp + 100   -> rdi
    ;; rsp + 92    -> rsp
    ;; rsp + 84    -> rbp
    ;; rsp + 76    -> r8
    ;; rsp + 68    -> r9
    ;; rsp + 60    -> r10
    ;; rsp + 52    -> r11
    ;; rsp + 44    -> r12
    ;; rsp + 36    -> r13
    ;; rsp + 28    -> r14
    ;; rsp + 20    -> r15
    ;; rsp + 18    -> ds                <-- 16 bit!
    ;; rsp + 16    -> es                <-- 16 bit!
    ;; rsp + 8     -> fs
    ;; rsp         -> gs
 ***************/
struct RegisterContext {
    uint64_t ss;
    uint64_t rsp;
    uint64_t rflags;
    uint64_t cs;
    uint64_t rip;
    uint64_t retptr;
    
    uint64_t interruptnumber; // Only used for continuity
    
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rsp2;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    
    uint16_t ds;
    uint16_t es;
    
    uint64_t fs;
    uint64_t gs;
};



struct Thread {
    struct RegisterContext *regs;
    
    uint32_t thread_id;
    
    void *stackbottom;    // Keep a pointer to their stack
    // Dead means the thread will be removed from the queue
    // next time it comes up (typically happens when thread
    // is currently running).
    
    // Sleeping means that it is waiting for a specified 
    // amount of time to pass before resuming.
    
    // Waiting means it's waiting for a some signal to occur
    // (haven't decided on the signal) before resuming
    
    // Active is active
    enum { dead, sleeping, waiting, active } status; 
};



void context_to_savedregs(struct RegisterContext *context, uint64_t savedregs);
void savedregs_to_context(struct RegisterContext *context, uint64_t savedregs);


// Fairly 'low' number for now (will use 64k of memory to store the queue)
#define MAX_THREADS         1024   
#define NULL                0


struct Thread **threads_queue;
int threads_queue_head = 0;
int threads_queue_tail = 0;

struct Thread *current_thread;


void init_threads()
{
    threads_queue = (struct Thread **)malloc(MAX_THREADS * sizeof(struct Thread *));
    
    threads_queue_head = 0;
    threads_queue_tail = 0;
    
    // Initialize the queue to null
    for(int i = 0; i < MAX_THREADS; i++)
    {
        threads_queue[i] = (struct Thread *)NULL;
    }
}

int is_thread_queue_full()
{
    return ( (threads_queue_tail + 1) % MAX_THREADS ) == threads_queue_head;
}
int is_thread_queue_empty()
{
    return (threads_queue_head == threads_queue_tail);
}

bool enqueue_thread(struct Thread* thread)
{
    if( is_thread_queue_full() )
    {
        // Queue is full!
        return false;
    }
    
    threads_queue[ threads_queue_tail ] = thread;
    
    threads_queue_tail = (threads_queue_tail + 1) % MAX_THREADS;
    
    return true;
}


struct Thread *dequeue_thread()
{
    struct Thread *tmp;
    if( is_thread_queue_empty() )
    {
        return NULL;
    }
    
    tmp = threads_queue[ threads_queue_head ];
    threads_queue_head = (threads_queue_head + 1) % MAX_THREADS;
    return tmp;
}


/*
 * Called when a timer interrupt is fired. 
 * 
 ***************/
void scheduler_event(uint64_t savedregs)
{
    struct Thread *temp = NULL;
    
    if(current_thread != NULL)
    {
        // We need to store the savedregs into this current process's store
        savedregs_to_context(current_thread->regs, savedregs);

        // Add this thread to the queue if it's not dead
        if( current_thread->status != dead )
        {
            if( !enqueue_thread(current_thread) )
            {
                // We ran out of space to store this thread!
                // Should not happen as we should not be able
                // to add a new thread when there's only 1 slot left
            }
        }
        else
        {
            // Cleanup the thread
            free(current_thread->stackbottom);
            free(current_thread->regs);
            free(current_thread);
        }
    }
    
    
    // Pull the next thread from the queue
    while((temp = dequeue_thread()) != NULL) // != NULL is for clarity only
    {
        // TODO: other thread statuses, check sleaping threads, and waiting threads
        if( temp->status == waiting )
        {
            
        }
        if( temp->status == sleeping )
        {
            
        }
        if( temp->status == active ) break; // Break off on the first active thread
    }
    
    
    if( temp != NULL ) // We have no active threads queued
    {
        current_thread = temp;
        register_trace_noerror(savedregs);
        // Place it's stored regisiters into saved regs
        context_to_savedregs(current_thread->regs, savedregs);
        // And that's it for now
        register_trace_noerror(savedregs);
        //__asm__("cli");
        //__asm__("hlt");
    }
}


uint32_t new_kthread( void (*function)(void), uint64_t stack_size )
{
    static int curid = 0;
    
    struct Thread *newthread = (struct Thread *)malloc(sizeof(struct Thread));
    
    newthread->regs = (struct RegisterContext *)malloc(sizeof(struct RegisterContext));
    
    newthread->stackbottom = malloc(stack_size);
    
    newthread->regs->rsp = (uint64_t)( (uint64_t)newthread->stackbottom + stack_size );
    
    
    newthread->regs->rip = (uint64_t)function;
    newthread->regs->retptr = (uint64_t)function;
    
    // Not sure what these values should be!!
    newthread->regs->cs = 0;
    newthread->regs->ds = 0;
    newthread->regs->es = 0;
    newthread->regs->fs = 0;
    newthread->regs->gs = 0;
    newthread->regs->ss = 0;
    
    newthread->regs->rflags = 0;
    
    newthread->regs->rbp = 0;
    newthread->regs->rdi = 0;
    newthread->regs->rsi = 0;
    
    newthread->regs->rax = 0;
    newthread->regs->rbx = 0;
    newthread->regs->rcx = 0;
    newthread->regs->rdx = 0;
    
    newthread->regs->r8  = 0;
    newthread->regs->r9  = 0;
    newthread->regs->r10 = 0;
    newthread->regs->r11 = 0;
    newthread->regs->r12 = 0;
    newthread->regs->r13 = 0;
    newthread->regs->r14 = 0;
    newthread->regs->r15 = 0;
    
    // Need to initialize the saved regs
    // What value should these be?!
    
    newthread->thread_id = curid++;
    newthread->status    = active;
    
    enqueue_thread(newthread);
    
    return newthread->thread_id;
}






// Set the return pointer of a thread to this!
void delete_thread_self( )
{
    
    current_thread->status = dead;
    
    
    
    // Now how do we properly go to the next thread
    // And is it really fair to swap without a clock since the next
    // thread will only get the remaining time...
    
    // Maybe we should mark the thread as dead so it's not readded
    // to the queue on the next context switch
}
/*
 * Copies the registers from savedregs into a context
 * 
 */

void savedregs_to_context(struct RegisterContext *context, uint64_t savedregs)
{
    // Probably could just be doing a memcpy since they both should be in the same
    // order in ram
    context->ss     = *(uint64_t *)(savedregs - ( 0 * 8 ));
    context->rsp    = *(uint64_t *)(savedregs - ( 1 * 8 ));
    context->rflags = *(uint64_t *)(savedregs - ( 2 * 8 ));
    context->cs     = *(uint64_t *)(savedregs - ( 3 * 8 ));
    context->rip    = *(uint64_t *)(savedregs - ( 4 * 8 ));
    context->retptr = *(uint64_t *)(savedregs - ( 5 * 8 ));
    
    context->interruptnumber = *(uint64_t *)(savedregs - (  6 * 8 ));
    
    context->rax = *(uint64_t *)(savedregs - (  7 * 8 ));
    context->rbx = *(uint64_t *)(savedregs - (  8 * 8 ));
    context->rcx = *(uint64_t *)(savedregs - (  9 * 8 ));
    context->rdx = *(uint64_t *)(savedregs - ( 10 * 8 ));
    context->rsi = *(uint64_t *)(savedregs - ( 11 * 8 ));
    context->rdi = *(uint64_t *)(savedregs - ( 12 * 8 ));
    context->rsp2 = *(uint64_t *)(savedregs - ( 13 * 8 ));
    context->rbp = *(uint64_t *)(savedregs - ( 14 * 8 ));
    
    context->r8  = *(uint64_t *)(savedregs - ( 15 * 8 ));
    context->r9  = *(uint64_t *)(savedregs - ( 16 * 8 ));
    context->r10 = *(uint64_t *)(savedregs - ( 17 * 8 ));
    context->r11 = *(uint64_t *)(savedregs - ( 18 * 8 ));
    context->r12 = *(uint64_t *)(savedregs - ( 19 * 8 ));
    context->r13 = *(uint64_t *)(savedregs - ( 20 * 8 ));
    context->r14 = *(uint64_t *)(savedregs - ( 21 * 8 ));
    context->r15 = *(uint64_t *)(savedregs - ( 22 * 8 ));
    
    context->ds  = *(uint16_t *)(savedregs - ( 23 * 8 ) + 0);
    context->es  = *(uint16_t *)(savedregs - ( 23 * 8 ) + 2);
    
    context->fs  = *(uint64_t *)(savedregs - ( 23 * 8 ) + 4);
    context->gs  = *(uint64_t *)(savedregs - ( 24 * 8 ) + 4);
    
}

/*
 * Copies the registers from savedregs into a context
 * 
 */
void context_to_savedregs(struct RegisterContext *context, uint64_t savedregs)
{
    // Probably could just be doing a memcpy since they both should be in the same
    // order in ram
    *(uint64_t *)(savedregs - ( 0 * 8 )) = context->ss;
    *(uint64_t *)(savedregs - ( 1 * 8 )) = context->rsp;
    *(uint64_t *)(savedregs - ( 2 * 8 )) = context->rflags;
    *(uint64_t *)(savedregs - ( 3 * 8 )) = context->cs;
    *(uint64_t *)(savedregs - ( 4 * 8 )) = context->rip;
    *(uint64_t *)(savedregs - ( 5 * 8 )) = context->retptr;

    *(uint64_t *)(savedregs - (  6 * 8 )) = context->interruptnumber;

    *(uint64_t *)(savedregs - (  7 * 8 )) = context->rax;
    *(uint64_t *)(savedregs - (  8 * 8 )) = context->rbx;
    *(uint64_t *)(savedregs - (  9 * 8 )) = context->rcx;
    *(uint64_t *)(savedregs - ( 10 * 8 )) = context->rdx;
    *(uint64_t *)(savedregs - ( 11 * 8 )) = context->rsi;
    *(uint64_t *)(savedregs - ( 12 * 8 )) = context->rdi;
    *(uint64_t *)(savedregs - ( 13 * 8 )) = context->rsp2;
    *(uint64_t *)(savedregs - ( 14 * 8 )) = context->rbp;

    *(uint64_t *)(savedregs - ( 15 * 8 )) = context->r8;
    *(uint64_t *)(savedregs - ( 16 * 8 )) = context->r9;
    *(uint64_t *)(savedregs - ( 17 * 8 )) = context->r10;
    *(uint64_t *)(savedregs - ( 18 * 8 )) = context->r11;
    *(uint64_t *)(savedregs - ( 19 * 8 )) = context->r12;
    *(uint64_t *)(savedregs - ( 20 * 8 )) = context->r13;
    *(uint64_t *)(savedregs - ( 21 * 8 )) = context->r14;
    *(uint64_t *)(savedregs - ( 22 * 8 )) = context->r15;

    *(uint16_t *)(savedregs - ( 23 * 8 ) + 0) = context->ds;
    *(uint16_t *)(savedregs - ( 23 * 8 ) + 2) = context->es;

    *(uint64_t *)(savedregs - ( 23 * 8 ) + 4) = context->fs;
    *(uint64_t *)(savedregs - ( 24 * 8 ) + 4) = context->gs;
    
}
