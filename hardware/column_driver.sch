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
Sheet 4 9
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
L GND #PWR057
U 1 1 58122B46
P 8350 2950
F 0 "#PWR057" H 8350 2700 50  0001 C CNN
F 1 "GND" H 8355 2777 50  0000 C CNN
F 2 "" H 8350 2950 50  0000 C CNN
F 3 "" H 8350 2950 50  0000 C CNN
	1    8350 2950
	1    0    0    -1  
$EndComp
$Comp
L R R24
U 1 1 58122B4C
P 8350 2800
F 0 "R24" H 8420 2846 50  0000 L CNN
F 1 "22k" H 8420 2755 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 8280 2800 50  0001 C CNN
F 3 "" H 8350 2800 50  0000 C CNN
	1    8350 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR058
U 1 1 58122B53
P 6250 2950
F 0 "#PWR058" H 6250 2700 50  0001 C CNN
F 1 "GND" H 6255 2777 50  0000 C CNN
F 2 "" H 6250 2950 50  0000 C CNN
F 3 "" H 6250 2950 50  0000 C CNN
	1    6250 2950
	1    0    0    -1  
$EndComp
$Comp
L R R22
U 1 1 58122B59
P 6250 2800
F 0 "R22" H 6320 2846 50  0000 L CNN
F 1 "22k" H 6320 2755 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 6180 2800 50  0001 C CNN
F 3 "" H 6250 2800 50  0000 C CNN
	1    6250 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR059
U 1 1 58122B60
P 4150 2950
F 0 "#PWR059" H 4150 2700 50  0001 C CNN
F 1 "GND" H 4155 2777 50  0000 C CNN
F 2 "" H 4150 2950 50  0000 C CNN
F 3 "" H 4150 2950 50  0000 C CNN
	1    4150 2950
	1    0    0    -1  
$EndComp
$Comp
L R R20
U 1 1 58122B66
P 4150 2800
F 0 "R20" H 4220 2846 50  0000 L CNN
F 1 "22k" H 4220 2755 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 4080 2800 50  0001 C CNN
F 3 "" H 4150 2800 50  0000 C CNN
	1    4150 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR060
U 1 1 58122B6D
P 2050 2950
F 0 "#PWR060" H 2050 2700 50  0001 C CNN
F 1 "GND" H 2055 2777 50  0000 C CNN
F 2 "" H 2050 2950 50  0000 C CNN
F 3 "" H 2050 2950 50  0000 C CNN
	1    2050 2950
	1    0    0    -1  
$EndComp
$Comp
L R R18
U 1 1 58122B73
P 2050 2800
F 0 "R18" H 2120 2846 50  0000 L CNN
F 1 "22k" H 2120 2755 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1980 2800 50  0001 C CNN
F 3 "" H 2050 2800 50  0000 C CNN
	1    2050 2800
	1    0    0    -1  
$EndComp
Text GLabel 7850 2650 3    60   Input ~ 0
COLPWCLK1
Text GLabel 7750 2650 3    60   Input ~ 0
COLLATCH
Text GLabel 7650 2650 3    60   Input ~ 0
COLCLK
Text GLabel 5750 2650 3    60   Input ~ 0
COLPWCLK0
Text GLabel 5650 2650 3    60   Input ~ 0
COLLATCH
Text GLabel 5550 2650 3    60   Input ~ 0
COLCLK
Text GLabel 3650 2650 3    60   Input ~ 0
COLPWCLK1
Text GLabel 3550 2650 3    60   Input ~ 0
COLLATCH
Text GLabel 3450 2650 3    60   Input ~ 0
COLCLK
Text GLabel 1550 2650 3    60   Input ~ 0
COLPWCLK0
Text GLabel 1450 2650 3    60   Input ~ 0
COLLATCH
Text GLabel 1350 2650 3    60   Input ~ 0
COLCLK
Text GLabel 1250 2650 3    60   Input ~ 0
COLSER
$Comp
L GND #PWR061
U 1 1 58122B87
P 8850 2350
F 0 "#PWR061" H 8850 2100 50  0001 C CNN
F 1 "GND" H 8855 2177 50  0000 C CNN
F 2 "" H 8850 2350 50  0000 C CNN
F 3 "" H 8850 2350 50  0000 C CNN
	1    8850 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR062
U 1 1 58122B8D
P 6750 2350
F 0 "#PWR062" H 6750 2100 50  0001 C CNN
F 1 "GND" H 6755 2177 50  0000 C CNN
F 2 "" H 6750 2350 50  0000 C CNN
F 3 "" H 6750 2350 50  0000 C CNN
	1    6750 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR063
U 1 1 58122B93
P 4650 2350
F 0 "#PWR063" H 4650 2100 50  0001 C CNN
F 1 "GND" H 4655 2177 50  0000 C CNN
F 2 "" H 4650 2350 50  0000 C CNN
F 3 "" H 4650 2350 50  0000 C CNN
	1    4650 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR064
