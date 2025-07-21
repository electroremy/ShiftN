//--------------------------------------------------------------------------
// Program to pull the information out of various types of EXIF digital 
// camera files and show it in a reasonably consistent way
//
// This module parses the very complicated exif structures.
//
// Matthias Wandel
// http://www.sentex.net/~mwandel/jhead/
// 
// modified for ShiftN by Marcus Hebel
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <sys/utime.h>
#include "jhead.h"

static unsigned char * DirWithThumbnailPtrs;
static double FocalplaneXRes;
static double FocalplaneUnits;
static int ExifImageWidth;
static int MotorolaOrder = 0;
// for fixing the rotation.
static void * OrientationPtr[2];
static int    OrientationNumFormat[2];
int NumOrientations = 0;
int jheadfocal=0;

typedef struct {
    unsigned short Tag;
    char * Desc;
}TagTable_t;

typedef struct {
	char * KamStr;
	double bre1;
	double bre2;
}KameraTable_t;

//--------------------------------------------------------------------------
// Table of Jpeg encoding process names
static const TagTable_t ProcessTable[] = {
    { M_SOF0,   "Baseline"},
    { M_SOF1,   "Extended sequential"},
    { M_SOF2,   "Progressive"},
    { M_SOF3,   "Lossless"},
    { M_SOF5,   "Differential sequential"},
    { M_SOF6,   "Differential progressive"},
    { M_SOF7,   "Differential lossless"},
    { M_SOF9,   "Extended sequential, arithmetic coding"},
    { M_SOF10,  "Progressive, arithmetic coding"},
    { M_SOF11,  "Lossless, arithmetic coding"},
    { M_SOF13,  "Differential sequential, arithmetic coding"},
    { M_SOF14,  "Differential progressive, arithmetic coding"},
    { M_SOF15,  "Differential lossless, arithmetic coding"},
};

