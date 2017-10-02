/**
 * @file debugfs.c
 * @author janczer
 * @version 0.1
 */

#include <linux/string.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("janczer");
MODULE_DESCRIPTION("Simple debugfs object");
MODULE_VERSION("0.1");

char data[PAGE_SIZE]; // variable to store data
int data_len = 0;

static struct dentry *data_file;
static struct dentry *subdir;

// function for handle read and write events
static ssize_t data_read(struct file *, char *, size_t, loff_t *);
static ssize_t data_write(struct file *, const char *, size_t, loff_t *);

const struct file_operations data_file_fops = {
	.owner = THIS_MODULE,
	.write = data_write,
	.read = data_read,
};

static int __init debugfs_init(void)
{
	subdir = debugfs_create_dir("debugfs_test", NULL);
	if (!subdir)
		return -ENOENT;

	data_file = debugfs_create_file("data", 0644, subdir, NULL,
			&data_file_fops);
	if (!data_file)
		goto exit;

	return 0;
exit:
	debugfs_remove_recursive(subdir);
	return -ENOENT;
}

static void __exit debugfs_exit(void)
{
	debugfs_remove_recursive(subdir);
}

static ssize_t data_read(struct file *f, char *buffer,
		size_t len, loff_t *offset)
{
	return simple_read_from_buffer(buffer, len, offset, data, data_len);
}

static ssize_t data_write(struct file *f, const char *buffer,
		size_t len, loff_t *offset)
{
	ssize_t ret;

	ret = simple_write_to_buffer(data, PAGE_SIZE, offset, buffer, len);

	if (ret < 0)
		return ret;

	data_len = ret;

	return len;
}

module_init(debugfs_init);
module_exit(debugfs_exit);