U 1 1 58122B99
P 2550 2350
F 0 "#PWR064" H 2550 2100 50  0001 C CNN
F 1 "GND" H 2555 2177 50  0000 C CNN
F 2 "" H 2550 2350 50  0000 C CNN
F 3 "" H 2550 2350 50  0000 C CNN
	1    2550 2350
	1    0    0    -1  
$EndComp
$Comp
L LED1642GW U9
U 1 1 58122BE2
P 8100 1650
F 0 "U9" H 8100 2747 60  0000 C CNN
F 1 "LED1642GW" H 8100 2641 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 8100 2558 30  0000 C CNN
F 3 "" H 7100 1950 60  0000 C CNN
	1    8100 1650
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U7
U 1 1 58122BE9
P 6000 1650
F 0 "U7" H 6000 2747 60  0000 C CNN
F 1 "LED1642GW" H 6000 2641 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 6000 2558 30  0000 C CNN
F 3 "" H 5000 1950 60  0000 C CNN
	1    6000 1650
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U5
U 1 1 58122BF0
P 3900 1650
F 0 "U5" H 3900 2747 60  0000 C CNN
F 1 "LED1642GW" H 3900 2641 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 3900 2558 30  0000 C CNN
F 3 "" H 2900 1950 60  0000 C CNN
	1    3900 1650
	0    -1   -1   0   
$EndComp
Text GLabel 1050 2350 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 3150 2350 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 5250 2350 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 7350 2350 3    60   Input ~ 0
LEDLOGICVDD
Wire Wire Line
	8050 2350 8050 3200
Wire Wire Line
	8350 2350 8350 2650
Wire Wire Line
	6250 2350 6250 2650
Wire Wire Line
	4150 2350 4150 2650
Wire Wire Line
	2050 2350 2050 2650
Wire Wire Line
	7850 2350 7850 2650
Wire Wire Line
	7750 2350 7750 2650
Wire Wire Line
	7650 2350 7650 2650
Wire Wire Line
	5750 2350 5750 2650
Wire Wire Line
	5650 2350 5650 2650
Wire Wire Line
	5550 2350 5550 2650
Wire Wire Line
	3650 2350 3650 2650
Wire Wire Line
	3550 2350 3550 2650
Wire Wire Line
	3450 2350 3450 2650
Wire Wire Line
	1550 2350 1550 2650
Wire Wire Line
	1450 2350 1450 2650
Wire Wire Line
	1350 2350 1350 2650
Wire Wire Line
	1250 2350 1250 2650
Wire Wire Line
	3350 2600 1750 2600
Wire Wire Line
	7550 2600 7550 2350
Wire Wire Line
	5950 2600 7550 2600
Wire Wire Line
	5950 2350 5950 2600
Wire Wire Line
	5450 2600 5450 2350
Wire Wire Line
	3850 2600 5450 2600
Wire Wire Line
	3850 2350 3850 2600
Wire Wire Line
	3350 2350 3350 2600
Wire Wire Line
	1750 2600 1750 2350
$Comp
L LED1642GW U3
U 1 1 58122BF8
P 1800 1650
F 0 "U3" H 1800 2747 60  0000 C CNN
F 1 "LED1642GW" H 1800 2641 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 1800 2558 30  0000 C CNN
F 3 "" H 800 1950 60  0000 C CNN
	1    1800 1650
	0    -1   -1   0   
