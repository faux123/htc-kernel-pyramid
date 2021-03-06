/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <linux/delay.h>
#include <linux/pwm.h>
#include <mach/gpio.h>
#include "msm_fb.h"
#include <linux/spi/spi.h>
#include <linux/delay.h>

#define LCDC_SHARP_SPI_DEVICE_NAME	"spi_panel"
struct mutex lock;
struct spi_device *lcdc_spi_client;

struct auo_spi_data {
	u8 addr;
	u8 data;
	u8 delay;
};

static struct auo_spi_data init_sequence[] = {
	{0x1, 0xC0, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x86, 0},
	{0x1, 0xC0, 0},
	{0x0, 0x1 , 0},
	{0x2, 0x0 , 0},
	{0x1, 0xC0, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x86, 0},
	{0x1, 0xC0, 0},
	{0x0, 0x3 , 0},
	{0x2, 0x0 , 0},
	{0x1, 0xC1, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x40, 0},
	{0x1, 0xC2, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x2, 1},
	{0x1, 0xC2, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x32, 0},

	//Gamma setting	-start
	{0x1, 0xE0, 0},
	{0x0, 0x0 , 0},
	{0x2, 0xE , 0},
	{0x1, 0xE0, 0},
	{0x0, 0x1 , 0},
	{0x2, 0x34, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x3F, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x3 , 0},
	{0x2, 0x49, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x4 , 0},
	{0x2, 0x1D, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x5 , 0},
	{0x2, 0x2C, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x6 , 0},
	{0x2, 0x5F, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x7 , 0},
	{0x2, 0x3A, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x8 , 0},
	{0x2, 0x20, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x9 , 0},
	{0x2, 0x28, 0},
	{0x1, 0xE0, 0},
	{0x0, 0xA , 0},
	{0x2, 0x80, 0},
	{0x1, 0xE0, 0},
	{0x0, 0xB , 0},
	{0x2, 0x13, 0},
	{0x1, 0xE0, 0},
	{0x0, 0xC , 0},
	{0x2, 0x32, 0},
	{0x1, 0xE0, 0},
	{0x0, 0xD , 0},
	{0x2, 0x56, 0},
	{0x1, 0xE0, 0},
	{0x0, 0xE , 0},
	{0x2, 0x79, 0},
	{0x1, 0xE0, 0},
	{0x0, 0xF , 0},
	{0x2, 0xB8, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x10, 0},
	{0x2, 0x55, 0},
	{0x1, 0xE0, 0},
	{0x0, 0x11, 0},
	{0x2, 0x57, 0},

	{0x1, 0xE1, 0},
	{0x0, 0x0 , 0},
	{0x2, 0xE , 0},
	{0x1, 0xE1, 0},
	{0x0, 0x1 , 0},
	{0x2, 0x34, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x3F, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x3 , 0},
	{0x2, 0x49, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x4 , 0},
	{0x2, 0x1D, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x5 , 0},
	{0x2, 0x2C, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x6 , 0},
	{0x2, 0x5F, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x7 , 0},
	{0x2, 0x3A, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x8 , 0},
	{0x2, 0x20, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x9 , 0},
	{0x2, 0x28, 0},
	{0x1, 0xE1, 0},
	{0x0, 0xA , 0},
	{0x2, 0x80, 0},
	{0x1, 0xE1, 0},
	{0x0, 0xB , 0},
	{0x2, 0x13, 0},
	{0x1, 0xE1, 0},
	{0x0, 0xC , 0},
	{0x2, 0x32, 0},
	{0x1, 0xE1, 0},
	{0x0, 0xD , 0},
	{0x2, 0x56, 0},
	{0x1, 0xE1, 0},
	{0x0, 0xE , 0},
	{0x2, 0x79, 0},
	{0x1, 0xE1, 0},
	{0x0, 0xF , 0},
	{0x2, 0xB8, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x10 ,0},
	{0x2, 0x55, 0},
	{0x1, 0xE1, 0},
	{0x0, 0x11 ,0},
	{0x2, 0x57, 0},