// Modification for ShiftN: Camera table
static const KameraTable_t KameraTable[] = {
{ "iPhone 4", 6.416667, 50 },
{ "iPhone 4S", 6.114286, 50 },
{ "iPhone 5", 6.257576, 50 },
{ "Canon DIGITAL IXUS 100 IS", 8.944694, 50 },
{ "Canon PowerShot SD780 IS", 8.944694, 50 },
{ "Canon DIGITAL IXUS 110 IS", 8.928571, 50 },
{ "Canon IXUS 117 HS", 8.928571, 50 },
{ "Canon PowerShot SD940 IS", 8.928571, 50 },
{ "Canon PowerShot SD1400 IS", 8.928571, 50 },
{ "Canon PowerShot SD980 IS", 8.958333, 50 },
{ "Canon PowerShot SD3500 IS", 8.958333, 50 },
{ "Canon IXUS 240 HS", 8.958333, 50 },
{ "Canon DIGITAL IXUS 30", 8.285714, 50 },
{ "Canon IXY 30S", 8.803245, 50 },
{ "Canon DIGITAL IXUS 40", 8.285714, 50 },
{ "Canon DIGITAL IXUS 400", 10.277778, 50 },
{ "Canon IXY DIGITAL 450", 10.277778, 50 },
{ "Canon DIGITAL IXUS 50", 8.285714, 50 },
{ "Canon IXY DIGITAL 500", 10.277778, 50 },
{ "Canon IXUS 510 HS", 7.142857, 50 },
{ "Canon DIGITAL IXUS 55", 8.285714, 50 },
{ "Canon PowerShot SD60", 8.285714, 50 },
{ "Canon IXY DIGITAL 80", 8.285714, 50 },
{ "Canon DIGITAL IXUS 700", 10.405405, 50 },
{ "Canon DIGITAL IXUS 750", 10.405405, 50 },
{ "Canon DIGITAL IXUS 80 IS", 8.157895, 50 },
{ "Canon IXY DIGITAL 20 IS", 8.157895, 50 },
{ "Canon DIGITAL IXUS 800 IS", 8.285714, 50 },
{ "Canon DIGITAL IXUS 850 IS", 8.226173, 50 },
{ "Canon DIGITAL IXUS 860 IS", 8.226173, 50 },
{ "Canon DIGITAL IXUS 900 Ti", 10.405405, 50 },
{ "Canon DIGITAL IXUS 95 IS", 8.857143, 50 },
{ "Canon DIGITAL IXUS 950 IS", 8.285714, 50 },
{ "Canon DIGITAL IXUS 960 IS", 10.704356, 50 },
{ "Canon DIGITAL IXUS 980 IS", 10.704356, 50 },
{ "Canon DIGITAL IXUS 990 IS", 8.918919, 50 },
{ "Canon DIGITAL IXUSII", 7.714286, 50 },
{ "Canon DIGITAL IXUS IIs", 7.714286, 50 },
{ "Canon IXY DIGITAL L", 8.205128, 50 },
{ "Canon DIGITAL IXUS i zoom", 8.283622, 50 },
{ "Canon DIGITAL IXUS i5", 8.205128, 50 },
{ "Canon DIGITAL IXUS i7 zoom", 8.283622, 50 },
{ "Canon IXY DIGITAL 200a", 7.714286, 50 },
{ "Canon IXY DIGITAL 320", 7.714286, 50 },
{ "Canon EOS Kiss F", 31.250000, 50 },
{ "Canon EOS 1000D", 31.250000, 50 },
{ "Canon EOS 10D", 31.250000, 50 },
{ "Canon EOS 1100D", 31.250000, 50 },
{ "Canon EOS 20D", 31.250000, 50 },
{ "EOS 20D", 31.250000, 50 },
{ "Canon EOS 300D DIGITAL", 31.250000, 50 },
{ "Canon EOS 30D", 31.250000, 50 },
{ "Canon EOS 350D DIGITAL", 31.250000, 50 },
{ "EOS Kiss Digital X", 31.250000, 50 },
{ "Canon EOS 400D DIGITAL", 31.250000, 50 },
{ "Canon EOS Kiss Digital X", 31.250000, 50 },
{ "Canon EOS 40D", 31.250000, 50 },
{ "Canon EOS 450D", 31.250000, 50 },
{ "Canon EOS 500D", 31.250000, 50 },
{ "Canon EOS 50D", 31.250000, 50 },
{ "Canon EOS 550D", 31.250000, 50 },
{ "Canon EOS 5D", 50.000000, 50 },
{ "Canon EOS 5D Mark II", 50.000000, 50 },
{ "Canon EOS 5D Mark III", 50.000000, 50 },
{ "Canon EOS 600D", 31.250000, 50 },
{ "Canon EOS 60D", 31.250000, 50 },
{ "Canon EOS 650D", 31.250000, 50 },
{ "Canon EOS 6D", 50.000000, 50 },
{ "Canon EOS 7D", 31.250000, 50 },
{ "Canon EOS D30", 31.250000, 50 },
{ "Canon EOS D60", 31.250000, 50 },
{ "EOS DIGITAL REBEL", 31.250000, 50 },
{ "Canon EOS DIGITAL REBEL XS", 31.250000, 50 },
{ "Canon EOS DIGITAL REBEL XSi", 31.250000, 50 },
{ "Canon EOS DIGITAL REBEL XT", 31.250000, 50 },
{ "Canon EOS DIGITAL REBEL XTi", 31.250000, 50 },
{ "Canon EOS Kiss X4", 31.250000, 50 },
{ "Canon EOS Kiss X5", 31.250000, 50 },
{ "Canon EOS Kiss X50", 31.250000, 50 },
{ "Canon EOS M", 31.250000, 50 },
{ "Canon EOS REBEL T1i", 31.250000, 50 },
{ "Canon EOS REBEL T2i", 31.250000, 50 },
{ "Canon EOS REBEL T3", 31.250000, 50 },
{ "Canon EOS REBEL T3i", 31.250000, 50 },
{ "Canon EOS REBEL T4i", 31.250000, 50 },
{ "Canon EOS-1D Mark II", 40.000000, 50 },
{ "Canon EOS-1D Mark II N", 40.000000, 50 },
{ "Canon EOS-1D Mark III", 38.461538, 50 },
{ "Canon EOS-1D Mark IV", 38.461538, 50 },
{ "Canon EOS-1D X", 50.000000, 50 },
{ "Canon EOS-1DS", 50.000000, 50 },
{ "Canon EOS-1Ds Mark II", 50.000000, 50 },
{ "Canon EOS-1Ds Mark III", 50.000000, 50 },
{ "Canon IXUS 1000HS", 8.750000, 50 },
{ "Canon IXUS 105", 8.928571, 50 },
{ "Canon PowerShot SD1300 IS", 8.928571, 50 },
{ "Canon IXUS 1100 HS", 8.928571, 50 },
{ "Canon IXUS 115 HS", 8.928571, 50 },
{ "Canon IXUS 125 HS", 8.958333, 50 },
{ "Canon IXUS 130", 8.928571, 50 },
{ "Canon DIGITAL IXUS 200 IS", 8.958333, 50 },
{ "Canon IXUS 210", 8.958333, 50 },
{ "Canon IXUS 220 HS", 8.958333, 50 },
{ "Canon IXUS 230 HS", 8.928571, 50 },
{ "Canon IXUS 300 HS", 8.803245, 50 },
{ "Canon IXUS 310 HS", 8.955356, 50 },
{ "Canon DIGITAL IXUS 430", 10.277778, 50 },
{ "Canon IXUS 500 HS", 7.142857, 50 },
{ "Canon DIGITAL IXUS 870 IS", 8.928571, 50 },
{ "Canon IXY 210F", 8.928571, 50 },
{ "Canon IXY 3", 7.142857, 50 },
{ "Canon IXY 410F", 8.958333, 50 },
{ "Canon IXY 50S", 8.750000, 50 },
{ "Canon IXY 600F", 8.928571, 50 },
{ "Canon IXY DIGITAL 25 IS", 8.857143, 50 },
{ "Canon IXY DIGITAL 700", 10.405405, 50 },
{ "Canon IXY DIGITAL WIRELESS", 8.285714, 50 },
{ "Canon PowerShot A10", 7.714286, 50 },
{ "Canon PowerShot A1000 IS", 8.857143, 50 },
{ "Canon PowerShot A1100 IS", 8.857143, 50 },
{ "Canon PowerShot A1200", 8.928571, 50 },
{ "Canon PowerShot A1300", 8.928571, 50 },
{ "Canon PowerShot A20", 7.714286, 50 },
{ "Canon PowerShot A200", 6.410256, 50 },
{ "Canon PowerShot A2000 IS", 8.888889, 50 },
{ "Canon PowerShot A2100 IS", 8.888889, 50 },
{ "Canon PowerShot A2200", 8.928571, 50 },
{ "Canon PowerShot A2300", 8.928571, 50 },
{ "Canon PowerShot A2400 IS", 8.928571, 50 },
{ "Canon PowerShot A30", 7.714286, 50 },
{ "Canon PowerShot A300", 7.575758, 50 },
{ "Canon PowerShot A3000 IS", 8.857143, 50 },
{ "Canon PowerShot A310", 7.575758, 50 },
{ "Canon PowerShot A3100 IS", 8.857143, 50 },
{ "Canon PowerShot A3300 IS", 8.928571, 50 },
{ "Canon PowerShot A3400IS", 8.928571, 50 },
{ "Canon PowerShot A40", 7.714286, 50 },
{ "Canon PowerShot A400", 6.577703, 50 },
{ "Canon PowerShot A4000 IS", 8.928571, 50 },
{ "Canon PowerShot A410", 6.594198, 50 },
{ "Canon PowerShot A430", 6.923077, 50 },
{ "Canon PowerShot A460", 7.105263, 50 },
{ "Canon PowerShot A470", 8.235294, 50 },
{ "Canon PowerShot A480", 8.885565, 50 },
{ "Canon PowerShot A495", 8.885565, 50 },
{ "Canon PowerShot A50", 7.696387, 50 },
{ "Canon PowerShot A510", 8.285714, 50 },
{ "Canon PowerShot A520", 8.285714, 50 },
{ "Canon PowerShot A530", 8.285714, 50 },
{ "Canon PowerShot A540", 8.285714, 50 },
{ "Canon PowerShot A550", 8.285714, 50 },
{ "Canon PowerShot A560", 8.285714, 50 },
{ "Canon PowerShot A570 IS", 8.285714, 50 },
{ "Canon PowerShot A580", 8.285714, 50 },
{ "Canon PowerShot A590 IS", 8.285714, 50 },
{ "Canon PowerShot A60", 7.714286, 50 },
{ "Canon PowerShot A610", 10.428571, 50 },
{ "Canon PowerShot A620", 10.428571, 50 },
{ "Canon PowerShot A630", 10.428571, 50 },
{ "Canon PowerShot A640", 10.428571, 50 },
{ "Canon PowerShot A650 IS", 10.571429, 50 },
{ "Canon PowerShot A70", 7.714286, 50 },
{ "Canon PowerShot A700", 8.285714, 50 },
{ "Canon PowerShot A710 IS", 8.285714, 50 },
{ "Canon PowerShot A720 IS", 8.285714, 50 },
{ "Canon PowerShot A75", 7.714286, 50 },
{ "Canon PowerShot A80", 10.263158, 50 },
{ "Canon PowerShot A800", 8.885565, 50 },
{ "Canon PowerShot A810", 8.928571, 50 },
{ "Canon PowerShot A85", 7.714286, 50 },
{ "Canon PowerShot A95", 10.263158, 50 },
{ "Canon PowerShot D10", 8.857143, 50 },
{ "Canon PowerShot D20", 8.928571, 50 },
{ "Canon PowerShot E1", 8.857143, 50 },
{ "Canon PowerShot ELPH 100 HS", 8.928571, 50 },
{ "Canon PowerShot ELPH 300 HS", 8.958333, 50 },
{ "Canon PowerShot ELPH 500 HS", 8.955356, 50 },
{ "Canon PowerShot G1", 10.294118, 50 },
{ "Canon PowerShot G1 X", 26.964286, 50 },
{ "Canon PowerShot G10", 10.892857, 50 },
{ "Canon PowerShot G11", 10.892857, 50 },
{ "Canon PowerShot G12", 10.892857, 50 },
{ "Canon PowerShot G15", 10.892857, 50 },
{ "Canon PowerShot G2", 10.294118, 50 },
{ "Canon PowerShot G3", 10.285714, 50 },
{ "Canon PowerShot G5", 10.285714, 50 },
{ "Canon PowerShot G6", 10.285714, 50 },
{ "Canon PowerShot G9", 10.571429, 50 },
{ "Canon PowerShot Pro1", 12.778088, 50 },
{ "Canon PowerShot Pro90 IS", 9.459459, 50 },
{ "Canon PowerShot S1 IS", 7.631579, 50 },
{ "Canon PowerShot S10", 9.000000, 50 },
{ "Canon PowerShot S100", 10.833333, 50 },
{ "Canon PowerShot S110", 7.714286, 50 },
{ "Canon PowerShot S2 IS", 8.333333, 50 },
{ "Canon PowerShot S20", 10.156250, 50 },
{ "Canon PowerShot S200", 7.714286, 50 },
{ "Canon PowerShot S230", 7.714286, 50 },
{ "Canon PowerShot S3 IS", 8.333333, 50 },
{ "Canon PowerShot S30", 10.142857, 50 },
{ "Canon PowerShot S300", 7.714286, 50 },
{ "Canon PowerShot S330", 7.714286, 50 },
{ "Canon PowerShot S40", 10.142857, 50 },
{ "Canon PowerShot S400", 10.277778, 50 },
{ "Canon PowerShot S410", 10.277778, 50 },
{ "Canon PowerShot S45", 10.142857, 50 },
{ "Canon PowerShot S5 IS", 8.333333, 50 },
{ "Canon PowerShot S50", 10.142857, 50 },
{ "Canon PowerShot S500", 10.277778, 50 },
{ "Canon PowerShot S60", 10.353570, 50 },
{ "Canon PowerShot S70", 10.353570, 50 },
{ "Canon PowerShot S80", 10.353570, 50 },
{ "Canon PowerShot S90", 10.714286, 50 },
{ "Canon PowerShot S95", 10.714286, 50 },
{ "Canon PowerShot SD10", 8.205128, 50 },
{ "Canon PowerShot SD100", 7.714286, 50 },
{ "Canon PowerShot SD1000", 8.285714, 50 },
{ "Canon PowerShot SD110", 7.714286, 50 },
{ "Canon PowerShot SD1100 IS", 8.157895, 50 },
{ "Canon PowerShot SD1200 IS", 8.857143, 50 },
{ "Canon PowerShot SD20", 8.205128, 50 },
{ "Canon PowerShot SD200", 8.285714, 50 },
{ "Canon PowerShot SD30", 8.283622, 50 },
{ "Canon PowerShot SD300", 8.285714, 50 },
{ "Canon PowerShot SD40", 8.283622, 50 },
{ "Canon PowerShot SD400", 8.285714, 50 },
{ "Canon PowerShot SD430 WIRELESS", 8.285714, 50 },
{ "Canon PowerShot SD450", 8.285714, 50 },
{ "Canon PowerShot SD4500 IS", 8.750000, 50 },
{ "Canon PowerShot SD500", 10.405405, 50 },
{ "Canon PowerShot SD550", 10.405405, 50 },
{ "Canon PowerShot SD600", 8.285714, 50 },
{ "Canon PowerShot SD630", 8.285714, 50 },
{ "Canon PowerShot SD700 IS", 8.285714, 50 },
{ "Canon PowerShot SD750", 8.285714, 50 },
{ "Canon PowerShot SD770 IS", 8.857143, 50 },
{ "Canon PowerShot SD800 IS", 8.226173, 50 },
{ "Canon PowerShot SD850 IS", 8.285714, 50 },
{ "Canon IXY DIGITAL 910 IS", 8.226173, 50 },
{ "Canon PowerShot SD870 IS", 8.226173, 50 },
{ "Canon PowerShot SD880 IS", 8.928571, 50 },
{ "Canon PowerShot SD900", 10.405405, 50 },
{ "Canon PowerShot SD950 IS", 10.704356, 50 },
{ "Canon PowerShot SD960 IS", 8.928571, 50 },
{ "Canon PowerShot SD970 IS", 8.918919, 50 },
{ "Canon PowerShot SD990 IS", 10.704356, 50 },
{ "Canon PowerShot SX1 IS", 8.928571, 50 },
{ "Canon PowerShot SX10 IS", 8.928571, 50 },
{ "Canon PowerShot SX100 IS", 8.333333, 50 },
{ "Canon PowerShot SX110 IS", 8.333333, 50 },
{ "Canon PowerShot SX120 IS", 8.333333, 50 },
{ "Canon PowerShot SX130 IS", 8.928571, 50 },
{ "Canon PowerShot SX150 IS", 8.928571, 50 },
{ "Canon PowerShot SX160 IS", 8.928571, 50 },
{ "Canon PowerShot SX20 IS", 8.928571, 50 },
{ "Canon PowerShot SX200 IS", 8.928571, 50 },
{ "Canon PowerShot SX210 IS", 8.928571, 50 },
{ "Canon PowerShot SX220 HS", 8.928571, 50 },
{ "Canon PowerShot SX230 HS", 8.928571, 50 },
{ "Canon PowerShot SX240 HS", 9.000000, 50 },
{ "Canon PowerShot SX260 HS", 9.000000, 50 },
{ "Canon PowerShot SX30 IS", 8.958333, 50 },
{ "Canon PowerShot SX40 HS", 8.958333, 50 },
{ "Canon PowerShot SX50 HS", 8.958333, 50 },
{ "Canon PowerShot SX500 IS", 8.958333, 50 },
{ "Canon PowerShot TX1", 8.333333, 50 },
{ "EX-S600", 8.157895, 50 },
{ "EX-Z1000", 10.394737, 50 },
{ "EX-Z60", 8.289474, 50 },
{ "EX-Z600", 8.157895, 50 },
{ "EX-Z850", 10.394737, 50 },
{ "EX-FC100", 8.675676, 50 },
{ "EX-FH100", 8.958333, 50 },
{ "EX-FH20", 8.846154, 50 },
{ "EX-FH25", 8.846154, 50 },
{ "EX-FS10", 8.740157, 50 },
{ "EX-G1", 8.763158, 50 },
{ "EX-H10", 8.958333, 50 },
{ "EX-H15", 8.958333, 50 },
{ "EX-H20G", 8.958333, 50 },
{ "EX-H30", 8.833333, 50 },
{ "EX-H5", 8.958333, 50 },
{ "EX-M1", 7.567568, 50 },
{ "EX-M2", 10.416667, 50 },
{ "EX-M20", 7.567568, 50 },
{ "EX-N1", 8.846154, 50 },
{ "EX-P505", 8.289474, 50 },
{ "EX-P600", 10.757576, 50 },
{ "EX-P700", 10.757576, 50 },
{ "EX-S1", 7.567568, 50 },
{ "EX-S10", 8.289474, 50 },
{ "EX-S100", 6.642066, 50 },
{ "EX-S12", 8.750000, 50 },
{ "EX-S2", 10.416667, 50 },
{ "EX-S20", 7.837838, 50 },
{ "EX-S200", 9.074074, 50 },
{ "EX-S3", 10.142857, 50 },
{ "EX-S500", 8.157895, 50 },
{ "EX-S770", 8.157895, 50 },
{ "EX-S880", 8.157895, 50 },
{ "EX-TR150", 9.047619, 50 },
{ "EX-V7", 8.289474, 50 },
{ "EX-V8", 8.289474, 50 },
{ "EX-Z1", 8.873239, 50 },
{ "EX-Z10", 8.289474, 50 },
{ "EX-Z100", 8.750000, 50 },
{ "EX-Z1050", 10.394737, 50 },
{ "EX-Z1080", 10.394737, 50 },
{ "EX-Z110", 8.289474, 50 },
{ "EX-Z120", 10.394737, 50 },
{ "EX-Z1200", 10.675676, 50 },
{ "EX-Z150", 8.303571, 50 },
{ "EX-Z2", 8.873239, 50 },
{ "EX-Z20", 8.289474, 50 },
{ "EX-Z200", 8.750000, 50 },
{ "EX-Z2000", 8.636752, 50 },
{ "EX-Z21", 8.289474, 50 },
{ "EX-Z2300", 9.038462, 50 },
{ "EX-Z25", 9.000000, 50 },
{ "EX-Z250", 8.303571, 50 },
{ "EX-Z270", 8.303571, 50 },
{ "EX-Z280", 8.942308, 50 },
{ "EX-Z29", 8.266667, 50 },
{ "EX-Z3", 8.285714, 50 },
{ "EX-Z300", 8.750000, 50 },
{ "EX-Z3000", 8.800000, 50 },
{ "EX-Z33", 8.873239, 50 },
{ "EX-Z330", 8.873239, 50 },
{ "EX-Z35", 8.873239, 50 },
{ "EX-Z350", 9.074074, 50 },
{ "EX-Z37", 8.923513, 50 },
{ "EX-Z370", 9.074074, 50 },
{ "EX-Z4", 8.285714, 50 },
{ "EX-Z40", 8.285714, 50 },
{ "EX-Z400", 8.750000, 50 },
{ "EX-Z450", 8.750000, 50 },
{ "EX-Z5", 8.223158, 50 },
{ "EX-Z50", 8.285714, 50 },
{ "EX-Z500", 8.157895, 50 },
{ "EX-Z55", 8.285714, 50 },
{ "EX-Z550", 8.942308, 50 },
{ "EX-Z57", 10.394737, 50 },
{ "EX-Z65", 8.289474, 50 },
{ "EX-Z70", 8.223158, 50 },
{ "EX-Z700", 8.157895, 50 },
{ "EX-Z75", 8.289474, 50 },
{ "EX-Z750", 10.394737, 50 },
{ "EX-Z77", 8.289474, 50 },
{ "EX-Z80", 8.289474, 50 },
{ "EX-Z800", 9.074074, 50 },
{ "EX-Z85", 8.223158, 50 },
{ "EX-Z9", 8.266667, 50 },
{ "EX-Z90", 9.000000, 50 },
{ "EX-ZR10", 8.928571, 50 },
{ "EX-ZR100", 8.833333, 50 },
{ "EX-ZR15", 8.928571, 50 },
{ "EX-ZR20", 8.800000, 50 },
{ "EX-ZR200", 8.833333, 50 },
{ "EX-ZR300", 8.833333, 50 },
{ "EX-ZS10", 8.846154, 50 },
{ "EX-ZS100", 8.833333, 50 },
{ "EX-ZS12", 8.846154, 50 },
{ "EX-ZS150", 8.833333, 50 },
{ "EX-ZS20", 8.846154, 50 },
{ "EX-ZS5", 9.038462, 50 },
{ "EX-ZS6", 9.038462, 50 },
{ "EX-F1", 10.138889, 50 },
{ "EX-TR100", 9.047619, 50 },
{ "GV-10", 6.571429, 50 },
{ "QV-2000UX", 9.027778, 50 },
{ "QV-2300UX", 7.560976, 50 },
{ "QV-2900UX", 7.500000, 50 },
{ "QV-3000EX", 10.552764, 50 },
{ "QV-3500EX", 10.552764, 50 },
{ "QV-3EX", 10.121951, 50 },
{ "QV-4000", 10.294118, 50 },
{ "QV-5700", 10.294118, 50 },
{ "QV-R200", 9.038462, 50 },
{ "QV-R4", 10.133333, 50 },
{ "QV-R40", 10.256410, 50 },
{ "QV-R41", 10.256410, 50 },
{ "QV-R51", 10.256410, 50 },
{ "QV-R52", 10.256410, 50 },
{ "QV-R61", 10.256410, 50 },
{ "QV-R62", 10.256410, 50 },
{ "8.00", 20.405494, 50 },
{ "Eye-Q 3340z", 7.631579, 50 },
{ "Eye-Q 4060 AF", 10.416667, 50 },
{ "Eye-Q 4360Z", 10.309469, 50 },
{ "PhotoPC 3000Z", 10.294118, 50 },
{ "PhotoPC 3100Z", 10.294118, 50 },
{ "PhotoPC 850Z", 9.285714, 50 },
{ "X10", 12.678571, 50 },
{ "A100", 8.873239, 50 },
{ "Fujifilm A170", 8.906250, 50 },
{ "Fujifilm A220", 8.906250, 50 },
{ "A850", 8.472222, 50 },
{ "DS-260HD", 10.571429, 50 },
{ "FinePix  Z110", 8.928571, 50 },
{ "FinePix1300", 8.055556, 50 },
{ "FinePix1400Zoom", 7.692308, 50 },
{ "FinePix2300", 8.055556, 50 },
{ "FinePix2400Zoom", 7.894737, 50 },
{ "FinePix2600Z", 7.894737, 50 },
{ "FinePix2650", 7.894737, 50 },
{ "FinePix2800Zoom", 7.894737, 50 },
{ "FinePix30i", 7.631579, 50 },
{ "FinePix 3800", 6.976744, 50 },
{ "FinePix40i", 11.527778, 50 },
{ "FinePix4700Z", 11.527778, 50 },
{ "FinePix4800Z", 11.527778, 50 },
{ "FinePix4900ZOOM", 11.142857, 50 },
{ "FinePix6800Z", 11.527778, 50 },
{ "FinePix6900ZOOM", 11.142857, 50 },
{ "FinePixA101", 8.055556, 50 },
{ "FinePix A120", 7.763158, 50 },
{ "FinePixA201", 8.055556, 50 },
{ "FinePix A202", 7.638889, 50 },
{ "FinePix A203", 7.500000, 50 },
{ "FinePix A205S", 7.638889, 50 },
{ "FinePix A210", 7.638889, 50 },
{ "FinePix A303", 7.500000, 50 },
{ "FinePix A310", 7.500000, 50 },
{ "FinePix A330", 7.500000, 50 },
{ "FinePix A340", 7.500000, 50 },
{ "FinePix A345", 8.285714, 50 },
{ "FinePix A350", 8.285714, 50 },
{ "FinePix A400", 8.421053, 50 },
{ "FinePix A500", 8.421053, 50 },
{ "FinePix A600", 11.111111, 50 },
{ "FinePix A610", 8.461538, 50 },
{ "FinePix A700", 11.111111, 50 },
{ "FinePix A800", 11.111111, 50 },
{ "FinePix A820", 11.282051, 50 },
{ "FinePix A900", 11.282051, 50 },
{ "FinePix A920", 11.282051, 50 },
{ "FinePix AV100 AV110", 8.906250, 50 },
{ "FinePix AV200", 8.906250, 50 },
{ "FinePix AV230", 8.906250, 50 },
{ "FinePix AX300", 8.939394, 50 },
{ "FinePix E500", 8.344503, 50 },
{ "FinePix E510", 8.344503, 50 },
{ "FinePix E550", 11.076923, 50 },
{ "FinePix E900", 11.250000, 50 },
{ "FinePix F10", 11.111111, 50 },
{ "FinePix F100fd", 11.428571, 50 },
{ "FinePix F11", 11.111111, 50 },
{ "FinePix F20", 11.111111, 50 },
{ "FinePix F200EXR", 11.428571, 50 },
{ "FinePix F30", 11.111111, 50 },
{ "FinePix F300EXR", 9.166667, 50 },
{ "FinePix F31fd", 11.111111, 50 },
{ "FinePix F40fd", 11.111111, 50 },
{ "FinePix F401", 7.500000, 50 },
{ "FinePix F410", 7.500000, 50 },
{ "FinePix F420", 7.638889, 50 },
{ "FinePix F440", 8.298573, 50 },
{ "FinePix F450", 8.298573, 50 },
{ "FinePix F455", 8.298573, 50 },
{ "FinePix F460", 8.285714, 50 },
{ "FinePix F470", 8.285714, 50 },
{ "FinePix F480", 8.214286, 50 },
{ "FinePix F500EXR", 9.166667, 50 },
{ "FinePix F50fd", 11.428571, 50 },
{ "FinePix F550EXR", 9.166667, 50 },
{ "FinePix F600EXR", 9.166667, 50 },
{ "FinePix F601", 11.527778, 50 },
{ "FinePix F60fd", 11.428571, 50 },
{ "FinePix F610", 10.428571, 50 },
{ "FinePix F650", 8.055556, 50 },
{ "FinePix F700", 10.428571, 50 },
{ "FinePix F70EXR", 9.259259, 50 },
{ "FinePix F710", 11.076923, 50 },
{ "FinePix F770EXR", 9.200000, 50 },
{ "FinePix F800EXR", 9.200000, 50 },
{ "FinePix F80EXR", 9.259259, 50 },
{ "FinePix F810", 11.076923, 50 },
{ "FinePix HS10 HS11", 8.750000, 50 },
{ "FinePix HS20EXR", 8.750000, 50 },
{ "FinePix HS30EXR", 8.750000, 50 },
{ "FinePix J10", 8.532110, 50 },
{ "FinePix J100", 9.000000, 50 },
{ "FinePix J110W", 9.107143, 50 },
{ "FinePix J120", 9.000000, 50 },
{ "FinePix J150W", 9.107143, 50 },
{ "FinePix J15fd", 8.193833, 50 },
{ "FinePix J20", 8.873239, 50 },
{ "FinePix J27 J28 J29", 8.906250, 50 },
{ "FinePix J30", 8.906250, 50 },
{ "FinePix J50", 8.378378, 50 },
{ "FinePix JV100 JV110", 8.918919, 50 },
{ "FinePix JV200", 9.027778, 50 },
{ "FinePix JX400", 8.928571, 50 },
{ "FinePix JX420", 8.928571, 50 },
{ "FinePix JZ300", 8.928571, 50 },
{ "FinePix JZ500", 8.928571, 50 },
{ "FinePix M603", 11.447368, 50 },
{ "FinePix S1000fd", 8.939394, 50 },
{ "FinePix S100FS", 12.683034, 50 },
{ "FinePix S1500", 8.939394, 50 },
{ "FinePix S1600", 8.928571, 50 },
{ "FinePix S1700", 8.928571, 50 },
{ "FinePix S1800", 8.928571, 50 },
{ "FinePix S20Pro", 11.142857, 50 },
{ "FinePix S2000HD", 9.057971, 50 },
{ "FinePix S200EXR", 11.639626, 50 },
{ "FinePix S2500HD", 8.964143, 50 },
{ "FinePix S2800HD", 8.928571, 50 },
{ "FinePix S2950", 8.928571, 50 },
{ "FinePix S3Pro", 33.333333, 50 },
{ "FinePix S3000", 7.894737, 50 },
{ "FinePix S304", 7.894737, 50 },
{ "FinePix S3100", 7.692308, 50 },
{ "FinePix S3200", 8.958333, 50 },
{ "FinePix S3400", 8.958333, 50 },
{ "FinePix S4000", 8.958333, 50 },
{ "FinePix S5Pro", 33.333333, 50 },
{ "FinePix S5000", 7.702703, 50 },
{ "FinePix S5100", 7.702703, 50 },
{ "FinePix S5200", 8.421053, 50 },
{ "FinePix S5600", 8.289474, 50 },
{ "FinePix S5700 S700", 8.328947, 50 },
{ "FinePix S5800 S800", 8.328947, 50 },
{ "FinePix S602 ZOOM", 11.142857, 50 },
{ "FinePix S6000fd", 11.094002, 50 },
{ "FinePix S7000", 11.142857, 50 },
{ "FinePix S8000fd", 8.683079, 50 },
{ "FinePix S8100fd", 8.683079, 50 },
{ "FinePix S9000", 11.094002, 50 },
{ "FinePix S9100", 11.094002, 50 },
{ "FinePix SL300", 8.958333, 50 },
{ "FinePix T200", 8.928571, 50 },
{ "FinePix T300", 8.928571, 50 },
{ "FinePix T350", 8.928571, 50 },
{ "FinePix T400", 8.928571, 50 },
{ "FinePix V10", 8.298573, 50 },
{ "FinePix X100", 32.857143, 50 },
{ "FinePix XP10", 8.888889, 50 },
{ "FinePix XP100", 8.928571, 50 },
{ "FinePix XP150", 8.928571, 50 },
{ "FinePix XP20", 8.928571, 50 },
{ "FinePix XP30", 8.928571, 50 },
{ "FinePix XP50", 8.928571, 50 },
{ "FinePix Z1", 8.472222, 50 },
{ "FinePix Z1000EXR", 8.928571, 50 },
{ "FinePix Z100fd", 8.194444, 50 },
{ "FinePix Z10fd", 8.289474, 50 },
{ "FinePix Z1100EXR", 8.928571, 50 },
{ "FinePix Z2", 8.472222, 50 },
{ "FinePix Z200fd", 8.939394, 50 },
{ "FinePix Z20fd", 9.000000, 50 },
{ "FinePix Z250fd", 8.939394, 50 },
{ "FinePix Z3", 8.472222, 50 },
{ "FinePix Z300", 8.888889, 50 },
{ "FinePix Z33WP", 9.000000, 50 },
{ "FinePix Z37", 8.939970, 50 },
{ "FinePix Z5fd", 8.472222, 50 },
{ "FinePix Z70", 8.888889, 50 },
{ "FinePix Z700EXR", 9.142857, 50 },
{ "FinePix Z80", 8.888889, 50 },
{ "FinePix Z800EXR", 9.142857, 50 },
{ "FinePix Z90", 8.928571, 50 },
{ "FinePix Z900EXR", 8.928571, 50 },
{ "MX-1200", 7.631579, 50 },
{ "MX-1700ZOOM", 8.684211, 50 },
{ "MX-2700", 10.857143, 50 },
{ "MX-2900ZOOM", 10.523594, 50 },
{ "X-E1", 33.333333, 50 },
{ "X-Pro1", 33.333333, 50 },
{ "X-S1", 12.708333, 50 },
{ "XF1", 12.800000, 50 },
{ "A830", 8.472222, 50 },
{ "G100", 8.928571, 50 },
{ "HD2", 7.817386, 50 },
{ "HTC One S", 6.258621, 50 },
{ "HTC One V", 5.050000, 50 },
{ "HTC One X", 5.224138, 50 },
{ "Hasselblad H4D-200MS", 71.428571, 50 },
{ "Hasselblad H4D-31", 64.102564, 50 },
{ "Hasselblad H4D-40", 64.102564, 50 },
{ "Hasselblad H4D-60", 78.125000, 50 },
{ "HP PhotoSmart C500 (V01.00)", 9.039301, 50 },
{ "HP Photosmart R837", 8.297872, 50 },
{ "hp PhotoSmart 620", 7.714286, 50 },
{ "HP PhotoSmart 715", 10.294118, 50 },
{ "HP PhotoSmart C812 (V08.28)", 10.270270, 50 },
{ "HP PhotoSmart C850 (V05.28)", 10.218206, 50 },
{ "HP PhotoSmart C912 (V01.00)", 12.057449, 50 },
{ "HP PhotoSmart C935 (V03.53)", 10.270270, 50 },
{ "HP PhotoSmart C945 (V01.60)", 10.218206, 50 },
{ "HP PhotoSmart R507 (V01.00)", 8.285714, 50 },
{ "HP PhotoSmart R607 (V01.00)", 8.285714, 50 },
{ "HP PhotoSmart R707 (V01.00)", 10.256410, 50 },
{ "HP PhotoSmart R717 (V01.00)", 10.256410, 50 },
{ "HP PhotoSmart R727  (V01.00)", 8.297872, 50 },
{ "HP PhotoSmart R817  (V01.00)", 8.055556, 50 },
{ "HP PhotoSmart R927  (V01.00)", 10.714286, 50 },
{ "hp photosmart 612", 7.906977, 50 },
{ "hp photosmart 720", 10.294118, 50 },
{ "hp photosmart 735", 7.631579, 50 },
{ "HP Photosmart E317", 8.324251, 50 },
{ "HP Photosmart E327", 8.351410, 50 },
{ "HP Photosmart M22 (V01.00) dÿÿÿÿ", 8.324251, 50 },
{ "Photosmart M307", 7.791328, 50 },
{ "Photosmart M407", 8.333333, 50 },
{ "Photosmart M425", 7.894737, 50 },
{ "HP Photosmart M437", 8.088235, 50 },
{ "Photosmart M517", 8.333333, 50 },
{ "Photosmart M527", 8.219178, 50 },
{ "HP Photosmart M537", 8.088235, 50 },
{ "HP Photosmart M540", 8.194444, 50 },
{ "Photosmart M627", 8.181818, 50 },
{ "HP Photosmart R827  (V01.00)", 8.319185, 50 },
{ "hp 635 Digital Camera", 6.756757, 50 },
{ "JD4100", 10.294118, 50 },
{ "JD5200", 10.526316, 50 },
{ "DC210 Zoom (V05.00)", 7.586207, 50 },
{ "KODAK DC280 ZOOM DIGITAL CAMERA", 10.833333, 50 },
{ "KODAK DC290 Zoom Digital Camera (V01.00)", 10.416667, 50 },
{ "KODAK DC3200 DIGITAL CAMERA", 6.923077, 50 },
{ "KODAK DC4800 ZOOM DIGITAL CAMERA", 10.714286, 50 },
{ "KODAK DC5000 ZOOM DIGITAL CAMERA", 10.833333, 50 },
{ "KODAK DX3600 DIGITAL CAMERA", 7.964126, 50 },
{ "KODAK DX3900 ZOOM DIGITAL CAMERA", 10.428571, 50 },
{ "KODAK DX4530 ZOOM DIGITAL CAMERA", 10.526316, 50 },
{ "KODAK DX4900 ZOOM DIGITAL CAMERA", 10.428571, 50 },
{ "KODAK DX6440 ZOOM DIGITAL CAMERA", 8.329544, 50 },
{ "KODAK DX6490 ZOOM DIGITAL CAMERA", 8.302611, 50 },
{ "KODAK EASYSHARE M590 DIGITAL CAMERA", 9.000000, 50 },
{ "KODAK EASYSHARE Mini Camera, M200", 7.068966, 50 },
{ "KODAK EASYSHARE Sport Camera, C123", 8.289474, 50 },
{ "KODAK EASYSHARE Touch Camera, M577", 9.107143, 50 },
{ "KODAK EASYSHARE C653 ZOOM DIGITAL CAMERA", 8.333333, 50 },
{ "KODAK EASYSHARE C763 ZOOM DIGITAL CAMERA", 8.291874, 50 },
{ "KODAK EASYSHARE CX7220 ZOOM DIGITAL CAMERA", 6.538462, 50 },
{ "KODAK CX7525 ZOOM DIGITAL CAMERA", 8.235294, 50 },
{ "KODAK EASYSHARE Camera, M532", 8.750000, 50 },
{ "KODAK EASYSHARE M753 ZOOM DIGITAL CAMERA", 8.389262, 50 },
{ "KODAK EASYSHARE M853 ZOOM DIGITAL CAMERA", 8.375209, 50 },
{ "KODAK EASYSHARE M873 ZOOM DIGITAL CAMERA", 8.333333, 50 },
{ "KODAK EASYSHARE M883 ZOOM DIGITAL CAMERA", 8.403361, 50 },
{ "KODAK EASYSHARE M893 IS DIGITAL CAMERA", 8.403361, 50 },
{ "KODAK EasyShare Z990 Digital Camera", 8.934520, 50 },
{ "KODAK EasyShare Z990 Digital Camera", 8.928571, 50 },
{ "KODAK P850 ZOOM DIGITAL CAMERA", 8.333333, 50 },
{ "KODAK EASYSHARE V1003 ZOOM DIGITAL CAMERA", 10.416667, 50 },
{ "KODAK V1253 ZOOM DIGITAL CAMERA", 10.683761, 50 },
{ "KODAK EASYSHARE V803 ZOOM DIGITAL CAMERA", 10.416667, 50 },
{ "KODAK Z1275 ZOOM DIGITAL CAMERA", 10.482180, 50 },
{ "KODAK Z712 IS ZOOM DIGITAL CAMERA", 8.130081, 50 },
{ "KODAK Z730 ZOOM DIGITAL CAMERA", 8.295282, 50 },
{ "KODAK Z7590 ZOOM DIGITAL CAMERA", 8.315789, 50 },
{ "KODAK Z812 IS ZOOM DIGITAL CAMERA", 8.389262, 50 },
{ "KODAK Z885 ZOOM DIGITAL CAMERA", 10.416667, 50 },
{ "KODAK EasyShare Z981 Digital Camera", 8.846154, 50 },
{ "Kodak EasyShare-One (S143)", 7.996146, 50 },
{ "KODAK EASYSHARE CX6200 DIGITAL CAMERA", 6.756757, 50 },
{ "KODAK EASYSHARE CX7300 DIGITAL CAMERA", 7.972973, 50 },
{ "KODAK EASYSHARE CX7310 DIGITAL CAMERA", 7.972973, 50 },
{ "KODAK CX7330 ZOOM DIGITAL CAMERA", 7.567568, 50 },
{ "KODAK CX7430 ZOOM DIGITAL CAMERA", 8.235294, 50 },
{ "KODAK CX7530 ZOOM DIGITAL CAMERA", 8.235294, 50 },
{ "KODAK EASYSHARE DX3700 Digital Camera", 10.675676, 50 },
{ "KODAK DX7440 ZOOM DIGITAL CAMERA", 8.295282, 50 },
{ "KODAK DX7590 ZOOM DIGITAL CAMERA", 8.315789, 50 },
{ "KODAK DX7630 ZOOM DIGITAL CAMERA", 10.256410, 50 },
{ "KODAK LS743 ZOOM DIGITAL CAMERA", 8.316633, 50 },
{ "KODAK LS753 ZOOM DIGITAL CAMERA", 8.316633, 50 },
{ "KODAK LS755 ZOOM DIGITAL CAMERA", 8.356417, 50 },
{ "KODAK LS420 DIGITAL CAMERA", 7.894737, 50 },
{ "KD-300Z", 10.540541, 50 },
{ "Konica Digital Camera KD-310Z", 10.256410, 50 },
{ "Konica Digital Camera KD-400Z", 10.256410, 50 },
{ "Konica Digital Camera KD-410Z", 10.256410, 50 },
{ "Konica Digital Camera KD-500Z", 10.256410, 50 },
{ "Konica Digital Camera KD-510Z", 10.256410, 50 },
{ "Konica Digital Camera KD-200Z", 7.714286, 50 },
{ "KD-220Z", 8.082902, 50 },
{ "Revio C2", 6.081081, 50 },
{ "DYNAX 5D", 33.333333, 50 },
{ "DYNAX 7D", 32.894737, 50 },
{ "DiMAGE A2", 12.778088, 50 },
{ "DiMAGE A200", 12.778088, 50 },
{ "DiMAGE G400", 8.275862, 50 },
{ "DiMAGE G530", 8.235294, 50 },
{ "DiMAGE G600", 10.256410, 50 },
{ "DiMAGE X1", 10.405405, 50 },
{ "DiMAGE X21", 6.486486, 50 },
{ "DiMAGE X31", 6.527778, 50 },
{ "DiMAGE X50", 8.192743, 50 },
{ "DiMAGE X60", 8.289474, 50 },
{ "DiMAGE Xg", 7.702703, 50 },
{ "DiMAGE Z10", 8.304498, 50 },
{ "DiMAGE Z2", 8.289474, 50 },
{ "DiMAGE Z20", 8.304498, 50 },
{ "DiMAGE Z3", 8.324998, 50 },
{ "DiMAGE Z5", 8.324998, 50 },
{ "DiMAGE Z6", 8.324998, 50 },
{ "MAXXUM 5D", 33.333333, 50 },
{ "MAXXUM 7D", 32.894737, 50 },
{ "CONTAX Tvs Digital", 10.428571, 50 },
{ "CONTAX SL300R T*", 7.598253, 50 },
{ "Finecam-L30", 7.598253, 50 },
{ "Finecam-L4v", 8.285714, 50 },
{ "Finecam M410R", 7.702703, 50 },
{ "FC-S3", 10.263158, 50 },
{ "Finecam S3L", 10.428571, 50 },
{ "FC-S3x", 10.428571, 50 },
{ "FC-S4", 10.428571, 50 },
{ "Finecam S5", 10.428571, 50 },
{ "Finecam S5R", 10.428571, 50 },
{ "Finecam SL300R", 7.598253, 50 },
{ "Finecam SL400R", 7.598253, 50 },
{ "Finecam-L3v", 7.631579, 50 },
{ "C-LUX 1", 8.224777, 50 },
{ "C-LUX 2", 8.207137, 50 },
{ "C-LUX 3", 8.800000, 50 },
{ "D-LUX3", 11.250000, 50 },
{ "D-LUX 4", 10.645793, 50 },
{ "D-LUX 5", 10.645793, 50 },
{ "D-LUX2", 11.250000, 50 },
{ "D-LUX", 8.285714, 50 },
{ "DIGILUX3", 25.000000, 50 },
{ "Digilux1", 10.552764, 50 },
{ "DIGILUX 2", 12.500000, 50 },
{ "Leica S3", 62.500000, 50 },
{ "M Monochrom", 50.000000, 50 },
{ "M8 Digital Camera", 37.593985, 50 },
{ "M9 Digital Camera", 50.000000, 50 },
{ "R9 - Digital Back DMR", 36.496350, 50 },
{ "S2", 62.500000, 50 },
{ "V-LUX 1", 10.571429, 50 },
{ "V-LUX 2", 9.000000, 50 },
{ "V-LUX 20", 8.200000, 50 },
{ "V-LUX 3", 9.000000, 50 },
{ "V-LUX 30", 8.958333, 50 },
{ "V-LUX 40", 8.958333, 50 },
{ "LEICA X1", 33.333333, 50 },
{ "LEICA X2", 33.333333, 50 },
{ "digilux 4.3", 11.527778, 50 },
{ "digilux zoom", 8.684211, 50 },
{ "Leica S2", 62.500000, 50 },
{ "MD9700", 10.526316, 50 },
{ "Dimage 2300", 10.789474, 50 },
{ "Dimage 2330 Zoom", 10.526316, 50 },
{ "DiMAGE 5", 10.222471, 50 },
{ "DiMAGE 7", 12.778088, 50 },
{ "DiMAGE 7Hi", 12.778088, 50 },
{ "DiMAGE 7i", 12.778088, 50 },
{ "DiMAGE A1", 12.778088, 50 },
{ "DiMAGE E201", 10.789474, 50 },
{ "DiMAGE E323", 7.777778, 50 },
{ "DiMAGE F100", 10.263158, 50 },
{ "DiMAGE F300", 10.263158, 50 },
{ "DiMAGE G500", 10.256410, 50 },
{ "DiMAGE S304", 10.214286, 50 },
{ "DiMAGE S404", 10.214286, 50 },
{ "DiMAGE S414", 10.214286, 50 },
{ "DiMAGE X", 7.702703, 50 },
{ "DiMAGE X20", 6.486486, 50 },
{ "DiMAGE Xi", 7.702703, 50 },
{ "DiMAGE Xt", 7.702703, 50 },
{ "DiMAGE Z1", 7.631579, 50 },
{ "NIKON 1 J1", 18.518519, 50 },
{ "NIKON 1 J2", 18.518519, 50 },
{ "NIKON 1 V1", 18.518519, 50 },
{ "E2000", 7.631579, 50 },
{ "E2100", 6.157205, 50 },
{ "E2500", 7.567568, 50 },
{ "E3100", 7.598253, 50 },
{ "E3200", 7.598253, 50 },
{ "E3500", 7.567568, 50 },
{ "E3700", 7.714286, 50 },
{ "E4100", 8.285714, 50 },
{ "E4200", 10.263158, 50 },
{ "E4300", 10.526316, 50 },
{ "E4500", 10.325763, 50 },
{ "E4600", 8.382353, 50 },
{ "E4800", 8.333333, 50 },
{ "E5000", 12.633242, 50 },
{ "E5200", 10.263158, 50 },
{ "E5400", 10.350982, 50 },
{ "E5600", 8.142857, 50 },
{ "E5700", 12.714286, 50 },
{ "E5900", 10.263158, 50 },
{ "E700", 9.285714, 50 },
{ "E7600", 10.263158, 50 },
{ "E775", 7.598253, 50 },
{ "E7900", 10.263158, 50 },
{ "E800", 9.210526, 50 },
{ "E8400", 12.707108, 50 },
{ "E8700", 12.714286, 50 },
{ "E880", 10.526316, 50 },
{ "E8800", 12.714286, 50 },
{ "E885", 10.526316, 50 },
{ "E950", 9.170306, 50 },
{ "E990", 10.480349, 50 },
{ "E995", 10.526316, 50 },
{ "COOLPIX AW100", 8.928571, 50 },
{ "COOLPIX L1", 8.276295, 50 },
{ "COOLPIX L10", 8.266667, 50 },
{ "COOLPIX L100", 8.928571, 50 },
{ "COOLPIX L11", 8.266667, 50 },
{ "COOLPIX L110", 8.928571, 50 },
{ "COOLPIX L12", 8.142857, 50 },
{ "COOLPIX L120", 9.000000, 50 },
{ "COOLPIX L14", 8.289474, 50 },
{ "COOLPIX L15", 8.142857, 50 },
{ "COOLPIX L16", 8.142857, 50 },
{ "COOLPIX L18", 8.142857, 50 },
{ "COOLPIX L19", 8.222961, 50 },
{ "COOLPIX L2", 8.282662, 50 },
{ "COOLPIX L20", 8.819658, 50 },
{ "COOLPIX L21", 8.222961, 50 },
{ "COOLPIX L22", 9.004368, 50 },
{ "COOLPIX L23", 7.142857, 50 },
{ "COOLPIX L25", 7.142857, 50 },
{ "COOLPIX L26", 8.846154, 50 },
{ "COOLPIX L3", 8.282662, 50 },
{ "COOLPIX L4", 8.289474, 50 },
{ "COOLPIX L5", 8.276295, 50 },
{ "COOLPIX L6", 8.282662, 50 },
{ "COOLPIX L610", 9.000000, 50 },
{ "COOLPIX L810", 8.888889, 50 },
{ "COOLPIX P1", 10.426578, 50 },
{ "COOLPIX P100", 8.847855, 50 },
{ "COOLPIX P2", 10.426578, 50 },
{ "COOLPIX P3", 10.426578, 50 },
{ "COOLPIX P300", 8.954165, 50 },
{ "COOLPIX P310", 8.954165, 50 },
{ "COOLPIX P4", 10.426578, 50 },
{ "COOLPIX P50", 8.362989, 50 },
{ "COOLPIX P500", 8.888889, 50 },
{ "COOLPIX P5000", 10.028981, 50 },
{ "COOLPIX P510", 8.979118, 50 },
{ "COOLPIX P5100", 10.702659, 50 },
{ "COOLPIX P60", 8.888889, 50 },
{ "COOLPIX P6000", 10.714286, 50 },
{ "COOLPIX P7000", 10.682046, 50 },
{ "COOLPIX P7100", 10.682046, 50 },
{ "COOLPIX P7700", 10.707138, 50 },
{ "COOLPIX P80", 8.703704, 50 },
{ "COOLPIX P90", 8.846154, 50 },
{ "COOLPIX S01", 7.068966, 50 },
{ "S1", 8.285714, 50 },
{ "COOLPIX S10", 8.289474, 50 },
{ "COOLPIX S100", 8.928571, 50 },
{ "COOLPIX S1000pj", 8.928571, 50 },
{ "COOLPIX S1100pj", 8.928571, 50 },
{ "COOLPIX S1200pj", 8.928571, 50 },
{ "COOLPIX S200", 8.289474, 50 },
{ "COOLPIX S210", 8.289474, 50 },
{ "COOLPIX S220", 9.000000, 50 },
{ "COOLPIX S230", 9.000000, 50 },
{ "COOLPIX S2500", 9.074074, 50 },
{ "COOLPIX S2600", 8.846154, 50 },
{ "COOLPIX S3", 8.285714, 50 },
{ "COOLPIX S30", 7.044674, 50 },
{ "COOLPIX S3000", 9.074074, 50 },
{ "COOLPIX S3100", 8.846154, 50 },
{ "COOLPIX S3300", 8.846154, 50 },
{ "COOLPIX S4", 8.289474, 50 },
{ "COOLPIX S4000", 9.074074, 50 },
{ "COOLPIX S4100", 8.846154, 50 },
{ "COOLPIX S4300", 8.846154, 50 },
{ "COOLPIX S5", 8.285714, 50 },
{ "COOLPIX S50", 8.289474, 50 },
{ "COOLPIX S500", 8.142857, 50 },
{ "COOLPIX S50c", 8.289474, 50 },
{ "COOLPIX S51", 8.289474, 50 },
{ "COOLPIX S510", 8.142857, 50 },
{ "COOLPIX S5100", 8.928571, 50 },
{ "COOLPIX S51c", 8.289474, 50 },
{ "COOLPIX S52", 8.289474, 50 },
{ "COOLPIX S520", 8.142857, 50 },
{ "COOLPIX S52c", 8.289474, 50 },
{ "COOLPIX S550", 8.750000, 50 },
{ "COOLPIX S560", 9.051724, 50 },
{ "COOLPIX S570", 8.928571, 50 },
{ "COOLPIX S6", 8.285714, 50 },
{ "COOLPIX S60", 8.939394, 50 },
{ "COOLPIX S600", 8.928571, 50 },
{ "COOLPIX S6000", 8.928571, 50 },
{ "COOLPIX S610", 8.928571, 50 },
{ "COOLPIX S6100", 8.928571, 50 },
{ "COOLPIX S610c", 8.928571, 50 },
{ "COOLPIX S6150", 8.928571, 50 },
{ "COOLPIX S620", 8.928571, 50 },
{ "COOLPIX S6200", 9.000000, 50 },
{ "COOLPIX S630", 8.901734, 50 },
{ "COOLPIX S6300", 9.000000, 50 },
{ "COOLPIX S640", 8.928571, 50 },
{ "COOLPIX S6400", 9.000000, 50 },
{ "COOLPIX S70", 8.928571, 50 },
{ "COOLPIX S700", 10.675676, 50 },
{ "COOLPIX S710", 10.703667, 50 },
{ "COOLPIX S8", 8.285714, 50 },
{ "COOLPIX S80", 9.000000, 50 },
{ "COOLPIX S8000", 9.000000, 50 },
{ "COOLPIX S800c", 9.000000, 50 },
{ "COOLPIX S8100", 9.000000, 50 },
{ "COOLPIX S8200", 9.000000, 50 },
{ "COOLPIX S9100", 9.000000, 50 },
{ "COOLPIX S9200", 9.000000, 50 },
{ "COOLPIX S9300", 9.000000, 50 },
{ "SQ", 7.567568, 50 },
{ "NIKON D100", 32.894737, 50 },
{ "NIKON D200", 33.333333, 50 },
{ "NIKON D2H", 32.258065, 50 },
{ "NIKON D2Hs", 33.333333, 50 },
{ "NIKON D2X", 33.333333, 50 },
{ "NIKON D2Xs", 33.333333, 50 },
{ "NIKON D3", 50.000000, 50 },
{ "NIKON D300", 33.333333, 50 },
{ "NIKON D3000", 33.333333, 50 },
{ "NIKON D300S", 33.333333, 50 },
{ "NIKON D3100", 33.333333, 50 },
{ "NIKON D3200", 33.333333, 50 },
{ "NIKON D3S", 50.000000, 50 },
{ "NIKON D3X", 50.000000, 50 },
{ "NIKON D4", 50.000000, 50 },
{ "NIKON D40", 33.333333, 50 },
{ "NIKON D40X", 33.333333, 50 },
{ "NIKON D50", 33.333333, 50 },
{ "NIKON D5000", 33.333333, 50 },
{ "NIKON D5100", 33.333333, 50 },
{ "NIKON D60", 33.333333, 50 },
{ "NIKON D600", 50.000000, 50 },
{ "NIKON D70", 32.894737, 50 },
{ "NIKON D700", 50.000000, 50 },
{ "NIKON D7000", 33.333333, 50 },
{ "NIKON D70s", 33.333333, 50 },
{ "NIKON D80", 33.333333, 50 },
{ "NIKON D800", 50.000000, 50 },
{ "NIKON D800E", 50.000000, 50 },
{ "NIKON D90", 33.333333, 50 },
{ "808 PureView", 15.423077, 50 },
{ "N9", 50.000000, 50 },
{ "C170,D425", 8.324251, 50 },
{ "C180,D435", 8.324251, 50 },
{ "C2020Z", 9.285714, 50 },
{ "C55Z,C5500Z", 10.394737, 50 },
{ "C700UZ", 7.763158, 50 },
{ "C7070WZ", 10.481812, 50 },
{ "C720UZ", 8.000000, 50 },
{ "C730UZ", 7.763158, 50 },
{ "C740UZ", 8.289474, 50 },
{ "C750UZ", 8.289474, 50 },
{ "C-1", 6.428571, 50 },
{ "C160,D395", 7.575758, 50 },
{ "C2000Z", 9.285714, 50 },
{ "C2040Z", 8.875000, 50 },
{ "C2100UZ", 9.210526, 50 },
{ "C2500L", 12.752475, 50 },
{ "C3030Z", 10.156250, 50 },
{ "C3040Z", 10.142857, 50 },
{ "C-5000Z", 10.263158, 50 },
{ "C5050Z", 10.142857, 50 },
{ "C5060WZ", 10.481812, 50 },
{ "C70Z,C7000Z", 10.394737, 50 },
{ "C725UZ", 8.000000, 50 },
{ "C760UZ", 7.500000, 50 },
{ "C765UZ", 8.289474, 50 },
{ "C770UZ", 8.289474, 50 },
{ "C8080WZ", 12.696403, 50 },
{ "C960Z,D460Z", 8.285714, 50 },
{ "C100,D370", 6.428571, 50 },
{ "C120,D380", 6.428571, 50 },
{ "C150,D390", 6.578947, 50 },
{ "C2,D230", 7.638889, 50 },
{ "C200Z,D510Z", 7.714286, 50 },
{ "C21", 9.210526, 50 },
{ "C2Z,D520Z,C220Z", 6.578947, 50 },
{ "C3000Z", 10.156250, 50 },
{ "C300Z,D550Z", 8.077717, 50 },
{ "C3100Z,C3020Z", 10.156250, 50 },
{ "C4040Z", 10.142857, 50 },
{ "C40Z,D40Z", 10.357143, 50 },
{ "C4100Z,C4000Z", 10.156250, 50 },
{ "X100,D540Z,C310Z", 7.631579, 50 },
{ "X-2,C-50Z", 10.263158, 50 },
{ "X-3,C-60Z", 10.263158, 50 },
{ "X350,D575Z,C360Z", 8.285714, 50 },
{ "X400,D580Z,C460Z", 8.285714, 50 },
{ "X500,D590Z,C470Z", 8.285714, 50 },
{ "X200,D560Z,C350Z", 8.285714, 50 },
{ "X300,D565Z,C450Z", 8.285714, 50 },
{ "D595Z,C500Z", 8.289474, 50 },
{ "E-1", 25.000000, 50 },
{ "E-20,E-20N,E-20P", 12.857143, 50 },
{ "E-3", 25.000000, 50 },
{ "E-30", 24.038462, 50 },
{ "E-300", 24.038462, 50 },
{ "E-330", 25.000000, 50 },
{ "E-400", 25.000000, 50 },
{ "E-410", 25.000000, 50 },
{ "E-420", 25.000000, 50 },
{ "E-450", 25.000000, 50 },
{ "E-5", 25.000000, 50 },
{ "E-500", 25.000000, 50 },
{ "E-510", 25.000000, 50 },
{ "E-520", 25.000000, 50 },
{ "E-600", 24.038462, 50 },
{ "E-620", 25.000000, 50 },
{ "E-M5", 25.000000, 50 },
{ "E-P1", 24.038462, 50 },
{ "E-P2", 24.038462, 50 },
{ "E-P3", 25.000000, 50 },
{ "E-P3", 25.000000, 50 },
{ "E-PL1", 24.038462, 50 },
{ "E-PL1s", 25.000000, 50 },
{ "E-PL2", 25.000000, 50 },
{ "E-PL3", 25.000000, 50 },
{ "E-PL5", 25.000000, 50 },
{ "E-PM1", 25.000000, 50 },
{ "E-PM2", 25.000000, 50 },
{ "FE100,X710", 8.182646, 50 },
{ "FE110,X705", 8.182646, 50 },
{ "FE115,X715", 8.182646, 50 },
{ "FE-120,X-700", 8.289474, 50 },
{ "FE-130,X-720", 8.289474, 50 },
{ "FE-140,X-725", 8.289474, 50 },
{ "FE170,X760", 8.289474, 50 },
{ "FE180/X745", 8.289474, 50 },
{ "FE190/X750", 8.289474, 50 },
{ "FE20,X15,C25", 8.750000, 50 },
{ "FE200", 8.214286, 50 },
{ "FE210,X775", 8.289474, 50 },
{ "FE230/X790", 8.289474, 50 },
{ "FE240/X795", 8.421053, 50 },
{ "FE25,X20", 8.750000, 50 },
{ "FE250/X800", 10.571429, 50 },
{ "FE270,X815,C510", 8.289474, 50 },
{ "FE280,X820,C520", 8.750000, 50 },
{ "FE290,X825", 8.169399, 50 },
{ "FE300,X830", 10.571429, 50 },
{ "FE3000,X890", 8.750000, 50 },
{ "FE3010,X895", 8.750000, 50 },
{ "FE310,X840,C530", 8.378378, 50 },
{ "FE320,X835,C540", 8.750000, 50 },
{ "FE330,X845,C550", 8.750000, 50 },
{ "FE340,X855,C560", 8.750000, 50 },
{ "FE35,X30", 8.750000, 50 },
{ "FE350WIDE,X865", 8.125000, 50 },
{ "FE360,X875,C570", 8.750000, 50 },
{ "FE370,X880,C575", 8.750000, 50 },
{ "FE4020,X940", 8.946884, 50 },
{ "FE4030,X950", 8.946884, 50 },
{ "FE45,X40", 8.750000, 50 },
{ "FE46,X41,X42", 8.750000, 50 },
{ "FE5000,X905", 8.750000, 50 },
{ "FE5010,X915", 8.750000, 50 },
{ "FE5020,X935", 8.958333, 50 },
{ "FE5030,X965,X960", 9.038462, 50 },
{ "FE47,X43", 8.750000, 50 },
{ "FerrariMODEL2004", 7.631579, 50 },
{ "IR-300", 8.289474, 50 },
{ "IR-500", 7.736584, 50 },
{ "SH-21", 8.750000, 50 },
{ "SH-25MR", 8.750000, 50 },
{ "SP310", 10.526316, 50 },
{ "SP320", 10.526316, 50 },
{ "SP350", 10.526316, 50 },
{ "SP500UZ", 8.289474, 50 },
{ "SP510UZ", 8.289474, 50 },
{ "SP550UZ", 8.406407, 50 },
{ "SP560UZ", 8.666667, 50 },
{ "SP565UZ", 8.846154, 50 },
{ "SP570UZ", 8.823017, 50 },
{ "SP590UZ", 8.846154, 50 },
{ "SP600UZ", 8.928571, 50 },
{ "SP-610UZ", 8.928571, 50 },
{ "SP-620UZ", 9.000000, 50 },
{ "SP700", 8.289474, 50 },
{ "SP-720UZ", 9.001327, 50 },
{ "SP800UZ", 8.750000, 50 },
{ "SP-810UZ", 8.958333, 50 },
{ "SZ-10", 8.928571, 50 },
{ "SZ-14", 9.000000, 50 },
{ "SZ-20", 8.750000, 50 },
{ "SZ-30MR", 9.000000, 50 },
{ "SZ-31MR", 9.000000, 50 },
{ "TG-1", 9.000000, 50 },
{ "TG-310", 8.925069, 50 },
{ "TG-320", 8.925069, 50 },
{ "TG-610", 8.928571, 50 },
{ "TG-620", 8.928571, 50 },
{ "TG-810", 8.928571, 50 },
{ "TG-820", 8.928571, 50 },
{ "VG110,D700", 8.888889, 50 },
{ "VG120,D705", 9.038462, 50 },
{ "VG130,D710", 9.038462, 50 },
{ "VG140,D715", 9.038462, 50 },
{ "VG170", 9.038462, 50 },
{ "VG150,D735", 8.888889, 50 },
{ "VH210", 9.038462, 50 },
{ "VR310,D720", 9.722222, 50 },
{ "VR320,D725", 8.750000, 50 },
{ "VR340,D750", 8.750000, 50 },
{ "VR350,D755", 8.750000, 50 },
{ "VR360,D760", 8.750000, 50 },
{ "X450,D535Z,C370Z", 7.631579, 50 },
{ "X550,D545Z,C480Z", 8.289474, 50 },
{ "X600,D630,FE5500", 8.285714, 50 },
{ "XZ-1", 10.714286, 50 },
{ "u1030SW,S1030SW", 8.925069, 50 },
{ "u1040,S1040", 8.815789, 50 },
{ "u1060,S1060", 8.901734, 50 },
{ "u1070,S1070", 8.815789, 50 },
{ "u1200,S1200", 10.571429, 50 },
{ "u5000", 8.750000, 50 },
{ "u550WP.S550WP", 8.815789, 50 },
{ "u710,S710", 8.783784, 50 },
{ "u720SW,S720SW", 8.815789, 50 },
{ "u795SW,S795SW", 8.815789, 50 },
{ "u810/S810", 10.571429, 50 },
{ "u820,S820", 8.888889, 50 },
{ "u830,S830", 8.888889, 50 },
{ "uT6000,ST6000", 8.925069, 50 },
{ "uT8000,ST8000", 8.925069, 50 },
{ "u1010,S1010", 8.901734, 50 },
{ "u1020,S1020", 8.901734, 50 },
{ "u725SW,S725SW", 8.815789, 50 },
{ "u840,S840", 8.888889, 50 },
{ "u850SW,S850SW", 8.815789, 50 },
{ "u1050SW,S1050SW", 8.815789, 50 },
{ "u-5010", 9.038462, 50 },
{ "u7000,S7000", 8.901734, 50 },
{ "u7010,S7010", 6.696429, 50 },
{ "u7020,S7020", 8.928571, 50 },
{ "u7030,S7030", 8.928571, 50 },
{ "u-7040", 8.928571, 50 },
{ "u9000,S9000", 8.928571, 50 },
{ "u-9010", 8.928571, 50 },
{ "uT6010,ST6010", 8.925069, 50 },
{ "uTough-3000", 8.925069, 50 },
{ "uTough-6020", 8.928571, 50 },
{ "uTough-8010", 8.928571, 50 },
{ "u-miniD,Stylus V", 8.285714, 50 },
{ "u1000/S1000", 10.523594, 50 },
{ "u10D,S300D,u300D", 8.285714, 50 },
{ "u20D,S400D,u400D", 8.285714, 50 },
{ "u30D,S410D,u410D", 8.285714, 50 },
{ "u40D,S500,uD500", 8.285714, 50 },
{ "uD600,S600", 8.285714, 50 },
{ "u730/S730", 8.815789, 50 },
{ "u740,S740", 8.648649, 50 },
{ "u750,S750", 8.888889, 50 },
{ "u760,S760", 8.783784, 50 },
{ "u770SW,S770SW", 8.815789, 50 },
{ "u790SW,S790SW", 8.815789, 50 },
{ "uD800,S800", 10.526316, 50 },
{ "DMC-FZ1", 6.731707, 50 },
{ "DMC-FZ10", 8.780488, 50 },
{ "DMC-GH1", 25.000000, 50 },
{ "DMC-LC33", 8.285714, 50 },
{ "DMC-LC5", 10.552764, 50 },
{ "DMC-F3", 8.928571, 50 },
{ "DMC-FS10", 8.928571, 50 },
{ "DMC-FH25", 8.928571, 50 },
{ "DMC-FH27", 8.928571, 50 },
{ "DMC-FS11", 8.928571, 50 },
{ "DMC-FH5", 8.928571, 50 },
{ "DMC-FP1", 9.000000, 50 },
{ "DMC-FP3", 9.000000, 50 },
{ "DMC-FP5", 9.000000, 50 },
{ "DMC-FP7", 9.000000, 50 },
{ "DMC-FP8", 8.956386, 50 },
{ "DMC-FS15", 8.965517, 50 },
{ "DMC-FS16", 8.928571, 50 },
{ "DMC-FS18", 8.928571, 50 },
{ "DMC-FS20", 8.666667, 50 },
{ "DMC-FH7", 8.928571, 50 },
{ "DMC-FS25", 8.965517, 50 },
{ "DMC-FS3", 8.291457, 50 },
{ "DMC-FS33", 8.928571, 50 },
{ "DMC-FS35", 8.928571, 50 },
{ "DMC-FS37", 8.928571, 50 },
{ "DMC-FS4", 8.333333, 50 },
{ "DMC-FS42", 8.333333, 50 },
{ "DMC-FH8", 8.958333, 50 },
{ "DMC-FS5", 8.666667, 50 },
{ "DMC-FS6", 8.333333, 50 },
{ "DMC-FS7", 8.333333, 50 },
{ "DMC-FS8", 8.333333, 50 },
{ "DMC-FT1", 8.827434, 50 },
{ "DMC-FT20", 9.000000, 50 },
{ "DMC-FT4", 8.827434, 50 },
{ "DMC-FX01", 8.224777, 50 },
{ "DMC-FX07", 8.224777, 50 },
{ "DMC-FX10", 8.285714, 50 },
{ "DMC-FX12", 8.285714, 50 },
{ "DMC-FX150", 10.707138, 50 },
{ "DMC-FX2", 8.285714, 50 },
{ "DMC-FX3", 8.285714, 50 },
{ "DMC-FX30", 8.207137, 50 },
{ "DMC-FX33", 8.207137, 50 },
{ "DMC-FX35", 8.800000, 50 },
{ "DMC-FX37", 8.800000, 50 },
{ "DMC-FX40", 8.800000, 50 },
{ "DMC-FX48", 8.800000, 50 },
{ "DMC-FX5", 8.285714, 50 },
{ "DMC-FX50", 8.224777, 50 },
{ "DMC-FX500", 8.800000, 50 },
{ "DMC-FX520", 8.800000, 50 },
{ "DMC-FX55", 8.207137, 50 },
{ "DMC-FX550", 8.800000, 50 },
{ "DMC-FX580", 8.800000, 50 },
{ "DMC-FX7", 8.285714, 50 },
{ "DMC-FX70", 8.958333, 50 },
{ "DMC-FX700", 8.958333, 50 },
{ "DMC-FX77", 8.958333, 50 },
{ "DMC-FX8", 8.285714, 50 },
{ "DMC-FX80", 8.958333, 50 },
{ "DMC-FX9", 8.285714, 50 },
{ "DMC-FX90", 8.958333, 50 },
{ "DMC-FZ100", 9.000000, 50 },
{ "DMC-FZ15", 8.333333, 50 },
{ "DMC-FZ150", 9.000000, 50 },
{ "DMC-FZ18", 8.247012, 50 },
{ "DMC-FZ20", 8.333333, 50 },
{ "DMC-FZ200", 9.000000, 50 },
{ "DMC-FZ28", 8.888889, 50 },
{ "DMC-FZ3", 6.571429, 50 },
{ "DMC-FZ30", 10.571429, 50 },
{ "DMC-FZ35", 8.888889, 50 },
{ "DMC-FZ38", 8.888889, 50 },
{ "DMC-FZ4", 8.571429, 50 },
{ "DMC-FZ40", 9.000000, 50 },
{ "DMC-FZ45", 9.000000, 50 },
{ "DMC-FZ47", 9.000000, 50 },
{ "DMC-FZ48", 9.000000, 50 },
{ "DMC-FZ5", 8.333333, 50 },
{ "DMC-FZ50", 10.571429, 50 },
{ "DMC-FZ62", 9.000000, 50 },
{ "DMC-FZ7", 8.470588, 50 },
{ "DMC-FZ8", 8.333333, 50 },
{ "DMC-G1", 25.000000, 50 },
{ "DMC-G10", 25.000000, 50 },
{ "DMC-G2", 24.038462, 50 },
{ "DMC-G3", 25.000000, 50 },
{ "DMC-G5", 25.000000, 50 },
{ "DMC-GF1", 24.038462, 50 },
{ "DMC-GF2", 25.000000, 50 },
{ "DMC-GF3", 25.000000, 50 },
{ "DMC-GF5", 25.000000, 50 },
{ "DMC-GH2", 25.000000, 50 },
{ "DMC-GH3", 25.000000, 50 },
{ "DMC-GX1", 25.000000, 50 },
{ "DMC-L1", 25.000000, 50 },
{ "DMC-L10", 25.000000, 50 },
{ "DMC-LC1", 12.500000, 50 },
{ "DMC-LC40", 10.552764, 50 },
{ "DMC-LC43", 8.285714, 50 },
{ "DMC-LC50", 8.285714, 50 },
{ "DMC-LC70", 8.285714, 50 },
{ "DMC-LS1", 8.285714, 50 },
{ "DMC-LS2", 8.285714, 50 },
{ "DMC-LS60", 8.285714, 50 },
{ "DMC-LS70", 8.285714, 50 },
{ "DMC-LS75", 8.285714, 50 },
{ "DMC-LS80", 8.291457, 50 },
{ "DMC-LS85", 8.333333, 50 },
{ "DMC-LX1", 11.250000, 50 },
{ "DMC-LX2", 11.250000, 50 },
{ "DMC-LX3", 10.645793, 50 },
{ "DMC-LX5", 10.645793, 50 },
{ "DMC-LX7", 9.812456, 50 },
{ "DMC-LZ1", 8.243243, 50 },
{ "DMC-LZ10", 8.666667, 50 },
{ "DMC-LZ2", 8.243243, 50 },
{ "DMC-LZ3", 8.243243, 50 },
{ "DMC-LZ5", 8.243243, 50 },
{ "DMC-LZ6", 8.243243, 50 },
{ "DMC-LZ7", 8.243243, 50 },
{ "DMC-LZ8", 8.125000, 50 },
{ "DMC-S3", 8.928571, 50 },
{ "DMC-SZ1", 9.000000, 50 },
{ "DMC-SZ7", 9.000000, 50 },
{ "DMC-TS1", 8.827434, 50 },
{ "DMC-TS10", 9.000000, 50 },
{ "DMC-FT3", 8.827434, 50 },
{ "DMC-TZ1", 7.428571, 50 },
{ "DMC-TZ10", 8.200000, 50 },
{ "DMC-TZ2", 8.214286, 50 },
{ "DMC-TZ25", 8.958333, 50 },
{ "DMC-TZ3", 8.214286, 50 },
{ "DMC-TZ31", 8.958333, 50 },
{ "DMC-TZ4", 8.214286, 50 },
{ "DMC-TZ5", 8.214286, 50 },
{ "DMC-TZ50", 8.392857, 50 },
{ "DMC-TZ6", 8.200000, 50 },
{ "DMC-TZ7", 8.200000, 50 },
{ "DMC-TZ8", 8.200000, 50 },
{ "DMC-ZR1", 9.000000, 50 },
{ "DMC-ZS1", 8.200000, 50 },
{ "DMC-ZS10", 8.958333, 50 },
{ "DMC-TZ20", 8.958333, 50 },
{ "DMC-ZS20", 8.958333, 50 },
{ "DMC-TZ18", 8.958333, 50 },
{ "DMC-ZX3", 9.000000, 50 },
{ "PENTAX *ist D", 32.679739, 50 },
{ "PENTAX *ist DL", 33.333333, 50 },
{ "PENTAX *ist DL2", 32.679739, 50 },
{ "PENTAX *ist DS", 32.679739, 50 },
{ "PENTAX *ist DS", 32.679739, 50 },
{ "PENTAX *ist DS2", 32.679739, 50 },
{ "PENTAX EI-200 (V01.00)", 7.642972, 50 },
{ "PENTAX K-01", 33.333333, 50 },
{ "PENTAX K-30", 33.333333, 50 },
{ "PENTAX K-5", 33.333333, 50 },
{ "PENTAX K-7", 32.258065, 50 },
{ "PENTAX K-m", 32.258065, 50 },
{ "PENTAX K100D", 33.333333, 50 },
{ "PENTAX K100D Super", 33.333333, 50 },
{ "PENTAX K10D", 33.333333, 50 },
{ "PENTAX K10D", 33.333333, 50 },
{ "PENTAX K110D", 33.333333, 50 },
{ "PENTAX K200D", 33.333333, 50 },
{ "PENTAX K20D", 33.333333, 50 },
{ "PENTAX Optio 230", 7.631579, 50 },
{ "PENTAX Optio 30", 7.631579, 50 },
{ "PENTAX Optio 330", 10.133333, 50 },
{ "PENTAX Optio 330GS", 7.631579, 50 },
{ "PENTAX Optio330RS", 10.201342, 50 },
{ "PENTAX Optio 33L", 7.631579, 50 },
{ "PENTAX Optio 33LF", 7.631579, 50 },
{ "PENTAX Optio 33WR", 7.697502, 50 },
{ "PENTAX Optio 430", 10.224215, 50 },
{ "PENTAX Optio430RS", 10.133333, 50 },
{ "PENTAX Optio 43WR", 7.697502, 50 },
{ "PENTAX Optio 450", 10.400000, 50 },
{ "PENTAX Optio 50", 8.437500, 50 },
{ "PENTAX Optio 550", 10.400000, 50 },
{ "PENTAX Optio 555", 10.400000, 50 },
{ "PENTAX Optio 60", 8.720930, 50 },
{ "PENTAX Optio 750Z", 10.400000, 50 },
{ "PENTAX Optio A20", 10.533333, 50 },
{ "PENTAX Optio A30", 10.394737, 50 },
{ "PENTAX Optio A40", 10.675676, 50 },
{ "PENTAX Optio E10", 7.236842, 50 },
{ "PENTAX OptioE20", 8.088235, 50 },
{ "PENTAX Optio E30", 8.571429, 50 },
{ "PENTAX Optio E40", 8.266667, 50 },
{ "PENTAX Optio E50", 8.266667, 50 },
{ "PENTAX Optio E60", 8.906250, 50 },
{ "PENTAX Optio E70", 9.000000, 50 },
{ "Optio E70L", 8.906250, 50 },
{ "PENTAX Optio E80", 8.952880, 50 },
{ "Optio E85", 8.906250, 50 },
{ "PENTAX Optio H90", 9.107143, 50 },
{ "PENTAX Optio I-10", 9.107143, 50 },
{ "PENTAX Optio L40", 8.750000, 50 },
{ "PENTAX Optio LS465", 8.928571, 50 },
{ "PENTAX Optio M10", 8.285714, 50 },
{ "PENTAX Optio M20", 8.289474, 50 },
{ "PENTAX Optio M30", 8.289474, 50 },
{ "PENTAX Optio M40", 8.750000, 50 },
{ "PENTAX Optio M50", 8.750000, 50 },
{ "PENTAX Optio M60", 8.750000, 50 },
{ "PENTAX Optio MX", 7.631579, 50 },
{ "PENTAX Optio MX4", 7.837838, 50 },
{ "PENTAX Optio P70", 8.990826, 50 },
{ "PENTAX Optio P80", 8.828829, 50 },
{ "PENTAX Optio RS1000", 8.909091, 50 },
{ "PENTAX Optio RS1500", 8.909091, 50 },
{ "PENTAX Optio RZ10", 8.928571, 50 },
{ "PENTAX Optio RZ18", 9.000000, 50 },
{ "PENTAX Optio S", 8.285714, 50 },
{ "PENTAX Optio S1", 8.928571, 50 },
{ "PENTAX Optio S10", 10.394737, 50 },
{ "PENTAX Optio S12", 10.394737, 50 },
{ "PENTAX Optio S30", 7.631579, 50 },
{ "PENTAX Optio S4", 8.285714, 50 },
{ "PENTAX Optio S40", 8.285714, 50 },
{ "PENTAX Optio S45", 8.285714, 50 },
{ "PENTAX Optio S4i", 8.285714, 50 },
{ "PENTAX Optio S50", 8.138447, 50 },
{ "PENTAX Optio S55", 8.138447, 50 },
{ "PENTAX Optio S5i", 8.138447, 50 },
{ "PENTAX Optio S5n", 8.138447, 50 },
{ "PENTAX Optio S5z", 8.138447, 50 },
{ "PENTAX Optio S6", 8.157895, 50 },
{ "PENTAX Optio S60", 8.246445, 50 },
{ "PENTAX Optio S7", 8.266667, 50 },
{ "PENTAX Optio SV", 8.055556, 50 },
{ "PENTAX Optio SVi", 8.055556, 50 },
{ "PENTAX Optio T10", 8.193833, 50 },
{ "PENTAX Optio T30", 8.266667, 50 },
{ "PENTAX Optio V10", 8.750000, 50 },
{ "PENTAX Optio V20", 8.750000, 50 },
{ "PENTAX Optio VS20", 8.928571, 50 },
{ "PENTAX Optio W10", 8.289474, 50 },
{ "PENTAX Optio W20", 8.289474, 50 },
{ "PENTAX Optio W30", 8.289474, 50 },
{ "PENTAX Optio W60", 8.928571, 50 },
{ "PENTAX Optio W80", 8.928571, 50 },
{ "PENTAX Optio W90", 8.928571, 50 },
{ "PENTAX Optio WG-1", 8.928571, 50 },
{ "PENTAX Optio WG-2", 8.928571, 50 },
{ "PENTAX Optio WP", 8.289474, 50 },
{ "PENTAX Optio WPi", 8.289474, 50 },
{ "PENTAX Optio WS80", 8.857143, 50 },
{ "PENTAX Optio X", 8.138447, 50 },
{ "PENTAX Optio Z10", 8.289474, 50 },
{ "PENTAX K-x", 33.333333, 50 },
{ "PENTAX X90", 8.846154, 50 },
{ "PENTAX X70", 8.846154, 50 },
{ "PENTAX Q", 9.041591, 50 },
{ "PENTAX K-r", 33.333333, 50 },
{ "PENTAX 645D", 64.102564, 50 },
{ "PDC2070", 7.812500, 50 },
{ "Polaroid PDC4350", 10.405405, 50 },
{ "PDC2150", 7.560976, 50 },
{ "Polaroid PDC2350", 6.714286, 50 },
{ "Polaroid PDC3350", 7.623697, 50 },
{ "CX1", 8.844640, 50 },
{ "CX2", 8.750000, 50 },
{ "CX3", 8.750000, 50 },
{ "CX4", 8.750000, 50 },
{ "CX5", 8.750000, 50 },
{ "CX6", 8.750000, 50 },
{ "Caplio 300G", 7.857143, 50 },
{ "Caplio400Gwide", 7.485265, 50 },
{ "Caplio500Gwide", 10.295858, 50 },
{ "Caplio 500SE", 10.295858, 50 },
{ "CaplioG3 modelM", 7.857143, 50 },
{ "CaplioG3 modelS", 7.857143, 50 },
{ "Caplio G4", 7.857143, 50 },
{ "Caplio G4 wide", 7.485265, 50 },
{ "Caplio GX", 10.295858, 50 },
{ "Caplio GX100", 10.625000, 50 },
{ "Caplio GX8", 10.295858, 50 },
{ "Caplio R1", 8.218252, 50 },
{ "Caplio R1V", 8.218252, 50 },
{ "Caplio R2", 8.218252, 50 },
{ "Caplio R3", 8.232104, 50 },
{ "Caplio R4", 8.232104, 50 },
{ "Caplio R40", 8.232104, 50 },
{ "Caplio R5", 8.232104, 50 },
{ "Caplio R6", 8.232104, 50 },
{ "Caplio R7", 8.232104, 50 },
{ "Caplio RR1", 10.428571, 50 },
{ "Caplio RR120", 8.157895, 50 },
{ "Caplio RR211", 9.795918, 50 },
{ "Caplio RR30", 7.857143, 50 },
{ "Caplio RR530", 8.437500, 50 },
{ "Caplio RX", 7.664259, 50 },
{ "G600", 8.928571, 50 },
{ "G700", 8.928571, 50 },
{ "GR DIGITAL 4", 10.714286, 50 },
{ "GR DIGITAL 3", 10.714286, 50 },
{ "GR Digital", 10.535714, 50 },
{ "GR DIGITAL 2", 10.535714, 50 },
{ "RICOH GX200", 10.625000, 50 },
{ "GXR P10", 8.750000, 50 },
{ "PX", 8.928571, 50 },
{ "RICOH R10", 8.844640, 50 },
{ "RICOH R50", 8.750000, 50 },
{ "RICOH R8", 8.844640, 50 },
{ "RDC-5000", 10.495627, 50 },
{ "RDC-5300", 10.526316, 50 },
{ "RDC-6000", 10.375000, 50 },
{ "RDC-7", 10.209790, 50 },
{ "RDC-i500", 10.428571, 50 },
{ "RDC-i700", 10.428571, 50 },
{ "DV300 / DV300F / DV305F", 9.000000, 50 },
{ "Digimax101", 9.782609, 50 },
{ "DIGIMAX 130", 6.470588, 50 },
{ "Digimax 200", 7.906977, 50 },
{ "Digimax 201", 6.511628, 50 },
{ "Digimax 202", 14.228571, 50 },
{ "Digimax 220SE", 7.598253, 50 },
{ "DIGIMAX-230", 7.647059, 50 },
{ "Digimax 240", 6.666667, 50 },
{ "Digimax 301", 7.727273, 50 },
{ "DIGIMAX-340", 10.294118, 50 },
{ "DIGIMAX 350SE", 10.294118, 50 },
{ "Digimax 360", 10.131579, 50 },
{ "DIGIMAX-410", 10.294118, 50 },
{ "Digimax 430 / Kenox D430", 7.631579, 50 },
{ "Digimax A40 / KENOX Q1", 8.285714, 50 },
{ "Digimax A400", 8.371876, 50 },
{ "Digimax A402", 8.285714, 50 },
{ "Digimax A5", 10.131579, 50 },
{ "Digimax A50 / KENOX Q2", 8.285714, 50 },
{ "Digimax A502", 8.285714, 50 },
{ "Digimax A55W / Kenox Q25 WID", 8.218252, 50 },
{ "Digimax A6", 10.131579, 50 },
{ "Digimax L50 / KENOX X1", 8.287293, 50 },
{ "Digimax L55W / Kenox X15 WIDE", 8.218252, 50 },
{ "Digimax L60", 8.285714, 50 },
{ "Digimax L85", 10.263158, 50 },
{ "<Digimax S500 / Kenox S500 / Digimax Cyber 530>", 8.285714, 50 },
{ "<Digimax S600 / Kenox S600 / Digimax Cyber 630>", 8.285714, 50 },
{ "<Digimax S800 / Kenox S800>", 10.571429, 50 },
{ "U-CA 3 Digital Camera", 7.631579, 50 },
{ "< Digimax U-CA 5, Kenox U-CA 5 / Kenox U-CA 50 >", 8.285714, 50 },
{ "Digimax V3", 10.131579, 50 },
{ "Digimax V4", 10.131579, 50 },
{ "<Digimax V700 / Kenox V10 / Digimax V10 >", 10.263158, 50 },
{ "<Digimax V800 / Kenox V20 / Digimax V20 >", 10.263158, 50 },
{ "<Digimax i5, Samsung #1>", 8.461538, 50 },
{ "<Digimax i50 MP3, Samsung #1 MP3>", 8.461538, 50 },
{ "<Digimax i6 PMP, Samsung #11 PMP>", 8.461538, 50 },
{ "SAMSUNG ES15 / VLUU ES15 / SAMSUNG SL30", 8.750000, 50 },
{ "SAMSUNG ES20/ VLUU ES20/ SAMSUNG SL35", 8.823529, 50 },
{ "SAMSUNG ES30/VLUU ES30", 9.074074, 50 },
{ "SAMSUNG ES55,ES57 / VLUU ES55 / SAMSUNG SL102", 8.750000, 50 },
{ "SAMSUNG ES60 / VLUU ES60 / SAMSUNG SL105", 9.000000, 50 },
{ "SAMSUNG ES70 / VLUU ES70", 9.074074, 50 },
{ "SAMSUNG ES80/SAMSUNG ES81", 9.074074, 50 },
{ "EX1", 10.833333, 50 },
{ "Samsung GX-1L", 33.333333, 50 },
{ "GX-1S", 33.333333, 50 },
{ "SAMSUNG GX20", 32.258065, 50 },
{ "SAMSUNG GX10", 33.333333, 50 },
{ "SAMSUNG GX20", 32.679739, 50 },
{ "GT-I9300", 6.607143, 50 },
{ "GT-I9100", 6.616667, 50 },
{ "SAMSUNG IT100 / VLUU IT100 / SAMSUNG SL820", 8.928571, 50 },
{ "VLUU L110  / Samsung L110", 8.378378, 50 },
{ "VLUU L110, M110  / Samsung L110, M110", 8.378378, 50 },
{ "VLUU L200  / Samsung L200", 8.378378, 50 },
{ "VLUU L210  / Samsung L210", 9.117647, 50 },
{ "VLUU L310W L313 M310W / Samsung L310W L313 M310W", 10.650888, 50 },
{ "Samsung L700", 8.285714, 50 },
{ "<VLUU L730  / Samsung L730>", 8.289474, 50 },
{ "<Samsung L74, Samsung VLUU L74>", 8.404334, 50 },
{ "VLUU L77 / L77 Digital Camera", 8.289474, 50 },
{ "KENOX X77 / L77 Digital Camera", 8.289474, 50 },
{ "<VLUU L830  / Samsung L830>", 8.289474, 50 },
{ "VLUU L83T/ Samsung L83T", 8.289474, 50 },
{ "SAMSUNG MV800", 9.400000, 50 },
{ "VLUU NV 7, NV 7", 8.283622, 50 },
{ "VLUU NV10, NV10", 10.571429, 50 },
{ "NV100HD, VLUU NV100HD, LANDIAO NV100HD, TL34HD", 10.650888, 50 },
{ "NV11, VLUU NV11", 10.263158, 50 },
{ "NV15, VLUU NV15", 10.735294, 50 },
{ "NV20, VLUU NV20", 10.735294, 50 },
{ "NV24HD, VLUU NV24HD, LANDIAO NV106HD", 8.958935, 50 },
{ "<Samsung NV3, Samsung VLUU NV3>", 8.289474, 50 },
{ "NV30, VLUU NV30, LANDIAO NV30", 8.378378, 50 },
{ "<Samsung NV4, Samsung VLUU NV4>", 8.289474, 50 },
{ "NV40, VLUU NV40, LANDIAO NV103, VLUU NV404", 9.117647, 50 },
{ "NV8 , VLUU NV8", 10.735294, 50 },
{ "<Samsung NV9, Samsung VLUU NV9>", 8.947368, 50 },
{ "NX10", 33.333333, 50 },
{ "NX100", 33.333333, 50 },
{ "NX1000", 33.333333, 50 },
{ "NX11", 33.333333, 50 },
{ "NX11", 32.679739, 50 },
{ "NX20", 33.333333, 50 },
{ "NX200", 32.679739, 50 },
{ "NX210", 33.333333, 50 },
{ "NX5", 33.333333, 50 },
{ "SAMSUNG PL120,PL121 / VLUU PL120,PL121", 9.038462, 50 },
{ "SAMSUNG PL150 / VLUU PL150 / SAMSUNG TL210 / SAMSUNG PL151", 9.074074, 50 },
{ "SAMSUNG PL170,PL171 / VLUUPL170,PL171", 9.038462, 50 },
{ "SAMSUNG PL20 / VLUU PL20", 9.074074, 50 },
{ "SAMSUNG PL200/VLUU PL200", 9.032258, 50 },
{ "SAMSUNG PL210, PL211 / VLUU PL210, PL211", 8.981481, 50 },
{ "SAMSUNG PL50 / VLUU PL50 / SAMSUNG SL202", 8.157895, 50 },
{ "SAMSUNG PL51 / VLUU PL51 / SAMSUNG SL203", 8.857143, 50 },
{ "SAMSUNG SL502", 9.000000, 50 },
{ "SAMSUNG PL60 / VLUU PL60 / SAMSUNG SL420", 9.000000, 50 },
{ "SAMSUNG SL620", 9.000000, 50 },
{ "SAMSUNG PL70 / VLUU PL70 / SAMSUNG SL720", 8.928571, 50 },
{ "SAMSUNG PL80/VLUU PL80/SAMSUNG SL630/SAMSUNG PL81", 9.107143, 50 },
{ "Pro 815", 12.857143, 50 },
{ "<Digimax S1000 / Kenox S1000>", 10.571429, 50 },
{ "<KENOX S1050  / Samsung S1050>", 10.263158, 50 },
{ "Samsung S1060", 8.289474, 50 },
{ "Samsung S1070 / KENOX S1070", 9.000000, 50 },
{ "<KENOX S760  / Samsung S760>", 8.289474, 50 },
{ "Samsung S85", 8.289474, 50 },
{ "Samsung S850", 10.263158, 50 },
{ "<KENOX S860  / Samsung S860>", 8.289474, 50 },
{ "VLUU SH100, SAMSUNG SH100", 9.038462, 50 },
{ "SAMSUNG ST1000 / VLUU ST1000 / SAMSUNG CL65", 9.000000, 50 },
{ "SAMSUNG ST1000 / SAMSUNG ST1100 / VLUU ST1000 / SAMSUNG CL65", 9.000000, 50 },
{ "SAMSUNG ST30", 7.321429, 50 },
{ "SAMSUNG ST45/ VLUU ST45/ SAMSUNG TL90", 9.000000, 50 },
{ "VLUU ST50/ SAMSUNG ST50", 9.000000, 50 },
{ "SAMSUNG ST500 / VLUU ST500 / SAMSUNG TL220", 9.066015, 50 },
{ "VLUU ST5000, ST5000, TL240", 9.032258, 50 },
{ "SAMSUNG ST550 / VLUU ST550 / SAMSUNG TL225", 9.066015, 50 },
{ "VLUU ST5500, ST5500, CL80", 9.032258, 50 },
{ "SAMSUNG ST60", 8.909091, 50 },
{ "VLUU ST600, ST600", 9.074074, 50 },
{ "SAMSUNG ST65 / VLUU ST65 / SAMSUNG ST67", 9.074074, 50 },
{ "SAMSUNG ST70 / VLUU ST70 / SAMSUNG ST71", 9.074074, 50 },
{ "ST77 / ST79", 9.000000, 50 },
{ "VLUU ST80, ST80", 9.000000, 50 },
{ "SAMSUNG ST90,ST91 / VLUU ST90,ST91", 9.038462, 50 },
{ "SAMSUNG ST93", 9.038462, 50 },
{ "SAMSUNG ST96", 9.038462, 50 },
{ "Samsung S1060", 8.289474, 50 },
{ "SAMSUNG TL100", 9.000000, 50 },
{ "VLUU ST95, SAMSUNG ST95", 9.038462, 50 },
{ "WB100/ WB101", 8.967063, 50 },
{ "SAMSUNG WB1000 / VLUU WB1000 / SAMSUNG TL320", 8.958333, 50 },
{ "WB150 / WB150F / WB152 / WB152F / WB151", 8.333333, 50 },
{ "WB2000", 8.958333, 50 },
{ "VLUU WB210, SAMSUNG WB210", 8.333333, 50 },
{ "SAMSUNG WB500, WB510 / VLUU WB500 / SAMSUNG HZ10W", 8.750000, 50 },
{ "SAMSUNG WB500 / VLUU WB500 / SAMSUNG HZ10W", 8.750000, 50 },
{ "SAMSUNG WB550, WB560 / VLUU WB550 / SAMSUNG HZ15W", 8.750000, 50 },
{ "SAMSUNG WB5500 / VLUU WB5500 / SAMSUNG HZ50W", 8.846154, 50 },
{ "SAMSUNG WB600 / VLUU WB600 / SAMSUNG WB610", 8.125000, 50 },
{ "SAMSUNG WB650 / VLUU WB650 / SAMSUNG WB660", 8.125000, 50 },
{ "WB690", 8.458333, 50 },
{ "WB700", 8.333333, 50 },
{ "SAMSUNG WB750", 8.333333, 50 },
{ "SAMSUNG WB850F/WB855F", 8.913043, 50 },
{ "SAMSUNG WP10", 8.750000, 50 },
{ "<Samsung i7, Samsung VLUU i7>", 8.289474, 50 },
{ "SAMSUNG i70", 8.289474, 50 },
{ "<Samsung i8, Samsung VLUU i8>", 8.289474, 50 },
{ "<Samsung i85, Samsung VLUU i85>", 8.333333, 50 },
{ "AZ3", 10.405405, 50 },
{ "VPC-MZ3GX", 10.405405, 50 },
{ "A5", 8.192743, 50 },
{ "E6", 8.289474, 50 },
{ "FH1", 8.333333, 50 },
{ "J1", 7.697502, 50 },
{ "J2", 7.697502, 50 },
{ "J4", 7.697502, 50 },
{ "S3", 7.802691, 50 },
{ "S4", 8.331789, 50 },
{ "S5", 8.331789, 50 },
{ "S6", 8.285714, 50 },
{ "HD1", 8.289474, 50 },
{ "HD1A", 8.289474, 50 },
{ "C4", 7.656616, 50 },
{ "C5", 8.315706, 50 },
{ "C6", 8.315706, 50 },
{ "HD1000", 8.289474, 50 },
{ "HD2", 8.289474, 50 },
{ "SIGMA DP1", 29.411765, 50 },
{ "SIGMA DP1S", 29.411765, 50 },
{ "SIGMA DP1X", 29.642857, 50 },
{ "SIGMA DP2", 29.512195, 50 },
{ "SIGMA DP2S", 29.512195, 50 },
{ "SIGMA DP2X", 29.512195, 50 },
{ "SIGMA SD1", 33.333333, 50 },
{ "SIGMA SD10", 29.411765, 50 },
{ "SIGMA SD14", 29.411765, 50 },
{ "SIGMA SD15", 29.411765, 50 },
{ "SIGMA SD9", 29.411765, 50 },
{ "DSC-D700", 9.285714, 50 },
{ "DSC-F828", 12.714185, 50 },
{ "DSC-F88", 8.815789, 50 },
{ "DSC-G1", 8.331140, 50 },
{ "DSC-H1", 8.333333, 50 },
{ "DSC-H10", 8.289474, 50 },
{ "DSC-H2", 8.333333, 50 },
{ "DSC-H20", 8.289474, 50 },
{ "DSC-H3", 8.289474, 50 },
{ "DSC-H5", 8.333333, 50 },
{ "DSC-H50", 8.387097, 50 },
{ "DSC-H55", 8.500000, 50 },
{ "DSC-H7", 8.387097, 50 },
{ "DSC-H70", 8.549708, 50 },
{ "DSC-H9", 8.387097, 50 },
{ "DSC-H90", 8.230769, 50 },
{ "DSC-HX1", 8.928571, 50 },
{ "DSC-HX100V", 8.888889, 50 },
{ "DSC-HX10V", 8.916667, 50 },
{ "DSC-HX200V", 8.888889, 50 },
{ "DSC-HX20V", 8.900000, 50 },
{ "DSC-HX5V", 8.500000, 50 },
{ "DSC-HX7V", 8.549708, 50 },
{ "DSC-HX9V", 8.916667, 50 },
{ "DSC-J10", 8.839250, 50 },
{ "DSC-L1", 7.968750, 50 },
{ "DSC-M1", 9.008836, 50 },
{ "DSC-M2", 8.331140, 50 },
{ "DSC-N1", 10.394737, 50 },
{ "DSC-P12", 10.394737, 50 },
{ "DSC-P10", 10.394737, 50 },
{ "DSC-P100", 10.394737, 50 },
{ "DSC-P120", 10.394737, 50 },
{ "DSC-P150", 10.438412, 50 },
{ "DSC-P200", 10.394737, 50 },
{ "DSC-P32", 7.575758, 50 },
{ "DSC-P41", 7.575758, 50 },
{ "DSC-P43", 7.575758, 50 },
{ "DSC-P52", 7.682927, 50 },
{ "DSC-P72", 7.692308, 50 },
{ "DSC-P73", 7.692308, 50 },
{ "DSC-P8", 7.692308, 50 },
{ "DSC-P92", 10.256410, 50 },
{ "DSC-P93", 10.394737, 50 },
{ "DSC-R1", 29.791667, 50 },
{ "DSC-RX1", 50.000000, 50 },
{ "DSC-RX100", 18.560708, 50 },
{ "DSC-S2000", 8.857143, 50 },
{ "DSC-S2100", 8.857143, 50 },
{ "DSC-S40", 7.968750, 50 },
{ "DSC-S600", 8.225806, 50 },
{ "DSC-S650", 8.285714, 50 },
{ "DSC-S700", 8.285714, 50 },
{ "DSC-S730", 8.285714, 50 },
{ "DSC-S750", 8.285714, 50 },
{ "DSC-S80", 7.692308, 50 },
{ "DSC-S800", 10.540541, 50 },
{ "DSC-S90", 7.692308, 50 },
{ "DSC-S930", 8.888889, 50 },
{ "DSC-S950", 8.787879, 50 },
{ "DSC-T1", 8.815789, 50 },
{ "DSC-T100", 8.285714, 50 },
{ "DSC-T2", 8.331140, 50 },
{ "DSC-T20", 8.331140, 50 },
{ "DSC-T200", 8.285714, 50 },
{ "DSC-T3", 8.815789, 50 },
{ "DSC-T30", 8.331140, 50 },
{ "DSC-T300", 8.787879, 50 },
{ "DSC-T33", 8.815789, 50 },
{ "DSC-T5", 8.311346, 50 },
{ "DSC-T500", 8.787879, 50 },
{ "DSC-T7", 8.311346, 50 },
{ "DSC-T70", 8.331140, 50 },
{ "DSC-T700", 8.824999, 50 },
{ "DSC-T77", 8.824999, 50 },
{ "DSC-T9", 8.311346, 50 },
{ "DSC-T90", 8.824999, 50 },
{ "DSC-T900", 8.824999, 50 },
{ "DSC-T99", 8.854997, 50 },
{ "DSC-TX1", 8.824999, 50 },
{ "DSC-TX10", 8.824929, 50 },
{ "DSC-TX100V", 8.824929, 50 },
{ "DSC-TX20", 8.854997, 50 },
{ "DSC-TX5", 8.854997, 50 },
{ "DSC-TX55", 9.038462, 50 },
{ "DSC-TX7", 8.854997, 50 },
{ "DSC-TX9", 8.854997, 50 },
{ "CYBERSHOT U", 7.575758, 50 },
{ "DSC-U30", 7.575758, 50 },
{ "DSC-U50", 7.575758, 50 },
{ "DSC-U60", 7.575758, 50 },
{ "DSC-V1", 10.294118, 50 },
{ "DSC-V3", 10.294118, 50 },
{ "DSC-W1", 10.394737, 50 },
{ "DSC-W100", 10.394737, 50 },
{ "DSC-W12", 10.394737, 50 },
{ "DSC-W120", 8.359375, 50 },
{ "DSC-W130", 8.359375, 50 },
{ "DSC-W150", 8.333333, 50 },
{ "DSC-W170", 8.928571, 50 },
{ "DSC-W180", 8.857143, 50 },
{ "DSC-W200", 10.857143, 50 },
{ "DSC-W210", 9.006734, 50 },
{ "DSC-W215", 9.006734, 50 },
{ "DSC-W220", 9.006734, 50 },
{ "DSC-W230", 8.916667, 50 },
{ "DSC-W270", 8.928571, 50 },
{ "DSC-W275", 8.928571, 50 },
{ "DSC-W290", 8.928571, 50 },
{ "DSC-W30", 8.289474, 50 },
{ "DSC-W300", 10.857143, 50 },
{ "DSC-W310", 8.928571, 50 },
{ "DSC-W320", 8.995215, 50 },
{ "DSC-W350", 8.995215, 50 },
{ "DSC-W370", 8.823529, 50 },
{ "DSC-W380", 8.864571, 50 },
{ "DSC-W5", 10.394737, 50 },
{ "DSC-W50", 8.289474, 50 },
{ "DSC-W510", 8.995215, 50 },
{ "DSC-W530", 8.995215, 50 },
{ "DSC-W560", 8.318584, 50 },
{ "DSC-W570", 9.000000, 50 },
{ "DSC-W610", 8.995215, 50 },
{ "DSC-W630", 9.000000, 50 },
{ "DSC-W650", 9.000000, 50 },
{ "DSC-W690", 8.900000, 50 },
{ "DSC-W7", 10.394737, 50 },
{ "DSC-W70", 8.289474, 50 },
{ "DSC-W80", 8.285714, 50 },
{ "DSC-W85", 8.285714, 50 },
{ "DSC-W90", 8.285714, 50 },
{ "DSC-WX1", 8.854167, 50 },
{ "DSC-WX10", 8.922615, 50 },
{ "DSC-WX100", 8.900000, 50 },
{ "DSC-WX150", 8.900000, 50 },
{ "DSC-WX30", 9.000000, 50 },
{ "DSC-WX5", 8.854167, 50 },
{ "DSC-WX7", 9.000000, 50 },
{ "DSC-WX9", 10.500000, 50 },
{ "DSC-N2", 10.394737, 50 },
{ "DSC-S500", 8.437500, 50 },
{ "DSC-T10", 8.331140, 50 },
{ "DSC-T50", 8.331140, 50 },
{ "DSC-W35", 8.289474, 50 },
{ "DSC-W55", 8.289474, 50 },
{ "DSLR-A100", 33.333333, 50 },
{ "DSLR-A200", 33.333333, 50 },
{ "DSLR-A230", 33.333333, 50 },
{ "DSLR-A290", 33.333333, 50 },
{ "DSLR-A300", 33.333333, 50 },
{ "DSLR-A330", 33.333333, 50 },
{ "DSLR-A350", 33.333333, 50 },
{ "DSLR-A380", 33.333333, 50 },
{ "DSLR-A390", 33.333333, 50 },
{ "DSLR-A450", 33.333333, 50 },
{ "DSLR-A500", 33.333333, 50 },
{ "DSLR-A550", 33.333333, 50 },
{ "DSLR-A580", 33.333333, 50 },
{ "DSLR-A700", 33.333333, 50 },
{ "DSLR-A850", 50.000000, 50 },
{ "DSLR-A900", 50.000000, 50 },
{ "MVC-CD500", 10.294118, 50 },
{ "NEX-5N", 33.333333, 50 },
{ "NEX-6", 33.333333, 50 },
{ "NEX-7", 33.333333, 50 },
{ "NEX-C3", 33.333333, 50 },
{ "NEX-F3", 33.333333, 50 },
{ "SLT-A33", 33.333333, 50 },
{ "SLT-A35", 33.333333, 50 },
{ "SLT-A37", 33.333333, 50 },
{ "SLT-A55V", 33.333333, 50 },
{ "SLT-A57", 33.333333, 50 },
{ "SLT-A65V", 33.333333, 50 },
{ "SLT-A77V", 33.333333, 50 },
{ "NEX-3", 33.333333, 50 },
{ "NEX-5", 33.333333, 50 },
{ "PDR2300", 7.500000, 50 },
{ "PDR-3300", 10.252525, 50 },
{ "PDR-3310", 10.428571, 50 },
{ "PDR-3320", 8.157895, 50 },
{ "PDR-4300", 10.252525, 50 },
{ "PDR-5300", 10.285714, 50 },
{ "PDR M11", 8.628205, 50 },
{ "PDR-M25", 8.157895, 50 },
{ "PDR-M3", 10.523594, 50 },
{ "PDRM5", 9.208145, 50 },
{ "PDR-M60", 10.495627, 50 },
{ "PDR-M61", 10.496649, 50 },
{ "PDR-M65", 10.496649, 50 },
{ "PDRM70", 10.000000, 50 },
{ "PDR-M700", 7.702703, 50 },
{ "PDR-M71", 10.357143, 50 },
{ "PDRM81", 10.252525, 50 },
{ "PDR-T10", 7.842105, 50 },
{ "PDR-T20", 7.631579, 50 },
{ "PDR-T30", 7.631579, 50 },
};

