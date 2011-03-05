/*
 * linux/drivers/mmc/card/block-quirks.c
 *
 * Copyright (C) 2010 Andrei Warkentin <andreiw@xxxxxxxxxxxx>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/mmc/card.h>

#include "queue.h"
#include "blk.h"

#ifdef CONFIG_MMC_BLOCK_QUIRK_TOSHIBA_32NM
static int toshiba_32nm_probe(struct mmc_blk_data *md, struct mmc_card *card)
{
	printk(KERN_INFO "Applying Toshiba 32nm workarounds\n");

	/* Page size 8K, this card doesn't like unaligned writes
	   across 8K boundary. */
	md->write_align_size = 8192;

	/* Doing the alignment for accesses > 12K seems to
	   result in decreased perf. */
	md->write_align_limit = 12288;
	return 0;
}
#endif /* CONFIG_MMC_BLOCK_QUIRK_TOSHIBA_32NM */

/*
   Caveat: Because this list is just looked up with a linear
   search, take care that either overlapping revision ranges
   don't exist, or that returning the first quirk that matches
   a given revision has the desired effect.
*/
struct mmc_blk_quirk mmc_blk_quirks[] = {
#ifdef CONFIG_MMC_BLOCK_QUIRK_TOSHIBA_32NM
        MMC_BLK_QUIRK("MMC16G", 0x11, 0x0, toshiba_32nm_probe, NULL),
        MMC_BLK_QUIRK("MMC32G", 0x11, 0x0100, toshiba_32nm_probe, NULL),
#endif /* CONFIG_MMC_BLOCK_QUIRK_TOSHIBA_32NM */
};

static int mmc_blk_quirk_cmp(const struct mmc_blk_quirk *quirk,
			      const struct mmc_card *card)
{
	u64 rev = mmc_blk_qrev_card(card);

	if (quirk->manfid == card->cid.manfid &&
	    quirk->oemid == card->cid.oemid &&
	    !strcmp(quirk->name, card->cid.prod_name) &&
	    rev >= quirk->rev_start &&
	    rev <= quirk->rev_end)
		return 0;

	return -1;
}

struct mmc_blk_quirk *mmc_blk_quirk_find(struct mmc_card *card)
{
	unsigned index;

	for (index = 0; index < ARRAY_SIZE(mmc_blk_quirks); index++)
		if (!mmc_blk_quirk_cmp(&mmc_blk_quirks[index], card))
			return &mmc_blk_quirks[index];

	return NULL;
}

