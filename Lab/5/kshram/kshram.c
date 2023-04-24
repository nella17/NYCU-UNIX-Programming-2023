#include "kshram.h"

#include <linux/module.h>	// included for all kernel modules
#include <linux/kernel.h>	// included for KERN_INFO
#include <linux/init.h>		// included for __init and __exit macros
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/errno.h>
#include <linux/sched.h>	// task_struct requried for current_uid()
#include <linux/cred.h>		// for current_uid();
#include <linux/slab.h>		// for kmalloc/kfree
#include <linux/uaccess.h>	// copy_to_user
#include <linux/string.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/pagemap.h>

#define DEVICE_NAME "kshram"
#define SLOTS 8
#define DEFAULT_SIZE PAGE_SIZE

MODULE_DESCRIPTION(DEVICE_NAME);
MODULE_AUTHOR("nella17");
MODULE_LICENSE("GPL");

static dev_t dev;
static struct class *clazz;
static struct cdev c_dev;

long SHRAM_SIZE[SLOTS] = {};
void* SHRAM_PTR[SLOTS] = {};

void kshram_free(long idx) {
    if (idx < 0 || idx >= SLOTS) {
        printk(KERN_INFO DEVICE_NAME ": invalid slot index %ld.\n", idx);
        return;
    }
    if (SHRAM_PTR[idx] == NULL)
        return;
    for (int i = 0; i < SHRAM_SIZE[idx]; i += PAGE_SIZE)
        ClearPageReserved(virt_to_page((unsigned long)SHRAM_PTR[idx] + i));
    kfree(SHRAM_PTR[idx]);
    SHRAM_SIZE[idx] = 0;
    SHRAM_PTR[idx] = NULL;
    printk(KERN_INFO DEVICE_NAME ": freed slot %ld.\n", idx);
}

void* kshram_alloc(long idx, long size) {
    if (idx < 0 || idx >= SLOTS) {
        printk(KERN_INFO DEVICE_NAME ": invalid slot index %ld.\n", idx);
        return NULL;
    }
    size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    SHRAM_SIZE[idx] = size;
    SHRAM_PTR[idx] = kzalloc(size, GFP_KERNEL);
    for (int i = 0; i < size; i += PAGE_SIZE)
        SetPageReserved(virt_to_page((unsigned long)SHRAM_PTR[idx] + i));
    printk(KERN_INFO DEVICE_NAME ": allocated %ld bytes @ %p. idx=%ld.\n", size, SHRAM_PTR[idx], idx);
    return SHRAM_PTR[idx];
}

static int kshram_dev_open(struct inode *inode, struct file *fp) {
    long idx = iminor(fp->f_inode);
    printk(KERN_INFO DEVICE_NAME ": device opened. idx=%ld.\n", idx);
    return 0;
}

static int kshram_dev_close(struct inode *inode, struct file *fp) {
    long idx = iminor(fp->f_inode);
    printk(KERN_INFO DEVICE_NAME ": device closed. idx=%ld.\n", idx);
    return 0;
}

static long kshram_dev_ioctl(struct file *fp, unsigned int cmd, unsigned long arg) {
    long idx = iminor(fp->f_inode);

    switch (cmd) {
        case KSHRAM_GETSLOTS:
            return SLOTS;

        case KSHRAM_GETSIZE:
            printk(KERN_INFO DEVICE_NAME ": GETSIZE. idx=%ld size=%ld.\n", idx, SHRAM_SIZE[idx]);
            return SHRAM_SIZE[idx];

        case KSHRAM_SETSIZE:
            printk(KERN_INFO DEVICE_NAME ": SETSIZE. idx=%ld size=%ld.\n", idx, arg);
            kshram_free(idx);
            kshram_alloc(idx, arg);
            return 0;
    }

    printk(KERN_INFO DEVICE_NAME ": ioctl cmd=%u arg=%lu. idx=%ld.\n", cmd, arg, idx);

    return 0;
}

static int kshram_dev_mmap(struct file *fp, struct vm_area_struct *vma) {
    long idx = iminor(fp->f_inode);
    unsigned long size = vma->vm_end - vma->vm_start;
    struct page* page;
    unsigned long pfn;

    printk(KERN_INFO DEVICE_NAME ": mmap. idx=%ld size=%lu.\n", idx, size);

    if (SHRAM_PTR[idx] == NULL || size > SHRAM_SIZE[idx])
        return -EINVAL;

    page = virt_to_page(SHRAM_PTR[idx]);
    pfn = page_to_pfn(page);

    if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot))
        return -EAGAIN;

    return 0;
}

static const struct file_operations kshram_dev_fops = {
    .owner = THIS_MODULE,
    .open = kshram_dev_open,
    .release = kshram_dev_close,
    .unlocked_ioctl = kshram_dev_ioctl,
    .mmap = kshram_dev_mmap,
};

static int kshram_proc_read(struct seq_file *m, void *v) {
    for (int i = 0; i < SLOTS; i++)
        seq_printf(m, "%02d: %ld\n", i, SHRAM_SIZE[i]);
    return 0;
}

static int kshram_proc_open(struct inode *inode, struct  file *file) {
    return single_open(file, kshram_proc_read, NULL);
}

static const struct proc_ops kshram_proc_ops = {
    .proc_open = kshram_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static char *kshram_devnode(const struct device *dev, umode_t *mode) {
    if(mode == NULL) return NULL;
    *mode = 0666;
    return NULL;
}

static int kshram_init(void)
{
    int i;
    // create char dev
    if(alloc_chrdev_region(&dev, 0, SLOTS, "updev") < 0)
        return -1;
    if((clazz = class_create(THIS_MODULE, DEVICE_NAME)) == NULL)
        goto release_region;
    clazz->devnode = kshram_devnode;
    for (i = 0; i < SLOTS; i++) {
        dev_t devnum = MKDEV(MAJOR(dev), i);
        if(device_create(clazz, NULL, devnum, NULL, DEVICE_NAME "%d", i) == NULL)
            goto release_class;
        cdev_init(&c_dev, &kshram_dev_fops);
        if(cdev_add(&c_dev, devnum, SLOTS) == -1)
            goto release_device;
    }
    // create proc
    proc_create(DEVICE_NAME, 0, NULL, &kshram_proc_ops);
    //
    for (int i = 0; i < SLOTS; i++)
        kshram_alloc(i, DEFAULT_SIZE);
    printk(KERN_INFO DEVICE_NAME ": initialized.\n");
    return 0;    // Non-zero return means that the module couldn't be loaded.

release_device:
    cdev_del(&c_dev);
release_class:
    for (int j = 0; j < i; j++)
        device_destroy(clazz, MKDEV(MAJOR(dev), j));
    class_destroy(clazz);
release_region:
    unregister_chrdev_region(dev, SLOTS);
    return -1;
}

static void kshram_exit(void)
{
    remove_proc_entry(DEVICE_NAME, NULL);

    for (int i = 0; i < SLOTS; i++)
        kshram_free(i);

    cdev_del(&c_dev);
    for (int i = 0; i < SLOTS; i++) {
        dev_t devnum = MKDEV(MAJOR(dev), i);
        device_destroy(clazz, devnum);
    }
    class_destroy(clazz);
    unregister_chrdev_region(dev, SLOTS);

    printk(KERN_INFO DEVICE_NAME ": cleaned up.\n");
}

module_init(kshram_init);
module_exit(kshram_exit);