#define PROCESS_TABLE_SIZE  (sizeof(ProcessTable) / sizeof(TagTable_t))
// 1 - "The 0th row is at the visual top of the image,    and the 0th column is the visual left-hand side."
// 2 - "The 0th row is at the visual top of the image,    and the 0th column is the visual right-hand side."
// 3 - "The 0th row is at the visual bottom of the image, and the 0th column is the visual right-hand side."
// 4 - "The 0th row is at the visual bottom of the image, and the 0th column is the visual left-hand side."
// 5 - "The 0th row is the visual left-hand side of of the image,  and the 0th column is the visual top."
// 6 - "The 0th row is the visual right-hand side of of the image, and the 0th column is the visual top."
// 7 - "The 0th row is the visual right-hand side of of the image, and the 0th column is the visual bottom."
// 8 - "The 0th row is the visual left-hand side of of the image,  and the 0th column is the visual bottom."
// Note: The descriptions here are the same as the name of the command line
// option to pass to jpegtran to right the image

static const char * OrientTab[9] = {
    "Undefined",
    "Normal",           // 1
    "flip horizontal",  // left right reversed mirror
    "rotate 180",       // 3
    "flip vertical",    // upside down mirror
    "transpose",        // Flipped about top-left <--> bottom-right axis.
    "rotate 90",        // rotate 90 cw to right it.
    "transverse",       // flipped about top-right <--> bottom-left axis
    "rotate 270",       // rotate 270 to right it.
};

