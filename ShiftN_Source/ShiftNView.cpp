/*

Copyright Marcus Hebel 1998-2010
http://www.shiftn.de
mhebel@gmx.de

This file is part of ShiftN 4.0.

ShiftN 4.0 is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, version 3 of the License.

ShiftN 4.0 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with ShiftN 4.0. If not, see <http://www.gnu.org/licenses/lgpl-3.0.txt>.

*/

/*

EXIF handling of jpg files is done with jhead by Matthias Wandel
http://www.sentex.net/~mwandel/

This software is based in part on the work of the Independent
JPEG Group (JPEG library).

ShiftN incorporates the libraries "libtiff" and "zlib" for
processing and manipulating TIFF images.

Line detection is based on: J. Brian Burns, Allen R. Hanson, Edward
M. Riseman: "Extracting Straight Lines", IEEE Transactions on Pattern
Analysis and Machine Intelligence, Vol. PAMI-8, No. 4, July 1986.

*/

#include "stdafx.h"
#include "ShiftN.h"
#include "ShiftNDoc.h"
#include "ShiftNView.h"
#include "Ladefehler_Dialog.h"
#include "Keine_Ergebnisse.h"
#include "Kein_Bild.h"
#include "abspDlg.h"
#include "DarstDlg.h"
#include "ParamDlg.h"
#include "SchaltflDlg.h"
#include "ProgDlg.h"
#include "SprachDlg.h"
#include "AdjustDlg.h"
#include "mystack.h"
#include <direct.h>
#include <math.h>
#include <string>
#include <fstream>
#include <afxole.h>
extern "C" {
#include "libjpeg/jpeglib.h"
}
#include "libtiff/libtiff/tiffio.h"
#include "setjmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#include <process.h>
#include <io.h>
#include <sys/utime.h>

#include "jhead.h"

static int FilesMatched;
static int FileSequence;
static const char * CurrentFile;
static const char * progname;   // program name for error messages
//--------------------------------------------------------------------------
// Command line options flags
static int TrimExif = FALSE;        // Cut off exif beyond interesting data.
static int RenameToDate = FALSE;
static int RenameAssociatedFiles = FALSE;
static char * strftime_args = NULL; // Format for new file name.
static int Exif2FileTime  = FALSE;
static int DoModify     = FALSE;
static int DoReadAction = FALSE;
static char * FilterModel = NULL;
static int    ExifOnly    = FALSE;
static int    PortraitOnly = FALSE;
static time_t ExifTimeAdjust = 0;   // Timezone adjust
static time_t ExifTimeSet = 0;      // Set exif time to a value.
static char DateSet[11];
static unsigned DateSetChars = 0;
static int DeleteComments = FALSE;
static int DeleteExif = FALSE;
static int DeleteIptc = FALSE;
static int DeleteXmp = FALSE;
static int DeleteUnknown = FALSE;
static char * ThumbSaveName = NULL; // If not NULL, use this string to make up
                                    // the filename to store the thumbnail to.
static char * ThumbInsertName = NULL; // If not NULL, use this string to make up
                                    // the filename to retrieve the thumbnail from.
static char * ExifXferScrFile = NULL;// Extract Exif header from this file, and
                                    // put it into the Jpegs processed.
static int SupressNonFatalErrors = FALSE; // Wether or not to pint warnings on recoverable errors
static int AutoRotate = FALSE;

JSAMPLE *image_buffer;
int image_height;
int image_width;

using namespace std;

CString m_dateiname;
CString m_sdateiname;
const int layer_c_intern=6;
const int layer_l_intern=2;
const double mypi=3.14159265358979323846264338328;
const double twopi=6.28318530717958647692528676656;

char *exifstream;
char *iptcstream;
JOCTET *icc_data_ptr;
unsigned int icc_data_len=0;
char ***bild;
BYTE *ebild;
BYTE *sbild;
BYTE *gbild;
BYTE *mbild;
BYTE *nbild;
BYTE *clipbild;
uint16 *tiff16r;
uint16 *tiff16n;
int *lookauf;
int *lookdrei;
int *mlookauf;
int *mlookdrei;
int mfaktor=1;
int relyexifori=0;
int parlot=0;
int exifdreh=1;
int batchlot=0;
int commandfocus=0;
long backrgb=0;
int usebackrgb=0;
int customgitter=0;
double customgitterx=0;
double customgittery=0;
int max_alloc;
int mhshowgui=1;
int mu3au;
int beforeafter=0;
long ***l_schicht;
long *struktur;
long countp[2];
long maxp[2];
long kp[2];
long count3p[2];
long mitte_offset;
char maske[8];
char ersatzmaske[8];
char **batchname;
int inivorhanden=0;
int tiff16_vorhanden=0;
int batch_voll=0;
int batch_anzahl=0;
int homo_vorhanden=0;
char buff[512];
int boolsch[8];
int x_global=0;
int y_global=0;
int mx_global=0;
int my_global=0;
int direkt_laden_clipboard=0;
int dorot=1;
int use_exif_date=1;
int speicher_voll=0;
int saveexit=1;
int exifstream_vorhanden=0;
int iptcstream_vorhanden=0;
int direkt_speichern_clipboard=0;
boolean iccstream_vorhanden=FALSE;
int ergebnis_vorhanden=0;
int ergvo=0;
int struvo=0;
long struktur_vorhanden=0;
int rgb[]={30,59,11};
int anzpart=10;
int glatt=1;
int direkt_laden;
int direkt_speichern;
int beschneiden=1;
int automatik_f=0;
int sprach=1;
int minlen=0;
int minlen2=minlen;
int mincon=3;
int minsize=0;
int nachbar=0;
int headersize=15;
int jpegquality=100;
int dpisettingx=300;
int dpisettingy=300;
int bilderart=1;
int bilderart2=0;
int schaerfen=1;
int selektion_vorhanden=0;
int afo=0;
int afu=1;
int afl=0;
int afr=1;
HANDLE *hCompleted;
int ist_jpeg=0;
int showgrid=0;
int kubik=1;
int max_statt_ip;
int max_statt_jp;
int yfp;
int y_maxp;
int respara=1200;
double ifak2p;
int x_minp;
double fkop;
double diparamp;
double xmp;
double ymp;
CRect m_rect=CRect(0,0,1,1);
double kf=1;
double shiftamount=1.0;
double ***Homograph;
double schaerfungsparameter=1.3;
int **grauwerte;
double mingrad=1.5;
double minsup=0.6;
double max_disp=0.5;
double distortion=0.0;
double reset_distortion=0.0;
double reset_param=0.0;
double reset_winkel=0.0;
double reset_horizontal=0.0;
double wstep;
double horifac=0.0;
double horifac_fest=0.0;
double distortion_fest=0.0;
double globwinkel=0.0;
double globparam=0.0;
int f_global=28;
int f_fest=28;
double sqmingrad=4*mingrad*mingrad;
ofstream ausgabedatei;
CString m_suffix = _T("_ShiftN");
CString startFolder = _T("");
COleDropTarget DragDrop;

int selection_mouse = 0;
boolean selection_rect_prec = false;
int selection_X1 = 0;
int selection_Y1 = 0;
int selection_X2 = 0;
int selection_Y2 = 0;
CRect selection_rect = CRect(0, 0, 1, 1);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShiftNView

IMPLEMENT_DYNCREATE(CShiftNView, CView)

BEGIN_MESSAGE_MAP(CShiftNView, CView)
	//{{AFX_MSG_MAP(CShiftNView)
	ON_COMMAND(MH_SAVE, OnSave)
	ON_COMMAND(ID_DATEI_SPEICHERNUNTER, OnSaveAs)
	ON_COMMAND(ID_SAVE_CLOSE, OnSaveExit)
	ON_COMMAND(MH_LOAD, OnLoad)
	ON_COMMAND(MH_PARAM, OnParam)
	ON_COMMAND(ID_CTRLC, OnCtrlC)
	ON_COMMAND(ID_CTRLV, OnCtrlV)
	ON_COMMAND(MH_Go, OnGo)
	ON_COMMAND(ID_GITTER, OnGitter)
	ON_COMMAND(MH_Re, Reoptimize)
	ON_COMMAND(MH_BATCH, OnBatch)
	ON_COMMAND(ID_DELMH, OnDelMH)
	ON_COMMAND(MH_DARST, OnDarst)
	ON_COMMAND(ID_EINSTELLUNGEN_SCHALTFLCHESPEICHERN, OnSchaltfl)
	ON_COMMAND(ID_OPTIONS_LANGUAGE, OnLang)
	ON_COMMAND(ID_MHUP, OnUpMH)
	ON_COMMAND(ID_MHDOWN, OnDownMH)
	ON_COMMAND(ID_ANWENDEN_KORREKTURANPASSEN, OnAnwendenKorrekturanpassen)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_UPDATE_COMMAND_UI(ID_GITTER, OnUpdateGitter)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShiftNView Konstruktion/Destruktion

CShiftNView::CShiftNView()
{
}

CShiftNView::~CShiftNView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CShiftNView Zeichnen


// BEGIN jhead section by Matthias Wandel
// http://www.sentex.net/~mwandel/

#define ICC_MARKER  (JPEG_APP0 + 2)	/* JPEG marker code for ICC */
#define ICC_OVERHEAD_LEN  14		/* size of non-profile data in APP2 */
#define MAX_BYTES_IN_MARKER  65533	/* maximum data len of a JPEG marker */
#define MAX_DATA_BYTES_IN_MARKER  (MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)


void write_icc_profile (j_compress_ptr cinfo, const JOCTET *icc_ptr, unsigned int icc_len) {

  unsigned int num_markers;	/* total number of markers we'll write */
  int cur_marker = 1;		/* per spec, counting starts at 1 */
  unsigned int length;		/* number of bytes to write in this marker */

  /* Calculate the number of markers we'll need, rounding up of course */
  num_markers = icc_len / MAX_DATA_BYTES_IN_MARKER;
  if (num_markers * MAX_DATA_BYTES_IN_MARKER != icc_len)
    num_markers++;

  while (icc_len > 0) {
    /* length of profile to put in this marker */
    length = icc_len;
    if (length > MAX_DATA_BYTES_IN_MARKER)
      length = MAX_DATA_BYTES_IN_MARKER;
    icc_len -= length;

    /* Write the JPEG marker header (APP2 code and marker length) */
    jpeg_write_m_header(cinfo, ICC_MARKER,
			(unsigned int) (length + ICC_OVERHEAD_LEN));

    /* Write the marker identifying string "ICC_PROFILE" (null-terminated).
     * We code it in this less-than-transparent way so that the code works
     * even if the local character set is not ASCII.
     */
    jpeg_write_m_byte(cinfo, 0x49);
    jpeg_write_m_byte(cinfo, 0x43);
    jpeg_write_m_byte(cinfo, 0x43);
    jpeg_write_m_byte(cinfo, 0x5F);
    jpeg_write_m_byte(cinfo, 0x50);
    jpeg_write_m_byte(cinfo, 0x52);
    jpeg_write_m_byte(cinfo, 0x4F);
    jpeg_write_m_byte(cinfo, 0x46);
    jpeg_write_m_byte(cinfo, 0x49);
    jpeg_write_m_byte(cinfo, 0x4C);
    jpeg_write_m_byte(cinfo, 0x45);
    jpeg_write_m_byte(cinfo, 0x0);

    /* Add the sequencing info */
    jpeg_write_m_byte(cinfo, cur_marker);
    jpeg_write_m_byte(cinfo, (int) num_markers);

    /* Add the profile data */
    while (length--) {
      jpeg_write_m_byte(cinfo, *icc_ptr);
      icc_ptr++;
    }
    cur_marker++;
  }
}

void setup_read_icc_profile (j_decompress_ptr cinfo)
{
  /* Tell the library to keep any APP2 data it may find */
  jpeg_save_markers(cinfo, ICC_MARKER, 0xFFFF);
}

static boolean marker_is_icc (jpeg_saved_marker_ptr marker)
{
  return
    marker->marker == ICC_MARKER &&
    marker->data_length >= ICC_OVERHEAD_LEN &&
    /* verify the identifying string */
    GETJOCTET(marker->data[0]) == 0x49 &&
    GETJOCTET(marker->data[1]) == 0x43 &&
    GETJOCTET(marker->data[2]) == 0x43 &&
    GETJOCTET(marker->data[3]) == 0x5F &&
    GETJOCTET(marker->data[4]) == 0x50 &&
    GETJOCTET(marker->data[5]) == 0x52 &&
    GETJOCTET(marker->data[6]) == 0x4F &&
    GETJOCTET(marker->data[7]) == 0x46 &&
    GETJOCTET(marker->data[8]) == 0x49 &&
    GETJOCTET(marker->data[9]) == 0x4C &&
    GETJOCTET(marker->data[10]) == 0x45 &&
    GETJOCTET(marker->data[11]) == 0x0;
}

boolean read_icc_profile (j_decompress_ptr cinfo)
{
  jpeg_saved_marker_ptr marker;
  int num_markers = 0;
  int seq_no;
  JOCTET *icc_data;
  unsigned int total_length;
  #define MAX_SEQ_NO  255		/* sufficient since marker numbers are bytes */
  char marker_present[MAX_SEQ_NO+1];	  /* 1 if marker found */
  unsigned int data_length[MAX_SEQ_NO+1]; /* size of profile data in marker */
  unsigned int data_offset[MAX_SEQ_NO+1]; /* offset for data in marker */

  icc_data_ptr = NULL;		/* avoid confusion if FALSE return */
  icc_data_len = 0;

  /* This first pass over the saved markers discovers whether there are
   * any ICC markers and verifies the consistency of the marker numbering.
   */

  for (seq_no = 1; seq_no <= MAX_SEQ_NO; seq_no++)
    marker_present[seq_no] = 0;

  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    if (marker_is_icc(marker)) {
      if (num_markers == 0)
	num_markers = GETJOCTET(marker->data[13]);
      else if (num_markers != GETJOCTET(marker->data[13]))
	return FALSE;		/* inconsistent num_markers fields */
      seq_no = GETJOCTET(marker->data[12]);
      if (seq_no <= 0 || seq_no > num_markers)
	return FALSE;		/* bogus sequence number */
      if (marker_present[seq_no])
	return FALSE;		/* duplicate sequence numbers */
      marker_present[seq_no] = 1;
      data_length[seq_no] = marker->data_length - ICC_OVERHEAD_LEN;
    }
  }

  if (num_markers == 0)
    return FALSE;

  /* Check for missing markers, count total space needed,
   * compute offset of each marker's part of the data.
   */

  total_length = 0;
  for (seq_no = 1; seq_no <= num_markers; seq_no++) {
    if (marker_present[seq_no] == 0)
      return FALSE;		/* missing sequence number */
    data_offset[seq_no] = total_length;
    total_length += data_length[seq_no];
  }

  if (total_length <= 0)
    return FALSE;		/* found only empty markers? */

  /* Allocate space for assembled data */
  icc_data = (JOCTET *) malloc(total_length * sizeof(JOCTET));
  if (icc_data == NULL)
    return FALSE;		/* oops, out of memory */

  /* and fill it in */
  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    if (marker_is_icc(marker)) {
      JOCTET FAR *src_ptr;
      JOCTET *dst_ptr;
      unsigned int length;
      seq_no = GETJOCTET(marker->data[12]);
      dst_ptr = icc_data + data_offset[seq_no];
      src_ptr = marker->data + ICC_OVERHEAD_LEN;
      length = data_length[seq_no];
      while (length--) {
	*dst_ptr++ = *src_ptr++;
      }
    }
  }
  icc_data_ptr=icc_data;
  icc_data_len=total_length;
  return TRUE;
}

void reset_jhead() {
	TrimExif = FALSE;        // Cut off exif beyond interesting data.
	RenameToDate = FALSE;
	RenameAssociatedFiles = FALSE;
	strftime_args = NULL; // Format for new file name.
	Exif2FileTime  = FALSE;
	DoModify     = FALSE;
	DoReadAction = FALSE;
	FilterModel = NULL;
	ExifOnly    = FALSE;
	PortraitOnly = FALSE;
	ExifTimeAdjust = 0;   // Timezone adjust
	ExifTimeSet = 0;      // Set exif time to a value.
	DateSetChars = 0;
	DeleteComments = FALSE;
	DeleteExif = FALSE;
	DeleteIptc = FALSE;
	DeleteUnknown = FALSE;
	ThumbSaveName = NULL; // If not NULL, use this string to make up
										// the filename to store the thumbnail to.
	ThumbInsertName = NULL; // If not NULL, use this string to make up
										// the filename to retrieve the thumbnail from.
	ExifXferScrFile = NULL;// Extract Exif header from this file, and
										// put it into the Jpegs processed.
	SupressNonFatalErrors = FALSE; // Wether or not to pint warnings on recoverable errors
	AutoRotate = FALSE;
}

//--------------------------------------------------------------------------
// Error exit handler
//--------------------------------------------------------------------------
void ErrFatal(char * msg)
{
    exit(EXIT_FAILURE);
}

//--------------------------------------------------------------------------
// Report non fatal errors.  Now that microsoft.net modifies exif headers,
// there's corrupted ones, and there could be more in the future.
//--------------------------------------------------------------------------
void ErrNonfatal(char * msg, int a1, int a2)
{
    if (SupressNonFatalErrors) return;
}

