/*
 * sysdep.h -- centralizing compatibility issues between 2.0, 2.2, 2.4
 * Cloned from Id: sysdep.h,v 1.12 2000/05/30 06:12:56 rubini Exp
 */


#ifndef _SYSDEP_H_
#define _SYSDEP_H_

#ifndef LINUX_VERSION_CODE
#  include <linux/version.h>
#endif

#ifndef VERSION_CODE
#  define VERSION_CODE(vers,rel,seq) ( ((vers)<<16) | ((rel)<<8) | (seq) )
#endif

/* remember about the current version */
#if LINUX_VERSION_CODE < VERSION_CODE(2,1,0)
#  define LINUX_20
#elif LINUX_VERSION_CODE < VERSION_CODE(2,3,0)
#  define LINUX_22
#elif LINUX_VERSION_CODE < VERSION_CODE(2,5,0)
#  define LINUX_24
#else
#  define LINUX_26
# endif

/* special check for 2.4.20-8 kernel of redhat 9 */
#if ((LINUX_VERSION_CODE == VERSION_CODE(2,4,20)) && defined(RED_HAT_LINUX_KERNEL))
# define LINUX_REDHAT9
#endif



#include <linux/types.h> /* used later in this header */

/* Modularization issues */
#ifdef LINUX_20
#  define __USE_OLD_SYMTAB__
#  define EXPORT_NO_SYMBOLS register_symtab(NULL);
#  define REGISTER_SYMTAB(tab) register_symtab(tab)
#else
#  define REGISTER_SYMTAB(tab) /* nothing */
#endif

#ifdef __USE_OLD_SYMTAB__
#  define __MODULE_STRING(s)         /* nothing */
#  define MODULE_PARM(v,t)           /* nothing */
#  define MODULE_PARM_DESC(v,t)      /* nothing */
#  define MODULE_AUTHOR(n)           /* nothing */
#  define MODULE_DESCRIPTION(d)      /* nothing */
#  define MODULE_SUPPORTED_DEVICE(n) /* nothing */
#endif


/*
 * "select" changed in 2.1.23. The implementation is twin, but this
 * header is new
 *
 */
#ifdef LINUX_20
#  define __USE_OLD_SELECT__
#  define INODE_FROM_F(filp) ((filp)->f_inode)
#else
#  include <linux/poll.h>
#  define INODE_FROM_F(filp) ((filp)->f_dentry->d_inode)
#endif

/* Other changes in the fops are solved using wrappers */

/*
 * Wait queues changed with 2.3
 */
#ifndef DECLARE_WAIT_QUEUE_HEAD
#  define DECLARE_WAIT_QUEUE_HEAD(head) struct wait_queue *head = NULL
   typedef  struct wait_queue *wait_queue_head_t;
#  define init_waitqueue_head(head) (*(head)) = NULL

/* offer wake_up_sync as an alias for wake_up */
#  define wake_up_sync(head) wake_up(head)
#  define wake_up_interruptible_sync(head) wake_up_interruptible(head)

#endif /* no DECLARE_WAIT_QUEUE_HEAD */



/* FIXME: implement the other versions of wake_up etc */

/* FIXME: implement fasync macros  */

/*
 * access to user space: use the 2.2 functions,
 * and implement them as macros for 2.0
 */

#ifdef LINUX_20
#  include <asm/segment.h>
#  define access_ok(t,a,sz)           (verify_area((t),(a),(sz)) ? 0 : 1)
#  define verify_area_20              verify_area
#  define copy_to_user(t,f,n)         (memcpy_tofs(t,f,n), 0)
#  define __copy_to_user(t,f,n)       copy_to_user((t),(f),(n))
#  define copy_to_user_ret(t,f,n,r)   copy_to_user((t),(f),(n))
#  define copy_from_user(t,f,n)       (memcpy_fromfs((t),(f),(n)), 0)
#  define __copy_from_user(t,f,n)     copy_from_user((t),(f),(n))
#  define copy_from_user_ret(t,f,n,r) copy_from_user((t),(f),(n))
#  define PUT_USER(val,add)           (put_user((val),(add)), 0)
#  define __PUT_USER(val,add)         PUT_USER((val),(add))
#  define PUT_USER_RET(val,add,ret)   PUT_USER((val),(add))
#  define GET_USER(dest,add)          ((dest)=get_user((add)), 0)
#  define __GET_USER(dest,add)        GET_USER((dest),(add))
#  define GET_USER_RET(dest,add,ret)  GET_USER((dest),(add))
#else
#  include <asm/uaccess.h>
#  include <asm/io.h>
#  define verify_area_20(t,a,sz) (0) /* == success */
#  define PUT_USER put_user
#  define __PUT_USER __put_user
#  define PUT_USER_RET put_user_ret
#  define GET_USER get_user
#  define __GET_USER __get_user
#  define GET_USER_RET get_user_ret
#endif