$EndComp
Text GLabel 8850 3750 1    60   Input ~ 0
ce0
Text GLabel 8750 3750 1    60   Input ~ 0
co0
Text GLabel 8650 3750 1    60   Input ~ 0
ce1
Text GLabel 8550 3750 1    60   Input ~ 0
co1
Text GLabel 8450 3750 1    60   Input ~ 0
ce2
Text GLabel 8350 3750 1    60   Input ~ 0
co2
Text GLabel 8250 3750 1    60   Input ~ 0
ce3
Text GLabel 8150 3750 1    60   Input ~ 0
co3
Text GLabel 8050 3750 1    60   Input ~ 0
ce4
Text GLabel 7950 3750 1    60   Input ~ 0
co4
Text GLabel 7850 3750 1    60   Input ~ 0
ce5
Text GLabel 7750 3750 1    60   Input ~ 0
co5
Text GLabel 7650 3750 1    60   Input ~ 0
ce6
Text GLabel 7550 3750 1    60   Input ~ 0
co6
Text GLabel 7450 3750 1    60   Input ~ 0
ce7
Text GLabel 7350 3750 1    60   Input ~ 0
co7
Text GLabel 6750 3750 1    60   Input ~ 0
ce8
Text GLabel 6650 3750 1    60   Input ~ 0
co8
Text GLabel 6550 3750 1    60   Input ~ 0
ce9
Text GLabel 6450 3750 1    60   Input ~ 0
co9
Text GLabel 4650 3750 1    60   Input ~ 0
ce16
Text GLabel 4550 3750 1    60   Input ~ 0
co16
Text GLabel 4450 3750 1    60   Input ~ 0
ce17
Text GLabel 4350 3750 1    60   Input ~ 0
co17
Text GLabel 4250 3750 1    60   Input ~ 0
ce18
Text GLabel 4150 3750 1    60   Input ~ 0
co18
Text GLabel 4050 3750 1    60   Input ~ 0
ce19
Text GLabel 3950 3750 1    60   Input ~ 0
co19
Text GLabel 3850 3750 1    60   Input ~ 0
ce20
Text GLabel 3750 3750 1    60   Input ~ 0
co20
Text GLabel 3650 3750 1    60   Input ~ 0
ce21
Text GLabel 3550 3750 1    60   Input ~ 0
co21
Text GLabel 3450 3750 1    60   Input ~ 0
ce22
Text GLabel 3350 3750 1    60   Input ~ 0
co22
Text GLabel 3250 3750 1    60   Input ~ 0
ce23
Text GLabel 3150 3750 1    60   Input ~ 0
co23
Text GLabel 1350 3750 1    60   Input ~ 0
ce30
Text GLabel 1250 3750 1    60   Input ~ 0
co30
Text GLabel 1150 3750 1    60   Input ~ 0
ce31
Text GLabel 1050 3750 1    60   Input ~ 0
co31
Text GLabel 8850 950  1    60   Input ~ 0
ce32
Text GLabel 8750 950  1    60   Input ~ 0
co32
Text GLabel 8650 950  1    60   Input ~ 0
ce33
Text GLabel 8550 950  1    60   Input ~ 0
co33
Text GLabel 8450 950  1    60   Input ~ 0
ce34
Text GLabel 8350 950  1    60   Input ~ 0
co34
Text GLabel 8250 950  1    60   Input ~ 0
ce35
Text GLabel 8150 950  1    60   Input ~ 0
co35
Text GLabel 8050 950  1    60   Input ~ 0
ce36
Text GLabel 7950 950  1    60   Input ~ 0
co36
Text GLabel 7850 950  1    60   Input ~ 0
ce37
Text GLabel 7750 950  1    60   Input ~ 0
co37
Text GLabel 7650 950  1    60   Input ~ 0
ce38
Text GLabel 7550 950  1    60   Input ~ 0
co38
Text GLabel 7450 950  1    60   Input ~ 0
ce39
Text GLabel 7350 950  1    60   Input ~ 0
co39
Text GLabel 6750 950  1    60   Input ~ 0
ce40
Text GLabel 6650 950  1    60   Input ~ 0
co40
Text GLabel 6550 950  1    60   Input ~ 0
ce41
Text GLabel 6450 950  1    60   Input ~ 0
co41
Text GLabel 6350 950  1    60   Input ~ 0
ce42
Text GLabel 6250 950  1    60   Input ~ 0
co42
Text GLabel 6150 950  1    60   Input ~ 0
ce43
Text GLabel 6050 950  1    60   Input ~ 0
co43
Text GLabel 5950 950  1    60   Input ~ 0
ce44
Text GLabel 5850 950  1    60   Input ~ 0
co44
Text GLabel 5750 950  1    60   Input ~ 0
ce45
Text GLabel 5650 950  1    60   Input ~ 0
co45
Text GLabel 5550 950  1    60   Input ~ 0
ce46
Text GLabel 5450 950  1    60   Input ~ 0
co46
Text GLabel 5350 950  1    60   Input ~ 0
ce47
Text GLabel 5250 950  1    60   Input ~ 0
co47
Text GLabel 4650 950  1    60   Input ~ 0
ce48
Text GLabel 4550 950  1    60   Input ~ 0
co48
Text GLabel 4450 950  1    60   Input ~ 0
ce49
Text GLabel 4350 950  1    60   Input ~ 0
co49
Text GLabel 4250 950  1    60   Input ~ 0
ce50
Text GLabel 4150 950  1    60   Input ~ 0
co50
Text GLabel 4050 950  1    60   Input ~ 0
ce51
Text GLabel 3950 950  1    60   Input ~ 0
co51
Text GLabel 3850 950  1    60   Input ~ 0
ce52
Text GLabel 3750 950  1    60   Input ~ 0
co52
Text GLabel 3650 950  1    60   Input ~ 0
ce53
Text GLabel 3550 950  1    60   Input ~ 0
co53
Text GLabel 3450 950  1    60   Input ~ 0
ce54
Text GLabel 3350 950  1    60   Input ~ 0
co54
Text GLabel 3250 950  1    60   Input ~ 0
ce55
Text GLabel 3150 950  1    60   Input ~ 0
co55
Text GLabel 2550 950  1    60   Input ~ 0
ce56
Text GLabel 2450 950  1    60   Input ~ 0
co56
Text GLabel 2350 950  1    60   Input ~ 0
ce57
Text GLabel 2250 950  1    60   Input ~ 0
co57
Text GLabel 2150 950  1    60   Input ~ 0
ce58
Text GLabel 2050 950  1    60   Input ~ 0
co58
Text GLabel 1950 950  1    60   Input ~ 0
ce59
Text GLabel 1850 950  1    60   Input ~ 0
co59
Text GLabel 1750 950  1    60   Input ~ 0
ce60
Text GLabel 1650 950  1    60   Input ~ 0
co60
Text GLabel 1550 950  1    60   Input ~ 0
ce61
Text GLabel 1450 950  1    60   Input ~ 0
co61
Text GLabel 1350 950  1    60   Input ~ 0
ce62
Text GLabel 1250 950  1    60   Input ~ 0
co62
Text GLabel 1150 950  1    60   Input ~ 0
ce63
Text GLabel 1050 950  1    60   Input ~ 0
co63
Text GLabel 8050 5450 3    60   Input ~ 0
COLSEROUT
$Comp
L GND #PWR065
U 1 1 5811DEEE
P 8350 5750
F 0 "#PWR065" H 8350 5500 50  0001 C CNN
F 1 "GND" H 8355 5577 50  0000 C CNN
F 2 "" H 8350 5750 50  0000 C CNN
F 3 "" H 8350 5750 50  0000 C CNN
	1    8350 5750
	1    0    0    -1  
