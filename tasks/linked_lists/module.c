#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>

#include "stack.h"
#include "assert.h"

static void __init test_stack(void)
{
    LIST_HEAD(data_stack);
    stack_entry_t *tos = NULL;
    const char *tos_data = NULL;
    const char* test_data[] = { "1", "2", "3", "4" };
    long i = 0;

    pr_alert("Testing basic stack");

    for (i = 0; i != ARRAY_SIZE(test_data); ++i) {
        stack_push(&data_stack,
            create_stack_entry((void*)test_data[i])
        );
    }

    for (i = ARRAY_SIZE(test_data) - 1; i >= 0; --i) {
        tos = stack_pop(&data_stack);
        tos_data = STACK_ENTRY_DATA(tos, const char*);
        delete_stack_entry(tos);
        printk(KERN_ALERT "%s == %s\n", tos_data, test_data[i]);
        assert(!strcmp(tos_data, test_data[i]));
    }

    assert(stack_empty(&data_stack));
}

static int __init print_processes_backwards(void)
{
    LIST_HEAD(data_stack);
    int result = 0; // OK

    stack_entry_t *task_entry = NULL;
    struct task_struct *task = NULL;
    char *task_name = NULL;

    for_each_process (task) {
        task_name = kmalloc(TASK_COMM_LEN, GFP_KERNEL);
        
        if (task_name) {
            task_entry = create_stack_entry(task_name);
            if (task_entry) {
                task_name = get_task_comm(task_name, task);
                stack_push(&data_stack, task_entry);
            } else {
                kfree(task_name);
                result = -ENOMEM;
                break;
            }
        } else {
            result = -ENOMEM;
            break;
        }
    }

    while (!stack_empty(&data_stack)) {
        task_entry = stack_pop(&data_stack);
        task_name = STACK_ENTRY_DATA(task_entry, char*);
        delete_stack_entry(task_entry);
        
        if (result != -ENOMEM) {
            printk(KERN_ALERT "task name = %s\n", task_name);
        }
        kfree(task_name);
    }

    return result;
}

static int __init ll_init(void)
{
    printk(KERN_ALERT "Hello, linked_lists\n");
    test_stack();
    print_processes_backwards();
    return 0;
}

static void __exit ll_exit(void)
{
    printk(KERN_ALERT "Goodbye, linked_lists!\n");
}

module_init(ll_init);
module_exit(ll_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linked list exercise module");
MODULE_AUTHOR("Kernel hacker!");
