EESchema Schematic File Version 4
LIBS:AmbientSensor-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 3 3
Title "PoE Ambient Sensor"
Date "2018-11-24"
Rev "1"
Comp "Alex M."
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
NoConn ~ 4500 3300
$Comp
L Device:R R12
U 1 1 5B95CB0E
P 6100 4950
F 0 "R12" H 6170 4996 50  0000 L CNN
F 1 "12.4k" H 6170 4905 50  0000 L CNN
F 2 "Resistors_SMD:R_0603" V 6030 4950 50  0001 C CNN
F 3 "~" H 6100 4950 50  0001 C CNN
F 4 "±1%" H 6100 4950 50  0001 C CNN "Tolerance"
F 5 "311-12.4KHRCT-ND " H 6100 4950 50  0001 C CNN "DigiKey Part Number"
	1    6100 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5B95CCC7
P 4100 5500
F 0 "#PWR0101" H 4100 5250 50  0001 C CNN
F 1 "GND" H 4105 5327 50  0000 C CNN
F 2 "" H 4100 5500 50  0001 C CNN
F 3 "" H 4100 5500 50  0001 C CNN
	1    4100 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 5300 4100 5500
Wire Wire Line
	4100 5200 4100 5300
Connection ~ 4100 5300
Wire Wire Line
	4100 5100 4100 5200
Connection ~ 4100 5200
Wire Wire Line
	4100 5000 4100 5100
Connection ~ 4100 5100
Wire Wire Line
	4100 4900 4100 5000
Connection ~ 4100 5000
Wire Wire Line
	4100 4800 4100 4900
Connection ~ 4100 4900
Wire Wire Line
	4100 4700 4100 4800
Connection ~ 4100 4800
$Comp
L power:GND #PWR0102
U 1 1 5B95D136
P 6100 5500
F 0 "#PWR0102" H 6100 5250 50  0001 C CNN
F 1 "GND" H 6105 5327 50  0000 C CNN
F 2 "" H 6100 5500 50  0001 C CNN
F 3 "" H 6100 5500 50  0001 C CNN
	1    6100 5500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C22
U 1 1 5B95EFD1
P 6500 4950
F 0 "C22" H 6615 4996 50  0000 L CNN
F 1 "4.7u" H 6615 4905 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6538 4800 50  0001 C CNN
F 3 "~" H 6500 4950 50  0001 C CNN
F 4 "1276-1907-1-ND" H 6500 4950 50  0001 C CNN "DigiKey Part Number"
	1    6500 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5B95F0AF
P 6500 5500
F 0 "#PWR0103" H 6500 5250 50  0001 C CNN
F 1 "GND" H 6505 5327 50  0000 C CNN
F 2 "" H 6500 5500 50  0001 C CNN
F 3 "" H 6500 5500 50  0001 C CNN
	1    6500 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 4600 6100 4600
Wire Wire Line
	6100 4600 6100 4800
Wire Wire Line
	6100 5100 6100 5500
$Comp
L Device:Crystal Y2
U 1 1 5B95F795
P 5900 4100
F 0 "Y2" V 5854 4231 50  0000 L CNN
F 1 "25MHz" V 5945 4231 50  0000 L CNN
F 2 "Crystals:Crystal_SMD_5032-2pin_5.0x3.2mm" H 5900 4100 50  0001 C CNN
F 3 "~" H 5900 4100 50  0001 C CNN
F 4 "CTX1434CT-ND" V 5900 4100 50  0001 C CNN "DigiKey Part Number"
	1    5900 4100
	0    1    1    0   
$EndComp
$Comp
L Device:R R13
U 1 1 5B95F83B
P 6400 4100
F 0 "R13" H 6470 4146 50  0000 L CNN
F 1 "1M" V 6400 4100 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6330 4100 50  0001 C CNN
F 3 "~" H 6400 4100 50  0001 C CNN
F 4 "311-1.0MGRCT-ND" H 6400 4100 50  0001 C CNN "DigiKey Part Number"
	1    6400 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 4300 5900 4300
