// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Tauchgang environment
 */

"bootretry=1\0"
"stdout=serial\0"
"stderr=serial,vidconsole\0"

/* Cosmetics */
"splashimage=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0"
"splashpos=m,m\0"


/*
 * Redirect stdout to vidconsole too so the menu and EFI messages are visible
 * (stdout is serial-only at this point, to have a somewhat clean splash screen)
 */
"setup_stdout=if test $stdout = serial; then setenv stdout serial,vidconsole; fi\0"
"preboot=run setup_stdout\0"

/*
 * bootretry will run this command over and over, if we fail once
 * then bail out to the boot menu instead (with a pause to read
 * the error message)
 */
"bootcmd=bootefi bootmgr; pause; run menucmd\0"

/*
 * When entering the menu (either from button press or failed boot)
 * remap bootcmd so it will re-open the menu and we won't get stuck
 * at the console with no way to type
 */
"menucmd=setenv bootcmd run menucmd; bootmenu -1\0"

/* Shortcut to enable USB serial gadget and disable bootretry */
"serial_gadget=setenv stdin serial,button-kbd,usbacm; setenv stdout serial,vidconsole,usbacm; setenv stderr serial,vidconsole,usbacm; setenv bootretry -1; echo Enabled U-Boot console serial gadget\0"

/*
 * Boot menu entries
 */

/* Pause is used so the output can be read on the display */
"bootmenu_0=Boot=bmp display; bootefi bootmgr; pause\0"
"bootmenu_1=Enable serial console gadget=bmp display; run serial_gadget\0"
#if IS_ENABLED(CONFIG_FASTBOOT)
"fastboot=fastboot usb 0\0"
"bootmenu_2=Enable fastboot mode=bmp display; run fastboot\0"
#elif IS_ENABLED(CONFIG_DFU)
"bootmenu_2=Enable DFU mode=bmp display; dfu 0 ram 0\0"
#else
"bootmenu_2=Fastboot/DFU mode unavailable=pause\0"
#endif
/* Disabling bootretry means we'll just drop the shell */
"bootmenu_3=Drop to shell=setenv bootretry -1\0"
"bootmenu_4=Reset device=reset\0"
"bootmenu_5=Dump clocks=clk dump; pause\0"
"bootmenu_6=Dump environment=printenv; pause\0"
"bootmenu_7=Board info=bdinfo; pause\0"
"bootmenu_8=Dump bootargs=fdt print /chosen bootargs; pause\0"