//--------------------------------------------------------------------------
// check if this file should be skipped based on contents.
//--------------------------------------------------------------------------
static int CheckFileSkip(void)
{
    // I sometimes add code here to only process images based on certain
    // criteria - for example, only to convert non progressive Jpegs to progressives, etc..
    if (FilterModel){
        // Filtering processing by camera model.
        // This feature is useful when pictures from multiple cameras are colated, 
        // the its found that one of the cameras has the time set incorrectly.
        if (strstr(ImageInfo.CameraModel, FilterModel) == NULL){
            // Skip.
            return TRUE;
        }
    }
    if (ExifOnly){
        // Filtering by EXIF only.  Skip all files that have no Exif.
        if (FindSection(M_EXIF) == NULL){
            return TRUE;
        }
    }
    if (PortraitOnly == 1){
        if (ImageInfo.Width > ImageInfo.Height) return TRUE;
    }
    if (PortraitOnly == -1){
        if (ImageInfo.Width < ImageInfo.Height) return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------------------
// Subsititute original name for '&i' if present in specified name.
// This to allow specifying relative names when manipulating multiple files.
//--------------------------------------------------------------------------
static void RelativeName(char * OutFileName, const char * NamePattern, const char * OrigName)
{
    // If the filename contains substring "&i", then substitute the 
    // filename for that.  This gives flexibility in terms of processing
    // multiple files at a time.
    char * Subst;
    Subst = (char *) strstr(NamePattern, "&i");
    if (Subst){
        strncpy(OutFileName, NamePattern, Subst-NamePattern);
        OutFileName[Subst-NamePattern] = 0;
        strncat(OutFileName, OrigName, PATH_MAX);
        strncat(OutFileName, Subst+2, PATH_MAX);
    }else{
        strncpy(OutFileName, NamePattern, PATH_MAX); 
    }
}

//--------------------------------------------------------------------------
// Rename associated files
//--------------------------------------------------------------------------
void RenameAssociated(const char * FileName, char * NewBaseName)
{
    int a;
    int PathLen;
    int ExtPos;
    char FilePattern[_MAX_PATH+1];
    char NewName[_MAX_PATH+1];
    struct _finddata_t finddata;
    long find_handle;
    for(ExtPos = strlen(FileName);FileName[ExtPos-1] != '.';){
        if (--ExtPos == 0) return; // No extension!
    }
    memcpy(FilePattern, FileName, ExtPos);
    FilePattern[ExtPos] = '*';
    FilePattern[ExtPos+1] = '\0';
    for(PathLen = strlen(FileName);FileName[PathLen-1] != '\\';){
        if (--PathLen == 0) break;
    }
    find_handle = _findfirst(FilePattern, &finddata);
    for (;;){
        if (find_handle == -1) break;
        // Eliminate the obvious patterns.
        if (!memcmp(finddata.name, ".",2)) goto next_file;
        if (!memcmp(finddata.name, "..",3)) goto next_file;
        if (finddata.attrib & _A_SUBDIR) goto next_file;
        strncpy(FilePattern+PathLen, finddata.name, PATH_MAX-PathLen); // full name with path

        strcpy(NewName, NewBaseName);
        for(a = strlen(finddata.name);finddata.name[a] != '.';){
            if (--a == 0) goto next_file;
        }
        strncat(NewName, finddata.name+a, _MAX_PATH-strlen(NewName)); // add extension to new name
        rename(FilePattern, NewName);
        next_file:
        if (_findnext(find_handle, &finddata) != 0) break;
    }
    _findclose(find_handle);
}

//--------------------------------------------------------------------------
// Handle renaming of files by date.
//--------------------------------------------------------------------------
static void DoFileRenaming(const char * FileName)
{
    int NumAlpha = 0;
    int NumDigit = 0;
    int PrefixPart = 0;
    int ExtensionPart = strlen(FileName);
    int a;
    struct tm tm;
    char NewBaseName[PATH_MAX*2];
    int AddLetter = 0;
    for (a=0;FileName[a];a++){
        if (FileName[a] == '/' || FileName[a] == '\\'){
            // Don't count path component.
            NumAlpha = 0;
            NumDigit = 0;
            PrefixPart = a+1;
        }
        if (FileName[a] == '.') ExtensionPart = a;  // Remember where extension starts.
        if (isalpha(FileName[a])) NumAlpha += 1;    // Tally up alpha vs. digits to judge wether to rename.
        if (isdigit(FileName[a])) NumDigit += 1;
    }
    if (RenameToDate <= 1){
        // If naming isn't forced, ensure name is mostly digits, or leave it alone.
        if (NumAlpha > 8 || NumDigit < 4){
            return;
        }
    }
    if (!Exif2tm(&tm, ImageInfo.DateTime)){
        // Use file date/time instead.
        tm = *localtime(&ImageInfo.FileDateTime);
    }
    strcpy(NewBaseName, FileName); // Get path component of name.
    if (strftime_args){
        // Complicated scheme for flexibility.  Just pass the args to strftime.
        time_t UnixTime;
        char *s;
        char pattern[PATH_MAX+20];
        int n = ExtensionPart - PrefixPart;
        // Call mktime to get weekday and such filled in.
        UnixTime = mktime(&tm);
        if ((int)UnixTime == -1){
            return;
        }
        // Substitute "%f" for the original name (minus path & extension)
        pattern[PATH_MAX-1]=0;
        strncpy(pattern, strftime_args, PATH_MAX-1);
        while ((s = strstr(pattern, "%f")) && strlen(pattern) + n < PATH_MAX-1){
            memmove(s + n, s + 2, strlen(s+2) + 1);
            memmove(s, FileName + PrefixPart, n);
        }
        {
            // Sequential number renaming part.  
            // '%i' type pattern becomes sequence number.
            int ppos = -1;
            for (a=0;pattern[a];a++){
                if (pattern[a] == '%'){
                     ppos = a;
                }else if (pattern[a] == 'i'){
                    if (ppos >= 0 && a<ppos+4){
                        // Replace this part with a number.
                        char pat[8], num[16];
                        int l,nl;
                        memcpy(pat, pattern+ppos, 4);
                        pat[a-ppos] = 'd'; // Replace 'i' with 'd' for '%d'
                        pat[a-ppos+1] = '\0';
                        sprintf(num, pat, FileSequence); // let printf do the number formatting.
                        nl = strlen(num);
                        l = strlen(pattern+a+1);
                        if (ppos+nl+l+1 >= PATH_MAX) ErrFatal("str overflow");
                        memmove(pattern+ppos+nl, pattern+a+1, l+1);
                        memcpy(pattern+ppos, num, nl);
                        break;
                    }
                }else if (!isdigit(pattern[a])){
                    ppos = -1;
                }
            }
        }
        strftime(NewBaseName+PrefixPart, PATH_MAX, pattern, &tm);
    }else{
        // My favourite scheme.
        sprintf(NewBaseName+PrefixPart, "%02d%02d-%02d%02d%02d",
             tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    AddLetter = isdigit(NewBaseName[strlen(NewBaseName)-1]);
    for (a=0;;a++){
        char NewName[PATH_MAX+10];
        char NameExtra[3];
        struct stat dummy;
        if (a){
            // Generate a suffix for the file name if previous choice of names is taken.
            // depending on wether the name ends in a letter or digit, pick the opposite to separate
            // it.  This to avoid using a separator character - this because any good separator
            // is before the '.' in ascii, and so sorting the names would put the later name before
            // the name without suffix, causing the pictures to more likely be out of order.
            if (AddLetter){
                NameExtra[0] = (char)('a'-1+a); // Try a,b,c,d... for suffix if it ends in a letter.
            }else{
                NameExtra[0] = (char)('0'-1+a); // Try 1,2,3,4... for suffix if it ends in a char.
            }
            NameExtra[1] = 0;
        }else{
            NameExtra[0] = 0;
        }
        sprintf(NewName, "%s%s.jpg", NewBaseName, NameExtra);
        if (!strcmp(FileName, NewName)) break; // Skip if its already this name.
        if (stat(NewName, &dummy)){
            // This name does not pre-exist.
            if (rename(FileName, NewName) == 0){
                if (RenameAssociatedFiles){
                    sprintf(NewName, "%s%s", NewBaseName, NameExtra);
                    RenameAssociated(FileName, NewName);
                }
            }
            break;
        }
        if (a > 25 || (!AddLetter && a > 9)){
            break;
        }
    }
}

//--------------------------------------------------------------------------
// Rotate the image and its thumbnail
//--------------------------------------------------------------------------
static int DoAutoRotate(const char * FileName)
{
    if (ImageInfo.Orientation >= 2 && ImageInfo.Orientation <= 8){
        const char * Argument;
        Argument = ClearOrientation();
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------------------
// Do selected operations to one file at a time.
//--------------------------------------------------------------------------
void ProcessFile(const char * FileName)
{
    int Modified = FALSE;
    ReadMode_t ReadMode;

    if (strlen(FileName) >= PATH_MAX-1){
        // Protect against buffer overruns in strcpy / strcat's on filename
        ErrFatal("filename too long");
    }

    ReadMode = READ_METADATA;
    CurrentFile = FileName;
    FilesMatched = 1; 
    ResetJpgfile();
    // Start with an empty image information structure.
    memset(&ImageInfo, 0, sizeof(ImageInfo));
    ImageInfo.FlashUsed = -1;
    ImageInfo.MeteringMode = -1;
    ImageInfo.Whitebalance = -1;
    // Store file date/time.
    {
        struct stat st;
        if (stat(FileName, &st) >= 0){
            ImageInfo.FileDateTime = st.st_mtime;
            ImageInfo.FileSize = st.st_size;
        }else{
            ErrFatal("No such file");
        }
    }
    if (DoModify || RenameToDate || Exif2FileTime){
        if (_access(FileName, 2 /*W_OK*/)){
            return;
        }
    }
    strncpy(ImageInfo.FileName, FileName, PATH_MAX);
    if (AutoRotate){
        // Applying a command is special - the headers from the file have to be
        // pre-read, then the command executed, and then the image part of the file read.
        if (!ReadJpegFile(FileName, READ_METADATA)) return;
        if (CheckFileSkip()){
            DiscardData();
            return;
        }
        DiscardAllButExif();
        if (DoAutoRotate(FileName)) {
            Modified = TRUE;
        }
        ReadMode = READ_IMAGE;   // Don't re-read exif section again on next read.
    }else if (ExifXferScrFile){
        char RelativeExifName[PATH_MAX+1];
        // Make a relative name.
        RelativeName(RelativeExifName, ExifXferScrFile, FileName);
        if(!ReadJpegFile(RelativeExifName, READ_METADATA)) return;
        DiscardAllButExif();    // Don't re-read exif section again on next read.
        Modified = TRUE;
        ReadMode = READ_IMAGE;
    }
    if (DoModify){
        ReadMode = (ReadMode_t) (ReadMode | READ_IMAGE);
    }
    if (!ReadJpegFile(FileName, ReadMode)) return;
    if (CheckFileSkip()){
        DiscardData();
        return;
    }
    FileSequence += 1; // Count files processed.
    if (!(DoModify || DoReadAction)){
        ShowImageInfo();
        {
            // if IPTC section is present, show it also.
            Section_t * IptcSection;
            IptcSection = FindSection(M_IPTC);
            if (IptcSection){
                show_IPTC(IptcSection->Data, IptcSection->Size);
            }
        }
    }
    if (ThumbSaveName){
        char OutFileName[PATH_MAX+1];
        // Make a relative name.
        RelativeName(OutFileName, ThumbSaveName, FileName);
        SaveThumbnail(OutFileName);
    }
    if (ThumbInsertName){
        char ThumbFileName[PATH_MAX+1];
        // Make a relative name.
        RelativeName(ThumbFileName, ThumbInsertName, FileName);
        if (ReplaceThumbnail(ThumbFileName)){
            Modified = TRUE;
        }
    }else if (TrimExif){
        // Deleting thumbnail is just replacing it with a null thumbnail.
        if (ReplaceThumbnail(NULL)){
            Modified = TRUE;
        }
    }
    if (ExifTimeAdjust || ExifTimeSet || DateSetChars){
       if (ImageInfo.numDateTimeTags){
            struct tm tm;
            time_t UnixTime;
            char TempBuf[50];
            int a;
            Section_t * ExifSection;
            if (ExifTimeSet){
                // A time to set was specified.
                UnixTime = ExifTimeSet;
            }else{
                if (DateSetChars){
                    memcpy(ImageInfo.DateTime, DateSet, DateSetChars);
                    a = 1970;
                    sscanf(DateSet, "%d", &a);
                    if (a < 1970){
                        strcpy(TempBuf, ImageInfo.DateTime);
                        goto skip_unixtime;
                    }
                }
                // A time offset to adjust by was specified.
                if (!Exif2tm(&tm, ImageInfo.DateTime)) goto badtime;
                // Convert to unix 32 bit time value, add offset, and convert back.
                UnixTime = mktime(&tm);
                if ((int)UnixTime == -1) goto badtime;
                UnixTime += ExifTimeAdjust;
            }
            tm = *localtime(&UnixTime);
            // Print to temp buffer first to avoid putting null termination in destination.
            // snprintf() would do the trick, hbut not available everywhere (like FreeBSD 4.4)
            sprintf(TempBuf, "%04d:%02d:%02d %02d:%02d:%02d",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
skip_unixtime:         
            ExifSection = FindSection(M_EXIF);
            for (a = 0; a < ImageInfo.numDateTimeTags; a++) {
                uchar * Pointer;
                Pointer = ExifSection->Data+ImageInfo.DateTimeOffsets[a]+8;
                memcpy(Pointer, TempBuf, 19);
            }
            memcpy(ImageInfo.DateTime, TempBuf, 19);
            Modified = TRUE;
        }
    }
    if (DeleteComments){
        if (RemoveSectionType(M_COM)) Modified = TRUE;
    }
    if (DeleteExif){
        if (RemoveSectionType(M_EXIF)) Modified = TRUE;
    }
    if (DeleteIptc){
        if (RemoveSectionType(M_IPTC)) Modified = TRUE;
    }
    if (DeleteXmp){
        if (RemoveSectionType(M_XMP)) Modified = TRUE;
    }    
    if (DeleteUnknown){
        if (RemoveUnknownSections()) Modified = TRUE;
    }
    if (Modified){
        char BackupName[PATH_MAX+5];
        struct stat buf;
        strcpy(BackupName, FileName);
        strcat(BackupName, ".t");
        // Remove any .old file name that may pre-exist
        _unlink(BackupName);
        // Rename the old file.
        rename(FileName, BackupName);
        // Write the new file.
        WriteJpegFile(FileName);
        // Copy the access rights from original file
        if (stat(BackupName, &buf) == 0){
            // set Unix access rights and time to new file
            struct utimbuf mtime;
            _chmod(FileName, buf.st_mode);
            mtime.actime = buf.st_mtime;
            mtime.modtime = buf.st_mtime;
            utime(FileName, &mtime);
        }
        // Now that we are done, remove original file.
        _unlink(BackupName);
    }
    if (Exif2FileTime){
        // Set the file date to the date from the exif header.
        if (ImageInfo.numDateTimeTags){
            // Converte the file date to Unix time.
            struct tm tm;
            time_t UnixTime;
            struct utimbuf mtime;
            if (!Exif2tm(&tm, ImageInfo.DateTime)) goto badtime;
            UnixTime = mktime(&tm);
            if ((int)UnixTime == -1){
                goto badtime;
            }
            mtime.actime = UnixTime;
            mtime.modtime = UnixTime;
            utime(FileName, &mtime);
        }
    }
    // Feature to rename image according to date and time from camera.
    // I use this feature to put images from multiple digicams in sequence.
    if (RenameToDate){
        DoFileRenaming(FileName);
    }
    DiscardData();
    return;
badtime:
    DiscardData();
}

//--------------------------------------------------------------------------
// Parse specified date or date+time from command line.
//--------------------------------------------------------------------------
time_t ParseCmdDate(char * DateSpecified)
{
    int a;
    struct tm tm;
    time_t UnixTime;
    tm.tm_wday = -1;
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    a = sscanf(DateSpecified, "%d:%d:%d/%d:%d:%d",
            &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
            &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    if (a != 3 && a < 5){
        // Date must be YYYY:MM:DD, YYYY:MM:DD+HH:MM
        // or YYYY:MM:DD+HH:MM:SS
        ErrFatal("Could not parse specified date");
    }
    tm.tm_isdst = -1;  
    tm.tm_mon -= 1;      // Adjust for unix zero-based months 
    tm.tm_year -= 1900;  // Adjust for year starting at 1900 
    UnixTime = mktime(&tm);
    if (UnixTime == -1){
        ErrFatal("Specified time is invalid or out of range");
    }
    return UnixTime;    
}

// END jhead section by Matthias Wandel
// http://www.sentex.net/~mwandel/

int aufgerundet(int i) {
	int ret=i;
	while (ret%4) ret++;
	return ret;
}

int write_JPEG_file (char * filename, int quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;
	JSAMPROW row_pointer[1];
	int row_stride;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	if ((outfile = fopen(filename, "wb")) == NULL) {
		return 1;
	}
	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.density_unit = 1;
	cinfo.X_density = dpisettingx;
	cinfo.Y_density = dpisettingy;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = image_width*3;
	if (iccstream_vorhanden) {
		write_icc_profile(&cinfo,icc_data_ptr,icc_data_len);
	}
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & image_buffer[cinfo.next_scanline*row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
	FileSequence = 0;
	if (ist_jpeg) {
		ExifXferScrFile = m_dateiname.GetBuffer(m_dateiname.GetLength());
		DoModify = TRUE;
		FileSequence = 0;
		MyGlob(filename, ProcessFile);
		reset_jhead();
		DeleteUnknown = FALSE;
		DoModify = TRUE;
		FileSequence = 0;
		MyGlob(filename, ProcessFile);
		reset_jhead();
		if (use_exif_date) {
			Exif2FileTime = TRUE;
			DoModify = TRUE;
			FileSequence = 0;
			MyGlob(filename, ProcessFile);
			reset_jhead();
		}
		if (exifdreh>1) {
			AutoRotate=1;
			DoModify = TRUE;
			FileSequence = 0;
			MyGlob(filename, ProcessFile);
			reset_jhead();
		}
	}
	return FileSequence;
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit(j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

char i_char(int i) {
	char c;
	if (i>255) i=255;
	if (i<0) i=0;
	if (i>127) i-=256;
	c=(char) i;
	return c;
}

void homography(double winkel, double param, int u, int v) {
	int i;
	int j;
	double exppa;
	double fdb;
	double rad;
	double alpha;
	double r;
	double cowi;
	double siwi;
	exppa=exp(param);
	fdb=((double) f_global)/(14.4+(((double) v)/((double) u)-1)*7.2);
	rad=fdb*(exppa-1)/(exppa+1);
	alpha=atan(rad);
	if (alpha>1.5) alpha=1.5;
	if (alpha<-1.5) alpha=-1.5;
	r=sin(0.5*alpha);
	r=2.0*(horifac-1.0)*r*r+1.0;
	if (r<0.1) r=0.1;
	cowi=cos(winkel);
	siwi=sin(winkel);
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			Homograph[2][i][j]=0.0;
			Homograph[3][i][j]=0.0;
			Homograph[0][i][j]=0.0;
		}
	}
	Homograph[0][1][1]=cowi;
	Homograph[0][2][1]=siwi;
	Homograph[0][1][2]=-siwi;
	Homograph[0][2][2]=cowi;
	Homograph[0][1][3]=-0.5*v*cowi+0.5*u*siwi+0.5*v;
	Homograph[0][2][3]=-0.5*v*siwi-0.5*u*cowi+0.5*u;
	Homograph[0][3][3]=1.0;
	Homograph[3][3][1]=(exppa-1)/((double) v);
	Homograph[3][1][1]=exppa;
	Homograph[3][2][1]=0.5*((exppa-1)*u)/((double) v);
	Homograph[3][2][2]=2*exppa/(exppa+1);
	Homograph[3][2][3]=-0.5*((exppa-1)*u)/(exppa+1);
	Homograph[3][3][3]=1.0;
	Homograph[2][1][1]=Homograph[3][1][1]*Homograph[0][1][1]+Homograph[3][1][2]*Homograph[0][2][1]+Homograph[3][1][3]*Homograph[0][3][1];
	Homograph[2][2][1]=Homograph[3][2][1]*Homograph[0][1][1]+Homograph[3][2][2]*Homograph[0][2][1]+Homograph[3][2][3]*Homograph[0][3][1];
	Homograph[2][3][1]=Homograph[3][3][1]*Homograph[0][1][1]+Homograph[3][3][2]*Homograph[0][2][1]+Homograph[3][3][3]*Homograph[0][3][1];
	Homograph[2][1][2]=Homograph[3][1][1]*Homograph[0][1][2]+Homograph[3][1][2]*Homograph[0][2][2]+Homograph[3][1][3]*Homograph[0][3][2];
	Homograph[2][2][2]=Homograph[3][2][1]*Homograph[0][1][2]+Homograph[3][2][2]*Homograph[0][2][2]+Homograph[3][2][3]*Homograph[0][3][2];
	Homograph[2][3][2]=Homograph[3][3][1]*Homograph[0][1][2]+Homograph[3][3][2]*Homograph[0][2][2]+Homograph[3][3][3]*Homograph[0][3][2];
	Homograph[2][1][3]=Homograph[3][1][1]*Homograph[0][1][3]+Homograph[3][1][2]*Homograph[0][2][3]+Homograph[3][1][3]*Homograph[0][3][3];
	Homograph[2][2][3]=Homograph[3][2][1]*Homograph[0][1][3]+Homograph[3][2][2]*Homograph[0][2][3]+Homograph[3][2][3]*Homograph[0][3][3];
	Homograph[2][3][3]=Homograph[3][3][1]*Homograph[0][1][3]+Homograph[3][3][2]*Homograph[0][2][3]+Homograph[3][3][3]*Homograph[0][3][3];
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			Homograph[3][i][j]=0.0;
			Homograph[0][i][j]=0.0;
		}
	}
	Homograph[3][1][1]=1;
	Homograph[3][2][2]=r;
	Homograph[3][3][3]=1;
	Homograph[3][2][3]=0.5*u*(1-r);
	Homograph[0][1][1]=Homograph[3][1][1]*Homograph[2][1][1]+Homograph[3][1][2]*Homograph[2][2][1]+Homograph[3][1][3]*Homograph[2][3][1];
	Homograph[0][2][1]=Homograph[3][2][1]*Homograph[2][1][1]+Homograph[3][2][2]*Homograph[2][2][1]+Homograph[3][2][3]*Homograph[2][3][1];
	Homograph[0][3][1]=Homograph[3][3][1]*Homograph[2][1][1]+Homograph[3][3][2]*Homograph[2][2][1]+Homograph[3][3][3]*Homograph[2][3][1];
	Homograph[0][1][2]=Homograph[3][1][1]*Homograph[2][1][2]+Homograph[3][1][2]*Homograph[2][2][2]+Homograph[3][1][3]*Homograph[2][3][2];
	Homograph[0][2][2]=Homograph[3][2][1]*Homograph[2][1][2]+Homograph[3][2][2]*Homograph[2][2][2]+Homograph[3][2][3]*Homograph[2][3][2];
	Homograph[0][3][2]=Homograph[3][3][1]*Homograph[2][1][2]+Homograph[3][3][2]*Homograph[2][2][2]+Homograph[3][3][3]*Homograph[2][3][2];
	Homograph[0][1][3]=Homograph[3][1][1]*Homograph[2][1][3]+Homograph[3][1][2]*Homograph[2][2][3]+Homograph[3][1][3]*Homograph[2][3][3];
	Homograph[0][2][3]=Homograph[3][2][1]*Homograph[2][1][3]+Homograph[3][2][2]*Homograph[2][2][3]+Homograph[3][2][3]*Homograph[2][3][3];
	Homograph[0][3][3]=Homograph[3][3][1]*Homograph[2][1][3]+Homograph[3][3][2]*Homograph[2][2][3]+Homograph[3][3][3]*Homograph[2][3][3];
}

double betrag(double d) {
	double ret=d;
	if (d<0) ret=(-1.0)*d;
	return ret;
}

int maxstell(int k) {
	int i;
	int s=k;
	for (i=k+1; i<=3; i++)
		if (betrag(Homograph[1][i][k])>=betrag(Homograph[1][s][k])) s=i;
	return s;
}

int ihomography(double winkel, double param, int u, int v) {
	int p[4];
	double b[4];
	int i;
	int j;
	int k;
	int r;
	int n=3;
	double zw;
	double pivot;
	homography(winkel,param,u,v);	
	for (i=1; i<=n; i++) p[i]=i;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			Homograph[1][i][j]=Homograph[0][i][j];
		}
	}
	for (k=1; k<=n; k++) {
		r=maxstell(k);
		if (Homograph[1][r][k]==0) {
			return 1;
		}
		if (k!=r) {
			j=p[r];
			p[r]=p[k];
			p[k]=j;
			for (j=1; j<=n; j++) {
				zw=Homograph[1][k][j];
				Homograph[1][k][j]=Homograph[1][r][j];
				Homograph[1][r][j]=zw;
			}
		}
		pivot=Homograph[1][k][k];
		for (j=1; j<=n; j++) {
			if (j!=k) Homograph[1][k][j]=Homograph[1][k][j]/(1.0*pivot);
			else Homograph[1][k][j]=(1.0)/pivot;
			for (i=1; i<=n; i++)
				if ((i!=k) && (j!=k)) Homograph[1][i][j]=Homograph[1][i][j]-Homograph[1][i][k]*Homograph[1][k][j];
		}
		for (i=1; i<=n; i++)
			if (i!=k) Homograph[1][i][k]=(-1.0)*Homograph[1][i][k]/pivot;
	}
	for (i=1; i<=n; i++) {
		for (j=1; j<=n; j++) b[p[j]]=Homograph[1][i][j];
		for (j=1; j<=n; j++) Homograph[1][i][j]=b[j];
	}
	return 0;
}

void get_xy(long l, int &u, int &v) {
	u=(l>>14);
	v=(l-(u<<14));
}

void verzeichne(double fko, double dparam, double x, double y, double xm, double ym, double &xx, double &yy) {
	double r;
	r=sqrt((x-xm)*(x-xm)+(y-ym)*(y-ym));
	xx=xm+(1+dparam*r*r)*(x-xm)/fko;
	yy=ym+(1+dparam*r*r)*(y-ym)/fko;
}

void entzeichne(int finaldist, double fko, double dparam, double x, double y, double xm, double ym, double &xx, double &yy) {
	double schritt=1.0;
	double r;
	double rf;
	double rff;
	double rneu;
	double eins=1.0/fko;
	double depara=dparam/fko;
	double uschranke;
	int i;
	if (finaldist) uschranke=0.06/((double) mfaktor);
	else uschranke=0.06;
	r=sqrt((x-xm)*(x-xm)+(y-ym)*(y-ym));
	if (r>0.7) {
		rf=r*(1-dparam*r*r)*fko;
		while (schritt>uschranke) {
			i=0;
			do {
				rneu=rf*(eins+depara*rf*rf);
				rf+=schritt;
				i++;
			}
			while ((rneu<r) && (i<10000));
			rf-=schritt;
			i=0;
			do {
				rneu=rf*(eins+depara*rf*rf);
				rf-=schritt;
				i++;
			}
			while ((rneu>r) && (rf>0.1) && (i<10000));
			rf+=schritt;
			schritt*=0.5;
		}
		rf+=schritt;
		rff=rf/r;
		xx=xm+rff*(x-xm);
		yy=ym+rff*(y-ym);
	}
	else {
		xx=xm;
		yy=ym;
	}
}

void fithomography(int xf, int yf, double &optparam, double &optwinkel) {
	const int maxlauf=5;
	double u1;
	double v1;
	double u2;
	double v2;
	int lauf;
	double xs1;
	double xs2;
	double uu1;
	double vv1;
	double uu2;
	double vv2;
	int uuu1;
	int vvv1;
	int uuu2;
	int vvv2;
	int nri=0;
	int nrind=0;
	double k_max;
	double k_max2;
	double mittel1;
	double mittel2;
	double winkel=globwinkel;
	double xm=x_global*0.5;
	double ym=y_global*0.5;
	double fko;
	double param=globparam;
	double diparam;
	double betr;
	double disp_r;
	double r_mol;
	double mindisp_r;
	long k;
	double nr;
	double diffparam=0.005;
	optparam=param;
	optwinkel=winkel;
	diparam=distortion/(x_global*x_global+y_global*y_global);
	if (distortion>=0) {
		if (xm<ym) {
			fko=1+diparam*(xm-1)*(xm-1);
		}
		else {
			fko=1+diparam*(ym-1)*(ym-1);
		}
	}
	else {
		fko=1+diparam*((xm+1)*(xm+1)+(ym+1)*(ym+1));
	}
	for (lauf=1; lauf<=maxlauf; lauf+=1) {
		homography(winkel,param,xf,yf);
		k=0;
		nr=0.0;
		nri=0;
		mittel1=0.0;
		mittel2=0.0;
		while (k<struktur_vorhanden) {
			if (struktur[k]>0) {
				get_xy(struktur[k+13],uuu1,vvv1);
				vvv1=y_global-1-vvv1;
				verzeichne(fko,diparam,uuu1,vvv1,xm,ym,u1,v1);
				vv1=(Homograph[0][1][1]*v1+Homograph[0][1][2]*u1+Homograph[0][1][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
				uu1=(Homograph[0][2][1]*v1+Homograph[0][2][2]*u1+Homograph[0][2][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
				get_xy(struktur[k+14],uuu2,vvv2);
				vvv2=y_global-1-vvv2;
				verzeichne(fko,diparam,uuu2,vvv2,xm,ym,u2,v2);
				vv2=(Homograph[0][1][1]*v2+Homograph[0][1][2]*u2+Homograph[0][1][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
				uu2=(Homograph[0][2][1]*v2+Homograph[0][2][2]*u2+Homograph[0][2][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
				betr=sqrt(sqrt((uu2-uu1)*(uu2-uu1)+(vv2-vv1)*(vv2-vv1)));
				if (fabs(vv2-vv1)>0.5) {
					xs1=uu1+((0-vv1)*(uu2-uu1))/(vv2-vv1);
					xs2=uu1+((yf-vv1)*(uu2-uu1))/(vv2-vv1);
					mittel1+=betr*xs1;
					mittel2+=betr*xs2;
					nr+=betr;
					nri+=1;
				}
			}
			k+=headersize+struktur[k+2]+struktur[k+3];
		}
		if ((nr>0.5) && (nri>1)) {
			nrind=1;
			mittel1/=nr;
			mittel2/=nr;
			winkel=atan2(mittel1-mittel2,yf);
			if (winkel>0.25*max_disp) winkel=0.25*max_disp;
			if (winkel<-0.25*max_disp) winkel=-0.25*max_disp;
		}
		if (!dorot) winkel=0.0;
		optwinkel+=winkel;
		winkel=optwinkel;
		do {
			homography(winkel,param,xf,yf);
			k=0;
			nr=0.0;
			nri=0;
			mittel1=0.0;
			mittel2=0.0;
			while (k<struktur_vorhanden) {
				if (struktur[k]>0) {
					get_xy(struktur[k+13],uuu1,vvv1);
					vvv1=y_global-1-vvv1;
					verzeichne(fko,diparam,uuu1,vvv1,xm,ym,u1,v1);
					vv1=(Homograph[0][1][1]*v1+Homograph[0][1][2]*u1+Homograph[0][1][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
					uu1=(Homograph[0][2][1]*v1+Homograph[0][2][2]*u1+Homograph[0][2][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
					get_xy(struktur[k+14],uuu2,vvv2);
					vvv2=y_global-1-vvv2;
					verzeichne(fko,diparam,uuu2,vvv2,xm,ym,u2,v2);
					vv2=(Homograph[0][1][1]*v2+Homograph[0][1][2]*u2+Homograph[0][1][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
					uu2=(Homograph[0][2][1]*v2+Homograph[0][2][2]*u2+Homograph[0][2][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
					betr=sqrt(sqrt((uu2-uu1)*(uu2-uu1)+(vv2-vv1)*(vv2-vv1)));
					if (fabs(vv2-vv1)>0.5) {
						xs1=uu1+((0-vv1)*(uu2-uu1))/(vv2-vv1);
						xs2=uu1+((yf-vv1)*(uu2-uu1))/(vv2-vv1);
						mittel1+=betr*fabs((xf>>1)-xs1);
						mittel2+=betr*fabs((xf>>1)-xs2);
						nr+=betr;
						nri+=1;
					}
				}
				k+=headersize+struktur[k+2]+struktur[k+3];
			}
			if ((nr>0.5) && (nri>1)) {
				nrind=1;
				mittel1/=nr;
				mittel2/=nr;
			}
			param+=diffparam;
		}
		while ((mittel1<mittel2) && (param<1.4));
		do {
			homography(winkel,param,xf,yf);
			k=0;
			nr=0.0;
			nri=0;
			mittel1=0.0;
			mittel2=0.0;
			while (k<struktur_vorhanden) {
				if (struktur[k]>0) {
					get_xy(struktur[k+13],uuu1,vvv1);
					vvv1=y_global-1-vvv1;
					verzeichne(fko,diparam,uuu1,vvv1,xm,ym,u1,v1);
					vv1=(Homograph[0][1][1]*v1+Homograph[0][1][2]*u1+Homograph[0][1][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
					uu1=(Homograph[0][2][1]*v1+Homograph[0][2][2]*u1+Homograph[0][2][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
					get_xy(struktur[k+14],uuu2,vvv2);
					vvv2=y_global-1-vvv2;
					verzeichne(fko,diparam,uuu2,vvv2,xm,ym,u2,v2);
					vv2=(Homograph[0][1][1]*v2+Homograph[0][1][2]*u2+Homograph[0][1][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
					uu2=(Homograph[0][2][1]*v2+Homograph[0][2][2]*u2+Homograph[0][2][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
					betr=sqrt(sqrt((uu2-uu1)*(uu2-uu1)+(vv2-vv1)*(vv2-vv1)));
					if (fabs(vv2-vv1)>0.5) {
						xs1=uu1+((0-vv1)*(uu2-uu1))/(vv2-vv1);
						xs2=uu1+((yf-vv1)*(uu2-uu1))/(vv2-vv1);
						mittel1+=betr*fabs((xf>>1)-xs1);
						mittel2+=betr*fabs((xf>>1)-xs2);
						nr+=betr;
						nri+=1;
					}
				}
				k+=headersize+struktur[k+2]+struktur[k+3];
			}
			if ((nr>0.5) && (nri>1)) {
				nrind=1;
				mittel1/=nr;
				mittel2/=nr;
			}
			param-=diffparam;
		}
		while ((mittel1>mittel2) && (param>-1.4));
		optparam=param+0.5*diffparam;
		if (lauf<maxlauf) {
			k=0;
			homography(optwinkel,optparam,xf,yf);
			nr=0.0;
			nri=0;
			disp_r=0.0;
			mindisp_r=0.0;
			while (k<struktur_vorhanden) {
				if (struktur[k]>0) {
					get_xy(struktur[k+13],uuu1,vvv1);
					vvv1=y_global-1-vvv1;
					verzeichne(fko,diparam,uuu1,vvv1,xm,ym,u1,v1);
					vv1=(Homograph[0][1][1]*v1+Homograph[0][1][2]*u1+Homograph[0][1][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
					uu1=(Homograph[0][2][1]*v1+Homograph[0][2][2]*u1+Homograph[0][2][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
					get_xy(struktur[k+14],uuu2,vvv2);
					vvv2=y_global-1-vvv2;
					verzeichne(fko,diparam,uuu2,vvv2,xm,ym,u2,v2);
					vv2=(Homograph[0][1][1]*v2+Homograph[0][1][2]*u2+Homograph[0][1][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
					uu2=(Homograph[0][2][1]*v2+Homograph[0][2][2]*u2+Homograph[0][2][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
					betr=(uu2-uu1)*(uu2-uu1)+(vv2-vv1)*(vv2-vv1);
					disp_r+=(uu2-uu1)*(uu2-uu1)/betr;
					nr+=1.0;
					nri+=1;
				}
				k+=headersize+struktur[k+2]+struktur[k+3];
			}
			if ((nr>0.5) && (nri>1)) {
				nrind=1;
				mindisp_r=sqrt(disp_r/((double) nr));
			}
			k_max=nr;
			k_max2=nr;
			r_mol=10.0;
			homography(optwinkel,optparam,xf,yf);
			while ((k_max>10) && (r_mol>2.0) && (k_max>(k_max2*0.8))) { 
				k=0;
				nr=0.0;
				while (k<struktur_vorhanden) {
					if (struktur[k]==-30) struktur[k]=-3;
					if (struktur[k]>0) {
						get_xy(struktur[k+13],uuu1,vvv1);
						vvv1=y_global-1-vvv1;
						verzeichne(fko,diparam,uuu1,vvv1,xm,ym,u1,v1);
						vv1=(Homograph[0][1][1]*v1+Homograph[0][1][2]*u1+Homograph[0][1][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
						uu1=(Homograph[0][2][1]*v1+Homograph[0][2][2]*u1+Homograph[0][2][3])/(Homograph[0][3][1]*v1+Homograph[0][3][2]*u1+Homograph[0][3][3]);
						get_xy(struktur[k+14],uuu2,vvv2);
						vvv2=y_global-1-vvv2;
						verzeichne(fko,diparam,uuu2,vvv2,xm,ym,u2,v2);
						vv2=(Homograph[0][1][1]*v2+Homograph[0][1][2]*u2+Homograph[0][1][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
						uu2=(Homograph[0][2][1]*v2+Homograph[0][2][2]*u2+Homograph[0][2][3])/(Homograph[0][3][1]*v2+Homograph[0][3][2]*u2+Homograph[0][3][3]);
						betr=sqrt((uu2-uu1)*(uu2-uu1)+(vv2-vv1)*(vv2-vv1));
						disp_r=fabs(uu2-uu1)/betr;
						if (disp_r>r_mol*mindisp_r) struktur[k]=-30;
						else nr+=1.0;
					}
					k+=headersize+struktur[k+2]+struktur[k+3];
				}
				k_max=nr;
				r_mol*=0.95;
			}
			k=0;
			while (k<struktur_vorhanden) {
				if (struktur[k]==-30) struktur[k]=1;
				k+=headersize+struktur[k+2]+struktur[k+3];
			}
		}
		winkel=optwinkel;
		param=optparam;
		diffparam*=0.8;
	}
	if (!nrind) {
		optparam=0.0;
		optwinkel=0.0;
	}
	globparam=optparam;
	globwinkel=optwinkel;
	reset_param=globparam;
	reset_winkel=globwinkel;
}

void hilo(int m, int b[4]) {
	int l=m;
	int i;
	for (i=0; i<4; i++) {
		b[i]=l;
		l=(l>>8);
	}
	b[2]-=(b[3]<<8);
	b[1]-=(b[2]<<8)+(b[3]<<16);
	b[0]-=(b[1]<<8)+(b[2]<<16)+(b[3]<<24);
}

int c_int(char c) {
	int ret=(int) c;
	if (ret<0) ret+=256;
	return ret;
}

int read16tiff(TIFF *tif,uint32 w,uint32 h) {
	int ret=1;
	int i;
	int ii;
	int iii;
	int j;
	uint16 linesize;
	uint16 zsbau;
	char *tiff16l;
	int *tmlookauf;
	int *tmlookdrei;
	int tmu3au=aufgerundet(3*((int) w));
	tmlookauf=(int*) calloc(h,sizeof(int));
	tmlookdrei=(int*) calloc(w,sizeof(int));
	tmlookauf[0]=0;
	tmlookdrei[0]=0;
	for (i=1; i<(int32) h; i++) tmlookauf[i]=tmlookauf[i-1]+tmu3au;
	for (i=1; i<(int32) w; i++) tmlookdrei[i]=tmlookdrei[i-1]+3;
	tiff16r=(uint16*) calloc(tmu3au*h,sizeof(uint16));
	if (tiff16r==NULL) ret=0;
	tiff16n=(uint16*) calloc(tmu3au*h,sizeof(uint16));
	if (tiff16n==NULL) ret=0;
	if (ret) {
		linesize = TIFFScanlineSize(tif);
		tiff16l=(char*) malloc(linesize);
		for (i=0; i<(int32) h; i++) {
			if (TIFFReadScanline(tif,&tiff16l[0],i,0)<0) ret=0;
			if (ret) {
				zsbau=0;
				iii=0;
				ii=0;
				for (j=0; j<(int) linesize; j++) {
					if (j%2) {
						zsbau+=((uint16) c_int(tiff16l[j]))<<8;
						tiff16r[tmlookauf[h-1-i]+tmlookdrei[ii]+(2-iii)]=zsbau;
						iii+=1;
						if (iii==3) {
							iii=0;
							ii+=1;
						}
					}
					else {
						zsbau=(uint16) c_int(tiff16l[j]);
					}
				}
			}
		}
		free(tiff16l);
	}
	if (!ret) {
		free(tiff16r);
		free(tiff16n);
	}
	free(tmlookauf);
	free(tmlookdrei);
	return ret;
}

int read16tiffg(TIFF *tif,uint32 w,uint32 h) {
	int ret=1;
	int i;
	int ii;
	int iii;
	int j;
	uint16 linesize;
	uint16 zsbau;
	char *tiff16l;
	int *tmlookauf;
	int *tmlookdrei;
	int tmu3au=aufgerundet(3*((int) w));
	tmlookauf=(int*) calloc(h,sizeof(int));
	tmlookdrei=(int*) calloc(w,sizeof(int));
	tmlookauf[0]=0;
	tmlookdrei[0]=0;
	for (i=1; i<(int32) h; i++) tmlookauf[i]=tmlookauf[i-1]+tmu3au;
	for (i=1; i<(int32) w; i++) tmlookdrei[i]=tmlookdrei[i-1]+3;
	tiff16r=(uint16*) calloc(tmu3au*h,sizeof(uint16));
	if (tiff16r==NULL) ret=0;
	tiff16n=(uint16*) calloc(tmu3au*h,sizeof(uint16));
	if (tiff16n==NULL) ret=0;
	if (ret) {
		linesize = TIFFScanlineSize(tif);
		tiff16l=(char*) malloc(linesize);
		for (i=0; i<(int32) h; i++) {
			if (TIFFReadScanline(tif,&tiff16l[0],i,0)<0) ret=0;
			if (ret) {
				zsbau=0;
				ii=0;
				for (j=0; j<(int) linesize; j++) {
					if (j%2) {
						zsbau+=((uint16) c_int(tiff16l[j]))<<8;
						for (iii=0; iii<3; iii++) {
							tiff16r[tmlookauf[h-1-i]+tmlookdrei[ii]+(2-iii)]=zsbau;
						}
						ii+=1;
					}
					else {
						zsbau=(uint16) c_int(tiff16l[j]);
					}
				}
			}
		}
		free(tiff16l);
	}
	if (!ret) {
		free(tiff16r);
		free(tiff16n);
	}
	free(tmlookauf);
	free(tmlookdrei);
	return ret;
}

int read_TIFF_file(char* filename, int &x, int &y)
{
	int j;
	int i;
	int u;
	int v;
	int ret=1;
	int b[4];
	BYTE c;
	CProgressDlg progdlg;
	uint16 spp;
	uint16 bpp;
	uint16 resunit=0;
	float dpi=300.0;
	int dpii;
	uint16 photom;
	uint16 planconf;
	TIFF* tif = TIFFOpen(filename, "r");
	if (tif) {
		ret=0;
		uint32 w, h;
		size_t npixels;
		uint32* raster;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
		TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &resunit);
		TIFFGetField(tif, TIFFTAG_XRESOLUTION, &dpi);
		dpii=0;
		if ((dpi>5) && (dpi<30000)) {
			if (resunit==2) dpii=(int) dpi;
			if (resunit==3) dpii=(int) (2.54*dpi);
		}
		if ((dpii>5) && (dpii<30000)) {
			dpisettingx=dpii;
		}
		else {
			dpisettingx=300;
		}
		TIFFGetField(tif, TIFFTAG_YRESOLUTION, &dpi);
		dpii=0;
		if ((dpi>5) && (dpi<30000)) {
			if (resunit==2) dpii=(int) dpi;
			if (resunit==3) dpii=(int) (2.54*dpi);
		}
		if ((dpii>5) && (dpii<30000)) {
			dpisettingy=dpii;
		}
		else {
			dpisettingy=300;
		}
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
		TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photom);
		TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planconf);
		x=w;
		y=h;
		npixels = w * h;
		u=x;
		v=y;
		raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
		if (raster == NULL) ret=1;
		if ((x<4) || (y<4) || (((long) x)*((long) y)>134217727)) {
			ret=1;
			x=0;
			y=0;
		}
		if (!ret) {
			mu3au=aufgerundet(3*u);
			mlookauf=(int*) calloc(v,sizeof(int));
			mlookdrei=(int*) calloc(u,sizeof(int));
			mlookauf[0]=0;
			mlookdrei[0]=0;
			for (i=1; i<v; i++) mlookauf[i]=mlookauf[i-1]+mu3au;
			for (i=1; i<u; i++) mlookdrei[i]=mlookdrei[i-1]+3;
			mbild=(BYTE*) calloc(mu3au*v,sizeof(BYTE));
			if (mbild==NULL) ret=1;
		}
		if (!ret) {
			if (!batchlot) {
				globwinkel=0;
				globparam=0;
			}
			reset_param=globparam;
			reset_winkel=globwinkel;
			progdlg.Create();
			progdlg.SetRange(0,1+y);
			progdlg.SetStep(1);
			progdlg.SetPos(1);
			if (TIFFReadRGBAImage(tif, w, h, raster, 0)) {
				for (j=0; j<y; j++) {
					progdlg.StepIt();
					for (i=0; i<x; i++) {
						hilo(raster[j*x+i],b);
						c=(BYTE) b[2];
						mbild[mlookauf[j]+mlookdrei[i]]=c;
						c=(BYTE) b[1];
						mbild[mlookauf[j]+mlookdrei[i]+1]=c;
						c=(BYTE) b[0];
						mbild[mlookauf[j]+mlookdrei[i]+2]=c;
					}
				}
			}
			speicher_voll=1;
			progdlg.DestroyWindow();
			_TIFFfree(raster);
		}
		if ((speicher_voll) && (spp==3) && (bpp==16) && (photom==2) && (planconf==1)) {
			tiff16_vorhanden=read16tiff(tif,w,h);
		}
		if ((speicher_voll) && (spp==1) && (bpp==16) && (photom==1) && (planconf==1)) {
			tiff16_vorhanden=read16tiffg(tif,w,h);
		}
		TIFFClose(tif);
	}
	return 1-ret;
}

int read_JPEG_file (char* filename, int &x, int &y)
{
	int i;
	int u=0;
	int v=0;
	int ret=0;
	CProgressDlg progdlg;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE * infile;
	JSAMPARRAY buffer;
	int row_stride;
	if ((infile = fopen(filename, "rb")) == NULL) {
		return 0;
	}
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 0;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	setup_read_icc_profile(&cinfo);
	(void) jpeg_read_header(&cinfo, TRUE);
	iccstream_vorhanden=read_icc_profile(&cinfo);
	(void) jpeg_start_decompress(&cinfo);
	u=cinfo.output_width;
	v=cinfo.output_height;
	ret=1;
	dpisettingx=300;
	dpisettingy=300;
	if ((cinfo.X_density>5) && (cinfo.Y_density>5) && (cinfo.X_density<30000) && (cinfo.Y_density<30000)) {
		if (cinfo.density_unit==1) {
			dpisettingx=cinfo.X_density;
			dpisettingy=cinfo.Y_density;
		}
		if (cinfo.density_unit==2) {
			dpisettingx=(int) (cinfo.X_density*2.54);
			dpisettingy=(int) (cinfo.Y_density*2.54);
		}
	}
	if (cinfo.output_components==3) {
		ret=0;
		if ((u<4) || (v<4) || (((long) u)*((long) v)>134217727)) {
			ret=1;
			u=0;
			v=0;
		}
		if (!ret) {
			exifstream=(char*) malloc(8192*sizeof(char));
			exifstream_vorhanden=1;
			iptcstream=(char*) malloc(8192*sizeof(char));
			iptcstream_vorhanden=1;
			iptcstream[0]='\0';
			exifstream[0]='\0';
			ist_jpeg=1;
			DoModify = FALSE;
			FileSequence = 0;
			MyGlob(filename, ProcessFile);
			if ((jheadfocal>7) && (jheadfocal<201)) {
				f_global=jheadfocal;
				automatik_f=1;
			}
			reset_jhead();
			if (exifdreh>8) exifdreh=1;
			if (exifdreh<1) exifdreh=1;
			if (!relyexifori) exifdreh=1;
			if (exifdreh>4) {
				v=cinfo.output_width;
				u=cinfo.output_height;
			}
			mu3au=aufgerundet(3*u);
			mlookauf=(int*) calloc(v,sizeof(int));
			mlookdrei=(int*) calloc(u,sizeof(int));
			mlookauf[0]=0;
			mlookdrei[0]=0;
			for (i=1; i<v; i++) mlookauf[i]=mlookauf[i-1]+mu3au;
			for (i=1; i<u; i++) mlookdrei[i]=mlookdrei[i-1]+3;
			mbild=(BYTE*) calloc(mu3au*v,sizeof(BYTE));
			if (mbild==NULL) ret=1;
		}
		if (!ret) {
			if (!batchlot) {
				globwinkel=0;
				globparam=0;
			}
			reset_param=globparam;
			reset_winkel=globwinkel;
			row_stride = cinfo.output_width * cinfo.output_components;
			buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
			progdlg.Create();
			progdlg.SetRange(0,1+cinfo.output_height);
			progdlg.SetStep(1);
			progdlg.SetPos(1);
			while (cinfo.output_scanline < cinfo.output_height) {
				progdlg.StepIt();
				(void) jpeg_read_scanlines(&cinfo, buffer, 1);
				switch (exifdreh) {
				case 1 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[i]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[i]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[i]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				case 7 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[i]+mlookdrei[u-cinfo.output_scanline]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[i]+mlookdrei[u-cinfo.output_scanline]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[i]+mlookdrei[u-cinfo.output_scanline]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				case 8 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[i]+mlookdrei[-1+cinfo.output_scanline]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[i]+mlookdrei[-1+cinfo.output_scanline]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[i]+mlookdrei[-1+cinfo.output_scanline]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				case 5 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[v-1-i]+mlookdrei[-1+cinfo.output_scanline]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[v-1-i]+mlookdrei[-1+cinfo.output_scanline]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[v-1-i]+mlookdrei[-1+cinfo.output_scanline]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				case 6 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[v-1-i]+mlookdrei[u-cinfo.output_scanline]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[v-1-i]+mlookdrei[u-cinfo.output_scanline]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[v-1-i]+mlookdrei[u-cinfo.output_scanline]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				case 2 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[u-1-i]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[u-1-i]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[u-1-i]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				case 3 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[u-1-i]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[u-1-i]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[u-1-i]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				case 4 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[i]]=(BYTE) (buffer[0][3*i+2]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[i]+1]=(BYTE) (buffer[0][3*i+1]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[i]+2]=(BYTE) (buffer[0][3*i]);
					}
					break;
				}
			}
			speicher_voll=1;
			progdlg.DestroyWindow();
			(void) jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
		}
		x=u;
		y=v;
	}
	if (cinfo.output_components==1) {
		ret=0;
		if ((u<4) || (v<4) || (((long) u)*((long) v)>134217727)) {
			ret=1;
			u=0;
			v=0;
		}
		if (!ret) {
			exifstream=(char*) malloc(8192*sizeof(char));
			exifstream_vorhanden=1;
			iptcstream=(char*) malloc(8192*sizeof(char));
			iptcstream_vorhanden=1;
			iptcstream[0]='\0';
			exifstream[0]='\0';
			ist_jpeg=1;
			DoModify = FALSE;
			FileSequence = 0;
			MyGlob(filename, ProcessFile);
			if ((jheadfocal>7) && (jheadfocal<201)) {
				f_global=jheadfocal;
				automatik_f=1;
			}
			reset_jhead();
			if (exifdreh>8) exifdreh=1;
			if (exifdreh<1) exifdreh=1;
			if (!relyexifori) exifdreh=1;
			if (exifdreh>4) {
				v=cinfo.output_width;
				u=cinfo.output_height;
			}
			mu3au=aufgerundet(3*u);
			mlookauf=(int*) calloc(v,sizeof(int));
			mlookdrei=(int*) calloc(u,sizeof(int));
			mlookauf[0]=0;
			mlookdrei[0]=0;
			for (i=1; i<v; i++) mlookauf[i]=mlookauf[i-1]+mu3au;
			for (i=1; i<u; i++) mlookdrei[i]=mlookdrei[i-1]+3;
			mbild=(BYTE*) calloc(mu3au*v,sizeof(BYTE));
			if (mbild==NULL) ret=1;
		}
		if (!ret) {
			if (!batchlot) {
				globwinkel=0;
				globparam=0;
			}
			reset_param=globparam;
			reset_winkel=globwinkel;
			row_stride = cinfo.output_width * cinfo.output_components;
			buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
			progdlg.Create();
			progdlg.SetRange(0,1+cinfo.output_height);
			progdlg.SetStep(1);
			progdlg.SetPos(1);
			while (cinfo.output_scanline < cinfo.output_height) {
				progdlg.StepIt();
				(void) jpeg_read_scanlines(&cinfo, buffer, 1);
				switch (exifdreh) {
				case 1 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[i]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[i]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[i]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				case 7 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[i]+mlookdrei[u-cinfo.output_scanline]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[i]+mlookdrei[u-cinfo.output_scanline]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[i]+mlookdrei[u-cinfo.output_scanline]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				case 8 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[i]+mlookdrei[-1+cinfo.output_scanline]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[i]+mlookdrei[-1+cinfo.output_scanline]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[i]+mlookdrei[-1+cinfo.output_scanline]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				case 5 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[v-1-i]+mlookdrei[-1+cinfo.output_scanline]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-1-i]+mlookdrei[-1+cinfo.output_scanline]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-1-i]+mlookdrei[-1+cinfo.output_scanline]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				case 6 :
					for (i=0; i<v; i++) {
						mbild[mlookauf[v-1-i]+mlookdrei[u-cinfo.output_scanline]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-1-i]+mlookdrei[u-cinfo.output_scanline]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-1-i]+mlookdrei[u-cinfo.output_scanline]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				case 2 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[u-1-i]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[u-1-i]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[v-cinfo.output_scanline]+mlookdrei[u-1-i]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				case 3 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[u-1-i]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[u-1-i]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[u-1-i]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				case 4 :
					for (i=0; i<u; i++) {
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[i]]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[i]+1]=(BYTE) (buffer[0][i]);
						mbild[mlookauf[-1+cinfo.output_scanline]+mlookdrei[i]+2]=(BYTE) (buffer[0][i]);
					}
					break;
				}
			}
			speicher_voll=1;
			progdlg.DestroyWindow();
			(void) jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
		}
		x=u;
		y=v;
	}
	return 1-ret;
}

void vista_setpixel(CDC* pDC, int u, int v, long farbe) {
	CRect rPix;
	CBrush* MHBrush;
	rPix.left=u;
	rPix.top=v;
	rPix.right=rPix.left+1;
	rPix.bottom=rPix.top+1;
	MHBrush=new(CBrush);
	MHBrush->CreateSolidBrush(farbe);
	pDC->FillRect(rPix,MHBrush);
	MHBrush->DeleteObject();
	delete(MHBrush);
}

void linie(CDC* pDC, int x1_o, int y1_o, int x2_o, int y2_o, long farbe) {
	int x1;
	int y1;
	int x2;
	int y2;
	int dx;
	int dy;
	int u;
	int v;
	int i;
	int s;
	int E;
	if ((x1_o!=x2_o) || (y1_o!=y2_o)) {
		if (x1_o>x2_o) {
			x1=x2_o;
			y1=y2_o;
			x2=x1_o;
			y2=y1_o;
		}
		else {
			x1=x1_o;
			y1=y1_o;
			x2=x2_o;
			y2=y2_o;
		}
		dx=x2-x1;
		dy=y2-y1;
		u=x1;
		v=y1;
		if (dy>=0) s=1;
		else {
			s=-1;
			dy=-dy;
		}
		if (dy<=dx) {
			E=(dy<<1)-dx;
			for (i=0; i<=dx; i++) {
				vista_setpixel(pDC,u,v,farbe);
				while (E>=0) {
					v+=s;
					E-=(dx<<1);
				}
				u++;
				E+=(dy<<1);
			}
		}
		else {
			E=(dx<<1)-dy;
			for (i=0; i<=dy; i++) {
				vista_setpixel(pDC,u,v,farbe);
				while (E>=0) {
					u++;
					E-=(dy<<1);
				}
				v+=s;
				E+=(dx<<1);
			}
		}
	}
}

int i_betrag(int i) {
	int ret=i;
	if (i<0) ret=-i;
	return ret;
}

int linientest(int x, int y, int gr, int x1_o, int y1_o, int x2_o, int y2_o) {
	int x1;
	int y1;
	int x2;
	int y2;
	int dx;
	int dy;
	int u;
	int v;
	int i;
	int s;
	int E;
	int ret=0;
	if ((x1_o!=x2_o) || (y1_o!=y2_o)) {
		if (x1_o>x2_o) {
			x1=x2_o;
			y1=y2_o;
			x2=x1_o;
			y2=y1_o;
		}
		else {
			x1=x1_o;
			y1=y1_o;
			x2=x2_o;
			y2=y2_o;
		}
		dx=x2-x1;
		dy=y2-y1;
		u=x1;
		v=y1;
		if (dy>=0) s=1;
		else {
			s=-1;
			dy=-dy;
		}
		if (dy<=dx) {
			E=(dy<<1)-dx;
			for (i=0; i<=dx; i++) {
				if ((i_betrag(u-x)<=gr) && (i_betrag(v-y)<=gr)) ret=1;
				while (E>=0) {
					v+=s;
					E-=(dx<<1);
				}
				u++;
				E+=(dy<<1);
			}
		}
		else {
			E=(dx<<1)-dy;
			for (i=0; i<=dy; i++) {
				if ((i_betrag(u-x)<=gr) && (i_betrag(v-y)<=gr)) ret=1;
				while (E>=0) {
					u++;
					E-=(dy<<1);
				}
				v+=s;
				E+=(dx<<1);
			}
		}
	}
	return ret;
}

void bild_zeichnen(CDC* pDC, int max_x, int max_y) {
	int rand_x;
	int rand_y;
	int u1;
	int u2;
	int v1;
	int v2;
	int k;
	int i;
	int j;
	int jj;
	CBrush *mhbrush;
	TEXTMETRIC Metrics;
	int outx;
	int outy;
	int hoehe;
	int hochkant=1;
	double flaeche1;
	double flaeche2;
	long farbe;
	long farbe1;
	long farbe2;
	long farbe3;
	int gridx;
	int gridy;
	int ind;
	HDC dc;
	CDC bmDC;
	BITMAPINFO bmi;
	VOID *pvBits;
	HBITMAP hbmp;
	CBitmap bmp;
	CBitmap *pOldbmp;
	BITMAP bi;
	HDC dc2;
	CDC bmDC2;
	BITMAPINFO bmi2;
	VOID *pvBits2;
	HBITMAP hbmp2;
	CBitmap bmp2;
	CBitmap *pOldbmp2;
	BITMAP bi2;
	mhbrush=new(CBrush);
	if ((speicher_voll==2) && (x_global>0) && (y_global>0) && (max_x>36) && (max_y>36)) {
		rand_x=max_x/2-18;
		rand_y=max_y-24;
		kf=((double) rand_x)/(1.0*(x_global+1));
		ind=0;
		if (((double) rand_y)/(1.0*(y_global+1))<kf) {
			kf=((double) rand_y)/(1.0*(y_global+1)-1.0);
			ind=1;
		}
		if (ind) flaeche1=(max_x-(2*kf*x_global+24))*max_y;
		else flaeche1=(max_y-(kf*y_global+12))*max_x;
		rand_y=max_y/2-18;
		rand_x=max_x-24;
		kf=((double) rand_x)/(1.0*(x_global+1));
		ind=0;
		if (((double) rand_y)/(1.0*(y_global+1))<kf) {
			kf=((double) rand_y)/(1.0*(y_global+1)-1.0);
			ind=1;
		}
		if (ind) flaeche2=(max_x-(kf*x_global+12))*max_y;
		else flaeche2=(max_y-(2*kf*y_global+24))*max_x;
		if (flaeche1<flaeche2) {
			rand_x=max_x/2-18;
			rand_y=max_y-24;
		}
		else {
			rand_y=max_y/2-18;
			rand_x=max_x-24;
		}
		kf=((double) rand_x)/(1.0*(x_global+1));
		if (((double) rand_y)/(1.0*(y_global+1))<kf)
			kf=((double) rand_y)/(1.0*(y_global+1)-1.0);
		if (flaeche1<flaeche2) {
			afo=13;
			afu=afo+((int) (y_global*kf));
			afl=25+((int) (x_global*kf));
			afr=afl+((int) (x_global*kf));
		}
		else {
			afo=25+((int) (y_global*kf));
			afu=afo+((int) (y_global*kf));
			afl=13;
			afr=afl+((int) (x_global*kf));
		}
		pDC->SelectStockObject(ANSI_FIXED_FONT);
		pDC->GetTextMetrics(&Metrics);
		if (ergvo) {
			dc=CreateCompatibleDC(NULL);
			ZeroMemory(&bmi.bmiHeader,sizeof(BITMAPINFOHEADER));
			bmi.bmiHeader.biWidth=x_global;
			bmi.bmiHeader.biHeight=y_global;
			bmi.bmiHeader.biPlanes=1;
			bmi.bmiHeader.biBitCount=24;
			bmi.bmiHeader.biSizeImage=0;
			bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biClrUsed= 0;
			bmi.bmiHeader.biClrImportant=0;
			hbmp=CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,&pvBits,NULL,0);
			if (beforeafter) {
				SetDIBits(dc,hbmp,0,y_global,sbild,&bmi,DIB_RGB_COLORS);
			}
			else {
				SetDIBits(dc,hbmp,0,y_global,ebild,&bmi,DIB_RGB_COLORS);
			}
			bmp.Attach(hbmp);
			bmDC.CreateCompatibleDC(pDC);
			pOldbmp=bmDC.SelectObject(&bmp);
			bmp.GetBitmap(&bi);
			pDC->SetStretchBltMode(HALFTONE);
			pDC->StretchBlt(13,13,(int) (x_global*kf),(int) (y_global*kf),&bmDC,0,0,x_global,y_global,SRCCOPY);
			DeleteDC(bmDC);
			DeleteObject(hbmp);
			DeleteDC(dc);
			dc2=CreateCompatibleDC(NULL);
			ZeroMemory(&bmi2.bmiHeader,sizeof(BITMAPINFOHEADER));
			bmi2.bmiHeader.biWidth=x_global;
			bmi2.bmiHeader.biHeight=y_global;
			bmi2.bmiHeader.biPlanes=1;
			bmi2.bmiHeader.biBitCount=24;
			bmi2.bmiHeader.biSizeImage=0;
			bmi2.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
			bmi2.bmiHeader.biClrUsed= 0;
			bmi2.bmiHeader.biClrImportant=0;
			hbmp2=CreateDIBSection(dc2,&bmi2,DIB_RGB_COLORS,&pvBits2,NULL,0);
			if (struvo) SetDIBits(dc2,hbmp2,0,y_global,gbild,&bmi2,DIB_RGB_COLORS);
			else SetDIBits(dc2,hbmp2,0,y_global,sbild,&bmi2,DIB_RGB_COLORS);
			bmp2.Attach(hbmp2);
			bmDC2.CreateCompatibleDC(pDC);
			pOldbmp2=bmDC2.SelectObject(&bmp2);
			bmp2.GetBitmap(&bi2);
			pDC->SetStretchBltMode(HALFTONE);
			pDC->StretchBlt(afl,afo,(int) (x_global*kf),(int) (y_global*kf),&bmDC2,0,0,x_global,y_global,SRCCOPY);
			DeleteDC(bmDC2);
			DeleteObject(hbmp2);
			DeleteDC(dc2);
			if ((beforeafter) && ((kf*x_global)>Metrics.tmAveCharWidth*10)) {
				outx=12+16;
				outy=afo+32;
				pDC->SetBkMode(OPAQUE);
				hoehe=Metrics.tmHeight+3;
				pDC->SetTextColor(0+(16<<8)+(64<<16));
				pDC->SetBkColor(255+(255<<8)+(255<<16));
				if (outy<(afu-30)) {
					switch (sprach) {
						case 1: pDC->TextOut(outx,outy,"Before"); break;
						case 2: pDC->TextOut(outx,outy,"Avant"); break;
						case 3: pDC->TextOut(outx,outy,"Antes"); break;
						default: pDC->TextOut(outx,outy,"Vorher"); break;
					}
				}
			}
		}
		else {
			dc=CreateCompatibleDC(NULL);
			ZeroMemory(&bmi.bmiHeader,sizeof(BITMAPINFOHEADER));
			bmi.bmiHeader.biWidth=x_global;
			bmi.bmiHeader.biHeight=y_global;
			bmi.bmiHeader.biPlanes=1;
			bmi.bmiHeader.biBitCount=24;
			bmi.bmiHeader.biSizeImage=0;
			bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biClrUsed= 0;
			bmi.bmiHeader.biClrImportant=0;
			hbmp=CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,&pvBits,NULL,0);
			SetDIBits(dc,hbmp,0,y_global,sbild,&bmi,DIB_RGB_COLORS);
			bmp.Attach(hbmp);
			bmDC.CreateCompatibleDC(pDC);
			pOldbmp=bmDC.SelectObject(&bmp);
			bmp.GetBitmap(&bi);
			pDC->SetStretchBltMode(HALFTONE);
			pDC->StretchBlt(13,13,(int) (x_global*kf),(int) (y_global*kf),&bmDC,0,0,x_global,y_global,SRCCOPY);
			DeleteDC(bmDC);
			DeleteObject(hbmp);
			DeleteDC(dc);
		}
		if ((!ergvo) && (!struvo)) {
			farbe=(255<<16)+(255<<8)+255;
		}
		else {
			if (ergvo && struvo) farbe=(215<<16)+(215<<8)+231;
			else farbe=(215<<16)+(231<<8)+215;
		}
		pDC->SetBkColor(farbe);
		if (showgrid) {
			gridx=(int) (kf*x_global/5.0);
			gridy=(int) (kf*y_global/3.0);
			if (gridx) {
				for (i=0; i<4; i++) {
					for (j=13; j<=(12+((int) (y_global*kf))); j++) {
						farbe=pDC->GetPixel(12+gridx*(i+1),j);
						farbe1=(farbe>>16);
						farbe2=((farbe-(farbe1<<16))>>8);
						farbe3=farbe-(farbe1<<16)-(farbe2<<8);
						farbe1=383-farbe1;
						if (farbe1>255) farbe1=511-farbe1;
						farbe2=383-farbe2;
						if (farbe2>255) farbe2=511-farbe2;
						farbe3=383-farbe3;
						if (farbe3>255) farbe3=511-farbe3;
						farbe=(farbe1<<16)+(farbe2<<8)+farbe3;
						vista_setpixel(pDC,12+gridx*(i+1),j,farbe);
					}
				}
				if (customgitter) {
					for (j=13; j<=(12+((int) (y_global*kf))); j++) {
						farbe=pDC->GetPixel(12+(int) (kf*x_global*customgitterx),j);
						farbe1=(farbe>>16);
						farbe2=((farbe-(farbe1<<16))>>8);
						farbe3=farbe-(farbe1<<16)-(farbe2<<8);
						farbe1=383-farbe1;
						if (farbe1>255) farbe1=511-farbe1;
						farbe2=383-farbe2;
						if (farbe2>255) farbe2=511-farbe2;
						farbe3=383-farbe3;
						if (farbe3>255) farbe3=511-farbe3;
						farbe=(farbe1<<16)+(farbe2<<8)+farbe3;
						vista_setpixel(pDC,12+(int) (kf*x_global*customgitterx),j,farbe);
					}
				}
			}
			if (gridy) {
				for (i=0; i<2; i++) {
					for (j=13; j<=(12+((int) (x_global*kf))); j++) {
						farbe=pDC->GetPixel(j,12+gridy*(i+1));
						farbe1=(farbe>>16);
						farbe2=((farbe-(farbe1<<16))>>8);
						farbe3=farbe-(farbe1<<16)-(farbe2<<8);
						farbe1=383-farbe1;
						if (farbe1>255) farbe1=511-farbe1;
						farbe2=383-farbe2;
						if (farbe2>255) farbe2=511-farbe2;
						farbe3=383-farbe3;
						if (farbe3>255) farbe3=511-farbe3;
						farbe=(farbe1<<16)+(farbe2<<8)+farbe3;
						vista_setpixel(pDC,j,12+gridy*(i+1),farbe);
					}
				}
				if (customgitter) {
					for (j=13; j<=(12+((int) (x_global*kf))); j++) {
						farbe=pDC->GetPixel(j,12+(int) (kf*y_global*customgittery));
						farbe1=(farbe>>16);
						farbe2=((farbe-(farbe1<<16))>>8);
						farbe3=farbe-(farbe1<<16)-(farbe2<<8);
						farbe1=383-farbe1;
						if (farbe1>255) farbe1=511-farbe1;
						farbe2=383-farbe2;
						if (farbe2>255) farbe2=511-farbe2;
						farbe3=383-farbe3;
						if (farbe3>255) farbe3=511-farbe3;
						farbe=(farbe1<<16)+(farbe2<<8)+farbe3;
						vista_setpixel(pDC,j,12+(int) (kf*y_global*customgittery),farbe);
					}
				}
			}
		}
		i=0;
		if ((!ergvo) && ((kf*x_global)>Metrics.tmAveCharWidth*50)) {
			outx=afl+16;
			outy=afo+16;
			pDC->SetBkMode(TRANSPARENT);
			hoehe=Metrics.tmHeight+3;
			pDC->SetTextColor(0+(0<<8)+(0<<16));
			if ((outy+i*hoehe)<(afu-30)) pDC->TextOut(outx,outy+i*hoehe,"--- ShiftN 2025 ---");
			i+=1;
			if ((outy+i*hoehe)<(afu-30)) {
				switch (sprach) {
					case 1: pDC->TextOut(outx,outy+i*hoehe,"Automatic correction of converging lines."); break;
					case 2: pDC->TextOut(outx,outy+i*hoehe,"Correction automatique des lignes convergentes."); break;
					case 3: pDC->TextOut(outx,outy+i*hoehe,"Corrección automática de líneas convergentes."); break;
					default: pDC->TextOut(outx,outy+i*hoehe,"Automatische Korrektur stürzender Linien."); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "Please click \"Automatic correction\" to"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "Cliquez sur le bouton \"Correction automatique\""); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "Por favor, haga clic en \"Corrección Automática\""); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "Bitte klicken Sie \"Automatische Korrektur\""); break;
				}
			}
			i+=1;
			if ((outy+i*hoehe)<(afu-30)) {
				switch (sprach) {
					case 1: pDC->TextOut(outx,outy+i*hoehe,"proceed."); break;
					case 2: pDC->TextOut(outx,outy+i*hoehe,"pour continuer."); break;
					case 3: pDC->TextOut(outx,outy+i*hoehe,"para continuar."); break;
					default: pDC->TextOut(outx,outy+i*hoehe,"um fortzufahren."); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "DEL = All lines in red"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "SUPPR = Toutes lignes en rouge"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "DEL = All lines in red"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "DEL = All lines in red"); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "Left clic = line in red"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "Bouton gauche = ligne en rouge"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "Left clic = line in red"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "Left clic = line in red"); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "Right clic = line in green"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "Bouton droit = ligne en vert"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "Right clic = line in green"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "Right clic = line in green"); break;
				}
			}
			pDC->SetTextColor(long(255));
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "New features in 2025 version:"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "Nouvelles fonctionnalités dans la version 2025"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "New features in 2025 version:"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "New features in 2025 version:"); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "CTRL + left button + move = lines into select rectangle in red"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "CTRL + bouton gauche + mouvement = lignes à l'intérieur du rectangle de sélection en rouge"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "CTRL + left button + move = lines into select rectangle in red"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "CTRL + left button + move = lines into select rectangle in red"); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "CTRL + right button + move = lines into select rectangle in green"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "CTRL + bouton droit + mouvement = lignes à l'intérieur du rectangle de sélection en vert"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "CTRL + right button + move = lines into select rectangle in green"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "CTRL + right button + move = lines into select rectangle in green"); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + left button + move = lines witch center into select rectangle in red"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + bouton gauche + mouvement = lignes dont le centre est à l'intérieur du rectangle de sélection en rouge"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + left button + move = lines witch center into select rectangle in red"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + left button + move = lines witch center into select rectangle in red"); break;
				}
			}
			i += 1;
			if ((outy + i*hoehe)<(afu - 30)) {
				switch (sprach) {
				case 1: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + right button + move = lines witch center into select rectangle in green"); break;
				case 2: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + bouton droit + mouvement = lignes dont le centre est à l'intérieur du rectangle de sélection en vert"); break;
				case 3: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + right button + move = lines witch center into select rectangle in green"); break;
				default: pDC->TextOut(outx, outy + i*hoehe, "SHIFT + right button + move = lines witch center into select rectangle in green"); break;
				}
			}
			i+=2;
			pDC->SetTextColor(0+(0<<8)+(255<<16));
			if ((outy+i*hoehe)<(afu-30)) {
				switch (sprach) {
					case 1: pDC->TextOut(outx,outy+i*hoehe,"Marcus Hebel 1998-2010"); break;
					case 2: pDC->TextOut(outx,outy+i*hoehe,"Marcus Hebel 1998-2010"); break;
					case 3: pDC->TextOut(outx,outy+i*hoehe,"Marcus Hebel 1998-2010"); break;
					default: pDC->TextOut(outx,outy+i*hoehe,"Marcus Hebel 1998-2010"); break;
				}
			}
			i+=3;
		}
		if ((!ergvo) && ((kf*x_global)>Metrics.tmAveCharWidth*5)) {
			outx=afl+16;
			outy=afo+16;
			pDC->SetBkMode(TRANSPARENT);
			hoehe=Metrics.tmHeight+3;
			pDC->SetTextColor(0+(0<<8)+(0<<16));
			k=0;
			if (exifstream_vorhanden) {
				pDC->SetTextColor(63+(127<<8)+(95<<16));
				while (exifstream[k]) {
					j=0;
					while (exifstream[k+j]!='\n') {
						if ((outy+i*hoehe)<(afu-30)) {
							if (((Metrics.tmAveCharWidth+1)*(j+1))<(kf*x_global-15)) {
								pDC->TextOut(outx+Metrics.tmAveCharWidth*j,outy+i*hoehe,exifstream[k+j]);
							}
						}
						j+=1;
					}
					i+=1;
					k+=j+1;
				}
			}
			k=0;
			i+=1;
			if (iptcstream_vorhanden) {
				pDC->SetTextColor(106+(95<<8)+(58<<16));
				while ((iptcstream[k]) && (k<32000)) {
					j=0;
					jj=0;
					while (iptcstream[k+j]!='\n') {
						if (iptcstream[k+j]!='\r') {
							if ((outy+i*hoehe)<(afu-30)) {
								if (((Metrics.tmAveCharWidth+1)*(jj+1))<(kf*x_global-15)) {
									pDC->TextOut(outx+Metrics.tmAveCharWidth*jj,outy+i*hoehe,iptcstream[k+j]);
								}
								else {
									jj=0;
									i=i+1;
									if ((outy+i*hoehe)<(afu-30)) {
										if (((Metrics.tmAveCharWidth+1)*(jj+1))<(kf*x_global-15)) {
											pDC->TextOut(outx+Metrics.tmAveCharWidth*jj,outy+i*hoehe,iptcstream[k+j]);
										}
									}
								}
							}
						}
						j+=1;
						jj+=1;
					}
					i+=1;
					k+=j+1;
				}
			}
		}
		if (struvo) {
			k=0;
			while (k<struktur_vorhanden) {
				if (struktur[k]>0) {
					get_xy(struktur[k+13],u1,v1);
					get_xy(struktur[k+14],u2,v2); 
					u1=afl+(int) (kf*u1);
					u2=afl+(int) (kf*u2);
					v1=afu-(int) (kf*v1);
					v2=afu-(int) (kf*v2);
					farbe=(255<<8);
					linie(pDC,u1,v1,u2,v2,farbe);
				}
				else {
					if (struktur[k]<0) {
						get_xy(struktur[k+13],u1,v1);
						get_xy(struktur[k+14],u2,v2);
						switch (struktur[k]) {
							case (-3) : farbe=255; break;
							default : farbe=(255<<16)+(127<<8)+127; break;
						}
						u1=afl+(int) (kf*u1);
						u2=afl+(int) (kf*u2);
						v1=afu-(int) (kf*v1);
						v2=afu-(int) (kf*v2);
						linie(pDC,u1,v1,u2,v2,farbe);
					}
				}
				k+=headersize+struktur[k+2]+struktur[k+3];
			}
		}
		farbe=16711680;
		if (tiff16_vorhanden) farbe=255;
		for (i=12; i<=(13+((int) (x_global*kf))); i++) {
			j=12;
			vista_setpixel(pDC,i,j,farbe);
			j=13+((int) (y_global*kf));
			vista_setpixel(pDC,i,j,farbe);
		}
		for (j=12; j<=(13+((int) (y_global*kf))); j++) {
			i=12;
			vista_setpixel(pDC,i,j,farbe);
			i=13+((int) (x_global*kf));
			vista_setpixel(pDC,i,j,farbe);
		}
		for (i=afl-1; i<=afr; i++) {
			j=afo-1;
			vista_setpixel(pDC,i,j,farbe);
			j=afu;
			vista_setpixel(pDC,i,j,farbe);
		}
		for (j=afo-1; j<=afu; j++) {
			i=afl-1;
			vista_setpixel(pDC,i,j,farbe);
			i=afr;
			vista_setpixel(pDC,i,j,farbe);
		}
		if ((tiff16_vorhanden) && ((kf*x_global)>Metrics.tmAveCharWidth*10)) {
			outx=12+16;
			outy=afo+16;
			pDC->SetBkMode(OPAQUE);
			hoehe=Metrics.tmHeight+3;
			pDC->SetTextColor(255+(0<<8)+(0<<16));
			pDC->SetBkColor(255+(255<<8)+(255<<16));
			if (outy<(afu-30)) pDC->TextOut(outx,outy,"16 bit");
		}
	}
	else {
		if ((max_x>36) && (max_y>36)) {
			farbe=(255<<16)+(255<<8)+255;
			for (i=12; i<=max_x-13; i++) {
				j=12;
				vista_setpixel(pDC,i,j,farbe);
				j=max_y-13;
				vista_setpixel(pDC,i,j,farbe);
			}
			for (j=12; j<=max_y-13; j++) {
				i=12;
				vista_setpixel(pDC,i,j,farbe);
				i=max_x-13;
				vista_setpixel(pDC,i,j,farbe);
			}
			farbe=(200<<16)+(200<<8)+200;
			for (i=13; i<=max_x-14; i++) {
				j=13;
				vista_setpixel(pDC,i,j,farbe);
				j=max_y-14;
				vista_setpixel(pDC,i,j,farbe);
			}
			for (j=13; j<=max_y-14; j++) {
				i=13;
				vista_setpixel(pDC,i,j,farbe);
				i=max_x-14;
				vista_setpixel(pDC,i,j,farbe);
			}
		}
	}
	delete(mhbrush);
}

/////////////////////////////////////////////////////////////////////////////
// CShiftNView Diagnose

#ifdef _DEBUG
void CShiftNView::AssertValid() const
{
	CView::AssertValid();
}

void CShiftNView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShiftNDoc* CShiftNView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShiftNDoc)));
	return (CShiftNDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShiftNView Nachrichten-Handler

float mybetrag(float d) {
	float ret=d;
	if (d<0) ret=-d;
	return ret;
}

int householder (long nspme, long nze, float *A, float *x) {
	int ret=0;
	long ij;
	long jj;
	long ik;
	long nsp;
	long i;
	long j;
	long k;
	float Aij;
	float Ajj;
	float beta;
	float s;
	float sigma;
	float sum;
	nsp=nspme + 1;
	for (j=0; j<nspme; j++) {
		sigma=0.0;
		for (i=j; i<nze; i++) {
			ij=i*nsp+j;
			Aij=A[ij];
			sigma+=Aij*Aij;
		}
		if (mybetrag(sigma)<1e-20) return 1;
		jj=j*nsp+j;
		Ajj = A[jj];
		(Ajj<0.0) ? (s=sqrt(sigma)) : (s=-sqrt(sigma));
		x[j]=s;
		beta=(float) (1.0/(s*Ajj-sigma));
		A[jj]=Ajj-s;
		for (k=j+1; k<nsp; k++) {
			sum=0.0;
			for (i=j; i<nze; i++) {
				ij=i*nsp + j;
				ik=i*nsp+k;
				sum+=A[ij]*A[ik];
			}
			sum=beta*sum;
			for (i=j; i<nze; i++) {
				ij=i*nsp+j;	
				ik=i*nsp+k;
				A[ik]+=A[ij]*sum;
			}
		}
	}
	for (i=nspme-1; i>=0; i--) {
		sum=0;
		for (j=i+1; j<nspme; j++) {
			ij=i*nsp+j;	
			sum+=A[ij]*x[j];
		}
		k=i*nsp+nspme;
		sigma=x[i];
		if (mybetrag(sigma)<1e-20) {
			ret=1;
			i=-1;
		}
		else {
			x[i]=(A[k]-sum)/sigma;
		}
	}
	return ret;
}

void gauss(int id_alt, int id_neu) {
	int i;
	int j;
	int k;
	int l;
	int summe;
	int anzahl;
	int multiplikator;
	for (j=0; j<y_global; j++) {
		for (i=0; i<x_global; i++) {
			summe=0;
			anzahl=0;
			for (k=-1; k<=1; k++) {
				for (l=-1; l<=1; l++) {
					if ((i+k>=0) && (i+k<x_global) && (j+l>=0) && (j+l<y_global)) {
						multiplikator=3-k*k-l*l;
						if ((k==0) && (l==0)) multiplikator+=1;
						summe+=multiplikator*c_int(bild[id_alt][i+k][j+l]);
						anzahl+=multiplikator;
					}
				}
			}
			bild[id_neu][i][j]=i_char((int) (summe/anzahl));
		}
	}
}

void preprocessing(int glatt) {
	int i;
	int j;
	if (glatt) {
		gauss(5,4);
	}
	else {
		for (j=0; j<y_global; j++) {
			for (i=0; i<x_global; i++) {
				bild[4][i][j]=bild[5][i][j];
			}
		}
	}
}

int alloc_bilder(int u, int v) {
	int i;
	int j;
	int ii;
	int jj;
	int ic;
	int ret=0;
	int u3auf=0;
	int data;
	int u_neu;
	int v_neu;
	int interim;
	char c;
	CProgressDlg progdlg;
	interim=(u+v)/2;
	mfaktor=interim/respara;
	if (!mfaktor) mfaktor=1;
	u_neu=u/mfaktor;
	v_neu=v/mfaktor;
	u3auf=aufgerundet(3*u_neu);
	lookauf=(int*) calloc(v_neu,sizeof(int));
	lookdrei=(int*) calloc(u_neu,sizeof(int));
	lookauf[0]=0;
	lookdrei[0]=0;
	for (i=1; i<v_neu; i++) lookauf[i]=lookauf[i-1]+u3auf;
	for (i=1; i<u_neu; i++) lookdrei[i]=lookdrei[i-1]+3;
	ebild=(BYTE*) calloc(u3auf*v_neu,sizeof(BYTE));
	if (ebild==NULL) ret=1;
	sbild=(BYTE*) calloc(u3auf*v_neu,sizeof(BYTE));
	if (sbild==NULL) ret=1;
	gbild=(BYTE*) calloc(u3auf*v_neu,sizeof(BYTE));
	if (gbild==NULL) ret=1;
	bild=(char***) calloc(layer_c_intern,sizeof(char**));
	if (bild==NULL) ret=1;
	else {
		for (i=0; i<layer_c_intern; i++) {
			bild[i]=(char**) calloc(u_neu,sizeof(char*));
			if (bild[i]==NULL) ret=1;
			else {
				for (j=0; j<u_neu; j++) {
					bild[i][j]=(char*) calloc(v_neu,sizeof(char));
					if (bild[i][j]==NULL) ret=1;
				}
			}
		}
	}
	nbild=(BYTE*) calloc(mu3au*v,sizeof(BYTE));
	if (nbild==NULL) ret=1;
	l_schicht=(long***) calloc(layer_l_intern,sizeof(long**));
	if (l_schicht==NULL) ret=1;
	else {
		for (i=0; i<layer_l_intern; i++) {
			l_schicht[i]=(long**) calloc(u_neu,sizeof(long*));
			if (l_schicht[i]==NULL) ret=1;
			else {
				for (j=0; j<u_neu; j++) {
					l_schicht[i][j]=(long*) calloc(v_neu,sizeof(long));
					if (l_schicht[i][j]==NULL) ret=1;
				}
			}
		}
	}
	if (!ret) {
		progdlg.Create();
		switch (sprach) {
			case 1: progdlg.SetCaptionText("ShiftN - Allocating memory"); break;
			case 2: progdlg.SetCaptionText("ShiftN - Allocation mémoire"); break;
			case 3: progdlg.SetCaptionText("ShiftN - Alocación de memoria"); break;
			default: progdlg.SetCaptionText("ShiftN - Speicher reservieren"); break;
		}
		progdlg.SetRange(0,1+v_neu);
		progdlg.SetStep(1);
		progdlg.SetPos(1);
		for (j=0; j<v_neu; j++) {
			progdlg.StepIt();
			for (i=0; i<u_neu; i++) {
				data=0;
				if ((mfaktor>1) && (mfaktor*j<(v-mfaktor+1)) && (mfaktor*i<(u-mfaktor+1))) {
					ic=0;
					for (ii=0; ii<mfaktor; ii++) {
						for (jj=0; jj<mfaktor; jj++) {
							ic+=(int) (mbild[mlookauf[mfaktor*j+jj]+mlookdrei[mfaktor*i+ii]]);
						}
					}
					ic/=mfaktor*mfaktor;
					c=i_char(ic);
				}
				else c=i_char((int) (mbild[mlookauf[mfaktor*j]+mlookdrei[mfaktor*i]]));
				sbild[lookauf[j]+lookdrei[i]]=(BYTE) c_int(c);
				data+=rgb[2]*c_int(c);
				if ((mfaktor>1) && (mfaktor*j<(v-mfaktor+1)) && (mfaktor*i<(u-mfaktor+1))) {
					ic=0;
					for (ii=0; ii<mfaktor; ii++) {
						for (jj=0; jj<mfaktor; jj++) {
							ic+=(int) (mbild[mlookauf[mfaktor*j+jj]+mlookdrei[mfaktor*i+ii]+1]);
						}
					}
					ic/=mfaktor*mfaktor;
					c=i_char(ic);
				}
				else c=i_char((int) (mbild[mlookauf[mfaktor*j]+mlookdrei[mfaktor*i]+1]));
				sbild[lookauf[j]+lookdrei[i]+1]=(BYTE) c_int(c);
				data+=rgb[1]*c_int(c);
				if ((mfaktor>1) && (mfaktor*j<(v-mfaktor+1)) && (mfaktor*i<(u-mfaktor+1))) {
					ic=0;
					for (ii=0; ii<mfaktor; ii++) {
						for (jj=0; jj<mfaktor; jj++) {
							ic+=(int) (mbild[mlookauf[mfaktor*j+jj]+mlookdrei[mfaktor*i+ii]+2]);
						}
					}
					ic/=mfaktor*mfaktor;
					c=i_char(ic);
				}
				else c=i_char((int) (mbild[mlookauf[mfaktor*j]+mlookdrei[mfaktor*i]+2]));
				sbild[lookauf[j]+lookdrei[i]+2]=(BYTE) c_int(c);
				data+=rgb[0]*c_int(c);
				data/=(rgb[0]+rgb[1]+rgb[2]);
				bild[5][i][j]=i_char(data);
				gbild[lookauf[j]+lookdrei[i]]=(BYTE) data;
				gbild[lookauf[j]+lookdrei[i]+1]=(BYTE) data;
				gbild[lookauf[j]+lookdrei[i]+2]=(BYTE) data;
			}
		}
		progdlg.DestroyWindow();
		x_global=u_neu;
		y_global=v_neu;
	}
	if (!ret) speicher_voll=2;
	return ret;
}

int lese_bild(int &x, int &y, CString dateiname) {
	int i;
	int j;
	int u=0;
	int v=0;
	int u_auf;
	int ret=0;
	int art=0;
	int anzpal;
	char c;
	char *pal;
	ifstream datei;
	CProgressDlg progdlg;
	ist_jpeg=0;
	customgitter=0;
	customgitterx=0;
	customgittery=0;
	f_global=f_fest;
	jheadfocal=0;
	exifdreh=1;
	dpisettingx=300;
	dpisettingy=300;
	if (exifstream_vorhanden) free(exifstream);
	exifstream_vorhanden=0;
	if (iptcstream_vorhanden) free(iptcstream);
	iptcstream_vorhanden=0;
	if (iccstream_vorhanden) free(icc_data_ptr);
	iccstream_vorhanden=FALSE;
	if (!homo_vorhanden) {
		Homograph=(double***) calloc(4,sizeof(double**));
		for (i=0; i<4; i++) {
			Homograph[i]=(double**) calloc(4,sizeof(double*));
			for (j=0; j<4; j++) {
				Homograph[i][j]=(double*) calloc(4,sizeof(double));
			}
		}
		homo_vorhanden=1;
	}
	datei.open(dateiname,ios::in|ios::binary);
	if (!datei) ret=1;
	else {
		datei.get(c);
		if (c!='B') ret=2;
		datei.get(c);
		if (c!='M') ret=2;
		if (!ret) {
			for (i=0; i<9; i++) datei.get(c);
			datei.get(c);
			for (i=0; i<3; i++) datei.get(c);
			if (c!=40) ret=2;
			for (i=0; i<4; i++) datei.get(c);
		}
		if (!ret) {
			u=c_int(c);
			datei.get(c);
			u+=(c_int(c))<<8;
			datei.get(c);
			u+=(c_int(c))<<16;
			datei.get(c);
			u+=(c_int(c))<<24;
			datei.get(c);
			v=c_int(c);
			datei.get(c);
			v+=(c_int(c))<<8;
			datei.get(c);
			v+=(c_int(c))<<16;
			datei.get(c);
			v+=(c_int(c))<<24;
			datei.get(c);
			if (c!=1) ret=2;
			for (i=0; i<2; i++) datei.get(c);
			if ((c!=8) && (c!=24)) ret=2;
			if (c==8) art=1;
			for (i=0; i<2; i++) datei.get(c);
			if (c!=0) ret=2;
			for (i=0; i<16; i++) datei.get(c);
			anzpal=c_int(c);
			if (anzpal==0) anzpal=256;
			for (i=0; i<7; i++) datei.get(c);
		}
		if (!ret) {
			if ((u<4) || (v<4) || (((long) u)*((long) v)>134217727)) {
				u=0;
				v=0;
				ret=1;
			}
		}
		if (!ret) {
			mu3au=aufgerundet(3*u);
			mlookauf=(int*) calloc(v,sizeof(int));
			mlookdrei=(int*) calloc(u,sizeof(int));
			mlookauf[0]=0;
			mlookdrei[0]=0;
			for (i=1; i<v; i++) mlookauf[i]=mlookauf[i-1]+mu3au;
			for (i=1; i<u; i++) mlookdrei[i]=mlookdrei[i-1]+3;
			mbild=(BYTE*) calloc(mu3au*v,sizeof(BYTE));
			if (mbild==NULL) ret=1;
		}
		if ((!ret) && art) {
			if (!batchlot) {
				globwinkel=0;
				globparam=0;
			}
			reset_param=globparam;
			reset_winkel=globwinkel;
			u_auf=aufgerundet(u);
			pal=(char*) calloc(1024,sizeof(char));
			for (i=0; i<anzpal; i++) {
				datei.get(c);
				pal[(i<<2)]=c;
				datei.get(c);
				pal[(i<<2)+1]=c;
				datei.get(c);
				pal[(i<<2)+2]=c;
				datei.get(c);
			}
			progdlg.Create();
			progdlg.SetRange(0,1+v);
			progdlg.SetStep(1);
			progdlg.SetPos(1);
			for (j=0; j<v; j++) {
				progdlg.StepIt();
				for (i=0; i<u; i++) {
					datei.get(c);
					mbild[mlookauf[j]+mlookdrei[i]]=(BYTE) c_int(pal[(c_int(c)<<2)]);
					mbild[mlookauf[j]+mlookdrei[i]+1]=(BYTE) c_int(pal[(c_int(c)<<2)+1]);
					mbild[mlookauf[j]+mlookdrei[i]+2]=(BYTE) c_int(pal[(c_int(c)<<2)+2]);
				}
				for (i=u; i<u_auf; i++) datei.get(c);
			}
			speicher_voll=1;
			progdlg.DestroyWindow();
			free(pal);
		}
		if ((!ret) && (!art)) {
			if (!batchlot) {
				globwinkel=0;
				globparam=0;
			}
			reset_param=globparam;
			reset_winkel=globwinkel;
			u_auf=aufgerundet(3*u);
			progdlg.Create();
			progdlg.SetRange(0,1+v);
			progdlg.SetStep(1);
			progdlg.SetPos(1);
			for (j=0; j<v; j++) {
				progdlg.StepIt();
				for (i=0; i<u; i++) {
					datei.get(c);
					mbild[mlookauf[j]+mlookdrei[i]]=(BYTE) c_int(c);
					datei.get(c);
					mbild[mlookauf[j]+mlookdrei[i]+1]=(BYTE) c_int(c);
					datei.get(c);
					mbild[mlookauf[j]+mlookdrei[i]+2]=(BYTE) c_int(c);
				}
				for (i=3*u; i<u_auf; i++) datei.get(c);
			}
			speicher_voll=1;
			progdlg.DestroyWindow();
		}
	}
	datei.close();
	datei.clear();
	if (ret==2) {
		ret=1-read_JPEG_file(dateiname.GetBuffer(dateiname.GetLength()),u,v);	
		if (ret) {
			ret=1-read_TIFF_file(dateiname.GetBuffer(dateiname.GetLength()),u,v);
		}
	}
	if ((!ret) && (speicher_voll)) {
		ret=alloc_bilder(u,v);
	}
	x=u;
	y=v;
	if (commandfocus) {
		f_global=commandfocus;
		automatik_f=1;
	}
	beforeafter=0;
	return ret;
}

BOOL CShiftNView::PreCreateWindow(CREATESTRUCT& cs) {
	ifstream inidatei;
	CString ininame;
	string s;
	UINT nl;
	char c;
	int i;
	int *lsprach;
	int *lsave;
	int versionnr=0;
	direkt_laden=0;
	direkt_speichern=0;
	bilderart2=0;
	saveexit=1;
	mhshowgui=1;
	if (AfxGetApp()->GetProfileBinary("SaveExit","SAVE", (LPBYTE*)&lsave, &nl)) {
		if (!(lsave[0])) saveexit=0;
		delete [] lsave;
	}
	if (AfxGetApp()->GetProfileBinary("Language","SPRACH", (LPBYTE*)&lsprach, &nl)) {
		switch (lsprach[0]) {
			case 1: sprach=1; break;
			case 2: sprach=2; break;
			case 3: sprach=3; break;
			default: sprach=0; break;
		}
		delete [] lsprach;
	}
	if (!inivorhanden) {
		nl=__argc;
		inivorhanden=1;
		SHGetSpecialFolderPath(NULL,buff,CSIDL_APPDATA,FALSE); 
		ininame=buff;
		ininame+="\\ShiftN.ini";
		inidatei.open(ininame,ios::in);
		if (inidatei) {
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s);
			getline(inidatei,s); 
			getline(inidatei,s);
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s);
			getline(inidatei,s); 
			getline(inidatei,s);
			getline(inidatei,s);
			getline(inidatei,s); 
			getline(inidatei,s);
			getline(inidatei,s); 
			getline(inidatei,s); 
			getline(inidatei,s);
			getline(inidatei,s);
			getline(inidatei,s);
			getline(inidatei,s);
			getline(inidatei,s);
			inidatei >> versionnr;
		}
		inidatei.close();
		inidatei.clear();
		inidatei.open(ininame,ios::in);
		if ((inidatei) && (versionnr>=100) && (versionnr<=32767)) {
			inidatei >> rgb[0];
			getline(inidatei,s); 
			inidatei >> rgb[1];
			getline(inidatei,s); 
			inidatei >> rgb[2];
			getline(inidatei,s); 
			inidatei >> anzpart;
			getline(inidatei,s); 
			inidatei >> glatt;
			getline(inidatei,s); 
			inidatei >> beschneiden;
			getline(inidatei,s);
			inidatei >> kubik;
			getline(inidatei,s); 
			inidatei >> minlen;
			getline(inidatei,s); 
			inidatei >> mincon;
			getline(inidatei,s); 
			inidatei >> minsize;
			getline(inidatei,s); 
			inidatei >> nachbar;
			getline(inidatei,s); 
			inidatei >> jpegquality;
			getline(inidatei,s); 
			inidatei >> bilderart;
			getline(inidatei,s); 
			inidatei >> schaerfen;
			getline(inidatei,s); 
			inidatei >> shiftamount;
			getline(inidatei,s); 
			inidatei >> schaerfungsparameter;
			getline(inidatei,s); 
			inidatei >> mingrad;
			sqmingrad=4*mingrad*mingrad;
			getline(inidatei,s); 
			inidatei >> minsup;
			getline(inidatei,s); 
			inidatei >> max_disp;
			getline(inidatei,s); 
			inidatei >> dorot;
			getline(inidatei,s); 
			inidatei >> relyexifori;
			getline(inidatei,s); 
			inidatei >> distortion;
			getline(inidatei,s); 
			inidatei >> f_fest;
			getline(inidatei,s);
			c=' ';
			i=0;
			m_suffix=_T("");
			while ((c!='\t') && (i<1024)) {
				inidatei.get(c);
				if (c!='\t') m_suffix+=c;
				i+=1;
			}
			getline(inidatei,s);
			inidatei >> showgrid;
			getline(inidatei,s);
			inidatei >> use_exif_date;
			getline(inidatei,s);
			inidatei >> horifac;
			getline(inidatei,s);
			inidatei >> respara;
			getline(inidatei,s);
			inidatei >> usebackrgb;
			getline(inidatei,s);
			inidatei >> backrgb;
			getline(inidatei,s);
			distortion_fest=distortion;
			horifac_fest=horifac;
			reset_distortion=distortion;
			reset_horizontal=horifac;
			f_global=f_fest;
			inidatei >> parlot;
			getline(inidatei,s);
		}
		inidatei.close();
		inidatei.clear();
		if (nl>=3) {
			m_sdateiname=__argv[2];
			direkt_speichern=2;
			if (_tcsstr(m_sdateiname.GetBuffer(m_sdateiname.GetLength()),_T("Clipboard"))) {
				direkt_speichern_clipboard=1;
			}
			else {
				direkt_speichern_clipboard=0;
			}
			mhshowgui=0;
		}
		if (nl>=4) {
			i=0;
			while ((__argv[3][i]!='\0') && (i<1000)) i++;
			if (i>0) {
				if ((__argv[3][0]==109) || (__argv[3][0]==77) || (__argv[3][0]==65) || (__argv[3][0]==97)) {
					if ((__argv[3][0]==109) || (__argv[3][0]==77)) {
						direkt_speichern=3;
						mhshowgui=1;
					} 
					if (i>1) {
						c=__argv[3][1];
						if ((c>48) && (c<56)) bilderart2=(int) (c-48);
					}
				}
			}
		}
		if (nl>=5) {
			commandfocus=atoi(__argv[4]);
			if ((commandfocus<8) || (commandfocus>200)) commandfocus=0;
		}
		else {
			commandfocus=0;
		}
		if (nl>=2) {
			m_dateiname=__argv[1];
			direkt_laden=2;
			if (_tcsstr(m_dateiname.GetBuffer(m_dateiname.GetLength()),_T("Clipboard"))) {
				direkt_laden_clipboard=1;
			}
			else {
				direkt_laden_clipboard=0;
			}
		}
		else {
			if (!speicher_voll) {
				switch (sprach) {
					case 1: m_dateiname="No file loaded"; break;
					case 2: m_dateiname="Aucun fichier chargé"; break;
					case 3: m_dateiname="No hay archivo cargado"; break;
					default: m_dateiname="Keine Datei geladen"; break;
				}
			}
		}
	}
	return CView::PreCreateWindow(cs);
}

int partitionswert(double w, double wsteph) {
	int i;
	int ret=0;
	double wunten;
	wunten=-mypi;
	i=1;
	while (i<=anzpart) {
		if ((w>=wunten) && (w<(wunten+wsteph))) {
			ret=i;
			i=anzpart;
		}
		wunten+=wsteph;
		i+=1;
	}
	return ret;
}

void CShiftNView::OnFinalRelease() 
{
	CView::OnFinalRelease();
}

void pb(char b) {
	ausgabedatei.write((char*) &b,sizeof(char));
}

void header(int x, int y) {
	int res;
	int size;
	int b[4];
	res=aufgerundet(3*x)*y;
	size=54+res;
	pb((char) 66);
	pb((char) 77);
	hilo(size,b);
	pb((char) b[0]);
	pb((char) b[1]);
	pb((char) b[2]);
	pb((char) b[3]);	
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 54);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 40);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	hilo(x,b);
	pb((char) b[0]);
	pb((char) b[1]);
	pb((char) b[2]);
	pb((char) b[3]);
	hilo(y,b);
	pb((char) b[0]);
	pb((char) b[1]);
	pb((char) b[2]);
	pb((char) b[3]);
	pb((char) 1);
	pb((char) 0);
	pb((char) 24);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	hilo(res,b);
	pb((char) b[0]);
	pb((char) b[1]);
	pb((char) b[2]);
	pb((char) b[3]);
	hilo(3780,b);
	pb((char) b[0]);
	pb((char) b[1]);
	pb((char) b[2]);
	pb((char) b[3]);
	pb((char) b[0]);
	pb((char) b[1]);
	pb((char) b[2]);
	pb((char) b[3]);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
	pb((char) 0);
}

void headerc(int x, int y) {
	int res;
	int b[4];
	int k=0;
	res=aufgerundet(3*x)*y;
	clipbild[k++]=(BYTE) (40);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	hilo(x,b);
	clipbild[k++]=(BYTE) (b[0]);
	clipbild[k++]=(BYTE) (b[1]);
	clipbild[k++]=(BYTE) (b[2]);
	clipbild[k++]=(BYTE) (b[3]);
	hilo(y,b);
	clipbild[k++]=(BYTE) (b[0]);
	clipbild[k++]=(BYTE) (b[1]);
	clipbild[k++]=(BYTE) (b[2]);
	clipbild[k++]=(BYTE) (b[3]);
	clipbild[k++]=(BYTE) (1);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (24);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	hilo(res,b);
	clipbild[k++]=(BYTE) (b[0]);
	clipbild[k++]=(BYTE) (b[1]);
	clipbild[k++]=(BYTE) (b[2]);
	clipbild[k++]=(BYTE) (b[3]);
	hilo(3780,b);
	clipbild[k++]=(BYTE) (b[0]);
	clipbild[k++]=(BYTE) (b[1]);
	clipbild[k++]=(BYTE) (b[2]);
	clipbild[k++]=(BYTE) (b[3]);
	clipbild[k++]=(BYTE) (b[0]);
	clipbild[k++]=(BYTE) (b[1]);
	clipbild[k++]=(BYTE) (b[2]);
	clipbild[k++]=(BYTE) (b[3]);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
	clipbild[k++]=(BYTE) (0);
}

void cutter(double xf, double yf, double &max_oben, double &max_unten, double &max_links, double &max_rechts) {
	int i;
	double rechts;
	double links;
	double unten;
	double oben;
	double px;
	double py;
	double TPx;
	double TPy;
	double bild_mitte_x;
	double bild_mitte_y;
	double Ahier;
	double Amax=-1e99;
	double **P;
	int ind;
	max_oben=yf;
	max_unten=0;
	max_links=0;
	max_rechts=xf;
	bild_mitte_y=(Homograph[0][1][1]*yf*0.5+Homograph[0][1][2]*xf*0.5+Homograph[0][1][3])/(Homograph[0][3][1]*yf*0.5+Homograph[0][3][2]*xf*0.5+Homograph[0][3][3]);
	bild_mitte_x=(Homograph[0][2][1]*yf*0.5+Homograph[0][2][2]*xf*0.5+Homograph[0][2][3])/(Homograph[0][3][1]*yf*0.5+Homograph[0][3][2]*xf*0.5+Homograph[0][3][3]);
	P=(double**) calloc(4,sizeof(double*));
	for (i=0; i<4; i++) {
		P[i]=(double*) calloc(2,sizeof(double));
	}
	for (px=0.01; px<=1.99; px+=0.01) {
		py=2.0-px;
		rechts=bild_mitte_x;
		unten=bild_mitte_y;
		links=bild_mitte_x;
		oben=bild_mitte_y;
		ind=0;
		P[0][1]=(Homograph[0][1][1]*0+Homograph[0][1][2]*0+Homograph[0][1][3])/(Homograph[0][3][1]*0+Homograph[0][3][2]*0+Homograph[0][3][3]);
		P[0][0]=(Homograph[0][2][1]*0+Homograph[0][2][2]*0+Homograph[0][2][3])/(Homograph[0][3][1]*0+Homograph[0][3][2]*0+Homograph[0][3][3]);
		P[1][1]=(Homograph[0][1][1]*0+Homograph[0][1][2]*xf+Homograph[0][1][3])/(Homograph[0][3][1]*0+Homograph[0][3][2]*xf+Homograph[0][3][3]);
		P[1][0]=(Homograph[0][2][1]*0+Homograph[0][2][2]*xf+Homograph[0][2][3])/(Homograph[0][3][1]*0+Homograph[0][3][2]*xf+Homograph[0][3][3]);
		P[2][1]=(Homograph[0][1][1]*yf+Homograph[0][1][2]*xf+Homograph[0][1][3])/(Homograph[0][3][1]*yf+Homograph[0][3][2]*xf+Homograph[0][3][3]);
		P[2][0]=(Homograph[0][2][1]*yf+Homograph[0][2][2]*xf+Homograph[0][2][3])/(Homograph[0][3][1]*yf+Homograph[0][3][2]*xf+Homograph[0][3][3]);
		P[3][1]=(Homograph[0][1][1]*yf+Homograph[0][1][2]*0+Homograph[0][1][3])/(Homograph[0][3][1]*yf+Homograph[0][3][2]*0+Homograph[0][3][3]);
		P[3][0]=(Homograph[0][2][1]*yf+Homograph[0][2][2]*0+Homograph[0][2][3])/(Homograph[0][3][1]*yf+Homograph[0][3][2]*0+Homograph[0][3][3]);
		do {
			ind=0;
			rechts+=px;
			if (rechts>xf) ind=(ind|1);
			links-=px;
			if (links<0) ind=(ind|2);
			unten-=py;
			if (unten<0) ind=(ind|4);
			oben+=py;
			if (oben>yf) ind=(ind|8);
			TPx=links;
			TPy=unten;
			if (((TPx-P[0][0])*(P[1][1]-P[0][1])+(TPy-P[0][1])*(P[0][0]-P[1][0]))>0) ind=(ind|6);
			if (((TPx-P[1][0])*(P[2][1]-P[1][1])+(TPy-P[1][1])*(P[1][0]-P[2][0]))>0) ind=(ind|6);
			if (((TPx-P[2][0])*(P[3][1]-P[2][1])+(TPy-P[2][1])*(P[2][0]-P[3][0]))>0) ind=(ind|6);
			if (((TPx-P[3][0])*(P[0][1]-P[3][1])+(TPy-P[3][1])*(P[3][0]-P[0][0]))>0) ind=(ind|6);
			TPx=rechts;
			TPy=unten;
			if (((TPx-P[0][0])*(P[1][1]-P[0][1])+(TPy-P[0][1])*(P[0][0]-P[1][0]))>0) ind=(ind|5);
			if (((TPx-P[1][0])*(P[2][1]-P[1][1])+(TPy-P[1][1])*(P[1][0]-P[2][0]))>0) ind=(ind|5);
			if (((TPx-P[2][0])*(P[3][1]-P[2][1])+(TPy-P[2][1])*(P[2][0]-P[3][0]))>0) ind=(ind|5);
			if (((TPx-P[3][0])*(P[0][1]-P[3][1])+(TPy-P[3][1])*(P[3][0]-P[0][0]))>0) ind=(ind|5);
			TPx=rechts;
			TPy=oben;
			if (((TPx-P[0][0])*(P[1][1]-P[0][1])+(TPy-P[0][1])*(P[0][0]-P[1][0]))>0) ind=(ind|9);
			if (((TPx-P[1][0])*(P[2][1]-P[1][1])+(TPy-P[1][1])*(P[1][0]-P[2][0]))>0) ind=(ind|9);
			if (((TPx-P[2][0])*(P[3][1]-P[2][1])+(TPy-P[2][1])*(P[2][0]-P[3][0]))>0) ind=(ind|9);
			if (((TPx-P[3][0])*(P[0][1]-P[3][1])+(TPy-P[3][1])*(P[3][0]-P[0][0]))>0) ind=(ind|9);
			TPx=links;
			TPy=oben;
			if (((TPx-P[0][0])*(P[1][1]-P[0][1])+(TPy-P[0][1])*(P[0][0]-P[1][0]))>0) ind=(ind|10);
			if (((TPx-P[1][0])*(P[2][1]-P[1][1])+(TPy-P[1][1])*(P[1][0]-P[2][0]))>0) ind=(ind|10);
			if (((TPx-P[2][0])*(P[3][1]-P[2][1])+(TPy-P[2][1])*(P[2][0]-P[3][0]))>0) ind=(ind|10);
			if (((TPx-P[3][0])*(P[0][1]-P[3][1])+(TPy-P[3][1])*(P[3][0]-P[0][0]))>0) ind=(ind|10);
			if (ind&1) rechts-=px;
			if (ind&2) links+=px;
			if (ind&4) unten+=py;
			if (ind&8) oben-=py;
		}
		while (ind!=15);
		P[0][1]=(Homograph[1][1][1]*oben+Homograph[1][1][2]*links+Homograph[1][1][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*links+Homograph[1][3][3]);
		P[0][0]=(Homograph[1][2][1]*oben+Homograph[1][2][2]*links+Homograph[1][2][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*links+Homograph[1][3][3]);
		P[1][1]=(Homograph[1][1][1]*oben+Homograph[1][1][2]*rechts+Homograph[1][1][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[1][0]=(Homograph[1][2][1]*oben+Homograph[1][2][2]*rechts+Homograph[1][2][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[2][1]=(Homograph[1][1][1]*unten+Homograph[1][1][2]*rechts+Homograph[1][1][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[2][0]=(Homograph[1][2][1]*unten+Homograph[1][2][2]*rechts+Homograph[1][2][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[3][1]=(Homograph[1][1][1]*unten+Homograph[1][1][2]*links+Homograph[1][1][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*links+Homograph[1][3][3]);
		P[3][0]=(Homograph[1][2][1]*unten+Homograph[1][2][2]*links+Homograph[1][2][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*links+Homograph[1][3][3]);			
		Ahier=fabs((P[0][0]-P[1][0])*(P[0][1]-P[3][1])-(P[0][1]-P[1][1])*(P[0][0]-P[3][0]));
		Ahier+=fabs((P[2][0]-P[1][0])*(P[2][1]-P[3][1])-(P[2][1]-P[1][1])*(P[2][0]-P[3][0]));
		if (Ahier>Amax) {
			Amax=Ahier;
			max_oben=oben;
			max_unten=unten;
			max_links=links;
			max_rechts=rechts;
		}
	}
	for (i=0; i<4; i++) {
		free(P[i]);
	}
	free(P);
}

void hermite(double t, double vektor[4]) {
	double t2=t*t;
	double t3=t2*t;
	vektor[0]=2*t3-3*t2+1;
	vektor[2]=t3-t2;
	vektor[1]=vektor[2]-t2+t;
	vektor[3]=-vektor[0]+1;
}

int bilinear(int k, double uu, double vv) {
	int ii;
	int jj;
	double ru;
	double rv;
	int indi=255;
	ii=(int) (mfaktor*uu);
	jj=(int) (mfaktor*vv);
	ru=1.0-mfaktor*uu+(double) ii;
	rv=1.0-mfaktor*vv+(double) jj;
	if ((ii>=0) && (ii<=mx_global-1) && (jj>=0) && (jj<=my_global-1)) {
		if ((ii<=mx_global-2) && (jj<=my_global-2)) {
			indi=(int) (ru*rv*((int) mbild[k+mlookdrei[ii]+mlookauf[my_global-jj-1]])+(1-ru)*rv*((int) mbild[k+mlookdrei[ii+1]+mlookauf[my_global-jj-1]])+ru*(1-rv)*((int) mbild[k+mlookdrei[ii]+mlookauf[my_global-jj-2]])+(1-ru)*(1-rv)*((int) mbild[k+mlookdrei[ii+1]+mlookauf[my_global-jj-2]]));
		}
		else {
			indi=(int) mbild[k+mlookdrei[ii]+mlookauf[my_global-jj-1]];
		}
	}
	return indi;
}

int bilinear16(int k, double uu, double vv) {
	int ii;
	int jj;
	double ru;
	double rv;
	int indi=65535;
	ii=(int) (mfaktor*uu);
	jj=(int) (mfaktor*vv);
	ru=1.0-mfaktor*uu+(double) ii;
	rv=1.0-mfaktor*vv+(double) jj;
	if ((ii>=0) && (ii<=mx_global-1) && (jj>=0) && (jj<=my_global-1)) {
		if ((ii<=mx_global-2) && (jj<=my_global-2)) {
			indi=(int) (ru*rv*((int) tiff16r[k+mlookdrei[ii]+mlookauf[my_global-jj-1]])+(1-ru)*rv*((int) tiff16r[k+mlookdrei[ii+1]+mlookauf[my_global-jj-1]])+ru*(1-rv)*((int) tiff16r[k+mlookdrei[ii]+mlookauf[my_global-jj-2]])+(1-ru)*(1-rv)*((int) tiff16r[k+mlookdrei[ii+1]+mlookauf[my_global-jj-2]]));
		}
		else {
			indi=(int) tiff16r[k+mlookdrei[ii]+mlookauf[my_global-jj-1]];
		}
	}
	return indi;
}

int bikubisch(int k, double uu, double vv) {
	double C[4][4];
	double xu[4][4];
	double xv[4][4];
	double xuv[2][2];
	double hu[4];
	double hv[4];
	double wert;
	int wert2=255;
	int ur;
	int vu;
	int i;
	int j;
	ur=(int) (mfaktor*uu);
	vu=(int) (mfaktor*vv);
	if ((ur>=2) && (ur<=(mx_global-4)) && (vu>=2) && (vu<=(my_global-4))) {
		for (i=-1; i<3; i++) {
			for (j=-1; j<3; j++) {
				xu[i+1][j+1]=((int) mbild[k+mlookdrei[ur+i+1]+mlookauf[my_global-1-(vu+j)]]-(int) mbild[k+mlookdrei[ur+i-1]+mlookauf[my_global-1-(vu+j)]])/2.0;
			}
		}
		for (i=-1; i<3; i++) {
			for (j=-1; j<3; j++) {
				xv[i+1][j+1]=((int) mbild[k+mlookdrei[ur+i]+mlookauf[my_global-1-(vu+j+1)]]-(int) mbild[k+mlookdrei[ur+i]+mlookauf[my_global-1-(vu+j-1)]])/2.0;
			}
		}
		for (i=0; i<2; i++) {
			for (j=0; j<2; j++) {
				xuv[i][j]=0.5*(xv[i+2][j+1]-xv[i][j+1]);
				xuv[i][j]+=0.5*(xu[i+1][j+2]-xu[i+1][j]);
				xuv[i][j]-=0.25*((int) mbild[k+mlookdrei[ur+i-1]+mlookauf[my_global-1-(vu+j-1)]]-(int) mbild[k+mlookdrei[ur+i-1]+mlookauf[my_global-1-(vu+j+1)]]-(int) mbild[k+mlookdrei[ur+i+1]+mlookauf[my_global-1-(vu+j-1)]]+(int) mbild[k+mlookdrei[ur+i+1]+mlookauf[my_global-1-(vu+j+1)]]);
			}
		}
		C[0][0]=(double) mbild[k+mlookdrei[ur]+mlookauf[my_global-1-vu]];
		C[0][1]=xv[1][1];
		C[0][2]=xv[1][2];
		C[0][3]=(double) mbild[k+mlookdrei[ur]+mlookauf[my_global-1-(vu+1)]];
		C[1][0]=xu[1][1];
		C[1][1]=xuv[0][0];
		C[1][2]=xuv[0][1];
		C[1][3]=xu[1][2];
		C[2][0]=xu[2][1];
		C[2][1]=xuv[1][0];
		C[2][2]=xuv[1][1];
		C[2][3]=xu[2][2];
		C[3][0]=(double) mbild[k+mlookdrei[ur+1]+mlookauf[my_global-1-vu]];
		C[3][1]=xv[2][1];
		C[3][2]=xv[2][2];
		C[3][3]=(double) mbild[k+mlookdrei[ur+1]+mlookauf[my_global-1-(vu+1)]];
		hermite(mfaktor*uu-ur,hu);
		hermite(mfaktor*vv-vu,hv);
		wert=hu[0]*(hv[0]*C[0][0]+hv[1]*C[0][1]+hv[2]*C[0][2]+hv[3]*C[0][3]);
		wert+=hu[1]*(hv[0]*C[1][0]+hv[1]*C[1][1]+hv[2]*C[1][2]+hv[3]*C[1][3]);
		wert+=hu[2]*(hv[0]*C[2][0]+hv[1]*C[2][1]+hv[2]*C[2][2]+hv[3]*C[2][3]);
		wert+=hu[3]*(hv[0]*C[3][0]+hv[1]*C[3][1]+hv[2]*C[3][2]+hv[3]*C[3][3]);
		wert2=(int) wert;
	}
	else {
		wert2=bilinear(k,uu,vv);
	}
	return wert2;
}

int bikubisch16(int k, double uu, double vv) {
	double C[4][4];
	double xu[4][4];
	double xv[4][4];
	double xuv[2][2];
	double hu[4];
	double hv[4];
	double wert;
	int wert2=65535;
	int ur;
	int vu;
	int i;
	int j;
	ur=(int) (mfaktor*uu);
	vu=(int) (mfaktor*vv);
	if ((ur>=2) && (ur<=(mx_global-4)) && (vu>=2) && (vu<=(my_global-4))) {
		for (i=-1; i<3; i++) {
			for (j=-1; j<3; j++) {
				xu[i+1][j+1]=((int) tiff16r[k+mlookdrei[ur+i+1]+mlookauf[my_global-1-(vu+j)]]-(int) tiff16r[k+mlookdrei[ur+i-1]+mlookauf[my_global-1-(vu+j)]])/2.0;
			}
		}
		for (i=-1; i<3; i++) {
			for (j=-1; j<3; j++) {
				xv[i+1][j+1]=((int) tiff16r[k+mlookdrei[ur+i]+mlookauf[my_global-1-(vu+j+1)]]-(int) tiff16r[k+mlookdrei[ur+i]+mlookauf[my_global-1-(vu+j-1)]])/2.0;
			}
		}
		for (i=0; i<2; i++) {
			for (j=0; j<2; j++) {
				xuv[i][j]=0.5*(xv[i+2][j+1]-xv[i][j+1]);
				xuv[i][j]+=0.5*(xu[i+1][j+2]-xu[i+1][j]);
				xuv[i][j]-=0.25*((int) tiff16r[k+mlookdrei[ur+i-1]+mlookauf[my_global-1-(vu+j-1)]]-(int) tiff16r[k+mlookdrei[ur+i-1]+mlookauf[my_global-1-(vu+j+1)]]-(int) tiff16r[k+mlookdrei[ur+i+1]+mlookauf[my_global-1-(vu+j-1)]]+(int) tiff16r[k+mlookdrei[ur+i+1]+mlookauf[my_global-1-(vu+j+1)]]);
			}
		}
		C[0][0]=(double) tiff16r[k+mlookdrei[ur]+mlookauf[my_global-1-vu]];
		C[0][1]=xv[1][1];
		C[0][2]=xv[1][2];
		C[0][3]=(double) tiff16r[k+mlookdrei[ur]+mlookauf[my_global-1-(vu+1)]];
		C[1][0]=xu[1][1];
		C[1][1]=xuv[0][0];
		C[1][2]=xuv[0][1];
		C[1][3]=xu[1][2];
		C[2][0]=xu[2][1];
		C[2][1]=xuv[1][0];
		C[2][2]=xuv[1][1];
		C[2][3]=xu[2][2];
		C[3][0]=(double) tiff16r[k+mlookdrei[ur+1]+mlookauf[my_global-1-vu]];
		C[3][1]=xv[2][1];
		C[3][2]=xv[2][2];
		C[3][3]=(double) tiff16r[k+mlookdrei[ur+1]+mlookauf[my_global-1-(vu+1)]];
		hermite(mfaktor*uu-ur,hu);
		hermite(mfaktor*vv-vu,hv);
		wert=hu[0]*(hv[0]*C[0][0]+hv[1]*C[0][1]+hv[2]*C[0][2]+hv[3]*C[0][3]);
		wert+=hu[1]*(hv[0]*C[1][0]+hv[1]*C[1][1]+hv[2]*C[1][2]+hv[3]*C[1][3]);
		wert+=hu[2]*(hv[0]*C[2][0]+hv[1]*C[2][1]+hv[2]*C[2][2]+hv[3]*C[2][3]);
		wert+=hu[3]*(hv[0]*C[3][0]+hv[1]*C[3][1]+hv[2]*C[3][2]+hv[3]*C[3][3]);
		wert2=(int) wert;
	}
	else {
		wert2=bilinear16(k,uu,vv);
	}
	return wert2;
}

UINT interpol_row(LPVOID pParam) {
	int i;
	int j;
	int k;
	int i_min_part;
	int i_max_part;
	int *partpoint;
	int indi[4];
	double jd;
	double id;
	double uu;
	double vv;
	double uuu;
	double vvv;
	int part;
	partpoint=(int*) pParam; 
	part=*partpoint;
	if (part) {
		i_min_part=(max_statt_ip*mfaktor)/2;
		i_max_part=max_statt_ip*mfaktor-1;
	}
	else {
		i_min_part=0;
		i_max_part=(max_statt_ip*mfaktor)/2-1;
	}
	for (i=i_min_part; i<=i_max_part; i+=1) {
		for (j=0; j<max_statt_jp*mfaktor; j+=1) {
			jd=(double) (yfp-1-y_maxp)+(((double) j)/((double) mfaktor))/ifak2p;
			id=(double) x_minp+(((double) i)/((double) mfaktor))/ifak2p;
			vvv=(Homograph[1][1][1]*jd+Homograph[1][1][2]*id+Homograph[1][1][3])/(Homograph[1][3][1]*jd+Homograph[1][3][2]*id+Homograph[1][3][3]);
			uuu=(Homograph[1][2][1]*jd+Homograph[1][2][2]*id+Homograph[1][2][3])/(Homograph[1][3][1]*jd+Homograph[1][3][2]*id+Homograph[1][3][3]);
			for (k=0; k<3; k++) {
				indi[k]=255;  // Background
			}
			if (usebackrgb) {
				indi[2]=(int) (backrgb>>16);
				indi[1]=(int) ((backrgb-((indi[2])<<16))>>8);
				indi[0]=(int) (backrgb-((backrgb>>8)<<8));
			}
			if ((uuu>=0) && (uuu<=x_global-1) && (vvv>=0) && (vvv<=y_global-1)) {
				if (fabs(distortion)>1e-8) {
					entzeichne(1,fkop,diparamp,uuu,vvv,xmp,ymp,uu,vv);
				}
				else {
					uu=uuu;
					vv=vvv;
				}
				for (k=0; k<3; k++) {
					if (kubik) indi[k]=bikubisch(k,uu,vv);
					else indi[k]=bilinear(k,uu,vv);
					if (indi[k]<0) indi[k]=0;
					if (indi[k]>255) indi[k]=255;
				}
			}
			for (k=0; k<3; k++) {
				nbild[mlookauf[j]+mlookdrei[i]+k]=(BYTE) indi[k];
			}
		}
	}
	SetEvent(hCompleted[part]);
	delete partpoint;
	return 0;
}

UINT interpol16_row(LPVOID pParam) {
	int i;
	int j;
	int k;
	int i_min_part;
	int i_max_part;
	int *partpoint;
	int indi[4];
	double jd;
	double id;
	double uu;
	double vv;
	double uuu;
	double vvv;
	int part;
	partpoint=(int*) pParam; 
	part=*partpoint;
	if (part) {
		i_min_part=(max_statt_ip*mfaktor)/2;
		i_max_part=max_statt_ip*mfaktor-1;
	}
	else {
		i_min_part=0;
		i_max_part=(max_statt_ip*mfaktor)/2-1;
	}
	for (i=i_min_part; i<=i_max_part; i+=1) {
		for (j=0; j<max_statt_jp*mfaktor; j+=1) {
			jd=(double) (yfp-1-y_maxp)+(((double) j)/((double) mfaktor))/ifak2p;
			id=(double) x_minp+(((double) i)/((double) mfaktor))/ifak2p;
			vvv=(Homograph[1][1][1]*jd+Homograph[1][1][2]*id+Homograph[1][1][3])/(Homograph[1][3][1]*jd+Homograph[1][3][2]*id+Homograph[1][3][3]);
			uuu=(Homograph[1][2][1]*jd+Homograph[1][2][2]*id+Homograph[1][2][3])/(Homograph[1][3][1]*jd+Homograph[1][3][2]*id+Homograph[1][3][3]);
			for (k=0; k<3; k++) {
				indi[k]=65535; // Background
			}
			if (usebackrgb) {
				indi[2]=(int) ((backrgb>>16)<<8);
				indi[1]=(int) (((backrgb-((indi[2])<<16))>>8)<<8);
				indi[0]=(int) ((backrgb-((backrgb>>8)<<8))<<8);
			}
			if ((uuu>=0) && (uuu<=x_global-1) && (vvv>=0) && (vvv<=y_global-1)) {
				if (fabs(distortion)>1e-8) {
					entzeichne(1,fkop,diparamp,uuu,vvv,xmp,ymp,uu,vv);
				}
				else {
					uu=uuu;
					vv=vvv;
				}
				for (k=0; k<3; k++) {
					if (kubik) indi[k]=bikubisch16(k,uu,vv);
					else indi[k]=bilinear16(k,uu,vv);
					if (indi[k]<0) indi[k]=0;
					if (indi[k]>65535) indi[k]=65535;
				}
			}
			for (k=0; k<3; k++) {
				tiff16n[mlookauf[j]+mlookdrei[i]+k]=(uint16) (indi[k]);
			}
		}
	}
	SetEvent(hCompleted[part]);
	delete partpoint;
	return 0;
}

//   schreibe_bild(    1,            4,       x_global,y_global,  dateiname,     1,           1,            x_neu,      y_neu);
void schreibe_bild(int savedisk, int id2, int xf, int yf, CString dateiname, int skala_x, int skala_y, int &x_neu, int &y_neu, boolean saveas = false) {
	int i;
	int j;
	int u;
	int v;
	double diparam;
	double xm=x_global*0.5;
	double ym=y_global*0.5;
	double fko;
	int part;
	int x_min;
	int y_min;
	int x_max;
	int y_max;
	int x=xf;
	int y=yf;
	int progmax;
	int progmax2;
	int tmpbilderart2=bilderart2;
	int tmpbilderart=bilderart;
	long k;
	long ll;
	int x_neu_auf;
	int ind;
	int *pj;
	double ifak;
	double ifak2;
	double max_rechts;
	double max_links;
	double max_unten;
	double max_oben;
	double rechts;
	double links;
	double unten;
	double oben;
	double sollr;
	double sollg;
	double sollb;
	double diffr;
	double diffg;
	double diffb;
	double wirdr;
	double wirdg;
	double wirdb;
	double A_neu;
	double A_alt;
	double **P;
	int max_statt_i;
	int max_statt_j;
	int ergvo_old;
	int struvo_old;
	CProgressDlg progdlg;
	unsigned char *tiff_scanline;
	uint16 *tiff16_scanline;
	CString datei2n;
	TIFF *dif;
	ifak2=1.0;
	x_neu=xf;
	y_neu=yf;
	if (!beschneiden) {
		y_min=0;
		y_max=yf;
		x_min=0;
		x_max=xf;
		x=xf;
		y=yf;
	}
	if (selektion_vorhanden>1) {
		m_rect.NormalizeRect();
		if (((m_rect.bottom-m_rect.top)<3) || ((m_rect.right-m_rect.left)<3)) {
			selektion_vorhanden=0;
		}
	}
	if (selektion_vorhanden>1) {
		P=(double**) calloc(4,sizeof(double*));
		for (i=0; i<4; i++) {
			P[i]=(double*) calloc(2,sizeof(double));
		}
		ifak=1.0;
		if (kf>0.0001) {
			y_min=yf-2-(int) ((m_rect.bottom-13)/kf);
			y_max=yf-1-(int) ((m_rect.top-13)/kf);
			x_min=(int) ((m_rect.left-13)/kf);
			x_max=1+(int) ((m_rect.right-13)/kf);
			if (y_min<0) y_min=0;
			if (y_max>yf) y_max=yf;
			if (x_min<0) x_min=0;
			if (x_max>xf) x_max=xf;
			if ((x_max==x_min) || (y_max==y_min)) {
				y_min=0;
				y_max=yf;
				x_min=0;
				x_max=xf;
			}
		}
		else {
			y_min=0;
			y_max=yf;
			x_min=0;
			x_max=xf;
		}
		oben=(double) (yf-y_min);
		unten=(double) (yf-y_max);
		links=(double) x_min;
		rechts=(double) x_max;
		P[0][1]=(Homograph[1][1][1]*oben+Homograph[1][1][2]*links+Homograph[1][1][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*links+Homograph[1][3][3]);
		P[0][0]=(Homograph[1][2][1]*oben+Homograph[1][2][2]*links+Homograph[1][2][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*links+Homograph[1][3][3]);
		P[1][1]=(Homograph[1][1][1]*oben+Homograph[1][1][2]*rechts+Homograph[1][1][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[1][0]=(Homograph[1][2][1]*oben+Homograph[1][2][2]*rechts+Homograph[1][2][3])/(Homograph[1][3][1]*oben+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[2][1]=(Homograph[1][1][1]*unten+Homograph[1][1][2]*rechts+Homograph[1][1][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[2][0]=(Homograph[1][2][1]*unten+Homograph[1][2][2]*rechts+Homograph[1][2][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*rechts+Homograph[1][3][3]);
		P[3][1]=(Homograph[1][1][1]*unten+Homograph[1][1][2]*links+Homograph[1][1][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*links+Homograph[1][3][3]);
		P[3][0]=(Homograph[1][2][1]*unten+Homograph[1][2][2]*links+Homograph[1][2][3])/(Homograph[1][3][1]*unten+Homograph[1][3][2]*links+Homograph[1][3][3]);			
		A_alt=fabs((P[0][0]-P[1][0])*(P[0][1]-P[3][1])-(P[0][1]-P[1][1])*(P[0][0]-P[3][0]));
		A_alt+=fabs((P[2][0]-P[1][0])*(P[2][1]-P[3][1])-(P[2][1]-P[1][1])*(P[2][0]-P[3][0]));
		A_alt*=0.5;
		x=x_max-x_min;
		y=y_max-y_min;
		A_neu=x*y;
		for (i=0; i<4; i++) {
			free(P[i]);
		}
		free(P);
		if ((A_neu>1) && (A_alt>A_neu)) ifak=sqrt(A_alt/A_neu);
		ind=0;
		while ((ifak2<ifak) && (x<xf) && (y<yf)) {
			ifak2+=0.01;
			x=(int) (ifak2*(x_max-x_min)+0.001);
			y=(int) (ifak2*(y_max-y_min)+0.001);
			ind=1;
		}
		if (ind) {
			ifak2-=0.01;
			x=x_max-x_min;
			y=y_max-y_min;
		}
	}
	if ((beschneiden) && (!(selektion_vorhanden>1)))  {
		cutter(xf,yf,max_oben,max_unten,max_links,max_rechts);
		y_min=yf-((int) max_oben);
		y_max=yf-2-((int) max_unten);
		x_min=((int) max_links)+1;
		x_max=((int) max_rechts)-1;
		x=x_max-x_min;
		y=y_max-y_min;
		if ((x<5) || (x>xf) || (y<5) || (y>yf)) {
			x=xf;
			y=yf;
			y_min=0;
			y_max=yf;
			x_min=0;
			x_max=xf;
		}
	}
	progmax=0;
	progmax2=0;
	if (!savedisk) {
		bilderart2=0;
		bilderart=1;
	}
	if (!bilderart2) {
		if (bilderart&1) { 
			progmax+=1;
			progmax2=1;
		}
		if (bilderart&2) {
			progmax+=1;
			progmax2=1;
		}
		if (bilderart&4) {
			progmax+=1;
			progmax2=1;
		}
	}
	else {
		if (bilderart2&1) { 
			progmax+=1;
			progmax2=1;
		}
		if (bilderart2&2) {
			progmax+=1;
			progmax2=1;
		}
		if (bilderart2&4) {
			progmax+=1;
			progmax2=1;
		}
	}
	max_statt_i=(int) (x*ifak2+0.0005);
	max_statt_j=(int) (y*ifak2+0.0005);
	ergvo_old=ergvo;
	struvo_old=struvo;
	ergvo=0;
	struvo=0;
	progdlg.Create();
	progdlg.SetRange(0,20+progmax*max_statt_j*mfaktor+2*progmax2*max_statt_i*mfaktor);
	progdlg.SetStep(1);
	progdlg.SetPos(19);
	switch (sprach) {
		case 1:
			if (kubik) progdlg.SetCaptionText("ShiftN - Bicubic resampling");
			else progdlg.SetCaptionText("ShiftN - Bilinear resampling");
			break;
		case 2:
			if (kubik) progdlg.SetCaptionText("ShiftN - Ré-échantillonnage Bicubique");
			else progdlg.SetCaptionText("ShiftN - Ré-échantillonnage Bilinéaire");
			break;
		case 3:
			if (kubik) progdlg.SetCaptionText("ShiftN - Remuestreado bicúbico");
			else progdlg.SetCaptionText("ShiftN - Remuestreado bilineal");
			break;
		default:
			if (kubik) progdlg.SetCaptionText("ShiftN - Bikubische Interpolation");
			else progdlg.SetCaptionText("ShiftN - Bilineare Interpolation");
			break;
	}
	if  (((!bilderart2) && ((bilderart&1) || (bilderart&2) || (bilderart&4))) || ((bilderart2) && ((bilderart2&1) || (bilderart2&2) || (bilderart2&4)))) {
		if (!(((!bilderart2) && (bilderart==4) && (tiff16_vorhanden)) || ((bilderart2) && (bilderart2==4) && (tiff16_vorhanden)))) {
			diparam=distortion/(x_global*x_global+y_global*y_global);
			if (distortion>=0) {
				if (xm<ym) {
					fko=1+diparam*(xm-1)*(xm-1);
				}
				else {
					fko=1+diparam*(ym-1)*(ym-1);
				}
			}
			else {
				fko=1+diparam*((xm+1)*(xm+1)+(ym+1)*(ym+1));
			}
			max_statt_ip=max_statt_i;
			max_statt_jp=max_statt_j;
			yfp=yf;
			y_maxp=y_max;
			ifak2p=ifak2;
			x_minp=x_min;
			fkop=fko;
			diparamp=diparam;
			xmp=xm;
			ymp=ym;
			hCompleted=(HANDLE*) malloc(2*sizeof(HANDLE));
			for (part=0; part<2; part++) hCompleted[part]=CreateEvent(NULL,TRUE,FALSE,NULL);
			for (part=0; part<2; part++) {
				ResetEvent(hCompleted[part]);
				pj=new int(part);
				AfxBeginThread(interpol_row,pj,THREAD_PRIORITY_NORMAL,0,0,NULL);
				if (!part) {
					for (k=0; k<(max_statt_ip*mfaktor)/2; k++) {
						progdlg.StepIt();
					}
				}
				else {
					for (k=(max_statt_ip*mfaktor)/2; k<max_statt_ip*mfaktor; k++) {
						progdlg.StepIt();
					}
				}
			}
			WaitForMultipleObjects(2,hCompleted,TRUE,INFINITE);
			free(hCompleted);
		}
	}
	if (((!bilderart2) && (bilderart&4)) || ((bilderart2) && (bilderart2&4))) {
		if (tiff16_vorhanden) {
			diparam=distortion/(x_global*x_global+y_global*y_global);
			if (distortion>=0) {
				if (xm<ym) {
					fko=1+diparam*(xm-1)*(xm-1);
				}
				else {
					fko=1+diparam*(ym-1)*(ym-1);
				}
			}
			else {
				fko=1+diparam*((xm+1)*(xm+1)+(ym+1)*(ym+1));
			}
			max_statt_ip=max_statt_i;
			max_statt_jp=max_statt_j;
			yfp=yf;
			y_maxp=y_max;
			ifak2p=ifak2;
			x_minp=x_min;
			fkop=fko;
			diparamp=diparam;
			xmp=xm;
			ymp=ym;
			hCompleted=(HANDLE*) malloc(2*sizeof(HANDLE));
			for (part=0; part<2; part++) hCompleted[part]=CreateEvent(NULL,TRUE,FALSE,NULL);
			for (part=0; part<2; part++) {
				ResetEvent(hCompleted[part]);
				pj=new int(part);
				AfxBeginThread(interpol16_row,pj,THREAD_PRIORITY_NORMAL,0,0,NULL);
				if (((!bilderart2) && (bilderart==4)) || ((bilderart2) && (bilderart2==4))) {
					if (!part) {
						for (k=0; k<(max_statt_ip*mfaktor)/2; k++) {
							progdlg.StepIt();
						}
					}
					else {
						for (k=(max_statt_ip*mfaktor)/2; k<max_statt_ip*mfaktor; k++) {
							progdlg.StepIt();
						}
					}
				}
			}
			WaitForMultipleObjects(2,hCompleted,TRUE,INFINITE);
			free(hCompleted);
			datei2n=dateiname.Left(dateiname.GetLength()-3)+"tif";
			dif=TIFFOpen(datei2n,"w");
			if (dif) {
				x_neu=skala_x*max_statt_i*mfaktor;
				y_neu=skala_y*max_statt_j*mfaktor;
				TIFFSetField(dif,TIFFTAG_IMAGEWIDTH,x_neu);
				TIFFSetField(dif,TIFFTAG_IMAGELENGTH,y_neu);
				TIFFSetField(dif,TIFFTAG_BITSPERSAMPLE,16);
				TIFFSetField(dif,TIFFTAG_XRESOLUTION,(float) dpisettingx);
				TIFFSetField(dif,TIFFTAG_YRESOLUTION,(float) dpisettingy);
				TIFFSetField(dif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
				TIFFSetField(dif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);
				TIFFSetField(dif,TIFFTAG_SAMPLESPERPIXEL,3);
				TIFFSetField(dif,TIFFTAG_ROWSPERSTRIP,1);
				TIFFSetField(dif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
				TIFFSetField(dif,TIFFTAG_RESOLUTIONUNIT,RESUNIT_INCH);
				TIFFSetField(dif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
				tiff16_scanline=(uint16*) calloc(3*x_neu,sizeof(uint16));
				for (j=0; j<max_statt_j*mfaktor; j++) {
					ll=0;
					progdlg.StepIt();
					for (i=0; i<max_statt_i*mfaktor; i++) {
						for (u=0; u<skala_x; u++) {
							if (!schaerfen) {
								tiff16_scanline[ll]=tiff16n[2+mlookauf[j]+mlookdrei[i]];
								tiff16_scanline[ll+1]=tiff16n[1+mlookauf[j]+mlookdrei[i]];
								tiff16_scanline[ll+2]=tiff16n[mlookauf[j]+mlookdrei[i]];
								ll+=3;
							}
							else {
								if ((j==0) || (j==max_statt_j*mfaktor-1) || (i==0) || (i==max_statt_i*mfaktor-1)) {
									tiff16_scanline[ll]=tiff16n[2+mlookauf[j]+mlookdrei[i]];
									tiff16_scanline[ll+1]=tiff16n[1+mlookauf[j]+mlookdrei[i]];
									tiff16_scanline[ll+2]=tiff16n[mlookauf[j]+mlookdrei[i]];
									ll+=3;
								}
								else {
									sollr=(int) (tiff16n[2+mlookdrei[i-1]+mlookauf[j-1]]);
									sollr+=((int) (tiff16n[2+mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollr+=(int) (tiff16n[2+mlookdrei[i-1]+mlookauf[j+1]]);
									sollr+=((int) (tiff16n[2+mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollr+=((int) (tiff16n[2+mlookdrei[i]+mlookauf[j]]))<<2;
									sollr+=((int) (tiff16n[2+mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollr+=(int) (tiff16n[2+mlookdrei[i+1]+mlookauf[j-1]]);
									sollr+=((int) (tiff16n[2+mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollr+=(int) (tiff16n[2+mlookdrei[i+1]+mlookauf[j+1]]);
									sollr/=16.0;	
									sollg=(int) (tiff16n[1+mlookdrei[i-1]+mlookauf[j-1]]);
									sollg+=((int) (tiff16n[1+mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollg+=(int) (tiff16n[1+mlookdrei[i-1]+mlookauf[j+1]]);
									sollg+=((int) (tiff16n[1+mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollg+=((int) (tiff16n[1+mlookdrei[i]+mlookauf[j]]))<<2;
									sollg+=((int) (tiff16n[1+mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollg+=(int) (tiff16n[1+mlookdrei[i+1]+mlookauf[j-1]]);
									sollg+=((int) (tiff16n[1+mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollg+=(int) (tiff16n[1+mlookdrei[i+1]+mlookauf[j+1]]);
									sollg/=16.0;
									sollb=(int) (tiff16n[mlookdrei[i-1]+mlookauf[j-1]]);
									sollb+=((int) (tiff16n[mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollb+=(int) (tiff16n[mlookdrei[i-1]+mlookauf[j+1]]);
									sollb+=((int) (tiff16n[mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollb+=((int) (tiff16n[mlookdrei[i]+mlookauf[j]]))<<2;
									sollb+=((int) (tiff16n[mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollb+=(int) (tiff16n[mlookdrei[i+1]+mlookauf[j-1]]);
									sollb+=((int) (tiff16n[mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollb+=(int) (tiff16n[mlookdrei[i+1]+mlookauf[j+1]]);
									sollb/=16.0;
									diffr=(int) (tiff16n[2+mlookdrei[i]+mlookauf[j]])-sollr;
									diffg=(int) (tiff16n[1+mlookdrei[i]+mlookauf[j]])-sollg;
									diffb=(int) (tiff16n[mlookdrei[i]+mlookauf[j]])-sollb;
									wirdr=sollr+schaerfungsparameter*diffr;
									wirdg=sollg+schaerfungsparameter*diffg;
									wirdb=sollb+schaerfungsparameter*diffb;
									if (wirdr>65535.9) wirdr=65535.9;
									if (wirdr<0.1) wirdr=0.1;
									if (wirdg>65535.9) wirdg=65535.9;
									if (wirdg<0.1) wirdg=0.1;
									if (wirdb>65535.9) wirdb=65535.9;
									if (wirdb<0.1) wirdb=0.1;
									tiff16_scanline[ll]=(uint16) wirdr;
									tiff16_scanline[ll+1]=(uint16) wirdg;
									tiff16_scanline[ll+2]=(uint16) wirdb;
									ll+=3;
								}
							}
						}
					}
					for (v=0; v<skala_y; v++) {
						TIFFWriteScanline(dif,tiff16_scanline,j*skala_y+v,0);
					}
				}
				free(tiff16_scanline);
				TIFFClose(dif);
			}
		}
		else {
			datei2n=dateiname.Left(dateiname.GetLength()-3)+"tif";
			dif=TIFFOpen(datei2n,"w");
			if (dif) {
				x_neu=skala_x*max_statt_i*mfaktor;
				y_neu=skala_y*max_statt_j*mfaktor;
				TIFFSetField(dif,TIFFTAG_IMAGEWIDTH,x_neu);
				TIFFSetField(dif,TIFFTAG_IMAGELENGTH,y_neu);
				TIFFSetField(dif,TIFFTAG_BITSPERSAMPLE,8);
				TIFFSetField(dif,TIFFTAG_XRESOLUTION,(float) dpisettingx);
				TIFFSetField(dif,TIFFTAG_YRESOLUTION,(float) dpisettingy);
				TIFFSetField(dif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
				TIFFSetField(dif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);
				TIFFSetField(dif,TIFFTAG_SAMPLESPERPIXEL,3);
				TIFFSetField(dif,TIFFTAG_ROWSPERSTRIP,1);
				TIFFSetField(dif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
				TIFFSetField(dif,TIFFTAG_RESOLUTIONUNIT,RESUNIT_INCH);
				TIFFSetField(dif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
				tiff_scanline=(unsigned char*) calloc(3*x_neu,sizeof(unsigned char));
				for (j=0; j<max_statt_j*mfaktor; j++) {
					ll=0;
					progdlg.StepIt();
					for (i=0; i<max_statt_i*mfaktor; i++) {
						for (u=0; u<skala_x; u++) {
							if (!schaerfen) {
								tiff_scanline[ll]=(unsigned char) nbild[2+mlookauf[j]+mlookdrei[i]];
								tiff_scanline[ll+1]=(unsigned char) nbild[1+mlookauf[j]+mlookdrei[i]];
								tiff_scanline[ll+2]=(unsigned char) nbild[mlookauf[j]+mlookdrei[i]];
								ll+=3;
							}
							else {
								if ((j==0) || (j==max_statt_j*mfaktor-1) || (i==0) || (i==max_statt_i*mfaktor-1)) {
									tiff_scanline[ll]=(unsigned char) nbild[2+mlookauf[j]+mlookdrei[i]];
									tiff_scanline[ll+1]=(unsigned char) nbild[1+mlookauf[j]+mlookdrei[i]];
									tiff_scanline[ll+2]=(unsigned char) nbild[mlookauf[j]+mlookdrei[i]];
									ll+=3;
								}
								else {
									sollr=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j-1]]);
									sollr+=((int) (nbild[2+mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollr+=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j+1]]);
									sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j]]))<<2;
									sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j-1]]);
									sollr+=((int) (nbild[2+mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j+1]]);
									sollr/=16.0;	
									sollg=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j-1]]);
									sollg+=((int) (nbild[1+mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollg+=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j+1]]);
									sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j]]))<<2;
									sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j-1]]);
									sollg+=((int) (nbild[1+mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j+1]]);
									sollg/=16.0;
									sollb=(int) (nbild[mlookdrei[i-1]+mlookauf[j-1]]);
									sollb+=((int) (nbild[mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollb+=(int) (nbild[mlookdrei[i-1]+mlookauf[j+1]]);
									sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j]]))<<2;
									sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j-1]]);
									sollb+=((int) (nbild[mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j+1]]);
									sollb/=16.0;
									diffr=(int) (nbild[2+mlookdrei[i]+mlookauf[j]])-sollr;
									diffg=(int) (nbild[1+mlookdrei[i]+mlookauf[j]])-sollg;
									diffb=(int) (nbild[mlookdrei[i]+mlookauf[j]])-sollb;
									wirdr=sollr+schaerfungsparameter*diffr;
									wirdg=sollg+schaerfungsparameter*diffg;
									wirdb=sollb+schaerfungsparameter*diffb;
									if (wirdr>255.9) wirdr=255.9;
									if (wirdr<0.1) wirdr=0.1;
									if (wirdg>255.9) wirdg=255.9;
									if (wirdg<0.1) wirdg=0.1;
									if (wirdb>255.9) wirdb=255.9;
									if (wirdb<0.1) wirdb=0.1;
									tiff_scanline[ll]=(unsigned char) ((int) wirdr);
									tiff_scanline[ll+1]=(unsigned char) ((int) wirdg);
									tiff_scanline[ll+2]=(unsigned char) ((int) wirdb);
									ll+=3;
								}
							}
						}
					}
					for (v=0; v<skala_y; v++) {
						TIFFWriteScanline(dif,tiff_scanline,j*skala_y+v,0);
					}
				}
				free(tiff_scanline);
				TIFFClose(dif);
			}
		}
	}
	if (((!bilderart2) && (bilderart&2)) || ((bilderart2) && (bilderart2&2))) {
		dateiname=dateiname.Left(dateiname.GetLength()-3)+"bmp";
		ausgabedatei.open(dateiname,ios::out|ios::binary);
		if (ausgabedatei) {
			x_neu=skala_x*max_statt_i*mfaktor;
			y_neu=skala_y*max_statt_j*mfaktor;
			header(x_neu,y_neu);
			x_neu_auf=aufgerundet(3*x_neu);
			for (j=max_statt_j*mfaktor-1; j>=0; j--) {
				progdlg.StepIt();
				for (v=0; v<skala_y; v++) {
					for (i=0; i<max_statt_i*mfaktor; i++) {
						for (u=0; u<skala_x; u++) {
							if (!schaerfen) {
								pb(i_char((int) nbild[mlookauf[j]+mlookdrei[i]]));
								pb(i_char((int) nbild[mlookauf[j]+mlookdrei[i]+1]));
								pb(i_char((int) nbild[mlookauf[j]+mlookdrei[i]+2]));
							}
							else {
								if ((j==0) || (j==max_statt_j*mfaktor-1) || (i==0) || (i==max_statt_i*mfaktor-1)) {
									pb(i_char((int) nbild[mlookauf[j]+mlookdrei[i]]));
									pb(i_char((int) nbild[mlookauf[j]+mlookdrei[i]+1]));
									pb(i_char((int) nbild[mlookauf[j]+mlookdrei[i]+2]));
								}
								else {
									sollr=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j-1]]);
									sollr+=((int) (nbild[2+mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollr+=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j+1]]);
									sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j]]))<<2;
									sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j-1]]);
									sollr+=((int) (nbild[2+mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j+1]]);
									sollr/=16.0;	
									sollg=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j-1]]);
									sollg+=((int) (nbild[1+mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollg+=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j+1]]);
									sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j]]))<<2;
									sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j-1]]);
									sollg+=((int) (nbild[1+mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j+1]]);
									sollg/=16.0;
									sollb=(int) (nbild[mlookdrei[i-1]+mlookauf[j-1]]);
									sollb+=((int) (nbild[mlookdrei[i-1]+mlookauf[j]]))<<1;
									sollb+=(int) (nbild[mlookdrei[i-1]+mlookauf[j+1]]);
									sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j-1]]))<<1;
									sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j]]))<<2;
									sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j+1]]))<<1;
									sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j-1]]);
									sollb+=((int) (nbild[mlookdrei[i+1]+mlookauf[j]]))<<1;
									sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j+1]]);
									sollb/=16.0;
									diffr=(int) (nbild[2+mlookdrei[i]+mlookauf[j]])-sollr;
									diffg=(int) (nbild[1+mlookdrei[i]+mlookauf[j]])-sollg;
									diffb=(int) (nbild[mlookdrei[i]+mlookauf[j]])-sollb;
									wirdr=sollr+schaerfungsparameter*diffr;
									wirdg=sollg+schaerfungsparameter*diffg;
									wirdb=sollb+schaerfungsparameter*diffb;
									if (wirdr>255.9) wirdr=255.9;
									if (wirdr<0.1) wirdr=0.1;
									if (wirdg>255.9) wirdg=255.9;
									if (wirdg<0.1) wirdg=0.1;
									if (wirdb>255.9) wirdb=255.9;
									if (wirdb<0.1) wirdb=0.1;
									pb(i_char((int) wirdb));
									pb(i_char((int) wirdg));
									pb(i_char((int) wirdr));
								}
							}
						}
					}
					for (i=3*x_neu; i<x_neu_auf; i++) pb((char) 0);
				}
			}
		}
		ausgabedatei.close();
		ausgabedatei.clear();
	}
	if (((!bilderart2) && (bilderart&1)) || ((bilderart2) && (bilderart2&1))) {
		if (savedisk) {
			if (saveas) {
				dateiname = dateiname.Left(dateiname.GetLength() - 3) + "jpg";
			} else {
				if (m_suffix.GetLength()) {
					dateiname = dateiname.Left(dateiname.GetLength() - 3) + "jpg";
				}
				else {
					dateiname = dateiname.Left(dateiname.GetLength() - 4) + "_tmp.jpg";
				}
			}
			ausgabedatei.open(dateiname,ios::out|ios::binary);
			if (ausgabedatei) {
				x_neu=skala_x*max_statt_i*mfaktor;
				y_neu=skala_y*max_statt_j*mfaktor;
				image_height=y_neu;
				image_width=x_neu;
				image_buffer=(JSAMPLE*) calloc(x_neu*y_neu*3,sizeof(JSAMPLE));
				ll=0;
				for (j=0; j<max_statt_j*mfaktor; j++) {
					progdlg.StepIt();
					for (v=0; v<skala_y; v++) {
						for (i=0; i<max_statt_i*mfaktor; i++) {
							for (u=0; u<skala_x; u++) {
								if (!schaerfen) {
									image_buffer[ll]=(JSAMPLE) nbild[2+mlookauf[j]+mlookdrei[i]];
									image_buffer[ll+1]=(JSAMPLE) nbild[1+mlookauf[j]+mlookdrei[i]];
									image_buffer[ll+2]=(JSAMPLE) nbild[mlookauf[j]+mlookdrei[i]];
									ll+=3;
								}
								else {
									if ((j==0) || (j==max_statt_j*mfaktor-1) || (i==0) || (i==max_statt_i*mfaktor-1)) {
										image_buffer[ll]=(JSAMPLE) nbild[2+mlookauf[j]+mlookdrei[i]];
										image_buffer[ll+1]=(JSAMPLE) nbild[1+mlookauf[j]+mlookdrei[i]];
										image_buffer[ll+2]=(JSAMPLE) nbild[mlookauf[j]+mlookdrei[i]];
										ll+=3;
									}
									else {
										sollr=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j-1]]);
										sollr+=((int) (nbild[2+mlookdrei[i-1]+mlookauf[j]]))<<1;
										sollr+=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j+1]]);
										sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j-1]]))<<1;
										sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j]]))<<2;
										sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j+1]]))<<1;
										sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j-1]]);
										sollr+=((int) (nbild[2+mlookdrei[i+1]+mlookauf[j]]))<<1;
										sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j+1]]);
										sollr/=16.0;	
										sollg=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j-1]]);
										sollg+=((int) (nbild[1+mlookdrei[i-1]+mlookauf[j]]))<<1;
										sollg+=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j+1]]);
										sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j-1]]))<<1;
										sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j]]))<<2;
										sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j+1]]))<<1;
										sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j-1]]);
										sollg+=((int) (nbild[1+mlookdrei[i+1]+mlookauf[j]]))<<1;
										sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j+1]]);
										sollg/=16.0;
										sollb=(int) (nbild[mlookdrei[i-1]+mlookauf[j-1]]);
										sollb+=((int) (nbild[mlookdrei[i-1]+mlookauf[j]]))<<1;
										sollb+=(int) (nbild[mlookdrei[i-1]+mlookauf[j+1]]);
										sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j-1]]))<<1;
										sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j]]))<<2;
										sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j+1]]))<<1;
										sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j-1]]);
										sollb+=((int) (nbild[mlookdrei[i+1]+mlookauf[j]]))<<1;
										sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j+1]]);
										sollb/=16.0;
										diffr=(int) (nbild[2+mlookdrei[i]+mlookauf[j]])-sollr;
										diffg=(int) (nbild[1+mlookdrei[i]+mlookauf[j]])-sollg;
										diffb=(int) (nbild[mlookdrei[i]+mlookauf[j]])-sollb;
										wirdr=sollr+schaerfungsparameter*diffr;
										wirdg=sollg+schaerfungsparameter*diffg;
										wirdb=sollb+schaerfungsparameter*diffb;
										if (wirdr>255.9) wirdr=255.9;
										if (wirdr<0.1) wirdr=0.1;
										if (wirdg>255.9) wirdg=255.9;
										if (wirdg<0.1) wirdg=0.1;
										if (wirdb>255.9) wirdb=255.9;
										if (wirdb<0.1) wirdb=0.1;
										image_buffer[ll]=(JSAMPLE) ((int) wirdr);
										image_buffer[ll+1]=(JSAMPLE) ((int) wirdg);
										image_buffer[ll+2]=(JSAMPLE) ((int) wirdb);
										ll+=3;
									}
								}
							}
						}
					}
				}
				write_JPEG_file(dateiname.GetBuffer(dateiname.GetLength()),jpegquality);
				free(image_buffer);
			}
			ausgabedatei.close();
			ausgabedatei.clear();
			if (saveas) {
			} else {
				if (!m_suffix.GetLength()) {
					// Dans tous les cas il ecrase le fichier original et le remplace par le nouveau fichier
					// y compris avec la fonction "enregistrer sous..."
					remove(m_dateiname.GetBuffer(m_dateiname.GetLength()));
					rename(dateiname.GetBuffer(dateiname.GetLength()), m_dateiname.GetBuffer(m_dateiname.GetLength()));
				}
			}
		}
		else {
			x_neu=max_statt_i*mfaktor;
			y_neu=max_statt_j*mfaktor;
			x_neu_auf=aufgerundet(3*x_neu);
			clipbild=(BYTE*) calloc(40+x_neu_auf*y_neu,sizeof(BYTE));
			headerc(x_neu,y_neu);
			for (j=max_statt_j*mfaktor-1; j>=0; j--) {
				progdlg.StepIt();
				for (i=0; i<max_statt_i*mfaktor; i++) {
					if (!schaerfen) {
						clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i]=nbild[mlookauf[j]+mlookdrei[i]];
						clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i+1]=nbild[mlookauf[j]+mlookdrei[i]+1];
						clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i+2]=nbild[mlookauf[j]+mlookdrei[i]+2];
					}
					else {
						if ((j==0) || (j==max_statt_j*mfaktor-1) || (i==0) || (i==max_statt_i*mfaktor-1)) {
							clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i]=nbild[mlookauf[j]+mlookdrei[i]];
							clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i+1]=nbild[mlookauf[j]+mlookdrei[i]+1];
							clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i+2]=nbild[mlookauf[j]+mlookdrei[i]+2];
						}
						else {
							sollr=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j-1]]);
							sollr+=((int) (nbild[2+mlookdrei[i-1]+mlookauf[j]]))<<1;
							sollr+=(int) (nbild[2+mlookdrei[i-1]+mlookauf[j+1]]);
							sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j-1]]))<<1;
							sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j]]))<<2;
							sollr+=((int) (nbild[2+mlookdrei[i]+mlookauf[j+1]]))<<1;
							sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j-1]]);
							sollr+=((int) (nbild[2+mlookdrei[i+1]+mlookauf[j]]))<<1;
							sollr+=(int) (nbild[2+mlookdrei[i+1]+mlookauf[j+1]]);
							sollr/=16.0;	
							sollg=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j-1]]);
							sollg+=((int) (nbild[1+mlookdrei[i-1]+mlookauf[j]]))<<1;
							sollg+=(int) (nbild[1+mlookdrei[i-1]+mlookauf[j+1]]);
							sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j-1]]))<<1;
							sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j]]))<<2;
							sollg+=((int) (nbild[1+mlookdrei[i]+mlookauf[j+1]]))<<1;
							sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j-1]]);
							sollg+=((int) (nbild[1+mlookdrei[i+1]+mlookauf[j]]))<<1;
							sollg+=(int) (nbild[1+mlookdrei[i+1]+mlookauf[j+1]]);
							sollg/=16.0;
							sollb=(int) (nbild[mlookdrei[i-1]+mlookauf[j-1]]);
							sollb+=((int) (nbild[mlookdrei[i-1]+mlookauf[j]]))<<1;
							sollb+=(int) (nbild[mlookdrei[i-1]+mlookauf[j+1]]);
							sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j-1]]))<<1;
							sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j]]))<<2;
							sollb+=((int) (nbild[mlookdrei[i]+mlookauf[j+1]]))<<1;
							sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j-1]]);
							sollb+=((int) (nbild[mlookdrei[i+1]+mlookauf[j]]))<<1;
							sollb+=(int) (nbild[mlookdrei[i+1]+mlookauf[j+1]]);
							sollb/=16.0;
							diffr=(int) (nbild[2+mlookdrei[i]+mlookauf[j]])-sollr;
							diffg=(int) (nbild[1+mlookdrei[i]+mlookauf[j]])-sollg;
							diffb=(int) (nbild[mlookdrei[i]+mlookauf[j]])-sollb;
							wirdr=sollr+schaerfungsparameter*diffr;
							wirdg=sollg+schaerfungsparameter*diffg;
							wirdb=sollb+schaerfungsparameter*diffb;
							if (wirdr>255.9) wirdr=255.9;
							if (wirdr<0.1) wirdr=0.1;
							if (wirdg>255.9) wirdg=255.9;
							if (wirdg<0.1) wirdg=0.1;
							if (wirdb>255.9) wirdb=255.9;
							if (wirdb<0.1) wirdb=0.1;
							clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i]=(BYTE) wirdb;
							clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i+1]=(BYTE) wirdg;
							clipbild[40+x_neu_auf*(max_statt_j*mfaktor-1-j)+3*i+2]=(BYTE) wirdr;
						}
					}
				}
			}
		}
	}
	if (((!bilderart2) && ((bilderart&1) || (bilderart&2) || (bilderart&4))) || ((bilderart2) && ((bilderart2&1) || (bilderart2&2) || (bilderart2&4)))) {
		for (i=0; i<max_statt_i*mfaktor; i+=1) {
			progdlg.StepIt();
		}
	}
	ergvo=ergvo_old;
	struvo=struvo_old;
	progdlg.DestroyWindow();
	bilderart2=tmpbilderart2;
	bilderart=tmpbilderart;
}