const int BytesPerFormat[] = {0,1,1,2,4,8,1,1,2,4,8,4,8};
//--------------------------------------------------------------------------
// Describes tag values

#define TAG_INTEROP_INDEX          0x0001
#define TAG_INTEROP_VERSION        0x0002
#define TAG_IMAGE_WIDTH            0x0100
#define TAG_IMAGE_LENGTH           0x0101
#define TAG_BITS_PER_SAMPLE        0x0102
#define TAG_COMPRESSION            0x0103
#define TAG_PHOTOMETRIC_INTERP     0x0106
#define TAG_FILL_ORDER             0x010A
#define TAG_DOCUMENT_NAME          0x010D
#define TAG_IMAGE_DESCRIPTION      0x010E
#define TAG_MAKE                   0x010F
#define TAG_MODEL                  0x0110
#define TAG_SRIP_OFFSET            0x0111
#define TAG_ORIENTATION            0x0112
#define TAG_SAMPLES_PER_PIXEL      0x0115
#define TAG_ROWS_PER_STRIP         0x0116
#define TAG_STRIP_BYTE_COUNTS      0x0117
#define TAG_X_RESOLUTION           0x011A
#define TAG_Y_RESOLUTION           0x011B
#define TAG_PLANAR_CONFIGURATION   0x011C
#define TAG_RESOLUTION_UNIT        0x0128
#define TAG_TRANSFER_FUNCTION      0x012D
#define TAG_SOFTWARE               0x0131
#define TAG_DATETIME               0x0132
#define TAG_ARTIST                 0x013B
#define TAG_WHITE_POINT            0x013E
#define TAG_PRIMARY_CHROMATICITIES 0x013F
#define TAG_TRANSFER_RANGE         0x0156
#define TAG_JPEG_PROC              0x0200
#define TAG_THUMBNAIL_OFFSET       0x0201
#define TAG_THUMBNAIL_LENGTH       0x0202
#define TAG_Y_CB_CR_COEFFICIENTS   0x0211
#define TAG_Y_CB_CR_SUB_SAMPLING   0x0212
#define TAG_Y_CB_CR_POSITIONING    0x0213
#define TAG_REFERENCE_BLACK_WHITE  0x0214
#define TAG_RELATED_IMAGE_WIDTH    0x1001
#define TAG_RELATED_IMAGE_LENGTH   0x1002
#define TAG_CFA_REPEAT_PATTERN_DIM 0x828D
#define TAG_CFA_PATTERN1           0x828E
#define TAG_BATTERY_LEVEL          0x828F
#define TAG_COPYRIGHT              0x8298
#define TAG_EXPOSURETIME           0x829A
#define TAG_FNUMBER                0x829D
#define TAG_IPTC_NAA               0x83BB
#define TAG_EXIF_OFFSET            0x8769
#define TAG_INTER_COLOR_PROFILE    0x8773
#define TAG_EXPOSURE_PROGRAM       0x8822
#define TAG_SPECTRAL_SENSITIVITY   0x8824
#define TAG_GPSINFO                0x8825
#define TAG_ISO_EQUIVALENT         0x8827
#define TAG_OECF                   0x8828
#define TAG_EXIF_VERSION           0x9000
#define TAG_DATETIME_ORIGINAL      0x9003
#define TAG_DATETIME_DIGITIZED     0x9004
#define TAG_COMPONENTS_CONFIG      0x9101
#define TAG_CPRS_BITS_PER_PIXEL    0x9102
#define TAG_SHUTTERSPEED           0x9201
#define TAG_APERTURE               0x9202
#define TAG_BRIGHTNESS_VALUE       0x9203
#define TAG_EXPOSURE_BIAS          0x9204
#define TAG_MAXAPERTURE            0x9205
#define TAG_SUBJECT_DISTANCE       0x9206
#define TAG_METERING_MODE          0x9207
#define TAG_LIGHT_SOURCE           0x9208
#define TAG_FLASH                  0x9209
#define TAG_FOCALLENGTH            0x920A
#define TAG_SUBJECTAREA            0x9214
#define TAG_MAKER_NOTE             0x927C
#define TAG_USERCOMMENT            0x9286
#define TAG_SUBSEC_TIME            0x9290
#define TAG_SUBSEC_TIME_ORIG       0x9291
#define TAG_SUBSEC_TIME_DIG        0x9292

