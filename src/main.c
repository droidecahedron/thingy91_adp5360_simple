/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

#if defined(CONFIG_ADP536X)
#include <adp536x.h>
#endif

#define DEBUG_USE_SYSINIT false // Set to true if you want to use sys_init the way the atv2/thingy board inits do.

LOG_MODULE_REGISTER(thingy91_adp5360_main, LOG_LEVEL_ERR);
#define ADP536X_I2C_DEVICE DEVICE_DT_GET(DT_NODELABEL(i2c2))

static bool g_started_up = false; // don't feel like using any sync primitives.

// pmic init
static int power_mgmt_init(void)
{
	int err;

	err = adp536x_init(ADP536X_I2C_DEVICE);
	if (err)
	{
		LOG_ERR("ADP536X failed to initialize, error: %d\n", err);
		return err;
	}

	/* Sets the VBUS current limit to 500 mA. */
	err = adp536x_vbus_current_set(ADP536X_VBUS_ILIM_500mA);
	if (err)
	{
		LOG_ERR("Could not set VBUS current limit, error: %d\n", err);
		return err;
	}

	/* Sets the charging current to 320 mA. */
	err = adp536x_charger_current_set(ADP536X_CHG_CURRENT_320mA);
	if (err)
	{
		LOG_ERR("Could not set charging current, error: %d\n", err);
		return err;
	}

	/* Sets the charge current protection threshold to 400 mA. */
	err = adp536x_oc_chg_current_set(ADP536X_OC_CHG_THRESHOLD_400mA);
	if (err)
	{
		LOG_ERR("Could not set charge current protection, error: %d\n",
				err);
		return err;
	}

	err = adp536x_charging_enable(true);
	if (err)
	{
		LOG_ERR("Could not enable charging: %d\n", err);
		return err;
	}

	err = adp536x_fg_set_mode(ADP566X_FG_ENABLED, ADP566X_FG_MODE_SLEEP);
	if (err)
	{
		LOG_ERR("Could not enable fuel gauge: %d", err);
		return err;
	}

	return 0;
}

static int thingy91_board_init(void)
{
	int err;

	err = power_mgmt_init();
	if (err)
	{
		LOG_ERR("power_mgmt_init failed with error: %d", err);
		return err;
	}
	g_started_up = true;
	return 0;
}

int main(void)
{

	uint8_t battery_percentage;
	while(!g_started_up) // wait for g_started_up to go true, either by sys init or the startup thread.
	{
		k_yield();
	} 

	printk("Hello World! %s, battery percentage portions stripped out of atv2\n", CONFIG_BOARD);
	while (1)
	{
		adp536x_fg_soc(&battery_percentage);
		printk("Batt percentage as uint8 : %d\n", battery_percentage);
		k_msleep(5000);
	}
}

int startup_thread(void)
{
	printk("STARTUP Thread: Not using SYS_INIT for pmic inits\n");
	#if(!DEBUG_USE_SYSINIT)
		if(thingy91_board_init() != 0)
		printk("Could not initialize PMIC\n");
		return 0;
	#endif
}

// You can uncomment this if you want. I think it's odd to do this way.
#if(DEBUG_USE_SYSINIT)
SYS_INIT(thingy91_board_init, POST_KERNEL, CONFIG_BOARD_INIT_PRIORITY);
#else
#define STACKSIZE 1024
#define STARTUP_THREAD_PRIORITY -1
K_THREAD_DEFINE(startup_thread_id, STACKSIZE, startup_thread, NULL, NULL, NULL,
		STARTUP_THREAD_PRIORITY, 0, 0);
#endif