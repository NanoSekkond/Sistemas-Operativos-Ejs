#include <linux/init.h>
#include <linux/module.h> // THIS_MODULE
#include <linux/kernel.h> // Kernel cosas
#include <linux/fs.h>     // File operations
#include <linux/cdev.h>   // Char devices
#include <linux/device.h> // Nodos filesystem
#include <linux/uaccess.h> // copy_to_user
#include <linux/slab.h>    // kmalloc
#include <linux/random.h>  // get_random_bytes

static int upper_bound = -1;

static ssize_t azar_read(struct file *filp, char __user *data, size_t size, loff_t *offset) {

    /* Completar */
    printk(KERN_ALERT "El upper bound es %d\n", upper_bound);

    if (upper_bound < 1) {
        return -EPERM;
    }

    int rand;
    get_random_bytes(&rand, sizeof(rand));

    int num = (rand + upper_bound) % upper_bound;

    char* res = kmalloc(size, 0);
    snprintf(res, size, "%d\n", num);
    copy_to_user(data, res, size);

    kfree(res);

    return size;
}

static ssize_t azar_write(struct file *filp, const char __user *data, size_t size, loff_t *offset) {

    /* Completar */
    char* buffer = kmalloc((size + 1) * sizeof(char), 0);
    copy_from_user(buffer, data, size);
    buffer[size] = '\0';

    int num;
    if (kstrtoint(buffer, 10, &num)) {
        printk(KERN_ALERT "Recibi fruta :(\n");
        kfree(buffer);
        return -EPERM;
    }

    upper_bound = num;
    kfree(buffer);

    return size;
}

static struct file_operations azar_fops = {
    .owner = THIS_MODULE,
    .read = azar_read,
    .write = azar_write
};

static dev_t azar_devno;
static struct cdev azar_cdev;
static struct class *azar_class;

static int __init azar_init(void) {
    cdev_init(&azar_cdev, &azar_fops);
    alloc_chrdev_region(&azar_devno, 0, 1, "SO-azar");
    cdev_add(&azar_cdev, azar_devno, 1);

    azar_class = class_create(THIS_MODULE, "azar");
    device_create(azar_class, NULL, azar_devno, NULL, "azar");

    printk(KERN_ALERT "Loading module 'azar'\n");
    return 0;
}

static void __exit azar_exit(void) {
    printk(KERN_ALERT "Unloading module 'azar'\n");

    device_destroy(azar_class, azar_devno);
    class_destroy(azar_class);

    unregister_chrdev_region(azar_devno, 1);
    cdev_del(&azar_cdev);
}

module_init(azar_init);
module_exit(azar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Generador de números al azar");