#define TAG_WINXP_TITLE            0x9c9b // Windows XP - not part of exif standard.
#define TAG_WINXP_COMMENT          0x9c9c // Windows XP - not part of exif standard.
#define TAG_WINXP_AUTHOR           0x9c9d // Windows XP - not part of exif standard.
#define TAG_WINXP_KEYWORDS         0x9c9e // Windows XP - not part of exif standard.
#define TAG_WINXP_SUBJECT          0x9c9f // Windows XP - not part of exif standard.

#define TAG_FLASH_PIX_VERSION      0xA000
#define TAG_COLOR_SPACE            0xA001
#define TAG_PIXEL_X_DIMENSION      0xA002
#define TAG_PIXEL_Y_DIMENSION      0xA003
#define TAG_RELATED_AUDIO_FILE     0xA004
#define TAG_INTEROP_OFFSET         0xA005
#define TAG_FLASH_ENERGY           0xA20B
#define TAG_SPATIAL_FREQ_RESP      0xA20C
#define TAG_FOCAL_PLANE_XRES       0xA20E
#define TAG_FOCAL_PLANE_YRES       0xA20F
#define TAG_FOCAL_PLANE_UNITS      0xA210
#define TAG_SUBJECT_LOCATION       0xA214
#define TAG_EXPOSURE_INDEX         0xA215
#define TAG_SENSING_METHOD         0xA217
#define TAG_FILE_SOURCE            0xA300
#define TAG_SCENE_TYPE             0xA301
#define TAG_CFA_PATTERN            0xA302
#define TAG_CUSTOM_RENDERED        0xA401
#define TAG_EXPOSURE_MODE          0xA402
#define TAG_WHITEBALANCE           0xA403
#define TAG_DIGITALZOOMRATIO       0xA404
#define TAG_FOCALLENGTH_35MM       0xA405
#define TAG_SCENE_CAPTURE_TYPE     0xA406
#define TAG_GAIN_CONTROL           0xA407
#define TAG_CONTRAST               0xA408
#define TAG_SATURATION             0xA409
#define TAG_SHARPNESS              0xA40A
#define TAG_DISTANCE_RANGE         0xA40C
#define TAG_IMAGE_UNIQUE_ID        0xA420