Wire Wire Line
	5900 4300 5900 4250
Wire Wire Line
	5700 3900 5900 3900
Wire Wire Line
	5900 3900 5900 3950
Wire Wire Line
	5900 4300 6400 4300
Wire Wire Line
	6400 4300 6400 4250
Connection ~ 5900 4300
Wire Wire Line
	5900 3900 6400 3900
Wire Wire Line
	6400 3900 6400 3950
Connection ~ 5900 3900
Wire Wire Line
	5700 4500 6500 4500
Wire Wire Line
	6500 4500 6500 4800
Wire Wire Line
	6500 5500 6500 5100
Wire Wire Line
	4500 2500 3600 2500
Wire Wire Line
	3600 2500 3600 1700
Wire Wire Line
	4500 2600 3600 2600
Wire Wire Line
	3600 2600 3600 2500
Connection ~ 3600 2500
Wire Wire Line
	4500 2700 3600 2700
Wire Wire Line
	3600 2700 3600 2600
Connection ~ 3600 2600
Wire Wire Line
	4500 2800 3600 2800
Wire Wire Line
	3600 2800 3600 2700
Connection ~ 3600 2700
Wire Wire Line
	4500 2900 3600 2900
Wire Wire Line
	3600 2900 3600 2800
Connection ~ 3600 2800
Connection ~ 3600 2900
$Comp
L Device:C C21
U 1 1 5B974852
P 4100 3500
F 0 "C21" H 4150 3600 50  0000 L CNN
F 1 "10n" H 4150 3400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4138 3350 50  0001 C CNN
F 3 "~" H 4100 3500 50  0001 C CNN
F 4 "1276-1921-1-ND" H 4100 3500 50  0001 C CNN "DigiKey Part Number"
	1    4100 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3200 4100 3200
$Comp
L power:GND #PWR0104
U 1 1 5B975EA7
P 4100 3800
F 0 "#PWR0104" H 4100 3550 50  0001 C CNN
F 1 "GND" H 4105 3627 50  0000 C CNN
F 2 "" H 4100 3800 50  0001 C CNN
F 3 "" H 4100 3800 50  0001 C CNN
	1    4100 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C20
U 1 1 5B977420
P 3600 3500
F 0 "C20" H 3650 3600 50  0000 L CNN
F 1 "100n" H 3650 3400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3638 3350 50  0001 C CNN
F 3 "~" H 3600 3500 50  0001 C CNN
F 4 "1276-1012-1-ND" H 3600 3500 50  0001 C CNN "DigiKey Part Number"
	1    3600 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C19
U 1 1 5B978BCC
P 3300 3500
F 0 "C19" H 3350 3600 50  0000 L CNN
F 1 "100n" H 3350 3400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3338 3350 50  0001 C CNN
F 3 "~" H 3300 3500 50  0001 C CNN
F 4 "1276-1012-1-ND" H 3300 3500 50  0001 C CNN "DigiKey Part Number"
	1    3300 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C18
U 1 1 5B978BFC
P 3000 3500
F 0 "C18" H 3050 3600 50  0000 L CNN
F 1 "100n" H 3050 3400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3038 3350 50  0001 C CNN
F 3 "~" H 3000 3500 50  0001 C CNN
F 4 "1276-1012-1-ND" H 3000 3500 50  0001 C CNN "DigiKey Part Number"
	1    3000 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C17
U 1 1 5B978C2A
P 2700 3500
F 0 "C17" H 2750 3600 50  0000 L CNN
F 1 "100n" H 2750 3400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2738 3350 50  0001 C CNN
F 3 "~" H 2700 3500 50  0001 C CNN
F 4 "1276-1012-1-ND" H 2700 3500 50  0001 C CNN "DigiKey Part Number"
	1    2700 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C16
U 1 1 5B978C5A
P 2400 3500
F 0 "C16" H 2450 3600 50  0000 L CNN
F 1 "100n" H 2450 3400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2438 3350 50  0001 C CNN
F 3 "~" H 2400 3500 50  0001 C CNN
F 4 "1276-1012-1-ND" H 2400 3500 50  0001 C CNN "DigiKey Part Number"
	1    2400 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5B97D533
