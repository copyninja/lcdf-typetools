#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#ifdef __GNUG__
# pragma implementation "error.hh"
#endif
#include "error.hh"
#include <assert.h>
#include <string.h>
#include <stdio.h>

const char *program_name;


void
ErrorHandler::warning(const Landmark &landmark, const char *s, ...)
{
  va_list val;
  va_start(val, s);
  verror(WarningKind, landmark, s, val);
  va_end(val);
}

void
ErrorHandler::error(const Landmark &landmark, const char *s, ...)
{
  va_list val;
  va_start(val, s);
  verror(ErrorKind, landmark, s, val);
  va_end(val);
}


void
ErrorHandler::fatal(const Landmark &landmark, const char *s, ...)
{
  va_list val;
  va_start(val, s);
  verror(FatalKind, landmark, s, val);
  va_end(val);
}

void
ErrorHandler::warning(const char *s, ...)
{
  va_list val;
  va_start(val, s);
  verror(WarningKind, Landmark(), s, val);
  va_end(val);
}

void
ErrorHandler::error(const char *s, ...)
{
  va_list val;
  va_start(val, s);
  verror(ErrorKind, Landmark(), s, val);
  va_end(val);
}


void
ErrorHandler::fatal(const char *s, ...)
{
  va_list val;
  va_start(val, s);
  verror(FatalKind, Landmark(), s, val);
  va_end(val);
}


void
ErrorHandler::verror(Kind kind, const Landmark &landmark,
		     const char *s, va_list val)
{
  if (landmark && landmark.has_line())
    fprintf(stderr, "%s:%u: ", landmark.file().cc(), landmark.line());
  else if (landmark)
    fprintf(stderr, "%s: ", landmark.file().cc());
  else if (program_name)
    fprintf(stderr, "%s: ", program_name);
  
  if (kind == WarningKind)
    fputs("warning: ", stderr);
  
  while (1) {
    
    const char *pct = strchr(s, '%');
    if (!pct) {
      if (*s) fputs(s, stderr);
      break;
    }
    if (pct != s) {
      fwrite(s, 1, pct - s, stderr);
      s = pct;
    }
    
    while (1)
      switch (*++s) {
	
       case 's':
	 {
	   const char *x = va_arg(val, const char *);
	   if (!x) x = "(null)";
	   fputs(x, stderr);
	   goto pctdone;
	 }
	 
       case 'c':
	 {
	   int c = va_arg(val, char);
	   if (c == 0)
	     fputs("\\0", stderr);
	   else if (c == '\n')
	     fputs("\\n", stderr);
	   else if (c == '\r')
	     fputs("\\r", stderr);
	   else if (c == '\t')
	     fputs("\\t", stderr);
	   else if (c == '\\')
	     fputs("\\\\", stderr);
	   else if (c >= ' ' && c <= '~')
	     fputc(c, stderr);
	   else
	     fprintf(stderr, "\\%03d", c);
	   goto pctdone;
	 }
	 
       case 'd':
	 {
	   int x = va_arg(val, int);
	   fprintf(stderr, "%d", x);
	   goto pctdone;
	 }
	 
       case 'u':
	 {
	   unsigned x = va_arg(val, unsigned);
	   fprintf(stderr, "%u", x);
	   goto pctdone;
	 }
	 
       case 'g':
	 {
	   double x = va_arg(val, double);
	   fprintf(stderr, "%g", x);
	   goto pctdone;
	 }
	 
       default:
	assert(0 && "Bad % in error");
	goto pctdone;
	
      }
    
   pctdone:
    s++;
  }
  
  fputc('\n', stderr);
  if (kind == FatalKind)
    exit(1);
}


void
PinnedErrorHandler::verror(Kind kind, const Landmark &landmark, const char *s,
			   va_list val)
{
  if (!landmark)
    _errh->verror(kind, _landmark, s, val);
  else
    _errh->verror(kind, landmark, s, val);
}


/*****
 * NullErrorHandler
 **/

class NullErrorHandler: public ErrorHandler {

 public:
  
  NullErrorHandler()						{ }

  void verror(Kind, const Landmark &, const char *, va_list)	{ }

};


ErrorHandler *
ErrorHandler::null_handler()
{
  static ErrorHandler *errh = new NullErrorHandler;
  return errh;
}

