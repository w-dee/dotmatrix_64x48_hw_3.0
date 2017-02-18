EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:dsp
LIBS:analog_switches
LIBS:audio
LIBS:interface
LIBS:display
LIBS:siliconi
LIBS:opto
LIBS:contrib
LIBS:valves
LIBS:DeeComponents
LIBS:bb-ch340t
LIBS:wroom-02
LIBS:actel
LIBS:silabs
LIBS:ac-dc
LIBS:ir
LIBS:74xgxx
LIBS:Zilog
LIBS:dc-dc
LIBS:references
LIBS:cmos_ieee
LIBS:sensors
LIBS:bbd
LIBS:bosch
LIBS:zetex
LIBS:hc11
LIBS:ESD_Protection
LIBS:Oscillators
LIBS:allegro
LIBS:switches
LIBS:pspice
LIBS:transf
LIBS:logo
LIBS:relays
LIBS:ttl_ieee
LIBS:diode
LIBS:wiznet
LIBS:mechanical
LIBS:dotmatrix_64x48-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 9 9
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L USB_OTG CONN2
U 1 1 584C8CCD
P 2150 3800
AR Path="/584C83E3/584C8CCD" Ref="CONN2"  Part="1" 
AR Path="/584C8CCD" Ref="CONN2"  Part="1" 
F 0 "CONN2" V 1900 3900 50  0000 L CNN
F 1 "USB_OTG" V 1800 3850 50  0000 L CNN
F 2 "components:USB-MICRO-B-RECEPT-UNIVERSAL" V 2136 3988 50  0001 L CNN
F 3 "" V 2100 3700 50  0000 C CNN
	1    2150 3800
	1    0    0    -1  
$EndComp
Text GLabel 4400 3800 0    60   Input ~ 0
USB_D_P
Text GLabel 4450 3900 0    60   Input ~ 0
USB_D_N
$Comp
L C C30
U 1 1 584C8CFB
P 4000 3100
F 0 "C30" H 4000 3450 50  0000 L CNN
F 1 "0.1u" H 4000 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4115 3009 50  0001 L CNN
F 3 "" H 4000 3100 50  0000 C CNN
	1    4000 3100
	1    0    0    -1  
$EndComp
Connection ~ 4400 2900
Wire Wire Line
	4400 2950 4400 2900
Wire Wire Line
	4400 3250 4400 3350
Wire Wire Line
	6150 4050 7000 4050
Wire Wire Line
	6150 3950 6900 3950
Wire Wire Line
	6150 4150 6500 4150
Wire Wire Line
	6350 3750 6150 3750
Wire Wire Line
	5100 3850 4450 3850
Wire Wire Line
	4450 3850 4450 3900
Wire Wire Line
	4450 3900 2450 3900
Wire Wire Line
	2450 3800 4400 3800
Wire Wire Line
	4400 3800 4400 3750
Wire Wire Line
	4400 3750 5100 3750
Wire Wire Line
	2550 3700 4200 3700
Wire Wire Line
	4950 2750 5100 2750
Wire Wire Line
	4750 3200 5100 3200
$Comp
L R R53
U 1 1 584C8D46
P 6250 2500
F 0 "R53" V 6250 2400 50  0000 L CNN
F 1 "10k" V 6200 2600 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 6180 2500 50  0001 C CNN
F 3 "" H 6250 2500 50  0000 C CNN
	1    6250 2500
	1    0    0    1   
$EndComp
Wire Wire Line
	6250 2100 6250 2350
Wire Wire Line
	6250 2650 6250 2750
Wire Wire Line
	6250 2750 6150 2750
Wire Wire Line
	4950 2300 7150 2300
NoConn ~ 5100 4100
NoConn ~ 6150 4250
NoConn ~ 6150 3850
NoConn ~ 6150 3650
NoConn ~ 6150 3550
NoConn ~ 6150 3400
NoConn ~ 6150 3300
NoConn ~ 6150 3200
NoConn ~ 6150 3100
NoConn ~ 6150 2950
Connection ~ 4400 3350
Wire Wire Line
	4750 3200 4750 3600
