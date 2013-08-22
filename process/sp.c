#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/page.h>
#include <asm/thread_info.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/gfp.h>
#include <linux/limits.h>

// max path length, WRONG!!!!
//#define PATH_LENGTH 4096

MODULE_AUTHOR("hitmoon <xiaoqiang.zhao@i-soft.com.cn>");
MODULE_LICENSE("GPL");

static pid_t pid = 1;
module_param(pid, int, 0);

char *buf; // for path name 

unsigned long report_stack()
{
	unsigned long esp;
	unsigned long ret;
	__asm__("mov %%rsp, %0\n\t"
			:"=r"(esp)
		);

	ret = (void*)esp - (void*)current->thread_info - sizeof(struct thread_info);

	printk("current free size: %ld\n", ret);
	return ret;
}




// list all files a process open
void list_files(struct task_struct *p)
{
	struct files_struct *files = NULL;
	struct file *file;
	struct dentry *root;
	struct vfsmount *rootmnt;
	char *path;
	int fd;
	struct fdtable *fdt;
	if (p) {
		get_task_struct(p);

		// get_files_struct
		task_lock(p);
		files = p->files;
		if (files)
			atomic_inc(&files->count);
		task_unlock(p);
		// end get_files_struct
		
		put_task_struct(p);
	}

	printk(KERN_ALERT "open files(separated by';'): ");

	if (files) {
		rcu_read_lock();
		fdt = files_fdtable(files);
		for (fd = 0; fd < fdt->max_fds; fd++) {

			file = fcheck_files(files, fd);
			rcu_read_unlock();
			if (file) {
				rootmnt = mntget(file->f_vfsmnt);
				// here should check for null f_dentry!!
				root = dget(file->f_dentry);
				spin_unlock(&files->file_lock);
				path = d_path(root, rootmnt, buf, PATH_MAX);
				printk("%s; ",path);
			}
			rcu_read_lock();
		}
		rcu_read_unlock();
			
	}

	put_files_struct(files);
	printk("\n\n");

}


void list_children(struct task_struct *parent)
{
	struct list_head *list;
	struct task_struct *chld;

	list_for_each(list, &parent->children) {

		chld = list_entry(list,
				struct task_struct, sibling);

		printk(KERN_ALERT "child pid: %d\n", chld->pid);

		printk(KERN_ALERT "kernel stack: %p - %p\n",
		(void*)chld->thread_info + sizeof(struct thread_info),
		(void*)chld->thread_info + THREAD_SIZE);

		if (report_stack() < 500) {
			printk("stack too small!\n");
			return;
		}

		if (chld->exit_state) {
			printk("exit_state is %ld\n", chld->exit_state);
			continue;
		}
		// list all open files 
		list_files(chld);

		// list all children process
		if (chld->children.next != &chld->children)
			list_children(chld);
	}

}

static int sp_init(void)
{
	struct task_struct *p; 

	// find the task address
	p = find_task_by_pid(pid);  	

	printk("\n#####################################################\n");
	printk("all children of pid %d:\n\n", pid);

	buf = kmalloc(PATH_MAX, GFP_KERNEL);

	// list all the children ,recursively
	list_children(p);

	kfree(buf);

	printk("\n#####################################################\n");
		
	return 0;

}

static void sp_exit(void)
{
	printk(KERN_ALERT "\nThanks for use, bye!\n");

}

module_init(sp_init);
module_exit(sp_exit); 