void CShiftNView::OnSaveExit() 
{	
	int i;
	int ind;
	int x_neu;
	int y_neu;
	CString dateiname;
	Keine_Ergebnisse noresultsdlg;
	CDC* pDC;
	if (mhshowgui) pDC=GetDC();
	if (!ergebnis_vorhanden) {
		noresultsdlg.DoModal();
	}
	else {
		if (direkt_speichern) dateiname=m_sdateiname;
		else dateiname=m_dateiname;
		ind=1;
		for (i=0; i<dateiname.GetLength(); i++) {
			if (dateiname.GetAt(i)=='.') ind=0;
			if (dateiname.GetAt(i)=='\\') ind=1;
		}
		if (ind) dateiname+=".bmp";
		if (!direkt_speichern) {
			i=dateiname.GetLength()-1;
			ind=1;
			while ((ind) && (i)) {
				if (dateiname.GetAt(i)=='.') ind=0;
				if (ind) i-=1;
			}
			dateiname=dateiname.Left(i)+m_suffix+".bmp";
		}
		if (!direkt_speichern_clipboard) {
			schreibe_bild(1,4,x_global,y_global,dateiname,1,1,x_neu,y_neu);
		}
		else {
			OnCtrlC();
		}
		if (mhshowgui) {
			if (AfxGetMainWnd!=NULL) {
				AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
			}
		}
	}
	if (mhshowgui) ReleaseDC(pDC);
}