Connection ~ 4750 3600
Wire Wire Line
	4950 2300 4950 2900
Connection ~ 4950 2900
Connection ~ 4950 2750
$Comp
L C C60
U 1 1 584C8CEE
P 4400 3100
F 0 "C60" H 4350 3450 50  0000 L CNN
F 1 "1u" H 4400 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4515 3009 50  0001 L CNN
F 3 "" H 4400 3100 50  0000 C CNN
	1    4400 3100
	1    0    0    -1  
$EndComp
$Comp
L C C28
U 1 1 584CA50A
P 3450 3550
F 0 "C28" H 3400 3150 50  0000 L CNN
F 1 "1u" H 3450 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3565 3459 50  0001 L CNN
F 3 "" H 3450 3550 50  0000 C CNN
	1    3450 3550
	1    0    0    -1  
$EndComp
$Comp
L CP2104 U16
U 1 1 584C8D2C
P 5300 2700
F 0 "U16" H 5625 2937 60  0000 C CNN
F 1 "CP2104" H 5625 2831 60  0000 C CNN
F 2 "components:QFN-24-1EP_4x4mm_Pitch0.5mm-handsoldering" H 5300 2700 60  0001 C CNN
F 3 "" H 5300 2700 60  0001 C CNN
	1    5300 2700
	1    0    0    -1  
$EndComp
Connection ~ 4550 3350
Wire Wire Line
	4550 3350 4550 4250
Wire Wire Line
	4550 4250 5100 4250
$Comp
L CONN_01X08 P1
U 1 1 584CCEBC
P 7350 3450
F 0 "P1" H 7427 3536 50  0000 L CNN
F 1 "CONN_01X08" H 7427 3445 50  0000 L CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x08_Pitch2.54mm" H 7427 3354 50  0000 L CNN
F 3 "" H 7350 3450 50  0000 C CNN
	1    7350 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 2300 7150 3100
Wire Wire Line
	6300 3200 7150 3200
Wire Wire Line
	6900 3950 6900 3300
Wire Wire Line
	6900 3300 7150 3300
Wire Wire Line
	7000 4050 7000 3400
Wire Wire Line
	7000 3400 7150 3400
Wire Wire Line
	7150 3500 6350 3500
Wire Wire Line
	6350 3500 6350 3750
Wire Wire Line
	6500 4150 6500 3600
Wire Wire Line
	6500 3600 7150 3600
Wire Wire Line
	4650 3250 4650 3350
$Comp
L C C73
U 1 1 584E0C6E
P 4650 3100
F 0 "C73" H 4600 3450 50  0000 L CNN
F 1 "1u" H 4600 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4765 3009 50  0001 L CNN
F 3 "" H 4650 3100 50  0000 C CNN
	1    4650 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 2950 4650 2900
Connection ~ 4650 2900
Connection ~ 4650 3350
Text Label 6450 3500 0    60   ~ 0
USBDTR
Text Label 6500 3600 0    60   ~ 0
USBRTS
Text Label 6550 3950 0    60   ~ 0
USBTXD
Text Label 6550 4050 0    60   ~ 0
USBRXD
Text Label 3750 3700 0    60   ~ 0
VBUS
Text Label 6450 2300 0    60   ~ 0
USB3V3VDD
$Comp
L LED_Small D9
U 1 1 584E6A14
P 6700 2850
F 0 "D9" V 6746 2782 50  0000 R CNN
F 1 "Led_Small" V 6655 2782 50  0000 R CNN
F 2 "components:LED1608_Handsoldering" V 6700 2850 50  0001 C CNN
F 3 "" V 6700 2850 50  0000 C CNN
	1    6700 2850
	-1   0    0    -1  
$EndComp
$Comp
L R R125
U 1 1 584E6A1B
P 6300 2850
F 0 "R125" V 6300 2750 50  0000 L CNN
F 1 "1k" V 6400 2900 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 6230 2850 50  0001 C CNN
F 3 "" H 6300 2850 50  0000 C CNN
	1    6300 2850
	0    1    -1   0   
