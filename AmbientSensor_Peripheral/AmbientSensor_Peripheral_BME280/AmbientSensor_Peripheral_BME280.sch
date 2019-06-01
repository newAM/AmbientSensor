EESchema Schematic File Version 4
LIBS:AmbientSensor_Peripheral_BME280-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "BME280 Peripheral"
Date "2018-09-13"
Rev "1"
Comp "Alex M."
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:RJ22 X1
U 1 1 5B9B0A65
P 3400 4000
F 0 "X1" H 3455 4467 50  0000 C CNN
F 1 "RJ22" H 3455 4376 50  0000 C CNN
F 2 "AlexComponents:A-2004-3-4-LP-N-R" V 3400 4050 50  0001 C CNN
F 3 "~" V 3400 4050 50  0001 C CNN
F 4 "AE10381-ND" H 3400 4000 50  0001 C CNN "DigiKey Part Number"
	1    3400 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5B9B0BF6
P 7000 3950
F 0 "C1" H 7115 3996 50  0000 L CNN
F 1 "100u" H 7115 3905 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 7038 3800 50  0001 C CNN
F 3 "~" H 7000 3950 50  0001 C CNN
	1    7000 3950
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0101
U 1 1 5B9B0D4A
P 4300 3450
F 0 "#PWR0101" H 4300 3300 50  0001 C CNN
F 1 "+3.3V" H 4315 3623 50  0000 C CNN
F 2 "" H 4300 3450 50  0001 C CNN
F 3 "" H 4300 3450 50  0001 C CNN
	1    4300 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5B9B0D87
P 4300 4450
F 0 "#PWR0102" H 4300 4200 50  0001 C CNN
F 1 "GND" H 4305 4277 50  0000 C CNN
F 2 "" H 4300 4450 50  0001 C CNN
F 3 "" H 4300 4450 50  0001 C CNN
	1    4300 4450
	1    0    0    -1  
$EndComp
Text GLabel 4000 4100 2    50   Output ~ 0
SCL
Text GLabel 5250 4100 0    50   Input ~ 0
SCL
Text GLabel 4000 3900 2    50   BiDi ~ 0
SDA
$Comp
L Jumper:SolderJumper_2_Open J1
U 1 1 5B9B125A
P 4750 3650
F 0 "J1" V 4704 3718 50  0000 L CNN
F 1 "NO" V 4795 3718 50  0000 L CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 4750 3650 50  0001 C CNN
F 3 "~" H 4750 3650 50  0001 C CNN
	1    4750 3650
	0    1    1    0   
$EndComp
$Comp
L Jumper:SolderJumper_2_Bridged J2
U 1 1 5B9B12E2
P 4750 4150
F 0 "J2" V 4796 4062 50  0000 R CNN
F 1 "NC" V 4705 4062 50  0000 R CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Bridged2Bar_RoundedPad1.0x1.5mm" H 4750 4150 50  0001 C CNN
F 3 "~" H 4750 4150 50  0001 C CNN
	1    4750 4150
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR0105
U 1 1 5B9B1404
P 7000 3450
F 0 "#PWR0105" H 7000 3300 50  0001 C CNN
F 1 "+3.3V" H 7015 3623 50  0000 C CNN
F 2 "" H 7000 3450 50  0001 C CNN
F 3 "" H 7000 3450 50  0001 C CNN
	1    7000 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5B9B1419
P 7000 4450
F 0 "#PWR0106" H 7000 4200 50  0001 C CNN
F 1 "GND" H 7005 4277 50  0000 C CNN
F 2 "" H 7000 4450 50  0001 C CNN
F 3 "" H 7000 4450 50  0001 C CNN
	1    7000 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 5B9B294C
P 4750 4450
F 0 "#PWR0107" H 4750 4200 50  0001 C CNN
F 1 "GND" H 4755 4277 50  0000 C CNN
F 2 "" H 4750 4450 50  0001 C CNN
F 3 "" H 4750 4450 50  0001 C CNN
	1    4750 4450
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0108
U 1 1 5B9B2A94
P 4750 3450
F 0 "#PWR0108" H 4750 3300 50  0001 C CNN
F 1 "+3.3V" H 4765 3623 50  0000 C CNN
F 2 "" H 4750 3450 50  0001 C CNN
F 3 "" H 4750 3450 50  0001 C CNN
	1    4750 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 4100 7000 4450
Wire Wire Line
	7000 3450 7000 3800
Wire Wire Line
	3800 4000 4300 4000
Wire Wire Line
	4300 4000 4300 4450
Wire Wire Line
	3800 3800 4300 3800
Wire Wire Line
	4300 3800 4300 3450
Wire Wire Line
	4000 3900 3800 3900
Wire Wire Line
	3800 4100 4000 4100
$Comp
L AlexComponents:BME280 U1
U 1 1 5B9B666C
P 5900 3950
F 0 "U1" H 5650 4200 50  0000 L CNN
F 1 "BME280" H 5900 3700 50  0000 C CNN
F 2 "Housings_LGA:Bosch_LGA-8_2.5x2.5mm_Pitch0.65mm_ClockwisePinNumbering" H 5850 3500 50  0001 C CNN
F 3 "https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS002-13.pdf" H 5900 3400 50  0001 C CNN
F 4 "828-1063-1-ND‎" H 5850 3600 50  0001 C CNN "DigiKey Part Number"
	1    5900 3950
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR02
U 1 1 5B9B698B
P 6550 3450
F 0 "#PWR02" H 6550 3300 50  0001 C CNN
F 1 "+3.3V" H 6565 3623 50  0000 C CNN
F 2 "" H 6550 3450 50  0001 C CNN
F 3 "" H 6550 3450 50  0001 C CNN
	1    6550 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5B9B6991
P 6550 4450
F 0 "#PWR03" H 6550 4200 50  0001 C CNN
F 1 "GND" H 6555 4277 50  0000 C CNN
F 2 "" H 6550 4450 50  0001 C CNN
F 3 "" H 6550 4450 50  0001 C CNN
	1    6550 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 4000 6550 4000
Wire Wire Line
	6550 4000 6550 4100
Wire Wire Line
	6350 4100 6550 4100
Connection ~ 6550 4100
Wire Wire Line
	6550 4100 6550 4450
Wire Wire Line
	6350 3900 6550 3900
Wire Wire Line
	6350 3800 6550 3800
Wire Wire Line
	6550 3450 6550 3800
Connection ~ 6550 3800
Wire Wire Line
	6550 3800 6550 3900
Wire Wire Line
	5250 4100 5450 4100
Text GLabel 5250 4000 0    50   BiDi ~ 0
SDA
Wire Wire Line
	5250 4000 5450 4000
$Comp
L power:+3.3V #PWR01
U 1 1 5B9B7D2D
P 5250 3450
F 0 "#PWR01" H 5250 3300 50  0001 C CNN
F 1 "+3.3V" H 5265 3623 50  0000 C CNN
F 2 "" H 5250 3450 50  0001 C CNN
F 3 "" H 5250 3450 50  0001 C CNN
	1    5250 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3800 5250 3800
Wire Wire Line
	5250 3800 5250 3450
Wire Wire Line
	5450 3900 4750 3900
Wire Wire Line
	4750 3900 4750 4000
Wire Wire Line
	4750 3900 4750 3800
Connection ~ 4750 3900
Wire Wire Line
	4750 3500 4750 3450
Wire Wire Line
	4750 4450 4750 4300
$EndSCHEMATC