void CShiftNView::OnDraw(CDC* pDC)
{
	int max_x;
	int max_y;
	unsigned long color;
	CBrush* MHBrush;
	CShiftNDoc* pDoc; 
	CString titu;
	CRect rect;
	if (mhshowgui) {
		pDoc=GetDocument();
		if (!speicher_voll) {
			switch (sprach) {
				case 1: pDoc->SetTitle("No file loaded"); break;
				case 2: pDoc->SetTitle("Aucun fichier chargé"); break;
				case 3: pDoc->SetTitle("No hay archivo cargado"); break;
				default: pDoc->SetTitle("Keine Datei geladen"); break;
			}
		}
		else {
			pDoc->SetTitle(m_dateiname);
		}
		GetClientRect(rect);
		max_x=rect.right;
		max_y=rect.bottom;
		ASSERT_VALID(pDoc);
		MHBrush=new(CBrush);
		color=15724527;
		MHBrush->CreateSolidBrush(color);
		pDC->FillRect(rect,MHBrush);
		bild_zeichnen(pDC,max_x,max_y);
		MHBrush->DeleteObject();
		delete(MHBrush);
		if (selektion_vorhanden>1) {
			pDC->SetROP2(R2_NOTXORPEN);
			pDC->Rectangle(&m_rect);
		}
		if ((direkt_speichern==3) && (direkt_laden==0)) {
			direkt_speichern=1;
		}
	}
}

