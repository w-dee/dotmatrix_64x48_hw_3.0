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
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:DeeComponents
LIBS:bb-ch340t
LIBS:wroom-02
LIBS:dotmatrix_64x24-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 8
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
L GND #PWR64
U 1 1 58122B46
P 9100 3400
F 0 "#PWR64" H 9100 3150 50  0001 C CNN
F 1 "GND" H 9105 3227 50  0000 C CNN
F 2 "" H 9100 3400 50  0000 C CNN
F 3 "" H 9100 3400 50  0000 C CNN
	1    9100 3400
	1    0    0    -1  
$EndComp
$Comp
L R R24
U 1 1 58122B4C
P 9100 3250
F 0 "R24" H 9170 3296 50  0000 L CNN
F 1 "R" H 9170 3205 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 9030 3250 50  0001 C CNN
F 3 "" H 9100 3250 50  0000 C CNN
	1    9100 3250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR60
U 1 1 58122B53
P 7000 3400
F 0 "#PWR60" H 7000 3150 50  0001 C CNN
F 1 "GND" H 7005 3227 50  0000 C CNN
F 2 "" H 7000 3400 50  0000 C CNN
F 3 "" H 7000 3400 50  0000 C CNN
	1    7000 3400
	1    0    0    -1  
$EndComp
$Comp
L R R22
U 1 1 58122B59
P 7000 3250
F 0 "R22" H 7070 3296 50  0000 L CNN
F 1 "R" H 7070 3205 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 6930 3250 50  0001 C CNN
F 3 "" H 7000 3250 50  0000 C CNN
	1    7000 3250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR56
U 1 1 58122B60
P 4900 3400
F 0 "#PWR56" H 4900 3150 50  0001 C CNN
F 1 "GND" H 4905 3227 50  0000 C CNN
F 2 "" H 4900 3400 50  0000 C CNN
F 3 "" H 4900 3400 50  0000 C CNN
	1    4900 3400
	1    0    0    -1  
$EndComp
$Comp
L R R20
U 1 1 58122B66
P 4900 3250
F 0 "R20" H 4970 3296 50  0000 L CNN
F 1 "R" H 4970 3205 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 4830 3250 50  0001 C CNN
F 3 "" H 4900 3250 50  0000 C CNN
	1    4900 3250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR52
U 1 1 58122B6D
P 2800 3400
F 0 "#PWR52" H 2800 3150 50  0001 C CNN
F 1 "GND" H 2805 3227 50  0000 C CNN
F 2 "" H 2800 3400 50  0000 C CNN
F 3 "" H 2800 3400 50  0000 C CNN
	1    2800 3400
	1    0    0    -1  
$EndComp
$Comp
L R R18
U 1 1 58122B73
P 2800 3250
F 0 "R18" H 2870 3296 50  0000 L CNN
F 1 "R" H 2870 3205 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 2730 3250 50  0001 C CNN
F 3 "" H 2800 3250 50  0000 C CNN
	1    2800 3250
	1    0    0    -1  
$EndComp
Text GLabel 8600 3100 3    60   Input ~ 0
COLPWCLK1
Text GLabel 8500 3100 3    60   Input ~ 0
COLLATCH
Text GLabel 8400 3100 3    60   Input ~ 0
COLCLK
Text GLabel 6500 3100 3    60   Input ~ 0
COLPWCLK0
Text GLabel 6400 3100 3    60   Input ~ 0
COLLATCH
Text GLabel 6300 3100 3    60   Input ~ 0
COLCLK
Text GLabel 4400 3100 3    60   Input ~ 0
COLPWCLK1
Text GLabel 4300 3100 3    60   Input ~ 0
COLLATCH
Text GLabel 4200 3100 3    60   Input ~ 0
COLCLK
Text GLabel 2300 3100 3    60   Input ~ 0
COLPWCLK0
Text GLabel 2200 3100 3    60   Input ~ 0
COLLATCH
Text GLabel 2100 3100 3    60   Input ~ 0
COLCLK
Text GLabel 2000 3100 3    60   Input ~ 0
COLSER
$Comp
L GND #PWR66
U 1 1 58122B87
P 9600 2800
F 0 "#PWR66" H 9600 2550 50  0001 C CNN
F 1 "GND" H 9605 2627 50  0000 C CNN
F 2 "" H 9600 2800 50  0000 C CNN
F 3 "" H 9600 2800 50  0000 C CNN
	1    9600 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR62
