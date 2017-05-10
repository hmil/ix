/*
 * Copyright 2017 Ecole Polytechnique Federale Lausanne (EPFL)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * nvmedev.c - nvme device support
 */

#include <ix/cfg.h>
#include <ix/errno.h>
#include <ix/log.h>
#include <ix/spdk.h>

#include <spdk/nvme.h>

#define spdk_pci_device rte_pci_device

struct nvme_request * alloc_local_nvme_request(struct nvme_request **req)
{
	log_err("Don't know what to do here \n");
	assert(NULL);
	return NULL;
}

void free_local_nvme_request(struct nvme_request *req)
{
	log_err("Don't know what to do here either\n");
	assert(NULL);
}

static bool
probe_cb(void *cb_ctx, struct spdk_pci_device *dev, struct spdk_nvme_ctrlr_opts *opts)
{
	int i;

	log_info("probe return\n");

	// Check that the config accepts this device
	for (i = 0; i < CFG.num_nvmedev; i++) {
		struct pci_addr cfg_addr = CFG.nvmedev[i];
		if (	cfg_addr.domain == dev->addr.domain &&
				cfg_addr.bus == dev->addr.bus &&
				cfg_addr.slot == dev->addr.devid &&
				cfg_addr.func == dev->addr.function) {
			log_info("attaching to nvme device\n");
			// As a first test, we want to see if we can get here
			// return true;
			return false;
		 }
	}

	log_info("Found NVME device at addr %04x:%02x:%02x.%d but ignoring it as it does not appear in the config.\n",
			dev->addr.domain, dev->addr.bus,
		 	dev->addr.devid, dev->addr.function);
	return false;
}

static void
attach_cb(void *cb_ctx, struct spdk_pci_device *dev, struct spdk_nvme_ctrlr *ctrlr,
	  const struct spdk_nvme_ctrlr_opts *opts)
{
	/*unsigned int num_ns, nsid;
	const struct spdk_nvme_ctrlr_data *cdata;
	struct spdk_nvme_ns *ns = spdk_nvme_ctrlr_get_ns(ctrlr, 1);
	
	bitmap_init(ioq_bitmap, MAX_NUM_IO_QUEUES, 0);
	nvme_ctrlr = ctrlr;
	cdata = spdk_nvme_ctrlr_get_data(ctrlr);

	if (!spdk_nvme_ns_is_active(ns)) {
		log_info("Controller %-20.20s (%-20.20s): Skipping inactive NS %u\n",
		       cdata->mn, cdata->sn,
		       spdk_nvme_ns_get_id(ns));
		return;
	}

	log_info("Attached to device %-20.20s (%-20.20s) controller: %p\n", cdata->mn, cdata->sn, ctrlr);

	num_ns = spdk_nvme_ctrlr_get_num_ns(ctrlr);
	log_info("Found %i namespaces\n", num_ns);
	for (nsid = 1; nsid <= num_ns; nsid++) {
		struct spdk_nvme_ns *ns = spdk_nvme_ctrlr_get_ns(ctrlr, nsid);
		log_info("NS: %i, size: %lx\n", nsid, spdk_nvme_ns_get_size(ns));
	}*/
}

/**
 * init_nvmedev - initializes an nvme device
 */
int init_nvmedev(void)
{
	int ret;
	int i;
	for (i = 0; i < CFG.num_nvmedev; i++) {
		const struct pci_addr *addr = &CFG.nvmedev[i];
		struct pci_dev *dev;
		// struct ix_rte_eth_dev *eth;

		dev = pci_alloc_dev(addr);
		if (!dev)
			return -ENOMEM;

		ret = pci_enable_device(dev);
		if (ret) {
			log_err("init: failed to enable PCI device\n");
			free(dev);
			goto err;
		}


		/* Don't know if we are going to need this
		ret = pci_set_master(dev);
		if (ret) {
			log_err("init: failed to set master\n");
			free(dev);
			goto err;
		}*/

		/*
		ret = eth_dev_add(eth);
		if (ret) {
			log_err("init: unable to add ethernet device\n");
			eth_dev_destroy(eth);
			goto err;
		}
		*/
	}

	if (spdk_nvme_probe(NULL, probe_cb, attach_cb) != 0) {
		log_info("spdk_nvme_probe() failed\n");
		return 1;
	}

	return 0;

err:
	return ret;
}