	{0x1, 0xE2, 0},
	{0x0, 0x0 , 0},
	{0x2, 0xE , 0},
	{0x1, 0xE2, 0},
	{0x0, 0x1 , 0},
	{0x2, 0x34, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x3F, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x3 , 0},
	{0x2, 0x49, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x4 , 0},
	{0x2, 0x1D, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x5 , 0},
	{0x2, 0x2C, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x6 , 0},
	{0x2, 0x5D, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x7 , 0},
	{0x2, 0x3A, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x8 , 0},
	{0x2, 0x20, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x9 , 0},
	{0x2, 0x28, 0},
	{0x1, 0xE2, 0},
	{0x0, 0xA , 0},
	{0x2, 0x80, 0},
	{0x1, 0xE2, 0},
	{0x0, 0xB , 0},
	{0x2, 0x13, 0},
	{0x1, 0xE2, 0},
	{0x0, 0xC , 0},
	{0x2, 0x32, 0},
	{0x1, 0xE2, 0},
	{0x0, 0xD , 0},
	{0x2, 0x56, 0},
	{0x1, 0xE2, 0},
	{0x0, 0xE , 0},
	{0x2, 0x79, 0},
	{0x1, 0xE2, 0},
	{0x0, 0xF , 0},
	{0x2, 0xB8, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x10 ,0},
	{0x2, 0x55, 0},
	{0x1, 0xE2, 0},
	{0x0, 0x11 ,0},
	{0x2, 0x57, 0},

	{0x1, 0xE3, 0},
	{0x0, 0x0 , 0},
	{0x2, 0xE , 0},
	{0x1, 0xE3, 0},
	{0x0, 0x1 , 0},
	{0x2, 0x34, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x3F, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x3 , 0},
	{0x2, 0x49, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x4 , 0},
	{0x2, 0x1D, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x5 , 0},
	{0x2, 0x2C, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x6 , 0},
	{0x2, 0x5F, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x7 , 0},
	{0x2, 0x3A, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x8 , 0},
	{0x2, 0x20, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x9 , 0},
	{0x2, 0x28, 0},
	{0x1, 0xE3, 0},
	{0x0, 0xA , 0},
	{0x2, 0x80, 0},
	{0x1, 0xE3, 0},
	{0x0, 0xB , 0},
	{0x2, 0x13, 0},
	{0x1, 0xE3, 0},
	{0x0, 0xC , 0},
	{0x2, 0x32, 0},
	{0x1, 0xE3, 0},
	{0x0, 0xD , 0},
	{0x2, 0x56, 0},
	{0x1, 0xE3, 0},
	{0x0, 0xE , 0},
	{0x2, 0x79, 0},
	{0x1, 0xE3, 0},
	{0x0, 0xF , 0},
	{0x2, 0xB8, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x10, 0},
	{0x2, 0x55, 0},
	{0x1, 0xE3, 0},
	{0x0, 0x11 ,0},
	{0x2, 0x57, 0},

	{0x1, 0xE4, 0},
	{0x0, 0x0 , 0},
	{0x2, 0xE , 0},
	{0x1, 0xE4, 0},
	{0x0, 0x1 , 0},
	{0x2, 0x34, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x3F, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x3 , 0},
	{0x2, 0x49, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x4 , 0},
	{0x2, 0x1D, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x5 , 0},
	{0x2, 0x2C, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x6 , 0},
	{0x2, 0x5F, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x7 , 0},
	{0x2, 0x3A, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x8 , 0},
	{0x2, 0x20, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x9 , 0},
	{0x2, 0x28, 0},
	{0x1, 0xE4, 0},
	{0x0, 0xA , 0},
	{0x2, 0x80, 0},
	{0x1, 0xE4, 0},
	{0x0, 0xB , 0},
	{0x2, 0x13, 0},
	{0x1, 0xE4, 0},
	{0x0, 0xC , 0},
	{0x2, 0x32, 0},
	{0x1, 0xE4, 0},
	{0x0, 0xD , 0},
	{0x2, 0x56, 0},
	{0x1, 0xE4, 0},
	{0x0, 0xE , 0},
	{0x2, 0x79, 0},
	{0x1, 0xE4, 0},
	{0x0, 0xF , 0},
	{0x2, 0xB8, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x10 ,0},
	{0x2, 0x55, 0},
	{0x1, 0xE4, 0},
	{0x0, 0x11 ,0},
	{0x2, 0x57, 0},

