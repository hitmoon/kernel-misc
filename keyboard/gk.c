#include <linux/module.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/delay.h>


extern struct platform_device *i8042_platform_device;
extern irqreturn_t i8042_interrupt(int, void *, struct pt_regs *);

#define I8042_KBD_IRQ	1
#define I8042_AUX_IRQ	12

static DEFINE_SPINLOCK(i8042_lock);

MODULE_AUTHOR("hitmoon <xiaoqiang.zhao@i-soft.com.cn>");
MODULE_LICENSE("GPL");

static irqreturn_t i8042_interrupt_gk(int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long flags;
	unsigned char str, data;
	int ret;

	spin_lock_irqsave(&i8042_lock, flags);
	str = inb(0x64);
	if (unlikely(~str & 0x01)) {
		spin_unlock_irqrestore(&i8042_lock, flags);
		ret = 1;
		goto out;
	}

	data = inb(0x60);

	printk(KERN_ALERT "key %s,scan code: %02x\n",(data & 0x80) ? "released" : "pressed",data);

	spin_unlock_irqrestore(&i8042_lock, flags);

	ret = 0;
out:
	return IRQ_RETVAL(ret);
}


static int init_gk(void)
{

	printk(KERN_ALERT "uninstall old handler!\n");
	free_irq(I8042_KBD_IRQ, i8042_platform_device);

	printk(KERN_ALERT "install my new handler!\n");
	request_irq(I8042_KBD_IRQ,i8042_interrupt_gk, IRQF_SHARED, "i8042", i8042_platform_device);


	return 0;
}

static void exit_gk(void)
{
	printk(KERN_ALERT "uninstall new  handler!\n");
	free_irq(I8042_KBD_IRQ, i8042_platform_device);

	printk(KERN_ALERT "install the old handler back!\n");
	printk(KERN_ALERT "Bye!\n");
	request_irq(I8042_KBD_IRQ,i8042_interrupt, IRQF_SHARED, "i8042", i8042_platform_device);

}


module_init(init_gk);
module_exit(exit_gk);