U 1 1 58122B8D
P 7500 2800
F 0 "#PWR62" H 7500 2550 50  0001 C CNN
F 1 "GND" H 7505 2627 50  0000 C CNN
F 2 "" H 7500 2800 50  0000 C CNN
F 3 "" H 7500 2800 50  0000 C CNN
	1    7500 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR58
U 1 1 58122B93
P 5400 2800
F 0 "#PWR58" H 5400 2550 50  0001 C CNN
F 1 "GND" H 5405 2627 50  0000 C CNN
F 2 "" H 5400 2800 50  0000 C CNN
F 3 "" H 5400 2800 50  0000 C CNN
	1    5400 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR54
U 1 1 58122B99
P 3300 2800
F 0 "#PWR54" H 3300 2550 50  0001 C CNN
F 1 "GND" H 3305 2627 50  0000 C CNN
F 2 "" H 3300 2800 50  0000 C CNN
F 3 "" H 3300 2800 50  0000 C CNN
	1    3300 2800
	1    0    0    -1  
$EndComp
$Comp
L LED1642GW U9
U 1 1 58122BE2
P 8850 2100
F 0 "U9" H 8850 3197 60  0000 C CNN
F 1 "LED1642GW" H 8850 3091 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 8850 3008 30  0000 C CNN
F 3 "" H 7850 2400 60  0000 C CNN
	1    8850 2100
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U7
U 1 1 58122BE9
P 6750 2100
F 0 "U7" H 6750 3197 60  0000 C CNN
F 1 "LED1642GW" H 6750 3091 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 6750 3008 30  0000 C CNN
F 3 "" H 5750 2400 60  0000 C CNN
	1    6750 2100
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U5
U 1 1 58122BF0
P 4650 2100
F 0 "U5" H 4650 3197 60  0000 C CNN
F 1 "LED1642GW" H 4650 3091 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 4650 3008 30  0000 C CNN
F 3 "" H 3650 2400 60  0000 C CNN
	1    4650 2100
	0    -1   -1   0   
$EndComp
Text GLabel 1800 2800 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 3900 2800 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 6000 2800 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 8100 2800 3    60   Input ~ 0
LEDLOGICVDD
Wire Wire Line
	8800 2800 8800 3650
Wire Wire Line
	9100 2800 9100 3100
Wire Wire Line
	7000 2800 7000 3100
Wire Wire Line
	4900 2800 4900 3100
Wire Wire Line
	2800 2800 2800 3100
Wire Wire Line
	8600 2800 8600 3100
Wire Wire Line
	8500 2800 8500 3100
Wire Wire Line
	8400 2800 8400 3100
Wire Wire Line
	6500 2800 6500 3100
Wire Wire Line
	6400 2800 6400 3100
Wire Wire Line
	6300 2800 6300 3100
Wire Wire Line
	4400 2800 4400 3100
Wire Wire Line
	4300 2800 4300 3100
Wire Wire Line
	4200 2800 4200 3100
Wire Wire Line
	2300 2800 2300 3100
Wire Wire Line
	2200 2800 2200 3100
Wire Wire Line
	2100 2800 2100 3100
Wire Wire Line
	2000 2800 2000 3100
Wire Wire Line
	4100 3050 2500 3050
Wire Wire Line
	8300 3050 8300 2800
Wire Wire Line
	6700 3050 8300 3050
Wire Wire Line
	6700 2800 6700 3050
Wire Wire Line
	6200 3050 6200 2800
Wire Wire Line
	4600 3050 6200 3050
Wire Wire Line
	4600 2800 4600 3050
Wire Wire Line
	4100 2800 4100 3050
Wire Wire Line
	2500 3050 2500 2800
$Comp
L LED1642GW U3
U 1 1 58122BF8
P 2550 2100
F 0 "U3" H 2550 3197 60  0000 C CNN
F 1 "LED1642GW" H 2550 3091 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 2550 3008 30  0000 C CNN
F 3 "" H 1550 2400 60  0000 C CNN
	1    2550 2100
	0    -1   -1   0   
