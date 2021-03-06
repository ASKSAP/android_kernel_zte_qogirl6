#ifndef __AW881XX_H__
#define __AW881XX_H__

#include <linux/version.h>
#include <sound/control.h>
#include <sound/soc.h>
#include "aw881xx_monitor.h"
#include "aw881xx_cali.h"


#if (KERNEL_VERSION(4, 19, 1) <= LINUX_VERSION_CODE)
#define AW_KERNEL_VER_OVER_4_19_1
#endif

/********************************************
 *
 * enum
 *
 *******************************************/
enum aw881xx_scene_mode {
	AW881XX_SPK_MODE,
	AW881XX_VOICE_MODE,
	AW881XX_FM_MODE,
	AW881XX_RCV_MODE,
	AW881XX_DSPBYPASS_MODE,
	AW881XX_OFF_MODE,
	AW881XX_MODE_MAX,
};

enum aw881xx_pa_switch_st {
	AW881XX_OFF_PA = 0,
	AW881XX_ON_PA,
};

enum aw881xx_chip_st {
	AW881XX_PA_CLOSE_ST = 0,
	AW881XX_PA_CLOSING_ST,
	AW881XX_PA_OPEN_ST,
	AW881XX_PA_OPENING_ST,
};

enum aw881xx_audio_stream_st {
	AW881XX_AUDIO_STOP = 0,
	AW881XX_AUDIO_START = 1,
};

enum aw881xx_id {
	AW881XX_CHIPID = 0x1806,
	AW881XX_PID_01 = 0x01,
	AW881XX_PID_02 = 0x02,
	AW881XX_PID_03 = 0x03,
};

enum aw881xx_dsp_pid {
	AW881XX_DSP_PID_01 = 0x0000,
	AW881XX_DSP_PID_02 = 0x0001,
	AW881XX_DSP_PID_03 = 0x6E90,
};

enum aw881xx_memclk {
	AW881XX_MEMCLK_OSC = 0,
	AW881XX_MEMCLK_PLL = 1,
};

enum aw881xx_init {
	AW881XX_INIT_ST = 0,
	AW881XX_INIT_OK = 1,
	AW881XX_INIT_NG = 2,
};

enum aw881xx_dsp_cfg {
	AW881XX_DSP_WORK = 0,
	AW881XX_DSP_BYPASS = 1,
};

enum aw881xx_cfg_load_st {
	AW881XX_CFG_NOT_LOAD = 0,
	AW881XX_CFG_LOADING = 1,
	AW881XX_CFG_READY = 2
};

enum aw881xx_cfg_file_offset {
	AW881XX_REG_FILE_OFFSET = 0,
	AW881XX_DSP_FW_FILE_OFFSET = 1,
	AW881XX_DSP_CFG_FILE_OFFSET = 2,
};

enum aw881xx_baseaddr {
	AW881XX_SPK_REG_ADDR = 0x00,
	AW881XX_SPK_DSP_FW_ADDR = 0x8c00,
	AW881XX_SPK_DSP_CFG_ADDR = 0x8600,
	AW881XX_VOICE_REG_ADDR = 0x00,
	AW881XX_VOICE_DSP_FW_ADDR = 0x8c00,
	AW881XX_VOICE_DSP_CFG_ADDR = 0x8600,
	AW881XX_FM_REG_ADDR = 0x00,
	AW881XX_FM_DSP_FW_ADDR = 0x8c00,
	AW881XX_FM_DSP_CFG_ADDR = 0x8600,
	AW881XX_RCV_REG_ADDR = 0x00,
	AW881XX_RCV_DSP_FW_ADDR = 0x8c00,
	AW881XX_RCV_DSP_CFG_ADDR = 0x8600,
	AW881XX_DSPBYPASS_REG_ADDR = 0x00,
	AW881XX_DSPBYPASS_DSP_FW_ADDR = 0x8c00,
	AW881XX_DSPBYPASS_DSP_CFG_ADDR = 0x8600,
};

#define AW881XX_ADD_CHAN_NAME_SHIFT		2

#define AW881XX_READ_MSG_NUM			2

/*
 * i2c transaction on Linux limited to 64k
 * (See Linux kernel documentation: Documentation/i2c/writing-clients)
*/
#define MAX_I2C_BUFFER_SIZE				(65536)

#define AW881XX_FLAG_START_ON_MUTE		(1 << 0)
#define AW881XX_FLAG_SKIP_INTERRUPTS	(1 << 1)

#define AW881XX_NUM_RATES				(9)
#define AW881XX_SYSST_CHECK_MAX			(10)

#define AW881XX_DFT_CALI_RE				(0x8000)

#define AW881XX_MONITOR_DFT_FLAG		(0)
#define AW881XX_MONITOR_TIMER_DFT_VAL	(30000)

#define AW881XX_VBAT_COEFF_INT_10BIT	(1023)

#define AW881XX_MODE_CFG_NUM_MAX		(3)
#define AW881XX_NO_CFG_MODE_NUM			(1)
#define AW881XX_CFG_MODE_NUM			(AW881XX_MODE_MAX - AW881XX_NO_CFG_MODE_NUM)
#define AW881XX_CFG_NUM_MAX				(AW881XX_CFG_MODE_NUM * AW881XX_MODE_CFG_NUM_MAX)
#define AW881XX_CFG_NAME_MAX			(64)

