#include <linux/kernel.h>
#include <linux/sched.h> 
#include <linux/module.h>
#include <linux/list.h>

asmlinkage long sys_hello(void) 
{
 printk("Hello, World!\n");
 return 0;
}

asmlinkage long sys_set_weight(int weight)
{
    if (weight >= 0)
    {
        current->weight = weight;
        return 0;
    }
    return -EINVAL;
}

asmlinkage long sys_get_weight(void)
{
    return current->weight;
}

asmlinkage long sys_get_ancestor_sum(void)
{
    long weight_sum = 0;
    struct task_struct* curr_task = current;
    while (curr_task->pid != 1)
    {
        weight_sum += curr_task->weight;
        curr_task = curr_task->real_parent;
    }
    if (curr_task->pid == 1) // which it should be!
    {
        weight_sum += curr_task->weight;
    }
    
    return weight_sum;
}

void get_heaviest_descendant_aux(struct task_struct* tsk, long* heaviest_weight, long* heaviest_weight_pid)
{
    struct task_struct* curr_task;
    struct list_head* curr_list;
    if (list_empty( &(tsk->children) ))
    {
        if (tsk->weight > *heaviest_weight)
        {
            *heaviest_weight = tsk->weight;
            *heaviest_weight_pid = tsk->pid;    
        }
        else if ( (tsk->weight == *heaviest_weight) && (*heaviest_weight_pid  > tsk->pid) )
        {
            *heaviest_weight_pid = tsk->pid;
        }
        return;
    }
    else
    {
        list_for_each(curr_list, &(tsk->children) ){
            /*
                list_entry() is a macro that returns a pointer of (type = struct task_struct)
                of the (member = sibling) inside the argument (ptr = curr_list)
            */
            curr_task = list_entry(curr_list,struct task_struct,sibling);
            if (curr_task->weight > *heaviest_weight)
            {
                *heaviest_weight = curr_task->weight;
                *heaviest_weight_pid = curr_task->pid;    
            }
            else if ( (curr_task->weight == *heaviest_weight) && (*heaviest_weight_pid  > curr_task->pid) )
            {
                *heaviest_weight_pid = curr_task->pid;
            }  
            get_heaviest_descendant_aux(curr_task, heaviest_weight, heaviest_weight_pid);
        }
    }
}

asmlinkage long sys_get_heaviest_descendant(void)
{
    long heaviest_weight = 0;
    long heaviest_weight_pid = 0;
    struct task_struct* curr_task = current;
    if (list_empty( &(curr_task->children) ))
    {
        return -ECHILD;
    }
    get_heaviest_descendant_aux(curr_task, &heaviest_weight, &heaviest_weight_pid);
    return heaviest_weight_pid;
}