$EndComp
Text GLabel 1800 1400 1    60   Input ~ 0
ce0
Text GLabel 1900 1400 1    60   Input ~ 0
co0
Text GLabel 2000 1400 1    60   Input ~ 0
ce1
Text GLabel 2100 1400 1    60   Input ~ 0
co1
Text GLabel 2200 1400 1    60   Input ~ 0
ce2
Text GLabel 2300 1400 1    60   Input ~ 0
co2
Text GLabel 2400 1400 1    60   Input ~ 0
ce3
Text GLabel 2500 1400 1    60   Input ~ 0
co3
Text GLabel 2600 1400 1    60   Input ~ 0
ce4
Text GLabel 2700 1400 1    60   Input ~ 0
co4
Text GLabel 2800 1400 1    60   Input ~ 0
ce5
Text GLabel 2900 1400 1    60   Input ~ 0
co5
Text GLabel 3000 1400 1    60   Input ~ 0
ce6
Text GLabel 3100 1400 1    60   Input ~ 0
co6
Text GLabel 3200 1400 1    60   Input ~ 0
ce7
Text GLabel 3300 1400 1    60   Input ~ 0
co7
Text GLabel 3900 1400 1    60   Input ~ 0
ce8
Text GLabel 4000 1400 1    60   Input ~ 0
co8
Text GLabel 4100 1400 1    60   Input ~ 0
ce9
Text GLabel 4200 1400 1    60   Input ~ 0
co9
Text GLabel 6000 1400 1    60   Input ~ 0
ce16
Text GLabel 6100 1400 1    60   Input ~ 0
co16
Text GLabel 6200 1400 1    60   Input ~ 0
ce17
Text GLabel 6300 1400 1    60   Input ~ 0
co17
Text GLabel 6400 1400 1    60   Input ~ 0
ce18
Text GLabel 6500 1400 1    60   Input ~ 0
co18
Text GLabel 6600 1400 1    60   Input ~ 0
ce19
Text GLabel 6700 1400 1    60   Input ~ 0
co19
Text GLabel 6800 1400 1    60   Input ~ 0
ce20
Text GLabel 6900 1400 1    60   Input ~ 0
co20
Text GLabel 7000 1400 1    60   Input ~ 0
ce21
Text GLabel 7100 1400 1    60   Input ~ 0
co21
Text GLabel 7200 1400 1    60   Input ~ 0
ce22
Text GLabel 7300 1400 1    60   Input ~ 0
co22
Text GLabel 7400 1400 1    60   Input ~ 0
ce23
Text GLabel 7500 1400 1    60   Input ~ 0
co23
Text GLabel 9300 1400 1    60   Input ~ 0
ce30
Text GLabel 9400 1400 1    60   Input ~ 0
co30
Text GLabel 9500 1400 1    60   Input ~ 0
ce31
Text GLabel 9600 1400 1    60   Input ~ 0
co31
Text GLabel 1800 4200 1    60   Input ~ 0
ce32
Text GLabel 1900 4200 1    60   Input ~ 0
co32
Text GLabel 2000 4200 1    60   Input ~ 0
ce33
Text GLabel 2100 4200 1    60   Input ~ 0
co33
Text GLabel 2200 4200 1    60   Input ~ 0
ce34
Text GLabel 2300 4200 1    60   Input ~ 0
co34
Text GLabel 2400 4200 1    60   Input ~ 0
ce35
Text GLabel 2500 4200 1    60   Input ~ 0
co35
Text GLabel 2600 4200 1    60   Input ~ 0
ce36
Text GLabel 2700 4200 1    60   Input ~ 0
co36
Text GLabel 2800 4200 1    60   Input ~ 0
ce37
Text GLabel 2900 4200 1    60   Input ~ 0
co37
Text GLabel 3000 4200 1    60   Input ~ 0
ce38
Text GLabel 3100 4200 1    60   Input ~ 0
co38
Text GLabel 3200 4200 1    60   Input ~ 0
ce39
Text GLabel 3300 4200 1    60   Input ~ 0
co39
Text GLabel 3900 4200 1    60   Input ~ 0
ce40
Text GLabel 4000 4200 1    60   Input ~ 0
co40
Text GLabel 4100 4200 1    60   Input ~ 0
ce41
Text GLabel 4200 4200 1    60   Input ~ 0
co41
Text GLabel 4300 4200 1    60   Input ~ 0
ce42
Text GLabel 4400 4200 1    60   Input ~ 0
co42
Text GLabel 4500 4200 1    60   Input ~ 0
ce43
Text GLabel 4600 4200 1    60   Input ~ 0
co43
Text GLabel 4700 4200 1    60   Input ~ 0
ce44
Text GLabel 4800 4200 1    60   Input ~ 0
co44
Text GLabel 4900 4200 1    60   Input ~ 0
ce45
Text GLabel 5000 4200 1    60   Input ~ 0
co45
Text GLabel 5100 4200 1    60   Input ~ 0
ce46
Text GLabel 5200 4200 1    60   Input ~ 0
co46
Text GLabel 5300 4200 1    60   Input ~ 0
ce47
Text GLabel 5400 4200 1    60   Input ~ 0
co47
Text GLabel 6000 4200 1    60   Input ~ 0
ce48
Text GLabel 6100 4200 1    60   Input ~ 0
co48
Text GLabel 6200 4200 1    60   Input ~ 0
ce49
Text GLabel 6300 4200 1    60   Input ~ 0
co49
Text GLabel 6400 4200 1    60   Input ~ 0
ce50
Text GLabel 6500 4200 1    60   Input ~ 0
co50
Text GLabel 6600 4200 1    60   Input ~ 0
ce51
Text GLabel 6700 4200 1    60   Input ~ 0
co51
Text GLabel 6800 4200 1    60   Input ~ 0
ce52
Text GLabel 6900 4200 1    60   Input ~ 0
co52
Text GLabel 7000 4200 1    60   Input ~ 0
ce53
Text GLabel 7100 4200 1    60   Input ~ 0
co53
Text GLabel 7200 4200 1    60   Input ~ 0
ce54
Text GLabel 7300 4200 1    60   Input ~ 0
co54
Text GLabel 7400 4200 1    60   Input ~ 0
ce55
Text GLabel 7500 4200 1    60   Input ~ 0
co55
Text GLabel 8100 4200 1    60   Input ~ 0
ce56
Text GLabel 8200 4200 1    60   Input ~ 0
co56
Text GLabel 8300 4200 1    60   Input ~ 0
ce57
Text GLabel 8400 4200 1    60   Input ~ 0
co57
Text GLabel 8500 4200 1    60   Input ~ 0
ce58
Text GLabel 8600 4200 1    60   Input ~ 0
co58
Text GLabel 8700 4200 1    60   Input ~ 0
ce59
Text GLabel 8800 4200 1    60   Input ~ 0
co59
Text GLabel 8900 4200 1    60   Input ~ 0
ce60
Text GLabel 9000 4200 1    60   Input ~ 0
co60
Text GLabel 9100 4200 1    60   Input ~ 0
ce61
Text GLabel 9200 4200 1    60   Input ~ 0
co61
Text GLabel 9300 4200 1    60   Input ~ 0
ce62
Text GLabel 9400 4200 1    60   Input ~ 0
co62
Text GLabel 9500 4200 1    60   Input ~ 0
ce63
Text GLabel 9600 4200 1    60   Input ~ 0
co63
Text GLabel 8800 5900 3    60   Input ~ 0
COLSEROUT
$Comp
L GND #PWR65
U 1 1 5811DEEE
P 9100 6200
F 0 "#PWR65" H 9100 5950 50  0001 C CNN
F 1 "GND" H 9105 6027 50  0000 C CNN
F 2 "" H 9100 6200 50  0000 C CNN
F 3 "" H 9100 6200 50  0000 C CNN
	1    9100 6200
	1    0    0    -1  