/* ioremap */
#ifdef LINUX_20
#  if 1
   /*
    * although this doesn't work for ISA memory (FIXME: check),
    * it's more portable
    */
#    define ioremap vremap
#    define iounmap vfree
#  else /* this work with ISA too, but is too much PC-centric */
extern inline void *ioremap(unsigned long phys_addr, unsigned long size)
{
    return (phys_addr < 0x10000) ? (void *) phys_addr : vremap(phys_addr, size);
}

extern inline void iounmap(void *addr)
{
    if ((unsigned long) addr > 0x10000)
        vfree(addr);
}
#  endif
#endif

/* implement capable() for 2.0 */
#ifdef LINUX_20
#  define capable(anything)  suser()
#endif

/* The use_count of exec_domain and binfmt changed in 2.1.23 */

#ifdef LINUX_20
#  define INCRCOUNT(p)  ((p)->module ? __MOD_INC_USE_COUNT((p)->module) : 0)
#  define DECRCOUNT(p)  ((p)->module ? __MOD_DEC_USE_COUNT((p)->module) : 0)
#  define CURRCOUNT(p)  ((p)->module && (p)->module->usecount)
#else
#  define INCRCOUNT(p)  ((p)->use_count++)
#  define DECRCOUNT(p)  ((p)->use_count--)
#  define CURRCOUNT(p)  ((p)->use_count)
#endif

/*
 * /proc has changed a lot across the versions...
 */
#ifdef LINUX_20
#  define USE_PROC_REGISTER
# endif


/*
 * 2.2 didn't have create_proc_read_entry yet
 */
#ifdef LINUX_22
#ifdef _LINUX_PROC_FS_H  /* Only if proc_fs.h included */
extern inline void create_proc_read_entry (const char *name, mode_t mode,
                struct proc_dir_entry *base, void *read_func, void *data)
{
        struct proc_dir_entry *ent = create_proc_entry (name, 0, NULL);
        if (ent)
                ent->read_proc = read_func;
}
# endif
#endif



#ifdef LINUX_20
#  define test_and_set_bit(nr,addr)  test_bit((nr),(addr))
#  define test_and_clear_bit(nr,addr) clear_bit((nr),(addr))
#  define test_and_change_bit(nr,addr) change_bit((nr),(addr))
#endif

/* 2.1.30 removed these functions. Let's define them, just in case */
#ifndef LINUX_20
#  define queue_task_irq      queue_task
#  define queue_task_irq_off  queue_task
#endif

/* 2.1.10 and 2.1.43 introduced new functions. They are worth using */

#ifdef LINUX_20

#  include <asm/byteorder.h>
#  ifdef __LITTLE_ENDIAN
#    define cpu_to_le16(x) (x)
#    define cpu_to_le32(x) (x)
#    define cpu_to_be16(x) htons((x))
#    define cpu_to_be32(x) htonl((x))
#  else
#    define cpu_to_be16(x) (x)
#    define cpu_to_be32(x) (x)
     extern inline __u16 cpu_to_le16(__u16 x) { return (x<<8) | (x>>8);}
     extern inline __u32 cpu_to_le32(__u32 x) { return((x>>24) |
             ((x>>8)&0xff00) | ((x<<8)&0xff0000) | (x<<24));}
#  endif

#  define le16_to_cpu(x)  cpu_to_le16(x)
#  define le32_to_cpu(x)  cpu_to_le32(x)
#  define be16_to_cpu(x)  cpu_to_be16(x)
#  define be32_to_cpu(x)  cpu_to_be32(x)

#  define cpu_to_le16p(addr) (cpu_to_le16(*(addr)))
#  define cpu_to_le32p(addr) (cpu_to_le32(*(addr)))
#  define cpu_to_be16p(addr) (cpu_to_be16(*(addr)))
#  define cpu_to_be32p(addr) (cpu_to_be32(*(addr)))

   extern inline void cpu_to_le16s(__u16 *a) {*a = cpu_to_le16(*a);}
   extern inline void cpu_to_le32s(__u16 *a) {*a = cpu_to_le32(*a);}
   extern inline void cpu_to_be16s(__u16 *a) {*a = cpu_to_be16(*a);}
   extern inline void cpu_to_be32s(__u16 *a) {*a = cpu_to_be32(*a);}