$EndComp
$Comp
L R R25
U 1 1 5811DEF4
P 8350 5600
F 0 "R25" H 8420 5646 50  0000 L CNN
F 1 "22k" H 8420 5555 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 8280 5600 50  0001 C CNN
F 3 "" H 8350 5600 50  0000 C CNN
	1    8350 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR066
U 1 1 5811DEFA
P 6250 5750
F 0 "#PWR066" H 6250 5500 50  0001 C CNN
F 1 "GND" H 6255 5577 50  0000 C CNN
F 2 "" H 6250 5750 50  0000 C CNN
F 3 "" H 6250 5750 50  0000 C CNN
	1    6250 5750
	1    0    0    -1  
$EndComp
$Comp
L R R23
U 1 1 5811DF00
P 6250 5600
F 0 "R23" H 6320 5646 50  0000 L CNN
F 1 "22k" H 6320 5555 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 6180 5600 50  0001 C CNN
F 3 "" H 6250 5600 50  0000 C CNN
	1    6250 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR067
U 1 1 5811DF06
P 4150 5750
F 0 "#PWR067" H 4150 5500 50  0001 C CNN
F 1 "GND" H 4155 5577 50  0000 C CNN
F 2 "" H 4150 5750 50  0000 C CNN
F 3 "" H 4150 5750 50  0000 C CNN
	1    4150 5750
	1    0    0    -1  
$EndComp
$Comp
L R R21
U 1 1 5811DF0C
P 4150 5600
F 0 "R21" H 4220 5646 50  0000 L CNN
F 1 "22k" H 4220 5555 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 4080 5600 50  0001 C CNN
F 3 "" H 4150 5600 50  0000 C CNN
	1    4150 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR068
U 1 1 5811DF12
P 2050 5750
F 0 "#PWR068" H 2050 5500 50  0001 C CNN
F 1 "GND" H 2055 5577 50  0000 C CNN
F 2 "" H 2050 5750 50  0000 C CNN
F 3 "" H 2050 5750 50  0000 C CNN
	1    2050 5750
	1    0    0    -1  
$EndComp
$Comp
L R R19
U 1 1 5811DF18
P 2050 5600
F 0 "R19" H 2120 5646 50  0000 L CNN
F 1 "22k" H 2120 5555 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 1980 5600 50  0001 C CNN
F 3 "" H 2050 5600 50  0000 C CNN
	1    2050 5600
	1    0    0    -1  