$EndComp
$Comp
L R R25
U 1 1 5811DEF4
P 9100 6050
F 0 "R25" H 9170 6096 50  0000 L CNN
F 1 "R" H 9170 6005 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 9030 6050 50  0001 C CNN
F 3 "" H 9100 6050 50  0000 C CNN
	1    9100 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR61
U 1 1 5811DEFA
P 7000 6200
F 0 "#PWR61" H 7000 5950 50  0001 C CNN
F 1 "GND" H 7005 6027 50  0000 C CNN
F 2 "" H 7000 6200 50  0000 C CNN
F 3 "" H 7000 6200 50  0000 C CNN
	1    7000 6200
	1    0    0    -1  
$EndComp
$Comp
L R R23
U 1 1 5811DF00
P 7000 6050
F 0 "R23" H 7070 6096 50  0000 L CNN
F 1 "R" H 7070 6005 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 6930 6050 50  0001 C CNN
F 3 "" H 7000 6050 50  0000 C CNN
	1    7000 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR57
U 1 1 5811DF06
P 4900 6200
F 0 "#PWR57" H 4900 5950 50  0001 C CNN
F 1 "GND" H 4905 6027 50  0000 C CNN
F 2 "" H 4900 6200 50  0000 C CNN
F 3 "" H 4900 6200 50  0000 C CNN
	1    4900 6200
	1    0    0    -1  