void CShiftNView::OnSave() 
{	
	int i;
	int ind;
	int x_neu;
	int y_neu;
	CString dateiname;
	CDC* pDC;
	abspDlg absp_dlg;
	Keine_Ergebnisse noresultsdlg;
	if (!ergebnis_vorhanden) {
		noresultsdlg.DoModal();
	}
	else {
		if (mhshowgui) {
			pDC=GetDC();
			if (direkt_speichern) dateiname=m_sdateiname;
			else dateiname=m_dateiname;
			ind=1;
			for (i=0; i<dateiname.GetLength(); i++) {
				if (dateiname.GetAt(i)=='.') ind=0;
				if (dateiname.GetAt(i)=='\\') ind=1;
			}
			if (ind) dateiname+=".bmp";
			if (!direkt_speichern) {
				i=dateiname.GetLength()-1;
				ind=1;
				while ((ind) && (i)) {
					if (dateiname.GetAt(i)=='.') ind=0;
					if (ind) i-=1;
				}
				dateiname=dateiname.Left(i)+m_suffix+".bmp";
			}
			if (!direkt_speichern_clipboard) {
				schreibe_bild(1,4,x_global,y_global,dateiname,1,1,x_neu,y_neu);
			} else {
				OnCtrlC();
			}
			//absp_dlg.DoModal(); // Disable "file saved - clic on OK button" dialog box
			OnDraw(pDC);
			ReleaseDC(pDC);
		}
	}
}

void CShiftNView::OnSaveAs() 
{	
	int i;
	int ind;
	int tmp=0;
	int x_neu;
	int y_neu;
	CDC* pDC;
	CString dateiname;
	Keine_Ergebnisse noresultsdlg;
	CFileDialog CDatei(FALSE);
	switch (sprach) {
		case 1: CDatei.m_ofn.lpstrTitle="Save as..."; break;
		case 2: CDatei.m_ofn.lpstrTitle="Enregistrer sous..."; break;
		case 3: CDatei.m_ofn.lpstrTitle="Guardar como..."; break;
		default: CDatei.m_ofn.lpstrTitle="Speichern unter..."; break;
	}
	switch (sprach) {
		case 1: CDatei.m_ofn.lpstrFilter="(none)\0*.*\0\0"; break;
		case 2: CDatei.m_ofn.lpstrFilter="(Tous les fichiers)\0*.*\0\0"; break;
		case 3: CDatei.m_ofn.lpstrFilter="(Todos los archivos)\0*.*\0\0"; break;
		default: CDatei.m_ofn.lpstrFilter="(kein)\0*.*\0\0"; break;
	}
	if (selektion_vorhanden>1) tmp=selektion_vorhanden;
	if (!ergebnis_vorhanden) {
		noresultsdlg.DoModal();
	} else {
		if (direkt_speichern) dateiname=m_sdateiname; else dateiname=m_dateiname; // direkt_speichern = Enregistrer directement
		
		ind=1;
		for (i=0; i<dateiname.GetLength(); i++) {
			if (dateiname.GetAt(i)=='.') ind=0;
			if (dateiname.GetAt(i)=='\\') ind=1;
		}
		if (ind) dateiname+=".bmp";  // Not BMP file format, see inside schreibe_bild() function

		if (!direkt_speichern) {
			i=dateiname.GetLength()-1;
			ind=1;
			while ((ind) && (i)) {
				if (dateiname.GetAt(i)=='.') ind=0;
				if (ind) i-=1;
			}
			dateiname=dateiname.Left(i)+m_suffix;  // Not BMP file format, see inside schreibe_bild() function
		} else {
			i=dateiname.GetLength()-1;
			ind=1;
			while ((ind) && (i)) {
				if (dateiname.GetAt(i)=='.') ind=0;
				if (ind) i-=1;
			}
			dateiname=dateiname.Left(i); 
		}

		CDatei.m_ofn.lpstrFile=(char*) malloc(_MAX_PATH+1);
		for (i=0; i<dateiname.GetLength(); i++) CDatei.m_ofn.lpstrFile[i]=dateiname.GetAt(i);
		CDatei.m_ofn.lpstrFile[i]='\0';
		// Le nom est mis dans la boite de dialogue

		if (CDatei.DoModal()==IDOK) {
			pDC=GetDC();

			dateiname=CDatei.GetPathName();
						
			ind=1;
			for (i=0; i<dateiname.GetLength(); i++) {
				if (dateiname.GetAt(i)=='.') ind=0;
				if (dateiname.GetAt(i)=='\\') ind=1;
			}
			if (ind) dateiname+=".bmp"; // Not BMP file format, see inside schreibe_bild() function
			i=dateiname.GetLength()-1;
			ind=1;
			while ((ind) && (i)) {
				if (dateiname.GetAt(i)=='.') ind=0;
				if (ind) i-=1;
			}
			dateiname=dateiname.Left(i)+".bmp"; // Not BMP file format, see inside schreibe_bild() function
			
			/*
			// For debug only
			CString s;
			s.Format("%d", direkt_speichern);
			MessageBox(s);
			MessageBox(dateiname);
			*/

			selektion_vorhanden=tmp;
			
			// schreibe_bild() function can save .JPG or/and .BMP or/and .TIF image file
			// last parameter "true" is added to avoid schreibe_bild() erase original .JPG file if suffix = ""
			schreibe_bild(1,4,x_global,y_global,dateiname,1,1,x_neu,y_neu, true);
			
			OnDraw(pDC);
			ReleaseDC(pDC);
		}
		free(CDatei.m_ofn.lpstrFile);
	}
}

