#include <pspsdk.h>
#include <pspkernel.h>

#include <usbasync.h>
#include <usbhostfs.h>
#include <systemctrl.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>

#define NINFO  (sizeof(fmtinfo)/sizeof(info))  /* Size of the fmtinfo table */
#define BUFSIZE 100  /* Size of the output buffer */
#define MAXDIG 20

enum e_type {    /* The type of the format field */
   RADIX,            /* Integer types.  %d, %x, %o, and so forth */
   FLOAT,            /* Floating point.  %f */
   EXP,              /* Exponentional notation. %e and %E */
   GENERIC,          /* Floating or exponential, depending on exponent. %g */
   SIZE,             /* Return number of characters processed so far. %n */
   STRING,           /* Strings. %s */
   PERCENT,          /* Percent symbol. %% */
   CHAR,             /* Characters. %c */
   ERROR,            /* Used to indicate no such conversion type */
/* The rest are extensions, not normally found in printf() */
   CHARLIT,          /* Literal characters.  %' */
   SEEIT,            /* Strings with visible control characters. %S */
   MEM_STRING,       /* A string which should be deleted after use. %z */
   ORDINAL,          /* 1st, 2nd, 3rd and so forth */
};

typedef struct s_info {   /* Information about each format field */
  int  fmttype;              /* The format field code letter */
  int  base;                 /* The base for radix conversion */
  char *charset;             /* The character set for conversion */
  int  flag_signed;          /* Is the quantity signed? */
  char *prefix;              /* Prefix on non-zero values in alt format */
  enum e_type type;          /* Conversion paradigm */
} info;


static info fmtinfo[] = {
  { 'd',  10,  "0123456789",       1,    0, RADIX,      },
  { 's',   0,  0,                  0,    0, STRING,     },
  { 'S',   0,  0,                  0,    0, SEEIT,      },
  { 'z',   0,  0,                  0,    0, MEM_STRING, },
  { 'c',   0,  0,                  0,    0, CHAR,       },
  { 'o',   8,  "01234567",         0,  "0", RADIX,      },
  { 'u',  10,  "0123456789",       0,    0, RADIX,      },
  { 'x',  16,  "0123456789abcdef", 0, "x0", RADIX,      },
  { 'X',  16,  "0123456789ABCDEF", 0, "X0", RADIX,      },
  { 'r',  10,  "0123456789",       0,    0, ORDINAL,    },
  { 'f',   0,  0,                  1,    0, FLOAT,      },
  { 'e',   0,  "e",                1,    0, EXP,        },
  { 'E',   0,  "E",                1,    0, EXP,        },
  { 'g',   0,  "e",                1,    0, GENERIC,    },
  { 'G',   0,  "E",                1,    0, GENERIC,    },
  { 'i',  10,  "0123456789",       1,    0, RADIX,      },
  { 'n',   0,  0,                  0,    0, SIZE,       },
  { 'S',   0,  0,                  0,    0, SEEIT,      },
  { '%',   0,  0,                  0,    0, PERCENT,    },
  { 'b',   2,  "01",               0, "b0", RADIX,      }, /* Binary notation */
  { 'p',  16,  "0123456789ABCDEF", 0, "x0", RADIX,      }, /* Pointers */
  { '\'',  0,  0,                  0,    0, CHARLIT,    }, /* Literal char */
};

struct s_strargument {    /* Describes the string being written to */
  char *next;                   /* Next free slot in the string */
  char *last;                   /* Last available slot in the string */
};

void __sout(txt,amt,arg)
  char *txt;
  int amt;
  void *arg;
{
  register char *head;
  register const char *t;
  register int a;
  register char *tail;
  a = amt;
  t = txt;
  head = ((struct s_strargument*)arg)->next;
  tail = ((struct s_strargument*)arg)->last;
  if( tail ){
    while( a-- >0 && head<tail ) *(head++) = *(t++);
  }else{
    while( a-- >0 ) *(head++) = *(t++);
  }
  *head = 0;
  ((struct s_strargument*)arg)->next = head;
}

static int getdigit(long double *val, int *cnt){
  int digit;
  long double d;
  if( (*cnt)++ >= MAXDIG ) return '0';
  digit = (int)*val;
  d = digit;
  digit += '0';
  *val = (*val - d)*10.0;
  return digit;
}

