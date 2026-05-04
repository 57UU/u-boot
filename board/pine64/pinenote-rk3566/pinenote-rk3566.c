// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2019 Rockchip Electronics Co., Ltd
 * (C) Copyright 2023 Collabora Ltd
 */

#include <adc.h>
#include <stdio.h>
#include <dm.h>
#include <efi_loader.h>

#define KEY_DOWN_MIN_VAL        0
#define KEY_DOWN_MAX_VAL        30
#define PINENOTE_UPDATABLE_IMAGES	2

// UUID: B48DD0E3-2342-5704-A97C-40A65CF83FD5
#define PINENOTE_IDBLOADER_IMAGE_GUID \
	EFI_GUID(0xb48dd0e3, 0x2342, 0x5704, 0xa9, 0x7c, \
		 0x40, 0xa6, 0x5c, 0xf8, 0x3f, 0xd5)

// UUID: CB2A307D-DEF1-5238-954F-0C7BABF6B078
#define PINENOTE_UBOOT_IMAGE_GUID \
	EFI_GUID(0xcb2a307d, 0xdef1, 0x5238, 0x95, 0x4f, \
		 0x0c, 0x7b, 0xab, 0xf6, 0xb0, 0x78)

int rockchip_dnl_key_pressed(void)
{
	unsigned int val;

	if (adc_channel_single_shot("saradc@fe720000", 0, &val)) {
		printf("%s read adc key val failed\n", __func__);
		return false;
	}

	if (val >= KEY_DOWN_MIN_VAL && val <= KEY_DOWN_MAX_VAL)
		return true;
	else
		return false;
}


#if IS_ENABLED(CONFIG_EFI_HAVE_CAPSULE_SUPPORT)
static struct efi_fw_image fw_images[PINENOTE_UPDATABLE_IMAGES] = {0};

struct efi_capsule_update_info update_info = {
	.num_images = PINENOTE_UPDATABLE_IMAGES,
	.images = fw_images,
};

#endif

#ifndef CONFIG_XPL_BUILD
#if IS_ENABLED(CONFIG_EFI_HAVE_CAPSULE_SUPPORT) && IS_ENABLED(CONFIG_EFI_PARTITION)
static bool board_is_pinenote(void)
{
	return of_machine_is_compatible("pine64,pinenote-v1.2");
}

void rockchip_capsule_update_board_setup(void)
{
	if (board_is_pinenote()) {
		efi_guid_t idbldr_image_type_guid =
			PINENOTE_IDBLOADER_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = PINENOTE_UBOOT_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &idbldr_image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);

		fw_images[0].fw_name = u"PINENOTE-IDBLOADER";
		fw_images[1].fw_name = u"PINENOTE-UBOOT";
	} else {
		update_info.num_images = 0;
	}
}
#endif /* CONFIG_EFI_HAVE_CAPSULE_SUPPORT && CONFIG_EFI_PARTITION */
#endif /* !CONFIG_XPL_BUILD */