void CShiftNView::OnLoad() 
{
	CShiftNDoc* pDoc=GetDocument();
	CDC* pDC=GetDC();
	int i;
	int j;
	Ladefehler_Dialog errDlg;
	CFileDialog CDatei(TRUE);
	CDatei.m_ofn.lpstrFile=(char*) malloc(_MAX_PATH+1);
	CDatei.m_ofn.lpstrFile[0]='\0';
	switch (sprach) {
		case 1: CDatei.m_ofn.lpstrTitle="Open file"; break;
		case 2: CDatei.m_ofn.lpstrTitle="Ouvrir..."; break;
		case 3: CDatei.m_ofn.lpstrTitle="Abrir archivo"; break;
		default: CDatei.m_ofn.lpstrTitle="Bild laden"; break;
	}
	switch (sprach) {
		case 1: CDatei.m_ofn.lpstrFilter="Supported image formats\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0All files(*.*)\0*.*\0\0"; break;
		case 2: CDatei.m_ofn.lpstrFilter="Toutes les Images\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0Tous les fichiers(*.*)\0*.*\0\0"; break;
		case 3: CDatei.m_ofn.lpstrFilter="Formatos de imagen admitidos\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0Todos los archivos(*.*)\0*.*\0\0"; break;
		default: CDatei.m_ofn.lpstrFilter="Unterstützte Bilddateitypen\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0Alle Dateien(*.*)\0*.*\0\0"; break;
	}
	if (CDatei.DoModal()==IDOK) {
		m_dateiname=CDatei.GetPathName();
		if (batch_voll) {
			for (i=0; i<batch_anzahl; i++) free(batchname[i]);
			free(batchname);
			batch_voll=0;
			batch_anzahl=0;
		}
		if (speicher_voll) {
			speicher_voll=0;
			for (i=0; i<layer_c_intern; i++) {
				for (j=0; j<x_global; j++) {
					free(bild[i][j]);
				}
				free(bild[i]);
			}
			free(bild);
			if (tiff16_vorhanden) {
				free(tiff16r);
				free(tiff16n);
			}
			tiff16_vorhanden=0;
			if (exifstream_vorhanden) free(exifstream);
			exifstream_vorhanden=0;
			if (iptcstream_vorhanden) free(iptcstream);
			iptcstream_vorhanden=0;
			if (iccstream_vorhanden) free(icc_data_ptr);
			iccstream_vorhanden=FALSE;
			free(ebild);
			free(sbild);
			free(mbild);
			free(nbild);
			free(gbild);
			free(lookauf);
			free(lookdrei);
			free(mlookauf);
			free(mlookdrei);
			for (i=0; i<layer_l_intern; i++) {
				for (j=0; j<x_global; j++) {
					free(l_schicht[i][j]);
				}
				free(l_schicht[i]);
			}
			free(l_schicht);
			globwinkel=0;
			globparam=0;
			reset_param=globparam;
			reset_winkel=globwinkel;
			if (struktur_vorhanden) free(struktur);
			struktur_vorhanden=0;
			struvo=0;
			ergebnis_vorhanden=0;
			ergvo=0;
			selektion_vorhanden=0;
			x_global=0;
			y_global=0;
			mx_global=0;
			my_global=0;
			OnDraw(pDC);
		}	
		if (lese_bild(mx_global,my_global,m_dateiname)) {
			switch (sprach) {
				case 1: m_dateiname="An error occurred"; break;
				case 2: m_dateiname="Une erreur s'est produite"; break;
				case 3: m_dateiname="Ha ocurrido un error"; break;
				default: m_dateiname="Es ist ein Fehler aufgetreten"; break;
			}
			errDlg.DoModal();
		}
		else {
			preprocessing(glatt);
		}
		pDoc->SetTitle(m_dateiname);
	}
	else {
		if (!speicher_voll) {
			switch (sprach) {
				case 1: m_dateiname="No file loaded"; break;
				case 2: m_dateiname="Aucun fichier chargé"; break;
				case 3: m_dateiname="No hay archivo cargado"; break;
				default: m_dateiname="Keine Datei geladen"; break;
			}
		}
	}
	free(CDatei.m_ofn.lpstrFile);
	OnDraw(pDC);
	ReleaseDC(pDC);
}

long koord_von(int u, int v) {
	long ret;
	ret=u;
	ret=ret<<14;
	ret+=v;
	return ret;
}

long fill(int nr, int x1, int y1, long max) {
	char farbe;
	char schwarz;
	char oldcolor;
	int u;
	int v;
	int ind;
	long i;
	long pos;
	long count=max;
	long nebenan=0;
	long *position;
	Stack<long> stapel;
	Stack<long> stapel2;
	farbe=i_char(0);
	schwarz=i_char(255);
	oldcolor=bild[nr][x1][y1];
	stapel.push(koord_von(x1,y1));
	while (stapel.pop(pos)) {
		get_xy(pos,u,v);
		bild[nr][u][v]=farbe;
		count+=1;
		if (u<x_global-1) {
			if (bild[nr][u+1][v]!=oldcolor) {
				stapel2.push(koord_von(u+1,v));
				nebenan+=1;
			}
			if (v<y_global-1) {
				if (bild[nr][u+1][v+1]!=oldcolor) {
					stapel2.push(koord_von(u+1,v+1));
					nebenan+=1;
				}
			}
		}
		if (v<y_global-1) {
			if (bild[nr][u][v+1]!=oldcolor) {
				stapel2.push(koord_von(u,v+1));
				nebenan+=1;
			}
		}
		if (nachbar) {
			if (u>0) {
				if (v>0) {
					if (bild[nr][u-1][v-1]==oldcolor) {
						stapel.push(koord_von(u-1,v-1));
						bild[nr][u-1][v-1]=schwarz;
					}
				}
				if (bild[nr][u-1][v]==oldcolor) {
					stapel.push(koord_von(u-1,v));
					bild[nr][u-1][v]=schwarz;
				}
				if (v<y_global-1) {
					if (bild[nr][u-1][v+1]==oldcolor) {
						stapel.push(koord_von(u-1,v+1));
						bild[nr][u-1][v+1]=schwarz;
					}
				}
			}
			if (u<x_global-1) {
				if (v>0) {
					if (bild[nr][u+1][v-1]==oldcolor) {
						stapel.push(koord_von(u+1,v-1));
						bild[nr][u+1][v-1]=schwarz;
					}
				}
				if (bild[nr][u+1][v]==oldcolor) {
					stapel.push(koord_von(u+1,v));
					bild[nr][u+1][v]=schwarz;
				}
				if (v<y_global-1) {
					if (bild[nr][u+1][v+1]==oldcolor) {
						stapel.push(koord_von(u+1,v+1));
						bild[nr][u+1][v+1]=schwarz;
					}
				}
			}
			if (v>0) {
				if (bild[nr][u][v-1]==oldcolor) {
					stapel.push(koord_von(u,v-1));
					bild[nr][u][v-1]=schwarz;
				}
			}
			if (v<y_global-1) {
				if (bild[nr][u][v+1]==oldcolor) {
					stapel.push(koord_von(u,v+1));
					bild[nr][u][v+1]=schwarz;
				}
			}
		}
		else {
			if (v>0) {
				if (bild[nr][u][v-1]==oldcolor) {
					stapel.push(koord_von(u,v-1));
					bild[nr][u][v-1]=schwarz;
				}
			}
			if (v<y_global-1) {
				if (bild[nr][u][v+1]==oldcolor) {
					stapel.push(koord_von(u,v+1));
					bild[nr][u][v+1]=schwarz;
				}
			}
			if (u>0) {
				if (bild[nr][u-1][v]==oldcolor) {
					stapel.push(koord_von(u-1,v));
					bild[nr][u-1][v]=schwarz;
				}
			}
			if (u<x_global-1) {
				if (bild[nr][u+1][v]==oldcolor) {
					stapel.push(koord_von(u+1,v));
					bild[nr][u+1][v]=schwarz;
				}
			}
		}
	}
	if (nebenan) {
		position=(long*) calloc(nebenan,sizeof(long));
		nebenan=0;
		while (stapel2.pop(pos)) {
			i=0;
			ind=1;
			while ((ind) && (i<nebenan)) {
				if (pos==position[i]) {
					ind=0;
				}
				i+=1;
			}
			if (ind) {
				position[nebenan]=pos;
				nebenan+=1;
			}
		}
		free(position);
	}
	return count+nebenan;
}

long fill2(int nr, int x1, int y1, long max) {
	char farbe;
	char schwarz;
	char oldcolor;
	int u;
	int v;
	int ind;
	int min_x=x_global-1;
	int min_y=y_global-1;
	int max_x=0;
	int max_y=0;
	long i;
	long pos;
	long count=max+headersize;
	long count2=0;
	long nebenan=0;
	long *position;
	long betrquadr;
	Stack<long> stapel;
	Stack<long> stapel2;
	Stack<long> stapel3;
	farbe=i_char(0);
	schwarz=i_char(255);
	oldcolor=bild[nr][x1][y1];
	stapel.push(koord_von(x1,y1));
	struktur[max+8]=(long) c_int(oldcolor);
	struktur[max+6]=anzpart;
	while (stapel.pop(pos)) {
		stapel3.push(pos);
		get_xy(pos,u,v);
		if (u<min_x) min_x=u;
		if (u>max_x) max_x=u;
		if (v<min_y) min_y=v;
		if (v>max_y) max_y=v;
		bild[nr][u][v]=farbe;
		struktur[count]=pos;
		count+=1;
		count2+=1;
		if (u<x_global-1) {
			if (bild[nr][u+1][v]!=oldcolor) {
				stapel2.push(koord_von(u+1,v));
				nebenan+=1;
			}
			if (v<y_global-1) {
				if (bild[nr][u+1][v+1]!=oldcolor) {
					stapel2.push(koord_von(u+1,v+1));
					nebenan+=1;
				}
			}
		}
		if (v<y_global-1) {
			if (bild[nr][u][v+1]!=oldcolor) {
				stapel2.push(koord_von(u,v+1));
				nebenan+=1;
			}
		}
		if (nachbar) {
			if (u>0) {
				if (v>0) {
					if (bild[nr][u-1][v-1]==oldcolor) {
						stapel.push(koord_von(u-1,v-1));
						bild[nr][u-1][v-1]=schwarz;
					}
				}
				if (bild[nr][u-1][v]==oldcolor) {
					stapel.push(koord_von(u-1,v));
					bild[nr][u-1][v]=schwarz;
				}
				if (v<y_global-1) {
					if (bild[nr][u-1][v+1]==oldcolor) {
						stapel.push(koord_von(u-1,v+1));
						bild[nr][u-1][v+1]=schwarz;
					}
				}
			}
			if (u<x_global-1) {
				if (v>0) {
					if (bild[nr][u+1][v-1]==oldcolor) {
						stapel.push(koord_von(u+1,v-1));
						bild[nr][u+1][v-1]=schwarz;
					}
				}
				if (bild[nr][u+1][v]==oldcolor) {
					stapel.push(koord_von(u+1,v));
					bild[nr][u+1][v]=schwarz;
				}
				if (v<y_global-1) {
					if (bild[nr][u+1][v+1]==oldcolor) {
						stapel.push(koord_von(u+1,v+1));
						bild[nr][u+1][v+1]=schwarz;
					}
				}
			}
			if (v>0) {
				if (bild[nr][u][v-1]==oldcolor) {
					stapel.push(koord_von(u,v-1));
					bild[nr][u][v-1]=schwarz;
				}
			}
			if (v<y_global-1) {
				if (bild[nr][u][v+1]==oldcolor) {
					stapel.push(koord_von(u,v+1));
					bild[nr][u][v+1]=schwarz;
				}
			}
		}
		else {
			if (v>0) {
				if (bild[nr][u][v-1]==oldcolor) {
					stapel.push(koord_von(u,v-1));
					bild[nr][u][v-1]=schwarz;
				}
			}
			if (v<y_global-1) {
				if (bild[nr][u][v+1]==oldcolor) {
					stapel.push(koord_von(u,v+1));
					bild[nr][u][v+1]=schwarz;
				}
			}
			if (u>0) {
				if (bild[nr][u-1][v]==oldcolor) {
					stapel.push(koord_von(u-1,v));
					bild[nr][u-1][v]=schwarz;
				}
			}
			if (u<x_global-1) {
				if (bild[nr][u+1][v]==oldcolor) {
					stapel.push(koord_von(u+1,v));
					bild[nr][u+1][v]=schwarz;
				}
			}
		}
	}
	struktur[max+2]=count2;
	struktur[max]=1;
	struktur[max+4]=koord_von(min_x,min_y);
	struktur[max+5]=koord_von(max_x,max_y);
	betrquadr=(long) ((max_x-min_x)*(max_x-min_x)+(max_y-min_y)*(max_y-min_y));
	while (stapel3.pop(pos)) {
		get_xy(pos,u,v);
		l_schicht[nr][u][v]=betrquadr;
	}
	if (nebenan) {
		position=(long*) calloc(nebenan,sizeof(long));
		nebenan=0;
		while (stapel2.pop(pos)) {
			i=0;
			ind=1;
			while ((ind) && (i<nebenan)) {
				if (pos==position[i]) {
					ind=0;
				}
				i+=1;
			}
			if (ind) {
				position[nebenan]=pos;
				struktur[count+nebenan]=pos;
				nebenan+=1;
			}
		}
		free(position);
	}
	struktur[max+3]=nebenan;
	return count+nebenan;
}

void sortiere(int part, int links, int rechts) {
	int li=links;
	int re=rechts;
	int test=grauwerte[part][(li+re)/2];
	int hilf;
	while (li<=re) {
		while (grauwerte[part][li]<test) li+=1;
		while (grauwerte[part][re]>test) re-=1;
		if (li<=re) {
			hilf=grauwerte[part][li];
			grauwerte[part][li]=grauwerte[part][re];
			grauwerte[part][re]=hilf;
			li+=1;
			re-=1;
		}
	}
	if (links<re) sortiere(part,links,re);
	if (li<rechts) sortiere(part,li,rechts);
}

void bestimme_werte(double ad1, double ad2, int &u, int &v) {
	u=(int) (ad1+0.5);
	v=(int) (ad2+0.5);
}

UINT calc_row(LPVOID pParam) {
	int i;
	int j;
	int gv;
	int gh;
	int part;
	int j_min;
	int j_max;
	int j_mitte;
	int *partpoint;
	double betr;
	double w1;
	double w2;
	partpoint=(int*) pParam; 
	part=*partpoint;
	j_mitte=y_global/2;
	if (part==1) {
		j_min=j_mitte;
		j_max=y_global-2;
	}
	else {
		j_min=0;
		j_max=j_mitte-1;
	}
	for (j=j_min; j<=j_max; j++) {
		for (i=0; i<(x_global-1); i++) {
			gv=-c_int(bild[4][i][j])-c_int(bild[4][i][j+1])+c_int(bild[4][i+1][j])+c_int(bild[4][i+1][j+1]);
			gh=-c_int(bild[4][i][j])+c_int(bild[4][i][j+1])-c_int(bild[4][i+1][j])+c_int(bild[4][i+1][j+1]);
			betr=gv*gv+gh*gh;
			if (betr<sqmingrad) {
				bild[0][i][j]=i_char(0);
				bild[1][i][j]=i_char(0);
				bild[2][i][j]=i_char(0);
				bild[3][i][j]=i_char(0);
			}
			else {
				w1=atan2((double) gv,(double) gh);
				while (w1>=mypi) {
					w1=w1-twopi;
				}
				while (w1<-mypi) {
					w1=w1+twopi;
				}
				w2=w1+mypi/((double) anzpart);
				while (w2>=mypi) {
					w2=w2-twopi;
				}
				while (w2<-mypi) {
					w2=w2+twopi;
				}
				bild[0][i][j]=i_char((partitionswert(w1,wstep)<<1)-1);
				bild[1][i][j]=i_char(partitionswert(w2,wstep)<<1);
				bild[2][i][j]=bild[0][i][j];
				bild[3][i][j]=bild[1][i][j];
			}
		}
	}
	SetEvent(hCompleted[part]);
	delete partpoint;
	return 0;
}

UINT fill_row1(LPVOID pParam) {
	int i;
	int j;
	int *partpoint;
	int part;
	long max_gp;
	partpoint=(int*) pParam; 
	part=*partpoint;
	for (i=0; i<x_global; i++) {
		for (j=0; j<y_global; j++) {
			if (c_int(bild[2+part][i][j])>0) {
				max_gp=fill(2+part,i,j,maxp[part]);
				if ((max_gp-maxp[part])>=minsize) {
					countp[part]+=1;
					maxp[part]=max_gp;
				}
			}
		}
	}
	SetEvent(hCompleted[part]);
	delete partpoint;
	return 0;
}

UINT fill_row2(LPVOID pParam) {
	int i;
	int j;
	int *partpoint;
	int part;
	long max_gp;
	partpoint=(int*) pParam; 
	part=*partpoint;
	for (i=0; i<x_global; i++) {
		for (j=0; j<y_global; j++) {
			if (c_int(bild[part][i][j])>0) {
				if (((part==1) || (kp[part]<mitte_offset)) && ((part==0) || (kp[part]<struktur_vorhanden))) {
					max_gp=fill2(part,i,j,kp[part]);
					if ((max_gp-kp[part]-headersize)>=minsize) {
						struktur[kp[part]+1]=part;
						kp[part]=max_gp;
						countp[part]+=1;
					}
				}
			}
		}
	}
	SetEvent(hCompleted[part]);
	delete partpoint;
	return 0;
}

UINT sort_row(LPVOID pParam) {
	int j;
	int u;
	int v;
	long l;
	long k;
	long k_maxh;
	int contrast;
	int *partpoint;
	int part;
	partpoint=(int*) pParam; 
	part=*partpoint;
	if (part) {
		k=mitte_offset;
		k_maxh=struktur_vorhanden;
	}
	else {
		k=0;
		k_maxh=mitte_offset;
	}
	while (k<k_maxh) {
		if (struktur[k]>0) {
			for (l=0; l<struktur[k+2]+struktur[k+3]; l++) {
				get_xy(struktur[k+headersize+l],u,v);
				grauwerte[part][l]=c_int(bild[4][u][v]);
			}
			l=struktur[k+2]+struktur[k+3];
			sortiere(part,0,l-1);
			if (!(l>>3)) {
				contrast=grauwerte[part][l-1]-grauwerte[part][0];
			}
			else {
				contrast=0;
				for (j=0; j<(l>>3); j++) {
					contrast=contrast+grauwerte[part][l-j-1]-grauwerte[part][j];
				}
				contrast=contrast/(l>>3);
			}
			struktur[k+7]=(long) contrast;
			if (contrast<mincon) {
				struktur[k]=0;
			}
		}
		k+=headersize+struktur[k+2]+struktur[k+3];				
	}
	SetEvent(hCompleted[part]);
	delete partpoint;
	return 0;
}

UINT plane_row(LPVOID pParam) {
	int i;
	int u;
	int v;
	int m;
	int wu;
	int wv;
	int gxmin;
	int gxmax;
	int gymin;
	int gymax;
	int count2;
	int indi[4];
	long l;
	long k;
	long k_maxh;
	int *partpoint;
	int schwarzcount;
	int weisscount;
	int buntcount;
	int part;
	float gw;
	float *A2;
	float *x;
	double gw_med;
	double gxmind;
	double gxmaxd;
	double gymind;
	double gymaxd;
	double betr;
	double ad[4][2];
	partpoint=(int*) pParam; 
	part=*partpoint;
	if (part) {
		k=mitte_offset;
		k_maxh=struktur_vorhanden;
	}
	else {
		k=0;
		k_maxh=mitte_offset;
	}
	count3p[part]=0;
	A2=(float*) calloc((max_alloc<<2),sizeof(float));
	x=(float*) calloc(3,sizeof(float));
	while (k<k_maxh) {
		if (struktur[k]>0) {
			gxmin=x_global-1;
			gxmax=0;
			gymin=y_global-1;
			gymax=0;
			m=struktur[k+2]+struktur[k+3];
			gw_med=0;
			schwarzcount=0;
			weisscount=0;
			buntcount=0;
			for (l=0; l<m; l++) {
				get_xy(struktur[k+headersize+l],u,v);
				if (c_int(bild[4][u][v])<2) {
					schwarzcount+=1;
				}
				if (c_int(bild[4][u][v])>253) {
					weisscount+=1;
				}
				buntcount+=1;
				if (u<gxmin) gxmin=u;
				if (u>gxmax) gxmax=u;
				if (v<gymin) gymin=v;
				if (v>gymax) gymax=v;
				A2[(l<<2)]=(float) u;
				A2[(l<<2)+1]=(float) v;
				A2[(l<<2)+2]=1.0;
				gw=(float) c_int(bild[4][u][v]);
				A2[(l<<2)+3]=gw;
				gw_med+=gw;
			}
			gw_med/=((double) m);
			gxmind=((double) gxmin)-0.475;
			gymind=((double) gymin)-0.475;
			gxmaxd=((double) gxmax)+0.475;
			gymaxd=((double) gymax)+0.475;
			householder(3,m,A2,x);
			for (i=0; i<3; i++) {
				struktur[k+10+i]=(long) (10000.0*x[i]);
			}
			ad[0][0]=(gw_med-x[2]-x[1]*gymind)/x[0];
			ad[0][1]=gymind;
			ad[1][0]=(gw_med-x[2]-x[1]*gymaxd)/x[0];
			ad[1][1]=gymaxd;
			ad[2][0]=gxmind;
			ad[2][1]=(gw_med-x[2]-x[0]*gxmind)/x[1];
			ad[3][0]=gxmaxd;
			ad[3][1]=(gw_med-x[2]-x[0]*gxmaxd)/x[1];
			count2=0;
			for (i=0; i<4; i++) indi[i]=1;
			if ((ad[0][0]<gxmind) || (ad[0][0]>gxmaxd)) indi[0]=0;
			if ((ad[1][0]<gxmind) || (ad[1][0]>gxmaxd)) indi[1]=0;
			if ((ad[2][1]<gymind) || (ad[2][1]>gymaxd)) indi[2]=0;
			if ((ad[3][1]<gymind) || (ad[3][1]>gymaxd)) indi[3]=0;
			for (i=0; i<4; i++) {
				if (indi[i]) count2+=1;
			}
			if (buntcount>3) {
				if (((1.0*schwarzcount)/(1.0*buntcount)>0.07) || ((1.0*weisscount)/(1.0*buntcount)>0.07)) {
					count2=0;
				}
			}
			if (count2<2) {
				struktur[k]=0;
			}
			if (count2>1) {
				i=0;
				while (!indi[i]) i++;
				bestimme_werte(ad[i][0],ad[i][1],u,v);
				i+=1;
				while (((!indi[i]) || ((u==ad[i][0]) && (v==ad[i][1]))) && (i<4)) i++;
				if (i==4) count2=0;
				else {
					bestimme_werte(ad[i][0],ad[i][1],wu,wv);
					struktur[k+13]=koord_von(u,v);
					struktur[k+14]=koord_von(wu,wv);
					betr=sqrt((double) ((u-wu)*(u-wu)+(v-wv)*(v-wv)));
					if (betr<minlen2) {
						if (betr>2) struktur[k]=-1;
						else struktur[k]=0;
					}
					else if (fabs((double) (u-wu))>max_disp*betr) struktur[k]=-2;
				}
			}						
		}
		if (struktur[k]>0) count3p[part]+=1;
		k+=headersize+struktur[k+2]+struktur[k+3];				
	}
	free(A2);
	free(x);
	SetEvent(hCompleted[part]);
	delete partpoint;
	return 0;
}