P 3600 3800
F 0 "#PWR0105" H 3600 3550 50  0001 C CNN
F 1 "GND" H 3605 3627 50  0000 C CNN
F 2 "" H 3600 3800 50  0001 C CNN
F 3 "" H 3600 3800 50  0001 C CNN
	1    3600 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 3800 3600 3750
Wire Wire Line
	3600 3750 3300 3750
Connection ~ 3600 3750
Wire Wire Line
	3300 3750 3000 3750
Connection ~ 3300 3750
Wire Wire Line
	3000 3750 2700 3750
Connection ~ 3000 3750
Wire Wire Line
	2700 3750 2400 3750
Connection ~ 2700 3750
Wire Wire Line
	4100 3650 4100 3800
Wire Wire Line
	4100 3350 4100 3200
Wire Wire Line
	3600 3350 3600 3200
Connection ~ 3600 3200
Wire Wire Line
	3600 3650 3600 3750
Wire Wire Line
	3300 3650 3300 3750
Wire Wire Line
	3000 3650 3000 3750
Wire Wire Line
	2700 3650 2700 3750
Wire Wire Line
	2400 3650 2400 3750
Wire Wire Line
	2400 3350 2400 3200
Wire Wire Line
	2400 3200 2700 3200
Wire Wire Line
	2700 3350 2700 3200
Connection ~ 2700 3200
Wire Wire Line
	2700 3200 3000 3200
Wire Wire Line
	3000 3350 3000 3200
Connection ~ 3000 3200
Wire Wire Line
	3000 3200 3300 3200
Wire Wire Line
	3300 3350 3300 3200
Connection ~ 3300 3200
Wire Wire Line
	3300 3200 3600 3200
$Comp
L Device:C C23
U 1 1 5B9B507B
P 6900 4950
F 0 "C23" H 6950 5050 50  0000 L CNN
F 1 "18p" H 6950 4850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6938 4800 50  0001 C CNN
F 3 "~" H 6900 4950 50  0001 C CNN
F 4 "1276-1089-1-ND" H 6900 4950 50  0001 C CNN "DigiKey Part Number"
	1    6900 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C24
U 1 1 5B9B521C
P 7300 4950
F 0 "C24" H 7350 5050 50  0000 L CNN
F 1 "18p" H 7350 4850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 7338 4800 50  0001 C CNN
F 3 "~" H 7300 4950 50  0001 C CNN
F 4 "1276-1089-1-ND" H 7300 4950 50  0001 C CNN "DigiKey Part Number"
	1    7300 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 4300 6900 4300
Wire Wire Line
	6900 4300 6900 4800
Connection ~ 6400 4300
Wire Wire Line
	6400 3900 7300 3900
Wire Wire Line
	7300 3900 7300 4800
Connection ~ 6400 3900
$Comp
L power:GND #PWR0106
U 1 1 5B9BA7CF
P 6900 5500
F 0 "#PWR0106" H 6900 5250 50  0001 C CNN
F 1 "GND" H 6905 5327 50  0000 C CNN
F 2 "" H 6900 5500 50  0001 C CNN
F 3 "" H 6900 5500 50  0001 C CNN
	1    6900 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 5B9BA82F
P 7300 5500
F 0 "#PWR0107" H 7300 5250 50  0001 C CNN
F 1 "GND" H 7305 5327 50  0000 C CNN
F 2 "" H 7300 5500 50  0001 C CNN
F 3 "" H 7300 5500 50  0001 C CNN
	1    7300 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 5500 6900 5100
Wire Wire Line
	7300 5100 7300 5500
Text Label 4100 3200 0    50   ~ 0
1V2O
Text Label 6700 4300 0    50   ~ 0
XO
Text Label 6700 3900 0    50   ~ 0
XI
Text Label 5750 4600 0    50   ~ 0
XRES
Text Label 5750 4500 0    50   ~ 0
XCAP
Wire Wire Line
	4100 2400 4500 2400