$EndComp
Text GLabel 7850 5450 3    60   Input ~ 0
COLPWCLK1
Text GLabel 7750 5450 3    60   Input ~ 0
COLLATCH
Text GLabel 7650 5450 3    60   Input ~ 0
COLCLK
Text GLabel 5750 5450 3    60   Input ~ 0
COLPWCLK0
Text GLabel 5650 5450 3    60   Input ~ 0
COLLATCH
Text GLabel 5550 5450 3    60   Input ~ 0
COLCLK
Text GLabel 3650 5450 3    60   Input ~ 0
COLPWCLK1
Text GLabel 3550 5450 3    60   Input ~ 0
COLLATCH
Text GLabel 3450 5450 3    60   Input ~ 0
COLCLK
Text GLabel 1550 5450 3    60   Input ~ 0
COLPWCLK0
Text GLabel 1450 5450 3    60   Input ~ 0
COLLATCH
Text GLabel 1350 5450 3    60   Input ~ 0
COLCLK
$Comp
L GND #PWR069
U 1 1 5811DF2B
P 8850 5150
F 0 "#PWR069" H 8850 4900 50  0001 C CNN
F 1 "GND" H 8855 4977 50  0000 C CNN
F 2 "" H 8850 5150 50  0000 C CNN
F 3 "" H 8850 5150 50  0000 C CNN
	1    8850 5150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR070
U 1 1 5811DF31
P 6750 5150
F 0 "#PWR070" H 6750 4900 50  0001 C CNN
F 1 "GND" H 6755 4977 50  0000 C CNN
F 2 "" H 6750 5150 50  0000 C CNN
F 3 "" H 6750 5150 50  0000 C CNN
	1    6750 5150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR071
U 1 1 5811DF37
P 4650 5150
F 0 "#PWR071" H 4650 4900 50  0001 C CNN
F 1 "GND" H 4655 4977 50  0000 C CNN
F 2 "" H 4650 5150 50  0000 C CNN
F 3 "" H 4650 5150 50  0000 C CNN
	1    4650 5150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR072
U 1 1 5811DF3D
P 2550 5150
F 0 "#PWR072" H 2550 4900 50  0001 C CNN
F 1 "GND" H 2555 4977 50  0000 C CNN
F 2 "" H 2550 5150 50  0000 C CNN
F 3 "" H 2550 5150 50  0000 C CNN
	1    2550 5150
	1    0    0    -1  
$EndComp
$Comp
L LED1642GW U10
U 1 1 5811DF43
P 8100 4450
F 0 "U10" H 8100 5547 60  0000 C CNN
F 1 "LED1642GW" H 8100 5441 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 8100 5358 30  0000 C CNN
F 3 "" H 7100 4750 60  0000 C CNN
	1    8100 4450
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U8
U 1 1 5811DF49
P 6000 4450
F 0 "U8" H 6000 5547 60  0000 C CNN
F 1 "LED1642GW" H 6000 5441 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 6000 5358 30  0000 C CNN
F 3 "" H 5000 4750 60  0000 C CNN
	1    6000 4450
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U6
U 1 1 5811DF4F
P 3900 4450
F 0 "U6" H 3900 5547 60  0000 C CNN
F 1 "LED1642GW" H 3900 5441 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 3900 5358 30  0000 C CNN
F 3 "" H 2900 4750 60  0000 C CNN
	1    3900 4450
	0    -1   -1   0   
$EndComp
Text GLabel 1050 5150 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 3150 5150 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 5250 5150 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 7350 5150 3    60   Input ~ 0
LEDLOGICVDD
Wire Wire Line
	8050 5150 8050 5450
Wire Wire Line
	8350 5150 8350 5450
Wire Wire Line
	6250 5150 6250 5450
Wire Wire Line
	4150 5150 4150 5450
Wire Wire Line
	2050 5150 2050 5450
Wire Wire Line
	7850 5150 7850 5450
Wire Wire Line
	7750 5150 7750 5450
Wire Wire Line
	7650 5150 7650 5450
Wire Wire Line
	5750 5150 5750 5450
Wire Wire Line
	5650 5150 5650 5450
Wire Wire Line
	5550 5150 5550 5450
Wire Wire Line
	3650 5150 3650 5450
Wire Wire Line
	3550 5150 3550 5450
Wire Wire Line
	3450 5150 3450 5450
Wire Wire Line
	1550 5150 1550 5450
Wire Wire Line
	1450 5150 1450 5450
Wire Wire Line
	1350 5150 1350 5450
Wire Wire Line
	1250 5150 1250 6050
Wire Wire Line
	3350 5400 1750 5400
Wire Wire Line
	7550 5400 7550 5150
Wire Wire Line
	5950 5400 7550 5400
Wire Wire Line
	5950 5150 5950 5400
Wire Wire Line
	5450 5400 5450 5150
Wire Wire Line
	3850 5400 5450 5400
Wire Wire Line
	3850 5150 3850 5400
Wire Wire Line
	3350 5150 3350 5400
Wire Wire Line
	1750 5400 1750 5150
$Comp
L LED1642GW U4
U 1 1 5811DF76
P 1800 4450
F 0 "U4" H 1800 5547 60  0000 C CNN
F 1 "LED1642GW" H 1800 5441 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 1800 5358 30  0000 C CNN
F 3 "" H 800 4750 60  0000 C CNN
	1    1800 4450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8050 3200 600  3200