void CShiftNView::OnGo() 
{
	int *pj;
	int i;
	int j;
	int part;
	int ii;
	int jj;
	int u;
	int v;
	int modu;
	long k;
	long l;
	long max;
	long max_g;
	long vote;
	long count;
	long count3=0;
	int indi[4];
	double owinkel=globwinkel;
	double oparam=globparam;
	double diparam;
	double xm=x_global*0.5;
	double ym=y_global*0.5;
	double uu;
	double vv;
	double uuu;
	double vvv;
	double fko;
	CDC* pDC;
	Kein_Bild noimagedlg;
	CProgressDlg progdlg;
	minlen2=minlen;
	struvo=0;
	ergvo=0;
	if (mhshowgui) pDC=GetDC();
	if (!speicher_voll) {
		noimagedlg.DoModal();
	}
	else {
		if (ergebnis_vorhanden) {
			for (j=0; j<y_global; j++) {
				for (i=0; i<x_global; i++) {
					bild[0][i][j]=i_char(0);
					bild[1][i][j]=i_char(0);
					l_schicht[0][i][j]=0;
					l_schicht[1][i][j]=0;
				}
			}
			ergebnis_vorhanden=0;
			ergvo=0;
			selektion_vorhanden=0;
			if (struktur_vorhanden) {
				free(struktur);
				struktur_vorhanden=0;
				struvo=0;
			}
		}
		if (minlen2==0) {
			if (x_global<y_global) {
				minlen2=x_global/36;
			}
			else {
				minlen2=y_global/36;
			}
		}
		if (minlen2<3) minlen2=3;
		minsize=2*minlen2;
		modu=(x_global/7)+1;
		distortion=distortion_fest;
		reset_distortion=distortion;
		horifac=horifac_fest;
		reset_horizontal=horifac;
		diparam=distortion/(x_global*x_global+y_global*y_global);
		if (distortion>=0) {
			if (xm<ym) {
				fko=1+diparam*(xm-1)*(xm-1);
			}
			else {
				fko=1+diparam*(ym-1)*(ym-1);
			}
		}
		else {
			fko=1+diparam*((xm+1)*(xm+1)+(ym+1)*(ym+1));
		}
		if (!ergebnis_vorhanden) {
			progdlg.Create();
			progdlg.SetRange(0,9+(x_global-1)/modu);
			progdlg.SetStep(1);
			progdlg.SetPos(0);
			wstep=twopi/((double) anzpart);
			switch (sprach) {
				case 1: progdlg.SetCaptionText("ShiftN - Computation of gradients"); break;
				case 2: progdlg.SetCaptionText("ShiftN - Étape 1"); break;
				case 3: progdlg.SetCaptionText("ShiftN - Paso 1"); break;
				default: progdlg.SetCaptionText("ShiftN - Berechnung der Gradienten"); break;
			}
			hCompleted=(HANDLE*) malloc(2*sizeof(HANDLE));
			for (part=0; part<2; part++) hCompleted[part]=CreateEvent(NULL,TRUE,FALSE,NULL);
			for (part=0; part<2; part++) {
				ResetEvent(hCompleted[part]);
				pj=new int(part);
				AfxBeginThread(calc_row,pj,THREAD_PRIORITY_NORMAL,0,0,NULL);
			}
			WaitForMultipleObjects(2,hCompleted,TRUE,INFINITE);
			maxp[0]=0;
			maxp[1]=0;
			countp[0]=0;
			countp[1]=0;
			progdlg.StepIt();
			for (part=0; part<2; part++) {
				ResetEvent(hCompleted[part]);
				pj=new int(part);
				AfxBeginThread(fill_row1,pj,THREAD_PRIORITY_NORMAL,0,0,NULL);
			}
			WaitForMultipleObjects(2,hCompleted,TRUE,INFINITE);
			free(hCompleted);
			count=countp[0]+countp[1];
			max=maxp[0]+maxp[1];
			switch (sprach) {
				case 1: progdlg.SetCaptionText("ShiftN - Line detection"); break;
				case 2: progdlg.SetCaptionText("ShiftN - Étape 2"); break;
				case 3: progdlg.SetCaptionText("ShiftN - Paso 2"); break;
				default: progdlg.SetCaptionText("ShiftN - Liniendetektion"); break;
			}
			struktur_vorhanden=count*headersize+max;
			if (struktur_vorhanden) {
				struktur=(long*) calloc(struktur_vorhanden,sizeof(long));
				if (!(struktur==NULL)) {
					kp[0]=0;
					mitte_offset=maxp[0]+countp[0]*headersize;
					kp[1]=mitte_offset;
					countp[0]=0;
					countp[1]=0;
					progdlg.StepIt();
					hCompleted=(HANDLE*) malloc(2*sizeof(HANDLE));
					for (part=0; part<2; part++) hCompleted[part]=CreateEvent(NULL,TRUE,FALSE,NULL);
					for (part=0; part<2; part++) {
						ResetEvent(hCompleted[part]);
						pj=new int(part);
						AfxBeginThread(fill_row2,pj,THREAD_PRIORITY_NORMAL,0,0,NULL);
					}
					WaitForMultipleObjects(2,hCompleted,TRUE,INFINITE);
					progdlg.StepIt();
					for (i=0; i<x_global; i++) {
						for (j=0; j<y_global; j++) {
							if ((l_schicht[0][i][j]) || (l_schicht[1][i][j])) {
								if (l_schicht[0][i][j]<l_schicht[1][i][j]) {
									bild[0][i][j]=i_char(2);
								}
								else {
									bild[0][i][j]=i_char(1);
								}
							}
						}
					}
					k=0;
					max_g=0;
					progdlg.StepIt();
					while (k<struktur_vorhanden) {
						if (struktur[k]>0) {
							vote=0;
							for (l=0; l<struktur[k+2]; l++) {
								get_xy(struktur[k+headersize+l],u,v);
								if (c_int(bild[0][u][v])==((int) (struktur[k+1]+1))) vote+=1;
							}
							struktur[k+9]=vote;
							if (((1.0*vote)/(1.0*struktur[k+2]))<minsup) {
								struktur[k]=0;
							}
						}
						if ((struktur[k+2]+struktur[k+3])>max_g) max_g=struktur[k+2]+struktur[k+3];
						k+=headersize+struktur[k+2]+struktur[k+3];				
					}
					progdlg.StepIt();
					grauwerte=(int**) calloc(2,sizeof(int*));
					for (part=0; part<2; part++) {
						grauwerte[part]=(int*) calloc(max_g,sizeof(int));
					}
					for (part=0; part<2; part++) {
						ResetEvent(hCompleted[part]);
						pj=new int(part);
						AfxBeginThread(sort_row,pj,THREAD_PRIORITY_NORMAL,0,0,NULL);
					}
					WaitForMultipleObjects(2,hCompleted,TRUE,INFINITE);
					for (part=0; part<2; part++) free(grauwerte[part]);
					free(grauwerte);
					progdlg.StepIt();
					max_alloc=max_g;
					for (part=0; part<2; part++) {
						ResetEvent(hCompleted[part]);
						pj=new int(part);
						AfxBeginThread(plane_row,pj,THREAD_PRIORITY_NORMAL,0,0,NULL);
					}
					WaitForMultipleObjects(2,hCompleted,TRUE,INFINITE);
					free(hCompleted);
					count3=count3p[0]+count3p[1];
					ergebnis_vorhanden=1;
				}
				if (count3<2) {
					if (struktur_vorhanden) {
						free(struktur);
						struktur_vorhanden=0;
						struvo=0;
					}
					ergebnis_vorhanden=0;
					ergvo=0;
					selektion_vorhanden=0;
				}
				else {
					progdlg.StepIt();
					switch (sprach) {
						case 1: progdlg.SetCaptionText("ShiftN - Homography estimation"); break;
						case 2: progdlg.SetCaptionText("ShiftN - Étape 3"); break;
						case 3: progdlg.SetCaptionText("ShiftN - Paso 3"); break;
						default: progdlg.SetCaptionText("ShiftN - Homographieschätzung"); break;
					}
					fithomography(x_global,y_global,oparam,owinkel);
					progdlg.StepIt();
					ihomography(owinkel,shiftamount*oparam,x_global,y_global);
					for (i=0; i<x_global; i+=1) {
						if ((i%modu)==0) progdlg.StepIt();
						for (j=0; j<y_global; j+=1) {
							vvv=(Homograph[1][1][1]*j+Homograph[1][1][2]*i+Homograph[1][1][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
							uuu=(Homograph[1][2][1]*j+Homograph[1][2][2]*i+Homograph[1][2][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
							for (k=0; k<3; k++) {
								indi[k]=255; // Background
							}
							if (usebackrgb) {
								indi[2]=(int) (backrgb>>16);
								indi[1]=(int) ((backrgb-((indi[2])<<16))>>8);
								indi[0]=(int) (backrgb-((backrgb>>8)<<8));
							}
							if ((uuu>=0) && (uuu<=x_global-1) && (vvv>=0) && (vvv<=y_global-1)) {
								if (fabs(distortion)>1e-8) {
									entzeichne(0,fko,diparam,uuu,vvv,xm,ym,uu,vv);
								}
								else {
									uu=uuu;
									vv=vvv;
								}
								ii=(int) uu;
								jj=(int) vv;
								if ((ii>=0) && (ii<=x_global-1) && (jj>=0) && (jj<=y_global-1)) {
									for (k=0; k<3; k++) {
										indi[k]=(int) sbild[k+lookdrei[ii]+lookauf[y_global-jj-1]];
									}
								}
							}
							for (k=0; k<3; k++) {
								ebild[lookauf[y_global-j-1]+lookdrei[i]+k]=(BYTE) indi[k];
							}
						}
					}
				}
			}
			progdlg.DestroyWindow();
		}
		if (ergebnis_vorhanden) {
			ergvo=1;
			beforeafter=0;
		}
		else ergvo=0;
		if (struktur_vorhanden) struvo=1;
		else struvo=0;
		if (mhshowgui) OnDraw(pDC);
	}
	if (mhshowgui) ReleaseDC(pDC);
	if (ergebnis_vorhanden) {
		ergvo=1;
		beforeafter=0;
	}
	else ergvo=0;
	if (struktur_vorhanden) struvo=1;
	else struvo=0;
}

void CShiftNView::Reoptimize() 
{
	int i;
	int j;
	int ii;
	int jj;
	int indi[4];
	int modu;
	long k;
	double owinkel=globwinkel;
	double oparam=globparam;
	double diparam;
	double xm=x_global*0.5;
	double ym=y_global*0.5;
	double uu;
	double vv;
	double uuu;
	double vvv;
	double fko;
	CProgressDlg progdlg;
	CDC* pDC=GetDC();
	struvo=0;
	ergvo=0;
	if (!struktur_vorhanden) {
		OnGo();
	}
	else {
		modu=(x_global/7)+1;
		progdlg.Create();
		progdlg.SetRange(0,4+(x_global-1)/modu);
		progdlg.SetStep(1);
		progdlg.SetPos(0);
		horifac=horifac_fest;
		reset_horizontal=horifac;
		distortion=distortion_fest;
		reset_distortion=distortion;
		diparam=distortion/(x_global*x_global+y_global*y_global);
		if (distortion>=0) {
			if (xm<ym) {
				fko=1+diparam*(xm-1)*(xm-1);
			}
			else {
				fko=1+diparam*(ym-1)*(ym-1);
			}
		}
		else {
			fko=1+diparam*((xm+1)*(xm+1)+(ym+1)*(ym+1));
		}
		progdlg.StepIt();
		fithomography(x_global,y_global,oparam,owinkel);
		progdlg.StepIt();
		ihomography(owinkel,shiftamount*oparam,x_global,y_global);
		progdlg.StepIt();
		for (i=0; i<x_global; i+=1) {
			if ((i%modu)==0) progdlg.StepIt();
			for (j=0; j<y_global; j+=1) {
				vvv=(Homograph[1][1][1]*j+Homograph[1][1][2]*i+Homograph[1][1][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
				uuu=(Homograph[1][2][1]*j+Homograph[1][2][2]*i+Homograph[1][2][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
				for (k=0; k<3; k++) {
					indi[k]=255; // Background
				}
				if (usebackrgb) {
					indi[2]=(int) (backrgb>>16);
					indi[1]=(int) ((backrgb-((indi[2])<<16))>>8);
					indi[0]=(int) (backrgb-((backrgb>>8)<<8));
				}
				if ((uuu>=0) && (uuu<=x_global-1) && (vvv>=0) && (vvv<=y_global-1)) {
					if (fabs(distortion)>1e-8) {
						entzeichne(0,fko,diparam,uuu,vvv,xm,ym,uu,vv);
					}
					else {
						uu=uuu;
						vv=vvv;
					}
					ii=(int) uu;
					jj=(int) vv;
					if ((ii>=0) && (ii<=x_global-1) && (jj>=0) && (jj<=y_global-1)) {
						for (k=0; k<3; k++) {
							indi[k]=(int) sbild[k+lookdrei[ii]+lookauf[y_global-jj-1]];
						}
					}
				}
				for (k=0; k<3; k++) {
					ebild[lookauf[y_global-j-1]+lookdrei[i]+k]=(BYTE) indi[k];
				}
			}
		}
		progdlg.DestroyWindow();
		if (ergebnis_vorhanden) {
			ergvo=1;
			beforeafter=0;
		}
		else ergvo=0;
		if (struktur_vorhanden) struvo=1;
		else struvo=0;
		OnDraw(pDC);
	}
	ReleaseDC(pDC);
	if (ergebnis_vorhanden) {
		ergvo=1;
		beforeafter=0;
	}
	else ergvo=0;
	if (struktur_vorhanden) struvo=1;
	else struvo=0;
}

void CShiftNView::OnLang() {
	SprachDlg sprach_dlg;
	sprach_dlg.m_sprach=sprach;
	if (sprach_dlg.DoModal()==IDOK) {
		sprach=sprach_dlg.m_sprach;
		AfxGetApp()->WriteProfileBinary("Language","SPRACH", (LPBYTE)&sprach, sizeof(sprach));
	}
}

void CShiftNView::OnDownMH() {
	CDC* pDC;
	if (ergvo && mhshowgui && beforeafter) {
		selektion_vorhanden=0;
		beforeafter=0;
		pDC=GetDC();
		OnDraw(pDC);
		ReleaseDC(pDC);
	}
}

void CShiftNView::OnUpMH() {
	CDC* pDC;
	if (ergvo && mhshowgui && (!beforeafter)) {
		selektion_vorhanden=0;
		beforeafter=1;
		pDC=GetDC();
		OnDraw(pDC);
		ReleaseDC(pDC);
	}
}

void CShiftNView::OnSchaltfl() {
	SchaltflDlg schaltfl_dlg;
	schaltfl_dlg.m_saveexit=saveexit;
	if (saveexit) schaltfl_dlg.m_saveexit_check=TRUE;
	if (schaltfl_dlg.DoModal()==IDOK) {
		saveexit=schaltfl_dlg.m_saveexit;
		AfxGetApp()->WriteProfileBinary("SaveExit","SAVE", (LPBYTE)&saveexit, sizeof(saveexit));
	}
}

void CShiftNView::OnCtrlC() {
	int x_neu;
	int y_neu;
	HGLOBAL ClipBuffer;
	BYTE* pData;
	if ((ergebnis_vorhanden && (!direkt_speichern)) || (ergebnis_vorhanden && direkt_speichern_clipboard && (direkt_speichern==1))){
		if (direkt_speichern) direkt_speichern=5;
		schreibe_bild(0,4,x_global,y_global,m_dateiname,1,1,x_neu,y_neu);
		if (OpenClipboard()) {;
			EmptyClipboard();
			ClipBuffer=GlobalAlloc(GMEM_DDESHARE,40+aufgerundet(3*x_neu)*y_neu);
			pData=(BYTE*) GlobalLock(ClipBuffer);
			memcpy(pData,clipbild,40+aufgerundet(3*x_neu)*y_neu);
			GlobalUnlock(ClipBuffer);
			SetClipboardData(CF_DIB,ClipBuffer);
			CloseClipboard();
			free(clipbild);
		}
	}
}

void CShiftNView::OnCtrlV() {
	CDC* pDC;
	HGLOBAL hMem;
	BYTE *pData;
	int u=0;
	int v=0;
	int u_auf;
	int ret=0;
	int art=0;
	int anzpal;
	char c;
	char *pal;
	int bitfields=0;
	int i;
	int j;
	int k;
	BOOL formatda=FALSE;
	CProgressDlg progdlg;
	CShiftNDoc* pDoc;
	if (!direkt_laden_clipboard) pDC=GetDC();
	if (!direkt_laden_clipboard) pDoc=GetDocument();
	if (OpenClipboard()) {
		formatda=IsClipboardFormatAvailable(CF_DIB);
		if (formatda) {
			if (batch_voll) {
				for (i=0; i<batch_anzahl; i++) free(batchname[i]);
				free(batchname);
				batch_voll=0;
				batch_anzahl=0;
			}
			if (speicher_voll) {
				speicher_voll=0;
				for (i=0; i<layer_c_intern; i++) {
					for (j=0; j<x_global; j++) {
						free(bild[i][j]);
					}
					free(bild[i]);
				}
				free(bild);
				if (tiff16_vorhanden) {
					free(tiff16r);
					free(tiff16n);
				}
				tiff16_vorhanden=0;
				if (exifstream_vorhanden) free(exifstream);
				exifstream_vorhanden=0;
				if (iptcstream_vorhanden) free(iptcstream);
				iptcstream_vorhanden=0;
				if (iccstream_vorhanden) free(icc_data_ptr);
				iccstream_vorhanden=FALSE;
				free(ebild);
				free(sbild);
				free(mbild);
				free(nbild);
				free(gbild);
				free(lookauf);
				free(lookdrei);
				free(mlookauf);
				free(mlookdrei);
				for (i=0; i<layer_l_intern; i++) {
					for (j=0; j<x_global; j++) {
						free(l_schicht[i][j]);
					}
					free(l_schicht[i]);
				}
				free(l_schicht);
				globwinkel=0;
				globparam=0;
				reset_param=globparam;
				reset_winkel=globwinkel;
				if (struktur_vorhanden) free(struktur);
				struktur_vorhanden=0;
				struvo=0;
				ergebnis_vorhanden=0;
				ergvo=0;
				selektion_vorhanden=0;
				x_global=0;
				y_global=0;
				mx_global=0;
				my_global=0;
			}
			hMem=(HGLOBAL) GetClipboardData(CF_DIB);
			pData=(BYTE*) GlobalLock(hMem);
			ist_jpeg=0;
			customgitter=0;
			customgitterx=0;
			customgittery=0;
			f_global=f_fest;
			jheadfocal=0;
			exifdreh=1;
			if (exifstream_vorhanden) free(exifstream);
			exifstream_vorhanden=0;
			if (iptcstream_vorhanden) free(iptcstream);
			iptcstream_vorhanden=0;
			if (iccstream_vorhanden) free(icc_data_ptr);
			iccstream_vorhanden=FALSE;
			if (!homo_vorhanden) {
				Homograph=(double***) calloc(4,sizeof(double**));
				for (i=0; i<4; i++) {
					Homograph[i]=(double**) calloc(4,sizeof(double*));
					for (j=0; j<4; j++) {
						Homograph[i][j]=(double*) calloc(4,sizeof(double));
					}
				}
				homo_vorhanden=1;
			}
			k=0;
			c=i_char((int) pData[k++]);
			if (c!=40) ret=2;
			for (i=0; i<4; i++) c=i_char((int) pData[k++]);
			if (!ret) {
				u=c_int(c);
				c=i_char((int) pData[k++]);
				u+=(c_int(c))<<8;
				c=i_char((int) pData[k++]);
				u+=(c_int(c))<<16;
				c=i_char((int) pData[k++]);
				u+=(c_int(c))<<24;
				c=i_char((int) pData[k++]);
				v=c_int(c);
				c=i_char((int) pData[k++]);
				v+=(c_int(c))<<8;
				c=i_char((int) pData[k++]);
				v+=(c_int(c))<<16;
				c=i_char((int) pData[k++]);
				v+=(c_int(c))<<24;
				c=i_char((int) pData[k++]);
				if (c!=1) ret=2;
				for (i=0; i<2; i++) c=i_char((int) pData[k++]);
				if ((c!=8) && (c!=24) && (c!=32)) ret=2;
				if (c==8) art=1;
				if (c==24) art=2;
				if (c==32) art=3;
				for (i=0; i<2; i++) c=i_char((int) pData[k++]);
				if ((c!=0) && ((c!=3) || (art!=3))) ret=2;
				if (c==3) bitfields=1;
				for (i=0; i<16; i++) c=i_char((int) pData[k++]);
				anzpal=c_int(c);
				if (anzpal==0) anzpal=256;
				for (i=0; i<7; i++) c=i_char((int) pData[k++]);
			}
			if (!ret) {
				if ((u<4) || (v<4) || (((long) u)*((long) v)>134217727)) {
					ret=1;
					u=0;
					v=0;
				}
			}
			if (!ret) {
				mu3au=aufgerundet(3*u);
				mlookauf=(int*) calloc(v,sizeof(int));
				mlookdrei=(int*) calloc(u,sizeof(int));
				mlookauf[0]=0;
				mlookdrei[0]=0;
				for (i=1; i<v; i++) mlookauf[i]=mlookauf[i-1]+mu3au;
				for (i=1; i<u; i++) mlookdrei[i]=mlookdrei[i-1]+3;
				mbild=(BYTE*) calloc(mu3au*v,sizeof(BYTE));
				if (mbild==NULL) ret=1;
				if ((!ret) && (art==1)) {
					globwinkel=0;
					globparam=0;
					reset_param=globparam;
					reset_winkel=globwinkel;
					u_auf=aufgerundet(u);
					pal=(char*) calloc(1024,sizeof(char));
					for (i=0; i<anzpal; i++) {
						c=i_char((int) pData[k++]);
						pal[(i<<2)]=c;
						c=i_char((int) pData[k++]);
						pal[(i<<2)+1]=c;
						c=i_char((int) pData[k++]);
						pal[(i<<2)+2]=c;
						c=i_char((int) pData[k++]);
					}
					progdlg.Create();
					progdlg.SetRange(0,1+v);
					progdlg.SetStep(1);
					progdlg.SetPos(1);
					for (j=0; j<v; j++) {
						progdlg.StepIt();
						for (i=0; i<u; i++) {
							c=i_char((int) pData[k++]);
							mbild[mlookauf[j]+mlookdrei[i]]=(BYTE) c_int(pal[(c_int(c)<<2)]);
							mbild[mlookauf[j]+mlookdrei[i]+1]=(BYTE) c_int(pal[(c_int(c)<<2)+1]);
							mbild[mlookauf[j]+mlookdrei[i]+2]=(BYTE) c_int(pal[(c_int(c)<<2)+2]);
						}
						for (i=u; i<u_auf; i++) c=i_char((int) pData[k++]);
					}
					speicher_voll=1;
					progdlg.DestroyWindow();
					free(pal);
				}
				if ((!ret) && (art==2)) {
					globwinkel=0;
					globparam=0;
					reset_param=globparam;
					reset_winkel=globwinkel;
					u_auf=aufgerundet(3*u);
					progdlg.Create();
					progdlg.SetRange(0,1+v);
					progdlg.SetStep(1);
					progdlg.SetPos(1);
					for (j=0; j<v; j++) {
						progdlg.StepIt();
						for (i=0; i<u; i++) {
							c=i_char((int) pData[k++]);
							mbild[mlookauf[j]+mlookdrei[i]]=(BYTE) c_int(c);
							c=i_char((int) pData[k++]);
							mbild[mlookauf[j]+mlookdrei[i]+1]=(BYTE) c_int(c);
							c=i_char((int) pData[k++]);
							mbild[mlookauf[j]+mlookdrei[i]+2]=(BYTE) c_int(c);
						}
						for (i=3*u; i<u_auf; i++) c=i_char((int) pData[k++]);
					}
					speicher_voll=1;
					progdlg.DestroyWindow();
				}
				if ((!ret) && (art==3)) {
					globwinkel=0;
					globparam=0;
					reset_param=globparam;
					reset_winkel=globwinkel;
					if (bitfields) {
						for (i=0; i<12; i++) {
							c=i_char((int) pData[k++]);
						}
					}
					progdlg.Create();
					progdlg.SetRange(0,1+v);
					progdlg.SetStep(1);
					progdlg.SetPos(1);
					for (j=0; j<v; j++) {
						progdlg.StepIt();
						for (i=0; i<u; i++) {
							c=i_char((int) pData[k++]);
							mbild[mlookauf[j]+mlookdrei[i]]=(BYTE) c_int(c);
							c=i_char((int) pData[k++]);
							mbild[mlookauf[j]+mlookdrei[i]+1]=(BYTE) c_int(c);
							c=i_char((int) pData[k++]);
							mbild[mlookauf[j]+mlookdrei[i]+2]=(BYTE) c_int(c);
							c=i_char((int) pData[k++]);
						}
					}
					speicher_voll=1;
					progdlg.DestroyWindow();
				}
			}
			mx_global=u;
			my_global=v;
			if (!ret) ret=alloc_bilder(u,v);
			if (!ret) preprocessing(glatt);
			if (!ret) {
				switch (sprach) {
					case 1: m_dateiname="Clipboard"; break;
					case 2: m_dateiname="Presse-papier"; break;
					case 3: m_dateiname="Memoria intermedia"; break;
					default: m_dateiname="Zwischenablage"; break;
				}
			}
			else {
				switch (sprach) {
					case 1: m_dateiname="No file loaded"; break;
					case 2: m_dateiname="Aucun fichier chargé"; break;
					case 3: m_dateiname="No hay archivo cargado"; break;
					default: m_dateiname="Keine Datei geladen"; break;
				}
			}
			if (!direkt_laden_clipboard) pDoc->SetTitle(m_dateiname);
			if (hMem) GlobalUnlock(hMem);
		}
		CloseClipboard();
	}
	if (!direkt_laden_clipboard) OnDraw(pDC);
	if (!direkt_laden_clipboard) ReleaseDC(pDC);
	if (commandfocus) {
		f_global=commandfocus;
		automatik_f=1;
	}
	beforeafter=0;
}

void CShiftNView::OnParam() 
{
	int i;
	int j;
	int tuenoch;
	int ret;
	CDC* pDC;
	ParamDlg param_dlg;
	param_dlg.m_minlen=minlen;
	param_dlg.m_mincon=mincon;
	param_dlg.m_distortion=distortion_fest;
	param_dlg.m_horifac=horifac_fest;
	param_dlg.m_maxdisp=(int) ((180*asin(max_disp))/mypi+0.5);
	param_dlg.m_shiftamount=(int) (100*shiftamount+0.5);
	param_dlg.m_flen=f_global;
	param_dlg.m_respara=respara;
	if (dorot) param_dlg.m_dorot=TRUE;
	else param_dlg.m_dorot=FALSE;
	if (relyexifori) param_dlg.m_ori=TRUE;
	else param_dlg.m_ori=FALSE;
	if (parlot) param_dlg.m_parlot=TRUE;
	else param_dlg.m_parlot=FALSE;
	if (param_dlg.DoModal()==IDOK) {
		minlen=param_dlg.m_minlen;
		minlen2=minlen;
		if (speicher_voll) {
			if (x_global<y_global) {
				minlen2=x_global/36;
			}
			else {
				minlen2=y_global/36;
			}
		}
		if (minlen2<3) minlen2=3;
		shiftamount=0.01*param_dlg.m_shiftamount;
		minsize=2*minlen2;
		horifac=param_dlg.m_horifac;
		distortion=param_dlg.m_distortion;
		distortion_fest=distortion;
		reset_distortion=distortion;
		reset_horizontal=horifac;
		horifac_fest=horifac;
		mincon=param_dlg.m_mincon;
		mingrad=0.5*mincon;
		respara=param_dlg.m_respara;
		sqmingrad=4*mingrad*mingrad;
		f_global=param_dlg.m_flen;
		if (!automatik_f) f_fest=param_dlg.m_flen;
		max_disp=sin((param_dlg.m_maxdisp*mypi)/180.0);
		if (param_dlg.m_dorot) dorot=1;
		else dorot=0;
		if (param_dlg.m_ori) relyexifori=1;
		else relyexifori=0;
		if (param_dlg.m_parlot) parlot=1;
		else parlot=0;
		if (speicher_voll) {
			speicher_voll=1;
			tuenoch=0;
			if (ergebnis_vorhanden) {
				tuenoch=1;
				for (j=0; j<y_global; j++) {
					for (i=0; i<x_global; i++) {
						bild[0][i][j]=i_char(0);
						bild[1][i][j]=i_char(0);
						l_schicht[0][i][j]=0;
						l_schicht[1][i][j]=0;
					}
				}
				ergebnis_vorhanden=0;
				ergvo=0;
				selektion_vorhanden=0;
				if (struktur_vorhanden) {
					free(struktur);
					struktur_vorhanden=0;
					struvo=0;
				}
			}
			for (i=0; i<layer_c_intern; i++) {
				for (j=0; j<x_global; j++) {
					free(bild[i][j]);
				}
				free(bild[i]);
			}
			free(bild);
			free(lookauf);
			free(lookdrei);
			free(ebild);
			free(sbild);
			free(nbild);
			free(gbild);
			for (i=0; i<layer_l_intern; i++) {
				for (j=0; j<x_global; j++) {
					free(l_schicht[i][j]);
				}
				free(l_schicht[i]);
			}
			free(l_schicht);
			ret=alloc_bilder(mx_global,my_global);
			if (!ret) preprocessing(glatt);
			if (tuenoch) OnGo();
			else {
				pDC=GetDC();
				OnDraw(pDC);
				ReleaseDC(pDC);
			}
		}
	}
}

void CShiftNView::OnGitter() 
{
	CDC* pDC=GetDC();
	if (showgrid) showgrid=0;
	else showgrid=1;
	OnDraw(pDC);
	ReleaseDC(pDC);
}

int kleiner(char* a, char* b) {
	int ret=0;
	int i;
	i=0;
	while ((a[i]!='\0') && (b[i]!='\0') && (!ret)) {
		if (a[i]<b[i]) ret=1;
		if (a[i]>b[i]) ret=2;
		i++;
	}
	if (a[i]=='\0') ret=1;
	if (b[i]=='\0') ret=2;
	if (ret==2) ret=0;
	return ret;
}

void sort_batch() {
	int i;
	int ind;
	char* z;
	do {
		ind=0;
		for (i=1; i<batch_anzahl; i++) {
			if (kleiner(batchname[i],batchname[i-1])) {
				ind=1;
				z=batchname[i];
				batchname[i]=batchname[i-1];
				batchname[i-1]=z;
			}
		}
	}
	while (ind);
}

static int CALLBACK BrowseForFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	if (uMsg == BFFM_INITIALIZED) {
		LPCTSTR startFolder = reinterpret_cast<LPCTSTR>(lpData);
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(startFolder));
	}
	return 0;
}

void CShiftNView::OnBatch() 
{
	int i;
	int j;
	int k;
	int l;
	int m;
	int n;
	int ind;
	int x_neu;
	int y_neu;
	int maxchar;
	char path[255];
	char displayName[255];
	char batchausgabename[511];
	CDC* pDC=GetDC();
	CString Cbatchausgabename;
	CShiftNDoc* pDoc = GetDocument();
	BROWSEINFO browseInfo;
	BOOL retVal;
	LPITEMIDLIST list;
	CFileDialog CDatei(TRUE);
	::ZeroMemory(&browseInfo,sizeof(browseInfo));
	switch (sprach) {
		case 1: CDatei.m_ofn.lpstrTitle="Select files"; break;
		case 2: CDatei.m_ofn.lpstrTitle="Liste de fichiers"; break;
		case 3: CDatei.m_ofn.lpstrTitle="Seleccionar archivos"; break;
		default: CDatei.m_ofn.lpstrTitle="Dateien festlegen"; break;
	}
	switch (sprach) {
		case 1: CDatei.m_ofn.lpstrFilter="Supported image formats\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0All files(*.*)\0*.*\0\0"; break;
		case 2: CDatei.m_ofn.lpstrFilter="Toutes les Images\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0Tous les fichiers(*.*)\0*.*\0\0"; break;
		case 3: CDatei.m_ofn.lpstrFilter="Formatos de imagen admitidos\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0Todos los archivos(*.*)\0*.*\0\0"; break;
		default: CDatei.m_ofn.lpstrFilter="Unterstützte Bilddateitypen\0*.jpg;*.tif;*.bmp\0JPEG (*.jpg)\0*.jpg\0TIFF (*.tif)\0*.tif\0BMP (*.bmp)\0*.bmp\0Alle Dateien(*.*)\0*.*\0\0"; break;
	}
	CDatei.m_ofn.Flags=CDatei.m_ofn.Flags | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	CDatei.m_ofn.lpstrFile=(char*) malloc(262143*sizeof(char));
	CDatei.m_ofn.lpstrFile[0]='\0';
	CDatei.m_ofn.nMaxFile=262143;
	if (parlot) batchlot=1;
	else batchlot=0;
	if (batch_voll) {
		for (i=0; i<batch_anzahl; i++) free(batchname[i]);
		free(batchname);
		batch_voll=0;
		batch_anzahl=0;
	}
	if (speicher_voll) {
		speicher_voll=0;
		for (i=0; i<layer_c_intern; i++) {
			for (j=0; j<x_global; j++) {
				free(bild[i][j]);
			}
			free(bild[i]);
		}
		free(bild);
		if (exifstream_vorhanden) free(exifstream);
		exifstream_vorhanden=0;
		if (iptcstream_vorhanden) free(iptcstream);
		iptcstream_vorhanden=0;
		if (iccstream_vorhanden) free(icc_data_ptr);
		iccstream_vorhanden=FALSE;
		if (tiff16_vorhanden) {
			free(tiff16r);
			free(tiff16n);
		}
		tiff16_vorhanden=0;
		free(ebild);
		free(sbild);
		free(mbild);
		free(nbild);
		free(gbild);
		free(lookauf);
		free(lookdrei);
		free(mlookauf);
		free(mlookdrei);
		for (i=0; i<layer_l_intern; i++) {
			for (j=0; j<x_global; j++) {
				free(l_schicht[i][j]);
			}
			free(l_schicht[i]);
		}
		free(l_schicht);
		if (!batchlot) {
			globwinkel=0;
			globparam=0;
		}
		reset_param=globparam;
		reset_winkel=globwinkel;
		if (struktur_vorhanden) {
			free(struktur);
			struktur_vorhanden=0;
			struvo=0;
		}
		ergebnis_vorhanden=0;
		ergvo=0;
		selektion_vorhanden=0;
		x_global=0;
		y_global=0;
		mx_global=0;
		my_global=0;
	}
	if (CDatei.DoModal()==IDOK) {
		i=0;
		j=0;
		k=0;
		l=0;
		maxchar=0;
		while (k<2) {
			if (CDatei.m_ofn.lpstrFile[i]=='\0') {
				k+=1;
				j+=1;
				l=0;
			}
			else {
				l+=1;
				if ((j) && (l>maxchar)) maxchar+=1;
				k=0;
			}
			i+=1;
		}
		maxchar+=1;
		j-=2;
		i=0;
		k=0;
		while (!k) {
			if (CDatei.m_ofn.lpstrFile[i]=='\0') k+=1;
			else {
				maxchar+=1;
				i+=1;
			}
		}
		if (j) maxchar+=1;
		else j=1;
		batchname=(char**) calloc(j,sizeof(char*));
		for (i=0; i<j; i++) 
			batchname[i]=(char*) calloc(maxchar,sizeof(char));
		if (j==1) {
			i=0;
			while (CDatei.m_ofn.lpstrFile[i]!='\0') {
				batchname[0][i]=CDatei.m_ofn.lpstrFile[i];
				i+=1;
			}
			batchname[0][i]='\0';
		}
		else {
			i=0;
			while (CDatei.m_ofn.lpstrFile[i]!='\0') i++;
			i+=1;
			for (k=0; k<j; k++) {
				l=i;
				ind=0;
				while (CDatei.m_ofn.lpstrFile[l]!='\0') {
					if (CDatei.m_ofn.lpstrFile[l]=='\\') ind=1;
					l+=1;
				}
				if (ind) {
					for (m=i; m<=l; m++)
						batchname[k][m-i]=CDatei.m_ofn.lpstrFile[m];
					i=l+1;
				}
				else {
					n=0;
					while (CDatei.m_ofn.lpstrFile[n]!='\0') {
						batchname[k][n]=CDatei.m_ofn.lpstrFile[n];
						n+=1;
					}
					batchname[k][n]='\\';
					for (m=i; m<=l; m++)
						batchname[k][n+1+m-i]=CDatei.m_ofn.lpstrFile[m];
					i=l+1;
				}
			}
		}
		batch_voll=1;
		batch_anzahl=j;
		sort_batch();
	}
	free(CDatei.m_ofn.lpstrFile);
	if (batch_voll) {
		browseInfo.hwndOwner=m_hWnd;
		browseInfo.pidlRoot=NULL;
		browseInfo.pszDisplayName=displayName;
		switch (sprach) {
			case 1: browseInfo.lpszTitle="Destination"; break;
			case 2: browseInfo.lpszTitle="Répertoire de sortie"; break;
			case 3: browseInfo.lpszTitle="Destino"; break;
			default: browseInfo.lpszTitle="Ausgabeverzeichnis"; break;
		}
		if (startFolder.GetLength()>0) {
			browseInfo.lpfn=BrowseForFolderCallback;
			browseInfo.lParam=reinterpret_cast<LPARAM>(static_cast<LPCTSTR>(startFolder));
		}
		else {
			browseInfo.lpfn=NULL;
		}
		browseInfo.ulFlags=BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		list=::SHBrowseForFolder(&browseInfo);
	}
	if ((batch_voll) && (list!=NULL)) {
		retVal=::SHGetPathFromIDList(list,path);
		startFolder=(CString) path;
		ind=0;
		for (j=0; j<batch_anzahl; j++) {
			m_dateiname=batchname[j];
			if (!lese_bild(mx_global,my_global,m_dateiname)) {
				preprocessing(glatt);
				if (parlot) {
					OnGoB();
				}
				else {
					OnGo();
				}
				i=0;
				k=0;
				while (batchname[j][i]!='\0') {
					if (batchname[j][i]=='\\') k=i;
					i++;
				}
				i=0;
				while (path[i]!='\0') {
					batchausgabename[i]=path[i];
					i++;
				}
				if (batchausgabename[i-1]!='\\') {
					batchausgabename[i]='\\';
					i+=1;
				}
				l=k+1;
				while (batchname[j][l]!='\0') {
					batchausgabename[i+l-k-1]=batchname[j][l];
					l+=1;
				}
				batchausgabename[i+l-k-1]='\0';
				ind=1;
				Cbatchausgabename=batchausgabename;
				for (i=0; i<Cbatchausgabename.GetLength(); i++) {
					if (Cbatchausgabename.GetAt(i)=='.') ind=0;
					if (Cbatchausgabename.GetAt(i)=='\\') ind=1;
				}
				if (ind) Cbatchausgabename+=".bmp";
				i=Cbatchausgabename.GetLength()-1;
				ind=1;
				while ((ind) && (i)) {
					if (Cbatchausgabename.GetAt(i)=='.') ind=0;
					if (ind) i-=1;
				}
				Cbatchausgabename=Cbatchausgabename.Left(i)+m_suffix+".bmp";
				if (ergebnis_vorhanden) {
					schreibe_bild(1,4,x_global,y_global,Cbatchausgabename,1,1,x_neu,y_neu);
				}
			}
			if (speicher_voll) {
				speicher_voll=0;
				for (i=0; i<layer_c_intern; i++) {
					for (k=0; k<x_global; k++) {
						free(bild[i][k]);
					}
					free(bild[i]);
				}
				free(bild);
				if (exifstream_vorhanden) free(exifstream);
				exifstream_vorhanden=0;
				if (iptcstream_vorhanden) free(iptcstream);
				iptcstream_vorhanden=0;
				if (iccstream_vorhanden) free(icc_data_ptr);
				iccstream_vorhanden=FALSE;
				if (tiff16_vorhanden) {
					free(tiff16r);
					free(tiff16n);
				}
				tiff16_vorhanden=0;
				free(ebild);
				free(sbild);
				free(mbild);
				free(nbild);
				free(gbild);
				free(lookauf);
				free(lookdrei);
				free(mlookauf);
				free(mlookdrei);
				for (i=0; i<layer_l_intern; i++) {
					for (k=0; k<x_global; k++) {
						free(l_schicht[i][k]);
					}
					free(l_schicht[i]);
				}
				free(l_schicht);
				if (!batchlot) {
					globwinkel=0;
					globparam=0;
				}
				reset_param=globparam;
				reset_winkel=globwinkel;
				if (struktur_vorhanden) {
					free(struktur);
					struktur_vorhanden=0;
					struvo=0;
				}
				ergebnis_vorhanden=0;
				ergvo=0;
				selektion_vorhanden=0;
				x_global=0;
				y_global=0;
				mx_global=0;
				my_global=0;
			}		
		}
	}
	if (batch_voll) {
		for (i=0; i<batch_anzahl; i++) free(batchname[i]);
		free(batchname);
		batch_voll=0;
		batch_anzahl=0;
	}
	switch (sprach) {
		case 1: pDoc->SetTitle("No file loaded"); break;
		case 2: pDoc->SetTitle("Aucun fichier chargé"); break;
		case 3: pDoc->SetTitle("No hay archivo cargado"); break;
		default: pDoc->SetTitle("Keine Datei geladen"); break;
	}
	if (batchlot) {
		batchlot=0;
		globwinkel=0;
		globparam=0;
		reset_param=globparam;
		reset_winkel=globwinkel;
	}
	OnDraw(pDC);
	ReleaseDC(pDC);
}

void CShiftNView::OnDarst() 
{	
	CDC* pDC=GetDC();
	DarstDlg darst_dlg;
	darst_dlg.m_scharfpar=schaerfungsparameter;
	darst_dlg.m_backrgb=backrgb;
	darst_dlg.m_jpegqualitaet=jpegquality;
	if (bilderart&1) darst_dlg.m_check_jpg=TRUE;
	if (bilderart&2) darst_dlg.m_check_bmp=TRUE;
	if (bilderart&4) darst_dlg.m_check_txt=TRUE;
	if (beschneiden) darst_dlg.m_beschneiden=TRUE;
	if (kubik) darst_dlg.m_kubik=TRUE;
	if (schaerfen) darst_dlg.m_schaerfen=TRUE;
	if (usebackrgb) darst_dlg.m_usebackrgb=TRUE;
	if (use_exif_date) darst_dlg.m_use_exif_date=TRUE;
	darst_dlg.m_dsuffix=m_suffix;
	if (darst_dlg.DoModal()==IDOK) {
		bilderart=darst_dlg.speichertyp;
		beschneiden=darst_dlg.besch;
		use_exif_date=darst_dlg.useexifdate;
		schaerfen=darst_dlg.sharp;
		kubik=darst_dlg.kub;
		m_suffix=darst_dlg.m_dsuffix;
		schaerfungsparameter=darst_dlg.m_scharfpar;
		jpegquality=darst_dlg.m_jpegqualitaet;
		usebackrgb=darst_dlg.usebackrgb;
		backrgb=darst_dlg.m_backrgb;
	}
	ReleaseDC(pDC);
}

int CShiftNApp::ExitInstance() 
{
	int i;
	int j;
	ofstream inidatei;
	CString ininame;
	SHGetSpecialFolderPath(NULL,buff,CSIDL_APPDATA,FALSE); 
	ininame=buff;
	ininame+="\\ShiftN.ini";
	inidatei.open(ininame,ios::out);
	if (inidatei) {
		inidatei << rgb[0] << "\tred\tdefault: 30" << endl;
		inidatei << rgb[1] << "\tgreen\tdefault: 59" << endl;
		inidatei << rgb[2] << "\tblue\tdefault: 11" << endl;
		inidatei << anzpart << "\tnumber of orientation segments\tdefault: 10" << endl;
		inidatei << glatt << "\tpreprocessing\tdefault: 1" << endl;
		inidatei << beschneiden << "\ttrim pictures\tdefault: 1" << endl;
		inidatei << kubik << "\tbicubic resampling\tdefault: 1"<< endl;
		inidatei << minlen << "\tminimum line length (0: automatic selection)\tdefault: 0"<< endl;
		inidatei << mincon << "\tminimum line contrast\tdefault: 3"<< endl;
		if (minlen) inidatei << minsize << "\tminimum region size (should be 2 x minimum line length)\tdefault: 0" << endl;
		else inidatei << ((int) 0) << "\tminimum region size (should be 2 x minimum line length)\tdefault: 0" << endl;
		inidatei << nachbar << "\ttype of neighborhood\tdefault: 0" << endl;
		inidatei << jpegquality << "\tJPEG quality\tdefault: 100" << endl;
		inidatei << bilderart << "\ttype of result\tdefault: 1" << endl;
		inidatei << schaerfen << "\tsharpen \tdefault: 1"<< endl;
		inidatei << shiftamount << "\tamount of correction\tdefault: 1" << endl;
		inidatei << schaerfungsparameter << "\tsharpen parameter\tdefault: 1.3" << endl;
		inidatei << mingrad << "\tminimum gradient magnitude\tdefault: 1.5" << endl;
		inidatei << minsup << "\tminimum region support\tdefault: 0.6" << endl;
		inidatei << max_disp << "\tmaximum tangential deviation\tdefault: 0.5" << endl;
		inidatei << dorot << "\tcorrect rotations\tdefault: 1" << endl;
		inidatei << relyexifori << "\trely on EXIF orientation tag\tdefault: 0" << endl;
		inidatei << distortion_fest << "\tdistortion parameter\tdefault: 0.0" << endl;
		inidatei << f_fest << "\tdefault focal length\tdefault: 28" << endl;
		inidatei << m_suffix.GetBuffer(m_suffix.GetLength()) << "\tsuffix added to the filename\tdefault: _ShiftN" << endl;
		inidatei << showgrid << "\tshow grid\tdefault: 0" << endl;
		inidatei << use_exif_date << "\tuse EXIF date\tdefault: 1" << endl;
		inidatei << horifac_fest << "\tparameter for horizontal resolution\tdefault: 0" << endl;
		inidatei << respara << "\tresolution parameter\tdefault: 1200" << endl;
		inidatei << usebackrgb << "\tuse background color\tdefault: 0" << endl;
		inidatei << backrgb << "\tbackground color\tdefault: 0" << endl;
		inidatei << parlot << "\tuse fixed parameters during batch processing\tdefault: 0" << endl;
	}
	inidatei.close();
	inidatei.clear();
	if (batch_voll) {
		for (i=0; i<batch_anzahl; i++) free(batchname[i]);
		free(batchname);
		batch_voll=0;
		batch_anzahl=0;
	}
	if (homo_vorhanden) {
		for (i=0; i<4; i++) {
			for (j=0; j<4; j++) {
				free(Homograph[i][j]);
			}
			free(Homograph[i]);
		}
		free(Homograph);
		homo_vorhanden=0;
	}
	if (speicher_voll) {
		speicher_voll=0;
		for (i=0; i<layer_c_intern; i++) {
			for (j=0; j<x_global; j++) {
				free(bild[i][j]);
			}
			free(bild[i]);
		}
		free(bild);
		if (exifstream_vorhanden) free(exifstream);
		exifstream_vorhanden=0;
		if (iptcstream_vorhanden) free(iptcstream);
		iptcstream_vorhanden=0;
		if (iccstream_vorhanden) free(icc_data_ptr);
		iccstream_vorhanden=FALSE;
		if (tiff16_vorhanden) {
			free(tiff16r);
			free(tiff16n);
		}
		tiff16_vorhanden=0;
		free(ebild);
		free(sbild);
		free(mbild);
		free(nbild);
		free(gbild);
		free(lookauf);
		free(lookdrei);
		free(mlookauf);
		free(mlookdrei);
		for (i=0; i<layer_l_intern; i++) {
			for (j=0; j<x_global; j++) {
				free(l_schicht[i][j]);
			}
			free(l_schicht[i]);
		}
		free(l_schicht);
		globwinkel=0;
		globparam=0;
		reset_param=globparam;
		reset_winkel=globwinkel;
		if (struktur_vorhanden) {
			free(struktur);
			struktur_vorhanden=0;
			struvo=0;
		}
		ergebnis_vorhanden=0;
		ergvo=0;
		selektion_vorhanden=0;
		x_global=0;
		y_global=0;
		mx_global=0;
		my_global=0;
	}	
	FreeJpgfile();
	return CWinApp::ExitInstance();
}

void CShiftNView::OnAnwendenKorrekturanpassen() 
{
	AdjustDlg adjust_dlg;
	int i;
	int j;
	int k;
	int ii;
	int jj;
	int indi[4];
	int drehreg;
	int shiftreg;
	int distoreg;
	int horizoreg;
	int modu;
	double owinkel=globwinkel;
	double oparam=globparam;
	double diparam;
	double xm=x_global*0.5;
	double ym=y_global*0.5;
	double uu;
	double vv;
	double uuu;
	double vvv;
	double fko;
	CDC* pDC=GetDC();
	Kein_Bild noimagedlg;
	CProgressDlg progdlg;
	struvo=0;
	ergvo=0;
	if (!speicher_voll) {
		noimagedlg.DoModal();
	}
	else {
		drehreg=(int) ((45000*globwinkel)/mypi);
		shiftreg=(int) (globparam*1250);
		distoreg=(int) (distortion*1250);
		horizoreg=(int) (horifac*400);
		if ((horizoreg<-2000) || (horizoreg>2000) || (drehreg<-2000) || (drehreg>2000) || (shiftreg<-2000) || (shiftreg>2000) || (distoreg<-2000) || (distoreg>2000)) {
			drehreg=0;
			shiftreg=0;
			distoreg=0;
			horizoreg=0;
		}
		adjust_dlg.m_drehreg=drehreg;
		adjust_dlg.m_shiftreg=shiftreg;
		adjust_dlg.m_distoreg=distoreg;
		adjust_dlg.m_horizoreg=horizoreg;
		adjust_dlg.m_reset_param=reset_param;
		adjust_dlg.m_reset_winkel=reset_winkel;
		adjust_dlg.m_reset_distortion=reset_distortion;
		adjust_dlg.m_reset_horizontal=reset_horizontal;
		if (adjust_dlg.DoModal()==IDOK) {
			progdlg.Create();
			modu=(x_global/7)+1;
			progdlg.SetRange(0,3+(x_global-1)/modu);
			progdlg.SetStep(1);
			progdlg.SetPos(0);
			oparam=adjust_dlg.m_shiftreg/1250.0;
			owinkel=(adjust_dlg.m_drehreg*mypi)/45000.0;
			distortion=adjust_dlg.m_distoreg/1250.0;
			horifac=adjust_dlg.m_horizoreg/400.0;
			diparam=distortion/(x_global*x_global+y_global*y_global);
			if (distortion>=0) {
				if (xm<ym) {
					fko=1+diparam*(xm-1)*(xm-1);
				}
				else {
					fko=1+diparam*(ym-1)*(ym-1);
				}
			}
			else {
				fko=1+diparam*((xm+1)*(xm+1)+(ym+1)*(ym+1));
			}
			if (ergebnis_vorhanden) {
				for (j=0; j<y_global; j++) {
					for (i=0; i<x_global; i++) {
						bild[0][i][j]=i_char(0);
						bild[1][i][j]=i_char(0);
						l_schicht[0][i][j]=0;
						l_schicht[1][i][j]=0;
					}
				}
				ergebnis_vorhanden=0;
				ergvo=0;
				selektion_vorhanden=0;
				if (struktur_vorhanden) {
					free(struktur);
					struktur_vorhanden=0;
					struvo=0;
				}
			}
			if (!ergebnis_vorhanden) {
				progdlg.StepIt();
				ihomography(owinkel,shiftamount*oparam,x_global,y_global);
				progdlg.StepIt();
				ergebnis_vorhanden=1;
				for (i=0; i<x_global; i+=1) {
					if ((i%modu)==0) progdlg.StepIt();
					for (j=0; j<y_global; j+=1) {
						vvv=(Homograph[1][1][1]*j+Homograph[1][1][2]*i+Homograph[1][1][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
						uuu=(Homograph[1][2][1]*j+Homograph[1][2][2]*i+Homograph[1][2][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
						for (k=0; k<3; k++) {
							indi[k]=255; // Background
						}
						if (usebackrgb) {
							indi[2]=(int) (backrgb>>16);
							indi[1]=(int) ((backrgb-((indi[2])<<16))>>8);
							indi[0]=(int) (backrgb-((backrgb>>8)<<8));
						}
						if ((uuu>=0) && (uuu<=x_global-1) && (vvv>=0) && (vvv<=y_global-1)) {
							if (fabs(distortion)>1e-8) {
								entzeichne(0,fko,diparam,uuu,vvv,xm,ym,uu,vv);
							}
							else {
								uu=uuu;
								vv=vvv;
							}
							ii=(int) uu;
							jj=(int) vv;
							if ((ii>=0) && (ii<=x_global-1) && (jj>=0) && (jj<=y_global-1)) {
								for (k=0; k<3; k++) {
									indi[k]=(int) sbild[k+lookdrei[ii]+lookauf[y_global-jj-1]];
								}
							}
						}
						for (k=0; k<3; k++) {
							ebild[lookauf[y_global-j-1]+lookdrei[i]+k]=(BYTE) indi[k];
						}
					}
				}
			}
			progdlg.DestroyWindow();
			if (ergebnis_vorhanden) {
				ergvo=1;
				beforeafter=0;
			}
			else ergvo=0;
			if (struktur_vorhanden) struvo=1;
			else struvo=0;
			globwinkel=owinkel;
			globparam=oparam;
		}
	}
	if (ergebnis_vorhanden) {
		ergvo=1;
		beforeafter=0;
	}
	else ergvo=0;
	if (struktur_vorhanden) struvo=1;
	else struvo=0;
	OnDraw(pDC);
	ReleaseDC(pDC);
}

void CShiftNView::OnGoB() 
{
	int i;
	int j;
	int k;
	int ii;
	int jj;
	int indi[4];
	int modu;
	double diparam;
	double xm=x_global*0.5;
	double ym=y_global*0.5;
	double uu;
	double vv;
	double uuu;
	double vvv;
	double fko;
	CDC* pDC=GetDC();
	Kein_Bild noimagedlg;
	CProgressDlg progdlg;
	struvo=0;
	ergvo=0;
	if (!speicher_voll) {
		noimagedlg.DoModal();
	}
	else {
		progdlg.Create();
		modu=(x_global/7)+1;
		progdlg.SetRange(0,3+(x_global-1)/modu);
		progdlg.SetStep(1);
		progdlg.SetPos(0);
		diparam=distortion/(x_global*x_global+y_global*y_global);
		if (distortion>=0) {
			if (xm<ym) {
				fko=1+diparam*(xm-1)*(xm-1);
			}
			else {
				fko=1+diparam*(ym-1)*(ym-1);
			}
		}
		else {
			fko=1+diparam*((xm+1)*(xm+1)+(ym+1)*(ym+1));
		}
		if (ergebnis_vorhanden) {
			for (j=0; j<y_global; j++) {
				for (i=0; i<x_global; i++) {
					bild[0][i][j]=i_char(0);
					bild[1][i][j]=i_char(0);
					l_schicht[0][i][j]=0;
					l_schicht[1][i][j]=0;
				}
			}
			ergebnis_vorhanden=0;
			ergvo=0;
			selektion_vorhanden=0;
			if (struktur_vorhanden) {
				free(struktur);
				struktur_vorhanden=0;
				struvo=0;
			}
		}
		if (!ergebnis_vorhanden) {
			progdlg.StepIt();
			ihomography(globwinkel,shiftamount*globparam,x_global,y_global);
			progdlg.StepIt();
			ergebnis_vorhanden=1;
			for (i=0; i<x_global; i+=1) {
				if ((i%modu)==0) progdlg.StepIt();
				for (j=0; j<y_global; j+=1) {
					vvv=(Homograph[1][1][1]*j+Homograph[1][1][2]*i+Homograph[1][1][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
					uuu=(Homograph[1][2][1]*j+Homograph[1][2][2]*i+Homograph[1][2][3])/(Homograph[1][3][1]*j+Homograph[1][3][2]*i+Homograph[1][3][3]);
					for (k=0; k<3; k++) {
						indi[k]=255; // Background
					}
					if (usebackrgb) {
						indi[2]=(int) (backrgb>>16);
						indi[1]=(int) ((backrgb-((indi[2])<<16))>>8);
						indi[0]=(int) (backrgb-((backrgb>>8)<<8));
					}
					if ((uuu>=0) && (uuu<=x_global-1) && (vvv>=0) && (vvv<=y_global-1)) {
						if (fabs(distortion)>1e-8) {
							entzeichne(0,fko,diparam,uuu,vvv,xm,ym,uu,vv);
						}
						else {
							uu=uuu;
							vv=vvv;
						}
						ii=(int) uu;
						jj=(int) vv;
						if ((ii>=0) && (ii<=x_global-1) && (jj>=0) && (jj<=y_global-1)) {
							for (k=0; k<3; k++) {
								indi[k]=(int) sbild[k+lookdrei[ii]+lookauf[y_global-jj-1]];
							}
						}
					}
					for (k=0; k<3; k++) {
						ebild[lookauf[y_global-j-1]+lookdrei[i]+k]=(BYTE) indi[k];
					}
				}
			}
		}
		progdlg.DestroyWindow();
		if (ergebnis_vorhanden) {
			ergvo=1;
			beforeafter=0;
		}
		else ergvo=0;
		if (struktur_vorhanden) struvo=1;
		else struvo=0;
	}
	if (ergebnis_vorhanden) {
		ergvo=1;
		beforeafter=0;
	}
	else ergvo=0;
	if (struktur_vorhanden) struvo=1;
	else struvo=0;
	OnDraw(pDC);
	ReleaseDC(pDC);
}

DROPEFFECT CShiftNView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{	
	return DROPEFFECT_LINK;
}

void CShiftNView::OnDragLeave() 
{
	CView::OnDragLeave();
}

DROPEFFECT CShiftNView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{	
	return DROPEFFECT_LINK;
}

BOOL CShiftNView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	int i;
	int j;
	char *filename;
	CShiftNDoc* pDoc=GetDocument();
	Ladefehler_Dialog errDlg;
	CDC* pDC=GetDC();
	if (pDataObject->IsDataAvailable(CF_HDROP)) {
		filename=(char*) calloc(512,sizeof(char));
		HGLOBAL hmem=pDataObject->GetGlobalData(CF_HDROP);
		UINT dropCount=DragQueryFile((HDROP)hmem,-1,NULL,0);
		for (i=0; i<1; i++) {
			DragQueryFile((HDROP)hmem,0,filename,511);
			m_dateiname=filename;
		}
		DragFinish((HDROP)hmem);
		free(filename);
		if (m_dateiname.GetLength()>0) {
			if (batch_voll) {
				for (i=0; i<batch_anzahl; i++) free(batchname[i]);
				free(batchname);
				batch_voll=0;
				batch_anzahl=0;
			}
			if (speicher_voll) {
				speicher_voll=0;
				for (i=0; i<layer_c_intern; i++) {
					for (j=0; j<x_global; j++) {
						free(bild[i][j]);
					}
					free(bild[i]);
				}
				free(bild);
				if (exifstream_vorhanden) free(exifstream);
				exifstream_vorhanden=0;
				if (iptcstream_vorhanden) free(iptcstream);
				iptcstream_vorhanden=0;
				if (iccstream_vorhanden) free(icc_data_ptr);
				iccstream_vorhanden=FALSE;
				if (tiff16_vorhanden) {
					free(tiff16r);
					free(tiff16n);
				}
				tiff16_vorhanden=0;
				free(ebild);
				free(sbild);
				free(mbild);
				free(nbild);
				free(gbild);
				free(lookauf);
				free(lookdrei);
				free(mlookauf);
				free(mlookdrei);
				for (i=0; i<layer_l_intern; i++) {
					for (j=0; j<x_global; j++) {
						free(l_schicht[i][j]);
					}
					free(l_schicht[i]);
				}
				free(l_schicht);
				globwinkel=0;
				globparam=0;
				reset_param=globparam;
				reset_winkel=globwinkel;
				if (struktur_vorhanden) free(struktur);
				struktur_vorhanden=0;
				struvo=0;
				ergebnis_vorhanden=0;
				ergvo=0;
				selektion_vorhanden=0;
				x_global=0;
				y_global=0;
				mx_global=0;
				my_global=0;
			}	
			if (lese_bild(mx_global,my_global,m_dateiname)) {
				switch (sprach) {
					case 1: m_dateiname="An error occurred"; break;
					case 2: m_dateiname="Une erreur s'est produite"; break;
					case 3: m_dateiname="Ha ocurrido un error"; break;
					default: m_dateiname="Es ist ein Fehler aufgetreten"; break;
				}
				errDlg.DoModal();
			}
			else {
				preprocessing(glatt);
			}
			pDoc->SetTitle(m_dateiname);
		}
		if (!speicher_voll) {
			switch (sprach) {
				case 1: pDoc->SetTitle("No file loaded"); break;
				case 2: pDoc->SetTitle("Aucun fichier chargé"); break;
				case 3: pDoc->SetTitle("No hay archivo cargado"); break;
				default: pDoc->SetTitle("Keine Datei geladen"); break;
			}
		}
		OnDraw(pDC);
		return TRUE;
	}
	return FALSE;
	ReleaseDC(pDC);
}

int CShiftNView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	int i;
	int j;
	Ladefehler_Dialog errDlg;
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;
	if (direkt_laden==2) {
		direkt_laden=1;
		if (batch_voll) {
			for (i=0; i<batch_anzahl; i++) free(batchname[i]);
			free(batchname);
			batch_voll=0;
			batch_anzahl=0;
		}
		if (speicher_voll) {
			speicher_voll=0;
			for (i=0; i<layer_c_intern; i++) {
				for (j=0; j<x_global; j++) {
					free(bild[i][j]);
				}
				free(bild[i]);
			}
			free(bild);
			if (exifstream_vorhanden) free(exifstream);
			exifstream_vorhanden=0;
			if (iptcstream_vorhanden) free(iptcstream);
			iptcstream_vorhanden=0;
			if (iccstream_vorhanden) free(icc_data_ptr);
			iccstream_vorhanden=FALSE;
			if (tiff16_vorhanden) {
				free(tiff16r);
				free(tiff16n);
			}
			tiff16_vorhanden=0;
			free(lookauf);
			free(lookdrei);
			free(mlookauf);
			free(mlookdrei);
			free(ebild);
			free(sbild);
			free(mbild);
			free(nbild);
			free(gbild);
			for (i=0; i<layer_l_intern; i++) {
				for (j=0; j<x_global; j++) {
					free(l_schicht[i][j]);
				}
				free(l_schicht[i]);
			}
			free(l_schicht);
			globwinkel=0;
			globparam=0;
			reset_param=globparam;
			reset_winkel=globwinkel;
			if (struktur_vorhanden) free(struktur);
			struktur_vorhanden=0;
			struvo=0;
			ergebnis_vorhanden=0;
			ergvo=0;
			selektion_vorhanden=0;
			x_global=0;
			y_global=0;
			mx_global=0;
			my_global=0;
		}	
		if (!direkt_laden_clipboard) {
			if (lese_bild(mx_global,my_global,m_dateiname)) {
				switch (sprach) {
					case 1: m_dateiname="An error occurred"; break;
					case 2: m_dateiname="Une erreur s'est produite"; break;
					case 3: m_dateiname="Ha ocurrido un error"; break;
					default: m_dateiname="Es ist ein Fehler aufgetreten"; break;
				}
				direkt_speichern=0;
				errDlg.DoModal();
			}
			else {
				preprocessing(glatt);
			}
		}
		else {
			OnCtrlV();
			preprocessing(glatt);
		}
		direkt_laden=0;
	}
	if ((direkt_speichern==2) && (direkt_laden==0)) {
		direkt_speichern=1;
		OnGo();
		OnSaveExit();
	}
	AfxOleInit();
	DragDrop.Register(this);
	return 0;
}

BOOL CShiftNView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{	
	return CView::OnScrollBy(sizeScroll, bDoScroll);
}

void CShiftNView::OnDelMH() // DEL / SUPPR key all lines in red
{
	int k;
	int u1;
	int u2;
	int v1;
	int v2;
	long farbe;
	CDC* pDC;
	if (mhshowgui) {
		pDC=GetDC();
		if ((struktur_vorhanden) && ((afu-afo)>20) && ((afr-afl)>20) && (kf>0.0001)) {
			k=0;
			while (k<struktur_vorhanden) {
				if (struktur[k]>0) { // Green line
					get_xy(struktur[k+13],u1,v1);
					get_xy(struktur[k+14],u2,v2); 
					u1=afl+(int) (kf*u1);
					u2=afl+(int) (kf*u2);
					v1=afu-(int) (kf*v1);
					v2=afu-(int) (kf*v2);
					struktur[k]=-3; // Red line
					farbe=(long) 255;
					linie(pDC,u1,v1,u2,v2,farbe);
				}
				k+=headersize+struktur[k+2]+struktur[k+3];
			}
		}
		ReleaseDC(pDC);
		globwinkel=0;
		globparam=0;
		reset_param=globparam;
		reset_winkel=globwinkel;
	}
}

// Mouse management



void CShiftNView::OnMButtonDown(UINT nFlags, CPoint point) {
	int x;
	int y;
	x=point.x;
	y=point.y;
	if (showgrid) {
		if (((afu-afo)>20) && ((afr-afl)>20) && (kf>0.0001) && (x>=13) && (x<=(13+afr-afl)) && (y>=13) && (y<=(13+afu-afo))) {
			customgitter=1;
			customgitterx=(x-13)/(1.0*(afr-afl));
			customgittery=(y-13)/(1.0*(afu-afo));
		}
		else {
			customgitter=0;
			customgitterx=0;
			customgittery=0;
		}
	}
	CView::OnMButtonDown(nFlags, point);
}

void CShiftNView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int x;
	int y;
	int i;
	int j;
	int k;
	int u1;
	int u2;
	int v1;
	int v2;
	int ind;
	long farbe;
	CDC* pDC;
	const int gr=2;
	x=point.x;
	y=point.y;
	if (mhshowgui) {
		pDC=GetDC();
		if ((struktur_vorhanden) && ((afu-afo)>20) && ((afr-afl)>20) && (kf>0.0001) && (x>=afl) && (x<=afr) && (y>=afo) && (y<=afu)) {

			if (nFlags & MK_CONTROL) {
				selection_mouse = 1;
				selection_X1 = x;
				selection_Y1 = y;
			}
			else if (nFlags & MK_SHIFT) {
				selection_mouse = 2;
				selection_X1 = x;
				selection_Y1 = y;
			} else {
				if (selection_rect_prec) {
					pDC->SetROP2(R2_NOTXORPEN);
					if (selection_X1 > selection_X2) {
						selection_rect.left = selection_X2;
						selection_rect.right = selection_X1;
					} else {
						selection_rect.left = selection_X1;
						selection_rect.right = selection_X2;
					}
					if (selection_Y1 > selection_Y2) {
						selection_rect.top = selection_Y2;
						selection_rect.bottom = selection_Y1;
					} else {
						selection_rect.top = selection_Y1;
						selection_rect.bottom = selection_Y2;
					}
					pDC->Rectangle(&selection_rect);
				}
				selection_rect_prec = false;
				selection_mouse = 0;
				
				ind = 0;
				for (i = -gr; i <= gr; i++) {
					for (j = -gr; j <= gr; j++) {
						if (pDC->GetPixel(x + i, y + j) == (255 << 8)) ind = 1;
					}
				}
				if (ind) {
					k = 0;
					while (k<struktur_vorhanden) {
						if (struktur[k]>0) { // Green line
							get_xy(struktur[k + 13], u1, v1);
							get_xy(struktur[k + 14], u2, v2);
							u1 = afl + (int)(kf*u1);
							u2 = afl + (int)(kf*u2);
							v1 = afu - (int)(kf*v1);
							v2 = afu - (int)(kf*v2);
							if (linientest(x, y, gr, u1, v1, u2, v2)) {
								struktur[k] = -3; // Red line
								farbe = (long)255;
								linie(pDC, u1, v1, u2, v2, farbe);
							}
						}
						k += headersize + struktur[k + 2] + struktur[k + 3];
					}
					globwinkel = 0;
					globparam = 0;
					reset_param = globparam;
					reset_winkel = globwinkel;
				}
			}
		}
		if ((ergebnis_vorhanden) && (!beforeafter) && ((afu-afo)>20) && ((afr-afl)>20) && (kf>0.0001) && (x>=13) && (x<=(13+afr-afl)) && (y>=13) && (y<=(13+afu-afo))) {
			if (selektion_vorhanden>1) {
				pDC->SetROP2(R2_NOTXORPEN);
				pDC->Rectangle(&m_rect);
			}
			selektion_vorhanden=1;	
			m_rect.top=y;
			m_rect.left=x;
		}
		ReleaseDC(pDC);
	}
	CView::OnLButtonDown(nFlags, point);
}

void CShiftNView::OnMouseMove(UINT nFlags, CPoint point) 
{	
	int x;
	int y;
	int i;
	int j;
	int k;
	int u1;
	int u2;
	int v1;
	int v2;
	int ind;
	long farbe;
	double verh;
	CDC* pDC;
	const int gr=2;
	if (selection_mouse > 0) {
		pDC = GetDC();
		pDC->SetROP2(R2_NOTXORPEN);
		x = point.x;
		y = point.y;
		if (selection_rect_prec) {
			if (selection_X1 > selection_X2) {
				selection_rect.left = selection_X2;
				selection_rect.right = selection_X1;
			} else {
				selection_rect.left = selection_X1;
				selection_rect.right = selection_X2;
			}
			if (selection_Y1 > selection_Y2) {
				selection_rect.top = selection_Y2;
				selection_rect.bottom = selection_Y1;
			} else {
				selection_rect.top = selection_Y1;
				selection_rect.bottom = selection_Y2;
			}
			pDC->Rectangle(&selection_rect);
		}

		selection_X2 = x;
		selection_Y2 = y;
		if (selection_X1 > selection_X2) {
			selection_rect.left = selection_X2;
			selection_rect.right = selection_X1;
		}
		else {
			selection_rect.left = selection_X1;
			selection_rect.right = selection_X2;
		}
		if (selection_Y1 > selection_Y2) {
			selection_rect.top = selection_Y2;
			selection_rect.bottom = selection_Y1;
		}
		else {
			selection_rect.top = selection_Y1;
			selection_rect.bottom = selection_Y2;
		}
		pDC->Rectangle(&selection_rect);
		selection_rect_prec = true;

		ReleaseDC(pDC);
	} else {
		if ((nFlags&MK_LBUTTON) && (mhshowgui)) {
			pDC = GetDC();
			x = point.x;
			y = point.y;
			if ((selektion_vorhanden>0) && (ergebnis_vorhanden) && (!beforeafter) && ((afu - afo)>20) && ((afr - afl)>20) && (kf>0.0001) && (x >= 13) && (x <= (13 + afr - afl)) && (y >= 13) && (y <= (13 + afu - afo))) {
				pDC->SetROP2(R2_NOTXORPEN);
				if (selektion_vorhanden>1) pDC->Rectangle(&m_rect);
				selektion_vorhanden = 2;
				m_rect.bottom = y;
				m_rect.right = x;
				pDC->Rectangle(&m_rect);
			}
			if ((!(nFlags&MK_RBUTTON)) && (struktur_vorhanden) && ((afu - afo)>20) && ((afr - afl)>20) && (kf>0.0001) && (x >= afl) && (x <= afr) && (y >= afo) && (y <= afu)) {
				ind = 0;
				for (i = -gr; i <= gr; i++) {
					for (j = -gr; j <= gr; j++) {
						if (pDC->GetPixel(x + i, y + j) == (255 << 8)) ind = 1;
					}
				}
				if (ind) {
					k = 0;
					while (k<struktur_vorhanden) {
						if (struktur[k]>0) { // Green line
							get_xy(struktur[k + 13], u1, v1);
							get_xy(struktur[k + 14], u2, v2);
							u1 = afl + (int)(kf*u1);
							u2 = afl + (int)(kf*u2);
							v1 = afu - (int)(kf*v1);
							v2 = afu - (int)(kf*v2);
							if (linientest(x, y, gr, u1, v1, u2, v2)) {
								struktur[k] = -3; // Red line
								farbe = (long)255;
								linie(pDC, u1, v1, u2, v2, farbe);
							}
						}
						k += headersize + struktur[k + 2] + struktur[k + 3];
					}
				}
			}
			ReleaseDC(pDC);
		}
		if ((nFlags&MK_RBUTTON) && (mhshowgui)) {
			pDC = GetDC();
			x = point.x;
			y = point.y;
			if ((selektion_vorhanden>0) && (ergebnis_vorhanden) && (!beforeafter) && ((afu - afo)>20) && ((afr - afl)>20) && (kf>0.0001) && (x >= 13) && (x <= (13 + afr - afl)) && (y >= 13) && (y <= (13 + afu - afo))) {
				pDC->SetROP2(R2_NOTXORPEN);
				if (selektion_vorhanden>1) pDC->Rectangle(&m_rect);
				selektion_vorhanden = 2;
				verh = ((double)x_global) / ((double)y_global);
				if (fabs((double)(y - m_rect.top))>3) {
					while ((fabs((double)(x - m_rect.left)) / fabs((double)(y - m_rect.top)))<verh) {
						if (x>m_rect.left) {
							x += 1;
							m_rect.left -= 1;
						}
						else {
							x -= 1;
							m_rect.left += 1;
						}
					}
					while ((fabs((double)(x - m_rect.left)) / fabs((double)(y - m_rect.top)))>verh) {
						if (x>m_rect.left) {
							x -= 1;
							m_rect.left += 1;
						}
						else {
							x += 1;
							m_rect.left -= 1;
						}
					}
					while (m_rect.left<13) m_rect.left += 1;
					while (m_rect.left>(13 + afr - afl)) m_rect.left -= 1;
				}
				m_rect.bottom = y;
				m_rect.right = x;
				while (m_rect.left<13) m_rect.left += 1;
				while (m_rect.left>(13 + afr - afl)) m_rect.left -= 1;
				while (m_rect.right<13) m_rect.right += 1;
				while (m_rect.right>(13 + afr - afl)) m_rect.right -= 1;
				pDC->Rectangle(&m_rect);
			}
			if ((!(nFlags&MK_LBUTTON)) && (struktur_vorhanden) && ((afu - afo)>20) && ((afr - afl)>20) && (kf>0.0001) && (x >= afl) && (x <= afr) && (y >= afo) && (y <= afu)) {
				ind = 0;
				for (i = -gr; i <= gr; i++) {
					for (j = -gr; j <= gr; j++) {
						if (pDC->GetPixel(x + i, y + j) == 255) ind = 1;
					}
				}
				if (ind) {
					k = 0;
					while (k<struktur_vorhanden) {
						if (struktur[k] == -3) { // Red line
							get_xy(struktur[k + 13], u1, v1);
							get_xy(struktur[k + 14], u2, v2);
							u1 = afl + (int)(kf*u1);
							u2 = afl + (int)(kf*u2);
							v1 = afu - (int)(kf*v1);
							v2 = afu - (int)(kf*v2);
							if (linientest(x, y, gr, u1, v1, u2, v2)) {
								struktur[k] = 1; // Green line
								farbe = (255 << 8);
								linie(pDC, u1, v1, u2, v2, farbe);
							}
						}
						k += headersize + struktur[k + 2] + struktur[k + 3];
					}
				}
			}
			ReleaseDC(pDC);
		}
	}
	CView::OnMouseMove(nFlags, point);
}

void CShiftNView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int x;
	int y;
	int i;
	int j;
	int k;
	int u1;
	int u2;
	int v1;
	int v2;
	int ind;
	long farbe;
	CDC* pDC;
	const int gr=2;
	x=point.x;
	y=point.y;
	if (mhshowgui) {
		pDC=GetDC();
		if ((struktur_vorhanden) && ((afu-afo)>20) && ((afr-afl)>20) && (kf>0.0001) && (x>=afl) && (x<=afr) && (y>=afo) && (y<=afu)) {

			if (nFlags & MK_CONTROL) {
				selection_mouse = 1;
				selection_X1 = x;
				selection_Y1 = y;
			} else if (nFlags & MK_SHIFT) {
				selection_mouse = 2;
				selection_X1 = x;
				selection_Y1 = y;
			} else {
				if (selection_rect_prec) {
					pDC->SetROP2(R2_NOTXORPEN);
					if (selection_X1 > selection_X2) {
						selection_rect.left = selection_X2;
						selection_rect.right = selection_X1;
					} else {
						selection_rect.left = selection_X1;
						selection_rect.right = selection_X2;
					}
					if (selection_Y1 > selection_Y2) {
						selection_rect.top = selection_Y2;
						selection_rect.bottom = selection_Y1;
					} else {
						selection_rect.top = selection_Y1;
						selection_rect.bottom = selection_Y2;
					}
					pDC->Rectangle(&selection_rect);
				}
				selection_rect_prec = false;
				selection_mouse = 0;

				ind = 0;
				for (i = -gr; i <= gr; i++) {
					for (j = -gr; j <= gr; j++) {
						if (pDC->GetPixel(x + i, y + j) == 255) ind = 1;
					}
				}
				if (ind) {
					k = 0;
					while (k<struktur_vorhanden) {
						if (struktur[k] == -3) {  // Red line
							get_xy(struktur[k + 13], u1, v1);
							get_xy(struktur[k + 14], u2, v2);
							u1 = afl + (int)(kf*u1);
							u2 = afl + (int)(kf*u2);
							v1 = afu - (int)(kf*v1);
							v2 = afu - (int)(kf*v2);
							if (linientest(x, y, gr, u1, v1, u2, v2)) {
								struktur[k] = 1;  // Green line
								farbe = (255 << 8);
								linie(pDC, u1, v1, u2, v2, farbe);
							}
						}
						k += headersize + struktur[k + 2] + struktur[k + 3];
					}
					globwinkel = 0;
					globparam = 0;
					reset_param = globparam;
					reset_winkel = globwinkel;
				}
			}
		}
		if ((ergebnis_vorhanden) && (!beforeafter) && ((afu-afo)>20) && ((afr-afl)>20) && (kf>0.0001) && (x>=13) && (x<=(13+afr-afl)) && (y>=13) && (y<=(13+afu-afo))) {
			if (selektion_vorhanden>1) {
				pDC->SetROP2(R2_NOTXORPEN);
				pDC->Rectangle(&m_rect);
			}
			selektion_vorhanden=1;	
			m_rect.top=y;
			m_rect.left=x;
		}
		ReleaseDC(pDC);
	}
	CView::OnRButtonDown(nFlags, point);
}

void CShiftNView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int k;
	int u1;
	int u2;
	int v1;
	int v2;
	long farbe;
	int x1;
	int y1;
	int x2;
	int y2;
	int u;
	int v;

	int x;
	int y;
	CDC* pDC;
	x=point.x;
	y=point.y;
	if (mhshowgui) {
		pDC=GetDC();

		if (selection_mouse != 0) {
			if (x < selection_X1) {
				x1 = x;
				x2 = selection_X1;
			} else {
				x1 = selection_X1;
				x2 = x;
			}
			if (y < selection_Y1) {
				y1 = y;
				y2 = selection_Y1;
			} else {
				y1 = selection_Y1;
				y2 = y;
			}
			if (selection_rect_prec) {
				pDC->SetROP2(R2_NOTXORPEN);
				if (selection_X1 > selection_X2) {
					selection_rect.left = selection_X2;
					selection_rect.right = selection_X1;
				}
				else {
					selection_rect.left = selection_X1;
					selection_rect.right = selection_X2;
				}
				if (selection_Y1 > selection_Y2) {
					selection_rect.top = selection_Y2;
					selection_rect.bottom = selection_Y1;
				}
				else {
					selection_rect.top = selection_Y1;
					selection_rect.bottom = selection_Y2;
				}
				pDC->Rectangle(&selection_rect);
			}
			selection_rect_prec = false;
			k = 0;
			if (selection_mouse == 1) {
				while (k<struktur_vorhanden) {
					if (struktur[k] == 1) {  // Green line
						get_xy(struktur[k + 13], u1, v1); // x1, y1
						get_xy(struktur[k + 14], u2, v2); // x2, y2
						u1 = afl + (int)(kf*u1);
						u2 = afl + (int)(kf*u2);
						v1 = afu - (int)(kf*v1);
						v2 = afu - (int)(kf*v2);
						if ((u1 <= x2) && (u1 >= x1) && (v1 <= y2) && (v1 >= y1) && (u2 <= x2) && (u2 >= x1) && (v2 <= y2) && (v2 >= y1)) {
							struktur[k] = -3;  // Red line
							farbe = (long)255;
							linie(pDC, u1, v1, u2, v2, farbe);
						}
					}
					k += headersize + struktur[k + 2] + struktur[k + 3];
				}
			} else {
				while (k<struktur_vorhanden) {
					if (struktur[k] == 1) {  // Green line
						get_xy(struktur[k + 13], u1, v1); // x1, y1
						get_xy(struktur[k + 14], u2, v2); // x2, y2
						u1 = afl + (int)(kf*u1);
						u2 = afl + (int)(kf*u2);
						v1 = afu - (int)(kf*v1);
						v2 = afu - (int)(kf*v2);
						u = (u1 + u2) * 0.5;
						v = (v1 + v2) * 0.5;
						if ((u <= x2) && (u >= x1) && (v <= y2) && (v >= y1)) {
							struktur[k] = -3;  // Red line
							farbe = (long)255;
							linie(pDC, u1, v1, u2, v2, farbe);
						}
					}
					k += headersize + struktur[k + 2] + struktur[k + 3];
				}
			}
			globwinkel = 0;
			globparam = 0;
			reset_param = globparam;
			reset_winkel = globwinkel;
			selection_mouse = 0;
		}		else {
			if ((selektion_vorhanden>0) && (ergebnis_vorhanden) && (!beforeafter) && ((afu - afo)>20) && ((afr - afl)>20) && (kf>0.0001) && (x >= 13) && (x <= (13 + afr - afl)) && (y >= 13) && (y <= (13 + afu - afo))) {
				if (!(nFlags&MK_RBUTTON)) {
					pDC->SetROP2(R2_NOTXORPEN);
					if (selektion_vorhanden>1) pDC->Rectangle(&m_rect);
					selektion_vorhanden = 2;
					m_rect.bottom = y;
					m_rect.right = x;
					pDC->Rectangle(&m_rect);
				}
				else {
					if (selektion_vorhanden>1) {
						pDC->SetROP2(R2_NOTXORPEN);
						pDC->Rectangle(&m_rect);
					}
					selektion_vorhanden = 1;
					m_rect.top = y;
					m_rect.left = x;
				}
			}
			m_rect.NormalizeRect();
		}
		ReleaseDC(pDC);
	}
	CView::OnLButtonUp(nFlags, point);
}

void CShiftNView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	int x;
	int y;
	CDC* pDC;
	x=point.x;
	y=point.y;
	if (mhshowgui && showgrid) {
		pDC=GetDC();
		if (customgitter) {
			OnDraw(pDC);
		}
		ReleaseDC(pDC);
	}
	CView::OnMButtonUp(nFlags, point);
}

void CShiftNView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	selektion_vorhanden=0;	
}

void CShiftNView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CShiftNView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	CDC* pDC;
	if ((selektion_vorhanden>0) && (mhshowgui)) {
		pDC=GetDC();
		selektion_vorhanden=0;
		OnDraw(pDC);
		ReleaseDC(pDC);
	}
}

void CShiftNView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
}

void CShiftNView::OnUpdateGitter(CCmdUI* pCmdUI) 
{
	if (!showgrid) pCmdUI->SetCheck(FALSE); 
	else pCmdUI->SetCheck(TRUE);
}

void CShiftNView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	int k;
	int u1;
	int u2;
	int v1;
	int v2;
	long farbe;
	int x1;
	int y1;
	int x2;
	int y2;
	int u;
	int v;


	int x;
	int y;
	double verh;
	CDC* pDC;
	x=point.x;
	y=point.y;
	if (mhshowgui) {
		pDC=GetDC();

		if (selection_mouse != 0) {
			if (x < selection_X1) {
				x1 = x;
				x2 = selection_X1;
			} else {
				x1 = selection_X1;
				x2 = x;
			}
			if (y < selection_Y1) {
				y1 = y;
				y2 = selection_Y1;
			} else {
				y1 = selection_Y1;
				y2 = y;
			}
			if (selection_rect_prec) {
				pDC->SetROP2(R2_NOTXORPEN);
				if (selection_X1 > selection_X2) {
					selection_rect.left = selection_X2;
					selection_rect.right = selection_X1;
				}
				else {
					selection_rect.left = selection_X1;
					selection_rect.right = selection_X2;
				}
				if (selection_Y1 > selection_Y2) {
					selection_rect.top = selection_Y2;
					selection_rect.bottom = selection_Y1;
				}
				else {
					selection_rect.top = selection_Y1;
					selection_rect.bottom = selection_Y2;
				}
				pDC->Rectangle(&selection_rect);
			}
			selection_rect_prec = false;
			k = 0;
			if (selection_mouse == 1) {
				while (k<struktur_vorhanden) {
					if (struktur[k] == -3) {  // Red line
						get_xy(struktur[k + 13], u1, v1); // x1, y1
						get_xy(struktur[k + 14], u2, v2); // x2, y2
						u1 = afl + (int)(kf*u1);
						u2 = afl + (int)(kf*u2);
						v1 = afu - (int)(kf*v1);
						v2 = afu - (int)(kf*v2);
						if ((u1 <= x2) && (u1 >= x1) && (v1 <= y2) && (v1 >= y1) && (u2 <= x2) && (u2 >= x1) && (v2 <= y2) && (v2 >= y1)) {
							struktur[k] = 1;  // Green line
							farbe = (255 << 8);
							linie(pDC, u1, v1, u2, v2, farbe);
						}
					}
					k += headersize + struktur[k + 2] + struktur[k + 3];
				}
			} else {
				while (k<struktur_vorhanden) {
					if (struktur[k] == -3) {  // Red line
						get_xy(struktur[k + 13], u1, v1); // x1, y1
						get_xy(struktur[k + 14], u2, v2); // x2, y2
						u1 = afl + (int)(kf*u1);
						u2 = afl + (int)(kf*u2);
						v1 = afu - (int)(kf*v1);
						v2 = afu - (int)(kf*v2);
						u = (u1 + u2) * 0.5;
						v = (v1 + v2) * 0.5;
						if ((u <= x2) && (u >= x1) && (v <= y2) && (v >= y1)) {
							struktur[k] = 1;  // Green line
							farbe = (255 << 8);
							linie(pDC, u1, v1, u2, v2, farbe);
						}
					}
					k += headersize + struktur[k + 2] + struktur[k + 3];
				}
			}
			globwinkel = 0;
			globparam = 0;
			reset_param = globparam;
			reset_winkel = globwinkel;
			selection_mouse = 0;
		} else {
			if ((selektion_vorhanden>0) && (ergebnis_vorhanden) && (!beforeafter) && ((afu - afo)>20) && ((afr - afl)>20) && (kf>0.0001) && (x >= 13) && (x <= (13 + afr - afl)) && (y >= 13) && (y <= (13 + afu - afo))) {
				if (!(nFlags&MK_LBUTTON)) {
					pDC->SetROP2(R2_NOTXORPEN);
					if (selektion_vorhanden>1) pDC->Rectangle(&m_rect);
					selektion_vorhanden = 2;
					verh = ((double)x_global) / ((double)y_global);
					if (fabs((double)(y - m_rect.top))>3) {
						while ((fabs((double)(x - m_rect.left)) / fabs((double)(y - m_rect.top)))<verh) {
							if (x>m_rect.left) {
								x += 1;
								m_rect.left -= 1;
							}
							else {
								x -= 1;
								m_rect.left += 1;
							}
						}
						while ((fabs((double)(x - m_rect.left)) / fabs((double)(y - m_rect.top)))>verh) {
							if (x>m_rect.left) {
								x -= 1;
								m_rect.left += 1;
							}
							else {
								x += 1;
								m_rect.left -= 1;
							}
						}
						while (m_rect.left<13) m_rect.left += 1;
						while (m_rect.left>(13 + afr - afl)) m_rect.left -= 1;
					}
					m_rect.bottom = y;
					m_rect.right = x;
					while (m_rect.left<13) m_rect.left += 1;
					while (m_rect.left>(13 + afr - afl)) m_rect.left -= 1;
					while (m_rect.right<13) m_rect.right += 1;
					while (m_rect.right>(13 + afr - afl)) m_rect.right -= 1;
					pDC->Rectangle(&m_rect);
				}
				else {
					if (selektion_vorhanden>1) {
						pDC->SetROP2(R2_NOTXORPEN);
						pDC->Rectangle(&m_rect);
					}
					selektion_vorhanden = 1;
					m_rect.top = y;
					m_rect.left = x;
				}
			}
			m_rect.NormalizeRect();
		}
		ReleaseDC(pDC);	
	}
	CView::OnRButtonUp(nFlags, point);
}
