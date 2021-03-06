/* prize-lifenfen-20161027 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <sound/tlv.h>

struct i2c_client *tps65132_i2c_client = NULL;

struct pinctrl *pinctrltps65132;


enum tps65132_gpio_type {
	GPIO_DEFAULT = 0,
	GPIO_ENP_HIGH,
	GPIO_ENP_LOW,
	GPIO_ENN_HIGH,
	GPIO_ENN_LOW,
	GPIO_NUM
};

struct tps65132_gpio_attr {
	const char *name;
	bool gpio_prepare;
	struct pinctrl_state *gpioctrl;
};

static struct tps65132_gpio_attr tps65132_gpios[GPIO_NUM] = {
	[GPIO_DEFAULT] = {"default", false, NULL},
	[GPIO_ENP_HIGH] = {"lcd_bias_enp_high", false, NULL},
	[GPIO_ENP_LOW] = {"lcd_bias_enp_low", false, NULL},
	[GPIO_ENN_HIGH] = {"lcd_bias_enn_high", false, NULL},
	[GPIO_ENN_LOW] = {"lcd_bias_enn_low", false, NULL},
};

int tps65132_get_dts_info(void *dev)
{
	int ret = 0;
	int i = 0;

	printk("%s\n", __func__);

	pinctrltps65132 = devm_pinctrl_get(dev);
	if (IS_ERR(pinctrltps65132)) {
		ret = PTR_ERR(pinctrltps65132);
		pr_err("Cannot find pinctrlaud!\n");
		return ret;
	}

	for (i = 1; i < ARRAY_SIZE(tps65132_gpios); i++) {
		tps65132_gpios[i].gpioctrl = pinctrl_lookup_state(pinctrltps65132, tps65132_gpios[i].name);
		if (IS_ERR(tps65132_gpios[i].gpioctrl)) {
			ret = PTR_ERR(tps65132_gpios[i].gpioctrl);
			pr_err("%s pinctrl_lookup_state %s fail %d\n", __func__, tps65132_gpios[i].name,
			       ret);
		} else {
			tps65132_gpios[i].gpio_prepare = true;
 			pr_err("%s pinctrl_lookup_state %s ok\n", __func__, tps65132_gpios[i].name);
		}
	}

	return ret;
}

int tps65132_enp_enable(bool bEnable)
{
	int retval = 0;

	if (bEnable) {
		if (tps65132_gpios[GPIO_ENP_HIGH].gpio_prepare) {
			retval =
			    pinctrl_select_state(pinctrltps65132, tps65132_gpios[GPIO_ENP_HIGH].gpioctrl);
			if (retval)
				pr_err("could not set tps65132_gpios[GPIO_ENP_HIGH] pins\n");
			else				
			pr_err("set tps65132_gpios[GPIO_ENP_HIGH] pins\n");
		}
	} else {
		if (tps65132_gpios[GPIO_ENP_LOW].gpio_prepare) {
			retval =
			    pinctrl_select_state(pinctrltps65132, tps65132_gpios[GPIO_ENP_LOW].gpioctrl);
			if (retval)
				pr_err("could not set tps65132_gpios[GPIO_ENP_LOW] pins\n");			
			else				
			pr_err("set tps65132_gpios[GPIO_ENP_LOW] pins\n");
		}

	}
	return retval;
}

int tps65132_enn_enable(bool bEnable)
{
	int retval = 0;

	if (bEnable) {
		if (tps65132_gpios[GPIO_ENN_HIGH].gpio_prepare) {
			retval =
			    pinctrl_select_state(pinctrltps65132, tps65132_gpios[GPIO_ENN_HIGH].gpioctrl);
			if (retval)
				pr_err("could not set tps65132_gpios[GPIO_ENN_HIGH] pins\n");
			else				
			pr_err("set tps65132_gpios[GPIO_ENN_HIGH] pins\n");
		}
	} else {
		if (tps65132_gpios[GPIO_ENN_LOW].gpio_prepare) {
			retval =
			    pinctrl_select_state(pinctrltps65132, tps65132_gpios[GPIO_ENN_LOW].gpioctrl);
			if (retval)
				pr_err("could not set tps65132_gpios[GPIO_ENN_LOW] pins\n");			
			else				
			pr_err("set tps65132_gpios[GPIO_ENN_LOW] pins\n");
		}

	}
	return retval;
}

int tps65132_write_bytes(unsigned char addr, unsigned char value)
{
	int ret = 0;
	struct i2c_client *client = tps65132_i2c_client;
	char write_data[2] = { 0 };
	write_data[0] = addr;
	write_data[1] = value;
	ret = i2c_master_send(client, write_data, 2);
	if (ret < 0)
		printk("tps65132 write data fail !!\n");
	return ret;
}


int tps65132_avdd_en(bool bEnable)
{
	unsigned char cmd = 0x0;
	unsigned char data = 0xFF;
	int ret = 0;

	
   // printk("%s, %d\n", __FUNCTION__, __LINE__);
	if (bEnable)
	{
		tps65132_enn_enable(bEnable);
		tps65132_enp_enable(bEnable);
		
		cmd = 0x03;
		data = 0x43;
		ret = tps65132_write_bytes(cmd, data);

	    if(NULL != tps65132_i2c_client){
			cmd = 0x00;
	        data = 0x14;
	        ret = tps65132_write_bytes(cmd, data);
	    }else{
	        printk("%s,%d,tps65132 invalid\n",__FUNCTION__,__LINE__);
	    }

	    
	    if(NULL != tps65132_i2c_client){
	        cmd = 0x01;
	        data = 0x14;
	        ret = tps65132_write_bytes(cmd, data);
	    }else{
	        printk("%s,%d,tps65132 invalid\n",__FUNCTION__,__LINE__);
	    }
	}
	else
	{
		tps65132_enn_enable(bEnable);
		tps65132_enp_enable(bEnable);
	}
	return ret;
}
EXPORT_SYMBOL(tps65132_avdd_en);

static int tps65132_i2c_probe(struct i2c_client *i2c_client, const struct i2c_device_id *id)
{

	tps65132_i2c_client = i2c_client;

	tps65132_get_dts_info(&i2c_client->dev);

	tps65132_avdd_en(true);

	return 0;
}

static int tps65132_i2c_remove(struct i2c_client *i2c_client)
{
	tps65132_i2c_client = NULL;
	return 0;
}
#if 0
static int tps65132_suspend(struct i2c_client *client, pm_message_t msg)
{
	printk("%s\n", __func__);
	return 0;
}
/*----------------------------------------------------------------------------*/
static int tps65132_resume(struct i2c_client *client)
{
	printk("%s\n", __func__);
	return 0;
}

static const struct dev_pm_ops tps65132_pm_ops = {
	.suspend = tps65132_suspend,
	.resume = tps65132_resume,
};
#endif
static const struct of_device_id tps65132_of_match[] = {
	{ .compatible = "mediatek, LCD_BIAS", },
	{},
};
MODULE_DEVICE_TABLE(of, tps65132_of_match);


static const struct i2c_device_id tps65132_id[] = {
	{"tps65132", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, tps65132_id);

static struct i2c_driver tps65132_i2c_driver = {
	.driver = {
		   .name = "tps65132",
		   .owner = THIS_MODULE,
		   #if 0
		   .pm = &tps65132_pm_ops,
		   #endif
		   .of_match_table = tps65132_of_match,
		   },
	.id_table = tps65132_id,
	.probe = tps65132_i2c_probe,
	.remove = tps65132_i2c_remove,
};

module_i2c_driver(tps65132_i2c_driver);