Wire Wire Line
	600  3200 600  6050
Wire Wire Line
	600  6050 1250 6050
Text GLabel 1450 3750 1    60   Input ~ 0
co29
Text GLabel 1550 3750 1    60   Input ~ 0
ce29
Text GLabel 1650 3750 1    60   Input ~ 0
co28
Text GLabel 1750 3750 1    60   Input ~ 0
ce28
Text GLabel 1850 3750 1    60   Input ~ 0
co27
Text GLabel 1950 3750 1    60   Input ~ 0
ce27
Text GLabel 2050 3750 1    60   Input ~ 0
co26
Text GLabel 2150 3750 1    60   Input ~ 0
ce26
Text GLabel 2250 3750 1    60   Input ~ 0
co25
Text GLabel 2350 3750 1    60   Input ~ 0
ce25
Text GLabel 2450 3750 1    60   Input ~ 0
co24
Text GLabel 2550 3750 1    60   Input ~ 0
ce24
Text GLabel 5250 3750 1    60   Input ~ 0
co15
Text GLabel 5350 3750 1    60   Input ~ 0
ce15
Text GLabel 5450 3750 1    60   Input ~ 0
co14
Text GLabel 5550 3750 1    60   Input ~ 0
ce14
Text GLabel 5650 3750 1    60   Input ~ 0
co13
Text GLabel 5750 3750 1    60   Input ~ 0
ce13
Text GLabel 5850 3750 1    60   Input ~ 0
co12
Text GLabel 5950 3750 1    60   Input ~ 0
ce12
Text GLabel 6050 3750 1    60   Input ~ 0
co11
Text GLabel 6150 3750 1    60   Input ~ 0
ce11
Text GLabel 6250 3750 1    60   Input ~ 0
co10
Text GLabel 6350 3750 1    60   Input ~ 0
ce10
$Comp
L C C5
U 1 1 581833BD
P 9400 2600
F 0 "C5" H 9515 2646 50  0000 L CNN
F 1 "1u" H 9515 2555 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9515 2509 50  0001 L CNN
F 3 "" H 9400 2600 50  0000 C CNN
	1    9400 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR073
U 1 1 581833C4
P 9400 2750
F 0 "#PWR073" H 9400 2500 50  0001 C CNN
F 1 "GND" H 9405 2577 50  0000 C CNN
F 2 "" H 9400 2750 50  0000 C CNN
F 3 "" H 9400 2750 50  0000 C CNN
	1    9400 2750
	1    0    0    -1  
$EndComp
Text GLabel 9400 2450 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L C C7
U 1 1 58183AA7
P 9700 2600
F 0 "C7" H 9815 2646 50  0000 L CNN
F 1 "1u" H 9815 2555 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9815 2509 50  0001 L CNN
F 3 "" H 9700 2600 50  0000 C CNN
	1    9700 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR074
U 1 1 58183AAD
P 9700 2750
F 0 "#PWR074" H 9700 2500 50  0001 C CNN
F 1 "GND" H 9705 2577 50  0000 C CNN
F 2 "" H 9700 2750 50  0000 C CNN
F 3 "" H 9700 2750 50  0000 C CNN
	1    9700 2750
	1    0    0    -1  
$EndComp
Text GLabel 9700 2450 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L C C9
U 1 1 58183AFE
P 10000 2600
F 0 "C9" H 10115 2646 50  0000 L CNN
F 1 "1u" H 10115 2555 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10115 2509 50  0001 L CNN
F 3 "" H 10000 2600 50  0000 C CNN
	1    10000 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR075
U 1 1 58183B04
P 10000 2750
F 0 "#PWR075" H 10000 2500 50  0001 C CNN
F 1 "GND" H 10005 2577 50  0000 C CNN
F 2 "" H 10000 2750 50  0000 C CNN
F 3 "" H 10000 2750 50  0000 C CNN
	1    10000 2750
	1    0    0    -1  
$EndComp
Text GLabel 10000 2450 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L C C11
U 1 1 58183B0B
P 10300 2600
F 0 "C11" H 10415 2646 50  0000 L CNN
F 1 "1u" H 10415 2555 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10415 2509 50  0001 L CNN
F 3 "" H 10300 2600 50  0000 C CNN
	1    10300 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR076
U 1 1 58183B11
P 10300 2750
F 0 "#PWR076" H 10300 2500 50  0001 C CNN
F 1 "GND" H 10305 2577 50  0000 C CNN
F 2 "" H 10300 2750 50  0000 C CNN
F 3 "" H 10300 2750 50  0000 C CNN
	1    10300 2750
	1    0    0    -1  
$EndComp
Text GLabel 10300 2450 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L C C6
U 1 1 58183BCC
P 9400 3900
F 0 "C6" H 9515 3946 50  0000 L CNN
F 1 "1u" H 9515 3855 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9515 3809 50  0001 L CNN
F 3 "" H 9400 3900 50  0000 C CNN
	1    9400 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR077
