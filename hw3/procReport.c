// https://github.com/coryd343/Page_Table_Walker/blob/master/procReport.c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <asm/pgtable.h>

static void print_phys_addr(void);
static unsigned long virt2phys(struct mm_struct *, unsigned long);

const pid_t PID = 1668;
const unsigned long addr[6] = {0x5631c25f8918, 0x5631c27f9010, 0x5631c27f902c, 0x5631c38a9260, 0x7ffeb6b86aac, 0x7fa847b61000};
// const pid_t PID = 31472;
// const unsigned long addr[6] = {0x55ea9dd2a918, 0x55ea9df2b010, 0x55ea9df2b02c, 0x55ea9e31f260, 0x7ffc1451bbac, 0x7f090ba9c000};
const char *str[6] = {"read-only", "data", "bss", "heap", "stack", "shared library"};

int proc_init (void) {
	printk(KERN_INFO "proc_report: kernel module initialized\n");
	printk(KERN_INFO "proc_id,proc_name,contig_pages,noncontig_pages,total_pages\n");
	print_phys_addr();
	return 0;
}

void proc_cleanup(void) {
	printk(KERN_INFO "proc_report: performing cleanup of module\n");
}

static void print_phys_addr() {
		struct task_struct *task;
	for_each_process(task)
		if(task->pid == PID) break;

		struct vm_area_struct *vma = 0;
		unsigned long vpage, p_addr;
	int i;

		if(task->mm && task->mm->mmap)
				for(vma = task->mm->mmap; vma; vma = vma->vm_next)
						for(vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE) 
				for(i = 0; i < 6; i++) 
					if(vpage <= addr[i] && addr[i] < vpage + PAGE_SIZE) {
						p_addr = virt2phys(task->mm, vpage);
						printk(KERN_INFO "[%s] vaddr: 0x%lx, paddr: 0x%lx\n", str[i], vpage, p_addr);
					}
}

static unsigned long virt2phys(struct mm_struct *mm, unsigned long vpage) {
		pgd_t *pgd;
		p4d_t *p4d;
		pud_t *pud;
		pmd_t *pmd;
		pte_t *pte;
		unsigned long physical_page_addr;
		struct page *page;
		pgd = pgd_offset(mm, vpage);
		if (pgd_none(*pgd) || pgd_bad(*pgd))
				return 0;
		p4d = p4d_offset(pgd, vpage);
		if (p4d_none(*p4d) || p4d_bad(*p4d))
				return 0;
		pud = pud_offset(p4d, vpage);
		if (pud_none(*pud) || pud_bad(*pud))
				return 0;
		pmd = pmd_offset(pud, vpage);
		if (pmd_none(*pmd) || pmd_bad(*pmd))
				return 0;
		if (!(pte = pte_offset_map(pmd, vpage)))
				return 0;
		if (!(page = pte_page(*pte)))
				return 0;
		physical_page_addr = page_to_phys(page);
		pte_unmap(pte);
		return physical_page_addr;
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
