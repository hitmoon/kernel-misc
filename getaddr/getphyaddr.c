
#include <asm/page.h>
#include <asm/pgtable.h>
#include <linux/sched.h>

asmlinkage void* sys_getphyaddr(pid_t pid, void *p)
{


	struct task_struct *s;
	unsigned long addr = (long)p;
	// we use 46 bit physical address
	unsigned long phy_mask = 0x3ffffffff000;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	
	printk("hello, getphyaddr system call is called!\n");
	s = find_task_by_pid(pid);
	if (!s)
		return NULL;
	if (s->exit_state)
		return NULL;
	if (s->mm) {
		pgd = pgd_offset(s->mm,addr);
		if (!pgd)
			return NULL;
		pud = pud_offset(pgd,addr);
		if (!pud->pud) {
			printk("pud is empty!\n");
			return NULL;
		}

		pmd = pmd_offset(pud,addr);
		if (!pmd->pmd){
			printk("pmd is empty!\n");
			return NULL;
		}
		pte = pte_offset_map(pmd,addr);
		if (!pte->pte){ 
			printk("pte is empty!\n");
			return NULL;
		}

		if (pte_present(*pte))  
			return  (void*)((pte->pte & phy_mask) + (addr & 0xfff));
		else {
			printk("page not in RAM!\n");
			return NULL;
		}
			
	}
	return NULL;
}