	{0x1, 0xE5, 0},
	{0x0, 0x0 , 0},
	{0x2, 0xE , 0},
	{0x1, 0xE5, 0},
	{0x0, 0x1 , 0},
	{0x2, 0x34, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x2 , 0},
	{0x2, 0x3F, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x3 , 0},
	{0x2, 0x49, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x4 , 0},
	{0x2, 0x1D, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x5 , 0},
	{0x2, 0x2C, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x6 , 0},
	{0x2, 0x5F, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x7 , 0},
	{0x2, 0x3A, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x8 , 0},
	{0x2, 0x20, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x9 , 0},
	{0x2, 0x28, 0},
	{0x1, 0xE5, 0},
	{0x0, 0xA , 0},
	{0x2, 0x80, 0},
	{0x1, 0xE5, 0},
	{0x0, 0xB , 0},
	{0x2, 0x13, 0},
	{0x1, 0xE5, 0},
	{0x0, 0xC , 0},
	{0x2, 0x32, 0},
	{0x1, 0xE5, 0},
	{0x0, 0xD , 0},
	{0x2, 0x56, 0},
	{0x1, 0xE5, 0},
	{0x0, 0xE , 0},
	{0x2, 0x79, 0},
	{0x1, 0xE5, 0},
	{0x0, 0xF , 0},
	{0x2, 0xB8, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x10 ,0},
	{0x2, 0x55, 0},
	{0x1, 0xE5, 0},
	{0x0, 0x11 ,0},
	{0x2, 0x57, 0},
	//Gamma setting	-done

	{0x1, 0x3A, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x70, 0},	//RGB888
	{0x1, 0x11, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x0 , 120},
	{0x1, 0x35, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x0 , 0},
	{0x1, 0x29, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x0 , 0},
	{0x1, 0x53, 0},
	{0x0, 0x0 , 0},
	{0x2, 0x24, 0},
	{0x1, 0x51, 0},
	{0x0, 0x0 , 0},
	{0x2, 0xFF, 0},
};
struct lcdc_auo_data {
	struct msm_panel_common_pdata *pdata;
	int vga_enabled;
	struct platform_device *fbpdev;
};

static struct lcdc_auo_data *dd;

static int lcdc_spi_write(unsigned char addr, unsigned char data)
{
	unsigned char buffer[2];
	int rc;

	if (!lcdc_spi_client)
		return 0;

	lcdc_spi_client->bits_per_word = 16;
	buffer[0] = addr << 5;
	buffer[1] = data;
	rc = spi_write(lcdc_spi_client, buffer, 2);

	return 1;
}

static int lcdc_auo_panel_on(struct platform_device *pdev)
{
	int i;
	static int init;

	printk("%s\n", __func__);
	if (init == 0) {
		init = 1;
		goto end;
	} else {
		mutex_lock(&lock);
		for (i = 0; i < ARRAY_SIZE(init_sequence); i++) {
			if (init_sequence[i].delay)
				mdelay(init_sequence[i].delay);
			else
				lcdc_spi_write(init_sequence[i].addr, init_sequence[i].data);
		}
		mutex_unlock(&lock);
	}
end:
	return 0;
}

static int lcdc_auo_panel_off(struct platform_device *pdev)
{
	printk("%s\n", __func__);
	mutex_lock(&lock);
	lcdc_spi_write(0x1, 0x28);
	lcdc_spi_write(0x0, 0x00);
	lcdc_spi_write(0x2, 0x00);
	lcdc_spi_write(0x1, 0x10);
	lcdc_spi_write(0x0, 0x00);
	lcdc_spi_write(0x2, 0x00);
	mutex_unlock(&lock);
	return 0;
}

static void lcdc_auo_panel_set_backlight(struct msm_fb_data_type *mfd)
{
	int bl_level;
	pr_info("%s+:bl=%d\n", __func__, mfd->bl_level);

	bl_level = mfd->bl_level;
	if (dd->pdata && dd->pdata->pmic_backlight && dd->pdata->shrink_pwm)
		dd->pdata->pmic_backlight(dd->pdata->shrink_pwm(mfd->bl_level));
}

static ssize_t show_vga_enable(struct device *device,
			       struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", dd->vga_enabled);
}

static ssize_t store_vga_enable(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	unsigned long enable;
	int rc;

	rc = strict_strtoul(buf, 10, &enable);
	if (rc)
		return -EINVAL;

	if (dd->pdata && dd->pdata->vga_switch)
		rc = dd->pdata->vga_switch(enable);
	else
		rc = -ENODEV;
	if (!rc) {
		dd->vga_enabled = enable;
		rc = count;
	}
	return rc;
}

