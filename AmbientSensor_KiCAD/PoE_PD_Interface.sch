EESchema Schematic File Version 4
LIBS:AmbientSensor-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 2 3
Title "PoE Ambient Sensor"
Date "2018-11-24"
Rev "1"
Comp "Alex M."
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2350 4000 0    60   Input ~ 0
VC12
Text HLabel 2350 3900 0    60   Input ~ 0
VC36
Text HLabel 2350 4100 0    60   Input ~ 0
VC45
Text HLabel 2350 4200 0    60   Input ~ 0
VC78
Text HLabel 8500 4250 2    60   Output ~ 0
GND
$Comp
L Device:D_Zener D3
U 1 1 57D62558
P 4300 4050
F 0 "D3" H 4300 4150 50  0000 C CNN
F 1 "SMAJ58A" H 4300 3950 50  0000 C CNN
F 2 "Diodes_SMD:D_SMA" H 4300 4050 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88390/smaj50a.pdf" H 4300 4050 50  0001 C CNN
F 4 "SMAJ58A-E3/61GICT-ND" H 4300 4050 60  0001 C CNN "DigiKey Part Number"
	1    4300 4050
	0    1    1    0   
$EndComp
$Comp
L Device:C C13
U 1 1 57D62575
P 3850 4050
F 0 "C13" H 3875 4150 50  0000 L CNN
F 1 "100n" H 3875 3950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 3888 3900 50  0001 C CNN
F 3 "http://www.samsungsem.com/kr/support/library/product-catalog/mlcc.jsp" H 3850 4050 50  0001 C CNN
F 4 "1276-6840-1-ND" H 3850 4050 60  0001 C CNN "DigiKey Part Number"
	1    3850 4050
	1    0    0    -1  
$EndComp
$Comp
L AmbientSensor-rescue:MDB6S-AlexComponents D1
U 1 1 57DD2149
P 3100 3650
F 0 "D1" V 2850 3750 60  0000 C CNN
F 1 "MDB6S" V 3350 3850 60  0000 C CNN
F 2 "AlexComponents:Micro-DIP-4" H 3150 3100 60  0001 L CNN
F 3 "https://www.fairchildsemi.com/datasheets/MD/MDB10S.pdf" H 3150 3300 60  0001 L CNN
F 4 "MDB6SFSCT-ND" H 3150 3200 60  0001 L CNN "DigiKey Part Number"
	1    3100 3650
	0    1    1    0   
$EndComp
$Comp
L AmbientSensor-rescue:MDB6S-AlexComponents D2
U 1 1 57DD3729
P 3100 4450
F 0 "D2" V 2850 4550 60  0000 C CNN
F 1 "MDB6S" V 3350 4650 60  0000 C CNN
F 2 "AlexComponents:Micro-DIP-4" H 3150 3900 60  0001 L CNN
F 3 "https://www.fairchildsemi.com/datasheets/MD/MDB10S.pdf" H 3150 4100 60  0001 L CNN
F 4 "MDB6SFSCT-ND" H 3150 4000 60  0001 L CNN "DigiKey Part Number"
	1    3100 4450
	0    1    1    0   
$EndComp
$Comp
L AmbientSensor-rescue:NCP1090-AlexComponents U3
U 1 1 59E6F539
P 5650 4050
F 0 "U3" H 5400 4300 60  0000 C CNN
F 1 "NCP1090" H 5650 3800 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5650 3700 60  0001 C CNN
F 3 "http://www.onsemi.com/pub/Collateral/NCP1090-D.PDF" H 5650 3600 60  0001 C CNN
F 4 "NCP1090DGOS-ND" H 5650 3500 60  0001 C CNN "DigiKey Part Number"
	1    5650 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 59E6FE35
P 4650 4450
F 0 "R10" V 4730 4450 50  0000 C CNN
F 1 "178k" V 4650 4450 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4580 4450 50  0001 C CNN
F 3 "" H 4650 4450 50  0000 C CNN
F 4 "311-178KHRCT-ND" V 4650 4450 60  0001 C CNN "DigiKey Part Number"
	1    4650 4450
	-1   0    0    1   
$EndComp
$Comp
L Device:R R11
U 1 1 59E6FEDA
P 4900 4450
F 0 "R11" V 4980 4450 50  0000 C CNN
F 1 "4420" V 4900 4450 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4830 4450 50  0001 C CNN
F 3 "" H 4900 4450 50  0000 C CNN
F 4 "311-4.42KHRCT-ND " V 4900 4450 60  0001 C CNN "DigiKey Part Number"
	1    4900 4450
	-1   0    0    1   
$EndComp
$Comp
L Device:R R9
U 1 1 59E6FD02
P 4650 3650
F 0 "R9" V 4730 3650 50  0000 C CNN
F 1 "24.9k" V 4650 3650 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4580 3650 50  0001 C CNN
F 3 "" H 4650 3650 50  0000 C CNN
F 4 "311-24.9KHRCT-ND " V 4650 3650 60  0001 C CNN "DigiKey Part Number"
	1    4650 3650
	-1   0    0    1   
$EndComp
$Comp
L Device:CP1 C14
U 1 1 59E7BB76
P 6500 4050
F 0 "C14" H 6525 4150 50  0000 L CNN
F 1 "22u" H 6525 3950 50  0000 L CNN
F 2 "Capacitors_THT:CP_Radial_D5.0mm_P2.00mm" H 6500 4050 50  0001 C CNN
F 3 "" H 6500 4050 50  0000 C CNN
F 4 "493-12798-1-ND" H 6500 4050 60  0001 C CNN "DigiKey Part Number"
	1    6500 4050
	1    0    0    -1  
