#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x92997ed8, "_printk" },
	{ 0xc832a04c, "gpio_to_desc" },
	{ 0x66b22ac, "gpiod_get_raw_value" },
	{ 0x9166fada, "strncpy" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x754d539c, "strlen" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xeb6ba750, "gpiod_direction_output_raw" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0xfe990052, "gpio_free" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x3f1584d8, "proc_create" },
	{ 0x999e8297, "vfree" },
	{ 0x2b68bd2f, "del_timer" },
	{ 0xe3013a81, "gpiod_set_raw_value" },
	{ 0x5d1d6f6f, "remove_proc_entry" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6C52E5FE143A26EABA7A761");