static const TagTable_t TagTable[] = {
  { TAG_INTEROP_INDEX,          "InteropIndex"},
  { TAG_INTEROP_VERSION,        "InteropVersion"},
  { TAG_IMAGE_WIDTH,            "ImageWidth"},
  { TAG_IMAGE_LENGTH,           "ImageLength"},
  { TAG_BITS_PER_SAMPLE,        "BitsPerSample"},
  { TAG_COMPRESSION,            "Compression"},
  { TAG_PHOTOMETRIC_INTERP,     "PhotometricInterpretation"},
  { TAG_FILL_ORDER,             "FillOrder"},
  { TAG_DOCUMENT_NAME,          "DocumentName"},
  { TAG_IMAGE_DESCRIPTION,      "ImageDescription"},
  { TAG_MAKE,                   "Make"},
  { TAG_MODEL,                  "Model"},
  { TAG_SRIP_OFFSET,            "StripOffsets"},
  { TAG_ORIENTATION,            "Orientation"},
  { TAG_SAMPLES_PER_PIXEL,      "SamplesPerPixel"},
  { TAG_ROWS_PER_STRIP,         "RowsPerStrip"},
  { TAG_STRIP_BYTE_COUNTS,      "StripByteCounts"},
  { TAG_X_RESOLUTION,           "XResolution"},
  { TAG_Y_RESOLUTION,           "YResolution"},
  { TAG_PLANAR_CONFIGURATION,   "PlanarConfiguration"},
  { TAG_RESOLUTION_UNIT,        "ResolutionUnit"},
  { TAG_TRANSFER_FUNCTION,      "TransferFunction"},
  { TAG_SOFTWARE,               "Software"},
  { TAG_DATETIME,               "DateTime"},
  { TAG_ARTIST,                 "Artist"},
  { TAG_WHITE_POINT,            "WhitePoint"},
  { TAG_PRIMARY_CHROMATICITIES, "PrimaryChromaticities"},
  { TAG_TRANSFER_RANGE,         "TransferRange"},
  { TAG_JPEG_PROC,              "JPEGProc"},
  { TAG_THUMBNAIL_OFFSET,       "ThumbnailOffset"},
  { TAG_THUMBNAIL_LENGTH,       "ThumbnailLength"},
  { TAG_Y_CB_CR_COEFFICIENTS,   "YCbCrCoefficients"},
  { TAG_Y_CB_CR_SUB_SAMPLING,   "YCbCrSubSampling"},
  { TAG_Y_CB_CR_POSITIONING,    "YCbCrPositioning"},
  { TAG_REFERENCE_BLACK_WHITE,  "ReferenceBlackWhite"},
  { TAG_RELATED_IMAGE_WIDTH,    "RelatedImageWidth"},
  { TAG_RELATED_IMAGE_LENGTH,   "RelatedImageLength"},
  { TAG_CFA_REPEAT_PATTERN_DIM, "CFARepeatPatternDim"},
  { TAG_CFA_PATTERN1,           "CFAPattern"},
  { TAG_BATTERY_LEVEL,          "BatteryLevel"},
  { TAG_COPYRIGHT,              "Copyright"},
  { TAG_EXPOSURETIME,           "ExposureTime"},
  { TAG_FNUMBER,                "FNumber"},
  { TAG_IPTC_NAA,               "IPTC/NAA"},
  { TAG_EXIF_OFFSET,            "ExifOffset"},
  { TAG_INTER_COLOR_PROFILE,    "InterColorProfile"},
  { TAG_EXPOSURE_PROGRAM,       "ExposureProgram"},
  { TAG_SPECTRAL_SENSITIVITY,   "SpectralSensitivity"},
  { TAG_GPSINFO,                "GPS Dir offset"},
  { TAG_ISO_EQUIVALENT,         "ISOSpeedRatings"},
  { TAG_OECF,                   "OECF"},
  { TAG_EXIF_VERSION,           "ExifVersion"},
  { TAG_DATETIME_ORIGINAL,      "DateTimeOriginal"},
  { TAG_DATETIME_DIGITIZED,     "DateTimeDigitized"},
  { TAG_COMPONENTS_CONFIG,      "ComponentsConfiguration"},
  { TAG_CPRS_BITS_PER_PIXEL,    "CompressedBitsPerPixel"},
  { TAG_SHUTTERSPEED,           "ShutterSpeedValue"},
  { TAG_APERTURE,               "ApertureValue"},
  { TAG_BRIGHTNESS_VALUE,       "BrightnessValue"},
  { TAG_EXPOSURE_BIAS,          "ExposureBiasValue"},
  { TAG_MAXAPERTURE,            "MaxApertureValue"},
  { TAG_SUBJECT_DISTANCE,       "SubjectDistance"},
  { TAG_METERING_MODE,          "MeteringMode"},
  { TAG_LIGHT_SOURCE,           "LightSource"},
  { TAG_FLASH,                  "Flash"},
  { TAG_FOCALLENGTH,            "FocalLength"},
  { TAG_MAKER_NOTE,             "MakerNote"},
  { TAG_USERCOMMENT,            "UserComment"},
  { TAG_SUBSEC_TIME,            "SubSecTime"},
  { TAG_SUBSEC_TIME_ORIG,       "SubSecTimeOriginal"},
  { TAG_SUBSEC_TIME_DIG,        "SubSecTimeDigitized"},
  { TAG_WINXP_TITLE,            "Windows-XP Title"},
  { TAG_WINXP_COMMENT,          "Windows-XP comment"},
  { TAG_WINXP_AUTHOR,           "Windows-XP author"},
  { TAG_WINXP_KEYWORDS,         "Windows-XP keywords"},
  { TAG_WINXP_SUBJECT,          "Windows-XP subject"},
  { TAG_FLASH_PIX_VERSION,      "FlashPixVersion"},
  { TAG_COLOR_SPACE,            "ColorSpace"},
  { TAG_PIXEL_X_DIMENSION,      "ExifImageWidth"},
  { TAG_PIXEL_Y_DIMENSION,      "ExifImageLength"},
  { TAG_RELATED_AUDIO_FILE,     "RelatedAudioFile"},
  { TAG_INTEROP_OFFSET,         "InteroperabilityOffset"},
  { TAG_FLASH_ENERGY,           "FlashEnergy"},              
  { TAG_SPATIAL_FREQ_RESP,      "SpatialFrequencyResponse"}, 
  { TAG_FOCAL_PLANE_XRES,       "FocalPlaneXResolution"},    
  { TAG_FOCAL_PLANE_YRES,       "FocalPlaneYResolution"},    
  { TAG_FOCAL_PLANE_UNITS,      "FocalPlaneResolutionUnit"}, 
  { TAG_SUBJECT_LOCATION,       "SubjectLocation"},          
  { TAG_EXPOSURE_INDEX,         "ExposureIndex"},            
  { TAG_SENSING_METHOD,         "SensingMethod"},            
  { TAG_FILE_SOURCE,            "FileSource"},
  { TAG_SCENE_TYPE,             "SceneType"},
  { TAG_CFA_PATTERN,            "CFA Pattern"},
  { TAG_CUSTOM_RENDERED,        "CustomRendered"},
  { TAG_EXPOSURE_MODE,          "ExposureMode"},
  { TAG_WHITEBALANCE,           "WhiteBalance"},
  { TAG_DIGITALZOOMRATIO,       "DigitalZoomRatio"},
  { TAG_FOCALLENGTH_35MM,       "FocalLengthIn35mmFilm"},
  { TAG_SUBJECTAREA,            "SubjectArea"},  
  { TAG_SCENE_CAPTURE_TYPE,     "SceneCaptureType"},
  { TAG_GAIN_CONTROL,           "GainControl"},
  { TAG_CONTRAST,               "Contrast"},
  { TAG_SATURATION,             "Saturation"},
  { TAG_SHARPNESS,              "Sharpness"},
  { TAG_DISTANCE_RANGE,         "SubjectDistanceRange"},
  { TAG_IMAGE_UNIQUE_ID,        "ImageUniqueId"},  
} ;