$EndComp
$Comp
L R R21
U 1 1 5811DF0C
P 4900 6050
F 0 "R21" H 4970 6096 50  0000 L CNN
F 1 "R" H 4970 6005 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 4830 6050 50  0001 C CNN
F 3 "" H 4900 6050 50  0000 C CNN
	1    4900 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR53
U 1 1 5811DF12
P 2800 6200
F 0 "#PWR53" H 2800 5950 50  0001 C CNN
F 1 "GND" H 2805 6027 50  0000 C CNN
F 2 "" H 2800 6200 50  0000 C CNN
F 3 "" H 2800 6200 50  0000 C CNN
	1    2800 6200
	1    0    0    -1  
$EndComp
$Comp
L R R19
U 1 1 5811DF18
P 2800 6050
F 0 "R19" H 2870 6096 50  0000 L CNN
F 1 "R" H 2870 6005 50  0000 L CNN
F 2 "Resistors_Universal:Resistor_SMDuniversal_0805to1206_HandSoldering" V 2730 6050 50  0001 C CNN
F 3 "" H 2800 6050 50  0000 C CNN
	1    2800 6050
	1    0    0    -1  
$EndComp
Text GLabel 8600 5900 3    60   Input ~ 0
COLPWCLK1
Text GLabel 8500 5900 3    60   Input ~ 0
COLLATCH
Text GLabel 8400 5900 3    60   Input ~ 0
COLCLK
Text GLabel 6500 5900 3    60   Input ~ 0
COLPWCLK0
Text GLabel 6400 5900 3    60   Input ~ 0
COLLATCH
Text GLabel 6300 5900 3    60   Input ~ 0
COLCLK
Text GLabel 4400 5900 3    60   Input ~ 0
COLPWCLK1
Text GLabel 4300 5900 3    60   Input ~ 0
COLLATCH
Text GLabel 4200 5900 3    60   Input ~ 0
COLCLK
Text GLabel 2300 5900 3    60   Input ~ 0
COLPWCLK0
Text GLabel 2200 5900 3    60   Input ~ 0
COLLATCH
Text GLabel 2100 5900 3    60   Input ~ 0
COLCLK
$Comp
L GND #PWR67
U 1 1 5811DF2B
P 9600 5600
F 0 "#PWR67" H 9600 5350 50  0001 C CNN
F 1 "GND" H 9605 5427 50  0000 C CNN
F 2 "" H 9600 5600 50  0000 C CNN
F 3 "" H 9600 5600 50  0000 C CNN
	1    9600 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR63
U 1 1 5811DF31
P 7500 5600
F 0 "#PWR63" H 7500 5350 50  0001 C CNN
F 1 "GND" H 7505 5427 50  0000 C CNN
F 2 "" H 7500 5600 50  0000 C CNN
F 3 "" H 7500 5600 50  0000 C CNN
	1    7500 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR59
U 1 1 5811DF37
P 5400 5600
F 0 "#PWR59" H 5400 5350 50  0001 C CNN
F 1 "GND" H 5405 5427 50  0000 C CNN
F 2 "" H 5400 5600 50  0000 C CNN
F 3 "" H 5400 5600 50  0000 C CNN
	1    5400 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR55
U 1 1 5811DF3D
P 3300 5600
F 0 "#PWR55" H 3300 5350 50  0001 C CNN
F 1 "GND" H 3305 5427 50  0000 C CNN
F 2 "" H 3300 5600 50  0000 C CNN
F 3 "" H 3300 5600 50  0000 C CNN
	1    3300 5600
	1    0    0    -1  
$EndComp
$Comp
L LED1642GW U10
U 1 1 5811DF43
P 8850 4900
F 0 "U10" H 8850 5997 60  0000 C CNN
F 1 "LED1642GW" H 8850 5891 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 8850 5808 30  0000 C CNN
F 3 "" H 7850 5200 60  0000 C CNN
	1    8850 4900
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U8
U 1 1 5811DF49
P 6750 4900
F 0 "U8" H 6750 5997 60  0000 C CNN
F 1 "LED1642GW" H 6750 5891 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 6750 5808 30  0000 C CNN
F 3 "" H 5750 5200 60  0000 C CNN
	1    6750 4900
	0    -1   -1   0   
