// SPDX-License-Identifier: GPL-2.0-only

/**
 * Driver for ModalAI VOXL RC Input connector (voxl_brc_in)
 *
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/kernel.h>

#define DEV_NAME "voxl_rc_in"

static int major;
static struct class *gp_en_class;

static const struct file_operations voxl_rc_in_fops = {
	.owner		= THIS_MODULE,
};

static int voxl_rc_in_probe(struct platform_device *pdev)
{
	int ret;
	struct device *dev = NULL;
	struct device_node *np = pdev->dev.of_node;

	pr_warn(DEV_NAME " probe\n");
	major = register_chrdev(0, DEV_NAME, &voxl_rc_in_fops);
	if (major < 0) {
		pr_warn(DEV_NAME ": unable to get major: %d\n", major);
		return major;
	}

	gp_en_class = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(gp_en_class))
		return PTR_ERR(gp_en_class);
	
	dev = device_create(gp_en_class, NULL, MKDEV(major, 0), NULL, DEV_NAME);
	if (IS_ERR(dev)) {
		pr_err(DEV_NAME ": failed to create device: %d\n", dev);
		return PTR_ERR(dev);
	}

	pr_warn(DEV_NAME " probe complete");

	return 0;

error:
	return -EINVAL;
}

static int voxl_rc_in_remove(struct platform_device *pdev)
{
	pr_warn(DEV_NAME " remove\n");
	device_destroy(gp_en_class, MKDEV(major, 0));
	class_destroy(gp_en_class);
	unregister_chrdev(major, DEV_NAME);
	return 0;
}

static const struct of_device_id of_voxl_rc_in_dt_match[] = {
	{.compatible	= "modalai,voxl-rc-in"},
	{},
};

MODULE_DEVICE_TABLE(of, of_voxl_rc_in_dt_match);

static struct platform_driver voxl_rc_in_driver = {
	.probe	= voxl_rc_in_probe,
	.remove	= voxl_rc_in_remove,
	.driver	= {
		.name	= "voxl_rc_in",
		.of_match_table	= of_voxl_rc_in_dt_match,
	},
};

static int __init voxl_rc_in_init(void)
{
	pr_warn(DEV_NAME " init\n");
	return platform_driver_register(&voxl_rc_in_driver);
}

static void __exit voxl_rc_in_exit(void)
{
	pr_warn(DEV_NAME " exit\n");
	platform_driver_unregister(&voxl_rc_in_driver);
}

module_init(voxl_rc_in_init);
module_exit(voxl_rc_in_exit);

MODULE_AUTHOR("Travis Bottalico <travis@modalai.com>");
MODULE_DESCRIPTION("Driver to configure VOXL2 default GPIO state for bootup for RC input");
MODULE_LICENSE("GPL v2");
