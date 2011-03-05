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

/*
   Caveat: Because this list is just looked up with a linear
   search, take care that either overlapping revision ranges
   don't exist, or that returning the first quirk that matches
   a given revision has the desired effect.
*/
struct mmc_blk_quirk mmc_blk_quirks[] = {
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