#define TAG_TABLE_SIZE  (sizeof(TagTable) / sizeof(TagTable_t))

//--------------------------------------------------------------------------
// Convert a 16 bit unsigned value to file's native byte order
//--------------------------------------------------------------------------
static void Put16u(void * Short, unsigned short PutValue)
{
    if (MotorolaOrder){
        ((uchar *)Short)[0] = (uchar)(PutValue>>8);
        ((uchar *)Short)[1] = (uchar)PutValue;
    }else{
        ((uchar *)Short)[0] = (uchar)PutValue;
        ((uchar *)Short)[1] = (uchar)(PutValue>>8);
    }
}

//--------------------------------------------------------------------------
// Convert a 16 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
int Get16u(void * Short)
{
    if (MotorolaOrder){
        return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
    }else{
        return (((uchar *)Short)[1] << 8) | ((uchar *)Short)[0];
    }
}

//--------------------------------------------------------------------------
// Convert a 32 bit signed value from file's native byte order
//--------------------------------------------------------------------------
int Get32s(void * Long)
{
    if (MotorolaOrder){
        return  ((( char *)Long)[0] << 24) | (((uchar *)Long)[1] << 16)
              | (((uchar *)Long)[2] << 8 ) | (((uchar *)Long)[3] << 0 );
    }else{
        return  ((( char *)Long)[3] << 24) | (((uchar *)Long)[2] << 16)
              | (((uchar *)Long)[1] << 8 ) | (((uchar *)Long)[0] << 0 );
    }
}

//--------------------------------------------------------------------------
// Convert a 32 bit unsigned value to file's native byte order
//--------------------------------------------------------------------------
void Put32u(void * Value, unsigned PutValue)
{
    if (MotorolaOrder){
        ((uchar *)Value)[0] = (uchar)(PutValue>>24);
        ((uchar *)Value)[1] = (uchar)(PutValue>>16);
        ((uchar *)Value)[2] = (uchar)(PutValue>>8);
        ((uchar *)Value)[3] = (uchar)PutValue;
    }else{
        ((uchar *)Value)[0] = (uchar)PutValue;
        ((uchar *)Value)[1] = (uchar)(PutValue>>8);
        ((uchar *)Value)[2] = (uchar)(PutValue>>16);
        ((uchar *)Value)[3] = (uchar)(PutValue>>24);
    }
}

//--------------------------------------------------------------------------
// Convert a 32 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
unsigned Get32u(void * Long)
{
    return (unsigned)Get32s(Long) & 0xffffffff;
}

//--------------------------------------------------------------------------
// Evaluate number, be it int, rational, or float from directory.
//--------------------------------------------------------------------------
double ConvertAnyFormat(void * ValuePtr, int Format)
{
    double Value;
    Value = 0;
    switch(Format){
        case FMT_SBYTE:     Value = *(signed char *)ValuePtr;  break;
        case FMT_BYTE:      Value = *(uchar *)ValuePtr;        break;
        case FMT_USHORT:    Value = Get16u(ValuePtr);          break;
        case FMT_ULONG:     Value = Get32u(ValuePtr);          break;
        case FMT_URATIONAL:
        case FMT_SRATIONAL: 
            {
                int Num,Den;
                Num = Get32s(ValuePtr);
                Den = Get32s(4+(char *)ValuePtr);
                if (Den == 0){
                    Value = 0;
                }else{
                    Value = (double)Num/Den;
                }
                break;
            }
        case FMT_SSHORT:    Value = (signed short)Get16u(ValuePtr);  break;
        case FMT_SLONG:     Value = Get32s(ValuePtr);                break;
        // Not sure if this is correct (never seen float used in Exif format)
        case FMT_SINGLE:    Value = (double)*(float *)ValuePtr;      break;
        case FMT_DOUBLE:    Value = *(double *)ValuePtr;             break;
        default:
            ErrNonfatal("Illegal format code %d",Format,0);
    }
    return Value;
}

//--------------------------------------------------------------------------
// Process one of the nested EXIF directories.
//--------------------------------------------------------------------------
static void ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, 
        unsigned ExifLength, int NestingLevel)
{
    int de;
    int a;
    int NumDirEntries;
    unsigned ThumbnailOffset = 0;
    unsigned ThumbnailSize = 0;
    char IndentString[25];
    if (NestingLevel > 4){
        ErrNonfatal("Maximum directory nesting exceeded (corrupt exif header)", 0,0);
        return;
    }
    memset(IndentString, ' ', 25);
    IndentString[NestingLevel * 4] = '\0';
    NumDirEntries = Get16u(DirStart);
    #define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))
    {
        unsigned char * DirEnd;
        DirEnd = DIR_ENTRY_ADDR(DirStart, NumDirEntries);
        if (DirEnd+4 > (OffsetBase+ExifLength)){
            if (DirEnd+2 == OffsetBase+ExifLength || DirEnd == OffsetBase+ExifLength){
                // Version 1.3 of jhead would truncate a bit too much.
                // This also caught later on as well.
            }else{
                ErrNonfatal("Illegally sized exif subdirectory (%d entries)",NumDirEntries,0);
                return;
            }
        }
    }
    for (de=0;de<NumDirEntries;de++){
        int Tag, Format, Components;
        unsigned char * ValuePtr;
        int ByteCount;
        unsigned char * DirEntry;
        DirEntry = DIR_ENTRY_ADDR(DirStart, de);
        Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);
        if ((Format-1) >= NUM_FORMATS) {
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            ErrNonfatal("Illegal number format %d for tag %04x", Format, Tag);
            continue;
        }
        if ((unsigned)Components > 0x10000){
            ErrNonfatal("Illegal number of components %d for tag %04x", Components, Tag);
            continue;
        }
        ByteCount = Components * BytesPerFormat[Format];
        if (ByteCount > 4){
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8);
            // If its bigger than 4 bytes, the dir entry contains an offset.
            if (OffsetVal+ByteCount > ExifLength){
                // Bogus pointer offset and / or bytecount value
                ErrNonfatal("Illegal value pointer for tag %04x", Tag,0);
                continue;
            }
            ValuePtr = OffsetBase+OffsetVal;
            if (OffsetVal > ImageInfo.LargestExifOffset){
                ImageInfo.LargestExifOffset = OffsetVal;
            }
        }else{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = DirEntry+8;
        }
        if (Tag == TAG_MAKER_NOTE){
            ProcessMakerNote(ValuePtr, ByteCount, OffsetBase, ExifLength);
            continue;
        }
        // Extract useful components of tag
        switch(Tag){
            case TAG_MAKE:
                strncpy(ImageInfo.CameraMake, (char *)ValuePtr, ByteCount < 31 ? ByteCount : 31);
                break;
            case TAG_MODEL:
                strncpy(ImageInfo.CameraModel, (char *)ValuePtr, ByteCount < 39 ? ByteCount : 39);
                break;
            case TAG_DATETIME_ORIGINAL:
                // If we get a DATETIME_ORIGINAL, we use that one.
                strncpy(ImageInfo.DateTime, (char *)ValuePtr, 19);
                // Fallthru...
            case TAG_DATETIME_DIGITIZED:
            case TAG_DATETIME:
                if (!isdigit(ImageInfo.DateTime[0])){
                    // If we don't already have a DATETIME_ORIGINAL, use whatever
                    // time fields we may have.
                    strncpy(ImageInfo.DateTime, (char *)ValuePtr, 19);
                }
                if (ImageInfo.numDateTimeTags >= MAX_DATE_COPIES){
                    ErrNonfatal("More than %d date fields!  This is nuts", MAX_DATE_COPIES, 0);
                    break;
                }
                ImageInfo.DateTimeOffsets[ImageInfo.numDateTimeTags++] = 
                    (char *)ValuePtr - (char *)OffsetBase;
                break;
            case TAG_WINXP_COMMENT:
                if (ImageInfo.Comments[0]){ // We already have a jpeg comment.
                    break; // Already have a windows comment, skip this one.
                }
                if (ByteCount > 1){
                    if (ByteCount > MAX_COMMENT_SIZE) ByteCount = MAX_COMMENT_SIZE;
                    memcpy(ImageInfo.Comments, ValuePtr, ByteCount);
                    ImageInfo.CommentWidchars = ByteCount/2;
                }
                break;                
            case TAG_USERCOMMENT:
                if (ImageInfo.Comments[0]){ // We already have a jpeg comment.
                    break; // Already have a windows comment, skip this one.
                }            	
                // Comments are often padded with trailing spaces.  Remove these first.
                for (a=ByteCount;;){
                    a--;
                    if ((ValuePtr)[a] == ' '){
                        (ValuePtr)[a] = '\0';
                    }else{
                        break;
                    }
                    if (a == 0) break;
                }
                // Copy the comment
                {
                    int msiz = ExifLength - (ValuePtr-OffsetBase);
                    if (msiz > ByteCount) msiz = ByteCount;
                    if (msiz > MAX_COMMENT_SIZE-1) msiz = MAX_COMMENT_SIZE-1;
                    if (msiz > 5 && memcmp(ValuePtr, "ASCII",5) == 0){
                        for (a=5;a<10 && a < msiz;a++){
                            int c = (ValuePtr)[a];
                            if (c != '\0' && c != ' '){
                                strncpy(ImageInfo.Comments, (char *)ValuePtr+a, msiz-a);
                                break;
                            }
                        }
                    }else{
                        strncpy(ImageInfo.Comments, (char *)ValuePtr, msiz);
                    }
                }
                break;
            case TAG_FNUMBER:
                // Simplest way of expressing aperture, so I trust it the most.
                // (overwrite previously computd value if there is one)
                ImageInfo.ApertureFNumber = (float)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_APERTURE:
            case TAG_MAXAPERTURE:
                // More relevant info always comes earlier, so only use this field if we don't 
                // have appropriate aperture information yet.
                if (ImageInfo.ApertureFNumber == 0){
                    ImageInfo.ApertureFNumber 
                        = (float)exp(ConvertAnyFormat(ValuePtr, Format)*log(2.0)*0.5);
                }
                break;
            case TAG_FOCALLENGTH:
                // Nice digital cameras actually save the focal length as a function
                // of how farthey are zoomed in.
                ImageInfo.FocalLength = (float)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_SUBJECT_DISTANCE:
                // Inidcates the distacne the autofocus camera is focused to.
                // Tends to be less accurate as distance increases.
                ImageInfo.Distance = (float)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_EXPOSURETIME:
                // Simplest way of expressing exposure time, so I trust it most.
                // (overwrite previously computd value if there is one)
                ImageInfo.ExposureTime = (float)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_SHUTTERSPEED:
                // More complicated way of expressing exposure time, so only use
                // this value if we don't already have it from somewhere else.
                if (ImageInfo.ExposureTime == 0){
                    ImageInfo.ExposureTime 
                        = (float)(1/exp(ConvertAnyFormat(ValuePtr, Format)*log(2.0)));
                }
                break;
            case TAG_FLASH:
                ImageInfo.FlashUsed=(int)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_ORIENTATION:
                if (NumOrientations >= 2){
                    // Can have another orientation tag for the thumbnail, but if there's
                    // a third one, things are stringae.
                    ErrNonfatal("More than two orientation tags!",0,0);
                    break;
                }
                OrientationPtr[NumOrientations] = ValuePtr;
                OrientationNumFormat[NumOrientations] = Format;
                if (NumOrientations == 0){
                    ImageInfo.Orientation = (int)ConvertAnyFormat(ValuePtr, Format);
                }
                if (ImageInfo.Orientation < 0 || ImageInfo.Orientation > 8){
                    ErrNonfatal("Undefined rotation value %d", ImageInfo.Orientation, 0);
                    ImageInfo.Orientation = 0;
                }
                NumOrientations += 1;
                break;
            case TAG_PIXEL_Y_DIMENSION:
            case TAG_PIXEL_X_DIMENSION:
                // Use largest of height and width to deal with images that have been
                // rotated to portrait format.
                a = (int)ConvertAnyFormat(ValuePtr, Format);
                if (ExifImageWidth < a) ExifImageWidth = a;
                break;
            case TAG_FOCAL_PLANE_XRES:
                FocalplaneXRes = ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_FOCAL_PLANE_UNITS:
                switch((int)ConvertAnyFormat(ValuePtr, Format)){
                    case 1: FocalplaneUnits = 25.4; break; // inch
                    case 2: 
                        // According to the information I was using, 2 means meters.
                        // But looking at the Canon powershot's files, inches is the only
                        // sensible value.
                        FocalplaneUnits = 25.4;
                        break;
                    case 3: FocalplaneUnits = 10;   break;  // centimeter
                    case 4: FocalplaneUnits = 1;    break;  // millimeter
                    case 5: FocalplaneUnits = .001; break;  // micrometer
                }
                break;
            case TAG_EXPOSURE_BIAS:
                ImageInfo.ExposureBias = (float)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_WHITEBALANCE:
                ImageInfo.Whitebalance = (int)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_LIGHT_SOURCE:
                ImageInfo.LightSource = (int)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_METERING_MODE:
                ImageInfo.MeteringMode = (int)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_EXPOSURE_PROGRAM:
                ImageInfo.ExposureProgram = (int)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_EXPOSURE_INDEX:
                if (ImageInfo.ISOequivalent == 0){
                    // Exposure index and ISO equivalent are often used interchangeably,
                    // so we will do the same in jhead.
                    // http://photography.about.com/library/glossary/bldef_ei.htm
                    ImageInfo.ISOequivalent = (int)ConvertAnyFormat(ValuePtr, Format);
                }
                break;
            case TAG_EXPOSURE_MODE:
                ImageInfo.ExposureMode = (int)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_ISO_EQUIVALENT:
                ImageInfo.ISOequivalent = (int)ConvertAnyFormat(ValuePtr, Format);
                if ( ImageInfo.ISOequivalent < 50 ){
                    // Fixes strange encoding on some older digicams.
                    ImageInfo.ISOequivalent *= 200;
                }
                break;
            case TAG_DIGITALZOOMRATIO:
                ImageInfo.DigitalZoomRatio = (float)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_THUMBNAIL_OFFSET:
                ThumbnailOffset = (unsigned)ConvertAnyFormat(ValuePtr, Format);
                DirWithThumbnailPtrs = DirStart;
                break;
            case TAG_THUMBNAIL_LENGTH:
                ThumbnailSize = (unsigned)ConvertAnyFormat(ValuePtr, Format);
                ImageInfo.ThumbnailSizeOffset = ValuePtr-OffsetBase;
                break;
            case TAG_EXIF_OFFSET:
            case TAG_INTEROP_OFFSET:
                {
                    unsigned char * SubdirStart;
                    SubdirStart = OffsetBase + Get32u(ValuePtr);
                    if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength){
                        ErrNonfatal("Illegal exif or interop ofset directory link",0,0);
                    }else{
                        ProcessExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
                    }
                    continue;
                }
                break;
            case TAG_GPSINFO:
                {
                    unsigned char * SubdirStart;
                    SubdirStart = OffsetBase + Get32u(ValuePtr);
                    if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength){
                        ErrNonfatal("Illegal GPS directory link",0,0);
                    }else{
                        ProcessGpsInfo(SubdirStart, ByteCount, OffsetBase, ExifLength);
                    }
                    continue;
                }
                break;
            case TAG_FOCALLENGTH_35MM:
                // The focal length equivalent 35 mm is a 2.2 tag (defined as of April 2002)
                // if its present, use it to compute equivalent focal length instead of 
                // computing it from sensor geometry and actual focal length.
                ImageInfo.FocalLength35mmEquiv = (unsigned)ConvertAnyFormat(ValuePtr, Format);
                jheadfocal = ImageInfo.FocalLength35mmEquiv;
                break;
            case TAG_DISTANCE_RANGE:
                // Three possible standard values:
                //   1 = macro, 2 = close, 3 = distant
                ImageInfo.DistanceRange = (int)ConvertAnyFormat(ValuePtr, Format);
                break;
                
            case TAG_X_RESOLUTION:
                if (NestingLevel==0) {// Only use the values from the top level directory
                    ImageInfo.xResolution = (float)ConvertAnyFormat(ValuePtr,Format);
                    // if yResolution has not been set, use the value of xResolution
                    if (ImageInfo.yResolution == 0.0) ImageInfo.yResolution = ImageInfo.xResolution;
                }
                break;

            case TAG_Y_RESOLUTION:
                if (NestingLevel==0) {// Only use the values from the top level directory
                    ImageInfo.yResolution = (float)ConvertAnyFormat(ValuePtr,Format);
                    // if xResolution has not been set, use the value of yResolution
                    if (ImageInfo.xResolution == 0.0) ImageInfo.xResolution = ImageInfo.yResolution;
                }
                break;

            case TAG_RESOLUTION_UNIT:
                if (NestingLevel==0) {// Only use the values from the top level directory
                    ImageInfo.ResolutionUnit = (int) ConvertAnyFormat(ValuePtr,Format);
                }
                break;                
        }
    }
    {
        // In addition to linking to subdirectories via exif tags, 
        // there's also a potential link to another directory at the end of each
        // directory.  this has got to be the result of a committee!
        unsigned char * SubdirStart;
        unsigned Offset;
        if (DIR_ENTRY_ADDR(DirStart, NumDirEntries) + 4 <= OffsetBase+ExifLength){
            Offset = Get32u(DirStart+2+12*NumDirEntries);
            if (Offset){
                SubdirStart = OffsetBase + Offset;
                if (SubdirStart > OffsetBase+ExifLength || SubdirStart < OffsetBase){
                    if (SubdirStart > OffsetBase && SubdirStart < OffsetBase+ExifLength+20){
                        // Jhead 1.3 or earlier would crop the whole directory!
                        // As Jhead produces this form of format incorrectness, 
                        // I'll just let it pass silently
                    }else{
                        ErrNonfatal("Illegal subdirectory link",0,0);
                    }
                }else{
                    if (SubdirStart <= OffsetBase+ExifLength){
                        ProcessExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
                    }
                }
                if (Offset > ImageInfo.LargestExifOffset){
                    ImageInfo.LargestExifOffset = Offset;
                }
            }
        }else{
            // The exif header ends before the last next directory pointer.
        }
    }
    if (ThumbnailOffset){
        ImageInfo.ThumbnailAtEnd = FALSE;
        if (ThumbnailOffset <= ExifLength){
            if (ThumbnailSize > ExifLength-ThumbnailOffset){
                // If thumbnail extends past exif header, only save the part that
                // actually exists.  Canon's EOS viewer utility will do this - the
                // thumbnail extracts ok with this hack.
                ThumbnailSize = ExifLength-ThumbnailOffset;
            }
            // The thumbnail pointer appears to be valid.  Store it.
            ImageInfo.ThumbnailOffset = ThumbnailOffset;
            ImageInfo.ThumbnailSize = ThumbnailSize;
        }
    }
}

