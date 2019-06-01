EESchema Schematic File Version 4
LIBS:AmbientSensor_Peripheral_OPT3002-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "OPT3002 Peripheral"
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
P 3600 4000
F 0 "X1" H 3655 4467 50  0000 C CNN
F 1 "RJ22" H 3655 4376 50  0000 C CNN
F 2 "AlexComponents:A-2004-3-4-LP-N-R" V 3600 4050 50  0001 C CNN
F 3 "~" V 3600 4050 50  0001 C CNN
F 4 "AE10381-ND" H 3600 4000 50  0001 C CNN "DigiKey Part Number"
	1    3600 4000
	1    0    0    -1  
$EndComp
$Comp
L AlexComponents:OPT3002 U1
U 1 1 5B9B0B68
P 6700 3950
F 0 "U1" H 6700 4275 50  0000 C CNN
F 1 "OPT3002" H 6700 4184 50  0000 C CNN
F 2 "Housings_DFN_QFN:DFN-6-1EP_2x2mm_Pitch0.65mm" H 6450 3450 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/opt3002.pdf" H 6450 3550 50  0001 L CNN
F 4 "296-47274-1-ND" H 6450 3650 50  0001 L CNN "DigiKey Part Number"
	1    6700 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5B9B0BF6
P 5000 3950
F 0 "C1" H 5115 3996 50  0000 L CNN
F 1 "100u" H 5115 3905 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5038 3800 50  0001 C CNN
F 3 "~" H 5000 3950 50  0001 C CNN
	1    5000 3950
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0101
U 1 1 5B9B0D4A
P 4500 3450
F 0 "#PWR0101" H 4500 3300 50  0001 C CNN
F 1 "+3.3V" H 4515 3623 50  0000 C CNN
F 2 "" H 4500 3450 50  0001 C CNN
F 3 "" H 4500 3450 50  0001 C CNN
	1    4500 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5B9B0D87
P 4500 4450
F 0 "#PWR0102" H 4500 4200 50  0001 C CNN
F 1 "GND" H 4505 4277 50  0000 C CNN
F 2 "" H 4500 4450 50  0001 C CNN
F 3 "" H 4500 4450 50  0001 C CNN
	1    4500 4450
	1    0    0    -1  
$EndComp
Text GLabel 4200 4100 2    50   Output ~ 0
SCL
Text GLabel 7350 4050 2    50   Input ~ 0
SCL
Wire Wire Line
	7150 4050 7350 4050
Text GLabel 7350 3850 2    50   BiDi ~ 0
SDA
Wire Wire Line
	7150 3850 7350 3850
Text GLabel 4200 3900 2    50   BiDi ~ 0
SDA
$Comp
L power:+3.3V #PWR0103
U 1 1 5B9B0F5D
P 6050 3450
F 0 "#PWR0103" H 6050 3300 50  0001 C CNN
F 1 "+3.3V" H 6065 3623 50  0000 C CNN
F 2 "" H 6050 3450 50  0001 C CNN
F 3 "" H 6050 3450 50  0001 C CNN
	1    6050 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5B9B0FAA
P 6050 4450
F 0 "#PWR0104" H 6050 4200 50  0001 C CNN
F 1 "GND" H 6055 4277 50  0000 C CNN
F 2 "" H 6050 4450 50  0001 C CNN
F 3 "" H 6050 4450 50  0001 C CNN
	1    6050 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 4050 6050 4050
Wire Wire Line
	6250 3850 6050 3850
NoConn ~ 7150 3950
$Comp
L Jumper:SolderJumper_2_Open J1
U 1 1 5B9B125A
P 5700 3700
F 0 "J1" V 5654 3768 50  0000 L CNN
F 1 "NO" V 5745 3768 50  0000 L CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 5700 3700 50  0001 C CNN
F 3 "~" H 5700 3700 50  0001 C CNN
	1    5700 3700
	0    1    1    0   
$EndComp
$Comp
L Jumper:SolderJumper_2_Bridged J2
U 1 1 5B9B12E2
P 5700 4200
F 0 "J2" V 5746 4112 50  0000 R CNN
F 1 "NC" V 5655 4112 50  0000 R CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Bridged2Bar_RoundedPad1.0x1.5mm" H 5700 4200 50  0001 C CNN
F 3 "~" H 5700 4200 50  0001 C CNN
	1    5700 4200
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR0105
U 1 1 5B9B1404
P 5000 3450
F 0 "#PWR0105" H 5000 3300 50  0001 C CNN
F 1 "+3.3V" H 5015 3623 50  0000 C CNN
F 2 "" H 5000 3450 50  0001 C CNN
F 3 "" H 5000 3450 50  0001 C CNN
	1    5000 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5B9B1419
P 5000 4450
F 0 "#PWR0106" H 5000 4200 50  0001 C CNN
F 1 "GND" H 5005 4277 50  0000 C CNN
F 2 "" H 5000 4450 50  0001 C CNN
F 3 "" H 5000 4450 50  0001 C CNN
	1    5000 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3950 5700 3950
Wire Wire Line
	5700 3950 5700 3850
Wire Wire Line
	5700 4050 5700 3950
Connection ~ 5700 3950
$Comp
L power:GND #PWR0107
U 1 1 5B9B294C
P 5700 4450
F 0 "#PWR0107" H 5700 4200 50  0001 C CNN
F 1 "GND" H 5705 4277 50  0000 C CNN
F 2 "" H 5700 4450 50  0001 C CNN
F 3 "" H 5700 4450 50  0001 C CNN
	1    5700 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 4450 5700 4350
$Comp
L power:+3.3V #PWR0108
U 1 1 5B9B2A94
P 5700 3450
F 0 "#PWR0108" H 5700 3300 50  0001 C CNN
F 1 "+3.3V" H 5715 3623 50  0000 C CNN
F 2 "" H 5700 3450 50  0001 C CNN
F 3 "" H 5700 3450 50  0001 C CNN
	1    5700 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 3550 5700 3450
Wire Wire Line
	6050 3450 6050 3850
Wire Wire Line
	6050 4050 6050 4450
Wire Wire Line
	5000 4100 5000 4450
Wire Wire Line
	5000 3450 5000 3800
Wire Wire Line
	4000 4000 4500 4000
Wire Wire Line
	4500 4000 4500 4450
Wire Wire Line
	4000 3800 4500 3800
Wire Wire Line
	4500 3800 4500 3450
Wire Wire Line
	4200 3900 4000 3900
Wire Wire Line
	4000 4100 4200 4100
$EndSCHEMATC
