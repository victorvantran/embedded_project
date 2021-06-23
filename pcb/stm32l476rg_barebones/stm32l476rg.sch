EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "stm32l476rg"
Date "2021-06-22"
Rev "1.0"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR?
U 1 1 60D2C03D
P 4300 4800
F 0 "#PWR?" H 4300 4550 50  0001 C CNN
F 1 "GND" H 4305 4627 50  0000 C CNN
F 2 "" H 4300 4800 50  0001 C CNN
F 3 "" H 4300 4800 50  0001 C CNN
	1    4300 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 4700 4300 4800
Wire Wire Line
	4300 1100 4300 1000
Wire Wire Line
	4800 1000 4800 1100
Connection ~ 4300 1100
Connection ~ 4500 1100
Wire Wire Line
	4500 1100 4400 1100
Wire Wire Line
	4400 1100 4300 1100
Connection ~ 4400 1100
Connection ~ 4300 4700
Wire Wire Line
	4500 4700 4400 4700
Wire Wire Line
	4400 4700 4300 4700
Connection ~ 4400 4700
Wire Wire Line
	4300 1000 4800 1000
Wire Wire Line
	4600 1100 4500 1100
$Comp
L MCU_ST_STM32L4:STM32L476RGTx U?
U 1 1 60D27E22
P 4500 2900
F 0 "U?" H 5350 4700 50  0000 C CNN
F 1 "STM32L476RGT6U" H 5100 1100 50  0000 C CNN
F 2 "Package_QFP:LQFP-64_10x10mm_P0.5mm" H 3900 1200 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00108832.pdf" H 4500 2900 50  0001 C CNN
	1    4500 2900
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 60D33FF0
P 4300 1000
F 0 "#PWR?" H 4300 850 50  0001 C CNN
F 1 "+3V3" H 4315 1173 50  0000 C CNN
F 2 "" H 4300 1000 50  0001 C CNN
F 3 "" H 4300 1000 50  0001 C CNN
	1    4300 1000
	1    0    0    -1  
$EndComp
Connection ~ 4300 1000
$Comp
L power:+3.3VA #PWR?
U 1 1 60D3DCA5
P 4700 900
F 0 "#PWR?" H 4700 750 50  0001 C CNN
F 1 "+3.3VA" H 4715 1073 50  0000 C CNN
F 2 "" H 4700 900 50  0001 C CNN
F 3 "" H 4700 900 50  0001 C CNN
	1    4700 900 
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR?
U 1 1 60D3DEEE
P 4700 4800
F 0 "#PWR?" H 4700 4550 50  0001 C CNN
F 1 "GNDA" H 4705 4627 50  0000 C CNN
F 2 "" H 4700 4800 50  0001 C CNN
F 3 "" H 4700 4800 50  0001 C CNN
	1    4700 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 1100 4700 900 
Wire Wire Line
	4500 4700 4600 4700
Connection ~ 4500 4700
Wire Wire Line
	4700 4700 4700 4800
Text GLabel 3800 1300 0    50   Input ~ 0
NRST
Text GLabel 3800 1500 0    50   Input ~ 0
BOOT0
$Comp
L Switch:SW_SPDT SW?
U 1 1 60D41840
P 1400 1500
F 0 "SW?" H 1400 1785 50  0000 C CNN
F 1 "SW_SPDT" H 1400 1694 50  0000 C CNN
F 2 "" H 1400 1500 50  0001 C CNN
F 3 "~" H 1400 1500 50  0001 C CNN
	1    1400 1500
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R?
U 1 1 60D42F38
P 1700 1500
F 0 "R?" V 1504 1500 50  0000 C CNN
F 1 "10K" V 1595 1500 50  0000 C CNN
F 2 "" H 1700 1500 50  0001 C CNN
F 3 "~" H 1700 1500 50  0001 C CNN
	1    1700 1500
	0    -1   -1   0   
$EndComp
Text GLabel 1800 1500 2    50   Input ~ 0
BOOT0
$Comp
L power:+3V3 #PWR?
U 1 1 60D44211
P 1100 1700
F 0 "#PWR?" H 1100 1550 50  0001 C CNN
F 1 "+3V3" H 1115 1873 50  0000 C CNN
F 2 "" H 1100 1700 50  0001 C CNN
F 3 "" H 1100 1700 50  0001 C CNN
	1    1100 1700
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60D44E00
P 1100 1300
F 0 "#PWR?" H 1100 1050 50  0001 C CNN
F 1 "GND" H 1105 1127 50  0000 C CNN
F 2 "" H 1100 1300 50  0001 C CNN
F 3 "" H 1100 1300 50  0001 C CNN
	1    1100 1300
	-1   0    0    1   
$EndComp
Wire Wire Line
	1200 1400 1100 1400
Wire Wire Line
	1100 1400 1100 1300
Wire Wire Line
	1200 1600 1100 1600
Wire Wire Line
	1100 1600 1100 1700
Text GLabel 3800 2500 0    50   Input ~ 0
HSE_IN
Text GLabel 3800 2600 0    50   Input ~ 0
HSE_OUT
Text GLabel 5200 2600 2    50   Input ~ 0
SWIDO
Text GLabel 5200 2700 2    50   Input ~ 0
SWCLK
Text GLabel 5200 3300 2    50   Input ~ 0
SWO
Text GLabel 5200 2500 2    50   Input ~ 0
USB_D+
Text GLabel 5200 2400 2    50   Input ~ 0
USB_D-
Text GLabel 5200 3600 2    50   Input ~ 0
I2C1_SCL
Text GLabel 5200 3700 2    50   Input ~ 0
I2C1_SDA
Text GLabel 5200 4000 2    50   Input ~ 0
UART3_TX
Text GLabel 5200 4100 2    50   Input ~ 0
UART3_RX
$EndSCHEMATC
