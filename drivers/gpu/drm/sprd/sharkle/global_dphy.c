/*
 * Copyright (C) 2019 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "sprd_dphy.h"

struct dphy_glb_context {
	unsigned int ctrl_reg;
	unsigned int ctrl_mask;
	struct regmap *regmap;
} le_ctx_enable, le_ctx_power;

static int dphy_glb_parse_dt(struct dphy_context *ctx,
				struct device_node *np)
{
	unsigned int syscon_args[2];
	int ret;

	le_ctx_enable.regmap = syscon_regmap_lookup_by_name(np, "enable");
	if (IS_ERR(le_ctx_enable.regmap)) {
		pr_warn("failed to map dphy glb reg: enable\n");
		return PTR_ERR(le_ctx_enable.regmap);
	}

	ret = syscon_get_args_by_name(np, "enable", 2, syscon_args);
	if (ret == 2) {
		le_ctx_enable.ctrl_reg = syscon_args[0];
		le_ctx_enable.ctrl_mask = syscon_args[1];
	} else
		pr_warn("failed to parse dphy glb reg: enable\n");

	le_ctx_power.regmap = syscon_regmap_lookup_by_name(np, "power");
	if (IS_ERR(le_ctx_power.regmap)) {
		pr_warn("failed to map dphy glb reg: power\n");
		return PTR_ERR(le_ctx_power.regmap);
	}

	ret = syscon_get_args_by_name(np, "power", 2, syscon_args);
	if (ret == 2) {
		le_ctx_power.ctrl_reg = syscon_args[0];
		le_ctx_power.ctrl_mask = syscon_args[1];
	} else
		pr_warn("failed to parse dphy glb reg: power");

	return 0;
}

static void dphy_glb_enable(struct dphy_context *ctx)
{
	regmap_update_bits(le_ctx_enable.regmap,
		le_ctx_enable.ctrl_reg,
		le_ctx_enable.ctrl_mask,
		le_ctx_enable.ctrl_mask);
}

static void dphy_glb_disable(struct dphy_context *ctx)
{
	regmap_update_bits(le_ctx_enable.regmap,
		le_ctx_enable.ctrl_reg,
		le_ctx_enable.ctrl_mask,
		(unsigned int)(~le_ctx_enable.ctrl_mask));
}

void cali_dphy_glb_disable(struct dphy_context *ctx)
{
	regmap_update_bits(le_ctx_enable.regmap,
		le_ctx_enable.ctrl_reg,
		le_ctx_enable.ctrl_mask,
		le_ctx_enable.ctrl_mask);

	regmap_update_bits(le_ctx_enable.regmap,
		le_ctx_enable.ctrl_reg,
		le_ctx_enable.ctrl_mask,
		(unsigned int)(~le_ctx_enable.ctrl_mask));
}

void cali_dphy_power_domain(struct dphy_context *ctx, int enable)
{
	void __iomem *base1;
	void __iomem *base2;
	unsigned int temp;
	unsigned int temp2;

	if (enable) {
		regmap_update_bits(le_ctx_power.regmap,
			le_ctx_power.ctrl_reg,
			le_ctx_power.ctrl_mask,
			(unsigned int)(~le_ctx_power.ctrl_mask));

		/* Dphy has a random wakeup failed after poweron,
		 * this will caused testclr reset failed and
		 * writing pll configuration parameter failed.
		 * Delay 100us after dphy poweron, waiting for pll is stable.
		 */
		udelay(100);
	} else {
		regmap_update_bits(le_ctx_power.regmap,
			le_ctx_power.ctrl_reg,
			le_ctx_power.ctrl_mask,
			le_ctx_power.ctrl_mask);
		base1 = ioremap_nocache(0x402e0024, 0x10000);
		temp = readl_relaxed(base1);
		temp |= (BIT(15) | BIT(14));
		writel_relaxed(temp, base1);

		base2 = ioremap_nocache(0x20e00000, 0x10000);
		temp2 = readl_relaxed(base2);
		temp2 &= ~BIT(0);
		writel_relaxed(temp2, base2);
	}
}

static void dphy_power_domain(struct dphy_context *ctx, int enable)
{
	if (enable) {
		regmap_update_bits(le_ctx_power.regmap,
			le_ctx_power.ctrl_reg,
			le_ctx_power.ctrl_mask,
			(unsigned int)(~le_ctx_power.ctrl_mask));

		/* Dphy has a random wakeup failed after poweron,
		 * this will caused testclr reset failed and
		 * writing pll configuration parameter failed.
		 * Delay 100us after dphy poweron, waiting for pll is stable.
		 */
		udelay(100);
	} else {
		regmap_update_bits(le_ctx_power.regmap,
			le_ctx_power.ctrl_reg,
			le_ctx_power.ctrl_mask,
			le_ctx_power.ctrl_mask);
	}
}

static struct dphy_glb_ops dphy_glb_ops = {
	.parse_dt = dphy_glb_parse_dt,
	.enable = dphy_glb_enable,
	.disable = dphy_glb_disable,
	.power = dphy_power_domain,
};

static struct ops_entry entry = {
	.ver = "sharkle",
	.ops = &dphy_glb_ops,
};

static int __init dphy_glb_register(void)
{
	return dphy_glb_ops_register(&entry);
}

subsys_initcall(dphy_glb_register);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("junxiao.feng@unisoc.com");
MODULE_DESCRIPTION("sprd sharkle dphy global AHB&APB regs low-level config");