$EndComp
$Comp
L AmbientSensor-rescue:PG02S4805A-AlexComponents U4
U 1 1 59E7C1EF
P 7250 4050
F 0 "U4" H 7000 4350 60  0000 C CNN
F 1 "PG02S4803A" H 7250 3750 60  0000 C CNN
F 2 "AlexComponents:PG02S" H 6850 3200 60  0001 L CNN
F 3 "http://www.deltaww.com/filecenter/products/download/01/0102/datasheet/DS_PG02S.pdf" H 6250 3550 60  0001 L CNN
F 4 "941-1185-ND" H 6950 3650 60  0001 L CNN "DigiKey Part Number"
	1    7250 4050
	1    0    0    -1  
$EndComp
NoConn ~ 6200 4000
NoConn ~ 6750 4050
Wire Wire Line
	3500 3650 3400 3650
Connection ~ 3500 3650
Connection ~ 2700 4450
Wire Wire Line
	3100 4150 3100 4100
Wire Wire Line
	3100 4000 3100 3950
Wire Wire Line
	3100 4750 3100 4800
Wire Wire Line
	3100 3350 3100 3300
Wire Wire Line
	3100 3300 2450 3300
Wire Wire Line
	2450 3300 2450 3900
Wire Wire Line
	2450 3900 2350 3900
Wire Wire Line
	2350 4000 3100 4000
Wire Wire Line
	3100 4800 2450 4800
Wire Wire Line
	2450 4800 2450 4200
Wire Wire Line
	2450 4200 2350 4200
Wire Wire Line
	3100 4100 2350 4100
Wire Wire Line
	2800 4450 2700 4450
Wire Wire Line
	3500 4450 3400 4450
Wire Wire Line
	2800 3650 2700 3650
Wire Wire Line
	2700 3650 2700 4450
Wire Wire Line
	3850 4950 3850 4200
Wire Wire Line
	3500 3150 3500 3650
Wire Wire Line
	3500 3150 3850 3150
Wire Wire Line
	3850 3150 3850 3900
Connection ~ 3850 3150
Connection ~ 3850 4950
Wire Wire Line
	5150 4950 5150 4200
Connection ~ 4300 4950
Wire Wire Line
	5150 4100 4900 4100
Wire Wire Line
	4900 4100 4900 4300
Wire Wire Line
	5150 4000 4650 4000
Wire Wire Line
	4650 4000 4650 4300
Wire Wire Line
	5150 3900 4650 3900
Wire Wire Line
	4650 3900 4650 3800
Wire Wire Line
	4650 3150 4650 3500
Connection ~ 4300 3150
Wire Wire Line
	4900 4950 4900 4600
Connection ~ 4900 4950
Wire Wire Line
	4650 4950 4650 4600
Connection ~ 4650 4950
Connection ~ 4650 3150
Wire Wire Line
	6200 4200 6250 4200
Wire Wire Line
	6250 4200 6250 4250
Wire Wire Line
	6250 4250 6500 4250
Connection ~ 6500 4250
Wire Wire Line
	6250 3850 6500 3850
Wire Wire Line
	6250 3850 6250 3900
Wire Wire Line
	6250 3900 6200 3900
Wire Wire Line
	6500 3150 6500 3850
Connection ~ 6500 3850
Connection ~ 8000 3850
Wire Wire Line
	8000 4200 8000 4250
Connection ~ 8000 4250
Text HLabel 8500 3850 2    60   Output ~ 0
3.3V
$Comp
L Device:C C15
U 1 1 59EB18F9
P 8000 4050
F 0 "C15" H 8025 4150 50  0000 L CNN
F 1 "10u" H 8025 3950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 8038 3900 50  0001 C CNN
F 3 "" H 8000 4050 50  0000 C CNN
F 4 "1276-6456-1-ND" H 8000 4050 60  0001 C CNN "DigiKey Part Number"
	1    8000 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 4950 3850 4950
Wire Wire Line
	6500 4250 6500 4200
Wire Wire Line
	7750 3850 8000 3850
Wire Wire Line
	8000 3850 8000 3900
Wire Wire Line
	7750 4250 8000 4250
Text Label 3500 3150 0    60   ~ 0
P
Text Label 3500 4950 0    60   ~ 0
N
Wire Wire Line
	3500 3650 3500 4450
Wire Wire Line
	2700 4450 2700 4950
Wire Wire Line
	3850 3150 4300 3150
Wire Wire Line
	3850 4950 4300 4950
Wire Wire Line
	4300 4950 4650 4950
Wire Wire Line
	4300 3150 4650 3150
Wire Wire Line
	4900 4950 5150 4950
Wire Wire Line
	4650 4950 4900 4950
Wire Wire Line
	4650 3150 6500 3150
Wire Wire Line
	6500 4250 6750 4250
Wire Wire Line
	6500 3850 6750 3850
Wire Wire Line
	6500 3850 6500 3900
Wire Wire Line
	8000 3850 8500 3850
Wire Wire Line
	8000 4250 8500 4250
Wire Wire Line
	4300 3150 4300 3900
Wire Wire Line
	4300 4200 4300 4950
$EndSCHEMATC
