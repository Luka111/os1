//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   segment.h                  
//===================================================

#ifndef __SEGMENT_H
#define __SEGMENT_H

#define MK_FP(seg,ofs) ( (void far*)( ( (unsigned long)(seg) << 16 ) | (unsigned)(ofs) ) )
#define FP_SEG(fp)     ( (unsigned)( (unsigned long)(fp) >> 16 ) )
#define FP_OFF(fp)     ( (unsigned)(fp) )

#endif