#  define le16_to_cpup(x) cpu_to_le16p(x)
#  define le32_to_cpup(x) cpu_to_le32p(x)
#  define be16_to_cpup(x) cpu_to_be16p(x)
#  define be32_to_cpup(x) cpu_to_be32p(x)

#  define le16_to_cpus(x) cpu_to_le16s(x)
#  define le32_to_cpus(x) cpu_to_le32s(x)
#  define be16_to_cpus(x) cpu_to_be16s(x)
#  define be32_to_cpus(x) cpu_to_be32s(x)

#endif

#ifdef LINUX_20
#  define __USE_OLD_REBUILD_HEADER__
#endif

/*
 * 2.0 didn't include sema_init, so we make our own - but only if it
 * looks like semaphore.h got included.
 */
#ifdef LINUX_20
#  ifdef MUTEX_LOCKED
        extern inline void sema_init (struct semaphore *sem, int val)
        {
                sem->count = val;
                sem->waking = sem->lock = 0;
                sem->wait = NULL;
        }
#  endif
#endif /* LINUX_20 */

/*
 * In 2.0, there is no real need for spinlocks, and they weren't really
 * implemented anyway.
 *
 * XXX the _irqsave variant should be defined eventually to do the
 * right thing.
 */
# ifdef LINUX_20
typedef int spinlock_t;
# define spin_lock(lock)
# define spin_unlock(lock)
# define spin_lock_init(lock)
# endif

/*
 * 2.1 stuffed the "flush" method into the middle of the file_operations
 * structure.  The FOP_NO_FLUSH symbol is for drivers that do not implement
 * flush (most of them), it can be inserted in initializers for all 2.x
 * kernel versions.
 */
#ifdef LINUX_20
#  define FOP_NO_FLUSH   /* nothing */
#  define TAG_LLSEEK    lseek
#  define TAG_POLL      select
#else
#  define FOP_NO_FLUSH  NULL,
#  define TAG_LLSEEK    llseek
#  define TAG_POLL      poll
#endif



/*
 * fasync changed in 2.2.
 */
# ifdef LINUX_20
/*  typedef struct inode *fasync_file; */
# define fasync_file struct inode *
# else
  typedef int fasync_file;
# endif

/* other things that are virtualized: define the new functions for the old k */
#ifdef LINUX_20
#  define in_interrupt() (intr_count!=0)
#  define mdelay(x) udelay((x)*1000)
#  define signal_pending(current)  (current->signal & ~current->blocked)
#endif

#ifdef LINUX_20 /* a whole set of replacement functions */
#  include "pci-compat.h"
#else /* a placeholder is needed. as 2.0 allocs/clears pci structures */
#  define  pci_release_device(d)
#endif

/*
 * Some task state stuff
 */

#ifndef set_current_state
#  define set_current_state(s) current->state = (s);
#endif

#ifdef LINUX_20
extern inline void schedule_timeout(int timeout)
{
        current->timeout = jiffies + timeout;
        current->state = TASK_INTERRUPTIBLE;
        schedule ();
        current->timeout = 0;
}

extern inline long sleep_on_timeout(wait_queue_head_t *q, signed long timeout)
{
        signed long early = 0;

        current->timeout = jiffies + timeout;
        sleep_on (q);
        if (current->timeout > 0)
        {
                early = current->timeout - jiffies;
                current->timeout = 0;
        }
        return (early);
}


extern inline long interruptible_sleep_on_timeout(wait_queue_head_t *q,
                signed long timeout)
{
        signed long early = 0;

        current->timeout = jiffies + timeout;
        interruptible_sleep_on (q);
        if (current->timeout > 0)
        {
                early = current->timeout - jiffies;
                current->timeout = 0;
        }
        return (early);
}

#endif /* LINUX_20 */

/*
 * I/O memory was not managed by ealier kernels, define them as success
 */

#if !defined(LINUX_24) && !defined(LINUX_26)
#  define check_mem_region(start, len)          0
#  define request_mem_region(start, len, name)  {}
#  define release_mem_region(start, len)        {}

#endif /* not LINUX_24 */

#ifdef LINUX_26

/*
 * MOD_INC_USE_COUNT is deprecated in 2.6. Instead, the calling module
 * must call try_module_get()
 */
# undef  MOD_INC_USE_COUNT
# undef  MOD_DEC_USE_COUNT
# define MOD_INC_USE_COUNT {}
# define MOD_DEC_USE_COUNT {}
/*
 * check_region deprecated. Why???
 */
# undef check_region
# define check_region(s,n) 		0
#else
/* pre 2.6 */
# define try_module_get(mod) 	1
# define module_put(mod)		do { ; } while(0);

#endif /* LINUX_26 */

#endif /* _SYSDEP_H_ */
