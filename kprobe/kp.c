#include <linux/kprobes.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/string.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("hitmoon <xiaoqiang.zhao@i-soft.com.cn>");

#define NAME_LENGTH	32
static int time = 5;
static char *func = "";

module_param(time, int, 0);
module_param(func, charp, 0);


extern unsigned long kallsyms_lookup_name(const char *);

struct kprobe kp;
struct kretprobe krp;

struct timer_list timer;
static int call_count = 0;
unsigned long start, run_time = 0;

int work = 1;

//kprobe pre_handler function ,to number the call count and mark start time 
int in(struct kprobe *p, struct pt_regs *regs)
{
	if(work){
		call_count++;
		start = jiffies;
	}

	return 0;
}

//kretprobe handler function, update the execution time
void out(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	if(work) 
		run_time += jiffies - start;


}

// timer handler, to print the result
void timeout(unsigned long data)
{
	work = 0;	
	printk("function %s called %d times,total run time is %ld ms\n", func, call_count, run_time * 10);


}

static int kp_init(void)
{

	if (!strcmp(func, "")) {
		printk("empty function name!\n");
		return -1;
	}
	kp.addr = (void*)kallsyms_lookup_name(func);

	if (!kp.addr) {
		printk("could not find symbol name: %s\n", func);
		return -1;
	}

	kp.pre_handler = in;
	kp.post_handler = NULL;
	kp.fault_handler = NULL;


	krp.kp.addr = kp.addr;
	krp.handler = out;
	krp.maxactive = 1;

	init_timer(&timer);
	timer.expires = jiffies + time * HZ;
	timer.data = 0;
	timer.function = timeout;

	// register the probe
	register_kprobe(&kp);
	register_kretprobe(&krp);

	add_timer(&timer);

	return 0;
}

static void kp_exit(void)
{
	unregister_kprobe(&kp);
	unregister_kretprobe(&krp);
	printk("bye!\n");
}


module_init(kp_init);
module_exit(kp_exit);
