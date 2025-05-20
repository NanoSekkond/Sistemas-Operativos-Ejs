#include <linux/init.h>
#include <linux/module.h> // THIS_MODULE
#include <linux/kernel.h> // Kernel cosas
#include <linux/fs.h>     // File operations
#include <linux/cdev.h>   // Char devices
#include <linux/device.h> // Nodos filesystem
#include <linux/uaccess.h> // copy_to_user
#include <linux/slab.h>    // kmalloc
#include <linux/random.h>  // get_random_bytes

#define SLOT_COUNT 3

static spinlock_t lock;
static int available_space;

typedef struct user_data {
    bool valid;
    char input;
} user_data;

static int letras_open(struct inode *inod, struct file *filp) {
    /* Completar */
    spin_lock(&lock);
    if (available_space == 0) {
        printk(KERN_ALERT "No habia espacio\n");
        spin_unlock(&lock);
        return -EPERM;
    }
    available_space--;
    spin_unlock(&lock);

    filp->private_data = kmalloc(sizeof(struct user_data), GFP_KERNEL);
    ((user_data *) filp->private_data)->valid = false;
    return 0;
}

static int letras_release(struct inode *inod, struct file *filp) {

    /* Completar */
    spin_lock(&lock);
    available_space++;
    spin_unlock(&lock);
    return 0;
}

static ssize_t letras_read(struct file *filp, char __user *data, size_t size, loff_t *offset) {
    user_data *udata = (user_data *) filp->private_data;

    /* Completar */

    // Si no mando un caracter
    if (udata->valid == false) {
        return -EPERM;
    }

    char* res = kmalloc(size * sizeof(char), 0);
    for (int i = 0; i < size; i++) {
        res[i] = udata->input;
    }

    copy_to_user(data, res, size * sizeof(char));
    return size;
}

static ssize_t letras_write(struct file *filp, const char __user *data, size_t size, loff_t *offset) {
    user_data *udata = (user_data *) filp->private_data;

    // No es la primera vez que escribe
    if (udata->valid) {
        return size;
    }

    udata->valid = true;

    char* buffer = kmalloc((size + 1) * sizeof(char), 0);
    copy_from_user(buffer, data, size);
    buffer[size] = '\0';

    udata->input = buffer[0];

    kfree(buffer);

    return size;
}

static struct file_operations letras_fops = {
    .owner = THIS_MODULE,
    .open = letras_open,
    .read = letras_read,
    .write = letras_write,
    .release = letras_release,
};

static dev_t letras_devno;
static struct cdev letras_cdev;
static struct class *letras_class;

static int __init letras_init(void) {
    available_space = 3;
    spin_lock_init(&lock);

    cdev_init(&letras_cdev, &letras_fops);
    alloc_chrdev_region(&letras_devno, 0, 1, "SO-letras123");
    cdev_add(&letras_cdev, letras_devno, 1);

    letras_class = class_create(THIS_MODULE, "letras123");
    device_create(letras_class, NULL, letras_devno, NULL, "letras123");

    printk(KERN_ALERT "Loading module 'letras123'\n");
    return 0;
}

static void __exit letras_exit(void) {
    printk(KERN_ALERT "Unloading module 'letras123'\n");

    device_destroy(letras_class, letras_devno);
    class_destroy(letras_class);

    unregister_chrdev_region(letras_devno, 1);
    cdev_del(&letras_cdev);
}

module_init(letras_init);
module_exit(letras_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Generador de números al letras");
