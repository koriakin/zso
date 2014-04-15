#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/bitops.h>

#define FINAL 10000

static struct proc_dir_entry *pde_dir;
static struct proc_dir_entry *pde_symlink;
static struct proc_dir_entry *pde_file;

static void *ct_seq_start(struct seq_file *s, loff_t *pos)
{
	loff_t *spos;
	if (*pos > FINAL) {
		return NULL;
	}
	while (hweight_long(*pos) != 4) {
		(*pos)++;
		if (*pos > FINAL) {
			return NULL;
		}
	}
	spos = kmalloc(sizeof *spos, GFP_KERNEL);
	if (!spos)
		return ERR_PTR(ENOMEM);
	*spos = *pos;
	return spos;
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	loff_t *spos = (loff_t *)v;
	(*pos)++;
	if (*pos > FINAL) {
		kfree(v);
		return NULL;
	}
	while (hweight_long(*pos) != 4) {
		(*pos)++;
		if (*pos > FINAL) {
			kfree(v);
			return NULL;
		}
	}
	*spos = *pos;
	return spos;
}

static void ct_seq_stop(struct seq_file *s, void *v)
{
	if (v)
		kfree(v);
}

static int ct_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *)v;
	seq_printf(s, "%Ld\n", *spos);
	return 0;
}

static struct seq_operations ct_seq_ops = {
	.start = ct_seq_start,
	.next  = ct_seq_next,
	.stop  = ct_seq_stop,
	.show  = ct_seq_show
};

static int ct_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &ct_seq_ops);
};

static const struct file_operations ct_file_ops = {
	.owner   = THIS_MODULE,
	.open    = ct_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};

static int seqfile_example_init(void) {
	pde_dir = proc_mkdir("seqfile_dir", NULL);
	if (!pde_dir)
		goto out_dir;
	pde_symlink = proc_symlink("seqfile_link", NULL, "seqfile_dir/counter");
	if (!pde_symlink)
		goto out_symlink;
	pde_file = proc_create("counter", 0444, pde_dir, &ct_file_ops);
	if (!pde_file)
		goto out_file;
	return 0;
out_file:
	proc_remove(pde_symlink);
out_symlink:
	proc_remove(pde_dir);
out_dir:
	return -ENOMEM;
}

static void seqfile_example_cleanup(void) {
	proc_remove(pde_file);
	proc_remove(pde_symlink);
	proc_remove(pde_dir);
}

module_init(seqfile_example_init);
module_exit(seqfile_example_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marcin Kościelnicki");
MODULE_DESCRIPTION("procfs + seqfile example");
