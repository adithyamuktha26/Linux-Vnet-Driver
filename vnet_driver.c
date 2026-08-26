#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

static struct net_device *vnet_dev;

// Transmission function: Called when packets are sent through this interface
static netdev_tx_t vnet_xmit(struct sk_buff *skb, struct net_device *dev) {
    // Update interface statistics
    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;
    
    // Free the socket buffer (drop the packet for now)
    dev_kfree_skb(skb); 
    
    return NETDEV_TX_OK;
}

// Map the transmission function to the device operations structure
static const struct net_device_ops vnet_netdev_ops = {
    .ndo_start_xmit = vnet_xmit,
};

// Setup function: Initialize MAC address and operation pointers
static void vnet_setup(struct net_device *dev) {
    ether_setup(dev); // Sets up standard Ethernet parameters
    dev->netdev_ops = &vnet_netdev_ops;
}

// Module Initialization
static int __init vnet_init(void) {
    printk(KERN_INFO "vnet_driver: Initializing\n");
    
    // Allocate the network device
    vnet_dev = alloc_netdev(0, "vnet%d", NET_NAME_UNKNOWN, vnet_setup);
    if (!vnet_dev)
        return -ENOMEM;

    // Register the network device with the kernel
    if (register_netdev(vnet_dev)) {
        printk(KERN_ERR "vnet_driver: Failed to register device\n");
        free_netdev(vnet_dev);
        return -1;
    }
    
    return 0;
}

// Module Exit
static void __exit vnet_exit(void) {
    printk(KERN_INFO "vnet_driver: Exiting\n");
    unregister_netdev(vnet_dev);
    free_netdev(vnet_dev);
}

module_init(vnet_init);
module_exit(vnet_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple virtual network device driver");