U 1 1 58183BD2
P 9400 4050
F 0 "#PWR077" H 9400 3800 50  0001 C CNN
F 1 "GND" H 9405 3877 50  0000 C CNN
F 2 "" H 9400 4050 50  0000 C CNN
F 3 "" H 9400 4050 50  0000 C CNN
	1    9400 4050
	1    0    0    -1  
$EndComp
Text GLabel 9400 3750 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L C C8
U 1 1 58183BD9
P 9700 3900
F 0 "C8" H 9815 3946 50  0000 L CNN
F 1 "1u" H 9815 3855 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9815 3809 50  0001 L CNN
F 3 "" H 9700 3900 50  0000 C CNN
	1    9700 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR078
U 1 1 58183BDF
P 9700 4050
F 0 "#PWR078" H 9700 3800 50  0001 C CNN
F 1 "GND" H 9705 3877 50  0000 C CNN
F 2 "" H 9700 4050 50  0000 C CNN
F 3 "" H 9700 4050 50  0000 C CNN
	1    9700 4050
	1    0    0    -1  
$EndComp
Text GLabel 9700 3750 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L C C10
U 1 1 58183BE6
P 10000 3900
F 0 "C10" H 10115 3946 50  0000 L CNN
F 1 "1u" H 10115 3855 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10115 3809 50  0001 L CNN
F 3 "" H 10000 3900 50  0000 C CNN
	1    10000 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR079
U 1 1 58183BEC
P 10000 4050
F 0 "#PWR079" H 10000 3800 50  0001 C CNN
F 1 "GND" H 10005 3877 50  0000 C CNN
F 2 "" H 10000 4050 50  0000 C CNN
F 3 "" H 10000 4050 50  0000 C CNN
	1    10000 4050
	1    0    0    -1  
$EndComp
Text GLabel 10000 3750 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L C C12
U 1 1 58183BF3
P 10300 3900
F 0 "C12" H 10415 3946 50  0000 L CNN
F 1 "1u" H 10415 3855 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10415 3809 50  0001 L CNN
F 3 "" H 10300 3900 50  0000 C CNN
	1    10300 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR080
U 1 1 58183BF9
P 10300 4050
F 0 "#PWR080" H 10300 3800 50  0001 C CNN
F 1 "GND" H 10305 3877 50  0000 C CNN
F 2 "" H 10300 4050 50  0000 C CNN
F 3 "" H 10300 4050 50  0000 C CNN
	1    10300 4050
	1    0    0    -1  
$EndComp
Text GLabel 10300 3750 1    60   Input ~ 0
LEDLOGICVDD
$Comp
L GND #PWR081
U 1 1 583CDD16
P 9750 4900
F 0 "#PWR081" H 9750 4650 50  0001 C CNN
F 1 "GND" H 9755 4727 50  0000 C CNN
F 2 "" H 9750 4900 50  0000 C CNN
F 3 "" H 9750 4900 50  0000 C CNN
	1    9750 4900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR082
U 1 1 583CDD34
P 10700 4900
F 0 "#PWR082" H 10700 4650 50  0001 C CNN
F 1 "GND" H 10705 4727 50  0000 C CNN
F 2 "" H 10700 4900 50  0000 C CNN
F 3 "" H 10700 4900 50  0000 C CNN
	1    10700 4900
	1    0    0    -1  
$EndComp
$Comp
L Ferrite_Bead_Small L5
U 1 1 583CF8BE
P 10200 4650
F 0 "L5" V 10400 4700 50  0000 C CNN
F 1 "BLM18RK102SN1 1k @ 100MHz" V 10300 4450 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 10200 4650 50  0001 C CNN
F 3 "" H 10200 4650 50  0000 C CNN
	1    10200 4650
	0    -1   1    0   
$EndComp
$Comp
L Jumper_NO_Small JP1
U 1 1 583D6FF6
P 10150 4550
F 0 "JP1" H 10150 4457 50  0000 C CNN
F 1 "Jumper_NO_Small" H 10150 4644 50  0001 C CNN
F 2 "components:SolderJumper" H 10150 4644 50  0001 C CNN
F 3 "" H 10150 4550 50  0000 C CNN
	1    10150 4550
	-1   0    0    1   
$EndComp
Wire Wire Line
	10700 4900 10700 4550
Wire Wire Line
	10700 4550 10250 4550
Wire Wire Line
	10500 4650 10500 4550
Connection ~ 10500 4550
Wire Wire Line
	10050 4550 9750 4550
Wire Wire Line
	9750 4550 9750 4900
Wire Wire Line
	9900 4650 9900 4550
