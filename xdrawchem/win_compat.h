#ifndef XDRAWCHEM_WIN_COMPAT_H
#define XDRAWCHEM_WIN_COMPAT_H

#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#ifndef strncasecmp
#define strncasecmp _strnicmp
#endif
#ifndef strdup
#define strdup _strdup
#endif
#endif /* _WIN32 */

#endif /* XDRAWCHEM_WIN_COMPAT_H */