//--------------------------------------------------------------------------
// Process a EXIF marker
// Describes all the drivel that most digital cameras include...
//--------------------------------------------------------------------------
void process_EXIF (unsigned char * ExifSection, unsigned int length)
{
    int FirstOffset;
    int a;
    FocalplaneXRes = 0;
    FocalplaneUnits = 0;
    ExifImageWidth = 0;
    NumOrientations = 0;
    {   // Check the EXIF header component
        static uchar ExifHeader[] = "Exif\0\0";
        if (memcmp(ExifSection+2, ExifHeader,6)){
            ErrNonfatal("Incorrect Exif header",0,0);
            return;
        }
    }
    if (memcmp(ExifSection+8,"II",2) == 0){
        MotorolaOrder = 0;
    }else{
        if (memcmp(ExifSection+8,"MM",2) == 0){
            MotorolaOrder = 1;
        }else{
            ErrNonfatal("Invalid Exif alignment marker.",0,0);
            return;
        }
    }
    // Check the next value for correctness.
    if (Get16u(ExifSection+10) != 0x2a){
        ErrNonfatal("Invalid Exif start (1)",0,0);
        return;
    }
    FirstOffset = Get32u(ExifSection+12);
    if (FirstOffset < 8 || FirstOffset > 16){
        // Usually set to 8, but other values valid too.
        ErrNonfatal("Suspicious offset of first IFD value",0,0);
    }
    DirWithThumbnailPtrs = NULL;
    // First directory starts 16 bytes in.  All offset are relative to 8 bytes in.
    ProcessExifDir(ExifSection+8+FirstOffset, ExifSection+8, length-8, 0);
    ImageInfo.ThumbnailAtEnd = ImageInfo.ThumbnailOffset >= ImageInfo.LargestExifOffset ? TRUE : FALSE;
	if ((ImageInfo.FocalLength>0.0001) && (ImageInfo.CameraModel)) {
		for (a=0; a<1846; a++) {
			if (!strcmp(KameraTable[a].KamStr,ImageInfo.CameraModel)) {
				ImageInfo.FocalLength35mmEquiv = (int)(ImageInfo.FocalLength*KameraTable[a].bre2/KameraTable[a].bre1);
				jheadfocal = ImageInfo.FocalLength35mmEquiv;
			}
		}
	}    
    // Compute the CCD width, in millimeters.
    if (FocalplaneXRes != 0){
        // Note: With some cameras, its not possible to compute this correctly because
        // they don't adjust the indicated focal plane resolution units when using less
        // than maximum resolution, so the CCDWidth value comes out too small.  Nothing
        // that Jhad can do about it - its a camera problem.
        ImageInfo.CCDWidth = (float)(ExifImageWidth * FocalplaneUnits / FocalplaneXRes);
        if ((ImageInfo.FocalLength>0.0001) && (ImageInfo.FocalLength35mmEquiv==0)){
            // Compute 35 mm equivalent focal length based on sensor geometry if we haven't
            // already got it explicitly from a tag.
            ImageInfo.FocalLength35mmEquiv = (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*36 + 0.5);
            jheadfocal = ImageInfo.FocalLength35mmEquiv;
        }
    }
}

//--------------------------------------------------------------------------
// Clear the rotation tag in the exif header to 1.
//--------------------------------------------------------------------------
const char * ClearOrientation(void)
{
    int a;
    if (NumOrientations == 0) return NULL;
    for (a=0;a<NumOrientations;a++){
        switch(OrientationNumFormat[a]){
            case FMT_SBYTE:
            case FMT_BYTE:      
                *(uchar *)(OrientationPtr[a]) = 1;
                break;
            case FMT_USHORT:    
                Put16u(OrientationPtr[a], 1);                
                break;
            case FMT_ULONG:     
            case FMT_SLONG:     
                memset(OrientationPtr, 0, 4);
                // Can't be bothered to write  generic Put32 if I only use it once.
                if (MotorolaOrder){
                    ((uchar *)OrientationPtr[a])[3] = 1;
                }else{
                    ((uchar *)OrientationPtr[a])[0] = 1;
                }
                break;
            default:
                return NULL;
        }
    }
    return OrientTab[ImageInfo.Orientation];
}

//--------------------------------------------------------------------------
// Remove thumbnail out of the exif image.
//--------------------------------------------------------------------------
int RemoveThumbnail(unsigned char * ExifSection)
{
    if (!DirWithThumbnailPtrs || 
        ImageInfo.ThumbnailOffset == 0 || 
        ImageInfo.ThumbnailSize == 0){
        // No thumbnail, or already deleted it.
        return 0;
    }
    if (ImageInfo.ThumbnailAtEnd == FALSE){
        ErrNonfatal("Thumbnail is not at end of header, can't chop it off", 0, 0);
        return 0;
    }
    {
        int de;
        int NumDirEntries;
        NumDirEntries = Get16u(DirWithThumbnailPtrs);
        for (de=0;de<NumDirEntries;de++){
            int Tag;
            unsigned char * DirEntry;
            DirEntry = DIR_ENTRY_ADDR(DirWithThumbnailPtrs, de);
            Tag = Get16u(DirEntry);
            if (Tag == TAG_THUMBNAIL_LENGTH){
                // Set length to zero.
                if (Get16u(DirEntry+2) != FMT_ULONG){
                    // non standard format encoding.  Can't do it.
                    ErrNonfatal("Can't remove thumbnail", 0, 0);
                    return 0;
                }
                Put32u(DirEntry+8, 0);
            }                    
        }
    }
    // This is how far the non thumbnail data went.
    return ImageInfo.ThumbnailOffset+8;
}

//--------------------------------------------------------------------------
// Convert exif time to Unix time structure
//--------------------------------------------------------------------------
int Exif2tm(struct tm * timeptr, char * ExifTime)
{
    int a;

    timeptr->tm_wday = -1;

    // Check for format: YYYY:MM:DD HH:MM:SS format.
    // Date and time normally separated by a space, but also seen a ':' there, so
    // skip the middle space with '%*c' so it can be any character.
    timeptr->tm_sec = 0;
    a = sscanf(ExifTime, "%d%*c%d%*c%d%*c%d:%d:%d",
            &timeptr->tm_year, &timeptr->tm_mon, &timeptr->tm_mday,
            &timeptr->tm_hour, &timeptr->tm_min, &timeptr->tm_sec);

    if (a >= 5){
        if (timeptr->tm_year <= 12 && timeptr->tm_mday > 2000 && ExifTime[2] == '.'){
            // LG Electronics VX-9700 seems to encode the date as 'MM.DD.YYYY HH:MM'
            // can't these people read the standard?  At least they left enough room
            // in the header to put an Exif format date in there.
            int tmp;
            tmp = timeptr->tm_year;
            timeptr->tm_year = timeptr->tm_mday;
            timeptr->tm_mday = timeptr->tm_mon;
            timeptr->tm_mon = tmp;
        }

        // Accept five or six parameters.  Some cameras do not store seconds.
        timeptr->tm_isdst = -1;  
        timeptr->tm_mon -= 1;      // Adjust for unix zero-based months 
        timeptr->tm_year -= 1900;  // Adjust for year starting at 1900 
        return TRUE; // worked. 
    }

    return FALSE; // Wasn't in Exif date format.
}

//--------------------------------------------------------------------------
// Show the collected image info, displaying camera F-stop and shutter speed
// in a consistent and legible fashion.
//--------------------------------------------------------------------------
void ShowImageInfo()
{
	int mhj;
	mhj=sprintf(exifstream,"======= EXIF info =======\n");
    if (ImageInfo.CameraMake[0]){
        mhj+=sprintf(exifstream+mhj,"Camera make  : %s\n",ImageInfo.CameraMake);
        mhj+=sprintf(exifstream+mhj,"Camera model : %s\n",ImageInfo.CameraModel);
    }
    if (ImageInfo.DateTime[0]){
        mhj+=sprintf(exifstream+mhj,"Date/Time    : %s\n",ImageInfo.DateTime);
    }
    mhj+=sprintf(exifstream+mhj,"Resolution   : %d x %d\n",ImageInfo.Width, ImageInfo.Height);
    exifdreh=1;
    if (ImageInfo.Orientation > 1){
        // Only print orientation if one was supplied, and if its not 1 (normal orientation)
        mhj+=sprintf(exifstream+mhj,"Orientation  : %s\n", OrientTab[ImageInfo.Orientation]);
        exifdreh=ImageInfo.Orientation;
    }
    if (ImageInfo.IsColor == 0){
        mhj+=sprintf(exifstream+mhj,"Color/bw     : Black and white\n");
    }
    if (ImageInfo.FlashUsed >= 0){
        if (ImageInfo.FlashUsed & 1){    
            mhj+=sprintf(exifstream+mhj,"Flash used   : Yes");
            switch (ImageInfo.FlashUsed){
	            case 0x5: mhj+=sprintf(exifstream+mhj," (Strobe light not detected)"); break;
	            case 0x7: mhj+=sprintf(exifstream+mhj," (Strobe light detected) "); break;
	            case 0x9: mhj+=sprintf(exifstream+mhj," (manual)"); break;
	            case 0xd: mhj+=sprintf(exifstream+mhj," (manual, return light not detected)"); break;
	            case 0xf: mhj+=sprintf(exifstream+mhj," (manual, return light  detected)"); break;
	            case 0x19:mhj+=sprintf(exifstream+mhj," (auto)"); break;
	            case 0x1d:mhj+=sprintf(exifstream+mhj," (auto, return light not detected)"); break;
	            case 0x1f:mhj+=sprintf(exifstream+mhj," (auto, return light detected)"); break;
	            case 0x41:mhj+=sprintf(exifstream+mhj," (red eye reduction mode)"); break;
	            case 0x45:mhj+=sprintf(exifstream+mhj," (red eye reduction mode return light not detected)"); break;
	            case 0x47:mhj+=sprintf(exifstream+mhj," (red eye reduction mode return light  detected)"); break;
	            case 0x49:mhj+=sprintf(exifstream+mhj," (manual, red eye reduction mode)"); break;
	            case 0x4d:mhj+=sprintf(exifstream+mhj," (manual, red eye reduction mode, return light not detected)"); break;
	            case 0x4f:mhj+=sprintf(exifstream+mhj," (red eye reduction mode, return light detected)"); break;
	            case 0x59:mhj+=sprintf(exifstream+mhj," (auto, red eye reduction mode)"); break;
	            case 0x5d:mhj+=sprintf(exifstream+mhj," (auto, red eye reduction mode, return light not detected)"); break;
	            case 0x5f:mhj+=sprintf(exifstream+mhj," (auto, red eye reduction mode, return light detected)"); break;
            }
        }else{
            mhj+=sprintf(exifstream+mhj,"Flash used   : No");
            switch (ImageInfo.FlashUsed){
	            case 0x18:mhj+=sprintf(exifstream+mhj," (auto)"); break;
            }
        }
        mhj+=sprintf(exifstream+mhj,"\n");
    }
    if (ImageInfo.FocalLength){
        mhj+=sprintf(exifstream+mhj,"Focal length : %4.1fmm",(double)ImageInfo.FocalLength);
        if (ImageInfo.FocalLength35mmEquiv){
            mhj+=sprintf(exifstream+mhj,"  (35mm equivalent: %dmm)", ImageInfo.FocalLength35mmEquiv);
        }
        mhj+=sprintf(exifstream+mhj,"\n");
    }
    if (ImageInfo.DigitalZoomRatio > 1){
        // Digital zoom used.  Shame on you!
        mhj+=sprintf(exifstream+mhj,"Digital Zoom : %1.3fx\n", (double)ImageInfo.DigitalZoomRatio);
    }
    if (ImageInfo.CCDWidth){
        mhj+=sprintf(exifstream+mhj,"CCD width    : %4.2fmm\n",(double)ImageInfo.CCDWidth);
    }
    if (ImageInfo.ExposureTime){
        if (ImageInfo.ExposureTime < 0.010){
            mhj+=sprintf(exifstream+mhj,"Exposure time: %6.4f s ",(double)ImageInfo.ExposureTime);
        }else{
            mhj+=sprintf(exifstream+mhj,"Exposure time: %5.3f s ",(double)ImageInfo.ExposureTime);
        }
        if (ImageInfo.ExposureTime <= 0.5){
            mhj+=sprintf(exifstream+mhj," (1/%d)",(int)(0.5 + 1/ImageInfo.ExposureTime));
        }
        mhj+=sprintf(exifstream+mhj,"\n");
    }
    if (ImageInfo.ApertureFNumber){
        mhj+=sprintf(exifstream+mhj,"Aperture     : f/%3.1f\n",(double)ImageInfo.ApertureFNumber);
    }
    if (ImageInfo.Distance){
        if (ImageInfo.Distance < 0){
            mhj+=sprintf(exifstream+mhj,"Focus dist.  : Infinite\n");
        }else{
            mhj+=sprintf(exifstream+mhj,"Focus dist.  : %4.2fm\n",(double)ImageInfo.Distance);
        }
    }
    if (ImageInfo.ISOequivalent){
        mhj+=sprintf(exifstream+mhj,"ISO equiv.   : %2d\n",(int)ImageInfo.ISOequivalent);
    }
    if (ImageInfo.ExposureBias){
        // If exposure bias was specified, but set to zero, presumably its no bias at all,
        // so only show it if its nonzero.
        mhj+=sprintf(exifstream+mhj,"Exposure bias: %4.2f\n",(double)ImageInfo.ExposureBias);
    }
    switch(ImageInfo.Whitebalance) {
        case 1:
            mhj+=sprintf(exifstream+mhj,"Whitebalance : Manual\n");
            break;
        case 0:
            mhj+=sprintf(exifstream+mhj,"Whitebalance : Auto\n");
            break;
    }
    //Quercus: 17-1-2004 Added LightSource, some cams return this, whitebalance or both
    switch(ImageInfo.LightSource) {
        case 1:
            mhj+=sprintf(exifstream+mhj,"Light Source : Daylight\n");
            break;
        case 2:
            mhj+=sprintf(exifstream+mhj,"Light Source : Fluorescent\n");
            break;
        case 3:
            mhj+=sprintf(exifstream+mhj,"Light Source : Incandescent\n");
            break;
        case 4:
            mhj+=sprintf(exifstream+mhj,"Light Source : Flash\n");
            break;
        case 9:
            mhj+=sprintf(exifstream+mhj,"Light Source : Fine weather\n");
            break;
        case 11:
            mhj+=sprintf(exifstream+mhj,"Light Source : Shade\n");
            break;
        default:; //Quercus: 17-1-2004 There are many more modes for this, check Exif2.2 specs
            // If it just says 'unknown' or we don't know it, then
            // don't bother showing it - it doesn't add any useful information.
    }
    if (ImageInfo.MeteringMode){ // 05-jan-2001 vcs
        switch(ImageInfo.MeteringMode) {
        case 2:
            mhj+=sprintf(exifstream+mhj,"Metering Mode: center weight\n");
            break;
        case 3:
            mhj+=sprintf(exifstream+mhj,"Metering Mode: spot\n");
            break;
        case 5:
            mhj+=sprintf(exifstream+mhj,"Metering Mode: matrix\n");
            break;
        }
    }
    if (ImageInfo.ExposureProgram){ // 05-jan-2001 vcs
        switch(ImageInfo.ExposureProgram) {
        case 1:
            mhj+=sprintf(exifstream+mhj,"Exposure     : Manual\n");
            break;
        case 2:
            mhj+=sprintf(exifstream+mhj,"Exposure     : program (auto)\n");
            break;
        case 3:
            mhj+=sprintf(exifstream+mhj,"Exposure     : aperture priority (semi-auto)\n");
            break;
        case 4:
            mhj+=sprintf(exifstream+mhj,"Exposure     : shutter priority (semi-auto)\n");
            break;
        case 5:
            mhj+=sprintf(exifstream+mhj,"Exposure     : Creative Program (based towards depth of field)\n"); 
            break;
        case 6:
            mhj+=sprintf(exifstream+mhj,"Exposure     : Action program (based towards fast shutter speed)\n");
            break;
        case 7:
            mhj+=sprintf(exifstream+mhj,"Exposure     : Portrait Mode\n");
            break;
        case 8:
            mhj+=sprintf(exifstream+mhj,"Exposure     : LandscapeMode \n");
            break;
        default:
            break;
        }
    }
    switch(ImageInfo.ExposureMode){
        case 0: // Automatic (not worth cluttering up output for)
            break;
        case 1: mhj+=sprintf(exifstream+mhj,"Exposure Mode: Manual\n");
            break;
        case 2: mhj+=sprintf(exifstream+mhj,"Exposure Mode: Auto bracketing\n");
            break;
    }
    if (ImageInfo.DistanceRange) {
        mhj+=sprintf(exifstream+mhj,"Focus range  : ");
        switch(ImageInfo.DistanceRange) {
            case 1:
                mhj+=sprintf(exifstream+mhj,"macro");
                break;
            case 2:
                mhj+=sprintf(exifstream+mhj,"close");
                break;
            case 3:
                mhj+=sprintf(exifstream+mhj,"distant");
                break;
        }
        mhj+=sprintf(exifstream+mhj,"\n");
    }
    if (ImageInfo.Process != M_SOF0){
        // don't show it if its the plain old boring 'baseline' process, but do
        // show it if its something else, like 'progressive' (used on web sometimes)
        int a;
        for (a=0;;a++){
            if (a >= PROCESS_TABLE_SIZE){
                // ran off the end of the table.
                mhj+=sprintf(exifstream+mhj,"Jpeg process : Unknown\n");
                break;
            }
            if (ProcessTable[a].Tag == ImageInfo.Process){
                mhj+=sprintf(exifstream+mhj,"Jpeg process : %s\n",ProcessTable[a].Desc);
                break;
            }
        }
    }
    if (ImageInfo.GpsInfoPresent){
        mhj+=sprintf(exifstream+mhj,"GPS Latitude : %s\n",ImageInfo.GpsLat);
        mhj+=sprintf(exifstream+mhj,"GPS Longitude: %s\n",ImageInfo.GpsLong);
        if (ImageInfo.GpsAlt[0]) mhj+=sprintf(exifstream+mhj,"GPS Altitude : %s\n",ImageInfo.GpsAlt);
    }
    // Print the comment. Print 'Comment:' for each new line of comment.
    if (ImageInfo.Comments[0]){
        int a,c;
        mhj+=sprintf(exifstream+mhj,"Comment      : ");
        if (!ImageInfo.CommentWidchars){
			for (a=0;a<MAX_COMMENT_SIZE;a++){
				c = ImageInfo.Comments[a];
				if (c == '\0') break;
				if (c == '\n'){
					// Do not start a new line if the string ends with a carriage return.
					if (ImageInfo.Comments[a+1] != '\0'){
						mhj+=sprintf(exifstream+mhj,"\nComment      : ");
					}else{
						mhj+=sprintf(exifstream+mhj,"\n");
					}
				}else{
					putchar(c);
				}
			}
			mhj+=sprintf(exifstream+mhj,"\n");
        }
		else {
            mhj+=sprintf(exifstream+mhj,"%.*ls\n", ImageInfo.CommentWidchars, (wchar_t *)ImageInfo.Comments);
		}
    }
}