#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/errno.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training with parameter and list");
MODULE_LICENSE("Dual BSD/GPL");

// Параметр count зі значенням за умовчанням 1
static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of times to print 'Hello, world!'");

// Структура для збереження часу та зв'язку в списку
struct hello_data {
	struct list_head list;
	ktime_t time;
};

// Головний елемент списку
static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
	int i;

	// Перевірка значення параметра
	if (count == 0 || (count >= 5 && count <= 10)) {
		pr_warn("Warning: count is 0 or between 5 and 10\n");
	}

	if (count > 10) {
		pr_err("Error: count is greater than 10\n");
		return -EINVAL;
	}

	// Виведення повідомлення "Hello, world!" count разів
	for (i = 0; i < count; i++) {
		struct hello_data *data;

		// Виділення пам'яті для елемента списку
		data = kmalloc(sizeof(*data), GFP_KERNEL);
		if (!data)
			return -ENOMEM;

		// Збереження часу і додавання елемента до списку
		data->time = ktime_get();
		list_add_tail(&data->list, &hello_list);

		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_data *data, *tmp;

	// Прохід по списку і виведення часу
	list_for_each_entry_safe(data, tmp, &hello_list, list) {
		pr_info("Time: %llu ns\n", ktime_to_ns(data->time));

		// Видалення елемента зі списку і звільнення пам'яті
		list_del(&data->list);
		kfree(data);
	}
}

module_init(hello_init);
module_exit(hello_exit);