static DEVICE_ATTR(vga_enable, S_IRUGO|S_IWUSR, show_vga_enable,
		   store_vga_enable);
static struct attribute *attrs[] = {
	&dev_attr_vga_enable.attr,
	NULL,
};
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __devinit auo_probe(struct platform_device *pdev)
{
	int rc = 0;
	struct msm_fb_data_type *mfd;
	printk("%s\n", __func__);

	if (pdev->id == 0) {
		dd = kzalloc(sizeof *dd, GFP_KERNEL);
		if (!dd)
			return -ENOMEM;
		dd->vga_enabled = 0;
		dd->pdata = pdev->dev.platform_data;
		return 0;
	} else if (!dd)
		return -ENODEV;

	dd->fbpdev = msm_fb_add_device(pdev);
	if (!dd->fbpdev) {
		dev_err(&pdev->dev, "failed to add msm_fb device\n");
		rc = -ENODEV;
		goto probe_exit;
	}

	mfd = platform_get_drvdata(dd->fbpdev);
	if (mfd && mfd->fbi && mfd->fbi->dev) {
		rc = sysfs_create_group(&mfd->fbi->dev->kobj, &attr_group);
		if (rc)
			dev_err(&pdev->dev, "failed to create sysfs group\n");
	} else {
		dev_err(&pdev->dev, "no dev to create sysfs group\n");
		rc = -ENODEV;
	}

probe_exit:
	return rc;
}

static int __devexit auo_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&dd->fbpdev->dev.kobj, &attr_group);
	return 0;
}
static int __devinit lcdc_auo_spi_probe(struct spi_device *spi)
{
	printk("%s\n", __func__);
	lcdc_spi_client = spi;
	return 0;
}
static int __devexit lcdc_auo_spi_remove(struct spi_device *spi)
{
	lcdc_spi_client = NULL;
	return 0;
}

static struct spi_driver lcdc_auo_spi_driver = {
	.driver = {
		.name  = "spi_panel",
		.owner = THIS_MODULE,
	},
	.probe         = lcdc_auo_spi_probe,
	.remove        = __devexit_p(lcdc_auo_spi_remove),
};

static struct platform_driver this_driver = {
	.probe  = auo_probe,
	.remove = auo_remove,
	.driver = {
		.name   = "lcdc_auo_wvga",
	},
};

static struct msm_fb_panel_data auo_panel_data = {
	.on = lcdc_auo_panel_on,
	.off = lcdc_auo_panel_off,
	.set_backlight = lcdc_auo_panel_set_backlight,
};

static struct platform_device this_device = {
	.name   = "lcdc_auo_wvga",
	.id	= 1,
	.dev	= {
		.platform_data = &auo_panel_data,
	}
};

static int __init lcdc_auo_panel_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;
	printk("%s\n", __func__);

#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("lcdc_auo_wvga"))
		return 0;
#endif
	printk("%s\n", __func__);

	mutex_init(&lock);

	ret = platform_driver_register(&this_driver);
	if (ret)
		return ret;

	pinfo = &auo_panel_data.panel_info;
	pinfo->xres = 480;
	pinfo->yres = 800;
	MSM_FB_SINGLE_MODE_PANEL(pinfo);
	pinfo->type = LCDC_PANEL;
	pinfo->pdest = DISPLAY_1;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;
	pinfo->fb_num = 2;
	pinfo->clk_rate = 24576000;
	pinfo->bl_max = 255;
	pinfo->bl_min = 1;

	pinfo->lcdc.h_back_porch = 4;
	pinfo->lcdc.h_front_porch = 4;
	pinfo->lcdc.h_pulse_width = 4;
	pinfo->lcdc.v_back_porch = 6;
	pinfo->lcdc.v_front_porch = 4;
	pinfo->lcdc.v_pulse_width = 4;
	pinfo->lcdc.border_clr = 0;
	pinfo->lcdc.underflow_clr = 0xff;
	pinfo->lcdc.hsync_skew = 0;

	ret = platform_device_register(&this_device);
	if (ret)
		platform_driver_unregister(&this_driver);

	ret = spi_register_driver(&lcdc_auo_spi_driver);

	if (ret) {
		printk(KERN_ERR "%s not able to register spi\n", __func__);
		platform_device_unregister(&this_device);
	}
	return ret;
}

module_init(lcdc_auo_panel_init);