Connection ~ 9900 4550
Wire Wire Line
	10300 4650 10500 4650
Wire Wire Line
	10100 4650 9900 4650
$Comp
L TEST_1P W10
U 1 1 58ABAB2B
P 1200 2450
F 0 "W10" V 1250 2500 50  0000 C CNN
F 1 "TEST_1P" V 1304 2524 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1400 2450 50  0001 C CNN
F 3 "" H 1400 2450 50  0000 C CNN
	1    1200 2450
	-1   0    0    1   
$EndComp
Wire Wire Line
	1200 2450 1250 2450
Connection ~ 1250 2450
$Comp
L TEST_1P W13
U 1 1 58ABB0BD
P 3300 2450
F 0 "W13" V 3350 2500 50  0000 C CNN
F 1 "TEST_1P" V 3404 2524 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 3500 2450 50  0001 C CNN
F 3 "" H 3500 2450 50  0000 C CNN
	1    3300 2450
	-1   0    0    1   
$EndComp
Wire Wire Line
	3300 2450 3350 2450
Connection ~ 3350 2450
$Comp
L TEST_1P W15
U 1 1 58ABB383
P 5400 2450
F 0 "W15" V 5450 2500 50  0000 C CNN
F 1 "TEST_1P" V 5504 2524 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 5600 2450 50  0001 C CNN
F 3 "" H 5600 2450 50  0000 C CNN
	1    5400 2450
	-1   0    0    1   
$EndComp
Wire Wire Line
	5400 2450 5450 2450
Connection ~ 5450 2450
$Comp
L TEST_1P W17
U 1 1 58ABB414
P 7500 2450
F 0 "W17" V 7550 2500 50  0000 C CNN
F 1 "TEST_1P" V 7604 2524 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7700 2450 50  0001 C CNN
F 3 "" H 7700 2450 50  0000 C CNN
	1    7500 2450
	-1   0    0    1   
$EndComp
Wire Wire Line
	7500 2450 7550 2450
Connection ~ 7550 2450
$Comp
L TEST_1P W12
U 1 1 58ABB953
P 1200 5250
F 0 "W12" V 1250 5300 50  0000 C CNN
F 1 "TEST_1P" V 1304 5324 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1400 5250 50  0001 C CNN
F 3 "" H 1400 5250 50  0000 C CNN
	1    1200 5250
	-1   0    0    1   
$EndComp
Wire Wire Line
	1200 5250 1250 5250
Connection ~ 1250 5250
$Comp
L TEST_1P W14
U 1 1 58ABB9DC
P 3300 5250
F 0 "W14" V 3350 5300 50  0000 C CNN
F 1 "TEST_1P" V 3404 5324 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 3500 5250 50  0001 C CNN
F 3 "" H 3500 5250 50  0000 C CNN
	1    3300 5250
	-1   0    0    1   
$EndComp
Wire Wire Line
	3300 5250 3350 5250
Connection ~ 3350 5250
$Comp
L TEST_1P W16
U 1 1 58ABBC10
P 5400 5250
F 0 "W16" V 5450 5300 50  0000 C CNN
F 1 "TEST_1P" V 5504 5324 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 5600 5250 50  0001 C CNN
F 3 "" H 5600 5250 50  0000 C CNN
	1    5400 5250
	-1   0    0    1   
$EndComp
Wire Wire Line
	5400 5250 5450 5250
Connection ~ 5450 5250
$Comp
L TEST_1P W18
U 1 1 58ABBDEF
P 7500 5250
F 0 "W18" V 7500 5500 50  0000 C CNN
F 1 "TEST_1P" V 7604 5324 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7700 5250 50  0001 C CNN
F 3 "" H 7700 5250 50  0000 C CNN
	1    7500 5250
	-1   0    0    1   
$EndComp
Wire Wire Line
	7500 5250 7550 5250
Connection ~ 7550 5250
$Comp
L TEST_1P W19
U 1 1 58ABBF5D
P 7600 5250
F 0 "W19" V 7500 5350 50  0000 C CNN
F 1 "TEST_1P" V 7704 5324 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7800 5250 50  0001 C CNN
F 3 "" H 7800 5250 50  0000 C CNN
	1    7600 5250
	-1   0    0    1   
$EndComp
Wire Wire Line
	7600 5250 7650 5250
Connection ~ 7650 5250
$Comp
L TEST_1P W8
U 1 1 58AD1B5D
P 1300 2450
F 0 "W8" V 1300 2350 50  0000 C CNN
F 1 "TEST_1P" V 1404 2524 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1500 2450 50  0001 C CNN
F 3 "" H 1500 2450 50  0000 C CNN
	1    1300 2450
	-1   0    0    1   
$EndComp
Wire Wire Line
	1300 2450 1350 2450
Connection ~ 1350 2450
$EndSCHEMATC