NoConn ~ 5700 3400
NoConn ~ 5700 3600
NoConn ~ 5700 3700
NoConn ~ 5700 3500
Text HLabel 7300 2800 2    50   Input ~ 0
CS
Text HLabel 7300 2900 2    50   Output ~ 0
MISO
Text HLabel 7300 3000 2    50   Input ~ 0
MOSI
Text HLabel 7300 3100 2    50   Input ~ 0
SCLK
Text HLabel 4100 2400 0    50   Input ~ 0
RESET
Text HLabel 4100 4300 0    50   Input ~ 0
RXIP
Text HLabel 4100 4400 0    50   Input ~ 0
RXIN
Text HLabel 4100 4100 0    50   Output ~ 0
TXOP
Text HLabel 4100 4200 0    50   Output ~ 0
TXON
Wire Wire Line
	5700 2800 7300 2800
Wire Wire Line
	5700 2900 7300 2900
Wire Wire Line
	5700 3000 7300 3000
Wire Wire Line
	5700 3100 7300 3100
$Comp
L power:+3.3V #PWR0108
U 1 1 5C41AF52
P 6100 1700
F 0 "#PWR0108" H 6100 1550 50  0001 C CNN
F 1 "+3.3V" H 6115 1873 50  0000 C CNN
F 2 "" H 6100 1700 50  0001 C CNN
F 3 "" H 6100 1700 50  0001 C CNN
	1    6100 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2400 6100 2400
Wire Wire Line
	6100 2400 6100 1700
Wire Wire Line
	5700 2500 6100 2500
Wire Wire Line
	6100 2500 6100 2400
Connection ~ 6100 2400
Wire Wire Line
	5700 2600 6100 2600
Wire Wire Line
	6100 2600 6100 2500
Connection ~ 6100 2500
Wire Wire Line
	4100 4100 4500 4100
Wire Wire Line
	4500 4200 4100 4200
Wire Wire Line
	4100 4300 4500 4300
Wire Wire Line
	4500 4400 4100 4400
Wire Wire Line
	4100 4700 4500 4700
Wire Wire Line
	4100 4800 4500 4800
Wire Wire Line
	4100 4900 4500 4900
Wire Wire Line
	4100 5000 4500 5000
Wire Wire Line
	4100 5100 4500 5100
Wire Wire Line
	4100 5200 4500 5200
Wire Wire Line
	4100 5300 4500 5300
Wire Wire Line
	3600 2900 3600 3000
Wire Wire Line
	4500 3000 3600 3000
Connection ~ 3600 3000
Wire Wire Line
	3600 3000 3600 3100
Wire Wire Line
	4500 3100 3600 3100
Connection ~ 3600 3100
Wire Wire Line
	3600 3100 3600 3200
$Comp
L power:+3V3 #PWR0109
U 1 1 5C441F3B
P 3600 1700
F 0 "#PWR0109" H 3600 1550 50  0001 C CNN
F 1 "+3V3" H 3615 1873 50  0000 C CNN
F 2 "" H 3600 1700 50  0001 C CNN
F 3 "" H 3600 1700 50  0001 C CNN
	1    3600 1700
	1    0    0    -1  
$EndComp
$Comp
L AmbientSensor-rescue:W5500-AlexComponents U5
U 1 1 5C1FAAE5
P 5100 4050
F 0 "U5" H 5100 5925 50  0000 C CNN
F 1 "W5500" H 5100 5834 50  0000 C CNN
F 2 "Housings_QFP:LQFP-48_7x7mm_Pitch0.5mm" H 4700 6000 50  0001 L CNN
F 3 "http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:w5500:w5500_ds_v108e.pdf" H 4700 5900 50  0001 L CNN
F 4 "1278-1021-ND" H 4700 6100 50  0001 L CNN "DigiKey Part Number"
	1    5100 4050
	1    0    0    -1  
$EndComp
Text HLabel 7300 3200 2    50   Output ~ 0
INT
Wire Wire Line
	5700 3200 7300 3200
$EndSCHEMATC