$EndComp
Wire Wire Line
	6450 2850 6600 2850
Wire Wire Line
	5100 3600 4200 3600
Wire Wire Line
	4200 3600 4200 3700
$Comp
L C C76
U 1 1 5886B762
P 3250 3550
F 0 "C76" H 3200 3150 50  0000 L CNN
F 1 "1u" H 3200 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3365 3459 50  0001 L CNN
F 3 "" H 3250 3550 50  0000 C CNN
	1    3250 3550
	1    0    0    -1  
$EndComp
$Comp
L C C75
U 1 1 5886B781
P 3050 3550
F 0 "C75" H 3000 3150 50  0000 L CNN
F 1 "1u" H 3000 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3165 3459 50  0001 L CNN
F 3 "" H 3050 3550 50  0000 C CNN
	1    3050 3550
	1    0    0    -1  
$EndComp
$Comp
L C C74
U 1 1 5886B7A1
P 2850 3550
F 0 "C74" H 2800 3150 50  0000 L CNN
F 1 "1u" H 2800 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 2965 3459 50  0001 L CNN
F 3 "" H 2850 3550 50  0000 C CNN
	1    2850 3550
	1    0    0    -1  
$EndComp
$Comp
L C C77
U 1 1 5886B7E4
P 3650 3550
F 0 "C77" H 3600 3150 50  0000 L CNN
F 1 "0.1u" H 3600 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3765 3459 50  0001 L CNN
F 3 "" H 3650 3550 50  0000 C CNN
	1    3650 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 3400 2850 3350
Connection ~ 2850 3350
Wire Wire Line
	3050 3400 3050 3350
Connection ~ 3050 3350
Wire Wire Line
	3250 3400 3250 3350
Connection ~ 3250 3350
Wire Wire Line
	3450 3400 3450 3350
Connection ~ 3450 3350
Wire Wire Line
	3650 3400 3650 3350
Connection ~ 3650 3350
Connection ~ 3650 3700
Connection ~ 3450 3700
Connection ~ 3250 3700
Connection ~ 3050 3700
Connection ~ 2850 3700
$Comp
L C C78
U 1 1 5886BE74
P 4200 3100
F 0 "C78" H 4150 3450 50  0000 L CNN
F 1 "1u" H 4200 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4315 3009 50  0001 L CNN
F 3 "" H 4200 3100 50  0000 C CNN
	1    4200 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 2950 4200 2900
Connection ~ 4200 2900
Wire Wire Line
	4200 3250 4200 3350
Connection ~ 4200 3350
Wire Wire Line
	4000 2950 4000 2900
Connection ~ 4000 2900
Wire Wire Line
	4000 3250 4000 3350
Connection ~ 4000 3350
Text Label 6250 2100 0    60   ~ 0
VBUS
Wire Wire Line
	2450 3600 2550 3600
Wire Wire Line
	2550 3600 2550 3700
Wire Wire Line
	5100 3350 1700 3350
Wire Wire Line
	1700 3350 1700 4300
Wire Wire Line
	1700 4300 2600 4300
Wire Wire Line
	2050 4300 2050 4200
Wire Wire Line
	2150 4300 2150 4200
Connection ~ 2050 4300
NoConn ~ 2450 4000
Wire Wire Line
	7150 3700 7100 3700
Wire Wire Line
	7100 3700 7100 3800
Wire Wire Line
	7100 3800 7150 3800
Wire Wire Line
	6300 3200 6300 4500
Wire Wire Line
	6300 4500 2600 4500
Wire Wire Line
	2600 4500 2600 4300
Connection ~ 2150 4300
Text Label 1900 3350 0    60   ~ 0
USBGND
Wire Wire Line
	6800 2850 6950 2850
Wire Wire Line
	6950 2850 6950 3200
Connection ~ 6950 3200
Wire Wire Line
	4000 2900 5100 2900
$EndSCHEMATC