$EndComp
$Comp
L LED1642GW U6
U 1 1 5811DF4F
P 4650 4900
F 0 "U6" H 4650 5997 60  0000 C CNN
F 1 "LED1642GW" H 4650 5891 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 4650 5808 30  0000 C CNN
F 3 "" H 3650 5200 60  0000 C CNN
	1    4650 4900
	0    -1   -1   0   
$EndComp
Text GLabel 1800 5600 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 3900 5600 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 6000 5600 3    60   Input ~ 0
LEDLOGICVDD
Text GLabel 8100 5600 3    60   Input ~ 0
LEDLOGICVDD
Wire Wire Line
	8800 5600 8800 5900
Wire Wire Line
	9100 5600 9100 5900
Wire Wire Line
	7000 5600 7000 5900
Wire Wire Line
	4900 5600 4900 5900
Wire Wire Line
	2800 5600 2800 5900
Wire Wire Line
	8600 5600 8600 5900
Wire Wire Line
	8500 5600 8500 5900
Wire Wire Line
	8400 5600 8400 5900
Wire Wire Line
	6500 5600 6500 5900
Wire Wire Line
	6400 5600 6400 5900
Wire Wire Line
	6300 5600 6300 5900
Wire Wire Line
	4400 5600 4400 5900
Wire Wire Line
	4300 5600 4300 5900
Wire Wire Line
	4200 5600 4200 5900
Wire Wire Line
	2300 5600 2300 5900
Wire Wire Line
	2200 5600 2200 5900
Wire Wire Line
	2100 5600 2100 5900
Wire Wire Line
	2000 5600 2000 6500
Wire Wire Line
	4100 5850 2500 5850
Wire Wire Line
	8300 5850 8300 5600
Wire Wire Line
	6700 5850 8300 5850
Wire Wire Line
	6700 5600 6700 5850
Wire Wire Line
	6200 5850 6200 5600
Wire Wire Line
	4600 5850 6200 5850
Wire Wire Line
	4600 5600 4600 5850
Wire Wire Line
	4100 5600 4100 5850
Wire Wire Line
	2500 5850 2500 5600
$Comp
L LED1642GW U4
U 1 1 5811DF76
P 2550 4900
F 0 "U4" H 2550 5997 60  0000 C CNN
F 1 "LED1642GW" H 2550 5891 60  0000 C CNN
F 2 "components:SOIC-24-0.65mm-w8.0mm" H 2550 5808 30  0000 C CNN
F 3 "" H 1550 5200 60  0000 C CNN
	1    2550 4900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8800 3650 1350 3650
Wire Wire Line
	1350 3650 1350 6500
Wire Wire Line
	1350 6500 2000 6500
Text GLabel 9200 1400 1    60   Input ~ 0
co29
Text GLabel 9100 1400 1    60   Input ~ 0
ce29
Text GLabel 9000 1400 1    60   Input ~ 0
co28
Text GLabel 8900 1400 1    60   Input ~ 0
ce28
Text GLabel 8800 1400 1    60   Input ~ 0
co27
Text GLabel 8700 1400 1    60   Input ~ 0
ce27
Text GLabel 8600 1400 1    60   Input ~ 0
co26
Text GLabel 8500 1400 1    60   Input ~ 0
ce26
Text GLabel 8400 1400 1    60   Input ~ 0
co25
Text GLabel 8300 1400 1    60   Input ~ 0
ce25
Text GLabel 8200 1400 1    60   Input ~ 0
co24
Text GLabel 8100 1400 1    60   Input ~ 0
ce24
Text GLabel 5400 1400 1    60   Input ~ 0
co15
Text GLabel 5300 1400 1    60   Input ~ 0
ce15
Text GLabel 5200 1400 1    60   Input ~ 0
co14
Text GLabel 5100 1400 1    60   Input ~ 0
ce14
Text GLabel 5000 1400 1    60   Input ~ 0
co13
Text GLabel 4900 1400 1    60   Input ~ 0
ce13
Text GLabel 4800 1400 1    60   Input ~ 0
co12
Text GLabel 4700 1400 1    60   Input ~ 0
ce12
Text GLabel 4600 1400 1    60   Input ~ 0
co11
Text GLabel 4500 1400 1    60   Input ~ 0
ce11
Text GLabel 4400 1400 1    60   Input ~ 0
co10
Text GLabel 4300 1400 1    60   Input ~ 0
ce10
$EndSCHEMATC