#define AW881XX_START_WORK_DELAY_MS		(0)
#define AW881XX_IIS_CHECK_MAX_CNT		(100)
#define AW881XX_IIS_CHECK_PASS_CNT		(5)
#define AW881XX_REQUEST_CFG_MAX_CNT		(10)

/********************************************
 *
 * DSP I2C WRITES
 *
 *******************************************/
#define AW881XX_DSP_I2C_WRITES
#define AW881XX_MAX_RAM_WRITE_BYTE_SIZE	(216)


/********************************************
 *
 * Compatible with codec and component
 *
 *******************************************/
#ifdef AW_KERNEL_VER_OVER_4_19_1
typedef struct snd_soc_component aw_snd_soc_codec_t;
typedef struct snd_soc_component_driver aw_snd_soc_codec_driver_t;
#else
typedef struct snd_soc_codec aw_snd_soc_codec_t;
typedef struct snd_soc_codec_driver aw_snd_soc_codec_driver_t;
#endif

struct aw_componet_codec_ops {
	aw_snd_soc_codec_t *(*aw_snd_soc_kcontrol_codec)(struct snd_kcontrol *kcontrol);
	void *(*aw_snd_soc_codec_get_drvdata)(aw_snd_soc_codec_t *codec);
	int (*aw_snd_soc_add_codec_controls)(aw_snd_soc_codec_t *codec,
		const struct snd_kcontrol_new *controls, unsigned int num_controls);
	void (*aw_snd_soc_unregister_codec)(struct device *dev);
	int (*aw_snd_soc_register_codec)(struct device *dev,
			const aw_snd_soc_codec_driver_t *codec_drv,
			struct snd_soc_dai_driver *dai_drv,
			int num_dai);
};
/********************************************
 *
 * aw881xx container
 *
 *******************************************/
struct aw881xx_container {
	int len;
	unsigned char *data;
};

struct aw881xx_cfg {
	enum aw881xx_cfg_load_st cfg_load_st;
	struct aw881xx_container cfg_data;
};

/********************************************
 *
 * aw881xx struct
 *
 *******************************************/
struct aw881xx {
	struct regmap *regmap;
	struct i2c_client *i2c;
	struct device *dev;
	struct mutex lock;
	struct mutex i2c_lock;
	struct aw881xx_monitor monitor;
	struct aw881xx_cali_attr cali_attr;
	struct delayed_work delay_work;
	struct delayed_work load_cfg_work;
	/*struct aw881xx_container *dsp_cfg_cnt;*/
	aw_snd_soc_codec_t *codec;

	int sysclk;
	int rate;
	int width;
	int pstream;
	int cstream;
	int startup_cnt;

	int reset_gpio;
	int irq_gpio;
	int aw881xx_pa_switch;
	int is_power_on;
	int audio_stream_st;

	uint8_t flags;
	bool work_flag;
	uint8_t init;
	uint8_t scene_mode;
	bool is_dsp_fw_updated;

	uint16_t chipid;
	uint8_t pid;

	uint8_t reg_addr;
	uint16_t dsp_addr;

	uint8_t dsp_cfg;
	char cfg_name[AW881XX_CFG_NUM_MAX][AW881XX_CFG_NAME_MAX];
	struct aw881xx_cfg cfg_info[AW881XX_CFG_NUM_MAX];
	uint16_t cfg_num;
	/*uint32_t dsp_fw_len;*/

	uint16_t intmask;
	char *name_suffix;

	uint32_t codec_id;
	uint32_t codecs_num;
	struct aw881xx **codecs;
};


/********************************************
 *
 * print information control
 *
 *******************************************/
#define aw_dev_err(dev, format, ...) \
			pr_err("[%s]" format, dev_name(dev), ##__VA_ARGS__)

#define aw_dev_info(dev, format, ...) \
			pr_info("[%s]" format, dev_name(dev), ##__VA_ARGS__)

#define aw_dev_dbg(dev, format, ...) \
			pr_debug("[%s]" format, dev_name(dev), ##__VA_ARGS__)

/******************************************************
 *
 * aw881xx i2c write/read
 *
 ******************************************************/
int aw881xx_reg_writes(struct aw881xx *aw881xx,
			uint8_t reg_addr, uint8_t *buf, uint16_t len);

int aw881xx_reg_write(struct aw881xx *aw881xx,
			uint8_t reg_addr, uint16_t reg_data);
int aw881xx_reg_read(struct aw881xx *aw881xx,
			uint8_t reg_addr, uint16_t *reg_data);
int aw881xx_reg_write_bits(struct aw881xx *aw881xx,
			uint8_t reg_addr, uint16_t mask, uint16_t reg_data);
int aw881xx_dsp_write(struct aw881xx *aw881xx,
			uint16_t dsp_addr, uint16_t dsp_data);
int aw881xx_dsp_read(struct aw881xx *aw881xx,
			uint16_t dsp_addr, uint16_t *dsp_data);

int aw881xx_get_iis_status(struct aw881xx *aw881xx);
int aw881xx_get_dsp_status(struct aw881xx *aw881xx);
int aw881xx_get_sysint(struct aw881xx *aw881xx, uint16_t *sysint);
int aw881xx_get_hmute(struct aw881xx *aw881xx);

#endif