int vxprintf(func,arg,format,ap)
  void (*func)(char*,int,void*);
  void *arg;
  const char *format;
  va_list ap;
{
  register const char *fmt; /* The format string. */
  register int c;           /* Next character in the format string */
  register char *bufpt;     /* Pointer to the conversion buffer */
  register int  precision;  /* Precision of the current field */
  register int  length;     /* Length of the field */
  register int  idx;        /* A general purpose loop counter */
  int count;                /* Total number of characters output */
  int width;                /* Width of the current field */
  int flag_leftjustify;     /* True if "-" flag is present */
  int flag_plussign;        /* True if "+" flag is present */
  int flag_blanksign;       /* True if " " flag is present */
  int flag_alternateform;   /* True if "#" flag is present */
  int flag_zeropad;         /* True if field width constant starts with zero */
  int flag_long;            /* True if "l" flag is present */
  int flag_center;          /* True if "=" flag is present */
  unsigned long long longvalue;  /* Value for integer types */

  long double realvalue;    /* Value for real types */
  info *infop;              /* Pointer to the appropriate info structure */
  char buf[BUFSIZE];        /* Conversion buffer */
  char prefix;              /* Prefix character.  "+" or "-" or " " or '\0'. */
  int  errorflag = 0;       /* True if an error is encountered */
  enum e_type xtype;        /* Conversion paradigm */
  char *zMem = 0;           /* String to be freed */
  static char spaces[] =
     "                                                    ";
#define SPACESIZE (sizeof(spaces)-1)
#ifndef NOFLOATINGPOINT
  int  exp;                 /* exponent of real numbers */
  long double rounder;      /* Used for rounding floating point values */
  int flag_dp;              /* True if decimal point should be shown */
  int flag_rtz;             /* True if trailing zeros should be removed */
  int flag_exp;             /* True to force display of the exponent */
  int nsd;                  /* Number of significant digits returned */
#endif

  fmt = format;                     /* Put in a register for speed */
  count = length = 0;
  bufpt = 0;
  for(; (c=(*fmt))!=0; ++fmt){
    if( c!='%' ){
      register int amt;
      bufpt = (char *)fmt;
      amt = 1;
      while( (c=(*++fmt))!='%' && c!=0 ) amt++;
      (*func)(bufpt,amt,arg);
      count += amt;
      if( c==0 ) break;
    }
    if( (c=(*++fmt))==0 ){
      errorflag = 1;
      (*func)("%",1,arg);
      count++;
      break;
    }
    /* Find out what flags are present */
    flag_leftjustify = flag_plussign = flag_blanksign =
     flag_alternateform = flag_zeropad = flag_center = 0;
    do{
      switch( c ){
        case '-':   flag_leftjustify = 1;     c = 0;   break;
        case '+':   flag_plussign = 1;        c = 0;   break;
        case ' ':   flag_blanksign = 1;       c = 0;   break;
        case '#':   flag_alternateform = 1;   c = 0;   break;
        case '0':   flag_zeropad = 1;         c = 0;   break;
        case '=':   flag_center = 1;          c = 0;   break;
        default:                                       break;
      }
    }while( c==0 && (c=(*++fmt))!=0 );
    if( flag_center ) flag_leftjustify = 0;
    /* Get the field width */
    width = 0;
    if( c=='*' ){
      width = va_arg(ap,int);
      if( width<0 ){
        flag_leftjustify = 1;
        width = -width;
      }
      c = *++fmt;
    }else{
      while( isdigit(c) ){
        width = width*10 + c - '0';
        c = *++fmt;
      }
    }
    if( width > BUFSIZE-10 ){
      width = BUFSIZE-10;
    }
    /* Get the precision */
    if( c=='.' ){
      precision = 0;
      c = *++fmt;
      if( c=='*' ){
        precision = va_arg(ap,int);
#ifndef COMPATIBILITY
        /* This is sensible, but SUN OS 4.1 doesn't do it. */
        if( precision<0 ) precision = -precision;
#endif
        c = *++fmt;
      }else{
        while( isdigit(c) ){
          precision = precision*10 + c - '0';
          c = *++fmt;
        }
      }
      /* Limit the precision to prevent overflowing buf[] during conversion */
      if( precision>BUFSIZE-40 ) precision = BUFSIZE-40;
    }else{
      precision = -1;
    }
    /* Get the conversion type modifier */
    if( c=='l' ){
      flag_long = 1;
      c = *++fmt;
      if( c == 'l' ){
	flag_long = 2;
	c = *++fmt;
      }
    }else{
      flag_long = 0;
    }
    /* Fetch the info entry for the field */
    infop = 0;
    for(idx=0; idx<NINFO; idx++){
      if( c==fmtinfo[idx].fmttype ){
        infop = &fmtinfo[idx];
        break;
      }
    }
    /* No info entry found.  It must be an error. */
    if( infop==0 ){
      xtype = ERROR;
    }else{
      xtype = infop->type;
    }

    /*
    ** At this point, variables are initialized as follows:
    **
    **   flag_alternateform          TRUE if a '#' is present.
    **   flag_plussign               TRUE if a '+' is present.
    **   flag_leftjustify            TRUE if a '-' is present or if the
    **                               field width was negative.
    **   flag_zeropad                TRUE if the width began with 0.
    **   flag_long                   TRUE if the letter 'l' (ell) prefixed
    **                               the conversion character.
    **   flag_blanksign              TRUE if a ' ' is present.
    **   width                       The specified field width.  This is
    **                               always non-negative.  Zero is the default.
    **   precision                   The specified precision.  The default
    **                               is -1.
    **   xtype                       The class of the conversion.
    **   infop                       Pointer to the appropriate info struct.
    */
    switch( xtype ){
      case ORDINAL:
      case RADIX:
        if(( flag_long>1 )&&( infop->flag_signed )){
	    signed long long t = va_arg(ap,signed long long);
	    longvalue = t;
	}else if(( flag_long>1 )&&( !infop->flag_signed )){
	    unsigned long long t = va_arg(ap,unsigned long long);
	    longvalue = t;
	}else if(( flag_long )&&( infop->flag_signed )){
	    signed long t = va_arg(ap,signed long);
	    longvalue = t;
	}else if(( flag_long )&&( !infop->flag_signed )){
	    unsigned long t = va_arg(ap,unsigned long);
	    longvalue = t;
	}else if(( !flag_long )&&( infop->flag_signed )){
	    signed int t = va_arg(ap,signed int) & ((unsigned long) 0xffffffff);
	    longvalue = t;
	}else{
	    unsigned int t = va_arg(ap,unsigned int) & ((unsigned long) 0xffffffff);
	    longvalue = t;
	}
#ifdef COMPATIBILITY
        /* For the format %#x, the value zero is printed "0" not "0x0".
        ** I think this is stupid. */
        if( longvalue==0 ) flag_alternateform = 0;
#else
        /* More sensible: turn off the prefix for octal (to prevent "00"),
        ** but leave the prefix for hex. */
        if( longvalue==0 && infop->base==8 ) flag_alternateform = 0;
#endif
        if( infop->flag_signed ){
          if( *(long long*)&longvalue<0 ){
	    longvalue = -*(long long*)&longvalue;
            prefix = '-';
          }else if( flag_plussign )  prefix = '+';
          else if( flag_blanksign )  prefix = ' ';
          else                       prefix = 0;
        }else                        prefix = 0;
        if( flag_zeropad && precision<width-(prefix!=0) ){
          precision = width-(prefix!=0);
	}
        bufpt = &buf[BUFSIZE];
        if( xtype==ORDINAL ){
          long a,b;
          a = longvalue%10;
          b = longvalue%100;
          bufpt -= 2;
          if( a==0 || a>3 || (b>10 && b<14) ){
            bufpt[0] = 't';
            bufpt[1] = 'h';
          }else if( a==1 ){
            bufpt[0] = 's';
            bufpt[1] = 't';
          }else if( a==2 ){
            bufpt[0] = 'n';
            bufpt[1] = 'd';
          }else if( a==3 ){
            bufpt[0] = 'r';
            bufpt[1] = 'd';
          }
        }
        {
          register char *cset;      /* Use registers for speed */
          register int base;
          cset = infop->charset;
          base = infop->base;
          do{                                           /* Convert to ascii */
            *(--bufpt) = cset[longvalue%base];
            longvalue = longvalue/base;
          }while( longvalue>0 );
	}
        length = (int)(&buf[BUFSIZE]-bufpt);
	if(infop->fmttype == 'p')
        {
		precision = 8;
		flag_alternateform = 1;
        }

        for(idx=precision-length; idx>0; idx--){
          *(--bufpt) = '0';                             /* Zero pad */
	}
        if( prefix ) *(--bufpt) = prefix;               /* Add sign */
        if( flag_alternateform && infop->prefix ){      /* Add "0" or "0x" */
          char *pre, x;
          pre = infop->prefix;
          if( *bufpt!=pre[0] ){
            for(pre=infop->prefix; (x=(*pre))!=0; pre++) *(--bufpt) = x;
	  }
        }

        length = (int)(&buf[BUFSIZE]-bufpt);
        break;
      case FLOAT:
      case EXP:
      case GENERIC:
        realvalue = va_arg(ap,double);
#ifndef NOFLOATINGPOINT
        if( precision<0 ) precision = 6;         /* Set default precision */
        if( precision>BUFSIZE-10 ) precision = BUFSIZE-10;
        if( realvalue<0.0 ){
          realvalue = -realvalue;
          prefix = '-';
	}else{
          if( flag_plussign )          prefix = '+';
          else if( flag_blanksign )    prefix = ' ';
          else                         prefix = 0;
	}
        if( infop->type==GENERIC && precision>0 ) precision--;
        rounder = 0.0;
#ifdef COMPATIBILITY
        /* Rounding works like BSD when the constant 0.4999 is used.  Wierd! */
        for(idx=precision, rounder=0.4999; idx>0; idx--, rounder*=0.1);
#else
        /* It makes more sense to use 0.5 */
        if( precision>MAXDIG-1 ) idx = MAXDIG-1;
        else                     idx = precision;
        for(rounder=0.5; idx>0; idx--, rounder*=0.1);
#endif
        if( infop->type==FLOAT ) realvalue += rounder;
        /* Normalize realvalue to within 10.0 > realvalue >= 1.0 */
        exp = 0;
        if( realvalue>0.0 ){
          int k = 0;
          while( realvalue>=1e8 && k++<100 ){ realvalue *= 1e-8; exp+=8; }
          while( realvalue>=10.0 && k++<100 ){ realvalue *= 0.1; exp++; }
          while( realvalue<1e-8 && k++<100 ){ realvalue *= 1e8; exp-=8; }
          while( realvalue<1.0 && k++<100 ){ realvalue *= 10.0; exp--; }
          if( k>=100 ){
            bufpt = "NaN";
            length = 3;
            break;
          }
	}
        bufpt = buf;
        /*
        ** If the field type is GENERIC, then convert to either EXP
        ** or FLOAT, as appropriate.
        */
        flag_exp = xtype==EXP;
        if( xtype!=FLOAT ){
          realvalue += rounder;
          if( realvalue>=10.0 ){ realvalue *= 0.1; exp++; }
        }
        if( xtype==GENERIC ){
          flag_rtz = !flag_alternateform;
          if( exp<-4 || exp>precision ){
            xtype = EXP;
          }else{
            precision = precision - exp;
            xtype = FLOAT;
          }
	}else{
          flag_rtz = 0;
	}
        /*
        ** The "exp+precision" test causes output to be of type EXP if
        ** the precision is too large to fit in buf[].
        */
        nsd = 0;
        if( xtype==FLOAT && exp+precision<BUFSIZE-30 ){
          flag_dp = (precision>0 || flag_alternateform);
          if( prefix ) *(bufpt++) = prefix;         /* Sign */
          if( exp<0 )  *(bufpt++) = '0';            /* Digits before "." */
          else for(; exp>=0; exp--) *(bufpt++) = getdigit(&realvalue,&nsd);
          if( flag_dp ) *(bufpt++) = '.';           /* The decimal point */
          for(exp++; exp<0 && precision>0; precision--, exp++){
            *(bufpt++) = '0';
          }
          while( (precision--)>0 ) *(bufpt++) = getdigit(&realvalue,&nsd);
          *(bufpt--) = 0;                           /* Null terminate */
          if( flag_rtz && flag_dp ){     /* Remove trailing zeros and "." */
            while( bufpt>=buf && *bufpt=='0' ) *(bufpt--) = 0;
            if( bufpt>=buf && *bufpt=='.' ) *(bufpt--) = 0;
          }
          bufpt++;                            /* point to next free slot */
	}else{    /* EXP or GENERIC */
          flag_dp = (precision>0 || flag_alternateform);
          if( prefix ) *(bufpt++) = prefix;   /* Sign */
          *(bufpt++) = getdigit(&realvalue,&nsd);  /* First digit */
          if( flag_dp ) *(bufpt++) = '.';     /* Decimal point */
          while( (precision--)>0 ) *(bufpt++) = getdigit(&realvalue,&nsd);
          bufpt--;                            /* point to last digit */
          if( flag_rtz && flag_dp ){          /* Remove tail zeros */
            while( bufpt>=buf && *bufpt=='0' ) *(bufpt--) = 0;
            if( bufpt>=buf && *bufpt=='.' ) *(bufpt--) = 0;
          }
          bufpt++;                            /* point to next free slot */
          if( exp || flag_exp ){
            *(bufpt++) = infop->charset[0];
            if( exp<0 ){ *(bufpt++) = '-'; exp = -exp; } /* sign of exp */
            else       { *(bufpt++) = '+'; }
            if( exp>=100 ){
              *(bufpt++) = (exp/100)+'0';                /* 100's digit */
              exp %= 100;
  	    }
            *(bufpt++) = exp/10+'0';                     /* 10's digit */
            *(bufpt++) = exp%10+'0';                     /* 1's digit */
          }
	}
        /* The converted number is in buf[] and zero terminated. Output it.
        ** Note that the number is in the usual order, not reversed as with
        ** integer conversions. */
        length = (int)(bufpt-buf);
        bufpt = buf;

        /* Special case:  Add leading zeros if the flag_zeropad flag is
        ** set and we are not left justified */
        if( flag_zeropad && !flag_leftjustify && length < width){
          int i;
          int nPad = width - length;
          for(i=width; i>=nPad; i--){
            bufpt[i] = bufpt[i-nPad];
          }
          i = prefix!=0;
          while( nPad-- ) bufpt[i++] = '0';
          length = width;
        }
#endif
        break;
      case SIZE:
        *(va_arg(ap,int*)) = count;
        length = width = 0;
        break;
      case PERCENT:
        buf[0] = '%';
        bufpt = buf;
        length = 1;
        break;
      case CHARLIT:
      case CHAR:
        c = buf[0] = (xtype==CHAR ? va_arg(ap,int) : *++fmt);
        if( precision>=0 ){
          for(idx=1; idx<precision; idx++) buf[idx] = c;
          length = precision;
	}else{
          length =1;
	}
        bufpt = buf;
        break;
      case STRING:
      case MEM_STRING:
        zMem = bufpt = va_arg(ap,char*);
        if( bufpt==0 ) bufpt = "(null)";
        length = strlen(bufpt);
        if( precision>=0 && precision<length ) length = precision;
        break;
      case SEEIT:
        {
          int i;
          int c;
          char *arg = va_arg(ap,char*);
          for(i=0; i<BUFSIZE-1 && (c = *arg++)!=0; i++){
            if( c<0x20 || c>=0x7f ){
              buf[i++] = '^';
              buf[i] = (c&0x1f)+0x40;
            }else{
              buf[i] = c;
            }
          }
          bufpt = buf;
          length = i;
          if( precision>=0 && precision<length ) length = precision;
        }
        break;
      case ERROR:
        buf[0] = '%';
        buf[1] = c;
        errorflag = 0;
        idx = 1+(c!=0);
        (*func)("%",idx,arg);
        count += idx;
        if( c==0 ) fmt--;
        break;
    }/* End switch over the format type */
    /*
    ** The text of the conversion is pointed to by "bufpt" and is
    ** "length" characters long.  The field width is "width".  Do
    ** the output.
    */
    if( !flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        if( flag_center ){
          nspace = nspace/2;
          width -= nspace;
          flag_leftjustify = 1;
	}
        count += nspace;
        while( nspace>=SPACESIZE ){
          (*func)(spaces,SPACESIZE,arg);
          nspace -= SPACESIZE;
        }
        if( nspace>0 ) (*func)(spaces,nspace,arg);
      }
    }
    if( length>0 ){
      (*func)(bufpt,length,arg);
      count += length;
    }
    if( xtype==MEM_STRING && zMem ){
      //free(zMem);
    }
    if( flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        count += nspace;
        while( nspace>=SPACESIZE ){
          (*func)(spaces,SPACESIZE,arg);
          nspace -= SPACESIZE;
        }
        if( nspace>0 ) (*func)(spaces,nspace,arg);
      }
    }
  }/* End for loop over the format string */
  return errorflag ? -1 : count;
} /* End of function */

int vsprintf(char *buf, const char *fmt, va_list ap)
{
	struct s_strargument arg;
	arg.next = buf;
	arg.last = NULL;
	*buf = 0;
	return vxprintf(__sout, &arg, fmt, ap);
}

int usbKprintf(char *fmt, ...)
{
	int k1 = psplinkSetK1(0);
	
	va_list list;
	char msg[128];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	int res = usbAsyncWrite(ASYNC_SHELL, msg, strlen(msg));	

	psplinkSetK1(k1);
	return res;
}

#define J_OPCODE	0x08000000
#define NOP	0x00000000

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(NOP, a+4);

struct AsyncEndpoint g_endp;

void installUsbKprintf()
{
	u32 kprintf = sctrlHENFindFunction("sceSystemMemoryManager", "KDebugForKernel", 0x84F370BC);

	if (kprintf)
	{
		usbAsyncRegister(ASYNC_SHELL, &g_endp);
		usbWaitForConnect();
		REDIRECT_FUNCTION(kprintf, usbKprintf);
	}

	u32 printf = sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x13A5ABEF);
	
	if (printf)
	{	
		REDIRECT_FUNCTION(kprintf, usbKprintf);
	}

	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

}
