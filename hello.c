#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Klymenko");
MODULE_DESCRIPTION("Lab 4: Hello World Kernel Module with list and param");

static unsigned int count = 1;
module_param(count, uint, 0644);
MODULE_PARM_DESC(count, "A uint parameter defining how many times to print 'Hello, world!'");

struct hello_event {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(event_list_head);

static int __init hello_init(void)
{
	int i;

	if (count == 0 || (count >= 5 && count <= 10)) {
		pr_warn("HelloModule: Warning, count is %u (0 or between 5 and 10)!\n", count);
	} else if (count > 10) {
		pr_err("HelloModule: Error, count %u is greater than 10!\n", count);
		return -EINVAL;
	}

	for (i = 0; i < count; i++) {
		struct hello_event *event = kmalloc(sizeof(*event), GFP_KERNEL);

		if (!event)
			return -ENOMEM;

		event->time = ktime_get();
		list_add_tail(&event->list, &event_list_head);

		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *event, *tmp;

	list_for_each_entry_safe(event, tmp, &event_list_head, list) {
		pr_info("HelloModule: Event time = %lld ns\n", ktime_to_ns(event->time));
		list_del(&event->list);
		kfree(event);
	}

	pr_info("HelloModule: Unloaded successfully\n");
}

module_init(hello_init);
module_exit(hello_exit);
